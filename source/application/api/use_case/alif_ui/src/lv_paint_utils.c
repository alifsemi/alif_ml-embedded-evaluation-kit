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

#if defined __clang__ || defined __GNUC__
#pragma GCC diagnostic ignored "-Wvla"
#endif

#include "lvgl.h"

#define RGB_BYTES       3
#define RGBA_BYTES      4
#define RGB565_BYTES    2

#if defined __ARMCC_VERSION && (__ARM_FEATURE_MVE & 1)
#define ENABLE_MVE_WRITE 1
#else
#define ENABLE_MVE_WRITE 0
#endif
#define ENABLE_WORD_WRITE 1

#if __ARM_FEATURE_MVE & 1
#include <arm_mve.h>
#endif

#if !(LV_COLOR_DEPTH == 16 || LV_COLOR_DEPTH == 32)
#error "Unsupported LV_COLOR_DEPTH"
#endif

void write_to_lvgl_buf_doubled(
        int width, int height,
        const uint8_t * restrict src_ptr,
        lv_color_t * restrict dst_ptr)
{
    const uint8_t (*src)[width][RGB_BYTES] = (const uint8_t (*)[width][RGB_BYTES]) src_ptr;
    lv_color_t (*dst)[width * 2] = (lv_color_t (*)[width * 2]) dst_ptr;
    if (width % 16) {
        abort();
    }
	for (int y1 = 0; y1 < height; y1++) {

#if ENABLE_MVE_WRITE && LV_COLOR_DEPTH == 32
		const uint32x4_t inc12 = vmulq_n_u32(vidupq_n_u32(0, 4), RGB_BYTES);
		const uint32x4_t incout = vmulq_n_u32(vidupq_n_u32(0, 4), 2 * LV_COLOR_DEPTH / 8);
		const uint32_t *restrict srcp32 = (const uint32_t *) src[y1][0];
		uint32_t *restrict dstp32 = (uint32_t *) dst[y1 * 2][0];
		uint32_t *restrict dst2p32 = (uint32_t *) dst[y1 * 2 + 1][0];
		for (int x1 = 0; x1 < width; x1 += 4 * 4)
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
#elif ENABLE_WORD_WRITE
		const uint8_t * restrict srcp = src[y1][0];
		lv_color_t * restrict dstp = dst[y1 * 2];
		lv_color_t * restrict dst2p = dst[y1 * 2 + 1];
		const uint32_t *srcp32 = (const uint32_t *)srcp;
		uint32_t *dstp32 = (uint32_t *)dstp;
		// Load 4 pixels as 3 words, and expand to 8 words, on two rows
		for (int x1 = 0; x1 < width; x1 += 4)
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
		memcpy(dst2p, dstp, 2 * width * RGBA_BYTES);
#else
		for (int x1 = 0; x1 < width; x1++) {
			uint8_t r, g, b;
			int32_t x, y;

			r = src[y1][x1][0];
			g = src[y1][x1][1];
			b = src[y1][x1][2];

			x = (x1 << 1);
			y = (y1 << 1);

			lv_color_t c = lv_color_make(r, g, b);
			dst[y][x] = c;
			dst[y][x+1] = c;
			dst[y+1][x] = c;
			dst[y+1][x+1] = c;
		}
#endif
	}
}

void write_to_lvgl_buf(
        int width, int height,
        const uint8_t * restrict src_ptr,
        lv_color_t * restrict dst_ptr)
{
    const uint8_t (*src)[width][RGB_BYTES] = (const uint8_t (*)[width][RGB_BYTES]) src_ptr;
    lv_color_t (*dst)[width] = (lv_color_t (*)[width]) dst_ptr;
    if (width % 16) {
        abort();
    }
	for (int y = 0; y < height; y++) {
#if ENABLE_MVE_WRITE
#if LV_COLOR_DEPTH == 32
		const uint32x4_t inc12 = vmulq_n_u32(vidupq_n_u32(0, 4), 3);
		const uint32_t *restrict srcp32 = (const uint32_t *) src[y][0];
		uint32_t *restrict dstp32 = &dst[y].full;
		for (int x1 = 0; x1 < width; x1 += 4 * 4)
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
		const uint8x16_t inc3 = vmulq_n_u8(vidupq_n_u8(0, 1), 3);
		const uint8_t *restrict srcp = src[y][0];
		uint8_t *restrict dstp = (uint8_t *)&dst[y];
		for (int x1 = 0; x1 < width; x1 += 16)
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
#endif
#elif ENABLE_WORD_WRITE
		const uint8_t * restrict srcp = src[y][0];
		lv_color_t * restrict dstp = dst[y];
		const uint32_t *srcp32 = (const uint32_t *)srcp;
		uint32_t *dstp32 = (uint32_t *)dstp;
		// Load 4 pixels as 3 words, and expand to 4 words
		for (int x = 0; x < width; x += 4)
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
		for (int x = 0; x < width; x++) {
			uint8_t r, g, b;

			r = src[y][x][0];
			g = src[y][x][1];
			b = src[y][x][2];

			dst[y][x] = lv_color_make(r, g, b);
		}
#endif
	}
}
