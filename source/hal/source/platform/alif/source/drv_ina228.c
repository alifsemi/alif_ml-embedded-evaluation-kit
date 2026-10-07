/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifdef USE_INA228
#include <string.h>
#include "alif.h"
#include "RTE_Components.h"
#include "drv_ina228.h"
#include "Driver_I3C.h"
#include "timer_alif.h"
#include "hal_log.h"

#define INA228_TX_COUNT     3
#define INA228_RX_COUNT     1
#define INA228_MAX_RX_COUNT 8
#define INA228_I2C_TIMEOUT  20

/* VSHUNT above this at boot means the 2 Ohm shunt is fitted (see INA228_DetectShunt). */
#define INA228_DETECT_THRESHOLD_NV 10000000   /* 10 mV */

static volatile uint32_t event_flags_i3c;

/* Current LSB in nA, set by INA228_ConfigureShunt() to match the SHUNT_CAL it programs. All
 * CURRENT/POWER/ENERGY/CHARGE conversions scale by this. Signed so that signed register
 * values are not promoted to unsigned. Assumes all attached devices share one shunt value. */
static int32_t current_lsb_nA = 1000;

/* Mirrors CONFIG.ADCRANGE so VSHUNT can be scaled without an extra register read.
 * Assumes all attached devices use the same range. */
static uint8_t adc_range = INA228_ADCRANGE_163_84MV;

/* Shadow copies of the last CONFIG (minus the self-clearing RST/RSTACC bits) and ADC_CONFIG
 * values written, so the per-inference start/stop/clear sequence is one I2C write each
 * instead of a read-modify-write. Assumes all attached devices share the same settings. */
#define INA228_CONFIG_RST    (1U << 15)
#define INA228_CONFIG_RSTACC (1U << 14)
#define INA228_ADC_MODE_MASK (0xF000U)
static uint16_t config_shadow = 0;
static uint16_t adc_config_shadow = 0xFB68;   /* device default */

extern ARM_DRIVER_I3C Driver_I3C;
static ARM_DRIVER_I3C *I3Cdrv = &Driver_I3C;

static int32_t _INA228_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t num_bytes) {
    uint32_t ms_wait;
    uint8_t  tx_data[INA228_TX_COUNT] = {0};
    int32_t  ret;

    tx_data[0] = reg_addr & 0x7F;
    event_flags_i3c = 0;

    if (num_bytes) {
        tx_data[1] = reg_data[1];
        tx_data[2] = reg_data[0];
        ret = I3Cdrv->MasterTransmit(dev_addr, tx_data, INA228_TX_COUNT);
        if(ret != ARM_DRIVER_OK)
        {
            printf_err("\r\n Error: i2c Master Transmit failed. \r\n");
            return -1;
        }
    } else {
        ret = I3Cdrv->MasterTransmit(dev_addr, tx_data, INA228_RX_COUNT);
        if(ret != ARM_DRIVER_OK)
        {
            printf_err("\r\n Error: i2c Master Transmit failed. \r\n");
            return -1;
        }
    }

    ms_wait = Get_SysTick_Count() + INA228_I2C_TIMEOUT;
    while (1) {
        if (event_flags_i3c) break;
        if ((int32_t)(Get_SysTick_Count() - ms_wait) >= 0) {
            printf_err("Error: I2C Master Transmit timeout after %dms\n", INA228_I2C_TIMEOUT);
            return -1;
        }
    }

    if (event_flags_i3c & ARM_I3C_EVENT_TRANSFER_ERROR) {
        /* TX Error: Got NACK from slave */
        printf_err(">> I2C Master Transmit Error: Got NACK from slave addr: 0x%X\n", dev_addr);
        return -1;
    }

    return 0;
}

static int32_t _INA228_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t num_bytes) {
    uint32_t ms_wait;
    uint8_t  rx_data[INA228_MAX_RX_COUNT] = {0};
    int32_t  ret;

    if (_INA228_Write(dev_addr, reg_addr, 0, 0)) return -1;

    event_flags_i3c = 0;
    ret = I3Cdrv->MasterReceive(dev_addr, &rx_data[num_bytes-2], num_bytes);
    if(ret != ARM_DRIVER_OK)
    {
        printf_err("\r\n Error: i2c Master Receive failed. \r\n");
        return -1;
    }

    ms_wait = Get_SysTick_Count() + INA228_I2C_TIMEOUT;
    while (1) {
        if (event_flags_i3c) break;
        if ((int32_t)(Get_SysTick_Count() - ms_wait) >= 0) {
            printf_err("Error: I2C Master Receive timeout after %dms\n", INA228_I2C_TIMEOUT);
            return -1;
        }
    }

    switch(num_bytes) {
    case INA228_16BREG:
        reg_data[0] = rx_data[1];
        reg_data[1] = rx_data[0];
        break;
    case INA228_24BREG:
        reg_data[0] = rx_data[3];
        reg_data[1] = rx_data[2];
        reg_data[2] = rx_data[1];
        reg_data[3] = rx_data[0];
        break;
    case INA228_40BREG:
        reg_data[0] = rx_data[7];
        reg_data[1] = rx_data[6];
        reg_data[2] = rx_data[5];
        reg_data[3] = rx_data[4];
        reg_data[4] = rx_data[3];
        reg_data[5] = rx_data[2];
        reg_data[6] = rx_data[1];
        reg_data[7] = rx_data[0];
        break;
    default:
        return -1;
    }

    if (event_flags_i3c & ARM_I3C_EVENT_TRANSFER_ERROR) {
        printf_err(">> I2C Master Receive Error: Got NACK from slave addr: 0x%X\n", dev_addr);
        return -1;
    }

    return 0;
}

static void INA228_CB(uint32_t event)
{
    event_flags_i3c |= event;
}

int32_t INA228_Init(uint8_t *dev_addr, uint8_t dev_count)
{
    int32_t ret;
    ret = I3Cdrv->Initialize(INA228_CB);
    if(ret != ARM_DRIVER_OK)
    {
        printf_err("\r\n Error: I3C Initialize failed.\r\n");
        return -1;
    }
    ret = I3Cdrv->PowerControl(ARM_POWER_FULL);
    if(ret != ARM_DRIVER_OK)
    {
        printf_err("\r\n Error: I3C Power Up failed.\r\n");
        return -1;
    }
    ret = I3Cdrv->Control(I3C_MASTER_INIT, 0);
    if(ret != ARM_DRIVER_OK)
    {
        printf_err("\r\n Error: I3C Control: Master Init failed.\r\n");
        return -1;
    }
    /* 400 kHz: the INA228 supports it and it keeps the per-inference start/stop writes
     * short relative to a ~3 ms inference window. */
    ret = I3Cdrv->Control(I3C_MASTER_SET_BUS_MODE,
                           I3C_BUS_MODE_MIXED_FAST_I2C_FM_SPEED_400_KBPS);
    if(ret != ARM_DRIVER_OK)
    {
        printf_err("\r\n Error: I3C Control: Set Bus Mode failed.\r\n");
        return -1;
    }

    while(dev_count--) {
        ret = I3Cdrv->AttachSlvDev(ARM_I3C_DEVICE_TYPE_I2C, *(dev_addr++));
        if(ret != ARM_DRIVER_OK)
        {
            printf_err("\r\n Error: I3C Attach I2C device failed.\r\n");
            return -1;
        }
    }
    return 0;
}

int32_t INA228_Deinit(uint8_t *dev_addr, uint8_t dev_count)
{
    int32_t  ret;
    while(dev_count--) {
        ret = I3Cdrv->Detachdev(*(dev_addr++));
        if(ret != ARM_DRIVER_OK)
        {
            printf_err("\r\n Error: I3C Detach I2C device failed.\r\n");
            return -1;
        }
    }

    ret = I3Cdrv->PowerControl(ARM_POWER_OFF);
    if(ret != ARM_DRIVER_OK)
    {
        printf_err("\r\n Error: I3C Power OFF failed.\r\n");
        return -1;
    }

    ret = I3Cdrv->Uninitialize();
    if(ret != ARM_DRIVER_OK)
    {
        printf_err("\r\n Error: I3C Uninitialize failed.\r\n");
        return -1;
    }

    return 0;
}

int32_t INA228_Write(uint8_t dev_addr, uint8_t reg_addr, uint16_t reg_data) {
    int32_t ret = _INA228_Write(dev_addr, reg_addr, (uint8_t *) &reg_data, INA228_16BREG);
    if (ret == 0) {
        if (reg_addr == CONFIG) {
            /* A device reset returns every CONFIG field to 0. */
            config_shadow = (reg_data & INA228_CONFIG_RST) ? 0
                          : (reg_data & (uint16_t)~(INA228_CONFIG_RST | INA228_CONFIG_RSTACC));
            if (reg_data & INA228_CONFIG_RST) {
                adc_config_shadow = 0xFB68;
                adc_range = INA228_ADCRANGE_163_84MV;
            }
        } else if (reg_addr == ADC_CONFIG) {
            adc_config_shadow = reg_data;
        }
    }
    return ret;
}

int32_t INA228_Read16(uint8_t dev_addr, uint8_t reg_addr, uint16_t *reg_data) {
    return _INA228_Read(dev_addr, reg_addr, (uint8_t *) reg_data, INA228_16BREG);
}

int32_t INA228_Read32(uint8_t dev_addr, uint8_t reg_addr, uint32_t *reg_data) {
    return _INA228_Read(dev_addr, reg_addr, (uint8_t *) reg_data, INA228_24BREG);
}

int32_t INA228_Read64(uint8_t dev_addr, uint8_t reg_addr, uint64_t *reg_data) {
    return _INA228_Read(dev_addr, reg_addr, (uint8_t *) reg_data, INA228_40BREG);
}

/* Sign-extends the upper 20 bits of a 24-bit register read (VSHUNT, CURRENT). */
static int32_t sign_extend_20(uint32_t raw24) {
    return ((int32_t)(raw24 << 8)) >> 12;
}

/* Selects the shunt ADC full-scale range (CONFIG.ADCRANGE, bit 4).
 * NOTE: SHUNT_CAL must be written *after* this, and multiplied by 4 for the 40.96 mV range. */
int32_t INA228_SetADCRange(uint8_t dev_addr, uint8_t range) {
    uint16_t reg_data = config_shadow;
    if (range == INA228_ADCRANGE_40_96MV) {
        reg_data |= 1U << 4;
    } else {
        reg_data &= (uint16_t)~(1U << 4);
    }
    if (INA228_Write(dev_addr, CONFIG, reg_data) != 0) {
        return -1;
    }
    adc_range = range;
    return 0;
}

/* Infers which shunt is fitted from the shunt voltage with the system running. The two
 * supported values (15 mOhm and 2 Ohm) are 133x apart, so at any plausible running current
 * (roughly 5 mA .. 650 mA) VSHUNT is either well below or well above 10 mV. This is a
 * plausibility test, not a resistance measurement. Requires the default +/-163.84 mV range
 * and conversions running with at least one conversion completed. */
int32_t INA228_DetectShunt(uint8_t dev_addr, uint16_t *shunt_mohm, int32_t *vshunt_nV) {
    int32_t nV = 0;
    if (adc_range != INA228_ADCRANGE_163_84MV) {
        printf_err("INA228: shunt detection needs the 163.84 mV range\n");
        return -1;
    }
    if (INA228_ReadVSHUNT(dev_addr, &nV) != 0) {
        return -1;
    }
    if (vshunt_nV) {
        *vshunt_nV = nV;
    }
    if (nV < 0) {
        nV = -nV;
    }
    *shunt_mohm = (nV >= INA228_DETECT_THRESHOLD_NV) ? 2000 : 15;
    return 0;
}

/* Applies the ADC range, current LSB and SHUNT_CAL for a supported shunt value:
 *   15 mOhm: +/-40.96 mV range, 500 nA LSB -> +/-262 mA (CURRENT register limited)
 *   2 Ohm:   +/-163.84 mV range, 250 nA LSB -> +/-82 mA (ADC limited)
 * SHUNT_CAL = 13107.2e6 * CURRENT_LSB[A] * R_SHUNT[Ohm], x4 for the 40.96 mV range; it is a
 * 15-bit register, which is why the 2 Ohm shunt cannot share the 15 mOhm settings. */
int32_t INA228_ConfigureShunt(uint8_t dev_addr, uint16_t shunt_mohm) {
    uint8_t range;
    int32_t lsb_nA;
    uint64_t cal;

    switch (shunt_mohm) {
    case 15:
        range  = INA228_ADCRANGE_40_96MV;
        lsb_nA = 500;
        break;
    case 2000:
        range  = INA228_ADCRANGE_163_84MV;
        lsb_nA = 250;
        break;
    default:
        printf_err("INA228: unsupported shunt value %u mOhm\n", shunt_mohm);
        return -1;
    }

    cal = (131072ULL * (uint32_t)lsb_nA * shunt_mohm *
           ((range == INA228_ADCRANGE_40_96MV) ? 4U : 1U) + 5000000ULL) / 10000000ULL;
    if ((cal == 0) || (cal > 0x7FFF)) {
        printf_err("INA228: SHUNT_CAL out of range for %u mOhm\n", shunt_mohm);
        return -1;
    }

    /* The range must be set before SHUNT_CAL is written. */
    if (INA228_SetADCRange(dev_addr, range) != 0) {
        return -1;
    }
    if (INA228_Write(dev_addr, SHUNT_CAL, (uint16_t)cal) != 0) {
        return -1;
    }
    current_lsb_nA = lsb_nA;
    return 0;
}

/* Puts the ADC into shutdown (MODE = 0). Conversions stop, so ENERGY/CHARGE stop accumulating
 * and can be read without the read latency extending the measurement window. */
int32_t INA228_StopConversions(uint8_t dev_addr) {
    uint16_t stopped = adc_config_shadow & (uint16_t)~INA228_ADC_MODE_MASK;
    int32_t ret = _INA228_Write(dev_addr, ADC_CONFIG, (uint8_t *) &stopped, INA228_16BREG);
    /* Deliberately bypasses INA228_Write so adc_config_shadow keeps the operational mode. */
    return ret;
}

/* Restarts conversions with the last operational ADC_CONFIG (mode, timing, averaging). */
int32_t INA228_StartConversions(uint8_t dev_addr) {
    return INA228_Write(dev_addr, ADC_CONFIG, adc_config_shadow);
}

/* returns measured VSHUNT as signed nano-volts */
int32_t INA228_ReadVSHUNT(uint8_t dev_addr, int32_t *reg_data) {
    uint32_t raw = 0;
    int32_t ret = INA228_Read32(dev_addr, VSHUNT, &raw);
    /* VSHUNT LSB is 312.5 nV (625/2) at +/-163.84 mV, 78.125 nV (625/8) at +/-40.96 mV. */
    if (adc_range == INA228_ADCRANGE_40_96MV) {
        *reg_data = (sign_extend_20(raw) * 625) / 8;
    } else {
        *reg_data = (sign_extend_20(raw) * 625) / 2;
    }
    return ret;
}

/* returns measured VBUS as micro-volts */
int32_t INA228_ReadVBUS(uint8_t dev_addr, uint32_t *reg_data) {
    int32_t ret = INA228_Read32(dev_addr, VBUS, reg_data);
    *reg_data >>= 4;
    *reg_data = (uint32_t)(((uint64_t)*reg_data * 3125) / 16);
    return ret;
}

/* returns measured DIETEMP as signed millidegrees-C */
int32_t INA228_ReadDIETEMP(uint8_t dev_addr, int32_t *reg_data) {
    uint16_t raw = 0;
    int32_t ret = INA228_Read16(dev_addr, DIETEMP, &raw);
    *reg_data = ((int32_t)(int16_t)raw * 125) / 16;
    return ret;
}

/* returns measured CURRENT as signed nano-amps */
int32_t INA228_ReadCURRENT(uint8_t dev_addr, int32_t *reg_data) {
    uint32_t raw = 0;
    int32_t ret = INA228_Read32(dev_addr, CURRENT, &raw);
    *reg_data = sign_extend_20(raw) * current_lsb_nA;
    return ret;
}

/* returns measured POWER as nano-Watts */
int32_t INA228_ReadPOWER(uint8_t dev_addr, uint64_t *reg_data) {
    uint32_t raw = 0;
    int32_t ret = INA228_Read32(dev_addr, POWER, &raw);
    *reg_data = ((uint64_t)raw * current_lsb_nA * 16) / 5;
    return ret;
}

/* Resets the ENERGY and CHARGE accumulators to zero (RSTACC is self-clearing). Uses the
 * CONFIG shadow so the other fields (ADCRANGE etc.) are preserved with a single write. */
int32_t INA228_ClearAccumulators(uint8_t dev_addr) {
    return INA228_Write(dev_addr, CONFIG, config_shadow | INA228_CONFIG_RSTACC);
}

/* returns measured ENERGY as nano-Joules */
int32_t INA228_ReadENERGY(uint8_t dev_addr, uint64_t *reg_data) {
    uint64_t raw = 0;
    int32_t ret = INA228_Read64(dev_addr, ENERGY, &raw);
    *reg_data = (raw * current_lsb_nA * 256) / 5;
    return ret;
}

/* returns measured CHARGE as signed nano-Coulombs.
 * Per the datasheet, Charge [C] = CURRENT_LSB * CHARGE (no x16 factor, unlike ENERGY). */
int32_t INA228_ReadCHARGE(uint8_t dev_addr, int64_t *reg_data) {
    uint64_t raw = 0;
    int32_t ret = INA228_Read64(dev_addr, CHARGE, &raw);
    *reg_data = (((int64_t)(raw << 24)) >> 24) * current_lsb_nA;
    return ret;
}

/* Cycle-counter timestamp of the open measurement window. One window at a time. */
static uint32_t window_start_cycles;

/* Opens a measurement window. The ADC is expected to be in shutdown (as init and
 * INA228_WindowEnd leave it), so clearing the accumulators cannot race with a conversion.
 * Starting conversions defines the leading edge; the timestamp is taken as soon as that
 * write completes. */
int32_t INA228_WindowBegin(uint8_t dev_addr) {
    if ((INA228_ClearAccumulators(dev_addr) != 0) || (INA228_StartConversions(dev_addr) != 0)) {
        return -1;
    }
    window_start_cycles = Get_SysTick_Cycle_Count32();
    return 0;
}

/* Closes the window opened by INA228_WindowBegin and returns the averages over it.
 *
 * Stopping conversions freezes the accumulators, so the trailing edge is the end of that
 * write and the reads that follow do not stretch the window. Average current comes from the
 * CHARGE accumulator. Power is derived as VBUS * I_avg rather than read from ENERGY, whose
 * LSB (51.2 x the current LSB, in joules) is too coarse for windows of a few milliseconds;
 * VBUS is a regulated rail and the register holds the last bus sample taken in the window.
 *
 * The window is limited to ~10 s by the 32-bit cycle counter. elapsed_us may be NULL. */
int32_t INA228_WindowEnd(uint8_t dev_addr, int32_t *avg_mA, int32_t *avg_mW, uint32_t *elapsed_us) {
    int64_t  charge_nC = 0;
    uint32_t vbus_uV   = 0;
    uint32_t cycles;
    int64_t  us;

    if (INA228_StopConversions(dev_addr) != 0) {
        return -1;
    }
    cycles = Get_SysTick_Cycle_Count32() - window_start_cycles;

    if ((INA228_ReadCHARGE(dev_addr, &charge_nC) != 0) ||
        (INA228_ReadVBUS(dev_addr, &vbus_uV) != 0)) {
        return -1;
    }

    /* The cycle counter is derived from SysTick, which is configured from this clock. */
    us = (int64_t)(((uint64_t)cycles * 1000000ULL) / GetSystemCoreClock());
    if (us <= 0) {
        return -1;
    }

    /* nC / us == mA */
    *avg_mA = (int32_t)(charge_nC / us);
    /* uV * nC / us == nW; keep full precision before the final /1e6 to mW. */
    *avg_mW = (int32_t)((((int64_t)vbus_uV * charge_nC) / us) / 1000000LL);
    if (elapsed_us) {
        *elapsed_us = (uint32_t)us;
    }
    return 0;
}
#endif
