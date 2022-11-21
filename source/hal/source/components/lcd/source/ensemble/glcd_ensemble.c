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

#include <arm_mve.h>

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
#include "display.h"
#include "LCD_panel.h"
#include "lvgl.h"
#include "delay.h"

extern void lv_port_disp_init(void);
extern uint8_t lcd_image[DIMAGE_Y][DIMAGE_X][RGB_BYTES];
extern uint8_t raw_image[CIMAGE_X*CIMAGE_Y*RGB_BYTES];
extern uint8_t rgb_image[CIMAGE_X*CIMAGE_Y*RGB_BYTES];
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

void write_to_lcd(
		const uint8_t src[static restrict MIMAGE_Y][MIMAGE_X][RGB_BYTES],
		uint8_t dst[static restrict DIMAGE_Y][DIMAGE_X][RGB_BYTES])
{
	const uint8x16_t inc3 = vmulq(vidupq_n_u8(0, 1), 3);
	const uint8x16_t inc6 = vshlq_n(inc3, 1);

	for (uint32_t y1 = 0; y1 < MIMAGE_Y; y1++) {
#define SRC_ROW_OFFSET_32 ((MIMAGE_X * 3) / 4)

#if 1
#if XOFFS % 4 || MIMAGE_X % 4 || DIMAGE_X % 4
#errof "bad alignment"
#endif
		const uint32x4_t inc12 = vmulq(vidupq_n_u32(0, 4), 3);
		const uint32x4_t inc24 = vshlq_n(inc12, 1);
		const uint32_t *restrict srcp32 = (const uint32_t *) src[y1][0];
		uint32_t *restrict dstp32 = (uint32_t *) dst[YOFFS + y1 * 2][XOFFS];
		uint32_t *restrict dst2p32 = (uint32_t *) dst[YOFFS + y1 * 2 + 1][XOFFS];
		for (uint32_t x1 = 0; x1 < MIMAGE_X; x1 += 4 * 4)
		{
			uint32x4_t r1b0g0r0 = vldrwq_gather_offset(srcp32 + 0, inc12);
			uint32x4_t g2r2b1g1 = vldrwq_gather_offset(srcp32 + 1, inc12);
			uint32x4_t b3g3r3b2 = vldrwq_gather_offset(srcp32 + 2, inc12);
			srcp32 += 4 * 3;
			uint32x4_t r0b0g0r0 = vsliq(r1b0g0r0, r1b0g0r0, 24);
			vstrwq_scatter_offset(dstp32 + 0, inc24, r0b0g0r0);
			vstrwq_scatter_offset(dst2p32 + 0, inc24, r0b0g0r0);
			uint32x4_t g1r1b0g0 = vsriq(vshlq_n(g2r2b1g1, 24), r1b0g0r0, 8);
			vstrwq_scatter_offset(dstp32 + 1, inc24, g1r1b0g0);
			vstrwq_scatter_offset(dst2p32 + 1, inc24, g1r1b0g0);
			uint32x4_t b1g1r1b0 = vsliq(vshrq(r1b0g0r0, 16), g2r2b1g1, 16);
			uint32x4_t b1g1r1b1 = vsriq(b1g1r1b0, b1g1r1b0, 24);
			vstrwq_scatter_offset(dstp32 + 2, inc24, b1g1r1b1);
			vstrwq_scatter_offset(dst2p32 + 2, inc24, b1g1r1b1);
			uint32x4_t r3b2g2r2 = vsriq(vshlq_n(b3g3r3b2, 16), g2r2b1g1, 16);
			uint32x4_t r2b2g2r2 = vsliq(r3b2g2r2, r3b2g2r2, 24);
			vstrwq_scatter_offset(dstp32 + 3, inc24, r2b2g2r2);
			vstrwq_scatter_offset(dst2p32 + 3, inc24, r2b2g2r2);
			uint32x4_t g3r3b2g2 = vsriq(vshlq_n(b3g3r3b2, 8), g2r2b1g1, 24);
			vstrwq_scatter_offset(dstp32 + 4, inc24, g3r3b2g2);
			vstrwq_scatter_offset(dst2p32 + 4, inc24, g3r3b2g2);
			uint32x4_t b3g3r3b3 = vsriq(b3g3r3b2, b3g3r3b2, 24);
			vstrwq_scatter_offset(dstp32 + 5, inc24, b3g3r3b3);
			vstrwq_scatter_offset(dst2p32 + 5, inc24, b3g3r3b3);
			dstp32 += 4 * 6;
			dst2p32 += 4 * 6;
		}
#elif 1
#if XOFFS % 4 || MIMAGE_X % 4 || DIMAGE_X % 4
#errof "bad alignment"
#endif
		const uint8_t * restrict srcp = src[y1][0];
		uint8_t * restrict dstp = dst[YOFFS + y1 * 2][XOFFS];
		uint8_t * restrict dst2p = dst[YOFFS + y1 * 2 + 1][XOFFS];
		const uint32_t *srcp32 = (const uint32_t *)srcp;
		uint32_t *dstp32 = (uint32_t *)dstp;
		// Load 4 pixels as 3 words, and expand to 6 words, on two rows
		// ARM compiler can further vectorise this to across 4 lanes, so 12 input words or 16 pixels per iteration
		// "& 0x00ffffff" or "& 0xffffff00" are valid constant forms for VBIC; we rely on shifts to get other masks
		// to try to avoid register pressure for mask constants (but compiler seems to convert to masking anyway)
		for (uint32_t x1 = 0; x1 < MIMAGE_X; x1 += 4)
		{
			uint32_t r1b0g0r0 = *srcp32++;
			uint32_t g2r2b1g1 = *srcp32++;
			uint32_t b3g3r3b2 = *srcp32++;
			*dstp32++ = (r1b0g0r0 << 24) | (r1b0g0r0 & 0xffffff); // r0b0g0r0
			*dstp32++ = (g2r2b1g1 << 24) | (r1b0g0r0 >> 8); // g1r1b0g0
			*dstp32++ = (g2r2b1g1 << 16) | ((r1b0g0r0 >> 24) << 8) | ((g2r2b1g1 << 16) >> 24); // b1g1r1b1
			*dstp32++ = ((g2r2b1g1 >> 16) << 24) | ((b3g3r3b2 << 24) >> 8) | (g2r2b1g1 >> 16); // r2b2g2r2
			*dstp32++ = (b3g3r3b2 << 8) | (g2r2b1g1 >> 24); // g3r3b2g2
			*dstp32++ = (b3g3r3b2 & 0xffffff00) | (b3g3r3b2 >> 24); // b3g3r3b3
		}
		// Memcpy the second row rather than doing it as-we-go, because as-we-go makes
		// the above too complex for the autovectoriser.
		memcpy(dst2p, dstp, 2 * MIMAGE_X * RGB_BYTES);
#else
		for (uint32_t x1 = 0; x1 < MIMAGE_X; x1++) {
			uint8_t r, g, b;
			int32_t x, y;

			b = src[y1][x1][0];
			r = src[y1][x1][1];
			g = src[y1][x1][2];

			x = XOFFS + (x1 << 1);
			y = YOFFS + (y1 << 1);

			dst[y][x][0] = b;
			dst[y][x][1] = r;
			dst[y][x][2] = g;

			dst[y][x+1][0] = b;
			dst[y][x+1][1] = r;
			dst[y][x+1][2] = g;

			dst[y+1][x][0] = b;
			dst[y+1][x][1] = r;
			dst[y+1][x][2] = g;

			dst[y+1][x+1][0] = b;
			dst[y+1][x+1][1] = r;
			dst[y+1][x+1][2] = g;
		}
#endif
	}
}

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

    write_to_lcd(data, lcd_image);

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
