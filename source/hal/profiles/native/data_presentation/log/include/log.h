/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
#ifndef NATIVE_LOG_H
#define NATIVE_LOG_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief  Data presentation initialiser
 **/
int log_psn_init(void);

/**
 * @brief       Log parameters for the image to be displayed.
 * @param[in]   data        Image pointer.
 * @param[in]   width       Image width.
 * @param[in]   height      Image height.
 * @param[in]   channels    Number of channels.
 * @param[in]   pos_x       Screen position x co-ordinate.
 * @param[in]   pos_y       Screen position y co-ordinate.
 * @param[in]   downsample_factor   Factor by which the image needs to be
 *                                  down-sampled.
 * @return      0 if successful, non-zero otherwise.
 **/

int log_display_image(uint8_t* data, const uint32_t width,
                      const uint32_t height, const uint32_t channels,
                      const uint32_t pos_x, const uint32_t pos_y,
                      const uint32_t downsample_factor);

/**
 * @brief       Log the parameters for text to be displayed.
 * @param[in]   str         Pointer to a null terminated string.
 * @param[in]   str_sz      Length of the string.
 * @param[in]   pos_x       Screen position x co-ordinate.
 * @param[in]   pos_y       Screen position y co-ordinate.
 * @param[in]   allow_multiple_lines  Specifies if multiple lines are allowed.
 * @return      0 if successful, non-zero otherwise.
 **/
int log_display_text(const char* str, const size_t str_sz,
                     const uint32_t pos_x, const uint32_t pos_y,
                     const bool allow_multiple_lines);

/**
 * @brief       Log parameters for the box to be displayed.
 * @param[in]   pos_x       Screen position x co-ordinate.
 * @param[in]   pos_y       Screen position y co-ordinate.
 * @param[in]   width       Width.
 * @param[in]   height      Height.
 * @param[in]   color       Fill color.
 * @return      0 if successful, non-zero otherwise.
 **/
int log_display_box_icon(const uint32_t pos_x, const uint32_t pos_y,
                         const uint32_t width, const uint32_t height, const uint16_t color);

/**
 * @brief       Logs the colour with which the display
 *              needs to be cleared with.
 * @param[in]   color       Fill color.
 * @return      0 if successful, non-zero otherwise.
 **/
int log_clear(const uint16_t color);

/**
 * @brief       Logs the text color to be set.
 * @param[in]   color       Fill color.
 * @return 0 if successful, non-zero otherwise.
 **/
int log_set_text_color (const uint16_t color);

#endif /* NATIVE_LOG_H */