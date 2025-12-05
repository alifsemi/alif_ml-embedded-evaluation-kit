/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2025 Arm Limited and/or
 * its affiliates <open-source-office@arm.com>
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

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**< Image colour formats supported by this interface */
typedef enum hal_display_colour_format_ {
    HAL_DISPLAY_COLOUR_FORMAT_ARGB8888 = 0,
    HAL_DISPLAY_COLOUR_FORMAT_RGB888,
    HAL_DISPLAY_COLOUR_FORMAT_RGB565,
    HAL_DISPLAY_COLOUR_FORMAT_INVALID
} hal_display_clr_format;

typedef struct hal_display_device {
    const char name[16];
    uint32_t hor_res;
    uint32_t ver_res;
    uint32_t bytes_per_pixel;
    hal_display_clr_format colour_format;
    bool initialized;
} hal_display_dev;

/**
 * @brief   Initialise the LCD
 * @return  0 if successful, error code otherwise.
 **/
int hal_display_init(void);

/**
 * @brief     Configures the display
 * @param[in] width     Horizontal resolution
 * @param[in] height    Vertical resolution
 * @param[in] format    Colour format
 * @return 0 if successful, error code otherwise.
 */
int hal_display_configure(uint32_t width, uint32_t height, hal_display_clr_format format);

/**
 * @brief   Gets horizontal resolution of the display
 * @return  Width of display
 */
uint32_t hal_display_get_width(void);

/**
 * @brief   Gets vertical resolution of the display
 * @return  Height of display
 */
uint32_t hal_display_get_height(void);

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
int hal_display_show_image(const uint8_t* data, const uint32_t width,
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
int hal_display_show_text(const char* str, const size_t str_sz,
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
int hal_display_show_box(const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t width, const uint32_t height, const uint16_t color);

/**
 * @brief       Clear LCD.
 * @param[in]   color   Fill color.
 * @return      0 if successful, non-zero otherwise.
 **/
int hal_display_clear(const uint16_t color);

/**
 * @brief       Set text color.
 * @param[in]   color   Fill color.
 * @return      0 if successful, non-zero otherwise.
 **/
int hal_display_set_text_color(const uint16_t color);

#if defined(__cplusplus)
}
#endif

#endif /* LCD_IMG_H */
