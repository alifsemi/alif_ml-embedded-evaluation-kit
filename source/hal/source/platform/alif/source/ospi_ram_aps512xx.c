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
#include <stdlib.h>
#include <string.h>

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header
#include "board.h"

#include "pinconf.h"
#include "Driver_OSPI.h"
//#include "Driver_OSPI_Private.h"
#include "ospi.h"
#include "sys_ctrl_aes.h"

#include "timer_alif.h"
#include "ospi_ram.h"
#include "log_macros.h"

#ifdef BOARD_HAS_APS512XX_RAM

#define DRIVER_OSPI_NUM 1
//#define DRIVER_OSPI_BUS_SPEED 200000000
//#define DRIVER_OSPI_BUS_SPEED 10000000 // 20MHz FPGA
#define DRIVER_OSPI_BUS_SPEED 5000000 // 20MHz FPGA
//#define DRIVER_OSPI_BUS_SPEED 500000 // 20MHz FPGA
#define DRIVER_RAM_DEVICES 2
#define DRIVER_FIRST_GOOD_RAM 1
#define DRIVER_LAST_GOOD_RAM 1
#define DRIVER_VL 1

#define DRIVER_INST_DDR 1

extern ARM_DRIVER_OSPI ARM_Driver_OSPI_(DRIVER_OSPI_NUM);
static ARM_DRIVER_OSPI * const ptrOSPI = &ARM_Driver_OSPI_(DRIVER_OSPI_NUM);

#if DRIVER_OSPI_NUM == 0
static OSPI_Type * const ospi = (OSPI_Type *) OSPI0_BASE;
static AES_Type * const aes = (AES_Type *)AES0_BASE;
#else
static OSPI_Type * const ospi = (OSPI_Type *) OSPI1_BASE;
static AES_Type * const aes = (AES_Type *)AES1_BASE;
#endif

#if DRIVER_INST_DDR
#define CMD_SYNC_READ           0x0000
#define CMD_SYNC_WRITE          0x8080
#define CMD_LINEAR_BURST_READ   0x2020
#define CMD_LINEAR_BURST_WRITE  0xA0A0
#define CMD_MODE_REGISTER_READ  0x4040
#define CMD_MODE_REGISTER_WRITE 0xC0C0
#define CMD_GLOBAL_RESET        0xFFFF
#else
#define CMD_SYNC_READ           0x00
#define CMD_SYNC_WRITE          0x80
#define CMD_LINEAR_BURST_READ   0x20
#define CMD_LINEAR_BURST_WRITE  0xA0
#define CMD_MODE_REGISTER_READ  0x40
#define CMD_MODE_REGISTER_WRITE 0xC0
#define CMD_GLOBAL_RESET        0xFF
#endif

#define RAM_DEVICE_FAST_READ_WAIT_CYCLES        (DRIVER_VL ? 2 : 5)
#define RAM_DEVICE_FAST_WRITE_WAIT_CYCLES       2
#define RAM_DEVICE_INIT_REG_READ_WAIT_CYCLES    4
#define RAM_DEVICE_REG_READ_WAIT_CYCLES         2
#define RAM_DEVICE_REG_WRITE_WAIT_CYCLES        0
#define RAM_DEVICE_RESET_WAIT_CYCLES            3

#define RXDS_DELAY                              11

#define WAIT_TIMEOUT                            128

static int reg_read_wait_cycles = RAM_DEVICE_INIT_REG_READ_WAIT_CYCLES;
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
            | (0x2 << XIP_CTRL_TRANS_TYPE_OFFSET)
            | (0x8 << XIP_CTRL_ADDR_L_OFFSET)
            | ((0x2 + DRIVER_INST_DDR) << XIP_CTRL_INST_L_OFFSET)
            | (0x0 << XIP_CTRL_MD_BITS_EN_OFFSET)
            | (RAM_DEVICE_FAST_READ_WAIT_CYCLES << XIP_CTRL_WAIT_CYCLES_OFFSET)
            | (0x1 << XIP_CTRL_DFS_HC_OFFSET)
            | (0x1 << XIP_CTRL_DDR_EN_OFFSET)
            | (DRIVER_INST_DDR << XIP_CTRL_INST_DDR_EN_OFFSET)
            | (0x1 << XIP_CTRL_RXDS_EN_OFFSET)
            | (0x1 << XIP_CTRL_INST_EN_OFFSET)
            | (0x0 << XIP_CTRL_CONT_XFER_EN_OFFSET)
            | (0x0 << XIP_CTRL_XIP_HYPERBUS_EN_OFFSET)
            | (0x0 << XIP_CTRL_RXDS_SIG_EN_OFFSET)
            | (0x0 << XIP_CTRL_XIP_MBL_OFFSET)
            | (0x0 << XIP_CTRL_XIP_PREFETCH_EN_OFFSET)
            | (DRIVER_VL << XIP_CTRL_RXDS_VL_EN_OFFSET);

    ospi->OSPI_XIP_CTRL = val;

    val = (SPI_FRAME_FORMAT_OCTAL << XIP_WRITE_CTRL_WR_FRF_OFFSET)
             | (0x2 << XIP_WRITE_CTRL_WR_TRANS_TYPE_OFFSET)
             | (0x8 << XIP_WRITE_CTRL_WR_ADDR_L_OFFSET)
             | ((0x2 + DRIVER_INST_DDR) << XIP_WRITE_CTRL_WR_INST_L_OFFSET)
             | (0x1 << XIP_WRITE_CTRL_WR_SPI_DDR_EN_OFFSET)
             | (DRIVER_INST_DDR << XIP_WRITE_CTRL_WR_INST_DDR_EN_OFFSET)
             | (0x0 << XIP_WRITE_CTRL_XIPWR_HYPERBUS_EN_OFFSET)
             | (0x0 << XIP_WRITE_CTRL_XIPWR_RXDS_SIG_EN_OFFSET)
             | (0x1 << XIP_WRITE_CTRL_XIPWR_DM_EN_OFFSET)
             | (RAM_DEVICE_FAST_WRITE_WAIT_CYCLES << XIP_WRITE_CTRL_XIPWR_WAIT_CYCLES)
             | (0x1 << XIP_WRITE_CTRL_XIPWR_DFS_HC_OFFSET);

    ospi->OSPI_XIP_WRITE_CTRL = val;

    ospi->OSPI_RX_SAMPLE_DELAY = 0;

    ospi->OSPI_XIP_MODE_BITS = 0x00;
    ospi->OSPI_XIP_INCR_INST = CMD_LINEAR_BURST_READ;
    ospi->OSPI_XIP_WRAP_INST = CMD_SYNC_READ;
    ospi->OSPI_XIP_WRITE_INCR_INST = CMD_LINEAR_BURST_WRITE;
    ospi->OSPI_XIP_WRITE_WRAP_INST = CMD_SYNC_WRITE;
    //ospi->OSPI_XIP_SER = 1;

    ospi->OSPI_XIP_CNT_TIME_OUT = 255;

    ospi_enable(ospi);

    aes->AES_CONTROL |= AES_CONTROL_XIP_EN;
}

static uint32_t control_flags = ARM_OSPI_DATA_BITS(16) |
        ARM_OSPI_SS_MASTER_HW_OUTPUT | ARM_OSPI_CPOL0_CPHA0 | ARM_OSPI_MSB_LSB;

static int32_t do_global_reset(void)
{
    int32_t ret;
    uint32_t cmd[1];

    ret = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (RAM_DEVICE_RESET_WAIT_CYCLES << ARM_OSPI_WAIT_CYCLE_POS));
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("do_global_reset OSPI control len/wait failed\n");
        return ret;
    }

    event_flag = 0;
    cmd[0] = CMD_GLOBAL_RESET;
    ret = ptrOSPI->Send(&cmd[0], 1);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("do_global_reset OSPI send failed\n");
        return ret;
    }
    ret = wait_for_completion();
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("do_global_reset wait failed\n");
        return ret;
    }

    return ARM_DRIVER_OK;
}


static int32_t reg_read(uint8_t reg, uint8_t *value)
{
    int32_t ret;
    uint32_t cmd[2];
    uint16_t data[2];

    ret = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (reg_read_wait_cycles << ARM_OSPI_WAIT_CYCLE_POS));
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("reg_read OSPI control len/wait failed\n");
        return ret;
    }

    event_flag = 0;
    cmd[0] = CMD_MODE_REGISTER_READ;
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
    *value = data[0] >> 8;
    return ARM_DRIVER_OK;
}

static int32_t reg_write(uint8_t reg, uint8_t value)
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
    cmd[0] = CMD_MODE_REGISTER_WRITE;
    cmd[1] = reg;
    cmd[2] = value << 8;
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

static void linear_test(uint8_t *ram)
{
#if 1
#define BIG_TEST_SIZE_M 1
    srand(1);
    printf("Writing %dM\n", BIG_TEST_SIZE_M);
    for (int i = 0; i < BIG_TEST_SIZE_M*1024*1024; i++) {
        ram[i] = rand();
    }
    printf("Reading %dM\n", BIG_TEST_SIZE_M);
    srand(1);
    int err_cnt = 0;
    for (int i = 0; i < BIG_TEST_SIZE_M*1024*1024; i++) {
        uint8_t expected = rand();
        if (ram[i] != expected) {
            ++err_cnt;
            if (err_cnt < 20) {
                printf_err("Mismatch at %p: Got %02X expected %02X\n", (void*) &ram[i], ram[i], expected);
            } else if (err_cnt == 20) {
                printf_err("...\n");
            }
        }
    }
    printf("Finished linear test (%d matches, %d mismatches)\n", BIG_TEST_SIZE_M*1024*1024 - err_cnt, err_cnt);
#endif
}

static void random_test(uint8_t *test_ram)
{
#define TEST_SIZE 0x0020000
#define TEST_ITERS (1024*1024)
    static uint8_t check_ram[TEST_SIZE] __attribute__((section(".bss.large_ram"))) __attribute__((aligned(16)));

    printf("Init test (%d iterations in %d bytes)\n", TEST_ITERS, TEST_SIZE);
    memset((uint8_t *)test_ram, 0xAA, TEST_SIZE);
    memset(check_ram, 0xAA, TEST_SIZE);
    printf("Start\n");
    for (int i = 0; i < TEST_ITERS; i++) {
        size_t pos = rand() & (TEST_SIZE-1);
        uint32_t val = rand();

        switch (rand() & 7) {
        case 0:
            check_ram[pos] = test_ram[pos] = val;
            break;
        case 1:
            if (pos + 2 > TEST_SIZE) break;
            __UNALIGNED_UINT16_WRITE(check_ram+pos, val);
            __UNALIGNED_UINT16_WRITE(test_ram+pos, val);
            break;
        case 2:
            if (pos + 4 > TEST_SIZE) break;
            __UNALIGNED_UINT32_WRITE(check_ram+pos, val);
            __UNALIGNED_UINT32_WRITE(test_ram+pos, val);
            break;
        case 3:
            if (pos + 11 > TEST_SIZE) break;
            memset(check_ram+pos, val, 11);
            memset((uint8_t *)test_ram+pos, val, 11);
            break;
        case 4:
            if (pos + 3 > TEST_SIZE) break;
            check_ram[pos] = check_ram[pos + 2] = val;
            test_ram[pos] = test_ram[pos + 2] = val;
            break;
        case 5:
            if (pos + 6 > TEST_SIZE) break;
            check_ram[pos] = check_ram[pos + 5] = val;
            test_ram[pos] = val; test_ram[pos + 5] = val;
            break;
        case 6:
#if 0
            check_ram[pos] = test_ram[pos] = val;
            check_ram[pos + 3] = test_ram[pos + 3] = val;
            check_ram[pos + 6] = test_ram[pos + 6] = val;
#else
            pos = (pos &~ 0x1F);
#if 1
            *(uint32_t *)(test_ram+pos + 0xC) = val;
            *(uint16_t *)(test_ram+pos + 0x10) = val;
            *(uint8_t *)(test_ram+pos + 0x12) = val;
            *(uint32_t *)(test_ram+pos + 0x14) = val;
            *(uint32_t *)(test_ram+pos + 0x18) = val;
            *(uint32_t *)(check_ram+pos + 0xC) = val;
            *(uint16_t *)(check_ram+pos + 0x10) = val;
            *(uint8_t *)(check_ram+pos + 0x12) = val;
            *(uint32_t *)(check_ram+pos + 0x14) = val;
            *(uint32_t *)(check_ram+pos + 0x18) = val;
#else
            *(uint32_t *)(check_ram+pos + 0x4) = val;
            *(uint16_t *)(check_ram+pos + 0x8) = val;
            *(uint8_t *)(check_ram+pos + 0xA) = val;
            *(uint32_t *)(test_ram+pos + 0x4) = val;
            *(uint16_t *)(test_ram+pos + 0x8) = val;
            *(uint8_t *)(test_ram+pos + 0xA) = val;
#endif
#endif
            break;
        case 7:
            if (!(rand() & 0xF) && pos + 1024 <= TEST_SIZE) {
                memset(check_ram+pos, val, 1024);
                memset((uint8_t *)test_ram+pos, val, 1024);
            } else {
                if (pos + 17 > TEST_SIZE) break;
                check_ram[pos] = test_ram[pos] = test_ram[pos + 16];
            }
            break;
        }
    }

   // test_ram[2000] = 3;

    int err_cnt = 0;
    for (int i = 0; i < TEST_SIZE; i++) {
        if (test_ram[i] != check_ram[i]) {
            ++err_cnt;
            if (err_cnt < 20) {
                printf_err("Mismatch at %p: Got %02X expected %02X\n", (void*) &test_ram[i], test_ram[i], check_ram[i]);
            } else if (err_cnt == 20) {
                printf_err("...\n");
            }
        }
    }
    printf("Test complete (%d matches, %d mismatches)\n", TEST_SIZE - err_cnt, err_cnt);
}

int32_t ospi_ram_init(void)
{
    event_flag = 0;

    int32_t ret;
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

    ret = ptrOSPI->Control(ARM_OSPI_SET_DDR_MODE | control_flags, DRIVER_INST_DDR ? ARM_OSPI_DDR_ENABLE_DDR_INST : ARM_OSPI_DDR_ENABLE);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI control DDR mode failed\n");
        return ret;
    }

    ret = ptrOSPI->Control(ARM_OSPI_SET_INST_LENGTH | control_flags, DRIVER_INST_DDR ? ARM_OSPI_INST_LENGTH_16_BITS : ARM_OSPI_INST_LENGTH_8_BITS);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI control inst length failed\n");
        return ret;
    }

    bool slave_good[DRIVER_RAM_DEVICES] = { 0 };
    for (int slave = DRIVER_FIRST_GOOD_RAM; slave <= DRIVER_LAST_GOOD_RAM; slave++) {
        printf("Attempting APS512XX RAM initialisation (device %d)\n", slave);

        // OSPI driver doesn't provide dynamic chip select control
        ospi_control_ss(ospi, slave, SPI_SS_STATE_ENABLE);

        do_global_reset();

        reg_read_wait_cycles = RAM_DEVICE_INIT_REG_READ_WAIT_CYCLES;

       /* for (int delay = 0; delay < 15; delay++) {
            aes_set_rxds_delay(aes, delay);*/

            uint8_t value;
            ret = reg_read(2, &value);
            if (ret != ARM_DRIVER_OK)
            {
                printf("RAM %d read error\n", slave);
                continue;
            }

            //printf("delay %2d: MR2 = %02" PRIX8 "\n", delay, value);
       // }
        printf("MR2 = %02" PRIX8 "\n", value);

        // MR0: Set minimum read latency (3/6 cycles)
        ret = reg_write(0, DRIVER_VL ? 0x00 : 0x20);
        if (ret != ARM_DRIVER_OK)
        {
            printf("RAM %d LC set error\n", slave);
            continue;
        }

        reg_read_wait_cycles = RAM_DEVICE_REG_READ_WAIT_CYCLES;

        // MR4: Minimum write latency (3 cycles), auto slow refresh
        ret = reg_write(4, 0x18);
        if (ret != ARM_DRIVER_OK)
        {
            printf("RAM %d WLC set error\n", slave);
            continue;
        }

        // MR8: x8, 32-byte wrap, no row cross
        ret = reg_write(8, 0x01);
        if (ret != ARM_DRIVER_OK)
        {
            printf("RAM %d wrap set error\n", slave);
            continue;
        }

        ret = reg_read(8, &value);
        if (ret != ARM_DRIVER_OK)
        {
            printf("RAM %d MR8 read error\n", slave);
            continue;
        }
        printf("MR8 set to %02" PRIX8 "\n", value);


        ospi_control_ss(ospi, slave, SPI_SS_STATE_DISABLE);
        printf("APS512XX RAM %d apparently okay\n", slave);
        slave_good[slave] = true;
    }

    ospi_ram_enable_xip();

    if (slave_good[0]) {
        printf("XIP test RAM 0 - may lock up\n");
        (void)*(volatile uint32_t *) 0xCC000000;
        printf("XIP test RAM 0 OK\n");
    }

    if (slave_good[1]) {
        printf("XIP test RAM 1 - may lock up\n");
        (void)*(volatile uint32_t *) 0xD0000000;
        printf("XIP test RAM 1 OK\n");
    }

    if (slave_good[0]) {
        printf("Linear test RAM 0\n");
        linear_test((uint8_t *) 0xCC000000);
    }

    if (slave_good[1]) {
        printf("Linear test RAM 1\n");
        linear_test((uint8_t *) 0xD0000000);
    }

    if (slave_good[0]) {
        printf("Random test RAM 0\n");
        random_test((uint8_t *) 0xCE000000);
    }
    if (slave_good[1]) {
        printf("Random test RAM 1\n");
        random_test((uint8_t *) 0xD2000000);
    }

    return ARM_DRIVER_OK;
}

#endif //BOARD_HAS_APS512XX_RAM

