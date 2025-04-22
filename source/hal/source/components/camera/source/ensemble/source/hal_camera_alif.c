/*
 * SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "hal_camera.h"
#include "log_macros.h"
#include "RTE_Components.h"
#include "RTE_Device.h"
#include "image_processing.h"
#include "bayer.h"
#include "camera.h"
#include "board.h"
#include "base_def.h"
#include "timer_ensemble.h"
#include "delay.h"
#include <tgmath.h>
#include <string.h>


#if !defined(RTE_Drivers_CPI)
#define USE_FAKE_CAMERA 1
#endif

/* Camera fills the raw_image buffer.
 * Bayer->RGB conversion transfers into the rgb_image buffer.
 * With MT9M114 camera this can be a RGB565 to RGB conversion.
 * Following steps (crop, interpolate, colour correct) all occur in the rgb_image buffer in-place.
 */
static struct {
	tiff_header_t tiff_header;
	uint8_t image_data[CIMAGE_RGB_WIDTH_MAX * CIMAGE_RGB_HEIGHT_MAX * RGB_BYTES];
} rgb_image __attribute__((section(".bss.camera_frame_bayer_to_rgb_buf")));

static uint8_t raw_image[CIMAGE_X * CIMAGE_Y + CIMAGE_USE_RGB565 * CIMAGE_X * CIMAGE_Y]
    __attribute__((aligned(32),section(".bss.camera_frame_buf")));


typedef struct hal_camera_device_ {
    char name[32];
    uint32_t frame_width;
    uint32_t frame_height;
    uint32_t bytes_per_frame;
    uint32_t output_buffer_size;
    uint8_t *output_buffer;
    hal_cam_clr_format format;
    hal_cam_mode mode;
    hal_cam_status status;
} hal_cam_dev;

/**< Static device used in this file. */
static hal_cam_dev s_cam_dev;

/**
 * @brief   Resets the camera device.
 */
static void hal_camera_reset(void)
{
    s_cam_dev.status = HAL_CAMERA_STATUS_INVALID;
    strncpy(s_cam_dev.name, "Alif camera", sizeof(s_cam_dev.name));
    s_cam_dev.frame_width = 0;
    s_cam_dev.frame_height = 0;
    s_cam_dev.bytes_per_frame = 0;
    s_cam_dev.output_buffer = NULL;
    s_cam_dev.format = HAL_CAMERA_COLOUR_FORMAT_INVALID;
    s_cam_dev.mode = HAL_CAMERA_MODE_INVALID;
#ifndef USE_FAKE_CAMERA
    camera_uninit();
#endif
}

bool hal_camera_init(void)
{
    hal_camera_reset();
    info("Initialising camera interface: %s\n", s_cam_dev.name);
#ifndef USE_FAKE_CAMERA
    int32_t err = 0;
    err = camera_init(raw_image);
	if (err != 0) {
        printf_err("Failed to initialise camera driver: %ld\n", err);
		while(1) {
		    BOARD_LED1_Control(BOARD_LED_STATE_LOW);
			sleep_or_wait_msec(300);
            BOARD_LED1_Control(BOARD_LED_STATE_HIGH);
			sleep_or_wait_msec(300);
		}
	}
	DEBUG_PRINTF("Camera initialized... \n");
    BOARD_LED1_Control(BOARD_LED_STATE_HIGH);
#endif
    return true;
}

bool hal_camera_configure(const uint32_t width,
                          const uint32_t height,
                          const hal_cam_mode mode,
                          const hal_cam_clr_format colour_format)
{

    if (HAL_CAMERA_STATUS_RUNNING == s_cam_dev.status) {
        printf_err("Camera is running; configuration failed\n");
        return false;
    }

    if (HAL_CAMERA_MODE_CONTINUOUS == mode) {
        printf_err("Only single shot mode is supported\n");
        return false;
    }

    s_cam_dev.frame_width = width;
    s_cam_dev.frame_height = height;
    s_cam_dev.mode = mode;
    s_cam_dev.format = colour_format;
    s_cam_dev.status = HAL_CAMERA_STATUS_STOPPED;

    switch (s_cam_dev.format) {
        case HAL_CAMERA_COLOUR_FORMAT_RGB888:
            s_cam_dev.bytes_per_frame = width * height * 3;
            break;
        default:
            printf_err("Unsupported colour format\n");
            hal_camera_reset();
            return false;
    }

    if (!hal_camera_set_buffer(rgb_image.image_data, sizeof rgb_image.image_data )) {
        return false;
    }

    info("Camera configured\n");
    return true;
}

bool hal_camera_set_buffer(uint8_t* buffer, const uint32_t size)
{
    if (size < s_cam_dev.bytes_per_frame) {
        printf_err("Buffer size is too small\n");
        return false;
    }

    s_cam_dev.output_buffer = buffer;
    s_cam_dev.output_buffer_size = size;

    return true;
}

bool hal_camera_start(void)
{
    if (s_cam_dev.status != HAL_CAMERA_STATUS_STOPPED) {
        debug("Camera is already running\n");
        return false;
    }

#ifndef USE_FAKE_CAMERA
    camera_start(CAMERA_MODE_SNAPSHOT);
#endif
    s_cam_dev.status = HAL_CAMERA_STATUS_RUNNING;
    return true;
}

/**
 * @brief   Waits for the input frame capture to complete.
 * @return  Status of the device as `hal_cam_status`.
 */
static hal_cam_status wait_for_capture(void)
{
#ifndef USE_FAKE_CAMERA
    /* Wait for video input frame */
    while ( hal_camera_get_status() != HAL_CAMERA_STATUS_STOPPED) {
        __WFE();
    }
#else
    s_cam_dev.status = HAL_CAMERA_STATUS_STOPPED;
#endif

    return s_cam_dev.status;
}

const uint8_t* hal_camera_get_captured_frame(uint32_t* size)
{
    const uint8_t* buffer = NULL;
    *size = 0;
    if (s_cam_dev.status == HAL_CAMERA_STATUS_RUNNING) {
        const hal_cam_status status = wait_for_capture();
        if (HAL_CAMERA_STATUS_ERROR == status) {
            printf_err("Error reported\n");
            return buffer;
        }
        SCB_CleanInvalidateDCache();
        buffer = get_image_data(s_cam_dev.frame_width, s_cam_dev.frame_height, rgb_image.tiff_header, s_cam_dev.output_buffer, s_cam_dev.output_buffer_size, raw_image);
        *size = s_cam_dev.bytes_per_frame;
    }
    return buffer;
}

bool hal_camera_stop(void)
{
    if (s_cam_dev.status == HAL_CAMERA_STATUS_RUNNING) {
        s_cam_dev.status = HAL_CAMERA_STATUS_STOPPED;
    }

    return true;
}

hal_cam_status hal_camera_get_status(void)
{
#ifndef USE_FAKE_CAMERA
    if (camera_image_ready()) {
        s_cam_dev.status = HAL_CAMERA_STATUS_STOPPED;
    }
#else
    s_cam_dev.status = HAL_CAMERA_STATUS_STOPPED;
#endif
    return s_cam_dev.status;
}

void hal_camera_release(void)
{
    hal_camera_reset();
}

const char* hal_camera_get_device_name(void)
{
    return s_cam_dev.name;
}
