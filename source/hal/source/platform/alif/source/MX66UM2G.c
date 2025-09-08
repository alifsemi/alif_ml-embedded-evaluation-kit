/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     Driver_FLASH.c
 * @author   Khushboo Singh
 * @email    khushboo.singh@alifsemi.com
 * @version  V1.0.0
 * @date     21-Oct-2022
 * @brief    ISSI FLASH Driver.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "Driver_Flash.h"
#include "Driver_OSPI.h"
#include "RTE_Device.h"
#include "RTE_Components.h"
#include "MX66UM2G.h"
#include CMSIS_device_header

#include "timer_alif.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#if FLASH_MX66UM2G_DO_DDR
#define DDR_OPTION ARM_OSPI_DDR_ENABLE_DDR_INST
#else
#define DDR_OPTION ARM_OSPI_DDR_DISABLE
#endif

#define DDR_FRAME_SIZE (DDR_OPTION == ARM_OSPI_DDR_DISABLE ? 8 : 16)

#if 0
#if !(RTE_MX66UM2G_FLASH)
#error "MX66UM2G Flash driver is not enabled in RTE_Device.h"
#endif

#if !(RTE_Drivers_MX66UM2G_FLASH)
#error "MX66UM2G Flash driver is not enabled in RTE_Components.h"
#endif
#endif

#define ARM_FLASH_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0) /* driver version */


#ifndef DRIVER_FLASH_NUM
#define DRIVER_FLASH_NUM                                         2         /* Default driver number */
#endif

#ifndef DRIVER_OSPI_NUM
#define DRIVER_OSPI_NUM                                          0 //RTE_MX66UM2G_FLASH_OSPI_DRV_NUM
#endif

#ifndef DRIVER_OSPI_BUS_SPEED
//#define DRIVER_OSPI_BUS_SPEED                                    10000000 //RTE_MX66UM2G_FLASH_OSPI_BUS_SPEED
#define DRIVER_OSPI_BUS_SPEED                                    5000000 //RTE_MX66UM2G_FLASH_OSPI_BUS_SPEED
#endif

#define CMD_SPI_WREN                                            (0x06U)
#define CMD_SPI_WRCR2                                           (0x72U)

/* OPI Data Flash Commands */
#define CMD_8READ                                               (0xEC13U) // 8 I/O read
#define CMD_8DTRD                                               (0xEE11U) // 8 I/O DT read
#define CMD_WREN                                                (0x06F9U) // write enable
#define CMD_WRDI                                                (0x04FBU) // write disable
#define CMD_PP4B                                                (0x12EDU) // page program
#define CMD_SE4B                                                (0x21DEU) // sector erase 4KB
#define CMD_BE4B                                                (0xDC23U) // block erase 64KB
#define CMD_CE                                                  (0x609FU) // chip erase
#define CMD_RDSR                                                (0x05FAU) // read status register
#define CMD_WRCR2                                               (0x728DU) // write configuration register 2
#define CMD_RDID                                                (0x9F60U) // read identification
#define CMD_RDSCUR                                              (0x2BD4U) // read security register
#define CMD_SBL                                                 (0xC03FU) // set burst length

#define IO_MODE_ADDRESS                                         0x00000000U
#define IO_MODE_STR_OPI                                         (0x01U)
#define IO_MODE_DTR_OPI                                         (0x02U)
#define DUMMY_CYCLE_ADDRESS                                     0x00000300U
#define DUMMY_CYCLE_20                                          (0x00U)
#define DUMMY_CYCLE_18                                          (0x01U)
#define DUMMY_CYCLE_16                                          (0x02U)
#define DUMMY_CYCLE_14                                          (0x03U)
#define DUMMY_CYCLE_12                                          (0x04U)
#define DUMMY_CYCLE_10                                          (0x05U)
#define DUMMY_CYCLE_8                                           (0x06U)
#define DUMMY_CYCLE_6                                           (0x07U)
#define DUMMY_CYCLE(N)                                          ((20-(N))/2)

#define BURST_LINEAR                                            0x10
#define BURST_WRAP16                                            0x01
#define BURST_WRAP32                                            0x02
#define BURST_WRAP64                                            0x03

#define DEFAULT_WAIT_CYCLES                                     6

#define FLAG_STATUS_WIP                                         0x01U
#define FLAG_STATUS_WEL                                         0x02U

#define SECREG_E_FAIL                                           0x40U
#define SECREG_P_FAIL                                           0x20U

/* Flash Driver Flash_Flags */
#define FLASH_INIT                                              (0x01U)
#define FLASH_POWER                                             (0x02U)

#define WAIT_TIMEOUT                                            128

/* SPI Driver */
extern ARM_DRIVER_OSPI ARM_Driver_OSPI_(DRIVER_OSPI_NUM);
static ARM_DRIVER_OSPI * ptrOSPI = &ARM_Driver_OSPI_(DRIVER_OSPI_NUM);

/* SPI Bus Speed */
#define OSPI_BUS_SPEED                                           ((uint32_t)DRIVER_OSPI_BUS_SPEED)
#define OSPI_MAX_RX_COUNT                                        0xFFFFFFFF

static uint32_t control_flags = ARM_OSPI_SS_MASTER_HW_OUTPUT | ARM_OSPI_CPOL0_CPHA0 | ARM_OSPI_MSB_LSB;

/* Flash Information */
ARM_FLASH_INFO MX66UM2G_FlashInfo = {
    NULL,
    FLASH_MX66UM2G_SECTOR_COUNT,
    FLASH_MX66UM2G_SECTOR_SIZE,
    FLASH_MX66UM2G_PAGE_SIZE,
    FLASH_MX66UM2G_PROGRAM_UNIT,
    FLASH_MX66UM2G_ERASED_VALUE,
#if (ARM_FLASH_API_VERSION > 0x201U)
    { 0U, 0U, 0U }
#endif
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus;

/* Flag to track the driver state */
static uint8_t Flash_Flags;

/* Flag to monitor OSPI events */
static volatile uint32_t event_flag;

/* Driver Version */
const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

/* Driver Capabilities */
const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    0U,                                 /* event_ready */
    1U,                                 /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1U,                                 /* erase_chip */
#if (ARM_FLASH_API_VERSION > 0x200U)
    0U                                  /* reserved */
#endif
};

/**
  \fn          void spi_callback_event(uint32_t event)
  \brief       Call back api called from OSPI.
  \return      none
**/
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

    uint32_t flag = event_flag;
    event_flag = 0;
    if (flag & ARM_OSPI_EVENT_TRANSFER_COMPLETE)
    {
        return ARM_DRIVER_OK;
    }
    else if (flag == 0)
    {
        return ARM_DRIVER_ERROR_TIMEOUT;
    }
    else
    {
        return ARM_DRIVER_ERROR;
    }
}
/**
  \fn          ARM_DRIVER_VERSION ARM_Flash_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
**/
static ARM_DRIVER_VERSION ARM_Flash_GetVersion (void)
{
    return DriverVersion;
}

/**
  \fn          ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_FLASH_CAPABILITIES
*/
static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities (void)
{
    return DriverCapabilities;
}

/**
  \fn          int32_t Read(uint32_t command, uint32_t addr, uint16_t *data, size_t len)
  \brief       Read register to check the status of flash device.
  \param[in]   command : Status register/ Flag Status Register
  \return      \ref execution_status
**/
#if DDR_OPTION == ARM_OSPI_DDR_DISABLE
static int32_t Read(uint16_t command, uint32_t addr, uint8_t *data, size_t len)
#else
static int32_t Read(uint16_t command, uint32_t addr, uint16_t *data, size_t len)
#endif
{
    int32_t status = ARM_DRIVER_OK;
    uint32_t cmd[2];

    status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                    ARM_OSPI_DATA_BITS(DDR_FRAME_SIZE) |
                    control_flags,
                    OSPI_BUS_SPEED);

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Set command */
    cmd[0] = command;
    cmd[1] = addr;

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (4 << ARM_OSPI_WAIT_CYCLE_POS));
    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Send command and receive register value */
    status = ptrOSPI->Transfer(&cmd, data, len);
    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return wait_for_completion();
}

/**
  \fn          int32_t ReadReg(uint32_t command, uint32_t addr, uint8_t *value)
  \brief       Read register to check the status of flash device.
  \param[in]   command : Status register/ Flag Status Register
  \return      \ref execution_status
**/
static int32_t ReadReg(uint16_t command, uint32_t addr, uint8_t *value)
{
    int32_t status = ARM_DRIVER_OK;
#if DDR_OPTION == ARM_OSPI_DDR_DISABLE
    uint8_t data;
#else
    uint16_t data;
#endif

    status = Read(command, addr, &data, 1);
    if (status != ARM_DRIVER_OK)
    {
        return status;
    }

#if DDR_OPTION == ARM_OSPI_DDR_DISABLE
    *value = data;
#else
    *value = data >> 8;
#endif

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ReadReg(uint8_t *value)
  \brief       Read register to check the status of flash device.
  \param[in]   command : Status register/ Flag Status Register
  \return      \ref execution_status
**/
static int32_t ReadStatusReg(uint8_t *value)
{
    return ReadReg(CMD_RDSR, 0, value);
}

static int32_t ReadSecurityReg(uint8_t *value)
{
    return ReadReg(CMD_RDSCUR, 0, value);
}

static int32_t set_burst_length(uint8_t sbl)
{
    uint32_t cmd[3];
    int32_t status;

    /* Prepare buffer with command and address to set burst length */
    cmd[0] = CMD_SBL;
    cmd[1] = 0;
    cmd[2] = (sbl << 8) | sbl;

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));
    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = ptrOSPI->Send(cmd, 3);

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = wait_for_completion();

    return status;
}

static int32_t AwaitResult(ARM_FLASH_STATUS *flash_status, uint8_t err_flags)
{
    int32_t status;
    uint8_t val;

    do
    {
        status = ReadStatusReg(&val);
        if (status != ARM_DRIVER_OK)
        {
            flash_status->error = 1;
            return status;
        }

        /* Check Flash_Flags Status register value */
        if ((val & (FLAG_STATUS_WIP | FLAG_STATUS_WEL)) == 0U)
        {
            flash_status->busy = 0U;
        }

    }while (flash_status->busy);

    status = ReadSecurityReg(&val);
    if (status != ARM_DRIVER_OK)
    {
        flash_status->error = 1;
        return status;
    }
    if (val & err_flags)
    {
        flash_status->error = 1;
    }
    return ARM_DRIVER_OK;
}

static int32_t AwaitProgramResult(ARM_FLASH_STATUS *status)
{
    return AwaitResult(status, SECREG_P_FAIL);
}

static int32_t AwaitEraseResult(ARM_FLASH_STATUS *status)
{
    return AwaitResult(status, SECREG_E_FAIL);
}

/**
  \fn          int32_t SetWriteEnable_SPI (void)
  \brief       Set write enable before attempting to configure OPI
  \param[in]   none
  \return      \ref execution_status
**/
static int32_t SetWriteEnable_SPI (void)
{
    int32_t status;
    uint32_t cmd;

    status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
             ARM_OSPI_DATA_BITS(8) |
             control_flags,
             OSPI_BUS_SPEED);

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    event_flag = 0;

    cmd = CMD_SPI_WREN;

    status = ptrOSPI->Send(&cmd, 1U);
    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    return wait_for_completion();
}


/**
  \fn          int32_t SetWriteEnable (void)
  \brief       Set write enable before writing to flash.
  \param[in]   none
  \return      \ref execution_status
**/
static int32_t SetWriteEnable (void)
{
    int32_t status;
    uint32_t cmd;
    uint8_t val;

    status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
             ARM_OSPI_DATA_BITS(DDR_FRAME_SIZE) |
             control_flags,
             OSPI_BUS_SPEED);

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    event_flag = 0;

    cmd = CMD_WREN;

    status = ptrOSPI->Send(&cmd, 1U);
    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = wait_for_completion();
    if (status != ARM_DRIVER_OK)
    {
        return status;
    }

    /* Reading status register will work only when FLASH is switched to Octal mode (after PowerControl(ARM_POWER_FULL)) */
    if (Flash_Flags & FLASH_POWER)
    {
        status = ReadStatusReg(&val);

        if((status == ARM_DRIVER_OK) && ((val & FLAG_STATUS_WEL) == 0))
        {
            return ARM_DRIVER_ERROR;
        }
    }
    return status;
}


/**
  \fn          int32_t ARM_Flash_Initialize (ARM_Flash_SignalEvent_t cb_event)
  \brief       Initialize the Flash Interface.
  \param[in]   cb_event  Pointer to \ref ARM_Flash_SignalEvent
  \return      \ref execution_status
**/
 static int32_t ARM_Flash_Initialize (ARM_Flash_SignalEvent_t cb_event)
 {
    int32_t status;
    (void)cb_event;

    FlashStatus.busy  = 0U;
    FlashStatus.error = 0U;

    status = ptrOSPI->Initialize(spi_callback_event);

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    Flash_Flags |= FLASH_INIT;

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_Flash_Uninitialize (void)
  \brief       De-initialize the Flash Interface.
  \return      \ref execution_status
**/
static int32_t ARM_Flash_Uninitialize (void)
{
    Flash_Flags = 0U;
    return ptrOSPI->Uninitialize();
}

/**
  \fn          int32_t ARM_Flash_PowerControl (ARM_POWER_STATE state)
  \brief       Control the Flash interface power.
  \param[in]   state  Power state
  \return      \ref execution_status
**/

static int32_t ARM_Flash_PowerControl (ARM_POWER_STATE state)
{
    int32_t status;
    uint32_t cmd[3];

    switch (state)
    {
        case ARM_POWER_OFF:
        {
            Flash_Flags &= ~FLASH_POWER;
            FlashStatus.busy  = 0U;
            FlashStatus.error = 0U;

            return ptrOSPI->PowerControl(ARM_POWER_OFF);
        }

        case ARM_POWER_FULL:
        {

            if ((Flash_Flags & FLASH_INIT) == 0U)
            {
                return ARM_DRIVER_ERROR;
            }

            if ((Flash_Flags & FLASH_POWER) == 0U)
            {
                status = ptrOSPI->PowerControl(ARM_POWER_FULL);

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_CONTROL_SS | control_flags, ARM_OSPI_SS_ACTIVE);
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                /* Ensure it is in OPI mode by issuing SPI mode change */
                status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                                 ARM_OSPI_DATA_BITS(8) |
                                 control_flags,
                                 OSPI_BUS_SPEED);

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_INST_LENGTH | control_flags, ARM_OSPI_INST_LENGTH_8_BITS);
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_FRAME_FORMAT | control_flags, ARM_OSPI_FRF_STANDRAD);

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_DDR_MODE | control_flags, ARM_OSPI_DDR_DISABLE);

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = SetWriteEnable_SPI();

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                /* Prepare command and address for setting flash in octal mode */
                cmd[0] = CMD_SPI_WRCR2;
                cmd[1] = IO_MODE_ADDRESS;
                cmd[2] = DDR_OPTION == ARM_OSPI_DDR_DISABLE ? IO_MODE_STR_OPI : IO_MODE_DTR_OPI;

                status = ptrOSPI->Send(cmd, 3);
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = wait_for_completion();
                if (status != ARM_DRIVER_OK)
                {
                    return status;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_INST_LENGTH | control_flags, ARM_OSPI_INST_LENGTH_16_BITS);
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_FRAME_FORMAT | control_flags, ARM_OSPI_FRF_OCTAL);

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_DDR_MODE | control_flags, DDR_OPTION);

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = SetWriteEnable();

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                                 ARM_OSPI_DATA_BITS(DDR_FRAME_SIZE) |
                                 control_flags,
                                 OSPI_BUS_SPEED);
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                /* Prepare buffer with command and address to configure default wait cycles */
                cmd[0] = CMD_WRCR2;
                cmd[1] = DUMMY_CYCLE_ADDRESS;
                cmd[2] = (DUMMY_CYCLE(DEFAULT_WAIT_CYCLES) << 8) | DUMMY_CYCLE(DEFAULT_WAIT_CYCLES);

                status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Send(cmd, 3);

                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

                status = wait_for_completion();
                if (status != ARM_DRIVER_OK)
                {
                    return status;
                }

                status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                                 ARM_OSPI_DATA_BITS(DDR_FRAME_SIZE) |
                                 control_flags,
                                 OSPI_BUS_SPEED);
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }

#if DDR_OPTION == ARM_OSPI_DDR_DISABLE
                uint8_t id[3];
                status = Read(CMD_RDID, 0, id, 3);
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }
                printf("ID = %" PRIx8 ",%" PRIx8 ",%" PRIx8 "\n", id[0], id[1], id[2]);
#else
                uint16_t id[3];
                status = Read(CMD_RDID, 0, id, 3);
                if (status != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }
                printf("ID = %" PRIx16 ",%" PRIx16 ",%" PRIx16 "\n", id[0], id[1], id[2]);
#endif

                FlashStatus.busy  = 0U;
                FlashStatus.error = 0U;

                Flash_Flags |= FLASH_POWER;
            }
            return ARM_DRIVER_OK;
        }

        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;

        default:
            return ARM_DRIVER_ERROR;
    }
}

/**
  \fn          int32_t ARM_Flash_ReadData (uint32_t addr, void *data, uint32_t cnt)
  \brief       Read data from Flash.
  \param[in]   addr  Data address.
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   cnt   Number of data items to read.
  \return      number of data items read or \ref execution_status
**/
static int32_t ARM_Flash_ReadData (uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t cmd[2], data_cnt;
    uint16_t *data_ptr;
    int32_t  status, num = 0;

    if ((addr > (FLASH_MX66UM2G_SECTOR_COUNT * FLASH_MX66UM2G_SECTOR_SIZE)) || (data == NULL) || ((addr + cnt) > (FLASH_MX66UM2G_SECTOR_COUNT * FLASH_MX66UM2G_SECTOR_SIZE)))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    data_ptr = (uint16_t *) data;

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = SetWriteEnable();

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (DEFAULT_WAIT_CYCLES << ARM_OSPI_WAIT_CYCLE_POS));

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Switch to 16 bit mode for reading data */
    status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                ARM_OSPI_DATA_BITS(DDR_FRAME_SIZE) |
                control_flags,
                OSPI_BUS_SPEED);

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    while(cnt)
    {
        /* At frequency > 2.5 MHz, max no. of frames that can be read by OSPI is 256 (RX_FIFO_DEPTH) */
        data_cnt = OSPI_MAX_RX_COUNT;

        if (data_cnt > cnt)
        {
            data_cnt = cnt;
        }

        /* Prepare command with address */
        cmd[0] = DDR_OPTION == ARM_OSPI_DDR_DISABLE ? CMD_8READ : CMD_8DTRD;
        cmd[1] = addr;

        status = ptrOSPI->Transfer(cmd, data_ptr, DDR_OPTION == ARM_OSPI_DDR_DISABLE ? data_cnt * 2 : data_cnt);

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK)
        {
            return status;
        }

        /* For 16 bit frames, update address by data_cnt * 2*/
        addr += (data_cnt * 2);
        cnt -= data_cnt;
        data_ptr += data_cnt;
        num += data_cnt;
    }

    status = num;
    return status;
}

/* temporary buffer used in ProgramData below */
#if DDR_OPTION == ARM_OSPI_DDR_DISABLE
static uint32_t cmd[1 + 1 + FLASH_MX66UM2G_PAGE_SIZE];
#else
static uint32_t cmd[1 + 1 + FLASH_MX66UM2G_PAGE_SIZE / 2];
#endif

/**
  \fn          int32_t ARM_Flash_ProgramData (uint32_t addr, const void *data, uint32_t cnt)
  \brief       Program data to Flash.
  \param[in]   addr  Data address.
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash.
  \param[in]   cnt   Number of data items to program.
  \return      number of data items programmed or \ref execution_status
**/
static int32_t ARM_Flash_ProgramData (uint32_t addr, const void *data, uint32_t cnt)
{
    const uint16_t *data_ptr;
    int32_t status = ARM_DRIVER_OK;
    uint32_t num = 0, data_cnt, index, iter = 0, it = 0;
    bool error_reported = false;

    if ((addr > (FLASH_MX66UM2G_SECTOR_COUNT * FLASH_MX66UM2G_SECTOR_SIZE)) || (data == NULL) || ((addr + cnt) > (FLASH_MX66UM2G_SECTOR_COUNT * FLASH_MX66UM2G_SECTOR_SIZE)))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    data_ptr = data;

    while (cnt)
    {
        FlashStatus.busy  = 1U;
        FlashStatus.error = 0U;

        status = SetWriteEnable();

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        data_cnt = (FLASH_MX66UM2G_PAGE_SIZE - (addr % FLASH_MX66UM2G_PAGE_SIZE)) >> 1;

        if (data_cnt > cnt)
        {
            data_cnt = cnt;
        }

        /* Prepare command with address */
        cmd[0] = CMD_PP4B;
        cmd[1] = addr;

        index = 2;

        for ( it = 0; it < data_cnt; it++)
        {
#if DDR_OPTION == ARM_OSPI_DDR_DISABLE
            // We are using 16-bit DFS cos of uninvestigated FIFO issue (driver barfs with a 258-frame write).
            // But we'll use 8-bit DFS to read. Reverse to compensate.
            // Note that if we switch to DDR mode, we and RAM will both switch to 16-bit DFS, and we'll still get the right thing.
            cmd[index++] = ((data_ptr[iter] & 0xFF) << 8) | ((data_ptr[iter] & 0xFF00) >> 8);
            iter++;
#else
            cmd[index++] = data_ptr[iter++];
#endif
        }

        status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        /* Switch to 16 bit mode for program data */
        status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                 ARM_OSPI_DATA_BITS(16) |
                 control_flags,
                 OSPI_BUS_SPEED);

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        do {
            /* Send data along with the command and address bytes (+2) */
            status = ptrOSPI->Send(cmd, index);
        } while (status == ARM_DRIVER_ERROR_BUSY);

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK)
        {
            return status;
        }

        /* Read status until device ready */
        status = AwaitProgramResult(&FlashStatus);
        if (status != ARM_DRIVER_OK)
        {
            return status;
        }
        if (FlashStatus.error)
        {
            break;
        }

#if 1
        status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                 ARM_OSPI_DATA_BITS(DDR_FRAME_SIZE) |
                 control_flags,
                 OSPI_BUS_SPEED);
        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (DEFAULT_WAIT_CYCLES << ARM_OSPI_WAIT_CYCLE_POS));
        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        /* Read back check */
        cmd[0] = DDR_OPTION == ARM_OSPI_DDR_DISABLE ? CMD_8READ : CMD_8DTRD;
        cmd[1] = addr;
        status = ptrOSPI->Transfer(cmd, cmd + 2, DDR_OPTION == ARM_OSPI_DDR_DISABLE ? data_cnt * 2 : data_cnt);
        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK)
        {
            return status;
        }

        if (memcmp(cmd + 2, data_ptr + iter - data_cnt, data_cnt * 2))
        {
            if (!error_reported) {
                printf("verify error at %" PRIX32 "\n", addr);
                char * p = (char *) (cmd + 2), *q = (char *) (data_ptr + iter - data_cnt);
                for (int i = 0; i < data_cnt * 2; i++) {
                    if (p[i] != q[i]) {
                        printf("to be precise, at %" PRIX32 "\n", addr + i);
                        break;
                    }
                }
                //error_reported = true;
            }
            FlashStatus.error = 1U;
            break;
        }
#endif

        /* For 16 bit data frames, increment the byte address with 2 * data_cnt programmed */
        addr += (data_cnt * 2);
        num  += data_cnt;
        cnt  -= data_cnt;
    }

    /* Number of data items programmed */
    status = (int32_t)num;
    return status;
}

/**
  \fn          int32_t ARM_Flash_EraseSector (uint32_t addr)
  \brief       Erase Flash Sector.
  \param[in]   addr  Sector address
  \return      \ref execution_status
**/
static int32_t ARM_Flash_EraseSector (uint32_t addr)
{
    int32_t status;
    uint32_t cmd[2];

    if (addr > (FLASH_MX66UM2G_SECTOR_COUNT * FLASH_MX66UM2G_SECTOR_SIZE))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = SetWriteEnable();

    if (status == ARM_DRIVER_OK)
    {
        FlashStatus.busy  = 1U;
        FlashStatus.error = 0U;

        status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                    ARM_OSPI_DATA_BITS(8) |
                    control_flags,
                    OSPI_BUS_SPEED);

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        /* Prepare command with address */
        cmd[0] = FLASH_MX66UM2G_DO_BLOCK_ERASE ? CMD_BE4B : CMD_SE4B;
        cmd[1] = addr;
        status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        status = ptrOSPI->Send(cmd, 2U);

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK)
        {
            return status;
        }

        status = AwaitEraseResult(&FlashStatus);
    }
    return status;
}


 /**
  \fn          int32_t ARM_Flash_EraseChip (void)
  \brief       Erase complete Flash.
               Optional function for faster full chip erase.
  \return      \ref execution_status
**/
static int32_t ARM_Flash_EraseChip (void)
{
    uint32_t cmd;
    int32_t status;

    FlashStatus.busy  = 1U;
    FlashStatus.error = 0U;

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

    if (status != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    status = SetWriteEnable();

    if (status == ARM_DRIVER_OK)
    {

        status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                    ARM_OSPI_DATA_BITS(8) |
                    control_flags,
                    OSPI_BUS_SPEED);

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }


        /* Prepare command */
        cmd = CMD_CE;

        status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags, (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) | (0 << ARM_OSPI_WAIT_CYCLE_POS));

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        status = ptrOSPI->Send(&cmd, 1U);

        if (status != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK)
        {
            return status;
        }

        status = AwaitEraseResult(&FlashStatus);
    }
    return status;
}

/**
  \fn          ARM_FLASH_STATUS ARM_Flash_GetStatus (void)
  \brief       Get Flash status.
  \return      Flash status \ref ARM_FLASH_STATUS
**/
static ARM_FLASH_STATUS ARM_Flash_GetStatus (void)
{
    uint8_t val;

    if (FlashStatus.busy == 1U)
    {
        /* Read flag status register */
        if (ReadStatusReg(&val) == ARM_DRIVER_OK)
        {
            /* Check "Program or erase controller" bit */
            if ((val & (FLAG_STATUS_WIP | FLAG_STATUS_WEL)) == 0U)
            {
                FlashStatus.busy = 0U;
            }

            FlashStatus.error = 0U;
        }
        else
        {
            FlashStatus.error = 1U;
        }
    }
    return FlashStatus;
}

/**
  \fn          ARM_FLASH_INFO * ARM_Flash_GetInfo (void)
  \brief       Get Flash information.
  \return      Pointer to Flash information \ref ARM_FLASH_INFO
**/
static ARM_FLASH_INFO * ARM_Flash_GetInfo (void)
{
    return &MX66UM2G_FlashInfo;
}


static ARM_FLASH_INFO * GetInfo (void)
{
    return ARM_Flash_GetInfo();
}

static ARM_DRIVER_VERSION GetVersion(void)
{
    return ARM_Flash_GetVersion();
}

static ARM_FLASH_CAPABILITIES GetCapabilities(void)
{
    return ARM_Flash_GetCapabilities();
}

static ARM_FLASH_STATUS GetStatus (void)
{
    return ARM_Flash_GetStatus();
}

static int32_t EraseChip (void)
{
    return ARM_Flash_EraseChip();
}

static int32_t EraseSector (uint32_t addr)
{
    return ARM_Flash_EraseSector (addr);
}

static int32_t ProgramData (uint32_t addr, const void *data, uint32_t cnt)
{
    return ARM_Flash_ProgramData (addr, data, cnt);
}

static int32_t ReadData (uint32_t addr, void *data, uint32_t cnt)
{
    return ARM_Flash_ReadData (addr, data, cnt);
}

static int32_t PowerControl (ARM_POWER_STATE state)
{
    return ARM_Flash_PowerControl (state);
}

static int32_t Initialize (ARM_Flash_SignalEvent_t cb_event)
{
    return ARM_Flash_Initialize (cb_event);
}

static int32_t Uninitialize (void)
{
    return ARM_Flash_Uninitialize ();
}

/* Flash Driver Control Block */
extern
ARM_DRIVER_FLASH ARM_Driver_Flash_(DRIVER_FLASH_NUM);
ARM_DRIVER_FLASH ARM_Driver_Flash_(DRIVER_FLASH_NUM) = {
    GetVersion,
    GetCapabilities,
    Initialize,
    Uninitialize,
    PowerControl,
    ReadData,
    ProgramData,
    EraseSector,
    EraseChip,
    GetStatus,
    GetInfo
};

int32_t MX66UM2G_set_linear(void)
{
    return set_burst_length(BURST_LINEAR);
}

int32_t MX66UM2G_set_wrap32(void)
{
    return set_burst_length(BURST_WRAP32);
}
