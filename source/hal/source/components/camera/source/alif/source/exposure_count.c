/* Copyright (C) 2022-2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdint.h>
#include "exposure_count.h"

#if __ARM_FEATURE_MVE & 1
#include <arm_mve.h>
#endif

// Exposure analysis thresholds (linear values corresponding to 20%/80% after sRGB gamma)
#define EXPOSURE_THRESH_LOW  9
#define EXPOSURE_THRESH_HIGH 154

uint32_t exposure_over_count, exposure_high_count, exposure_low_count, exposure_under_count;

void exposure_count_bayer(const uint8_t *bayer, int width, int height)
{
    uint32_t over_count = 0;
    uint32_t high_count = 0;
    uint32_t not_low_count = 0;
    uint32_t under_count = 0;
    int pairs = (width - 1) / 2;
    int rows = height - 1;

#if __ARM_FEATURE_MVE & 1
    for (int y = 0; y < rows; y++) {
        const uint8_t *row = bayer + y * width;
        int pairs_to_go = pairs;
        const uint8_t *p = row;

        while (pairs_to_go > 0) {
            mve_pred16_t pred = vctp8q(pairs_to_go);
            uint8x16x2_t px = vld2q(p);
            uint8x16_t max_val = vmaxq_x(px.val[0], px.val[1], pred);
            uint8x16_t min_val = vminq_x(px.val[0], px.val[1], pred);

            mve_pred16_t high = vcmpcsq_m(max_val, EXPOSURE_THRESH_HIGH, pred);
            high_count = vaddvaq_p(high_count, vdupq_n_u8(1), high);

            mve_pred16_t over = vcmpeqq_m(max_val, 255, pred);
            over_count = vaddvaq_p(over_count, vdupq_n_u8(1), over);

            mve_pred16_t not_low = vcmpcsq_m(min_val, EXPOSURE_THRESH_LOW, pred);
            not_low_count = vaddvaq_p(not_low_count, vdupq_n_u8(1), not_low);

            mve_pred16_t under = vcmpeqq_m(min_val, 0, pred);
            under_count = vaddvaq_p(under_count, vdupq_n_u8(1), under);

            p += 16 * 2;
            pairs_to_go -= 16;
        }
    }
#else
    for (int y = 0; y < rows; y++) {
        const uint8_t *row = bayer + y * width;
        for (int x = 0; x < pairs * 2; x += 2) {
            if (row[x] == 255 || row[x + 1] == 255) {
                over_count++;
            }
            if (row[x] >= EXPOSURE_THRESH_HIGH || row[x + 1] >= EXPOSURE_THRESH_HIGH) {
                high_count++;
            }
            if (row[x] >= EXPOSURE_THRESH_LOW && row[x + 1] >= EXPOSURE_THRESH_LOW) {
                not_low_count++;
            }
            if (row[x] == 0 || row[x + 1] == 0) {
                under_count++;
            }
        }
    }
#endif

    exposure_over_count = over_count * 2;
    exposure_high_count = high_count * 2;
    exposure_low_count = (width - 2) * (height - 1) - not_low_count * 2;
    exposure_under_count = under_count * 2;
}
