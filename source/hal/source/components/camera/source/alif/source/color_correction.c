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
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "base_def.h"
#include "image_processing.h"

#if __ARM_FEATURE_MVE & 1
#include <arm_mve.h>
#endif

#define SKIP_COLOR_CORRECTION 0
#if __ARM_FEATURE_MVE & 1
#define PIXELWISE_COLOR_CORRECTION 0
#else
#define PIXELWISE_COLOR_CORRECTION 1
#endif

/* Matrix coefficients - C_RG means red input contribution to green output */
#if 0
/* Previous matrix in use until June 2023 */
#define C_RR +2.092f
#define C_RG -0.492f
#define C_RB -0.139f
#define C_GR -0.369f
#define C_GG +1.315f
#define C_GB -0.664f
#define C_BR -0.636f
#define C_BG +0.162f
#define C_BB +3.017f
#else
/* Revised matrix (BECP-1455)
 * Manual WB, Illuminant 6021, Relative Red 1.42, Relative Blue 1.47
 */
#define C_RR +2.2583f
#define C_RG -0.5501f
#define C_RB -0.1248f
#define C_GR -0.1606f
#define C_GG +1.4318f
#define C_GB -0.5268f
#define C_BR -0.6317f
#define C_BG -0.0653f
#define C_BB +2.3735f
#endif

#if PIXELWISE_COLOR_CORRECTION
static void color_correction(const uint8_t sp[static 3], uint8_t dp[static 3], const uint8_t lut[static restrict 256])
{
//	volatile static uint32_t t0, ts;
//	ts = PMU_GetCounter();

#if __ARM_FEATURE_MVE & 1 && defined __ARMCC_VERSION

#define FIXED_SHIFT 20
#define FIXED(a) ((a) < 0 ? -(int32_t)((-(a) * (1UL << FIXED_SHIFT) + 0.5)) : (int32_t)(((a) * (1UL << FIXED_SHIFT) + 0.5)))

	const int32x4_t c0 = { FIXED(C_RR), FIXED(C_RG), FIXED(C_RB) };
	const int32x4_t c1 = { FIXED(C_GR), FIXED(C_GG), FIXED(C_GB) };
	const int32x4_t c2 = { FIXED(C_BR), FIXED(C_BG), FIXED(C_BB) };

	// multiply and accumulate, so d[0] = C_RR*sp[0] + C_GR*sp[1] + C_BR*sp[2], etc
	int32x4_t d = vmulq(c0, sp[0]);
	d = vmlaq(d, c1, sp[1]);
	d = vmlaq(d, c2, sp[2]);
	// saturating rounding shift right to unsigned and narrow - result into odd 16-bit lanes (bottom of 32-bit lanes)
	uint16x8_t ud16 = vqrshrunbq(vuninitializedq_u16(), d, 16);
	// saturating rounding shift right and narrow again - result into odd 8-bit lanes (bottom of 32-bit lanes)
	uint8x16_t ud8 = vqrshrnbq(vuninitializedq_u8(), ud16, FIXED_SHIFT - 16);
	uint32x4_t ud32 = vldrbq_gather_offset_u32(lut, vandq(vreinterpretq_u32(ud8), vdupq_n_u32(0x000000FF)));
	// write out 3 bytes from the first 3 32-bit lanes
	vstrbq_p(dp, ud32, vctp32q(3));
#else
	float dpixel_data[3];
	dpixel_data[0] = C_RR*sp[0] + C_GR*sp[1] + C_BR*sp[2];
	dpixel_data[1] = C_RG*sp[0] + C_GG*sp[1] + C_BG*sp[2];
	dpixel_data[2] = C_RB*sp[0] + C_GB*sp[1] + C_BB*sp[2];
	if (dpixel_data[0] < 0) dpixel_data[0] = 0;
	if (dpixel_data[1] < 0) dpixel_data[1] = 0;
	if (dpixel_data[2] < 0) dpixel_data[2] = 0;

	if (dpixel_data[0] > 255) dpixel_data[0] = 255;
	if (dpixel_data[1] > 255) dpixel_data[1] = 255;
	if (dpixel_data[2] > 255) dpixel_data[2] = 255;
	dp[0] = lut[(uint8_t)dpixel_data[0]]; // 0 = RED
	dp[1] = lut[(uint8_t)dpixel_data[1]]; // 1 = GREEN
	dp[2] = lut[(uint8_t)dpixel_data[2]]; // 2 = BLUE
#endif // __ARM_FEATURE_MVE & 1
//	t0 = PMU_GetCounter() - ts;
}

#else

static void bulk_color_correction(const uint8_t *sp, uint8_t *dp, ptrdiff_t len, const uint8_t lut[static restrict 256])
{
	const uint16x8_t pixel_offsets = vmulq_n_u16(vidupq_n_u16(0, 1), 3);

	while (len > 0) {
#if __CM55_REV == 0x0001 // Only worthwhile with r0p1 in A silicon
		// Fetching two iterations ahead seems optimal for RTSS-HP fetching from SRAM0
		__builtin_prefetch(sp + 3 * 8 * 2);
#endif
		float16x8_t r = vcvtq(vldrbq_gather_offset(sp + 0, pixel_offsets));
		float16x8_t g = vcvtq(vldrbq_gather_offset(sp + 1, pixel_offsets));
		float16x8_t b = vcvtq(vldrbq_gather_offset(sp + 2, pixel_offsets));
		sp += 3 * 8;

		{
			float16x8_t r_mac = vmulq(r, C_RR);
			r_mac = vfmaq(r_mac, g, C_GR);
			r_mac = vfmaq(r_mac, b, C_BR);

			uint16x8_t r_out = vcvtq_u16_f16(r_mac);
			r_out = vreinterpretq_u16(vqmovnbq(vdupq_n_u8(0), r_out));
			r_out = vldrbq_gather_offset(lut, r_out);
			vstrbq_scatter_offset(dp + 0, pixel_offsets, r_out);
		}

		{
			float16x8_t g_mac = vmulq(r, C_RG);
			g_mac = vfmaq(g_mac, g, C_GG);
			g_mac = vfmaq(g_mac, b, C_BG);

			uint16x8_t g_out = vcvtq_u16_f16(g_mac);
			g_out = vreinterpretq_u16(vqmovnbq(vdupq_n_u8(0), g_out));
			g_out = vldrbq_gather_offset(lut, g_out);
			vstrbq_scatter_offset(dp + 1, pixel_offsets, g_out);
		}

		{
			float16x8_t b_mac = vmulq(r, C_RB);
			b_mac = vfmaq(b_mac, g, C_GB);
			b_mac = vfmaq(b_mac, b, C_BB);

			uint16x8_t b_out = vcvtq_u16_f16(b_mac);
			b_out = vreinterpretq_u16(vqmovnbq(vdupq_n_u8(0), b_out));
			b_out = vldrbq_gather_offset(lut, b_out);
			vstrbq_scatter_offset(dp + 2, pixel_offsets, b_out);
		}

		dp += 3 * 8;
		len -= 3 * 8;
	}
}
#endif /* PIXELWISE_COLOR_CORRECTION */

#ifdef USE_REC709_OETF
static inline float rec709_oetf(float lum)
{
    if (lum < 0.018f) {
        return 4.5f * lum;
    } else {
        return 1.099f * powf(lum, 0.45f) - 0.099f;
    }
}

#define oetf rec709_oetf

#else
static inline float srgb_oetf(float lum)
{
    if (lum <= 0.0031308f) {
        return 12.92f * lum;
    } else {
        return 1.055f * powf(lum, 1.0f / 2.4f) - 0.055f;
    }
}

#define oetf srgb_oetf
#endif

static const uint8_t *prepare_gamma_lut(void)
{
    static bool inited;
    static uint8_t lut[256];

    if (inited) {
        return lut;
    }

    for (int i = 0; i < 256; i++) {
        lut[i] = 255.0f * oetf(i * (1.0f / 255.0f)) + 0.5f;
    }
    inited = true;
    return lut;
}

void white_balance(int ml_width, int ml_height, const uint8_t *sp, uint8_t *dp)
{
    const uint8_t * restrict lut = prepare_gamma_lut();

#if SKIP_COLOR_CORRECTION
    for (int i = 0; i < ml_width * ml_height * RGB_BYTES; i++) {
        dp[i] = lut[sp[i]];
    }
#elif !PIXELWISE_COLOR_CORRECTION
    bulk_color_correction(sp, dp, ml_width * ml_height * RGB_BYTES, lut);
#else
    for (int index = 0; index < ml_width * ml_height * RGB_BYTES; index += RGB_BYTES) {
        color_correction(&sp[index], &dp[index], lut);
    }
#endif
}
