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
#ifndef HAL_AUDIO_H
#define HAL_AUDIO_H

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)

#include <stdint.h>
#include <stdbool.h>

/**< Audio device's operating modes */
typedef enum hal_audio_mode_ {
    HAL_AUDIO_MODE_SINGLE_BURST = 0,
    HAL_AUDIO_MODE_CONTINUOUS,
    HAL_AUDIO_MODE_INVALID
} hal_audio_mode;

/**< Data formats */
typedef enum hal_audio_data_format_ {
    HAL_AUDIO_FORMAT_16KHZ_MONO_16BIT    = (1 << 16 | 16000),
    HAL_AUDIO_FORMAT_16KHZ_STEREO_16BIT  = (2 << 16 | 16000),
    HAL_AUDIO_FORMAT_32KHZ_MONO_16BIT    = (1 << 16 | 32000),
    HAL_AUDIO_FORMAT_32KHZ_STEREO_16BIT  = (2 << 16 | 32000),
    HAL_AUDIO_FORMAT_48KHZ_MONO_16BIT    = (1 << 16 | 48000),
    HAL_AUDIO_FORMAT_48KHZ_STEREO_16BIT  = (2 << 16 | 48000),
    HAL_AUDIO_FORMAT_INVALID = -1,
} hal_audio_format;

#define GET_AUDIO_SAMPLING_RATE(mode) (uint32_t)(mode & 0xFFFF)
#define GET_AUDIO_NUM_CHANNELS(mode)  (uint32_t)((mode >> 16) & 0xFFFF)

/**< Device status */
typedef enum hal_audio_status_ {
    HAL_AUDIO_STATUS_ERROR = -1,
    HAL_AUDIO_STATUS_STOPPED = 0,
    HAL_AUDIO_STATUS_RUNNING,
    HAL_AUDIO_STATUS_INVALID
} hal_audio_status;

/**
* @brief  Initialises the audio interface.
* @return true if successful, false otherwise.
*/
bool hal_audio_init(void);

/**
 *  @brief Configure audio device.
 *  @param[in]  mode            Audio mode.
 *  @param[in]  stream_format   Audio stream data format.
 *  @return     true if successful, false otherwise.
 */
bool hal_audio_configure(hal_audio_mode mode,
                         hal_audio_format stream_format);

/**
 *  @brief Sets the audio buffer location
 *  @param[in]  buffer      Buffer to be used for populating audio data.
 *  @param[in]  size        Size of the buffer provided.
 *  @return     true if successful, false otherwise.
 */
bool hal_audio_set_buffer(uint8_t* buffer, const uint32_t size);

/**
 *  @brief Starts capturing data from audio device.
 *  @return     true if successful, false otherwise.
 */
bool hal_audio_start(void);

/**
 *  @brief Gets the captured audio data
 *  @param[out] n_elements Number of elements present in captured data.
 *  @return     Pointer to the 16-bit signed data.
 */
const int16_t* hal_audio_get_captured_frame(uint32_t* n_elements);

/**
 * @brief  Stops the audio device.
 * @return true if successful, false otherwise.
 */
bool hal_audio_stop(void);

/**
 * @brief  Gets audio device status.
 * @return Status represented as the status enumeration.
 */
hal_audio_status hal_audio_get_status(void);

/**
 * @brief  Releases the audio device.
 */
void hal_audio_release(void);

/**
 * @brief  Gets the human friendly name of the audio device.
 * @return Name expressed as a C string.
 */
const char* hal_audio_get_device_name(void);

#if defined(__cplusplus)
}
#endif // defined(__cplusplus)
#endif /* HAL_AUDIO_H */
