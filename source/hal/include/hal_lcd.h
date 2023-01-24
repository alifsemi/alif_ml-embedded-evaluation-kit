/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HAL_LCD_H
#define HAL_LCD_H
/**
 * This file is the top level abstraction for the LCD related functions
 **/

#include "lcd_img.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * See lcd_img.h for function docstrings.
 * In the following macro definitions:
 * d = data (pointer)
 * w = width
 * h = height
 * c = channels
 * x = x position
 * y = y position
 * s = down-sample factor (images)
 * m = allow multiple lines (text)
 * cl = colour
 */
#define hal_lcd_init()                          lcd_init()
#define hal_lcd_display_image(d,w,h,c,x,y,s)    lcd_display_image(d,w,h,c,x,y,s)
#define hal_lcd_display_text(s,l,x,y,m)         lcd_display_text(s,l,x,y,m)
#define hal_lcd_display_box(x,y,w,h,cl)         lcd_display_box(x,y,w,h,cl)
#define hal_lcd_clear(cl)                       lcd_clear(cl)
#define hal_lcd_set_text_color(cl)              lcd_set_text_color(cl)

#endif /* HAL_LCD_H */
