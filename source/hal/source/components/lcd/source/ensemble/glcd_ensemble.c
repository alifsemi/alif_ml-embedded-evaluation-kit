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

#include "RTE_Device.h"
#include "RTE_Components.h"

#include CMSIS_device_header
//#include "cmsis.h"                  /* device specific header file    */
#include "peripheral_irqs.h"      /* peripheral memory map definitions */
#include "peripheral_memmap.h"      /* peripheral memory map definitions */

#include "Driver_PINMUX_AND_PINPAD.h"
#include "Driver_GPIO.h"
#include "base_def.h"
#include "system_utils.h"
#include "image_processing.h"
#include "image_buff.h"
#include "LCD_panel.h"
#include "lvgl.h"
#include "delay.h"

#if defined __ARMCC_VERSION && (__ARM_FEATURE_MVE & 1)
#define ENABLE_MVE_WRITE 1
#else
#define ENABLE_MVE_WRITE 0
#endif

#if __ARM_FEATURE_MVE & 1
#include <arm_mve.h>
#endif

extern void lv_port_disp_init(void);
extern ARM_DRIVER_GPIO Driver_GPIO1;

// GLCD_XXX methods taken from stubs_ensemble.c

#define XOFFS 16
#define YOFFS 16

void SetupLEDs()
{
	// Green LED
	Driver_GPIO1.Initialize(PIN_NUMBER_15,NULL);
	Driver_GPIO1.PowerControl(PIN_NUMBER_15,  ARM_POWER_FULL);
	Driver_GPIO1.SetValue(PIN_NUMBER_15, GPIO_PIN_OUTPUT_STATE_LOW);
	Driver_GPIO1.SetDirection(PIN_NUMBER_15, GPIO_PIN_DIRECTION_OUTPUT);
	PINMUX_Config (PORT_NUMBER_1, PIN_NUMBER_15, PINMUX_ALTERNATE_FUNCTION_0);

	// Red LED
	Driver_GPIO1.Initialize(PIN_NUMBER_14,NULL);
	Driver_GPIO1.PowerControl(PIN_NUMBER_14,  ARM_POWER_FULL);
	Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_LOW);
	Driver_GPIO1.SetDirection(PIN_NUMBER_14, GPIO_PIN_DIRECTION_OUTPUT);
	PINMUX_Config (PORT_NUMBER_1, PIN_NUMBER_14, PINMUX_ALTERNATE_FUNCTION_0);
}

#if !(LV_COLOR_DEPTH == 16 || LV_COLOR_DEPTH == 32)
#error "Unsupported LV_COLOR_DEPTH"
#endif

#ifndef USE_LVGL_ZOOM
static void write_to_lvgl_buf_doubled(
		const uint8_t src[static restrict MIMAGE_Y][MIMAGE_X][RGB_BYTES],
		lv_color_t dst[static restrict LIMAGE_Y][LIMAGE_X])
{
	for (uint32_t y1 = 0; y1 < MIMAGE_Y; y1++) {
#define SRC_ROW_OFFSET_32 ((MIMAGE_X * RGB_BYTES) / 4)

#if ENABLE_MVE_WRITE && LV_COLOR_DEPTH == 32
#if MIMAGE_X % 16
#error "bad alignment"
#endif
		const uint32x4_t inc12 = vmulq_n_u32(vidupq_n_u32(0, 4), RGB_BYTES);
		const uint32x4_t incout = vmulq_n_u32(vidupq_n_u32(0, 4), 2 * LV_COLOR_DEPTH / 8);
		const uint32_t *restrict srcp32 = (const uint32_t *) src[y1][0];
		uint32_t *restrict dstp32 = (uint32_t *) dst[y1 * 2][0];
		uint32_t *restrict dst2p32 = (uint32_t *) dst[y1 * 2 + 1][0];
		for (uint32_t x1 = 0; x1 < MIMAGE_X; x1 += 4 * 4)
		{
			uint32x4_t r1b0g0r0 = vldrwq_gather_offset(srcp32 + 0, inc12);
			uint32x4_t r0g0b0r1 = vreinterpretq_u32(vrev32q_u8(vreinterpretq_u8(r1b0g0r0)));
			uint32x4_t xxr0g0b0 = vorrq_n_u32(vshrq_n_u32(r0g0b0r1, 8), 0xff000000);
			vstrwq_scatter_offset(dstp32, incout, xxr0g0b0);
			vstrwq_scatter_offset(dstp32 + 1, incout, xxr0g0b0);
			vstrwq_scatter_offset(dst2p32, incout, xxr0g0b0);
			vstrwq_scatter_offset(dst2p32 + 1, incout, xxr0g0b0);
			uint32x4_t g2r2b1g1 = vldrwq_gather_offset(srcp32 + 1, inc12);
			uint32x4_t g1b1r2g2 = vreinterpretq_u32(vrev32q_u8(vreinterpretq_u8(g2r2b1g1)));
			uint32x4_t xxr1g1b1 = vorrq_n_u32(vsriq_n_u32(vshlq_n_u32(r0g0b0r1, 16), g1b1r2g2, 16), 0xff000000);
			vstrwq_scatter_offset(dstp32 + 2, incout, xxr1g1b1);
			vstrwq_scatter_offset(dstp32 + 3, incout, xxr1g1b1);
			vstrwq_scatter_offset(dst2p32 + 2, incout, xxr1g1b1);
			vstrwq_scatter_offset(dst2p32 + 3, incout, xxr1g1b1);
			uint32x4_t b3g3r3b2 = vldrwq_gather_offset(srcp32 + 2, inc12);
			uint32x4_t b2r3g3b3 = vreinterpretq_u32(vrev32q_u8(vreinterpretq_u8(b3g3r3b2)));
			uint32x4_t xxr2g2b2 = vorrq_n_u32(vsriq_n_u32(vshlq_n_u32(g1b1r2g2, 8), b2r3g3b3, 24), 0xff000000);
			vstrwq_scatter_offset(dstp32 + 4, incout, xxr2g2b2);
			vstrwq_scatter_offset(dstp32 + 5, incout, xxr2g2b2);
			vstrwq_scatter_offset(dst2p32 + 4, incout, xxr2g2b2);
			vstrwq_scatter_offset(dst2p32 + 5, incout, xxr2g2b2);
			uint32x4_t xxr3g3b3 = vorrq_n_u32(b2r3g3b3, 0xff000000);
			vstrwq_scatter_offset(dstp32 + 6, incout, xxr3g3b3);
			vstrwq_scatter_offset(dstp32 + 7, incout, xxr3g3b3);
			vstrwq_scatter_offset(dst2p32 + 6, incout, xxr3g3b3);
			vstrwq_scatter_offset(dst2p32 + 7, incout, xxr3g3b3);
			dstp32 += 4 * 2 * RGBA_BYTES;
			dst2p32 += 4 * 2 * RGBA_BYTES;
			srcp32 += 4 * RGB_BYTES;
		}
#elif 1
#if MIMAGE_X % 4 || LIMAGE_X % 4
#error "bad alignment"
#endif
		const uint8_t * restrict srcp = src[y1][0];
		lv_color_t * restrict dstp = dst[y1 * 2];
		lv_color_t * restrict dst2p = dst[y1 * 2 + 1];
		const uint32_t *srcp32 = (const uint32_t *)srcp;
		uint32_t *dstp32 = (uint32_t *)dstp;
		// Load 4 pixels as 3 words, and expand to 8 words, on two rows
		for (uint32_t x1 = 0; x1 < MIMAGE_X; x1 += 4)
		{
#if LV_COLOR_DEPTH == 32
			uint32_t r0g0b0r1 = __REV(*srcp32++);
			uint32_t xxr0g0b0 = (r0g0b0r1 >> 8) | 0xff000000;
			*dstp32++ = xxr0g0b0;
			*dstp32++ = xxr0g0b0;
			uint32_t g1r1b2g2 = __REV(*srcp32++);
			uint32_t xxr1g1b1 = (r0g0b0r1 << 16) | (g1r1b2g2 >> 16) | 0xff000000;
			*dstp32++ = xxr1g1b1;
			*dstp32++ = xxr1g1b1;
			uint32_t b2r3g3b3 = __REV(*srcp32++);
			uint32_t xxr2g2b2 = (g1r1b2g2 << 8) | (b2r3g3b3 >> 24) | 0xff000000;
			*dstp32++ = xxr2g2b2;
			*dstp32++ = xxr2g2b2;
			uint32_t xxr3g3b3 = b2r3g3b3 | 0xff000000;
			*dstp32++ = xxr3g3b3;
			*dstp32++ = xxr3g3b3;
#else
			uint32_t r1b0g0r0 = *srcp32++;
			uint32_t g2r2b1g1 = *srcp32++;
			uint32_t b3g3r3b2 = *srcp32++;
			uint32_t p;
			p = ((r1b0g0r0 << 8)  & 0x0000f800) |
				((r1b0g0r0 >> 5)  & 0x000007e0) |
				((r1b0g0r0 >> 19) & 0x0000001f);
			*dstp32++ = (p << 16) | p;
			p = (r1b0g0r0         & 0xf8000000) |
				((g2r2b1g1 << 19) & 0x07e00000) |
				((g2r2b1g1 << 5)  & 0x001f0000);
			*dstp32++ = p | (p >> 16);
			p = ((g2r2b1g1 >> 8)  & 0x0000f800) |
				((g2r2b1g1 >> 21) & 0x000007e0) |
				((b3g3r3b2 >> 3)  & 0x0000001f);
			*dstp32++ = (p << 16) | p;
			p = ((b3g3r3b2 << 16) & 0xf8000000) |
				((b3g3r3b2 << 3)  & 0x07e00000) |
				((b3g3r3b2 >> 11) & 0x001f0000);
			*dstp32++ = p | (p >> 16);
#endif
		}
		// Memcpy the second row rather than doing it as-we-go, because as-we-go makes
		// the above too complex for the autovectoriser.
		memcpy(dst2p, dstp, 2 * MIMAGE_X * RGBA_BYTES);
#else
		for (uint32_t x1 = 0; x1 < MIMAGE_X; x1++) {
			uint8_t r, g, b;
			int32_t x, y;

			r = src[y1][x1][0];
			g = src[y1][x1][1];
			b = src[y1][x1][2];

			x = XOFFS + (x1 << 1);
			y = YOFFS + (y1 << 1);

			lv_color_t c = lv_color_make(r, g, b);
			dst[y][x] = c;
			dst[y][x+1] = c;
			dst[y+1][x] = c;
			dst[y+1][x+1] = c;
		}
#endif
	}
}

#else // USE_LVGL_ZOOM

static void write_to_lvgl_buf(
		const uint8_t src[static restrict MIMAGE_Y][MIMAGE_X][RGB_BYTES],
		lv_color_t dst[static restrict MIMAGE_Y][MIMAGE_X])
{
	for (uint32_t y = 0; y < MIMAGE_Y; y++) {
#if ENABLE_MVE_WRITE
#if LV_COLOR_DEPTH == 32
#if MIMAGE_X % 16
#error "bad alignment"
#endif
		const uint32x4_t inc12 = vmulq_n_u32(vidupq_n_u32(0, 4), 3);
		const uint32_t *restrict srcp32 = (const uint32_t *) src[y][0];
		uint32_t *restrict dstp32 = &dst[y].full;
		for (uint32_t x1 = 0; x1 < MIMAGE_X; x1 += 4 * 4)
		{
			uint32x4_t r1b0g0r0 = vldrwq_gather_offset(srcp32 + 0, inc12);
			uint32x4_t r0g0b0r1 = vreinterpretq_u32(vrev32q_u8(vreinterpretq_u8(r1b0g0r0)));
			uint32x4_t g2r2b1g1 = vldrwq_gather_offset(srcp32 + 1, inc12);
			uint32x4_t g1b1r2g2 = vreinterpretq_u32(vrev32q_u8(vreinterpretq_u8(g2r2b1g1)));
			uint32x4_t b3g3r3b2 = vldrwq_gather_offset(srcp32 + 2, inc12);
			uint32x4_t b2r3g3b3 = vreinterpretq_u32(vrev32q_u8(vreinterpretq_u8(b3g3r3b2)));
			srcp32 += 4 * RGB_BYTES;

			uint32x4x4_t out;
			out.val[0] = vorrq_n_u32(vshrq_n_u32(r0g0b0r1, 8), 0xff000000);
			out.val[1] = vorrq_n_u32(vsriq_n_u32(vshlq_n_u32(r0g0b0r1, 16), g1b1r2g2, 16), 0xff000000);
			out.val[2] = vorrq_n_u32(vsriq_n_u32(vshlq_n_u32(g1b1r2g2, 8), b2r3g3b3, 24), 0xff000000);
			out.val[3] = vorrq_n_u32(b2r3g3b3, 0xff000000);
			vst4q_u32(dstp32, out);
			dstp32 += 4 * RGBA_BYTES;
		}
#else
#if MIMAGE_X % 16
#error "bad alignment"
#endif
		const uint8x16_t inc3 = vmulq_n_u8(vidupq_n_u8(0, 1), 3);
		const uint8_t *restrict srcp = src[y][0];
		uint8_t *restrict dstp = (uint8_t *)&dst[y];
		for (uint32_t x1 = 0; x1 < MIMAGE_X; x1 += 16)
		{
			uint8x16_t r = vldrbq_gather_offset(srcp + 0, inc3);
			uint8x16_t g = vldrbq_gather_offset(srcp + 1, inc3);
			uint8x16_t b = vldrbq_gather_offset(srcp + 2, inc3);
			srcp += 16 * RGB_BYTES;
			uint8x16x2_t out = { vsriq(vshlq_n_u8(g, 3), b, 3),
								 vsriq(r, g, 5) };
			vst2q(dstp, out);
			dstp += 16 * RGB565_BYTES;
		}
#
#endif
#elif 1
#if XOFFS % 4 || MIMAGE_X % 4 || DIMAGE_X % 4
#error "bad alignment"
#endif
		const uint8_t * restrict srcp = src[y][0];
		lv_color_t * restrict dstp = dst[y];
		const uint32_t *srcp32 = (const uint32_t *)srcp;
		uint32_t *dstp32 = (uint32_t *)dstp;
		// Load 4 pixels as 3 words, and expand to 4 words
		for (uint32_t x = 0; x < MIMAGE_X; x += 4)
		{
#if LV_COLOR_DEPTH == 32
			uint32_t r0g0b0r1 = __REV(*srcp32++);
			uint32_t g1b1r2g2 = __REV(*srcp32++);
			uint32_t b2r3g3b3 = __REV(*srcp32++);

			*dstp32++ = (r0g0b0r1 >> 8) | 0xff000000;
			*dstp32++ = (r0g0b0r1 << 16) | (g1b1r2g2 >> 16) | 0xff000000;
			*dstp32++ = (g1b1r2g2 << 8) | (b2r3g3b3 >> 24) | 0xff000000;
			*dstp32++ = b2r3g3b3 | 0xff000000;
#else
			uint32_t r1b0g0r0 = *srcp32++;
			uint32_t g2r2b1g1 = *srcp32++;
			uint32_t b3g3r3b2 = *srcp32++;
			*dstp32++ = (r1b0g0r0         & 0xf8000000) |
						((g2r2b1g1 << 19) & 0x07e00000) |
						((g2r2b1g1 << 5)  & 0x001f0000) |
						((r1b0g0r0 << 8)  & 0x0000f800) |
						((r1b0g0r0 >> 5)  & 0x000007e0) |
						((r1b0g0r0 >> 19) & 0x0000001f);
			*dstp32++ = ((b3g3r3b2 << 16) & 0xf8000000) |
						((b3g3r3b2 << 3)  & 0x07e00000) |
						((b3g3r3b2 >> 11) & 0x001f0000) |
						((g2r2b1g1 >> 8)  & 0x0000f800) |
						((g2r2b1g1 >> 21) & 0x000007e0) |
						((b3g3r3b2 >> 3)  & 0x0000001f);
#endif
		}
#else
		for (uint32_t x = 0; x < MIMAGE_X; x++) {
			uint8_t r, g, b;

			r = src[y][x][0];
			g = src[y][x][1];
			b = src[y][x][2];

			dst[y][x] = lv_color_make(r, g, b);
		}
#endif
	}
}

#endif // USE_LVGL_ZOOM

void GLCD_Initialize(void)
{
    volatile int test = 3;
    SetupLEDs();

    // core_cm55.h
    SysTick_Config(SystemCoreClock/1000);

    while(test) {
        Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_HIGH);
        sleep_or_wait_msec(300);
        Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_LOW);
        sleep_or_wait_msec(300);
        test--;
    }

    static volatile int dinit = 0;

    dinit = Display_initialization(&lcd_image[0][0][0]);

    if (dinit != 0) {
        while(1) {
            Driver_GPIO1.SetValue(PIN_NUMBER_15, GPIO_PIN_OUTPUT_STATE_LOW);
            sleep_or_wait_msec(300);
            Driver_GPIO1.SetValue(PIN_NUMBER_15, GPIO_PIN_OUTPUT_STATE_HIGH);
            sleep_or_wait_msec(300);
        }
    }
    Driver_GPIO1.SetValue(PIN_NUMBER_15, GPIO_PIN_OUTPUT_STATE_HIGH);
    lv_port_disp_init();
}

void GLCD_Clear(unsigned short color)
{
    UNUSED(color);
}

void GLCD_Image(const void *data, const uint32_t width,
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

#ifndef USE_LVGL_ZOOM
    write_to_lvgl_buf_doubled(data, lvgl_image);
#else
    write_to_lvgl_buf(data, lvgl_image);
#endif
    extern lv_obj_t *imageObj;
    lv_obj_invalidate(imageObj);

	lv_task_handler();
}

void GLCD_DisplayChar(
        unsigned int ln, unsigned int col,
        unsigned char fi, unsigned char c)
{
    UNUSED(ln);
    UNUSED(col);
    UNUSED(fi);
    UNUSED(c);
}

void GLCD_Box(
        unsigned int x, unsigned int y,
        unsigned int w, unsigned int h,
        unsigned short color)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(w);
    UNUSED(h);
    UNUSED(color);
}

void GLCD_SetTextColor(unsigned short color)
{
    UNUSED(color);
}
