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
#ifndef BSP_PACK_FASTMODEL_H
#define BSP_PACK_FASTMODEL_H

#include "cmsis.h"                  /* device specific header file    */
#include "peripheral_memmap.h"      /* peripheral memory map definitions */

/****************************************************************************/
/*              Definitions and stub functions for modules currently        */
/*              unavailable on the model                                    */
/****************************************************************************/
#define GLCD_WIDTH      320
#define GLCD_HEIGHT     240
#define Black           0x0000      /*   0,   0,   0 */
#define White           0xFFFF      /* 255, 255, 255 */

/*********************** Clock related functions *****************************/
uint32_t GetCoreClock(void);

/************************  GLCD related functions ****************************/
/**
 * @brief      Initialize the Himax LCD with HX8347-D LCD Controller
 */
void GLCD_Initialize(void);

/**
 * @brief      Display graphical bitmap image at position x horizontally and y
 *             vertically. This function is optimized for 16 bits per pixel
 *             format, it has to be adapted for any other format.
 * @param[in]  x        horizontal position.
 * @param[in]  y        vertical position.
 * @param[in]  w        width of bitmap.
 * @param[in]  h        height of bitmap.
 * @param[in]  bitmap   address at which the bitmap data resides.
 */
void GLCD_Bitmap(unsigned int x,  unsigned int y,
                unsigned int w, unsigned int h,
                unsigned short *bitmap);

/**
 * @brief Displays an 8 bit image, conversion to the LCD's
 *        16 bit codec is done on the fly.
 * @param[in] data      pointer to the full sized image data.
 * @param[in] width     image width.
 * @param[in] height    image height.
 * @param[in] channels  number of channels in the image.
 * @param[in] pos_x     start x position for the LCD.
 * @param[in] pos_y     start y position for the LCD.
 * @param[in] downsample_factor   factor by which the image
 *                                is downsampled by.
 */
void GLCD_Image(void *data, const uint32_t width,
                const uint32_t height, const uint32_t channels,
                const uint32_t pos_x, const uint32_t pos_y,
                const uint32_t downsample_factor);

/**
 * @brief      Clear display
 * @param[in]  color    display clearing color
 */
void GLCD_Clear(unsigned short color);

/**
 * @brief      Set foreground color
 * @param[in]  color    foreground color
 */
void GLCD_SetTextColor(unsigned short color);

/**
 * @brief      Display character on given line
 * @param[in]  ln    line number
 * @param[in]  col   column number
 * @param[in]  fi    font index (0 = 9x15)
 * @param[in]  c     ASCII character
 */
void GLCD_DisplayChar(unsigned int ln, unsigned int col,
                    unsigned char fi, unsigned char  c);

/**
 * @brief      Display string on given line
 * @param[in]  ln    line number
 * @param[in]  col   column number
 * @param[in]  fi    font index (0 = 9x15)
 * @param[in]  s     pointer to string
 */
void GLCD_DisplayString(unsigned int ln, unsigned int col,
                        unsigned char fi, char *s);

/**
 * @brief      Draw box filled with color
 * @param[in]  x        horizontal position
 * @param[in]  y:       vertical position
 * @param[in]  w:       window width in pixels
 * @param[in]  h:       window height in pixels
 * @param[in]  color    box color
 */
void GLCD_Box(unsigned int x, unsigned int y,
            unsigned int w, unsigned int h,
            unsigned short color);

#endif /* BSP_PACK_FASTMODEL_H */
