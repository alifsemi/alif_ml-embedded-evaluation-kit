/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/* Power measurement backed by a TI INA228 on the I3C controller (legacy I2C mode). */

#include <inttypes.h>
#include "alif.h"
#include "RTE_Components.h"
#include "power_measurement.h"
#include "drv_ina228.h"
#include "timer_alif.h"
#include "hal_log.h"

#ifndef INA228_SHUNT_MOHM
#define INA228_SHUNT_MOHM INA228_SHUNT_AUTO
#endif

#define INA228_I2C_ADDR         0x40
#define INA228_MANUFACTURER     0x5449  /* "TI" */
#define INA228_DIE_ID           0x228   /* DEVICE_ID[15:4] */

static uint8_t dev_list[] = {INA228_I2C_ADDR};

static bool sensor_available = false;

/* Cycle-counter timestamp of the open measurement window. */
static uint32_t window_start_cycles;

/* Checks that whatever answers at the address is an INA228. */
static int32_t probe_ina228(void)
{
    uint16_t manufacturer = 0;
    uint16_t device = 0;

    if ((INA228_Read16(dev_list[0], MANUFACTURER_ID, &manufacturer) != 0) ||
        (INA228_Read16(dev_list[0], DEVICE_ID, &device) != 0)) {
        return -1;
    }
    if ((manufacturer != INA228_MANUFACTURER) || ((device >> 4) != INA228_DIE_ID)) {
        warn("Device at 0x%02X is not an INA228 (MANUFACTURER_ID 0x%04X, DEVICE_ID 0x%04X)\n",
             dev_list[0], manufacturer, device);
        return -1;
    }
    return 0;
}

static int32_t configure_ina228(void)
{
    uint32_t vbus_uV;
    int32_t current_nA;
    uint16_t reg_data;

    /* Reset the device to its power-on defaults. */
    if (INA228_Write(dev_list[0], CONFIG, 1U << 15) != 0) {
        return -1;
    }

    /* ADC_CONFIG: MODE[15:12] = 0xB (continuous shunt + bus voltage)
     * VBUSCT[11:9] = VSHCT[8:6] = 0 (50us), AVG[2:0] = 0 (1 sample)
     */
    if (INA228_Write(dev_list[0], ADC_CONFIG, 0xB000) != 0) {
        return -1;
    }

    /* Boards are fitted with either a 15 mOhm or a 2 Ohm shunt and nothing on the board says
     * which, so infer it from the shunt voltage while the system is running (still on the
     * reset-default 163.84 mV range here). INA228_SHUNT_MOHM (CMake) can force a value; the
     * detection result is then only used as a cross-check. */
    {
        uint16_t shunt_mohm = INA228_SHUNT_MOHM;
        uint16_t detected_mohm = 0;
        int32_t vshunt_nV = 0;

        if (INA228_DetectShunt(dev_list[0], &detected_mohm, &vshunt_nV) != 0) {
            return -1;
        }
        if (shunt_mohm == INA228_SHUNT_AUTO) {
            shunt_mohm = detected_mohm;
            info("INA228 shunt auto-detected as %u mOhm (VSHUNT %" PRId32 " uV)\n",
                 shunt_mohm, vshunt_nV / 1000);
        } else if (shunt_mohm != detected_mohm) {
            warn("INA228 shunt configured as %u mOhm but VSHUNT %" PRId32
                 " uV looks like %u mOhm\n", shunt_mohm, vshunt_nV / 1000, detected_mohm);
        }

        /* Sets the ADC range, current LSB and SHUNT_CAL to match the shunt. */
        if (INA228_ConfigureShunt(dev_list[0], shunt_mohm) != 0) {
            return -1;
        }
    }
    if (INA228_Read16(dev_list[0], SHUNT_CAL, &reg_data) != 0) {
        return -1;
    }
    debug("INA228 SHUNT_CAL: 0x%04X\n", reg_data);

    if ((INA228_ReadVBUS(dev_list[0], &vbus_uV) != 0) ||
        (INA228_ReadCURRENT(dev_list[0], &current_nA) != 0)) {
        return -1;
    }
    info("INA228 VBUS: %" PRIu32 " mV, CURRENT: %" PRId32 " uA\n",
         vbus_uV / 1000, current_nA / 1000);

    /* Leave the ADC in shutdown; power_measurement_begin/end start and stop conversions so
     * the accumulators cover exactly the measured window. */
    return INA228_StopConversions(dev_list[0]);
}

int32_t power_measurement_init(void)
{
    const uint8_t dev_count = sizeof(dev_list) / sizeof(dev_list[0]);

    sensor_available = false;
    if (INA228_Init(dev_list, dev_count) != 0) {
        return -1;
    }
    if ((probe_ina228() != 0) || (configure_ina228() != 0)) {
        INA228_Deinit(dev_list, dev_count);
        return -1;
    }
    sensor_available = true;
    return 0;
}

bool power_measurement_is_available(void)
{
    return sensor_available;
}

/* The ADC is in shutdown between windows (init and power_measurement_end leave it so), so
 * clearing the accumulators cannot race with a conversion. Starting conversions defines the
 * leading edge; the timestamp is taken as soon as that write completes. */
int32_t power_measurement_begin(void)
{
    if (!sensor_available) {
        return -1;
    }
    if ((INA228_ClearAccumulators(dev_list[0]) != 0) ||
        (INA228_StartConversions(dev_list[0]) != 0)) {
        return -1;
    }
    window_start_cycles = Get_SysTick_Cycle_Count32();
    return 0;
}

/* Stopping conversions freezes the accumulators, so the trailing edge is the end of that
 * write and the reads that follow do not stretch the window. Average current comes from the
 * CHARGE accumulator. Power is derived as VBUS * I_avg rather than read from ENERGY, whose
 * LSB (51.2 x the current LSB, in joules) is too coarse for windows of a few milliseconds;
 * VBUS is a regulated rail and the register holds the last bus sample taken in the window.
 *
 * The window is limited to ~10 s by the 32-bit cycle counter. */
int32_t power_measurement_end(power_measurement_result_t *result)
{
    int64_t  charge_nC = 0;
    uint32_t vbus_uV   = 0;
    uint32_t cycles;
    int64_t  us;

    if (!sensor_available) {
        return -1;
    }
    if (INA228_StopConversions(dev_list[0]) != 0) {
        return -1;
    }
    cycles = Get_SysTick_Cycle_Count32() - window_start_cycles;

    if ((INA228_ReadCHARGE(dev_list[0], &charge_nC) != 0) ||
        (INA228_ReadVBUS(dev_list[0], &vbus_uV) != 0)) {
        return -1;
    }

    /* The cycle counter is derived from SysTick, which is configured from this clock. */
    us = (int64_t)(((uint64_t)cycles * 1000000ULL) / GetSystemCoreClock());
    if (us <= 0) {
        return -1;
    }

    /* nC / us == mA */
    result->avg_mA = (int32_t)(charge_nC / us);
    /* uV * nC / us == nW; keep full precision before the final /1e6 to mW. */
    result->avg_mW = (int32_t)((((int64_t)vbus_uV * charge_nC) / us) / 1000000LL);
    result->elapsed_us = (uint32_t)us;

    info("Power measurement: avg %" PRId32 " mA, %" PRId32 " mW over %" PRIu32 " us\n",
         result->avg_mA, result->avg_mW, result->elapsed_us);
    return 0;
}
