/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#include "lcd_img.h"

#include "log_macros.h"
#include "glcd.h"

#include <string.h>
#include <assert.h>

static int show_title(void)
{
    const char title[] = "Arm ML embedded code samples";
    lcd_set_text_color(White);
    return lcd_display_text(title, strlen(title), 10, 0, false);
}

int lcd_init(void)
{
    GLCD_Initialize();
    GLCD_Clear(Black);
    return show_title();
}

int lcd_display_image(const uint8_t* data, const uint32_t width,
    const uint32_t height, const uint32_t channels,
    const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t downsample_factor)
{
    /* Sanity checks */
    assert(data);
    if ((pos_x + width/downsample_factor > GLCD_WIDTH) ||
            (pos_y + height/downsample_factor > GLCD_HEIGHT)) {
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

int lcd_display_text(const char* str, const size_t str_sz,
    const uint32_t pos_x, const uint32_t pos_y,
    const bool allow_multiple_lines)
{
    /* We use a font 0 which is 9x15. */
    const uint32_t x_span =  9; /* Each character is this  9 pixels "wide". */
    const uint32_t y_span = 15; /* Each character is this 15 pixels "high". */

    if (str_sz == 0) {
        return 1;
    }

    /* If not within the LCD bounds, return error. */
    if (pos_x + x_span > GLCD_WIDTH || pos_y + y_span > GLCD_HEIGHT) {
        return 1;
    } else {
        const unsigned char font_idx = 0; /* We are using the custom font = 0 */

        const uint32_t col = pos_x/x_span;
        const uint32_t max_cols = GLCD_WIDTH/x_span - 1;
        const uint32_t max_lines = GLCD_HEIGHT/y_span - 1;

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

int lcd_display_box(const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t width, const uint32_t height, const uint16_t color)
{
    /* If not within the LCD bounds, return error. */
    if (pos_x > GLCD_WIDTH || pos_y > GLCD_HEIGHT) {
        return 1;
    }
    else {
        GLCD_Box(pos_x, pos_y, width, height, color);
    }
    return 0;
}

int lcd_clear(const uint16_t color)
{
    GLCD_Clear(color);
    GLCD_SetTextColor(White);
    return show_title();
}

int lcd_set_text_color(const uint16_t color)
{
    GLCD_SetTextColor(color);
    return 0;
}
