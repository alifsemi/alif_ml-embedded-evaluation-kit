/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <inttypes.h>
#include <stdlib.h>

#include "lv_port.h"
#include "lvgl.h"
#include "glcd.h"

static lv_obj_t *canvas;
static lv_draw_rect_dsc_t rect_dsc;
static lv_draw_label_dsc_t text_dsc;

void GLCD_Initialize(void)
{
    lv_port_disp_init();
#if LV_COLOR_DEPTH != 16
#error "LV_COLOR_DEPTH must be set to 16 to use GLCD emulation"
#endif
    uint32_t lv_lock_state = lv_port_lock();
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    canvas = lv_canvas_create(lv_scr_act());
    static LV_ATTRIBUTE_LARGE_RAM_ARRAY uint32_t buffer[LV_CANVAS_BUF_SIZE_TRUE_COLOR(GLCD_WIDTH, GLCD_HEIGHT) / sizeof(uint32_t)];
    lv_canvas_set_buffer(canvas, buffer, GLCD_WIDTH, GLCD_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
	lv_draw_rect_dsc_init(&rect_dsc);
	lv_draw_label_dsc_init(&text_dsc);
	extern const lv_font_t lv_font_9x15;
	text_dsc.font = &lv_font_9x15;
	// Use selection to draw the background
	text_dsc.sel_start = 0;
	text_dsc.sel_end = 0x7fff;
	text_dsc.sel_bg_color.full = Black;
	text_dsc.sel_color.full = White;
	lv_port_unlock(lv_lock_state);
}

void GLCD_Clear(unsigned short color)
{
    if (!canvas) {
        return;
    }
    uint32_t lv_lock_state = lv_port_lock();
    lv_canvas_fill_bg(canvas, (lv_color_t) { .full = color }, LV_OPA_COVER);
    lv_port_unlock(lv_lock_state);
}

/**
 * @brief       Converts a gray value to RGB565 representation.
 * @param[in]   src_uchar   Pointer to the source pixel.
 * @return      16 bit RGB565 value.
 */
static inline lv_color_t _GLCD_Gray8_to_RGB565(uint8_t *src_uchar)
{
    return lv_color_make(*src_uchar,  *src_uchar,  *src_uchar);
}

/**
 * @brief       Converts an RGB888 value to RGB565 representation.
 * @param[in]   src_uchar   Pointer to the source pixel for R (assumed to
 *                          be RGB format).
 * @return      16 bit RGB565 value.
 */
static inline lv_color_t _GLCD_RGB888_to_RGB565(uint8_t *src_uchar)
{
    return lv_color_make(src_uchar[0], src_uchar[1], src_uchar[2]);
}

/* Helper typedef to encapsulate the colour conversion function
 * signatures */
typedef lv_color_t (* std_clr_2_lcd_clr_fn)(uint8_t *src_uchar);

void GLCD_Image(const void *data, const uint32_t width,
    const uint32_t height, const uint32_t channels,
    const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t downsample_factor)
{
    if (!canvas) {
        return;
    }

    uint32_t i, j = 0; /* for loops */
    const uint32_t x_incr = channels * downsample_factor; /* stride. */
    const uint32_t y_incr = channels * width * (downsample_factor - 1); /* skip rows. */
    uint8_t* src_unsigned = (uint8_t *)data; /* temporary pointer. */
    std_clr_2_lcd_clr_fn cvt_clr_fn = 0; /* colour conversion function. */

    /* Based on number of channels, we decide which of the above functions to use. */
    switch (channels) {
        case 1:
            cvt_clr_fn = _GLCD_Gray8_to_RGB565;
            break;

        case 3:
            cvt_clr_fn = _GLCD_RGB888_to_RGB565;
            break;

        default:
            abort();
    }

    uint32_t lv_lock_state = lv_port_lock();
    /* Loop over the image. */
    uint32_t dst_y = pos_y;
    for (j = height; j != 0; j -= downsample_factor) {
        uint32_t dst_x = pos_x;
        for (i = width; i != 0; i -= downsample_factor) {
            lv_canvas_set_px_color(canvas, dst_x, dst_y, cvt_clr_fn(src_unsigned));
            src_unsigned += x_incr;
            dst_x++;
        }

        /* Skip rows if needed. */
        src_unsigned += y_incr;
        dst_y++;
    }
    lv_port_unlock(lv_lock_state);
}

void GLCD_DisplayString(
        unsigned int ln, unsigned int col,
        unsigned char fi, char *s)
{
    if (!canvas) {
        return;
    }

    switch (fi) {
    case 0: ;
        uint32_t lv_lock_state = lv_port_lock();
        lv_canvas_draw_text(canvas, col * 9, ln * 15, col * 9, &text_dsc, s);
        lv_port_unlock(lv_lock_state);
        break;
    }
}

void GLCD_DisplayChar(
        unsigned int ln, unsigned int col,
        unsigned char fi, unsigned char c)
{
    char txt[2] = { c, '\0' };
    GLCD_DisplayString(ln, col, fi, txt);
}

void GLCD_Box(
        unsigned int x, unsigned int y,
        unsigned int w, unsigned int h,
        unsigned short color)
{
    if (!canvas) {
        return;
    }
	rect_dsc.bg_color.full = color;
	uint32_t lv_lock_state = lv_port_lock();
	lv_canvas_draw_rect(canvas, x, y, w, h, &rect_dsc);
	lv_port_unlock(lv_lock_state);
}

void GLCD_SetTextColor(unsigned short color)
{
    text_dsc.sel_color.full = color;
}

void GLCD_SetBGColor(unsigned short color)
{
    text_dsc.sel_bg_color.full = color;
}
