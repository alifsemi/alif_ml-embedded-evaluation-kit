/*
 * 1394-Based Digital Camera Control Library
 *
 * Bayer pattern decoding functions
 *
 * Written by Damien Douxchamps and Frederic Devernay
 * The original VNG and AHD Bayer decoding are from Dave Coffin's DCRAW.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

// Original dc1394_bayer_Simple copied from libdc1394 - bayer.c
// Modified to use Helium instructions when available.

#include <string.h>
#include "base_def.h"
#include "bayer.h"

#include "RTE_Components.h"

#if __ARM_FEATURE_MVE & 1

// At the time of writing, GCC fails for either intrinsics or assembly
#ifdef __ARMCC_VERSION
#define INTRINSIC_MVE_BAYER2RGB 1
#define ASM_MVE_BAYER2RGB 1
#else
#define INTRINSIC_MVE_BAYER2RGB 0
#define ASM_MVE_BAYER2RGB 0
#endif

#if ASM_MVE_BAYER2RGB || INTRINSIC_MVE_BAYER2RGB
#include <arm_mve.h>
#endif
#endif // __ARM_FEATURE_MVE & 1

#include "image_processing.h"

#define CHECK_EXPOSURE

#ifdef CHECK_EXPOSURE
#define CE(...) __VA_ARGS__
#else
#define CE(...)
#endif

// High and low threshold values - code uses >=
// checks, so a neutral value is >= low and < high.
// These linear values correspond to 20%/80% thresholds after the sRGB opto-electronic function (gamma)
#define THRESH_LOW 9
#define THRESH_HIGH 154

uint32_t exposure_over_count, exposure_high_count, exposure_low_count, exposure_under_count;

dc1394error_t
dc1394_bayer_Simple(const uint8_t * restrict bayer, uint8_t * restrict rgb, int sx, int sy, int tile)
{
	DEBUG_PRINTF("\r\n\r\n >>> 1 bayer:0x%X rgb:0x%X sx:0x%X sy:0x%X tile:0x%X <<< \r\n",(uint32_t)bayer,(uint32_t)rgb,sx,sy,tile);

	const int bayerStep = sx;
	const int rgbStep = 3 * sx;
	int width = sx;
	int height = sy;
	int blue = tile == DC1394_COLOR_FILTER_BGGR
			|| tile == DC1394_COLOR_FILTER_GBRG ? -1 : 1;
	int start_with_green = tile == DC1394_COLOR_FILTER_GBRG
			|| tile == DC1394_COLOR_FILTER_GRBG;
	int i, imax, iinc;

#ifdef CHECK_EXPOSURE
	uint32_t under_count = 0, not_low_count = 0, high_count = 0, over_count = 0;
#endif

	if ((tile>DC1394_COLOR_FILTER_MAX)||(tile<DC1394_COLOR_FILTER_MIN))
		return DC1394_INVALID_COLOR_FILTER;

	DEBUG_PRINTF("\r\n\r\n >>> dc1394_bayer_Simple 1 <<< \r\n");

	DEBUG_PRINTF("\r\n\r\n >>> 2 bayer:0x%X rgb:0x%X sx:0x%X sy:0x%X tile:0x%X imax:0x%X iinc:0x%X i:0x%X<<< \r\n",(uint32_t)bayer,(uint32_t)rgb,sx,sy,tile,imax,iinc,i);

	/* add black border */
	imax = sx * sy * 3;
	for (i = sx * (sy - 1) * 3; i < imax; i++)
	{
		rgb[i] = 0;
	}

	DEBUG_PRINTF("\r\n\r\n >>> dc1394_bayer_Simple 11 <<< \r\n");

	iinc = (sx - 1) * 3;

	DEBUG_PRINTF("\r\n\r\n >>> bayer:0x%X rgb:0x%X sx:0x%X sy:0x%X tile:0x%X imax:0x%X iinc:0x%X i:0x%X<<< \r\n",(uint32_t)bayer,(uint32_t)rgb,sx,sy,tile,imax,iinc,i);

	for (i = (sx - 1) * 3; i < imax; i += iinc)
	{
		rgb[i++] = 0;
		rgb[i++] = 0;
		rgb[i++] = 0;
	}

	DEBUG_PRINTF("\r\n\r\n >>> dc1394_bayer_Simple 111 <<< \r\n");

	rgb += 1;
	width -= 1;
	height -= 1;

	DEBUG_PRINTF("\r\n\r\n >>> dc1394_bayer_Simple 2 <<< \r\n");

#if INTRINSIC_MVE_BAYER2RGB || ASM_MVE_BAYER2RGB
	// Index table into 16 RGB pairs for scatter stores: { 0, 6, 12, .. }
	const uint8x16_t inc6 = vmulq_n_u8(vidupq_n_u8(0, 1), 6);
#endif

	for (; height--; bayer += bayerStep, rgb += rgbStep) {
		const uint8_t *bayerEnd = bayer + width;

		if (start_with_green) {
			rgb[-blue] = bayer[1];
			rgb[0] = (bayer[0] + bayer[bayerStep + 1] + 1) >> 1;
			rgb[blue] = bayer[bayerStep];
			bayer++;
			rgb += 3;
		}

#if ASM_MVE_BAYER2RGB
		// Helium lets us process 16 at a time (8 per beat on Cortex-M55)
		int pairs_to_go = (bayerEnd - bayer) / 2;
		int tmp;
		// The asm pointers will overrun - easiest to just figure out where we should
		// end up here, and forget the final pointer from the assembler
		const uint8_t *bayerAsm = bayer;
		uint8_t *rgbAsm = rgb;
		bayer += pairs_to_go * 2;
		rgb += pairs_to_go * 6;
		__asm (

   "    WLSTP.8     LR, %[pairs_to_go], 2f\n"
   "1:\n\t"
   "    VLD20.8     {Q0,Q1},[%[bayer]]\n\t"
   "    ADD         %[tmp], %[bayer], %[bayerStep]\n\t"
CE("    VMAX.U8     Q4, Q0, Q1\n\t")
   "    VLD21.8     {Q0,Q1},[%[bayer]]!\n\t"
CE("    VPT.U8      HS, Q4, %[expHigh]\n\t")
CE("    VADDVAT.U8  %[high_count], %q[vector1]\n\t")
CE("    VPT.U8      EQ, Q4, %[scalar255]\n\t")
CE("    VADDVAT.U8  %[over_count], %q[vector1]\n\t")
   "    VLD20.8     {Q2,Q3},[%[tmp]]\n\t"
CE("    VMIN.U8     Q4, Q0, Q1\n\t")
   "    VLD21.8     {Q2,Q3},[%[tmp]]\n\t"
   "    PLD         [%[tmp], #64]\n\t"
   "    SUB         %[tmp], %[rgb], %[blue]\n\t"
   "    VSTRB.8     Q0,[%[tmp], %q[inc6]]\n\t"
CE("    VPT.U8      HS, Q4, %[expLow]\n\t")
CE("    VADDVAT.U8  %[not_low_count], %q[vector1]\n\t")
   "    VRHADD.U8   Q1, Q1, Q2\n\t"
CE("    VPT.U8      EQ, Q4, ZR\n\t")
CE("    VADDVAT.U8  %[under_count], %q[vector1]\n\t")
   "    VSTRB.8     Q1,[%[rgb], %q[inc6]]\n\t"
   "    ADD         %[tmp], %[rgb], %[blue]\n\t"
   "    VSTRB.8     Q3,[%[tmp], %q[inc6]]\n\t"
   "    SUB         %[tmp], %[bayer], #31\n\t"
   "    VLD20.8     {Q0,Q1}, [%[tmp]]\n\t"
   "    VLD21.8     {Q0,Q1}, [%[tmp]]\n\t"
   "    ADD         %[tmp], %[bayerStep]\n\t"
   "    VLD20.8     {Q2,Q3}, [%[tmp]]\n\t"
   "    VLD21.8     {Q2,Q3}, [%[tmp]]\n\t"
   "    ADD         %[rgb], %[rgb], #3\n\t"
   "    VRHADD.U8   Q0, Q0, Q3\n\t"
   "    VSTRB.8     Q0, [%[rgb], %q[inc6]]\n\t"
   "    SUB         %[tmp], %[rgb], %[blue]\n\t"
   "    VSTRB.8     Q1, [%[tmp], %q[inc6]]\n\t"
   "    ADD         %[tmp], %[rgb], %[blue]\n\t"
   "    VSTRB.8     Q2, [%[tmp], %q[inc6]]\n\t"
   "    ADD         %[rgb], #16*6-3\n\t"
   "    LETP        LR, 1b\n"

   "2:"
   : [bayer] "+r"(bayerAsm), [rgb] "+r"(rgbAsm),
     [tmp] "=&r"(tmp)
CE(, [high_count] "+Te"(high_count), [not_low_count] "+Te"(not_low_count))
CE(, [over_count] "+Te"(over_count), [under_count] "+Te"(under_count))
   : [bayerStep] "r"(bayerStep), [inc6] "w"(inc6), [blue] "r"(blue),
     [expHigh] "r"(THRESH_HIGH), [scalar255] "r"(255),
     [expLow] "r"(THRESH_LOW), [vector1] "w"(vdupq_n_u8(1)),
     [pairs_to_go] "r"(pairs_to_go)
   : "q0", "q1", "q2", "q3", "q4", "lr", "cc", "memory");
#elif INTRINSIC_MVE_BAYER2RGB
		// Helium lets us process 16 at a time (8 per beat on Cortex-M55)
		int pairs_to_go = (bayerEnd - bayer) / 2;

		while (pairs_to_go > 0) {
			mve_pred16_t p = vctp8q(pairs_to_go);
			uint8x16x2_t rg = vld2q(bayer);
			uint8x16x2_t gb = vld2q(bayer + bayerStep);
			__builtin_prefetch(bayer + bayerStep + 16 * 2 * 2);
#ifdef CHECK_EXPOSURE
			uint8x16_t max = vmaxq_x(rg.val[0], rg.val[1], p);
			mve_pred16_t high = vcmpcsq_m(max, THRESH_HIGH, p);
			high_count = vaddvaq_p(high_count, vdupq_n_u8(1), high);
			mve_pred16_t over = vcmpeqq_m(max, 255, p);
			over_count = vaddvaq_p(over_count, vdupq_n_u8(1), over);
			uint8x16_t min = vminq_x(rg.val[0], rg.val[1], p);
			mve_pred16_t not_low = vcmpcsq_m(min, THRESH_LOW, p);
			not_low_count = vaddvaq_p(not_low_count, vdupq_n_u8(1), not_low);
			mve_pred16_t under = vcmpeqq_m(min, 0, p);
			under_count = vaddvaq_p(under_count, vdupq_n_u8(1), under);
#endif
			uint8x16_t r0 = rg.val[0];
			uint8x16_t g0 = vrhaddq_x(rg.val[1], gb.val[0], p);
			uint8x16_t b0 = gb.val[1];
			vstrbq_scatter_offset_p(rgb - blue, inc6, r0, p);
			vstrbq_scatter_offset_p(rgb, inc6, g0, p);
			vstrbq_scatter_offset_p(rgb + blue, inc6, b0, p);

			uint8x16x2_t gr = vld2q(bayer + 1);
			uint8x16x2_t bg = vld2q(bayer + bayerStep + 1);
			uint8x16_t r1 = gr.val[1];
			uint8x16_t g1 = vrhaddq_x(gr.val[0], bg.val[1], p);
			uint8x16_t b1 = bg.val[0];
			vstrbq_scatter_offset_p(rgb + 3 - blue, inc6, r1, p);
			vstrbq_scatter_offset_p(rgb + 3, inc6, g1, p);
			vstrbq_scatter_offset_p(rgb + 3 + blue, inc6, b1, p);
			bayer += 16 * 2;
			rgb += 16 * 6;
			pairs_to_go -= 16;
		}

		bayer += pairs_to_go * 2;
		rgb += pairs_to_go * 6;
#else
		if (blue > 0) {
			for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
#ifdef CHECK_EXPOSURE
				if (bayer[0] == 255 || bayer[1] == 255) {
					over_count++;
				}
				if (bayer[0] >= THRESH_HIGH || bayer[1] >= THRESH_HIGH) {
					high_count++;
				}
				if (bayer[0] >= THRESH_LOW && bayer[1] >= THRESH_LOW) {
					not_low_count++;
				}
				if (bayer[0] == 0 || bayer[1] == 0) {
					under_count++;
				}
#endif
				rgb[-1] = bayer[0];
				rgb[0] = (bayer[1] + bayer[bayerStep] + 1) >> 1;
				rgb[1] = bayer[bayerStep + 1];

				rgb[2] = bayer[2];
				rgb[3] = (bayer[1] + bayer[bayerStep + 2] + 1) >> 1;
				rgb[4] = bayer[bayerStep + 1];
			}
		} else {
			for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
#ifdef CHECK_EXPOSURE
				if (bayer[0] == 255 || bayer[1] == 255) {
					over_count++;
				}
				if (bayer[0] >= THRESH_HIGH || bayer[1] >= THRESH_HIGH) {
					high_count++;
				}
				if (bayer[0] >= THRESH_LOW && bayer[1] >= THRESH_LOW) {
					not_low_count++;
				}
				if (bayer[0] == 1 || bayer[1] == 1) {
					under_count++;
				}
#endif
				rgb[1] = bayer[0];
				rgb[0] = (bayer[1] + bayer[bayerStep] + 1) >> 1;
				rgb[-1] = bayer[bayerStep + 1];

				rgb[4] = bayer[2];
				rgb[3] = (bayer[1] + bayer[bayerStep + 2] + 1) >> 1;
				rgb[2] = bayer[bayerStep + 1];
			}
		}
#endif

		if (bayer < bayerEnd) {
			rgb[-blue] = bayer[0];
			rgb[0] = (bayer[1] + bayer[bayerStep] + 1) >> 1;
			rgb[blue] = bayer[bayerStep + 1];
			bayer++;
			rgb += 3;
		}

		bayer -= width;
		rgb -= width * 3;

		blue = -blue;
		start_with_green = !start_with_green;
	}
#ifdef CHECK_EXPOSURE
	/* Adjust total to be in raw pixels - we processed pairs */
	exposure_over_count = over_count * 2;
	exposure_high_count = high_count * 2;
	exposure_low_count = (sx - 2) * (sy - 1) - not_low_count * 2;
	exposure_under_count = under_count * 2;
#endif

	DEBUG_PRINTF("\r\n\r\n >>> dc1394_bayer_Simple END <<< \r\n");
	return DC1394_SUCCESS;
}
