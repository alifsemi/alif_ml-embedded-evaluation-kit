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
#include "base_def.h"
#include "image_processing.h"

#if __ARM_FEATURE_MVE & 1
#include <arm_mve.h>
#endif

void color_correction(const uint8_t sp[static 3], uint8_t dp[static 3])
{
//	volatile static uint32_t t0, ts;
//	ts = PMU_GetCounter();

#if 0
	uint8_t tmp = dp[0];
	dp[0] = sp[2]
	dp[1] = sp[1];
	dp[2] = tmp;
#else

#if __ARM_FEATURE_MVE & 1

#define FIXED_SHIFT 20
#define FIXED(a) (int32_t)(((a) * (1UL << FIXED_SHIFT) + 0.5))

	const int32x4_t c0 = { +FIXED(3.017), +FIXED(0.162), -FIXED(0.636) };
	const int32x4_t c1 = { -FIXED(0.664), +FIXED(1.315), -FIXED(0.360) };
	const int32x4_t c2 = { -FIXED(0.139), -FIXED(0.495), +FIXED(2.092) };

	// multiply and accumulate, so d[0] = 3.017*sp[0] - 0.664*sp[1] - 0.139*sp[2], etc
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
	dpixel_data[0] =  2.092f*sp[2] - 0.369f*sp[1] - 0.636f*sp[0];
	dpixel_data[1] = -0.492f*sp[2] + 1.315f*sp[1] + 0.162f*sp[0];
	dpixel_data[2] = -0.139f*sp[2] - 0.664f*sp[1] + 3.017f*sp[0];
	if (dpixel_data[0] < 0) dpixel_data[0] = 0;
	if (dpixel_data[1] < 0) dpixel_data[1] = 0;
	if (dpixel_data[2] < 0) dpixel_data[2] = 0;

	if (dpixel_data[0] > 255) dpixel_data[0] = 255;
	if (dpixel_data[1] > 255) dpixel_data[1] = 255;
	if (dpixel_data[2] > 255) dpixel_data[2] = 255;
	dp[2] = (uint8_t)dpixel_data[0]; // 2 = RED
	dp[1] = (uint8_t)dpixel_data[1]; // 1 = GREEN
	dp[0] = (uint8_t)dpixel_data[2]; // 0 = BLUE
#endif // __ARM_FEATURE_MVE & 1
#endif // 0
//	t0 = PMU_GetCounter() - ts;
}

void white_balance(const uint8_t *sp, uint8_t *dp)
{
#if 0
    if (dp != sp) {
        memcpy(dp, sp, MIMAGE_X * MIMAGE_Y * RGB_BYTES);
    }
#else
    for (uint32_t index = 0; index < MIMAGE_X * MIMAGE_Y * RGB_BYTES; index += RGB_BYTES) {
        color_correction(&sp[index], &dp[index]);
    }
#endif
}
