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
#include "hal_camera_static_external.h"

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

static hal_cam_dev dev;

static void hal_camera_reset(void)
{
    dev.status = HAL_CAMERA_STATUS_INVALID;
    strncpy(dev.name, "Static sample images", sizeof(dev.name));
    dev.frame_width = 0;
    dev.frame_height = 0;
    dev.bytes_per_frame = 0;
    dev.format = HAL_CAMERA_COLOUR_FORMAT_INVALID;
    dev.mode = HAL_CAMERA_MODE_INVALID;
}

bool hal_camera_init(void)
{
    hal_camera_reset();
    info("Initialising camera interface: %s\n", dev.name);
    if (get_sample_n_elements() > 0) {
        return true;
    }
    return false;
}

bool hal_camera_configure(const uint32_t width,
                          const uint32_t height,
                          const hal_cam_mode mode,
                          const hal_cam_clr_format colour_format)
{
    if (width != get_sample_img_width() || height != get_sample_img_height()) {
        printf_err("Unsupported camera configuration\n");
        return false;
    }

    if (HAL_CAMERA_STATUS_RUNNING == dev.status) {
        printf_err("Camera is running; configuration failed\n");
        return false;
    }

    dev.frame_width = width;
    dev.frame_height = height;
    dev.mode = mode;
    dev.format = colour_format;
    dev.status = HAL_CAMERA_STATUS_STOPPED;

    switch (dev.format) {
        case HAL_CAMERA_COLOUR_FORMAT_RGB888:
            dev.bytes_per_frame = width * height * 3;
            break;
        case HAL_CAMERA_COLOUR_FORMAT_RGB565:
            dev.bytes_per_frame = width * height * 2;
            break;
        default:
            printf_err("Unsupported colour format\n");
            return false;
    }
    if (dev.bytes_per_frame != get_sample_img_total_bytes()) {
        hal_camera_reset();
        printf_err("Configuration failed due to size mismatch\n");
        return false;
    }
    return true;
}

bool hal_camera_set_buffer(uint8_t* buffer, const uint32_t size)
{
    UNUSED(buffer);
    if (size != dev.bytes_per_frame) {
        return false;
    }
    return true;
}

bool hal_camera_start(void)
{
    if (dev.status == HAL_CAMERA_STATUS_STOPPED) {
        dev.status = HAL_CAMERA_STATUS_RUNNING;
        return true;
    }
    return false;
}

const uint8_t* hal_camera_get_captured_frame(uint32_t* size)
{
    static uint32_t idx = 0;
    const uint8_t* buffer = NULL;
    *size = 0;
    if (dev.status == HAL_CAMERA_STATUS_RUNNING) {
        if (idx >= get_sample_n_elements()) {
#if defined(HAL_CAMERA_LOOP)
            idx = 0;
#else
            dev.status = HAL_CAMERA_STATUS_STOPPED;
            return buffer;
#endif /* HAL_CAMERA_LOOP */
        }

        info("Using sample image: %s\n", get_sample_data_filename(idx));
        buffer = get_sample_data_ptr(idx);
        *size = dev.bytes_per_frame;
        if (dev.mode == HAL_CAMERA_MODE_SINGLE_FRAME) {
            dev.status = HAL_CAMERA_STATUS_STOPPED;
        }
        ++idx;
    }
    return buffer;
}

bool hal_camera_stop(void)
{
    if (dev.status == HAL_CAMERA_STATUS_RUNNING) {
        dev.status = HAL_CAMERA_STATUS_STOPPED;
    }
    return true;
}

hal_cam_status hal_camera_get_status(void)
{
    return dev.status;
}

void hal_camera_release(void)
{
    hal_camera_reset();
}

const char* hal_camera_get_device_name(void)
{
    return dev.name;
}
