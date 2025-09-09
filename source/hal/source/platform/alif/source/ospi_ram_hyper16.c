/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
/**************************************************************************//**
 * @brief Methods for initializing OSPI flash in XIP mode for executing ML model
 *        from flash device. Defaults to E7 devkit gen2 pinconfig and flash device
 ******************************************************************************/

#include <stdatomic.h>
#include <stdbool.h>

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "board_defs.h"
#include "pinconf.h"
#include "Driver_GPIO.h"
#include "Driver_OSPI.h"
#include "ospi.h"
#include "ospi_hyperram_xip.h"
#include "sys_ctrl_aes.h"

#include "timer_alif.h"
#include "ospi_ram.h"
#include "ram_test.h"
#include "log_macros.h"

#ifdef BOARD_HAS_HYPER16_RAM

#define DRIVER_OSPI_NUM 1
//#define DRIVER_OSPI_BUS_SPEED 200000000
//#define DRIVER_OSPI_BUS_SPEED 10000000 // 20MHz FPGA
#define DRIVER_OSPI_BUS_SPEED 5000000 // 20MHz FPGA
//#define DRIVER_OSPI_BUS_SPEED 500000 // 20MHz FPGA
#define DDR_DRIVE_EDGE 1
#define RXDS_DELAY 11
#define SET_VARIABLE_LATENCY
#define WAIT_CYCLES    3

#define OSPI_RESET_PORT     BOARD_OSPI_RAM_RESET_GPIO_PORT
#define OSPI_RESET_PIN      BOARD_OSPI_RAM_RESET_PIN_NO

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(OSPI_RESET_PORT);
static ARM_DRIVER_GPIO* const GPIODrv = &ARM_Driver_GPIO_(OSPI_RESET_PORT);

static void hyperram_init(void);

static const ospi_hyperram_xip_config issi_config = {
    .instance       = DRIVER_OSPI_NUM,
    .bus_speed      = DRIVER_OSPI_BUS_SPEED,
    .hyperram_init  = hyperram_init,
    .ddr_drive_edge = DDR_DRIVE_EDGE,
    .rxds_delay     = RXDS_DELAY,
    .wait_cycles    = WAIT_CYCLES,
    .slave_select   = 0,
    .dfs            = 32,
    .spi_mode       = OSPI_SPI_MODE_DUAL_OCTAL
};

extern ARM_DRIVER_OSPI ARM_Driver_OSPI_(DRIVER_OSPI_NUM);
static ARM_DRIVER_OSPI * const ptrOSPI = &ARM_Driver_OSPI_(DRIVER_OSPI_NUM);

#if DRIVER_OSPI_NUM == 0
static OSPI_Type * const ospi = (OSPI_Type *) OSPI0_BASE;
static AES_Type * const aes = (AES_Type *)AES0_BASE;
#else
static OSPI_Type * const ospi = (OSPI_Type *) OSPI1_BASE;
static AES_Type * const aes = (AES_Type *)AES1_BASE;
#endif


#define WAIT_TIMEOUT                            128

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

#ifdef SET_VARIABLE_LATENCY
static void hyperram_write_register(uint32_t addr, uint16_t value)
{
    /* Note that register accesses use just an 8-bit bus */
    ospi_disable(ospi);
    ospi->OSPI_CTRLR0 = SPI_CTRLR0_SSI_IS_MST_MASTER
                      | SPI_CTRLR0_SPI_HYPERBUS_ENABLE
                      | SPI_CTRLR0_SPI_FRF_OCTAL
                      | SPI_CTRLR0_TMOD_SEND_ONLY
                      | SPI_CTRLR0_DFS_16bit;

    ospi->OSPI_SPI_CTRLR0 = (0 << SPI_CTRLR0_SPI_RXDS_SIG_EN_OFFSET)
                          | (0 << SPI_CTRLR0_SPI_DM_EN_OFFSET)
                          | (SPI_CTRLR0_SPI_RXDS_ENABLE << SPI_CTRLR0_SPI_RXDS_EN_OFFSET)
                          | (1 << SPI_CTRLR0_SPI_DDR_EN_OFFSET)
                          | (0 << SPI_CTRLR0_WAIT_CYCLES_OFFSET)
                          | (SPI_CTRLR0_INST_L_0bit << SPI_CTRLR0_INST_L_OFFSET)
                          | (12 << SPI_CTRLR0_ADDR_L_OFFSET)
                          | (SPI_TRANS_TYPE_FRF_DEFINED << SPI_CTRLR0_TRANS_TYPE_OFFSET);

    ospi->OSPI_TXFTLR = 2 << SPI_TXFTLR_TXFTHR_SHIFT;

    ospi_enable(ospi);

    ospi->OSPI_DR0 = 0x60000000 | (addr >> 16);
    ospi->OSPI_DR0 = addr & 0xFFFF;
    ospi->OSPI_DR0 = value;

    while (!(ospi->OSPI_SR & SPI_SR_TX_FIFO_EMPTY)) {

    }
}
#endif

static void hyperram_init(void)
{
    int32_t ret = ospi_toggle_reset();
    if (ret != ARM_DRIVER_OK) {
        printf_err("OSPI reset failed\n");
    }

#ifdef SET_VARIABLE_LATENCY
    ospi_disable(ospi);

    ospi->OSPI_SER = 1;

#if WAIT_CYCLES < 3 || WAIT_CYCLES > 7
#error "Invalid wait cycles"
#endif

    uint16_t cr = (1 << 15) // Normal operation
                | (0 << 12) // 34 ohm output drive strength (default)
                | (((WAIT_CYCLES - 5) & 0xF) << 4) // initial latency
                | (0 << 3) // variable latency
                | (1 << 2) // standard wrapped operation
                | (2 << 0); // 16-word (32-byte) wrap

    hyperram_write_register(0x01000000, cr);
#endif
}

int32_t ospi_ram_init(void)
{
#ifdef DEVICE_FEATURE_OSPI_CTRL_CLK_ENABLE
    CLKCTL_PER_SLV->OSPI_CTRL |= 1 << DRIVER_OSPI_NUM;
#endif

    if (ospi_hyperram_xip_init(&issi_config) < 0)
    {
        printf("Hyperram XIP init failed\n");
        return ARM_DRIVER_ERROR;
    }

    printf("XIP write test\n");
    *(volatile uint32_t *) BOARD_OSPI_RAM_BASE = 0x12345678;
    printf("Write OK\n");

    printf("XIP test HSPI RAM - may lock up\n");
    (void)*(volatile uint32_t *) BOARD_OSPI_RAM_BASE;
    printf("XIP test HSPI RAM OK\n");

    printf("Ram bit test\n");
    ram_address_test((uint8_t *) BOARD_OSPI_RAM_BASE, BOARD_OSPI_RAM_SIZE);

    printf("Linear test HSPI RAM\n");
    ram_linear_test((uint8_t *) BOARD_OSPI_RAM_BASE);

    printf("Random test HSPI RAM\n");
    ram_random_test((uint8_t *) (BOARD_OSPI_RAM_BASE + BOARD_OSPI_RAM_SIZE / 2));

    return ARM_DRIVER_OK;
}

#endif // BOARD_HAS_IS66_RAM
