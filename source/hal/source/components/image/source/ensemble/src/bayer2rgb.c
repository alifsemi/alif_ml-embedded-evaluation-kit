/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base_def.h"
#include "bayer.h"

// At the time of writing, GCC produces incorrect assembly
#define ENABLE_MVE_BAYER2RGB (defined __ARMCC_VERSION && (__ARM_FEATURE_MVE & 1))

#if __ARM_FEATURE_MVE & 1
#include <arm_mve.h>
#endif

#include "Driver_CPI.h"
#include "image_processing.h"

extern uint8_t  	rgb_image[CIMAGE_X*CIMAGE_Y*RGB_BYTES];
extern uint8_t  	raw_image[CIMAGE_X*CIMAGE_Y*RGB_BYTES];

// tiff types: short = 3, int = 4
// Tags: ( 2-byte tag ) ( 2-byte type ) ( 4-byte count ) ( 4-byte data )
//    0100 0003 0000 0001 0064 0000
//       |        |    |         |
// tag --+        |    |         |
// short int -----+    |         |
// one value ----------+         |
// value of 100 -----------------+
//

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

#if ENABLE_MVE_BAYER2RGB
	// Index table into 16 RGB pairs for scatter stores: { 0, 6, 12, .. }
	const uint8x16_t inc6 = vmulq(vidupq_n_u8(0, 1), 6);
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

#if ENABLE_MVE_BAYER2RGB
		// Helium lets us process 16 at a time (8 per beat on Cortex-M55)
		int pairs_to_go = (bayerEnd - bayer) / 2;
		while (pairs_to_go > 0) {
			mve_pred16_t p = vctp8q(pairs_to_go);
			uint8x16x2_t rg = vld2q(bayer);
			uint8x16x2_t gb = vld2q(bayer + bayerStep);
			__builtin_prefetch(bayer + bayerStep + 16 * 2 * 2);
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
				rgb[-1] = bayer[0];
				rgb[0] = (bayer[1] + bayer[bayerStep] + 1) >> 1;
				rgb[1] = bayer[bayerStep + 1];

				rgb[2] = bayer[2];
				rgb[3] = (bayer[1] + bayer[bayerStep + 2] + 1) >> 1;
				rgb[4] = bayer[bayerStep + 1];
			}
		} else {
			for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
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

	DEBUG_PRINTF("\r\n\r\n >>> dc1394_bayer_Simple END <<< \r\n");
	return DC1394_SUCCESS;
}

/*Add tiff format headers */
uint8_t *
put_tiff(uint8_t * header, uint32_t width, uint32_t height, uint16_t bpp)
{
	uint32_t ulTemp=0;
	uint16_t sTemp=0;
	DEBUG_PRINTF("width 0x%x, height 0x%x bpp 0x%x\n", width, height, bpp);


	sTemp = TIFF_HDR_NUM_ENTRY;
	memcpy(header + 8, &sTemp, 2);

	memcpy(header + 10 + 1*12 + 8, &width, 4);
	memcpy(header + 10 + 2*12 + 8, &height, 4);
	memcpy(header + 10 + 3*12 + 8, &bpp, 2);

	// strip byte count
	ulTemp = width * height * (bpp / 8) * 3;
	memcpy(header + 10 + 7*12 + 8, &ulTemp, 4);

	//strip offset
	sTemp = TIFF_HDR_SIZE;
	memcpy(header + 10 + 5*12 + 8, &sTemp, 2);

	return header;
};


/*Bayer to RGB conversion */
int bayer_to_RGB(uint8_t * restrict src, uint8_t * restrict dest)
{
	dc1394_bayer_Simple(src, dest, CIMAGE_X, CIMAGE_Y, DC1394_COLOR_FILTER_BGGR);
	return 0;
}
