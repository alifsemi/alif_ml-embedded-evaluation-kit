/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef LCD_IMG_H
#define LCD_IMG_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief   Initialise the LCD
 * @return  0 if successful, error code otherwise.
 **/
int lcd_init(void);

/**
 * @brief       Display a given image on the LCD. This allows displaying 8 bit
 *              single or multi-channel images on the LCD.
 * @param[in]   data        Pointer to start of the image.
 * @param[in]   width       Width of this image.
 * @param[in]   height      Image height.
 * @param[in]   channels    Number of channels.
 * @param[in]   pos_x       Screen position x co-ordinate.
 * @param[in]   pos_y       Screen position y co-ordinate.
 * @param[in]   downsample_factor   Factor by which the image needs to be
 *                                  downsampled.
 * @return      0 if successful, non-zero otherwise.
 **/
int lcd_display_image(const uint8_t* data, const uint32_t width,
    const uint32_t height, const uint32_t channels,
    const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t downsample_factor);

/**
 * @brief       Display a given image on the LCD. This allows displaying 8 bit
 *              single or multi-channel images on the LCD.
 * @param[in]   str         Pointer to a null terminated string.
 * @param[in]   str_sz      Length of the string.
 * @param[in]   pos_x       Screen position x co-ordinate.
 * @param[in]   pos_y       Screen position y co-ordinate.
 * @param[in]   allow_multiple_lines    The function will try and spread
 *                                      the string into multiple lines if
 *                                      they don't fit in one.
 * @return      0 if successful, non-zero otherwise.
 **/
int lcd_display_text(const char* str, const size_t str_sz,
     const uint32_t pos_x, const uint32_t pos_y,
     const bool allow_multiple_lines);

/**
 * @brief       Display a box with given color on LCD.
 * @param[in]   pos_x       Screen position x co-ordinate.
 * @param[in]   pos_y       Screen position y co-ordinate.
 * @param[in]   width       Width.
 * @param[in]   height      Height.
 * @param[in]   color       Fill color.
 * @return      0 if successful, non-zero otherwise.
 **/
int lcd_display_box(const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t width, const uint32_t height, const uint16_t color);

/**
 * @brief       Clear LCD.
 * @param[in]   color   Fill color.
 * @return      0 if successful, non-zero otherwise.
 **/
int lcd_clear(const uint16_t color);

/**
 * @brief       Set text color.
 * @param[in]   color   Fill color.
 * @return      0 if successful, non-zero otherwise.
 **/
int lcd_set_text_color(const uint16_t color);

#endif /* LCD_IMG_H */
