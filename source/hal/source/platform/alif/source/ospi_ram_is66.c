/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "board_defs.h"

#if BOARD_ISSI_HYPERRAM_PRESENT

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "pinconf.h"
#include "Driver_IO.h"
#include "ospi.h"
#include "ospi_psram_xip.h"
#include "ospi_ram.h"
#include "hal_log.h"

#define ISSI_WAIT_CYCLES    6 // 166MHz max
#define OSPI_RESET_PORT     BOARD_IS66_HYPERRAM_RESET_GPIO_PORT
#define OSPI_RESET_PIN      BOARD_IS66_HYPERRAM_RESET_GPIO_PIN

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(OSPI_RESET_PORT);
static ARM_DRIVER_GPIO* const GPIODrv = &ARM_Driver_GPIO_(OSPI_RESET_PORT);


static int32_t ospi_toggle_reset(void)
{
    int32_t ret = GPIODrv->Initialize(OSPI_RESET_PIN, NULL);
    if (ret != ARM_DRIVER_OK) { return ret; }

    ret = GPIODrv->PowerControl(OSPI_RESET_PIN, ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) { return ret; }

    ret = GPIODrv->SetDirection(OSPI_RESET_PIN, GPIO_PIN_DIRECTION_OUTPUT);
    if (ret != ARM_DRIVER_OK) { return ret; }

    ret = GPIODrv->SetValue(OSPI_RESET_PIN, GPIO_PIN_OUTPUT_STATE_LOW);
    if (ret != ARM_DRIVER_OK) { return ret; }

    ret = GPIODrv->SetValue(OSPI_RESET_PIN, GPIO_PIN_OUTPUT_STATE_HIGH);
    if (ret != ARM_DRIVER_OK) { return ret; }

    return ret;
}


int32_t is66_psram_init(OSPI_Type *ospi, AES_Type *aes)
{
    (void)aes;
    ospi_transfer_t ospi_config;
    uint32_t        buff[3];

    /*
     * CA bit assignment for Configuration Register 0 write operation
     * bit[47] - bit[40] -> 60h
     * bit[39] - bit[32] -> 00h
     * bit[31] - bit[24] -> 01h
     * bit[23] - bit[16] -> 00h
     * bit[15] - bit[8]  -> 00h
     * bit[7]  - bit[0]  -> 00h
     */
    const uint16_t cr0 = (1 << 15)  // Normal operation
                | (0 << 12)  // 34 ohm output drive strength
                | (7 << 8)   // reserved
                | (((ISSI_WAIT_CYCLES - 5) & 0xF) << 4)
                | (0 << 3)   // variable latency
                | (1 << 2)   // standard wrapped operation
                | (3 << 0);  // 32-byte wrap

    buff[0]                    = 0x60000100;                         /* bit[8] - bit[47] */
    buff[1]                    = 0x0;                                /* bit[0] - bit[7] */
    buff[2]                    = cr0;

    ospi_config.spi_frf        = SPI_FRF_OCTAL;
    ospi_config.ddr            = 1;
    ospi_config.inst_len       = SPI_INST_L_0_BIT;
    ospi_config.addr_len       = SPI_ADDR_L_48_BIT;
    ospi_config.dummy_cycle    = 0;
    ospi_config.tx_total_cnt   = 3;
    ospi_config.tx_current_cnt = 0;
    ospi_config.tx_buff        = buff;

    ospi_set_dfs(ospi, 16);
    ospi_hyperbus_send(ospi, &ospi_config);

    return 0;
}

static ospi_psram_xip_config ram_config = {
    .instance       = BOARD_PSRAM_OSPI_INSTANCE,
    .ram_init       = is66_psram_init,
    .ram_type       = RAM_TYPE_HYPERRAM
};

int32_t ospi_ram_init(void)
{
    int32_t ret = ospi_toggle_reset();
    if (ret != ARM_DRIVER_OK) {
        printf_err("OSPI reset failed\n");
    }

    if (ospi_psram_xip_init(&ram_config) < 0)
    {
        printf_err("PSRAM XIP init failed\n");
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

#endif // BOARD_ISSI_HYPERRAM_PRESENT
