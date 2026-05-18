/* Copyright (C) 2022-2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef EXPOSURE_COUNT_H
#define EXPOSURE_COUNT_H

#include <stdint.h>

extern uint32_t exposure_over_count;
extern uint32_t exposure_high_count;
extern uint32_t exposure_low_count;
extern uint32_t exposure_under_count;

void exposure_count_bayer(const uint8_t *bayer, int width, int height);

#endif /* EXPOSURE_COUNT_H */
