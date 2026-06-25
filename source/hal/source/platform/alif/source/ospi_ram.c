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

#include <string.h>
#include <stdio.h>
#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "pinconf.h"
#include "Driver_IO.h"
#include "ospi.h"
#include "ospi_psram_xip.h"
#include "ospi_ram.h"
#include "APS512XXN_PSRAM.h"
#include "hal_log.h"

#define ISSI_WAIT_CYCLES    6 // 166MHz max
#define DEBUG_ISSI_CR0_READBACK 0 // Set to 1 to enable reading back the ISSI CR0 register after writing it

#ifdef BOARD_IS66_HYPERRAM_RESET_GPIO_PORT

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
#else

static int32_t ospi_toggle_reset(void)
{
    return ARM_DRIVER_OK;
}

#endif

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
                | (0xF << 8) // reserved - must write all 1s
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
    ospi_config.dummy_cycle    = 0; // Configuration register write does not require dummy cycles
    ospi_config.tx_total_cnt   = 3;
    ospi_config.tx_current_cnt = 0;
    ospi_config.tx_buff        = buff;

    ospi_set_dfs(ospi, 16);
    ospi_hyperbus_send(ospi, &ospi_config);

#if DEBUG_ISSI_CR0_READBACK
    /* Read back Configuration Register 0 */
    uint16_t rx_data = 0;
    buff[0]                    = 0xE0000100;                         /* Read CR0: bit[47]=1 (read) */
    buff[1]                    = 0x0;

    ospi_config.tx_buff        = buff;
    ospi_config.dummy_cycle    = ISSI_WAIT_CYCLES;
    ospi_config.rx_total_cnt   = 1;
    ospi_config.rx_current_cnt = 0;
    ospi_config.rx_buff        = &rx_data;

    ospi_hyperbus_receive(ospi, &ospi_config);

    printf("IS66 CR0: 0x%04X\n", rx_data);
#endif

    return 0;
}


static int32_t init_psram_config(OSPI_INSTANCE instance, ospi_psram_xip_config *config)
{
    if (!config) {
        return -1;
    }

    memset(config, 0, sizeof(ospi_psram_xip_config));

    config->config_mode = CONFIG_MODE_USER_PARAMETERS;
    config->instance = instance;

    if (config->instance == OSPI_INSTANCE_0) {
        config->spi_frf = RTE_OSPI0_SPI_FRAME_FORMAT;
        config->bus_speed = RTE_OSPI0_BUS_SPEED;
        config->ddr_drive_edge = RTE_OSPI0_DDR_DRIVE_EDGE;
        config->rxds_delay = RTE_OSPI0_RXDS_DELAY;
#if SOC_FEAT_AES_OSPI_SIGNALS_DELAY
        config->signal_delay = RTE_OSPI0_SIGNAL_DELAY;
#endif
        config->dfs = RTE_OSPI0_DFS;

// On E1C, the RAM is connected to OSPI0 and uses chip select 0
// The RTE config may be configured for flash in same OSPI port, so we override it here to ensure the correct chip select is used for RAM
#ifdef BOARD_RAM_OSPI_SS
        config->slave_select = BOARD_RAM_OSPI_SS;
#else
        config->slave_select = RTE_OSPI0_CHIP_SELECTION_PIN;
#endif

        config->wait_cycles = RTE_OSPI0_WAIT_CYCLES;

#ifdef RTE_OSPI1
    } else if (config->instance == OSPI_INSTANCE_1) {
        config->spi_frf = RTE_OSPI1_SPI_FRAME_FORMAT;
        config->bus_speed = RTE_OSPI1_BUS_SPEED;
        config->ddr_drive_edge = RTE_OSPI1_DDR_DRIVE_EDGE;
        config->rxds_delay = RTE_OSPI1_RXDS_DELAY;
#if SOC_FEAT_AES_OSPI_SIGNALS_DELAY
        config->signal_delay = RTE_OSPI1_SIGNAL_DELAY;
#endif
        config->dfs = RTE_OSPI1_DFS;
        config->slave_select = RTE_OSPI1_CHIP_SELECTION_PIN;
        config->wait_cycles = RTE_OSPI1_WAIT_CYCLES;
#endif
    } else {
        return -1;
    }

    return 0;
}

static ospi_psram_xip_config psram_config = {0};


int32_t ospi_ram_init(void)
{
    int32_t ret = init_psram_config(BOARD_PSRAM_OSPI_INSTANCE, &psram_config);
    if (ret) {
        printf_err("PSRAM config error\n");
        return ARM_DRIVER_ERROR;
    }

    ret = ospi_toggle_reset();
    if (ret != ARM_DRIVER_OK) {
        printf_err("OSPI reset failed\n");
        return ret;
    }

#if BOARD_APS512XXN_PSRAM_PRESENT
    // Try to initialize APS512XXN PSRAM first
    psram_config.ram_init = aps512xxn_psram_init;
    psram_config.ram_type = RAM_TYPE_PSRAM;
    psram_config.wait_cycles = RTE_APS512XXN_PSRAM_WAIT_CYCLES;
    if (ospi_psram_xip_init(&psram_config) == 0)
    {
        printf("APS512XXN RAM detected\n");
        return ARM_DRIVER_OK;
    }
#endif

#if BOARD_ISSI_HYPERRAM_PRESENT
    // APS512XXN PSRAM not detected, try IS66 HyperRAM
    init_psram_config(BOARD_PSRAM_OSPI_INSTANCE, &psram_config);
    psram_config.ram_init = is66_psram_init;
    psram_config.ram_type = RAM_TYPE_HYPERRAM;
    psram_config.wait_cycles = ISSI_WAIT_CYCLES;
    if (ospi_psram_xip_init(&psram_config) == 0)
    {
        printf("IS66 RAM detected\n");
        return ARM_DRIVER_OK;
    }
#endif

    return ARM_DRIVER_ERROR;
}
