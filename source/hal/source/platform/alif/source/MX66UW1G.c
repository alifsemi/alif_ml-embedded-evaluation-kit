/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 * @file     MX66UW1G.c
 * @author   Manoj A Murudi
 * @email    manoj.murudi@alifsemi.com
 * @version  V1.0.0
 * @date     10-Dec-2025
 * @brief    Source file for MX66UW1G flash.
 ******************************************************************************/
#include "Driver_Flash.h"
#include "Driver_OSPI.h"
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "MX66UW1G.h"

#include <string.h>

#if defined(RTE_Drivers_MX66UW1G_FLASH)

#if !(RTE_MX66UW1G_FLASH)
#error "MX66UW1G Flash driver is not enabled in RTE_Device.h"
#endif

#define ARM_FLASH_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

#ifndef DRIVER_FLASH_NUM
#define DRIVER_FLASH_NUM                                        2      /* Default driver number */
#endif

#ifndef DRIVER_OSPI_NUM
#define DRIVER_OSPI_NUM                                         RTE_MX66UW1G_FLASH_OSPI_DRV_NUM
#endif

#ifndef DRIVER_OSPI_BUS_SPEED
#define DRIVER_OSPI_BUS_SPEED                                   RTE_MX66UW1G_FLASH_OSPI_BUS_SPEED
#endif

/* SPI Data Flash Commands */
#define CMD_SPI_WREN                                            0x06U
#define CMD_SPI_WRCR2                                           0x72U
#define CMD_SPI_RDID                                            0x9FU

/* OPI Data Flash Commands */
#define CMD_8READ                                               0xEC13U
#define CMD_8DTRD                                               0xEE11U
#define CMD_WREN                                                0x06F9U
#define CMD_WRDI                                                0x04FBU
#define CMD_PP4B                                                0x12EDU
#define CMD_SE4B                                                0x21DEU
#define CMD_BE4B                                                0xDC23U
#define CMD_CE                                                  0x609FU
#define CMD_RDSR                                                0x05FAU
#define CMD_WRCR2                                               0x728DU
#define CMD_RDID                                                0x9F60U
#define CMD_RDSCUR                                              0x2BD4U
#define CMD_SBL                                                 0xC03FU
#define CMD_RDCR2                                               0x718EU

#define IO_MODE_ADDRESS                                         0x00000000U
#define IO_MODE_STR_OPI                                         0x01U
#define IO_MODE_DTR_OPI                                         0x02U
#define DOS_ADDRESS                                             0x00000200U
#define DOS_ENABLE                                              0x02U
#define DUMMY_CYCLE_ADDRESS                                     0x00000300U
#define DUMMY_CYCLE_20                                          0x00U
#define DUMMY_CYCLE_18                                          0x01U
#define DUMMY_CYCLE_16                                          0x02U
#define DUMMY_CYCLE_14                                          0x03U
#define DUMMY_CYCLE_12                                          0x04U
#define DUMMY_CYCLE_10                                          0x05U
#define DUMMY_CYCLE_8                                           0x06U
#define DUMMY_CYCLE_6                                           0x07U
#define DUMMY_CYCLE(N)                                          ((20-(N))/2)

#define BURST_LINEAR                                            0x10U
#define BURST_WRAP16                                            0x01U
#define BURST_WRAP32                                            0x02U
#define BURST_WRAP64                                            0x03U

#define DEFAULT_WAIT_CYCLES                                     RTE_MX66UW1G_FLASH_WAIT_CYCLES

#define FLAG_STATUS_WIP                                         0x01U
#define FLAG_STATUS_WEL                                         0x02U

#define SECREG_E_FAIL                                           0x40U
#define SECREG_P_FAIL                                           0x20U

/* Flash Driver Flash_Flags */
#define FLASH_INIT                                              0x01U
#define FLASH_POWER                                             0x02U

#define WAIT_TIMEOUT                                            128

#define MX66UW1G_ID                                             0xC2813BUL

/* SPI Driver */
extern ARM_DRIVER_OSPI ARM_Driver_OSPI_(DRIVER_OSPI_NUM);
static ARM_DRIVER_OSPI *ptrOSPI = &ARM_Driver_OSPI_(DRIVER_OSPI_NUM);

/* SPI Bus Speed */
#define OSPI_BUS_SPEED                                          ((uint32_t)DRIVER_OSPI_BUS_SPEED)
#define OSPI_MAX_RX_COUNT                                       256

static uint32_t control_flags = (ARM_OSPI_SS_MASTER_HW_OUTPUT | ARM_OSPI_CPOL0_CPHA0 |
                                ARM_OSPI_MSB_LSB);

/* Flash Information */
ARM_FLASH_INFO MX66UW1G_FlashInfo = {NULL,
                                     FLASH_MX66UW1G_SECTOR_COUNT,
                                     FLASH_MX66UW1G_SECTOR_SIZE,
                                     FLASH_MX66UW1G_PAGE_SIZE,
                                     FLASH_MX66UW1G_PROGRAM_UNIT,
                                     FLASH_MX66UW1G_ERASED_VALUE,
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
const ARM_DRIVER_VERSION DriverVersion          = {ARM_FLASH_API_VERSION, ARM_FLASH_DRV_VERSION};


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
 * @fn         void spi_callback_event(uint32_t event)
 * @brief      Callback API called from OSPI.
 * @param[in]  event   Callback event.
 * @return     none
 */
static void spi_callback_event(uint32_t event)
{
    event_flag = event;
}

/**
 * @fn      int32_t wait_for_completion(void)
 * @brief   OSPI wait until IRQ operation completes.
 * @return  return status
 */
static int32_t wait_for_completion(void)
{
    uint32_t flag;

    while (!event_flag) {
        __WFE();
    }

    flag = event_flag;
    event_flag = 0;
    if (flag & ARM_OSPI_EVENT_TRANSFER_COMPLETE) {
        return ARM_DRIVER_OK;
    } else {
        return ARM_DRIVER_ERROR;
    }
}

/**
 * @fn      ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
 * @brief   Get driver version.
 * @return  ARM_DRIVER_VERSION
 */
static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

/**
 * @fn      ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
 * @brief   Get driver capabilities.
 * @return  ARM_FLASH_CAPABILITIES
 */
static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
 * @fn         int32_t MX66UW1G_Read(uint32_t addr, uint8_t *buffer, uint32_t length)
 * @brief      Read data from the flash device.
 * @param[in]  addr     Start address to read from
 * @param[in]  buffer   Pointer to the buffer where read data will be stored
 * @param[in]  length   Number of bytes to read
 * @return     execution_status
 */

static int32_t MX66UW1G_Read(uint16_t command, uint32_t addr, uint16_t *data, size_t len)
{
    int32_t status = ARM_DRIVER_OK;
    uint32_t cmd[2];

    status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                    ARM_OSPI_DATA_BITS(OSPI_DFS_16_BIT) |
                    control_flags,
                    OSPI_BUS_SPEED);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    /* Set command */
    cmd[0] = command;
    cmd[1] = addr;

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                             (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                             (4 << ARM_OSPI_WAIT_CYCLE_POS));
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    /* Send command and receive register value */
    status = ptrOSPI->Transfer(&cmd, data, len);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    return wait_for_completion();
}

/**
 * @fn         int32_t MX66UW1G_ReadReg(uint32_t command, uint32_t addr, uint8_t *value)
 * @brief      Read register of flash device.
 * @param[in]  command   Status register / Flag Status Register
 * @param[in]  addr      Register address
 * @param[in]  value     Pointer to store the read value
 * @return     execution_status
 */
static int32_t MX66UW1G_ReadReg(uint16_t command, uint32_t addr, uint8_t *value)
{
    int32_t status = ARM_DRIVER_OK;
    uint16_t data;

    status = MX66UW1G_Read(command, addr, &data, 1);
    if (status != ARM_DRIVER_OK) {
        return status;
    }

    *value = data >> 8;

    return ARM_DRIVER_OK;
}

/**
 * @fn         int32_t MX66UW1G_ReadStatusReg(uint8_t *value)
 * @brief      Read register to check the status of flash device.
 * @param[in]  value     Pointer to store the read value
 * @return     execution_status
 */
static int32_t MX66UW1G_ReadStatusReg(uint8_t *value)
{
    return MX66UW1G_ReadReg(CMD_RDSR, 0, value);
}

/**
 * @fn         int32_t MX66UW1G_ReadIDReg(uint8_t *value)
 * @brief      Read ID register of flash device.
 * @param[in]  value     Pointer to store the read value
 * @return     execution_status
 */
static int32_t MX66UW1G_ReadIDReg(uint32_t *value)
{
    uint16_t id[3];
    int32_t status = ARM_DRIVER_OK;

    status = MX66UW1G_Read(CMD_RDID, 0, id, 3);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    *value = (((id[0] & 0xFF) << 16) | ((id[1] & 0xFF) << 8) | (id[2] & 0xFF));

    return status;
}

/**
 * @fn         int32_t MX66UW1G_ReadSecurityReg(uint8_t *value)
 * @brief      Read security register of flash device.
 * @param[in]  value     Pointer to store the read value
 * @return     execution_status
 */
static int32_t MX66UW1G_ReadSecurityReg(uint8_t *value)
{
    return MX66UW1G_ReadReg(CMD_RDSCUR, 0, value);
}

/**
 * @fn         int32_t MX66UW1G_AwaitResult(ARM_FLASH_STATUS *flash_status, uint8_t err_flags)
 * @brief      waiting for error flag to be cleared.
 * @param[in]  flash_status     device status
 * @param[in]  err_flags        error flags to be checked
 * @return     execution_status
 */
static int32_t MX66UW1G_AwaitResult(ARM_FLASH_STATUS *flash_status, uint8_t err_flags)
{
    int32_t status;
    uint8_t val;

    do {
        status = MX66UW1G_ReadStatusReg(&val);
        if (status != ARM_DRIVER_OK) {
            flash_status->error = 1;
            return status;
        }

        /* Check Flash_Flags Status register value */
        if ((val & (FLAG_STATUS_WIP | FLAG_STATUS_WEL)) == 0U) {
            flash_status->busy = 0U;
        }

    } while (flash_status->busy);

    status = MX66UW1G_ReadSecurityReg(&val);
    if (status != ARM_DRIVER_OK) {
        flash_status->error = 1;
        return status;
    }

    if (val & err_flags) {
        flash_status->error = 1;
    }

    return ARM_DRIVER_OK;
}

/**
 * @fn         int32_t MX66UW1G_AwaitProgramResult(ARM_FLASH_STATUS *status)
 * @brief      wait till program opern completes.
 * @param[in]  status     device status
 * @return     execution_status
 */
static int32_t MX66UW1G_AwaitProgramResult(ARM_FLASH_STATUS *status)
{
    return MX66UW1G_AwaitResult(status, SECREG_P_FAIL);
}

/**
 * @fn         int32_t MX66UW1G_AwaitEraseResult(ARM_FLASH_STATUS *status)
 * @brief      wait till erase opern completes.
 * @param[in]  status     device status
 * @return     execution_status
 */
static int32_t MX66UW1G_AwaitEraseResult(ARM_FLASH_STATUS *status)
{
    return MX66UW1G_AwaitResult(status, SECREG_E_FAIL);
}

/**
 * @fn      int32_t MX66UW1G_SetWriteEnable_SPI(void)
 * @brief   Set write enable before attempting to configure OPI.
 * @return  execution_status
 */
static int32_t MX66UW1G_SetWriteEnable_SPI(void)
{
    int32_t status;
    uint32_t cmd;

    status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
             ARM_OSPI_DATA_BITS(8) |
             control_flags,
             OSPI_BUS_SPEED);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                             (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                             (0 << ARM_OSPI_WAIT_CYCLE_POS));
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    event_flag = 0;

    cmd = CMD_SPI_WREN;

    status = ptrOSPI->Send(&cmd, 1U);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    return wait_for_completion();
}


/**
 * @fn      int32_t MX66UW1G_SetWriteEnable(void)
 * @brief   Set write enable before writing to flash.
 * @return  execution_status
 */
static int32_t MX66UW1G_SetWriteEnable(void)
{
    int32_t status;
    uint32_t cmd;
    uint8_t val;

    status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
             ARM_OSPI_DATA_BITS(OSPI_DFS_16_BIT) |
             control_flags,
             OSPI_BUS_SPEED);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                             (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                             (0 << ARM_OSPI_WAIT_CYCLE_POS));
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    event_flag = 0;

    cmd = CMD_WREN;

    status = ptrOSPI->Send(&cmd, 1U);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    status = wait_for_completion();
    if (status != ARM_DRIVER_OK) {
        return status;
    }

    /* Reading status register will work only when FLASH is switched to Octal mode
     *  (after PowerControl(ARM_POWER_FULL))
     **/
    if (Flash_Flags & FLASH_POWER) {
        status = MX66UW1G_ReadStatusReg(&val);

        if ((status == ARM_DRIVER_OK) && ((val & FLAG_STATUS_WEL) == 0)) {
            return ARM_DRIVER_ERROR;
        }
    }
    return status;
}


/**
 * @fn          int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
 * @brief       Initialize the Flash Interface.
 * @param[in]   cb_event   Pointer to ARM_Flash_SignalEvent
 * @return      execution_status
 */
static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    int32_t status;
    (void)cb_event;

    FlashStatus.busy  = 0U;
    FlashStatus.error = 0U;

    status = ptrOSPI->Initialize(spi_callback_event);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    Flash_Flags |= FLASH_INIT;

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t ARM_Flash_Uninitialize(void)
 * @brief   De-initialize the Flash Interface.
 * @return  execution_status
 */
static int32_t ARM_Flash_Uninitialize(void)
{
    Flash_Flags = 0U;
    return ptrOSPI->Uninitialize();
}

/**
 * @fn         int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
 * @brief      Control the Flash interface power.
 * @param[in]  state   Power state.
 * @return     execution_status
 */
static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    int32_t status;
    uint32_t cmd[3], id_val = 0;

    switch (state) {
    case ARM_POWER_OFF:
        {
            Flash_Flags &= ~FLASH_POWER;
            FlashStatus.busy  = 0U;
            FlashStatus.error = 0U;

            return ptrOSPI->PowerControl(ARM_POWER_OFF);
        }

    case ARM_POWER_FULL:
        {
            if ((Flash_Flags & FLASH_INIT) == 0U) {
                return ARM_DRIVER_ERROR;
            }

            if ((Flash_Flags & FLASH_POWER) == 0U) {

                status = ptrOSPI->PowerControl(ARM_POWER_FULL);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_CONTROL_SS | control_flags, ARM_OSPI_SS_ACTIVE);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                /* Ensure it is in OPI mode by issuing SPI mode change */
                status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                                 ARM_OSPI_DATA_BITS(8) |
                                 control_flags,
                                 OSPI_BUS_SPEED);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_INST_LENGTH | control_flags,
                                          ARM_OSPI_INST_LENGTH_8_BITS);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_FRAME_FORMAT | control_flags,
                                          ARM_OSPI_FRF_STANDRAD);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_DDR_MODE | control_flags,
                                          ARM_OSPI_DDR_DISABLE);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = MX66UW1G_SetWriteEnable_SPI();
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                                        (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                                        (0 << ARM_OSPI_WAIT_CYCLE_POS));
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                /* Prepare command and address for setting flash in octal mode */
                cmd[0] = CMD_SPI_WRCR2;
                cmd[1] = IO_MODE_ADDRESS;
                cmd[2] = IO_MODE_DTR_OPI;

                status = ptrOSPI->Send(cmd, 3);
                if (status != ARM_DRIVER_OK)  {
                    return ARM_DRIVER_ERROR;
                }

                status = wait_for_completion();
                if (status != ARM_DRIVER_OK) {
                    return status;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_INST_LENGTH | control_flags,
                                          ARM_OSPI_INST_LENGTH_16_BITS);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_FRAME_FORMAT | control_flags,
                                          ARM_OSPI_FRF_OCTAL);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_SET_DDR_MODE | control_flags,
                                          ARM_OSPI_INST_DDR_ENABLE);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = MX66UW1G_SetWriteEnable();
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                                 ARM_OSPI_DATA_BITS(OSPI_DFS_16_BIT) |
                                 control_flags,
                                 OSPI_BUS_SPEED);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                /* Prepare buffer with command and address to configure default wait cycles */
                cmd[0] = CMD_WRCR2;
                cmd[1] = DUMMY_CYCLE_ADDRESS;
                cmd[2] = (DUMMY_CYCLE(DEFAULT_WAIT_CYCLES) << 8) | DUMMY_CYCLE(DEFAULT_WAIT_CYCLES);

                status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                                        (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                                        (0 << ARM_OSPI_WAIT_CYCLE_POS));
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = ptrOSPI->Send(cmd, 3);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = wait_for_completion();
                if (status != ARM_DRIVER_OK) {
                    return status;
                }

                status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                                 ARM_OSPI_DATA_BITS(OSPI_DFS_16_BIT) |
                                 control_flags,
                                 OSPI_BUS_SPEED);
                if (status != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR;
                }

                status = MX66UW1G_ReadIDReg(&id_val);
                if ((status != ARM_DRIVER_OK) || (id_val != MX66UW1G_ID)) {
                    return ARM_DRIVER_ERROR;
                }

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
 * @fn         int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
 * @brief      Read data from Flash.
 * @param[in]  addr   Data address.
 * @param[in]  data   Pointer to buffer storing the data read from Flash.
 * @param[in]  cnt    Number of data items to read.
 * @return     Number of data items read or execution_status
 */
static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t  cmd[2], data_cnt;
    uint16_t *data_ptr;
    int32_t   status, num = 0;

    if ((addr > (FLASH_MX66UW1G_SECTOR_COUNT * FLASH_MX66UW1G_SECTOR_SIZE)) || (data == NULL) ||
        ((addr + cnt) > (FLASH_MX66UW1G_SECTOR_COUNT * FLASH_MX66UW1G_SECTOR_SIZE))) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    data_ptr = (uint16_t *) data;

    status   = MX66UW1G_SetWriteEnable();
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE,
                             (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                             (DEFAULT_WAIT_CYCLES << ARM_OSPI_WAIT_CYCLE_POS));
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    /* Switch to 16 bit mode for reading data */
    status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER | ARM_OSPI_DATA_BITS(OSPI_DFS_16_BIT) |
                              ARM_OSPI_SS_MASTER_SW,
                              OSPI_BUS_SPEED);
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    while (cnt) {

        /* At frequency > 2.5 MHz, max no. of frames that can be read by OSPI is 256 (RX_FIFO_DEPTH)
         */
        data_cnt = OSPI_MAX_RX_COUNT;

        if (data_cnt > cnt) {
            data_cnt = cnt;
        }

        /* Prepare command with address */
        cmd[0] = CMD_8DTRD;
        cmd[1] = addr;

        status = ptrOSPI->Transfer(cmd, data_ptr, data_cnt);
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK) {
            return status;
        }

        /* For 16 bit frames, update address by data_cnt * 2*/
        addr     += (data_cnt * 2);
        cnt      -= data_cnt;
        data_ptr += data_cnt;
        num      += data_cnt;
    }

    status = num;
    return status;
}

/* temporary buffer used in ProgramData below */
static uint32_t cmd[1 + 1 + (FLASH_MX66UW1G_PAGE_SIZE / 2)];

/**
 * @fn         int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data, uint32_t cnt)
 * @brief      Program data to Flash.
 * @param[in]  addr   Data address.
 * @param[in]  data   Pointer to buffer containing the data to be programmed.
 * @param[in]  cnt    Number of data items to program.
 * @return     Number of data items programmed or execution_status
 */
static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data, uint32_t cnt)
{
    const uint16_t *data_ptr;
    int32_t status = ARM_DRIVER_OK;
    uint32_t num = 0, data_cnt, index, iter = 0, it = 0;

    if ((addr > (FLASH_MX66UW1G_SECTOR_COUNT * FLASH_MX66UW1G_SECTOR_SIZE)) ||
        (data == NULL) ||
        ((addr + cnt) > (FLASH_MX66UW1G_SECTOR_COUNT * FLASH_MX66UW1G_SECTOR_SIZE))) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    data_ptr = data;

    while (cnt) {
        FlashStatus.busy  = 1U;
        FlashStatus.error = 0U;

        status = MX66UW1G_SetWriteEnable();
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        data_cnt = (FLASH_MX66UW1G_PAGE_SIZE - (addr % FLASH_MX66UW1G_PAGE_SIZE)) >> 1;
        if (data_cnt > cnt) {
            data_cnt = cnt;
        }

        /* Prepare command with address */
        cmd[0] = CMD_PP4B;
        cmd[1] = addr;

        index = 2;

        for (it = 0; it < data_cnt; it++) {
            cmd[index++] = data_ptr[iter++];
        }

        status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                                 (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                                 (0 << ARM_OSPI_WAIT_CYCLE_POS));
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        /* Switch to 16 bit mode for program data */
        status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                 ARM_OSPI_DATA_BITS(16) |
                 control_flags,
                 OSPI_BUS_SPEED);
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        do {
            /* Send data along with the command and address bytes (+2) */
            status = ptrOSPI->Send(cmd, index);
        } while (status == ARM_DRIVER_ERROR_BUSY);

        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK) {
            return status;
        }

        /* Read status until device ready */
        status = MX66UW1G_AwaitProgramResult(&FlashStatus);
        if (status != ARM_DRIVER_OK) {
            return status;
        }

        if (FlashStatus.error) {
            break;
        }


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
 * @fn         int32_t ARM_Flash_EraseSector(uint32_t addr)
 * @brief      Erase Flash Sector.
 * @param[in]  addr   Sector address.
 * @return     execution_status
 */
static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    int32_t status;
    uint32_t cmd[2];

    if (addr > (FLASH_MX66UW1G_SECTOR_COUNT * FLASH_MX66UW1G_SECTOR_SIZE)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                             (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                             (0 << ARM_OSPI_WAIT_CYCLE_POS));
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    status = MX66UW1G_SetWriteEnable();
    if (status == ARM_DRIVER_OK) {

        FlashStatus.busy  = 1U;
        FlashStatus.error = 0U;

        status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                    ARM_OSPI_DATA_BITS(8) |
                    control_flags,
                    OSPI_BUS_SPEED);
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        /* Prepare command with address */
        cmd[0] = CMD_SE4B;
        cmd[1] = addr;

        status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                                 (ARM_OSPI_ADDR_LENGTH_32_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                                 (0 << ARM_OSPI_WAIT_CYCLE_POS));
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        status = ptrOSPI->Send(cmd, 2U);
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK) {
            return status;
        }

        status = MX66UW1G_AwaitEraseResult(&FlashStatus);
    }
    return status;
}


/**
 * @fn      int32_t ARM_Flash_EraseChip(void)
 * @brief   Erase complete Flash. Optional function for faster full chip erase.
 * @return  execution_status
 */
static int32_t ARM_Flash_EraseChip(void)
{
    uint32_t cmd;
    int32_t status;

    FlashStatus.busy  = 1U;
    FlashStatus.error = 0U;

    status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                             (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                             (0 << ARM_OSPI_WAIT_CYCLE_POS));
    if (status != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    status = MX66UW1G_SetWriteEnable();
    if (status == ARM_DRIVER_OK) {

        status = ptrOSPI->Control(ARM_OSPI_MODE_MASTER |
                    ARM_OSPI_DATA_BITS(8) |
                    control_flags,
                    OSPI_BUS_SPEED);
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        /* Prepare command */
        cmd = CMD_CE;

        status = ptrOSPI->Control(ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE | control_flags,
                                 (ARM_OSPI_ADDR_LENGTH_0_BITS << ARM_OSPI_ADDR_LENGTH_POS) |
                                 (0 << ARM_OSPI_WAIT_CYCLE_POS));
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        status = ptrOSPI->Send(&cmd, 1U);
        if (status != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }

        status = wait_for_completion();
        if (status != ARM_DRIVER_OK) {
            return status;
        }

        status = MX66UW1G_AwaitEraseResult(&FlashStatus);
    }

    return status;
}

/**
 * @fn      ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
 * @brief   Get Flash status.
 * @return  ARM_FLASH_STATUS
 */
static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    uint8_t val;

    if (FlashStatus.busy == 1U) {

        /* Read flag status register */
        if (MX66UW1G_ReadStatusReg(&val) == ARM_DRIVER_OK) {
            /* Check "Program or erase controller" bit */
            if ((val & (FLAG_STATUS_WIP | FLAG_STATUS_WEL)) == 0U) {
                FlashStatus.busy = 0U;
            }

            FlashStatus.error = 0U;
        } else {
            FlashStatus.error = 1U;
        }
    }

    return FlashStatus;
}

static ARM_FLASH_INFO *ARM_Flash_GetInfo(void)
{
    return &MX66UW1G_FlashInfo;
}

static ARM_FLASH_INFO *GetInfo(void)
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

static ARM_FLASH_STATUS GetStatus(void)
{
    return ARM_Flash_GetStatus();
}

static int32_t EraseChip(void)
{
    return ARM_Flash_EraseChip();
}

static int32_t EraseSector(uint32_t addr)
{
    return ARM_Flash_EraseSector(addr);
}

static int32_t ProgramData(uint32_t addr, const void *data, uint32_t cnt)
{
    return ARM_Flash_ProgramData(addr, data, cnt);
}

static int32_t ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    return ARM_Flash_ReadData(addr, data, cnt);
}

static int32_t PowerControl(ARM_POWER_STATE state)
{
    return ARM_Flash_PowerControl(state);
}

static int32_t Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    return ARM_Flash_Initialize(cb_event);
}

static int32_t Uninitialize(void)
{
    return ARM_Flash_Uninitialize();
}

/* Flash Driver Control Block */
extern ARM_DRIVER_FLASH ARM_Driver_Flash_(DRIVER_FLASH_NUM);
ARM_DRIVER_FLASH        ARM_Driver_Flash_(DRIVER_FLASH_NUM) = {
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

#endif /* defined(RTE_Drivers_MX66UW1G_FLASH) */
