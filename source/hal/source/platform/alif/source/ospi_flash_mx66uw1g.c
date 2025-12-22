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

#include <string.h>
#include <stdlib.h>

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header
#include "board_defs.h"

#include "pinconf.h"
#include "Driver_IO.h"
#include "Driver_Flash.h"
#include "Driver_OSPI.h"
#include "MX66UW1G.h"
#include "ospi.h"
#include "ospi_private.h"

#include "ospi_flash.h"
#include "log_macros.h"

#ifdef BOARD_HAS_MX66UW1G_FLASH

#define CMD_8READ                                               (0xEC13U) // 8 I/O read
#define CMD_8DTRD                                               (0xEE11U) // 8 I/O DT read

#define DDR_OPTION 1

#define OSPI_RESET_PORT     BOARD_OSPI_FLASH_RESET_GPIO_PORT
#define OSPI_RESET_PIN      BOARD_OSPI_FLASH_RESET_PIN_NO

#define FLASH_DEVICE_FAST_READ_WAIT_CYCLES RTE_MX66UW1G_FLASH_WAIT_CYCLES

#define WAIT_TIMEOUT                            1024

extern ARM_DRIVER_FLASH ARM_Driver_Flash_(2);
static ARM_DRIVER_FLASH* const ptrDrvFlash = &ARM_Driver_Flash_(2);

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(OSPI_RESET_PORT);
static ARM_DRIVER_GPIO* const GPIODrv = &ARM_Driver_GPIO_(OSPI_RESET_PORT);

int32_t ospi_flash_set_linear(void)
{
    return ARM_DRIVER_OK;
}

int32_t ospi_flash_set_wrap32(void)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static void ospi_flash_enable_xip(void)
{
#if RTE_MX66UW1G_FLASH_OSPI_DRV_NUM == 0
    OSPI_Type *ospi = (OSPI_Type *) OSPI0_BASE;
    AES_Type *aes = (AES_Type *)AES0_BASE;
#else
    OSPI_Type *ospi = (OSPI_Type *) OSPI1_BASE;
    AES_Type *aes = (AES_Type *)AES1_BASE;
#endif
    ospi_flash_set_wrap32();
    ospi_disable(ospi);

    uint32_t val = (1 << SPI_CTRLR0_SSI_IS_MST)
        | (SPI_FRAME_FORMAT_OCTAL << SPI_CTRLR0_SPI_FRF)
        | SPI_CTRLR0_SCPOL_LOW
        | SPI_CTRLR0_SCPH_LOW
        | (0 << SPI_CTRLR0_SSTE)
        | (SPI_TMOD_RX << SPI_CTRLR0_TMOD)
        | ((DDR_OPTION == 0 ? SPI_CTRLR0_DFS_8bit : SPI_CTRLR0_DFS_16bit) << SPI_CTRLR0_DFS);

    ospi->OSPI_CTRLR0 = val;

    val = (SPI_FRAME_FORMAT_OCTAL << XIP_CTRL_FRF_OFFSET)
            | (0x2 << XIP_CTRL_TRANS_TYPE_OFFSET)
            | (0x8 << XIP_CTRL_ADDR_L_OFFSET)
            | (0x3 << XIP_CTRL_INST_L_OFFSET)
            | (0x0 << XIP_CTRL_MD_BITS_EN_OFFSET)
            | (FLASH_DEVICE_FAST_READ_WAIT_CYCLES << XIP_CTRL_WAIT_CYCLES_OFFSET)
            | (0x1 << XIP_CTRL_DFS_HC_OFFSET)
            | (DDR_OPTION << XIP_CTRL_DDR_EN_OFFSET)
            | (DDR_OPTION << XIP_CTRL_INST_DDR_EN_OFFSET)
            | (DDR_OPTION << XIP_CTRL_RXDS_EN_OFFSET)
            | (0x1 << XIP_CTRL_INST_EN_OFFSET)
            | (0x0 << XIP_CTRL_CONT_XFER_EN_OFFSET)
            | (0x0 << XIP_CTRL_XIP_HYPERBUS_EN_OFFSET)
            | (0x0 << XIP_CTRL_RXDS_SIG_EN_OFFSET)
            | (0x0 << XIP_CTRL_XIP_MBL_OFFSET)
            | (0x0 << XIP_CTRL_XIP_PREFETCH_EN_OFFSET)
            | (0x0 << XIP_CTRL_RXDS_VL_EN_OFFSET);

    ospi->OSPI_XIP_CTRL = val;

    ospi->OSPI_RX_SAMPLE_DELAY = 0;

    ospi->OSPI_XIP_MODE_BITS = 0x00;
    ospi->OSPI_XIP_WRAP_INST =
    ospi->OSPI_XIP_INCR_INST = DDR_OPTION ? CMD_8DTRD : CMD_8READ;
    //ospi->OSPI_XIP_SER = 1;

    ospi->OSPI_XIP_CNT_TIME_OUT = 255;

    ospi_enable(ospi);

    aes->AES_CONTROL |= AES_CONTROL_XIP_EN;
}


static int32_t ospi_flash_toggle_reset(void)
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


int32_t ospi_flash_init(void)
{
    printf("Attempting MX66UW1G flash initialisation\n");
    int32_t ret = ospi_flash_toggle_reset();
    if (ret != ARM_DRIVER_OK) {
        printf_err("OSPI reset failed\n");
        return ret;
    }

    // Initialize ext-FLASH driver
    ret = ptrDrvFlash->Initialize(NULL);
    if (ret != ARM_DRIVER_OK) {
        printf_err("Ext flash driver init failed\n");
        return ret;
    }

    ret = ptrDrvFlash->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        printf_err("Ext flash device init failed\n");
        return ret;
    }

    ospi_flash_enable_xip();

    return ARM_DRIVER_OK;
}

#endif // BOARD_HAS_MX66UW1G_FLASH
