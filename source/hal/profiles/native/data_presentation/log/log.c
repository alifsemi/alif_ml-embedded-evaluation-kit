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
#include "log.h"
#include "log_macros.h"

#include <stdint.h>

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

int log_psn_init(void)
{
    return 0;
}

int log_display_image(uint8_t* data, const uint32_t width,
                      const uint32_t height, const uint32_t channels,
                      const uint32_t pos_x, const uint32_t pos_y,
                      const uint32_t downsample_factor)
{
    debug("Image details\n");
    debug("Data:                 %p\n", data);
    debug("WxHxC:                %dx%dx%d\n", width, height, channels);
    debug("Pos (x,y):            (%d,%d)\n", pos_x, pos_y);
    debug("Downsampling factor:  %u\n", downsample_factor);
    return 0;
}

int log_display_text(const char* str, const size_t str_sz,
                     const uint32_t pos_x, const uint32_t pos_y,
                     const bool allow_multiple_lines)
{
    UNUSED(allow_multiple_lines);
    debug("%s\n", str);
    debug("Text size: %lu, x: %d, y: %d\n", str_sz, pos_x, pos_y);
    return 0;
}


int log_display_box_icon(const uint32_t pos_x, const uint32_t pos_y,
                         const uint32_t width, const uint32_t height,
                         const uint16_t color)
{
    debug("Showing rectangular, width: %d, height: %d, color: %d, x: %d, y: %d\n",
            width, height, color, pos_x, pos_y);
    return 0;
}

int log_clear(const uint16_t color)
{
    debug("Clearing with color: %d\n", color);
    return 0;
}

int log_set_text_color (const uint16_t color)
{
    debug("Setting text color: %d\n", color);
    return 0;
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
