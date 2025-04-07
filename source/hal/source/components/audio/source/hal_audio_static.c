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
#include "hal_audio.h"
#include "log_macros.h"
#include "hal_audio_static_external.h"

#include <string.h>

typedef struct hal_audio_device_ {
    char name[32];
    uint32_t bytes_per_element;
    hal_audio_format format;
    hal_audio_mode mode;
    hal_audio_status status;
} hal_audio_dev;

static hal_audio_dev dev;

static void hal_audio_reset(void)
{
    dev.status = HAL_AUDIO_STATUS_INVALID;
    strncpy(dev.name, "Static sample audio files", sizeof(dev.name));
    dev.format = HAL_AUDIO_FORMAT_INVALID;
    dev.mode = HAL_AUDIO_MODE_INVALID;
}

bool hal_audio_init(void)
{
    hal_audio_reset();
    info("Initialising audio interface: %s\n", dev.name);
    if (get_sample_n_elements() > 0) {
        return true;
    }
    return false;
}

bool hal_audio_configure(const hal_audio_mode mode,
                         const hal_audio_format stream_format)
{
    if (!(HAL_AUDIO_STATUS_INVALID == dev.status || HAL_AUDIO_STATUS_STOPPED == dev.status)) {
        return false;
    }

    dev.mode = mode;
    dev.format = stream_format;
    dev.status = HAL_AUDIO_STATUS_STOPPED;
    dev.bytes_per_element = sizeof(int16_t); /* We only support 16-bit for this interface. */

    const uint32_t n_channels =  get_sample_audio_num_channels();
    const uint32_t sampling_freq = get_sample_sampling_frequency();

    if (GET_AUDIO_NUM_CHANNELS(stream_format) != n_channels ||
        GET_AUDIO_SAMPLING_RATE(stream_format) != sampling_freq) {
        printf_err("Unsupported audio format\n");
        return false;
    }
    return true;
}

bool hal_audio_set_buffer(uint8_t* buffer, const uint32_t size)
{
    UNUSED(buffer);
    UNUSED(size);
    return true;
}

bool hal_audio_start(void)
{
    if (dev.status == HAL_AUDIO_STATUS_STOPPED) {
        dev.status = HAL_AUDIO_STATUS_RUNNING;
        return true;
    }
    return false;
}

const int16_t* hal_audio_get_captured_frame(uint32_t* n_elements)
{
    static uint32_t idx = 0;
    const int16_t* buffer = NULL;
    *n_elements = 0;
    if (dev.status == HAL_AUDIO_STATUS_RUNNING) {
        if (idx >= get_sample_n_elements()) {
#if defined(HAL_AUDIO_LOOP)
            idx = 0;
#else
            dev.status = HAL_AUDIO_STATUS_STOPPED;
            return buffer;
#endif /* HAL_AUDIO_LOOP */
        }
        info("Using sample audio: %s\n", get_sample_data_filename(idx));
        buffer = get_sample_data_ptr(idx);
        *n_elements = get_sample_data_size(idx);
        if (dev.mode == HAL_AUDIO_MODE_SINGLE_BURST) {
            dev.status = HAL_AUDIO_STATUS_STOPPED;
        }

        ++idx;
    }
    return buffer;
}

bool hal_audio_stop(void)
{
    if (dev.status == HAL_AUDIO_STATUS_RUNNING) {
        dev.status = HAL_AUDIO_STATUS_STOPPED;
    }
    return true;
}

hal_audio_status hal_audio_get_status(void)
{
    return dev.status;
}

void hal_audio_release(void)
{
    hal_audio_reset();
}

const char* hal_audio_get_device_name(void)
{
    return dev.name;
}
