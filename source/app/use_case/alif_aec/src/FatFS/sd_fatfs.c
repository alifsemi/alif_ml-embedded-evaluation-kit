/* Copyright (C) Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/* System Includes */
#include "RTE_Device.h"
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "ff.h"
#include "diskio.h"

#include "pinconf.h"
#include "board_defs.h"
#include "services_lib_api.h"
extern uint32_t services_handle;

#define MEDIA_NAME                     "/SD_DISK/"
#define TEST_FILE                      "/audio_out.wav"
#define FILE_READ_TEST                 TEST_FILE

/* SD sector size; f_write does DMA-based direct writes only from sector-aligned offsets. */
#define SD_SECTOR_SIZE                 512U
/* DMA-capable staging buffer for f_write. Must be a multiple of SD_SECTOR_SIZE so every
 * chunk keeps the file position sector-aligned and the DMA source stays 512-aligned. */
#define FILEBUF_BYTES                  (16U * SD_SECTOR_SIZE) /* 8192 bytes */
unsigned char filebuffer[FILEBUF_BYTES] __attribute__((section("sd_dma_buf")))
__attribute__((aligned(512)));
FATFS sd_card __attribute__((section("sd_dma_buf"))) __attribute__((aligned(512)));
FIL test_file __attribute__((section("sd_dma_buf"))) __attribute__((aligned(512)));


int init_fs()
{
#ifdef BOARD_SD_RESET_GPIO_PORT

    pinconf_set(PORT_(BOARD_SD_RESET_GPIO_PORT), BOARD_SD_RESET_GPIO_PIN, 0, 0); //SD reset

#endif

#ifdef BOARD_SD_CARD_DETECT_GPIO_PORT
    /* Configure card detect GPIO with pull-up and read enable */
    pinconf_set(PORT_(BOARD_SD_CARD_DETECT_GPIO_PORT), BOARD_SD_CARD_DETECT_GPIO_PIN,
                PINMUX_ALTERNATE_FUNCTION_0,
                (PADCTRL_DRIVER_DISABLED_PULL_UP | PADCTRL_READ_ENABLE));
#endif

    pinconf_set(PORT_(BOARD_SD_CMD_A_GPIO_PORT),
                BOARD_SD_CMD_A_GPIO_PIN,
                BOARD_SD_CMD_ALTERNATE_FUNCTION,
                PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA);  // cmd
    pinconf_set(PORT_(BOARD_SD_CLK_A_GPIO_PORT),
                BOARD_SD_CLK_A_GPIO_PIN,
                BOARD_SD_CLK_ALTERNATE_FUNCTION,
                PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA);  // clk
    pinconf_set(PORT_(BOARD_SD_D0_A_GPIO_PORT),
                BOARD_SD_D0_A_GPIO_PIN,
                BOARD_SD_D0_ALTERNATE_FUNCTION,
                PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA);  // d0
#if RTE_SDC_BUS_WIDTH == SDMMC_4_BIT_MODE
    pinconf_set(PORT_(BOARD_SD_D1_A_GPIO_PORT),
                BOARD_SD_D1_A_GPIO_PIN,
                BOARD_SD_D1_ALTERNATE_FUNCTION,
                PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA);  // d1
    pinconf_set(PORT_(BOARD_SD_D2_A_GPIO_PORT),
                BOARD_SD_D2_A_GPIO_PIN,
                BOARD_SD_D2_ALTERNATE_FUNCTION,
                PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA);  // d2
    pinconf_set(PORT_(BOARD_SD_D3_A_GPIO_PORT),
                BOARD_SD_D3_A_GPIO_PIN,
                BOARD_SD_D3_ALTERNATE_FUNCTION,
                PADCTRL_READ_ENABLE | PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA);  // d3
#endif


    uint32_t service_error_code;
    uint32_t error_code;
    error_code = SERVICES_clocks_enable_clock(services_handle,
                                              CLKEN_CLK_100M,
                                              true,
                                              &service_error_code);
    if (error_code) {
        printf("SE: SDMMC 100MHz clock disable = %" PRIu32 "\n", error_code);
        return 0;
    }

    error_code = SERVICES_clocks_enable_clock(services_handle,
                                              CLKEN_CLK_20M,
                                              true,
                                              &service_error_code);
    if (error_code) {
        printf("SE: SDMMC 20MHz clock disable = %" PRIu32 "\n", error_code);
        return 0;
    }


    FRESULT fr;
    /* Open the SD disk. and initialize SD controller */
    fr = f_mount(&sd_card, MEDIA_NAME, 1);

    /* Check the media open status.  */
    if (fr) {
        printf("media open fail status = %" PRId16 "...\n", fr);
        return fr;
    }
    printf("SD Mounted Successfully...\n");

    /* Open the test file.  */
    fr = f_open(&test_file, TEST_FILE, FA_CREATE_ALWAYS | FA_WRITE);

    /* Check the file open status.  */
    if (fr) {
        printf("File open status: %" PRIi16 "\n", fr);
        /* Error opening file, break the loop.  */
        return fr;
    }

    return 0;
}

int close_file()
{
    FRESULT fr;
    printf("Closing File...%s\n", TEST_FILE);

    /* Close the test file.  */
    fr = f_close(&test_file);

    /* Check the file close status.  */
    if (fr) {
        printf("File close status: %" PRId16 "\n", fr);
        /* Error closing the file, break the loop.  */
        return fr;
    }
    return 0;
}

int write_file(const int16_t* data, uint32_t numSamples)
{
    FRESULT fr;
    UINT bw;
    printf("Writing Data in File...%s\n", TEST_FILE);

    /* Stage into the DMA-capable filebuffer in FILEBUF_BYTES chunks. FILEBUF_BYTES is a
     * multiple of SD_SECTOR_SIZE, so every chunk but the last keeps the file position
     * sector-aligned; this is required for the SD DMA direct-write path. */
    uint32_t remaining = numSamples * sizeof(int16_t);
    const uint8_t* src = (const uint8_t*) data;
    while (remaining > 0) {
        uint32_t chunk = (remaining > FILEBUF_BYTES) ? FILEBUF_BYTES : remaining;

        memcpy(filebuffer, src, chunk);

        fr = f_write(&test_file, (void *) filebuffer, chunk, &bw);

        /* Check the file write status.  */
        if (fr) {
            printf("File write status: %" PRIi16 "\n", fr);
            return fr;
        }

        if (bw != chunk) {
            printf("File write incomplete: %" PRIu32 " bytes written\n", bw);
            return -1;
        }

        src += chunk;
        remaining -= chunk;
    }

    return 0;
}

int deinit_fs()
{
    uint32_t service_error_code;
    uint32_t error_code;
    /* Unmount the SD card */
    f_unmount(MEDIA_NAME);

    /* Deinitialize SD card through diskio interface */
    disk_deinitialize(0);

    error_code = SERVICES_clocks_enable_clock(services_handle,
                                              CLKEN_CLK_100M,
                                              false,
                                              &service_error_code);

    error_code = SERVICES_clocks_enable_clock(services_handle,
                                              CLKEN_CLK_20M,
                                              false,
                                              &service_error_code);

    return 0;
}
