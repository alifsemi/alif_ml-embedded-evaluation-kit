/*
 * SPDX-FileCopyrightText: Copyright 2022-2025 Arm Limited and/or
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
#include "hal_display.h"
#include "hal_log.h"
#include "glcd.h"

#include <string.h>
#include <assert.h>

static hal_display_dev dev = {
    .name = "MPS3 GLCD",
    .hor_res = 0,
    .ver_res = 0,
    .bytes_per_pixel = 0,
    .colour_format = HAL_DISPLAY_COLOUR_FORMAT_INVALID,
    .initialized = false,
};

static hal_display_dev* get_display_device(void)
{
    return &dev;
}

static int show_title(void)
{
    int ret = 0;
#if defined(DISPLAY_TITLE_STRING)
    const char *title = DISPLAY_TITLE_STRING;
    if (get_display_device()->initialized) {
        hal_display_set_text_color(White);
        ret = hal_display_show_text(title, strlen(title), 10, 0, false);
    }
#endif /* DISPLAY_TITLE_STRING */
    return ret;
}

int hal_display_init(void)
{
    GLCD_Initialize();
    get_display_device()->initialized = true;
    hal_display_configure(GLCD_WIDTH, GLCD_HEIGHT, HAL_DISPLAY_COLOUR_FORMAT_RGB565);
    GLCD_Clear(Black);
    return show_title();
}

int hal_display_configure(uint32_t width, uint32_t height, hal_display_clr_format format)
{
    hal_display_dev* dev = get_display_device();
    if (!dev->initialized) {
        return -1;
    }

    dev->hor_res = width;
    dev->ver_res = height;

    char fmt_str[16];
    memset(fmt_str, 0, sizeof(fmt_str));

    switch (format) {
        case HAL_DISPLAY_COLOUR_FORMAT_RGB565:
            strncpy(fmt_str, "RGB565", sizeof(fmt_str) - 1);
            dev->bytes_per_pixel = 2;
            break;
        case HAL_DISPLAY_COLOUR_FORMAT_RGB888:
            strncpy(fmt_str, "RGB888", sizeof(fmt_str) - 1);
            dev->bytes_per_pixel = 3;
            break;
        case HAL_DISPLAY_COLOUR_FORMAT_ARGB8888:
            strncpy(fmt_str, "ARGB8888", sizeof(fmt_str) - 1);
            dev->bytes_per_pixel = 4;
            break;
        default:
            printf_err("Unsupported colour format\n");
            return -3;
    }

    dev->colour_format = format;
    dev->initialized = true;

    info("Device %s configured\n", dev->name);
    info("Resolution:    %" PRIu32 " x %" PRIu32 "\n", dev->hor_res, dev->ver_res);
    info("Colour format: %s\n", fmt_str);
    return 0;
}

uint32_t hal_display_get_width(void)
{
    hal_display_dev* dev = get_display_device();
    if (dev->initialized) {
        return dev->hor_res;
    }
    return 0;
}

uint32_t hal_display_get_height(void)
{
    hal_display_dev* dev = get_display_device();
    if (dev->initialized) {
        return dev->ver_res;
    }
    return 0;
}

int hal_display_show_image(const uint8_t* data, const uint32_t width,
    const uint32_t height, const uint32_t channels,
    const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t downsample_factor)
{
    /* Health checks */
    hal_display_dev* dev = get_display_device();
    assert(dev->initialized);
    if ((pos_x + width/downsample_factor > dev->hor_res) ||
            (pos_y + height/downsample_factor > dev->ver_res)) {
        printf_err("Invalid image size for given location!\n");
        return 1;
    }

    if (1 == channels || 3 == channels) {
        GLCD_Image(data, width, height, channels, pos_x, pos_y,
            downsample_factor);
    } else {
        printf_err("Only single and three channel images are supported!\n");
        return 1;
    }

    return 0;
}

int hal_display_show_text(const char* str, const size_t str_sz,
    const uint32_t pos_x, const uint32_t pos_y,
    const bool allow_multiple_lines)
{
    hal_display_dev* dev = get_display_device();
    assert(dev->initialized);

    /* We use a font 0 which is 9x15. */
    const uint32_t x_span =  9; /* Each character is this  9 pixels "wide". */
    const uint32_t y_span = 15; /* Each character is this 15 pixels "high". */

    if (str_sz == 0) {
        return 1;
    }

    /* If not within the LCD bounds, return error. */
    if (pos_x + x_span > dev->hor_res || pos_y + y_span > dev->ver_res) {
        return 1;
    } else {
        const unsigned char font_idx = 0; /* We are using the custom font = 0 */

        const uint32_t col = pos_x/x_span;
        const uint32_t max_cols = dev->hor_res/x_span - 1;
        const uint32_t max_lines = dev->ver_res/y_span - 1;

        uint32_t i = 0;
        uint32_t current_line = pos_y/y_span;
        uint32_t current_col = col;

        /* Display the string on the LCD. */
        for (i = 0; i < str_sz; ++i) {

            if (allow_multiple_lines) {

                /* If the next character won't fit. */
                if (current_col > max_cols) {
                    current_col = col;

                    /* If the next line won't fit. */
                    if (++current_line  > max_lines) {
                        return 1;
                    }
                }
            }

            GLCD_DisplayChar(current_line, current_col++, font_idx, str[i]);
        }
    }
    return 0;
}

int hal_display_show_box(const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t width, const uint32_t height, const uint16_t color)
{
    hal_display_dev* dev = get_display_device();
    assert(dev->initialized);

    /* If not within the LCD bounds, return error. */
    if (pos_x > dev->hor_res || pos_y > dev->ver_res) {
        return 1;
    }
    GLCD_Box(pos_x, pos_y, width, height, color);
    return 0;
}

int hal_display_clear(const uint16_t color)
{
    assert(get_display_device()->initialized);
    GLCD_Clear(color);
    GLCD_SetBackColor(color);
    GLCD_SetTextColor(White);
    return show_title();
}

int hal_display_set_text_color(const uint16_t color)
{
    assert(get_display_device()->initialized);
    GLCD_SetTextColor(color);
    return 0;
}
