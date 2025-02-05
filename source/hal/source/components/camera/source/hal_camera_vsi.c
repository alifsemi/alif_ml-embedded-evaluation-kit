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
#include "video_drv.h"
#include "RTE_Components.h"

#include <string.h>

typedef struct hal_camera_device_ {
    char name[32];
    uint32_t frame_width;
    uint32_t frame_height;
    uint32_t bytes_per_frame;
    hal_cam_clr_format format;
    hal_cam_mode mode;
    hal_cam_status status;
} hal_cam_dev;

/**< Static device used in this file. */
static hal_cam_dev s_vsi_dev;

/**< Extern functions supposed to be defined by the VSI driver. */
extern void ARM_VSI4_Handler(void);
extern void ARM_VSI5_Handler(void);
extern void ARM_VSI6_Handler(void);
extern void ARM_VSI7_Handler(void);

/**
 * @brief   Resets the camera device.
 */
static void hal_camera_reset(void)
{
    s_vsi_dev.status = HAL_CAMERA_STATUS_INVALID;
    strncpy(s_vsi_dev.name, "VSI", sizeof(s_vsi_dev.name));
    s_vsi_dev.frame_width = 0;
    s_vsi_dev.frame_height = 0;
    s_vsi_dev.bytes_per_frame = 0;
    s_vsi_dev.format = HAL_CAMERA_COLOUR_FORMAT_INVALID;
    s_vsi_dev.mode = HAL_CAMERA_MODE_INVALID;
    VideoDrv_Uninitialize();
}

bool hal_camera_init(void)
{
    hal_camera_reset();
    info("Initialising camera interface: %s\n", s_vsi_dev.name);

    if (VideoDrv_Initialize(NULL) != VIDEO_DRV_OK) {
        printf_err("Failed to initialise video driver\n");
        return false;
    }

    /** Entries into the interrrupt table */
    NVIC_SetVector((IRQn_Type)ARM_VSI4_IRQn, (uint32_t)ARM_VSI4_Handler);
    NVIC_SetVector((IRQn_Type)ARM_VSI5_IRQn, (uint32_t)ARM_VSI4_Handler);
    NVIC_SetVector((IRQn_Type)ARM_VSI6_IRQn, (uint32_t)ARM_VSI4_Handler);
    NVIC_SetVector((IRQn_Type)ARM_VSI7_IRQn, (uint32_t)ARM_VSI4_Handler);

    return true;
}

bool hal_camera_configure(const uint32_t width,
                          const uint32_t height,
                          const hal_cam_mode mode,
                          const hal_cam_clr_format colour_format)
{
    uint32_t vsi_driver_colour_format = VIDEO_DRV_COLOR_FORMAT_BEGIN;

    if (HAL_CAMERA_STATUS_RUNNING == s_vsi_dev.status) {
        printf_err("Camera is running; configuration failed\n");
        return false;
    }

    if (HAL_CAMERA_MODE_CONTINUOUS == mode) {
        printf_err("Only single shot mode is supported\n");
        return false;
    }

    s_vsi_dev.frame_width = width;
    s_vsi_dev.frame_height = height;
    s_vsi_dev.mode = mode;
    s_vsi_dev.format = colour_format;
    s_vsi_dev.status = HAL_CAMERA_STATUS_STOPPED;

    switch (s_vsi_dev.format) {
        case HAL_CAMERA_COLOUR_FORMAT_RGB888:
            s_vsi_dev.bytes_per_frame = width * height * 3;
            vsi_driver_colour_format = VIDEO_DRV_COLOR_RGB888;
            break;
        case HAL_CAMERA_COLOUR_FORMAT_RGB565:
            s_vsi_dev.bytes_per_frame = width * height * 2;
            vsi_driver_colour_format = VIDEO_DRV_COLOR_BGR565;
            break;
        default:
            printf_err("Unsupported colour format\n");
            hal_camera_reset();
            return false;
    }

    if (VideoDrv_Configure(VIDEO_DRV_IN0,
                           width,
                           height,
                           vsi_driver_colour_format,
                           24U /* frame rate */) != VIDEO_DRV_OK) {
        printf_err("Failed to configure video input\n");
        hal_camera_reset();
        return false;
    }

    if (DYNAMIC_IFM_SIZE < s_vsi_dev.bytes_per_frame) {
        printf_err("DYNAMIC_IFM_SIZE is too small\n");
        return false;
    }

    hal_camera_set_buffer((uint8_t *)(DYNAMIC_IFM_BASE), s_vsi_dev.bytes_per_frame);
    info("Camera configured\n");
    return true;
}

bool hal_camera_set_buffer(uint8_t* buffer, const uint32_t size)
{
    UNUSED(buffer);
    if (size != s_vsi_dev.bytes_per_frame) {
        return false;
    }

    /* Set Input Video buffer */
    if (VideoDrv_SetBuf(VIDEO_DRV_IN0,  buffer, size) != VIDEO_DRV_OK) {
        printf_err("Failed to set buffer for video input\n");
        return false;
    }

    return true;
}

bool hal_camera_start(void)
{
    if (s_vsi_dev.status != HAL_CAMERA_STATUS_STOPPED) {
        debug("Camera is already running\n");
        return false;
    }

    if (VideoDrv_StreamStart(VIDEO_DRV_IN0, VIDEO_DRV_MODE_SINGLE) != VIDEO_DRV_OK) {
        printf_err("Failed to start video capture\n");
        s_vsi_dev.status = HAL_CAMERA_STATUS_ERROR;
        return false;
    }

    s_vsi_dev.status = HAL_CAMERA_STATUS_RUNNING;
    return true;
}

/**
 * @brief   Waits for the input frame capture to complete.
 * @return  Status of the device as `hal_cam_status`.
 */
static hal_cam_status wait_for_vsi_capture(void)
{
    hal_cam_status status;
    debug("Waiting for active frame\n");

    /* Wait for video input frame */
    do {
        status = hal_camera_get_status();
        if (status == HAL_CAMERA_STATUS_ERROR) {
            printf_err("Camera error\n");
            break;
        }
    } while (status != HAL_CAMERA_STATUS_STOPPED);

    return status;
}

const uint8_t* hal_camera_get_captured_frame(uint32_t* size)
{
    static unsigned int idx = 0;
    uint8_t* buffer = NULL;
    *size = 0;
    if (s_vsi_dev.status == HAL_CAMERA_STATUS_RUNNING) {
        const hal_cam_status status = wait_for_vsi_capture();
        if (HAL_CAMERA_STATUS_ERROR == status) {
            printf_err("Error reported\n");
            return buffer;
        }

        info("Frame: %u\n", idx);
        buffer = (uint8_t*)VideoDrv_GetFrameBuf(VIDEO_DRV_IN0);
        *size = s_vsi_dev.bytes_per_frame;
        debug("Buffer: 0x%p; size: %u\n", buffer, *size);
        ++idx;
    }
    return buffer;
}

bool hal_camera_stop(void)
{
    if (s_vsi_dev.status == HAL_CAMERA_STATUS_RUNNING) {
        s_vsi_dev.status = HAL_CAMERA_STATUS_STOPPED;
    }

    /* Release input frame */
    VideoDrv_ReleaseFrame(VIDEO_DRV_IN0);

    return true;
}

hal_cam_status hal_camera_get_status(void)
{
    const VideoDrv_Status_t status = VideoDrv_GetStatus(VIDEO_DRV_IN0);
    if (status.overflow || status.underflow || status.eos) {
        s_vsi_dev.status = HAL_CAMERA_STATUS_ERROR;
        return s_vsi_dev.status;
    }
    if (!status.buf_empty) {
        /* Frame capture has completed */
        s_vsi_dev.status = HAL_CAMERA_STATUS_STOPPED;
        return s_vsi_dev.status;
    }
    if (status.active) {
        s_vsi_dev.status = HAL_CAMERA_STATUS_RUNNING;
        return s_vsi_dev.status;
    }
    return s_vsi_dev.status;
}

void hal_camera_release(void)
{
    hal_camera_reset();
}

const char* hal_camera_get_device_name(void)
{
    return s_vsi_dev.name;
}
