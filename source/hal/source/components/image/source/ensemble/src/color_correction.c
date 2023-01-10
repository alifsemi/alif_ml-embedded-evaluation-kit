/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdint.h>
#include <stddef.h>
#include "base_def.h"
#include "image_processing.h"

#if __ARM_FEATURE_MVE & 1
#include <arm_mve.h>
#endif

void color_correction(const uint8_t sp[static 3], uint8_t dp[static 3])
{
//	volatile static uint32_t t0, ts;
//	ts = PMU_GetCounter();

#if __ARM_FEATURE_MVE & 1 && defined __ARMCC_VERSION

#define FIXED_SHIFT 20
#define FIXED(a) (int32_t)(((a) * (1UL << FIXED_SHIFT) + 0.5))

	const int32x4_t c0 = { +FIXED(2.092), -FIXED(0.492), -FIXED(0.139) };
	const int32x4_t c1 = { -FIXED(0.369), +FIXED(1.315), -FIXED(0.664) };
	const int32x4_t c2 = { -FIXED(0.636), +FIXED(0.162), +FIXED(3.017) };

	// multiply and accumulate, so d[0] = 2.092*sp[0] - 0.369*sp[1] - 0.636*sp[2], etc
	int32x4_t d = vmulq(c0, sp[0]);
	d = vmlaq(d, c1, sp[1]);
	d = vmlaq(d, c2, sp[2]);
	// saturating rounding shift right to unsigned and narrow - result into odd 16-bit lanes (bottom of 32-bit lanes)
	uint16x8_t ud16 = vqrshrunbq(vuninitializedq_u16(), d, 16);
	// saturating rounding shift right and narrow again - result into odd 8-bit lanes (bottom of 32-bit lanes)
	uint8x16_t ud8 = vqrshrnbq(vuninitializedq_u8(), ud16, FIXED_SHIFT - 16);
	// write out 3 bytes from the first 3 32-bit lanes
	vstrbq_p(dp, vreinterpretq_u32(ud8), vctp32q(3));
#else
	float dpixel_data[3];
	dpixel_data[0] =  2.092f*sp[0] - 0.369f*sp[1] - 0.636f*sp[2];
	dpixel_data[1] = -0.492f*sp[0] + 1.315f*sp[1] + 0.162f*sp[2];
	dpixel_data[2] = -0.139f*sp[0] - 0.664f*sp[1] + 3.017f*sp[2];
	if (dpixel_data[0] < 0) dpixel_data[0] = 0;
	if (dpixel_data[1] < 0) dpixel_data[1] = 0;
	if (dpixel_data[2] < 0) dpixel_data[2] = 0;

	if (dpixel_data[0] > 255) dpixel_data[0] = 255;
	if (dpixel_data[1] > 255) dpixel_data[1] = 255;
	if (dpixel_data[2] > 255) dpixel_data[2] = 255;
	dp[0] = (uint8_t)dpixel_data[0]; // 0 = RED
	dp[1] = (uint8_t)dpixel_data[1]; // 1 = GREEN
	dp[2] = (uint8_t)dpixel_data[2]; // 2 = BLUE
#endif // __ARM_FEATURE_MVE & 1
//	t0 = PMU_GetCounter() - ts;
}

static void bulk_color_correction(const uint8_t *sp, uint8_t *dp, ptrdiff_t len)
{
	const uint16x8_t pixel_offsets = vmulq_n_u16(vidupq_n_u16(0, 1), 3);

	while (len > 0) {
		// Fetching two iterations ahead seems optimal for RTSS-HP fetching from SRAM0
		__builtin_prefetch(sp + 3 * 8 * 2);
		float16x8_t r = vcvtq(vldrbq_gather_offset(sp + 0, pixel_offsets));
		float16x8_t g = vcvtq(vldrbq_gather_offset(sp + 1, pixel_offsets));
		float16x8_t b = vcvtq(vldrbq_gather_offset(sp + 2, pixel_offsets));
		sp += 3 * 8;

		{
			// I was using the f16 suffix on constants below, but GCC objects (Bug 107515)
			float16x8_t r_mac = vmulq(r, 2.092);
			r_mac = vfmaq(r_mac, g, -0.369);
			r_mac = vfmaq(r_mac, b, -0.636);

			uint16x8_t r_out = vcvtq_u16_f16(r_mac);
			r_out = vreinterpretq_u16(vqmovnbq(vuninitializedq_u8(), r_out));
			vstrbq_scatter_offset(dp + 0, pixel_offsets, r_out);
		}

		{
			float16x8_t g_mac = vmulq(r, -0.492);
			g_mac = vfmaq(g_mac, g, 1.315);
			g_mac = vfmaq(g_mac, b, 0.162);

			uint16x8_t g_out = vcvtq_u16_f16(g_mac);
			g_out = vreinterpretq_u16(vqmovnbq(vuninitializedq_u8(), g_out));
			vstrbq_scatter_offset(dp + 1, pixel_offsets, g_out);
		}

		{
			float16x8_t b_mac = vmulq(r, -0.139);
			b_mac = vfmaq(b_mac, g, -0.664);
			b_mac = vfmaq(b_mac, b, 3.017);

			uint16x8_t b_out = vcvtq_u16_f16(b_mac);
			b_out = vreinterpretq_u16(vqmovnbq(vuninitializedq_u8(), b_out));
			vstrbq_scatter_offset(dp + 2, pixel_offsets, b_out);
		}

		dp += 3 * 8;
		len -= 3 * 8;
	}
}

void white_balance(int ml_width, int ml_height, const uint8_t *sp, uint8_t *dp)
{
#if 0
    if (dp != sp) {
        memcpy(dp, sp, ml_width * ml_height * RGB_BYTES);
    }
#elif 1
    bulk_color_correction(sp, dp, ml_width * ml_height * RGB_BYTES);
#else
    for (uint32_t index = 0; index < ml_width * ml_height * RGB_BYTES; index += RGB_BYTES) {
        color_correction(&sp[index], &dp[index]);
    }
#endif
}
