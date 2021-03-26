/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
#include "stubs_fvp.h"

#include "bsp_core_log.h"

uint32_t GetCoreClock(void)
{
    return 1;
}

void GLCD_Initialize(void) {}

void GLCD_Bitmap(unsigned int x,  unsigned int y,
    unsigned int w, unsigned int h, unsigned short *bitmap)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(w);
    UNUSED(h);
    UNUSED(bitmap);
}

void GLCD_Image(void *data, const uint32_t width, const uint32_t height,
    const uint32_t channels, const uint32_t pos_x,
    const uint32_t pos_y, const uint32_t downsample_factor)
{
    UNUSED(data);
    UNUSED(pos_x);
    UNUSED(pos_y);
    UNUSED(width);
    UNUSED(height);
    UNUSED(channels);
    UNUSED(downsample_factor);
    debug("image display: (x, y, w, h) = (%u, %u, %u, %u)\n",
        pos_x, pos_y, width, height);
    debug("image display: channels = %u, downsample factor = %u\n",
        channels, downsample_factor);
}

void GLCD_Clear(unsigned short color)
{
    UNUSED(color);
}

void GLCD_SetTextColor(unsigned short color)
{
    UNUSED(color);
}

void GLCD_DisplayChar (unsigned int ln, unsigned int col, unsigned char fi,
    unsigned char c)
{
    UNUSED(ln);
    UNUSED(col);
    UNUSED(fi);
    UNUSED(c);
}

void GLCD_DisplayString(unsigned int ln, unsigned int col, unsigned char fi,
    char *s)
{
    UNUSED(ln);
    UNUSED(col);
    UNUSED(fi);
    UNUSED(s);
    debug("text display: %s\n", s);
}

void GLCD_Box(unsigned int x, unsigned int y, unsigned int w, unsigned int h,
    unsigned short color)
{
    UNUSED(x);
    UNUSED(y);
    UNUSED(w);
    UNUSED(h);
    UNUSED(color);
}

void LED_Initialize(uint32_t port)
{
    UNUSED(port);
}

void LED_On(uint32_t num, uint32_t port)
{
    UNUSED(num);
    UNUSED(port);
    debug("LED %u ON\n", num);
}

void LED_Off(uint32_t num, uint32_t port)
{
    UNUSED(num);
    UNUSED(port);
    debug("LED %u OFF\n", num);
}
