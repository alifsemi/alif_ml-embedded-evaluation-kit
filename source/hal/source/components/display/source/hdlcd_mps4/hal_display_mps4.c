/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or
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
#include "font_9x15_h.h"

#include "hal_log.h"

#include <inttypes.h>
#include <string.h>
#include <assert.h>

#if !defined(__arm__) || (__ARM_ARCH_PROFILE != 77)
#error "This file writes to a fixed address (compile-time determined) meant for MCU targets."
#endif /* !defined(__arm__) || (__ARM_ARCH_PROFILE != 77) */

#if !defined(HDLCD_FRAME_BUFFER_BASE_ADDRESS) | \
    !defined(HDLCD_BYTES_PER_PIXEL) | \
    !defined(HDLCD_RES_WIDTH) | \
    !defined(HDLCD_RES_HEIGHT)
#error "Base address, pixel width, height and width should be defined."
#endif /* required definition check. */

#if (HDLCD_BYTES_PER_PIXEL == 4) || (HDLCD_BYTES_PER_PIXEL == 3)
struct pixel_value {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
#if HDLCD_BYTES_PER_PIXEL == 4
    uint8_t alpha;
#endif
} __attribute__((packed));
#else /* (HDLCD_BYTES_PER_PIXEL == 4) || (HDLCD_BYTES_PER_PIXEL == 3) */
#error "Only RGB24 and RGB32 are supported."
#endif


/**
* Text and background colour
*/
#define BG_COLOUR  0    /* Background colour */
#define TXT_COLOUR 1    /* Text colour */

static volatile struct pixel_value s_colour[] = {
    {
        .red = 0,
        .green = 0,
        .blue = 0,
#if HDLCD_BYTES_PER_PIXEL == 4
        .alpha = 0
#endif
    },
    {
        .red = 0xff,
        .green = 0xff,
        .blue = 0xff,
#if HDLCD_BYTES_PER_PIXEL == 4
        .alpha = 0
#endif
    },
};

static hal_display_dev dev = {
    .name = "MPS4 HDLCD",
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
        hal_display_set_text_color(0xFFFF);
        ret = hal_display_show_text(title, strlen(title), 10, 0, false);
    }
#endif /* DISPLAY_TITLE_STRING */
    return ret;
}

int hal_display_init(void)
{
    get_display_device()->initialized = true;
#if (4 == HDLCD_BYTES_PER_PIXEL)
    const hal_display_clr_format fmt = HAL_DISPLAY_COLOUR_FORMAT_ARGB8888;
#elif (3 == HDLCD_BYTES_PER_PIXEL)
    const hal_display_clr_format fmt = HAL_DISPLAY_COLOUR_FORMAT_RGB888;
#endif /* HDLCD_BYTES_PER_PIXEL */
    hal_display_configure(HDLCD_RES_WIDTH, HDLCD_RES_HEIGHT, fmt);
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
    debug("image display: (x, y, w, h) = "
        "(%" PRIu32 ", %" PRIu32 ", %" PRIu32 ", %" PRIu32 ")\n",
        pos_x, pos_y, width, height);
    debug("image display: channels = %" PRIu32 ", downsample factor = %" PRIu32 "\n",
        channels, downsample_factor);

    const uint32_t line_stride = HDLCD_RES_WIDTH * HDLCD_BYTES_PER_PIXEL;
    uint8_t* dst = (uint8_t*)(HDLCD_FRAME_BUFFER_BASE_ADDRESS);
    uint8_t* src = (uint8_t*)data;

    if (channels <= HDLCD_BYTES_PER_PIXEL) {
        for (uint32_t j = 0; j < height/downsample_factor; ++j) {
            uint8_t* dst_row = dst + ((j + pos_y) * line_stride);
            uint8_t* src_row = src + (j * width * channels * downsample_factor);
            for (uint32_t i = 0; i < width/downsample_factor; ++i) {
                memcpy(dst_row + ((pos_x + i) * HDLCD_BYTES_PER_PIXEL),
                   src_row + (i * channels * downsample_factor),
                   channels);
            }
        }
    } else {
        printf_err("Unsupported number of channels %" PRIu32 "\n", channels);
        return -1;
    }
    return 0;
}

/**
 * @brief       Gets the pixel values (RGB) from RGB16 value
 * @param[in]   color   16-bit value representing RGB656
 * @return      colour expressed as `struct pixel_value`.
 */
static struct pixel_value get_color_from_rgb565(unsigned short color)
{
    struct pixel_value tmp = {
        .red =   ((color >> 11) & 0x1F) << 3,
        .green = ((color >>  5) & 0x3F) << 2,
        .blue =  ((color >>  0) & 0x1F) << 3,
#if HDLCD_BYTES_PER_PIXEL == 4
        .alpha = 0
#endif
    };
    return tmp;
}

/**
 * @brief     Draws a character to framebuffer.
 * @param[in] x     Horizontal position to draw from.
 * @param[in] y     Vertical position to draw from.
 * @param[in] cw    Character width (depends on the font).
 * @param[in] ch    Character width (depends on the font).
 * @param[in] c     Character to be drawn.
 */
static void draw_char(
        unsigned int x, unsigned int y,
        unsigned int cw, unsigned int ch,
        unsigned char *c)
{
    unsigned int i, j, k, pixs;

    /* Health check: out of bounds? */
    if ((x + cw) > HDLCD_RES_WIDTH || (y + ch) > HDLCD_RES_HEIGHT) {
        return;
    }

    uint8_t* dst = (uint8_t*)(HDLCD_FRAME_BUFFER_BASE_ADDRESS) +
        (y * HDLCD_RES_WIDTH * HDLCD_BYTES_PER_PIXEL) +
        (x * HDLCD_BYTES_PER_PIXEL);
    struct pixel_value* dst_pixel = (struct pixel_value*)dst;

    k  = (cw + 7)/8;

    if (k == 1) {
        for (j = 0; j < ch; ++j) {
            pixs = *(unsigned char  *)c;
            c += 1;
            for (i = 0; i < cw; ++i) {
                 dst_pixel[i] = s_colour[(pixs >> i) & 1];
            }
            dst_pixel += HDLCD_RES_WIDTH;
        }
    }
    else if (k == 2) {
        for (j = 0; j < ch; ++j) {
            pixs = *(unsigned short *)c;
            c += 2;
            for (i = 0; i < cw; ++i) {
                dst_pixel[i] = s_colour[(pixs >> i) & 1];
            }
            dst_pixel += HDLCD_RES_WIDTH;
        }
    }
}

/**
 * @brief     Renders a character on framebuffer.
 * @param[in] ln    Line number (depends on the font).
 * @param[in] col   Horizontal position to draw from.
 * @param[in] fi    Font index to use.
 * @param[in] c     Character to be drawn.
 */
static void render_char(
        unsigned int ln, unsigned int col,
        unsigned char fi, unsigned char c)
{
    c -= 32;
    switch (fi) {
    case 0: /* Font 9 x 15. */
        draw_char(col * 9, ln * 15, 9, 15, (unsigned char *)&Font_9x15_h[c * 15]);
        break;
    }
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

            render_char(current_line, current_col++, font_idx, str[i]);
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

    /** TODO: Draw lines for a box */
    UNUSED(width);
    UNUSED(height);
    UNUSED(color);
    return 0;
}

int hal_display_clear(const uint16_t color)
{
    struct pixel_value c = get_color_from_rgb565(color);
    s_colour[BG_COLOUR] = c;

    struct pixel_value * dst = (struct pixel_value *)(HDLCD_FRAME_BUFFER_BASE_ADDRESS);
    for (uint32_t j = 0; j < HDLCD_RES_WIDTH * HDLCD_RES_HEIGHT; ++j) {
        *dst++ = c;
    }

    return show_title();
}

int hal_display_set_text_color(const uint16_t color)
{
    assert(get_display_device()->initialized);
    s_colour[TXT_COLOUR] = get_color_from_rgb565(color);
    return 0;
}
