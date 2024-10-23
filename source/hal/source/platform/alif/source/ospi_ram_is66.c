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

#include "board.h"
#include "pinconf.h"
#include "Driver_OSPI.h"
#include "ospi.h"
#include "ospi_hyperram_xip.h"
#include "sys_ctrl_aes.h"

#include "timer_alif.h"
#include "ospi_ram.h"
#include "log_macros.h"

#ifdef BOARD_HAS_IS66_RAM

#define DRIVER_OSPI_NUM 0
//#define DRIVER_OSPI_BUS_SPEED 200000000
//#define DRIVER_OSPI_BUS_SPEED 10000000 // 20MHz FPGA
#define DRIVER_OSPI_BUS_SPEED 5000000 // 20MHz FPGA
//#define DRIVER_OSPI_BUS_SPEED 500000 // 20MHz FPGA
#define DDR_DRIVE_EDGE 1
#define RXDS_DELAY 11
#define ISSI_WAIT_CYCLES    6

static const ospi_hyperram_xip_config issi_config = {
    .instance       = DRIVER_OSPI_NUM,
    .bus_speed      = DRIVER_OSPI_BUS_SPEED,
    .hyperram_init  = NULL, /* No special initialization needed by the hyperram device */
    .ddr_drive_edge = DDR_DRIVE_EDGE,
    .rxds_delay     = RXDS_DELAY,
    .wait_cycles    = ISSI_WAIT_CYCLES,
    .slave_select   = 0,
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

#if 0
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
#endif

int32_t ospi_ram_init(void)
{
#if 0 // Only needed if we want to do some speed tuning setup

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

    ret = ptrOSPI->Control(ARM_OSPI_SET_DDR_MODE | control_flags, ARM_OSPI_DDR_ENABLE);
    if (ret != ARM_DRIVER_OK)
    {
        printf_err("OSPI control DDR mode failed\n");
        return ret;
    }

    bool slave_good[DRIVER_RAM_DEVICES] = { 0 };
    for (int slave = 0; slave < DRIVER_RAM_DEVICES; slave++) {
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
                return ret;
            }

            //printf("delay %2d: MR2 = %02" PRIX8 "\n", delay, value);
       // }
        printf("MR2 = %02" PRIX8 "\n", value);

        // MR0: Set minimum variable read latency (3 cycles)
        ret = reg_write(0, 0);
        if (ret != ARM_DRIVER_OK)
        {
            return ret;
        }

        reg_read_wait_cycles = RAM_DEVICE_REG_READ_WAIT_CYCLES;

        // MR4: Minimum write latency (3 cycles), auto slow refresh
        ret = reg_write(4, 0x18);
        if (ret != ARM_DRIVER_OK)
        {
            return ret;
        }

        // MR8: x8, 32-byte wrap, no row cross
        ret = reg_write(8, 0x03);
        if (ret != ARM_DRIVER_OK)
        {
            return ret;
        }

        ospi_control_ss(ospi, slave, SPI_SS_STATE_DISABLE);
        printf("APS512XX RAM %d apparently okay\n", slave);
        slave_good[slave] = true;
    }

#else
#ifdef DEVICE_FEATURE_OSPI_CTRL_CLK_ENABLE
    CLKCTL_PER_SLV->OSPI_CTRL |= 1 << DRIVER_OSPI_NUM;
#endif
#endif

    if (ospi_hyperram_xip_init(&issi_config) < 0)
    {
        printf("Hyperram XIP init failed\n");
        return ARM_DRIVER_ERROR;
    }

#if 0
    printf("XIP test RAM - may lock up\n");
    (void)*(volatile uint32_t *) 0xA0000000;
    printf("XIP test RAM OK\n");
#endif

    return ARM_DRIVER_OK;
}

#endif // BOARD_HAS_IS66_RAM
