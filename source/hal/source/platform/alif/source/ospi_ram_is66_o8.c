/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header
#include "board.h"

#include "pinconf.h"
#include "Driver_OSPI.h"
//#include "Driver_OSPI_Private.h"
#include "Driver_GPIO.h"
#include "ospi.h"
#include "sys_ctrl_aes.h"

#include "timer_alif.h"
#include "ospi_ram.h"
#include "ram_test.h"
#include "log_macros.h"

#ifdef BOARD_HAS_IS66_O8_RAM
//#if 1

#define DRIVER_OSPI_NUM 1
//#define DRIVER_OSPI_BUS_SPEED 200000000
//#define DRIVER_OSPI_BUS_SPEED 10000000 // 20MHz FPGA
#define DRIVER_OSPI_BUS_SPEED 5000000 // 20MHz FPGA
//#define DRIVER_OSPI_BUS_SPEED 500000 // 20MHz FPGA
#define DRIVER_VL 1

extern ARM_DRIVER_OSPI ARM_Driver_OSPI_(DRIVER_OSPI_NUM);
static ARM_DRIVER_OSPI * const ptrOSPI = &ARM_Driver_OSPI_(DRIVER_OSPI_NUM);

#define OSPI_RESET_PORT     BOARD_OSPI_RAM_RESET_GPIO_PORT
#define OSPI_RESET_PIN      BOARD_OSPI_RAM_RESET_PIN_NO

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(OSPI_RESET_PORT);
static ARM_DRIVER_GPIO* const GPIODrv = &ARM_Driver_GPIO_(OSPI_RESET_PORT);

#if DRIVER_OSPI_NUM == 0
static OSPI_Type * const ospi = (OSPI_Type *) OSPI0_BASE;
static AES_Type * const aes = (AES_Type *)AES0_BASE;
#else
static OSPI_Type * const ospi = (OSPI_Type *) OSPI1_BASE;
static AES_Type * const aes = (AES_Type *)AES1_BASE;
#endif

#define CMD_READ_WRAPPED        0x8000
#define CMD_WRITE_WRAPPED       0x0000
#define CMD_READ_CONTINUOUS     0xA000
#define CMD_WRITE_CONTINUOUS    0x2000
#define CMD_REGISTER_READ       0xC000
#define CMD_REGISTER_WRITE      0x4000
#define CMD_GLOBAL_RESET        0xFFFF

#define REG_ID                  0x00000000
//#define REG_CONFIG              0x00001000
#define REG_CONFIG              0x00040000

#define RAM_DEVICE_DEFAULT_LC                   8
#define RAM_DEVICE_LC                           3

// Wait cycles counted from the end of the address phase as per the OSPI controller
// Latency count in ISSI datasheet counts from the last cycle of the address phase, so is one higher
// Plus we are in fixed-latency mode, so our controller wait cycles are 2*LC - 1.
#define RAM_DEVICE_DEFAULT_WAIT_CYCLES          (2*RAM_DEVICE_DEFAULT_LC - 1)
#define RAM_DEVICE_WAIT_CYCLES                  (2*RAM_DEVICE_LC - 1)
#define RAM_DEVICE_REG_WRITE_WAIT_CYCLES        0
// Device starts in variable latency mode, which gives constant 1*LC for register reads.
// We switch to fixed latency, which gives constant 2*LC.
#define RAM_DEVICE_DEFAULT_REG_READ_WAIT_CYCLES (RAM_DEVICE_DEFAULT_LC - 1)
#define RAM_DEVICE_REG_READ_WAIT_CYCLES         (2*RAM_DEVICE_LC - 1)

#define RXDS_DELAY                              11

#define WAIT_TIMEOUT                            128

static int current_wait_cycles;
static int current_reg_read_wait_cycles;
static atomic_uint_least32_t event_flag;
static void spi_callback_event(uint32_t event)
{
    event_flag = event;
}

static int32_t wait_for_completion(void)
{
    uint32_t start_time = Get_SysTick_Count();
    while (event_flag == 0 && Get_SysTick_Count() - start_time < WAIT_TIMEOUT) {
        __WFE();
    }
    int32_t flag = atomic_exchange(&event_flag, 0);
    if ((flag & (ARM_OSPI_EVENT_TRANSFER_COMPLETE | ARM_OSPI_EVENT_DATA_LOST | ARM_OSPI_EVENT_MODE_FAULT)) == ARM_OSPI_EVENT_TRANSFER_COMPLETE) {
        return ARM_DRIVER_OK;
    } else if (flag == 0) {
        return ARM_DRIVER_ERROR_TIMEOUT;
    } else {
        return ARM_DRIVER_ERROR;
    }
}

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

static void ospi_ram_enable_xip(void)
{
#if 0
    unsigned int clocks_per_sclk = getOSPICoreClock() / DRIVER_OSPI_BUS_SPEED;

    ospi_set_ddr_drive_edge(ospi, clocks_per_sclk / 4);

    ospi_set_bus_speed(ospi, DRIVER_OSPI_BUS_SPEED, getOSPICoreClock());
#endif

    ospi_disable(ospi);

    aes_set_rxds_delay(aes, RXDS_DELAY);

    uint32_t val = (1 << SPI_CTRLR0_SSI_IS_MST)
        | (SPI_FRAME_FORMAT_OCTAL << SPI_CTRLR0_SPI_FRF)
        | SPI_CTRLR0_SCPOL_LOW
        | SPI_CTRLR0_SCPH_LOW
        | (0 << SPI_CTRLR0_SSTE)
        | (SPI_TMOD_RX << SPI_CTRLR0_TMOD)
        | (SPI_CTRLR0_DFS_16bit << SPI_CTRLR0_DFS);

    ospi->OSPI_CTRLR0 = val;

    val = (SPI_FRAME_FORMAT_OCTAL << XIP_CTRL_FRF_OFFSET)
            | (SPI_TRANS_TYPE_FRF_DEFINED << XIP_CTRL_TRANS_TYPE_OFFSET)
            | (0x8 << XIP_CTRL_ADDR_L_OFFSET)
            | (0x3 << XIP_CTRL_INST_L_OFFSET)
            | (0x0 << XIP_CTRL_MD_BITS_EN_OFFSET)
            | (RAM_DEVICE_WAIT_CYCLES << XIP_CTRL_WAIT_CYCLES_OFFSET)
            | (0x1 << XIP_CTRL_DFS_HC_OFFSET)
            | (0x1 << XIP_CTRL_DDR_EN_OFFSET)
            | (0x1 << XIP_CTRL_INST_DDR_EN_OFFSET)
            | (0x1 << XIP_CTRL_RXDS_EN_OFFSET)
            | (0x1 << XIP_CTRL_INST_EN_OFFSET)
            | (0x0 << XIP_CTRL_CONT_XFER_EN_OFFSET)
            | (0x0 << XIP_CTRL_XIP_HYPERBUS_EN_OFFSET)
            | (0x0 << XIP_CTRL_RXDS_SIG_EN_OFFSET)
            | (0x0 << XIP_CTRL_XIP_MBL_OFFSET)
            | (0x0 << XIP_CTRL_XIP_PREFETCH_EN_OFFSET)
            | (0x0 << XIP_CTRL_RXDS_VL_EN_OFFSET);

    ospi->OSPI_XIP_CTRL = val;

    val = (SPI_FRAME_FORMAT_OCTAL << XIP_WRITE_CTRL_WR_FRF_OFFSET)
             | (SPI_TRANS_TYPE_FRF_DEFINED << XIP_WRITE_CTRL_WR_TRANS_TYPE_OFFSET)
             | (0x8 << XIP_WRITE_CTRL_WR_ADDR_L_OFFSET)
             | (0x3 << XIP_WRITE_CTRL_WR_INST_L_OFFSET)
             | (0x1 << XIP_WRITE_CTRL_WR_SPI_DDR_EN_OFFSET)
             | (0x1 << XIP_WRITE_CTRL_WR_INST_DDR_EN_OFFSET)
             | (0x0 << XIP_WRITE_CTRL_XIPWR_HYPERBUS_EN_OFFSET)
             | (0x0 << XIP_WRITE_CTRL_XIPWR_RXDS_SIG_EN_OFFSET)
             | (0x1 << XIP_WRITE_CTRL_XIPWR_DM_EN_OFFSET)
             | (RAM_DEVICE_WAIT_CYCLES << XIP_WRITE_CTRL_XIPWR_WAIT_CYCLES)
             | (0x1 << XIP_WRITE_CTRL_XIPWR_DFS_HC_OFFSET);

    ospi->OSPI_XIP_WRITE_CTRL = val;

    ospi->OSPI_RX_SAMPLE_DELAY = 0;

    ospi->OSPI_XIP_MODE_BITS = 0x00;
    ospi->OSPI_XIP_INCR_INST = CMD_READ_CONTINUOUS;
    ospi->OSPI_XIP_WRAP_INST = CMD_READ_WRAPPED;
    ospi->OSPI_XIP_WRITE_INCR_INST = CMD_WRITE_CONTINUOUS;
    ospi->OSPI_XIP_WRITE_WRAP_INST = CMD_WRITE_WRAPPED;
    //ospi->OSPI_XIP_SER = 1;

    ospi->OSPI_XIP_CNT_TIME_OUT = 255;

    ospi_enable(ospi);

    // SPI address signalling                 Corresponding HADDR/AxADDR bits in
    //
    //  --   RA7  CA9  --                         --  17     9  --
    // RA14  RA6  CA8  --                         23  16     8  --
    // RA13  RA5  CA7  --                         22  15     7  --
    // RA12  RA4  CA6  --                         22  14     6  --
    // RA11  RA3  CA5  CA3                        21  13     5   3
    // RA10  RA2  CA4  CA2                        20  12     4   2
    //  RA9  RA1  --   CA1                        19  11    --   1
    //  RA8  RA0  --   CA0 (=0)                   18  10    --   0 (=0)
    //
    // We massage HADDR using the shim in front of the controller to insert the gap between CA3 and CA4 - between HADDR[3] and HADDR[4]

    aes->AES_ADDR_CONTROL = (4 << 28) // 4 bits in the lower part of the address
                          | (10 << 22) // next bit HADDR[4] needs to be shifted to HADDR[10]
                          | (2 << 18) // apply transformation to only slave select 1
                          | (0x000F << 0); // mask for lower 4 bits

    aes->AES_CONTROL |= AES_CONTROL_XIP_EN;
}

static uint32_t control_flags = ARM_OSPI_DATA_BITS(16) |
        ARM_OSPI_SS_MASTER_HW_OUTPUT | ARM_OSPI_CPOL0_CPHA0 | ARM_OSPI_MSB_LSB;

static int32_t reg_read(uint32_t reg, uint16_t *value)
{
    int32_t ret;
    uint32_t cmd[2];
    uint16_t data[2];

    ret = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (current_reg_read_wait_cycles << ARM_OSPI_WAIT_CYCLE_POS));
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("reg_read OSPI control len/wait failed\n");
        return ret;
    }

    event_flag = 0;
    cmd[0] = CMD_REGISTER_READ;
    cmd[1] = reg;
    ret = ptrOSPI->Transfer(cmd, data, 1);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("reg_read OSPI transfer failed\n");
        return ret;
    }
    ret = wait_for_completion();
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("reg_read wait failed\n");
        return ret;
    }

    printf("data = %" PRIX16 "\n", data[0]);
    *value = data[0];
    return ARM_DRIVER_OK;
}

static int32_t reg_write(uint32_t reg, uint16_t value)
{
    int32_t ret;
    uint32_t cmd[3];

    ret = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (RAM_DEVICE_REG_WRITE_WAIT_CYCLES << ARM_OSPI_WAIT_CYCLE_POS));
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("reg_write OSPI control len/wait failed\n");
        return ret;
    }

    event_flag = 0;
    cmd[0] = CMD_REGISTER_WRITE;
    cmd[1] = reg;
    cmd[2] = value;
    ret = ptrOSPI->Send(cmd, 3);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("reg_write OSPI send failed\n");
        return ret;
    }
    ret = wait_for_completion();
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("reg_write wait failed\n");
        return ret;
    }

    return ARM_DRIVER_OK;
}

int32_t ospi_ram_init(void)
{
    int32_t ret = ospi_toggle_reset();
    if (ret != ARM_DRIVER_OK) {
        printf_err("OSPI reset failed\n");
        return ret;
    }

    event_flag = 0;

    ret = ptrOSPI->Initialize(spi_callback_event);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI driver init failed\n");
        return ret;
    }

#if DRIVER_OSPI_NUM == 1
    // Current pack doesn't enable OSPI1...
    CLKCTL_PER_SLV->OSPI_CTRL |= 1 << 1;
#endif

    ret = ptrOSPI->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI power control failed\n");
        return ret;
    }

    ret = ptrOSPI->Control(ARM_OSPI_CONTROL_SS | control_flags, ARM_OSPI_SS_ACTIVE);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI slave select control failed\n");
        return ret;
    }

    ret = ptrOSPI->Control(ARM_OSPI_MODE_MASTER | control_flags, DRIVER_OSPI_BUS_SPEED);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI control mode failed\n");
        return ret;
    }

    ret = ptrOSPI->Control(ARM_OSPI_SET_FRAME_FORMAT | control_flags, ARM_OSPI_FRF_OCTAL);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI control frame format failed\n");
        return ret;
    }

    ret = ptrOSPI->Control(ARM_OSPI_SET_DDR_MODE | control_flags, ARM_OSPI_DDR_ENABLE_DDR_INST);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI control DDR mode failed\n");
        return ret;
    }

    ret = ptrOSPI->Control(ARM_OSPI_SET_INST_LENGTH | control_flags, ARM_OSPI_INST_LENGTH_16_BITS);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI control inst length failed\n");
        return ret;
    }

    printf("Attempting IS66_O8 RAM initialisation\n");

    current_wait_cycles = RAM_DEVICE_DEFAULT_WAIT_CYCLES;
    current_reg_read_wait_cycles = RAM_DEVICE_DEFAULT_REG_READ_WAIT_CYCLES;

#if 1
    uint16_t id;
    ret = reg_read(REG_ID, &id);
    if (ret != ARM_DRIVER_OK)
    {
        printf("RAM ID read error\n");
        return ARM_DRIVER_ERROR;
    }
    printf("ID = %" PRIX16 "\n", id);
#endif

    uint16_t cr = (1 << 15) // Normal operation
                | (7 << 12) // 24 ohm output drive strength
                | (0 << 8) // 0-clock DQSM read pre-cycle
                | ((RAM_DEVICE_LC - 3) << 4) // latency count
                | (1 << 3) // fixed latency
                | (2 << 0); // 32-byte wrap

    ret = reg_write(REG_CONFIG, cr);
    if (ret != ARM_DRIVER_OK)
    {
        printf("RAM config write error\n");
        return ARM_DRIVER_ERROR;
    }

#if 1
    current_wait_cycles = RAM_DEVICE_WAIT_CYCLES;
    current_reg_read_wait_cycles = RAM_DEVICE_REG_READ_WAIT_CYCLES;

    ret = reg_read(REG_CONFIG, &cr);
    if (ret != ARM_DRIVER_OK)
    {
        printf("RAM CR read error\n");
        return ARM_DRIVER_ERROR;
    }
    printf("CR = %" PRIX16 "\n", cr);

    printf("OSPI RAM apparently okay\n");

#endif

    ospi_ram_enable_xip();

    printf("XIP write test\n");
    *(volatile uint32_t *) BOARD_OSPI_RAM_BASE = 0x12345678;
    printf("Write OK\n");

    printf("XIP test OSPI RAM - may lock up\n");
    (void)*(volatile uint32_t *) BOARD_OSPI_RAM_BASE;
    printf("XIP test OSPI RAM OK\n");

    printf("Ram bit test\n");
    ram_address_test((uint8_t *) BOARD_OSPI_RAM_BASE, BOARD_OSPI_RAM_SIZE);

    printf("Linear test OSPI RAM\n");
    ram_linear_test((uint8_t *) BOARD_OSPI_RAM_BASE);

    printf("Random test OSPI RAM\n");
    ram_random_test((uint8_t *) (BOARD_OSPI_RAM_BASE + BOARD_OSPI_RAM_SIZE / 2));

    return ARM_DRIVER_OK;
}

#endif //BOARD_HAS_IS66_O8_RAM

