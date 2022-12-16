/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef IMAGE_BUFF_H_
#define IMAGE_BUFF_H_

#include "lvgl.h"
#include "image_processing.h"

extern uint8_t rgb_image[CIMAGE_X*CIMAGE_Y*RGB_BYTES];
extern uint8_t raw_image[CIMAGE_X*CIMAGE_Y*RGB_BYTES + 0x460];

extern uint8_t lcd_image[DIMAGE_Y][DIMAGE_X][RGB565_BYTES];
extern lv_color_t lvgl_image[LIMAGE_Y][LIMAGE_X];

#endif /* IMAGE_BUFF_ */
