/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef LV_PAINT_UTILS_H_
#define LV_PAINT_UTILS_H_

#include "lvgl.h"
#include "image_processing.h"

#ifdef __cplusplus
extern "C" {
#endif

void write_to_lvgl_buf_doubled(
        int width, int height,
        const uint8_t *src,
        lv_color_t *dst);
void write_to_lvgl_buf(
        int width, int height,
        const uint8_t *src,
        lv_color_t *dst);

#ifdef __cplusplus
}
#endif

#endif /* LV_PAINT_UTILS_H_ */
