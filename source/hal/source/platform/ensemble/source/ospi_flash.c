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

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "pinconf.h"
#include "Driver_GPIO.h"
#include "Driver_Flash.h"
#include "Driver_OSPI.h"
#include "IS25WX256.h"
#include "ospi.h"

#include "log_macros.h"

#define OSPI_RESET_PORT     LP
#define OSPI_RESET_PIN      7

/* NOTE: According to IS25WX256 flash device datasheet fast read wait cycles
         can be set to 9 at 100MHz bus clock.
         As of Ensemble CMSIS pack 1.0.0 the default wait cycles is 16
         and it is set in IS25WX256.c
         Set both sides to 9 when doing performance testing. */
#define FLASH_DEVICE_FAST_READ_WAIT_CYCLES (16U)

extern ARM_DRIVER_FLASH ARM_Driver_Flash_(1);
static ARM_DRIVER_FLASH* const ptrDrvFlash = &ARM_Driver_Flash_(1);

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(OSPI_RESET_PORT);
static ARM_DRIVER_GPIO* const GPIODrv = &ARM_Driver_GPIO_(OSPI_RESET_PORT);

static void ospi_flash_enable_xip()
{
    // so far this is for the OSPI1 only.
    OSPI_Type *ospi = (OSPI_Type *) OSPI1_BASE;
    AES_Type *aes = (AES_Type *)AES1_BASE;
    ospi_disable(ospi);

    uint32_t val = (1 << 31)
        |(SPI_FRAME_FORMAT_OCTAL << 22)
        |(0 << 9)
        |(0 << SPI_CTRLR0_INST_L_OFFSET)
        |(0 << 14)
        |(SPI_TMOD_RX << 10)
        |(SPI_CTRLR0_DFS_16bit << SPI_CTRLR0_TRANS_TYPE_OFFSET);

    ospi->OSPI_CTRLR0 = val;

    val = (SPI_FRAME_FORMAT_OCTAL << XIP_CTRL_FRF_OFFSET)
            | (0x2 << XIP_CTRL_TRANS_TYPE_OFFSET)
            | (0x8 << XIP_CTRL_ADDR_L_OFFSET)
            | (0x2 << XIP_CTRL_INST_L_OFFSET)
            | (0x0 << XIP_CTRL_MD_BITS_EN_OFFSET)
            | (FLASH_DEVICE_FAST_READ_WAIT_CYCLES << XIP_CTRL_WAIT_CYCLES_OFFSET)
            | (0x1 << XIP_CTRL_DFS_HC_OFFSET)
            | (0x1 << XIP_CTRL_DDR_EN_OFFSET)
            | (0x0 << XIP_CTRL_INST_DDR_EN_OFFSET)
            | (0x1 << XIP_CTRL_RXDS_EN_OFFSET)
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
    ospi->OSPI_XIP_INCR_INST = 0xFD;
    ospi->OSPI_XIP_WRAP_INST = 0xFD;
    ospi->OSPI_XIP_SER = 1;

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


int32_t ospi_flash_init()
{
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
    return ret;
}
