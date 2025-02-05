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
#ifndef HAL_AUDIO_STATIC_EXTERNAL_H
#define HAL_AUDIO_STATIC_EXTERNAL_H

#include <stdint.h>

/** These functions must be provided to this interface */
/**
 * @brief       Gets the filename for the baked-in input array
 * @param[in]   idx     Index of the input.
 * @return      const C string pointer to the name.
 **/
const char* get_sample_data_filename(const uint32_t idx);

/**
 * @brief       Gets the pointer to audio data.
 * @param[in]   idx     Index of the input.
 * @return      Pointer to 16-bit signed integer data.
 **/
const int16_t* get_sample_data_ptr(const uint32_t idx);

/**
 * @brief       Gets the size of the input array.
 * @param[in]   idx     Index of the input.
 * @return      Size of the input array in bytes.
 **/
uint32_t get_sample_data_size(const uint32_t idx);

/**
 * @brief       Gets the total number of samples available.
 * @return      Number of samples.
 **/
uint32_t get_sample_n_elements(void);

/**
 * @brief       Gets the audio sampling frequency used to generate
 *              all the files.
 * @return      Sampling frequency as an integer.
 */
uint32_t get_sample_sampling_frequency(void);

/**
 * @brief       Gets the number of audio channels for every file.
 * @return      Number of channels as an integer.
 */
uint32_t get_sample_audio_num_channels(void);

#endif /* HAL_AUDIO_STATIC_EXTERNAL_H */
