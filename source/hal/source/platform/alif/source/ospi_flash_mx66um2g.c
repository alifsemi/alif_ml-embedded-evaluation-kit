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
#include "board.h"

#include "pinconf.h"
#include "Driver_GPIO.h"
#include "Driver_Flash.h"
#include "Driver_OSPI.h"
#include "MX66UM2G.h"
#include "ospi.h"

#include "ospi_flash.h"
#include "log_macros.h"

#ifdef BOARD_HAS_MX66UM2G_FLASH

#define CMD_8READ                                               (0xEC13U) // 8 I/O read
#define CMD_8DTRD                                               (0xEE11U) // 8 I/O DT read

#define DDR_OPTION FLASH_MX66UM2G_DO_DDR

//#define PROGRAM_TEST
#define IMMEDIATE_XIP
//#define SEMIHOSTING_WRITE "ER_OSPI_FLASH"
//#define DO_FLASH_WRITE

#define OSPI_RESET_PORT     LP
#define OSPI_RESET_PIN      2

#ifdef BOARD_IS_FPGA
#define FLASH_DEVICE_FAST_READ_WAIT_CYCLES 6
#else
todo #define FLASH_DEVICE_FAST_READ_WAIT_CYCLES
#endif

#define WAIT_TIMEOUT                            1024


extern ARM_DRIVER_FLASH ARM_Driver_Flash_(2);
static ARM_DRIVER_FLASH* const ptrDrvFlash = &ARM_Driver_Flash_(2);

extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(OSPI_RESET_PORT);
static ARM_DRIVER_GPIO* const GPIODrv = &ARM_Driver_GPIO_(OSPI_RESET_PORT);

extern int32_t MX66UM2G_set_linear(void);
extern int32_t MX66UM2G_set_wrap32(void);

int32_t ospi_flash_set_linear(void)
{
    int32_t status = MX66UM2G_set_linear();
    if (status != ARM_DRIVER_OK) {
        printf("Error setting flash linear\n");
    }
    return status;
}

int32_t ospi_flash_set_wrap32(void)
{
    int32_t status = MX66UM2G_set_wrap32();
    if (status != ARM_DRIVER_OK) {
        printf("Error setting flash wrap32\n");
    }
    return status;
}

static void ospi_flash_enable_xip(void)
{
    // so far this is for the OSPI0 only.
    OSPI_Type *ospi = (OSPI_Type *) OSPI0_BASE;
    AES_Type *aes = (AES_Type *)AES0_BASE;
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

#define FLASH_BUF_SIZE (32*1024)
static int32_t program_from_semihosting(const char *filename)
{
    int32_t ret;

    char *buf = malloc(FLASH_BUF_SIZE);
    if (!buf) {
        printf_err("Alloc failure\n");
        return ARM_DRIVER_ERROR;
    }

    printf("Attempting to open %s\n", filename);
    FILE *bin = fopen(filename, "rb");
    if (!bin) {
        printf_err("Failed to open %s\n", filename);
        ret = ARM_DRIVER_ERROR;
        goto err;
    }

    if (fseek(bin, 0, SEEK_END) != 0) {
        printf_err("Seek error\n");
        ret = ARM_DRIVER_ERROR;
        goto err;
    }

    long len = ftell(bin);
    if (len < 0) {
        printf_err("ftell error\n");
        ret = ARM_DRIVER_ERROR;
        goto err;
    }

    printf("File size %ld bytes\n", len);
    rewind(bin);

    if (len & 1) {
        len++;
    }

    if (len > 100000000) {
        printf("Erasing chip (may take 5 minutes)\n");
        ret = ptrDrvFlash->EraseChip();
        if (ret != ARM_DRIVER_OK) {
            printf_err("Ext flash erase failed\n");
            goto err;
        }
    } else {
        ARM_FLASH_INFO *info = ptrDrvFlash->GetInfo();

        for (uint32_t addr = 0; addr < (uint32_t) len; addr += info->sector_size) {
            printf("Erasing %" PRIX32 "\n", addr);
            ret = ptrDrvFlash->EraseSector(addr);
            if (ret != ARM_DRIVER_OK) {
                printf_err("Ext flash erase failed\n");
                goto err;
            }
        }
    }

    uint32_t addr = 0;
    while (!feof(bin) && !ferror(bin)) {
        size_t bytes = fread(buf, 1, FLASH_BUF_SIZE, bin);
        if (bytes > 0) {
            printf("Programming %" PRIX32 "\n", addr);
            /* I spent one whole day trying to figure out the symptoms of passing simply "bytes" */
            ret = ptrDrvFlash->ProgramData(addr, buf, bytes / 2);
            if (ret != (int32_t) (bytes / 2)) {
                printf("\nProgramming error %" PRId32 "\n", ret);
                goto err;
            }
        }
        addr += bytes;
    }

    if (ferror(bin)) {
        printf_err("File error\n");
        ret = ARM_DRIVER_ERROR;
        goto err;
    }
    printf("Programmed %" PRIu32 " bytes\n", addr);
    ret = ARM_DRIVER_OK;

err:
    free(buf);
    return ret;
}


int32_t ospi_flash_init(void)
{
    printf("Attempting MX66UM2G flash initialisation\n");
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

#ifdef PROGRAM_TEST
    ret = ptrDrvFlash->EraseSector(0);
    if (ret != ARM_DRIVER_OK) {
        printf_err("Ext flash erase failed\n");
        return ret;
    }

    uint16_t wdata[512];
    uint8_t * const data = (uint8_t *) wdata;
    for (size_t i = 0; i < sizeof wdata; i++) {
        data[i] = ((i & 0xFF) << (i / 256)) | ((i & 0xFF) >> (8-i/256));
    }

    ret = ptrDrvFlash->ProgramData(0, wdata, sizeof wdata / 2);
    if (ret != sizeof wdata / 2) {
        printf_err("Ext flash program failed\n");
        //return ARM_DRIVER_ERROR;
    }
    ARM_FLASH_STATUS flash_status = ptrDrvFlash->GetStatus();
    if (flash_status.error) {
        printf_err("Flash error status\n");
    } else {
        printf("Flash program successful\n");
    }

    printf("MX66UM2G flash apparently okay\n");
#endif

#ifdef SEMIHOSTING_WRITE
    program_from_semihosting(SEMIHOSTING_WRITE);
#endif

#ifdef IMMEDIATE_XIP
    ospi_flash_enable_xip();
    printf("XIP test flash - may lock up\n");
    (void)*(volatile uint32_t *) 0xA0000000;
    printf("XIP test flash OK\n");

#ifdef PROGRAM_TEST
    if (memcmp(wdata, (uint32_t *) 0xA0000000, sizeof wdata) == 0) {
        printf("XIP memcmp good\n");
    } else {
        printf("XIP memcmp fail\n");
    }
#endif
#endif

    return ARM_DRIVER_OK;
}

const uint8_t *do_flash_write(const uint8_t *ptr, size_t len)
{
#ifdef DO_FLASH_WRITE
    ARM_FLASH_INFO *info = ptrDrvFlash->GetInfo();
    int32_t ret;

    SCB_InvalidateDCache_by_Addr((volatile void *) 0xA8000000, len);
    for (uint32_t offset = 0x08000000; offset < 0x08000000 + len; offset += info->sector_size) {
        printf("\rErasing %" PRIX32, 0xA0000000 + offset);
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
    ret = ptrDrvFlash->ProgramData(0x08000000, ptr, (len + 1) / 2);
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
    SCB_InvalidateDCache_by_Addr((volatile void *) 0xA8000000, len);
    const uint8_t *flash_ptr = (const uint8_t *) 0xA8000000;
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

#endif // BOARD_HAS_MX66UM2G_FLASH

