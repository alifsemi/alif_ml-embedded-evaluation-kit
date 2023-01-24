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
#ifndef GLCD_H
#define GLCD_H

#include <stdint.h>

/******************************************************************************
  Color coding
  GLCD is coded:   15..11 red, 10..5 green, 4..0 blue  (unsigned short)
                   GLCD_R5, GLCD_G6, GLCD_B5
  original coding: 17..12 red, 11..6 green, 5..0 blue
                   ORG_R6,  ORG_G6,  ORG_B6

  ORG_R1..5 = GLCD_R0..4,  ORG_R0 = GLCD_R4
  ORG_G0..5 = GLCD_G0..5,
  ORG_B1..5 = GLCD_B0..4,  ORG_B0 = GLCD_B4

  GLCD RGB color definitions
******************************************************************************/
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255, 0   */
#define White           0xFFFF      /* 255, 255, 255 */

/************************** Orientation  configuration ************************/
#ifndef LANDSCAPE
#define LANDSCAPE   1               /* 1 for landscape, 0 for portrait.    */
#endif
#ifndef ROTATE180
#define ROTATE180   1               /* 1 to rotate the screen for 180 deg. */
#endif

/*------------------------- Speed dependent settings -------------------------*/

/* If processor works on high frequency delay has to be increased, it can be
   increased by factor 2^N by this constant. */
#define DELAY_2N    8

/*---------------------- Graphic LCD size definitions ------------------------*/
#if (LANDSCAPE == 1)
   #define GLCD_WIDTH       320                 /* Screen Width (in pixels). */
   #define GLCD_HEIGHT      240                 /* Screen Height (in pixels). */
#else
   #define GLCD_WIDTH       240                 /* Screen Width (in pixels). */
   #define GLCD_HEIGHT      320                 /* Screen Height (in pixels). */
#endif

#define BPP                 16                  /* Bits per pixel.           */
#define BYPP                ((BPP+7)/8)         /* Bytes per pixel.          */


/**
 * @brief      Initialize the Himax LCD with HX8347-D LCD Controller.
 */
void GLCD_Initialize(void);

/**
 * @brief      Set draw window region to whole screen.
 */
void GLCD_WindowMax(void);

/**
 * @brief      Set draw window region.
 * @param[in]  x  Horizontal position.
 * @param[in]  y  Vertical position.
 * @param[in]  w  Window width in pixel.
 * @param[in]  h  Window height in pixels.
 */
void GLCD_SetWindow(unsigned int x, unsigned int y,
                     unsigned int w, unsigned int h);

/**
 * @brief      Set foreground color.
 * @param[in]  color    Foreground color.
 */
void GLCD_SetTextColor(unsigned short color);

/**
 * @brief      Set background color.
 * @param[in]  color    Background color.
 */
void GLCD_SetBackColor(unsigned short color);

/**
 * @brief      Clear display.
 * @param[in]  color Display clearing color.
 *
 */
void GLCD_Clear(unsigned short color);

/**
 * @brief      Draw character on given position.
 * @param[in]  x     Horizontal position.
 * @param[in]  y     Vertical position.
 * @param[in]  cw    Character width in pixel.
 * @param[in]  ch    Character height in pixels.
 * @param[in]  c     Pointer to character bitmap.
 *
 */
void GLCD_DrawChar(unsigned int x,  unsigned int y,
                  unsigned int cw, unsigned int ch,
                  unsigned char *c);

/**
 * @brief      Display character on given line.
 * @param[in]  ln    Line number.
 * @param[in]  col   Column number.
 * @param[in]  fi    Font index (0 = 9x15).
 * @param[in]  c     ASCII character.
 */
void GLCD_DisplayChar(unsigned int ln, unsigned int col,
                     unsigned char fi, unsigned char  c);


/**
 * @brief      Display string on given line.
 * @param[in]  ln    Line number.
 * @param[in]  col   Column number.
 * @param[in]  fi    Font index (0 = 9x15).
 * @param[in]  s     Pointer to string.
 */
void GLCD_DisplayString(unsigned int ln, unsigned int col,
                        unsigned char fi, char *s);

/**
 * @brief      Clear given line.
 * @param[in]  ln:   Line number.
 * @param[in]  fi    Font index (0 = 9x15).
 */
void GLCD_ClearLn(unsigned int ln, unsigned char fi);

/**
 * @brief      Display graphical bitmap image at position x horizontally and y
 *             vertically. This function is optimized for 16 bits per pixel
 *             format, it has to be adapted for any other format.
 * @param[in]  x        Horizontal position.
 * @param[in]  y        Vertical position.
 * @param[in]  w        Width of bitmap.
 * @param[in]  h        Height of bitmap.
 * @param[in]  bitmap   Address at which the bitmap data resides.
 */
void GLCD_Bitmap(unsigned int x,  unsigned int y,
               unsigned int w, unsigned int h,
               unsigned short *bitmap);

/**
 * @brief Displays an 8 bit image, conversion to the LCD's
 *        16 bit codec is done on the fly.
 * @param[in]  data                 Pointer to the full sized image data.
 * @param[in]  width                Image width.
 * @param[in]  height               Image height.
 * @param[in]  channels             Number of channels in the image.
 * @param[in]  pos_x                Start x position for the LCD.
 * @param[in]  pos_y                Start y position for the LCD.
 * @param[in]  downsample_factor    Factor by which the image
 *                                  is downsampled by.
 */
void GLCD_Image(const void *data, const uint32_t width,
               const uint32_t height, const uint32_t channels,
               const uint32_t pos_x, const uint32_t pos_y,
               const uint32_t downsample_factor);

/**
 * @brief      Draw box filled with color.
 * @param[in]  x        Horizontal position.
 * @param[in]  y        Vertical position.
 * @param[in]  w        Window width in pixels.
 * @param[in]  h        Window height in pixels.
 * @param[in]  color    Box color.
 */
void GLCD_Box(unsigned int x, unsigned int y,
            unsigned int w, unsigned int h,
            unsigned short color);

#endif /* GLCD_H */
