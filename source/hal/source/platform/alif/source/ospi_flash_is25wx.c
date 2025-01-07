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
#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "pinconf.h"
#include "Driver_IO.h"
#include "Driver_Flash.h"
#include "Driver_OSPI.h"
#include "IS25WX256.h"
#include "ospi.h"
#include "ospi_private.h"

#include "ospi_flash.h"
#include "log_macros.h"

#ifdef BOARD_HAS_IS25WX_FLASH

#define IMMEDIATE_XIP
//#define SEMIHOSTING_WRITE "ER_OSPI_FLASH"
//#define DO_FLASH_WRITE

#ifndef BOARD_OSPI_FLASH_RESET_GPIO_PORT
#error X
#endif

#define OSPI_RESET_PORT     BOARD_OSPI_FLASH_RESET_GPIO_PORT
#define OSPI_RESET_PIN      BOARD_OSPI_FLASH_RESET_PIN_NO

/* NOTE: According to IS25WX256 flash device datasheet fast read wait cycles
         can be set to 9 at 100MHz bus clock.
         For IS25WX512 minimum wait cycles is 10 (@100MHz)
         See RTE_Device.h */
#define FLASH_DEVICE_FAST_READ_WAIT_CYCLES (RTE_ISSI_FLASH_WAIT_CYCLES)

extern ARM_DRIVER_FLASH ARM_Driver_Flash_(1);
static ARM_DRIVER_FLASH* const ptrDrvFlash = &ARM_Driver_Flash_(1);

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(OSPI_RESET_PORT);
static ARM_DRIVER_GPIO* const GPIODrv = &ARM_Driver_GPIO_(OSPI_RESET_PORT);

int32_t ospi_flash_set_linear(void)
{
    return ARM_DRIVER_OK;
}

int32_t ospi_flash_set_wrap32(void)
{
    // TO DO - flash device does support this
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static void ospi_flash_enable_xip()
{
#if BOARD_FLASH_OSPI_INSTANCE == 0
    OSPI_Type *ospi = (OSPI_Type *) OSPI0_BASE;
    AES_Type *aes = (AES_Type *)AES0_BASE;
#else
    OSPI_Type *ospi = (OSPI_Type *) OSPI1_BASE;
    AES_Type *aes = (AES_Type *)AES1_BASE;
#endif
    ospi_disable(ospi);

    uint32_t val = (1 << 31)
        |(SPI_FRAME_FORMAT_OCTAL << SPI_CTRLR0_SPI_FRF)
        | SPI_CTRLR0_SCPOL_LOW
        | SPI_CTRLR0_SCPH_LOW
        |(0 << SPI_CTRLR0_SSTE)
        |(SPI_TMOD_RX << SPI_CTRLR0_TMOD)
        |(SPI_CTRLR0_DFS_16bit << SPI_CTRLR0_DFS);

    ospi->OSPI_CTRLR0 = val;

    val = (SPI_FRAME_FORMAT_OCTAL << XIP_CTRL_FRF_OFFSET)
            | (0x2 << XIP_CTRL_TRANS_TYPE_OFFSET)
            | (0x8 << XIP_CTRL_ADDR_L_OFFSET)
            | (0x3 << XIP_CTRL_INST_L_OFFSET)
            | (0x0 << XIP_CTRL_MD_BITS_EN_OFFSET)
            | (FLASH_DEVICE_FAST_READ_WAIT_CYCLES << XIP_CTRL_WAIT_CYCLES_OFFSET)
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

    ospi->OSPI_RX_SAMPLE_DELAY = 0;

    ospi->OSPI_XIP_MODE_BITS = 0x00;
    ospi->OSPI_XIP_INCR_INST = 0x0B0B;
    ospi->OSPI_XIP_WRAP_INST = 0x0B0B;
#if SOC_FEAT_OSPI_HAS_XIP_SER // OSPI_XIP_SER not available on Spark and Eagle
    ospi->OSPI_XIP_SER = 1 << BOARD_FLASH_OSPI_SS;
#endif
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

#ifdef IMMEDIATE_XIP
    ospi_flash_enable_xip();
#endif
    return ret;
}

const uint8_t *do_flash_write(const uint8_t *ptr, size_t len)
{
#ifdef DO_FLASH_WRITE
    ARM_FLASH_INFO *info = ptrDrvFlash->GetInfo();
    int32_t ret;

    SCB_InvalidateDCache_by_Addr((volatile void *) BOARD_OSPI_FLASH_BASE, len);
    for (uint32_t offset = 0x00000000; offset < 0x00000000 + len; offset += info->sector_size) {
        printf("\rErasing %" PRIX32, BOARD_OSPI_FLASH_BASE + offset);
        fflush(stdout); // GCC/newlib seems to be fully buffered...
        ret = ptrDrvFlash->EraseSector(offset);
        if (ret != ARM_DRIVER_OK) {
            printf_err("Ext flash erase failed\n");
            return ptr;
        }
    }
    putchar('\n');
    ARM_FLASH_STATUS flash_status = ptrDrvFlash->GetStatus();
    if (flash_status.error) {
        printf_err("Flash error status\n");
        return ptr;
    }

    printf("Programming\n");
    ret = ptrDrvFlash->ProgramData(0, ptr, (len + 1) / 2);
    if (ret != (len + 1) / 2) {
         printf_err("Ext flash program failed\n");
         ospi_flash_enable_xip();
         return ptr;
    }

    flash_status = ptrDrvFlash->GetStatus();
    if (flash_status.error) {
        printf_err("Flash error status\n");
        ospi_flash_enable_xip();
        return ptr;
    }
#endif

#ifndef IMMEDIATE_XIP
    printf("Going for XIP\n");
    ospi_flash_enable_xip();
#endif
    printf("Verifying flash\n");
    SCB_InvalidateDCache_by_Addr((volatile void *) BOARD_OSPI_FLASH_BASE, len);
    const uint8_t *flash_ptr = (const uint8_t *) BOARD_OSPI_FLASH_BASE;
//#ifndef DO_FLASH_WRITE
    if (memcmp(flash_ptr, ptr, len)) {
        printf_err("Verify fail\n");
        for (size_t i = 0; i < len; i += 2) {
            if (flash_ptr[i] != ptr[i+1] || flash_ptr[i+1] != ptr[i]) {
                printf("Swap verify fail at %d\n", i);
                return ptr;
            }
        }
        //return ptr;
    } else {
        printf("Flash model verified\n");
    }
//#endif
    return flash_ptr;
   // return ptr;
}

#endif // BOARD_HAS_IS25WX_FLASH
