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

static hal_display_dev dev = {
    .name = "display stub",
    .hor_res = 0,
    .ver_res = 0,
    .bytes_per_pixel = 0,
    .colour_format = HAL_DISPLAY_COLOUR_FORMAT_INVALID,
    .initialized = false,
};

__attribute__((weak)) int hal_display_init(void)
{
    dev.initialized = true;
    return 0;
}

__attribute__((weak)) int hal_display_configure(uint32_t width,
                                                uint32_t height,
                                                hal_display_clr_format format)
{
    dev.hor_res = width;
    dev.ver_res = height;
    dev.colour_format = format;
    return 0;
}

__attribute__((weak)) uint32_t hal_display_get_width(void)
{
    return dev.hor_res;
}

__attribute__((weak)) uint32_t hal_display_get_height(void)
{
    return dev.ver_res;
}

__attribute__((weak)) int hal_display_show_image(
    const uint8_t* data, const uint32_t width,
    const uint32_t height, const uint32_t channels,
    const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t downsample_factor)
{
    UNUSED(data);
    UNUSED(pos_x);
    UNUSED(pos_y);
    UNUSED(width);
    UNUSED(height);
    UNUSED(channels);
    UNUSED(downsample_factor);
    debug("image display: (x, y, w, h) = "
        "(%" PRIu32 ", %" PRIu32 ", %" PRIu32 ", %" PRIu32 ")\n",
        pos_x, pos_y, width, height);
    debug("image display: channels = %" PRIu32 ", downsample factor = %" PRIu32 "\n",
        channels, downsample_factor);

    return 0;
}

__attribute__((weak)) int hal_display_show_text(const char* str, const size_t str_sz,
    const uint32_t pos_x, const uint32_t pos_y,
    const bool allow_multiple_lines)
{
    UNUSED(str);
    UNUSED(pos_x);
    UNUSED(pos_y);
    UNUSED(allow_multiple_lines);
    UNUSED(str_sz);
    debug("text display: %s\n", str);
    return 0;
}

__attribute__((weak)) int hal_display_show_box(const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t width, const uint32_t height, const uint16_t color)
{
    UNUSED(pos_x);
    UNUSED(pos_y);
    UNUSED(width);
    UNUSED(height);
    UNUSED(color);
    return 0;
}

__attribute__((weak)) int hal_display_clear(const uint16_t color)
{
    UNUSED(color);
    return 0;
}

__attribute__((weak)) int hal_display_set_text_color(const uint16_t color)
{
    UNUSED(color);
    return 0;
}
