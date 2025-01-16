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
#ifndef HAL_CAMERA_STATIC_EXTERNAL_H
#define HAL_CAMERA_STATIC_EXTERNAL_H

#include <stdint.h>

/** These functions must be provided to this interface */

/**
 * @brief       Gets the filename for the baked-in input array
 * @param[in]   idx     Index of the input.
 * @return      const C string pointer to the name.
 **/
const char* get_sample_data_filename(const uint32_t idx);

/**
 * @brief       Gets the pointer to image data.
 * @param[in]   idx     Index of the input.
 * @return      Pointer to the 8-bit unsigned integer data.
 **/
const uint8_t* get_sample_data_ptr(const uint32_t idx);

/**
 * @brief       Gets the total number of samples available.
 * @return      Number of samples.
 **/
uint32_t get_sample_n_elements(void);

/**
 * @brief       Gets the size of the input array.
 * @param[in]   idx     Index of the input.
 * @return      Size of the input array in bytes.
 **/
uint32_t get_sample_img_total_bytes(void);

/**
 * @brief       Gets the image width (pixels).
 * @return      Image width.
 **/
uint32_t get_sample_img_width(void);

/**
 * @brief       Gets the image height (pixels).
 * @return      Image height.
 **/
uint32_t get_sample_img_height(void);

#endif /* HAL_CAMERA_STATIC_EXTERNAL_H */
