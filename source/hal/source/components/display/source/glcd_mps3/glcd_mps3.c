/*
 * SPDX-FileCopyrightText: Copyright 2022-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "glcd.h"

#include "log_macros.h"
#include "font_9x15_h.h"

#define CLCD_CS_Pos        0
#define CLCD_CS_Msk        (1UL<<CLCD_CS_Pos)
#define SHIELD_0_nCS_Pos   1
#define SHIELD_0_nCS_Msk   (1UL<<SHIELD_0_nCS_Pos)
#define SHIELD_1_nCS_Pos   2
#define SHIELD_1_nCS_Msk   (1UL<<SHIELD_1_nCS_Pos)
#define CLCD_RESET_Pos     3
#define CLCD_RESET_Msk     (1UL<<CLCD_RESET_Pos)
#define CLCD_RS_Pos        4
#define CLCD_RS_Msk        (1UL<<CLCD_RS_Pos)
#define CLCD_RD_Pos        5
#define CLCD_RD_Msk        (1UL<<CLCD_RD_Pos)
#define CLCD_BL_Pos        6
#define CLCD_BL_Msk        (1UL<<CLCD_BL_Pos)

/*-------------- CLCD Controller Internal Register addresses ----------------*/
#define CHAR_COM        ((volatile unsigned int *)(CLCD_CONFIG_BASE + 0x000))
#define CHAR_DAT        ((volatile unsigned int *)(CLCD_CONFIG_BASE + 0x004))
#define CHAR_RD         ((volatile unsigned int *)(CLCD_CONFIG_BASE + 0x008))
#define CHAR_RAW        ((volatile unsigned int *)(CLCD_CONFIG_BASE + 0x00C))
#define CHAR_MASK       ((volatile unsigned int *)(CLCD_CONFIG_BASE + 0x010))
#define CHAR_STAT       ((volatile unsigned int *)(CLCD_CONFIG_BASE + 0x014))
#define CHAR_MISC       ((volatile unsigned int *)(CLCD_CONFIG_BASE + 0x04C))

/*--------------- Graphic LCD interface hardware definitions -----------------*/
/* Pin CS setting to 0 or 1                                                   */
#define LCD_CS(x)   ((x) ? (*CHAR_MISC |= CLCD_CS_Msk)    : (*CHAR_MISC &= ~CLCD_CS_Msk))
#define LCD_RST(x)  ((x) ? (*CHAR_MISC |= CLCD_RESET_Msk) : (*CHAR_MISC &= ~CLCD_RESET_Msk))
#define LCD_BL(x)   ((x) ? (*CHAR_MISC |= CLCD_BL_Msk)    : (*CHAR_MISC &= ~CLCD_BL_Msk))

#define BG_COLOR  0                     /* Background colour                  */
#define TXT_COLOR 1                     /* Text colour                        */

/**
* Text and background colour
*/
static volatile unsigned short Color[2] = {Black, White};

/**
 * @brief     Delay in while loop cycles.
 * @param[in] cnt    Number of while cycles to delay.
 **/
static void delay (int cnt)
{
    cnt <<= DELAY_2N;
    while (cnt != 0) {
        --cnt;
    }
}

/**
 * @brief       Write a command the LCD controller.
 * @param[in]   cmd    Command to be written.
 */
static __inline void wr_cmd(unsigned char cmd)
{
    LCD_CS(0);
    *CHAR_COM = cmd;
    LCD_CS(1);
}

/**
 * @brief       Start of data writing to the LCD controller.
 */
static __inline void wr_dat_start (void)
{
    LCD_CS(0);
}

/**
 * @brief       Stop of data writing to the LCD controller.
 */
static __inline void wr_dat_stop (void)
{
    LCD_CS(1);
}

/**
 * @brief       Data writing to the LCD controller.
 * @param[in]   dat    Data to be written.
 */
static __inline void wr_dat_only(unsigned short dat)
{
    *CHAR_DAT = (dat >>   8);   /* Write D8..D15    */
    *CHAR_DAT = (dat & 0xFF);   /* Write D0..D7     */
}

/**
 * @brief       Write a value to the to LCD register.
 * @param[in]   reg    Register to be written.
 * @param[in]   val    Value to write to the register.
 */
static __inline void wr_reg(unsigned char reg, unsigned short val)
{
    LCD_CS(0);
    *CHAR_COM = reg;
    wr_dat_only(val);
    LCD_CS(1);
}

/**
 * @brief       Converts a gray value to RGB565 representation.
 * @param[in]   src_uchar   Pointer to the source pixel.
 * @return      16 bit RGB565 value.
 */
static inline uint16_t _GLCD_Gray8_to_RGB565(uint8_t *src_uchar)
{
    uint16_t val_r = (*src_uchar >> 3);
    uint16_t val_g = (*src_uchar >> 2);
    return ((val_r << 11) | (val_g << 5) | val_r);
}

/**
 * @brief       Converts an RGB888 value to RGB565 representation.
 * @param[in]   src_uchar   Pointer to the source pixel for R (assumed to
 *                          be RGB format).
 * @return      16 bit RGB565 value.
 */
static inline uint16_t _GLCD_RGB888_to_RGB565(uint8_t *src_uchar)
{
    uint16_t val_r = (*src_uchar >> 3) & 0x1F;
    uint16_t val_g = (*(src_uchar+1) >> 2) & 0x3F;
    uint16_t val_b = (*(src_uchar+2) >> 3) & 0x1F;
    return ((val_r << 11) | (val_g << 5) | val_b);
}

/* Helper typedef to encapsulate the colour conversion function
 * signatures */
typedef uint16_t (* std_clr_2_lcd_clr_fn)(uint8_t *src_uchar);

void GLCD_SetWindow(unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
    unsigned int xe, ye;

    xe = x+w-1;
    ye = y+h-1;

    wr_reg(0x02, x  >>    8);   /* Column address start MSB           */
    wr_reg(0x03, x  &  0xFF);   /* Column address start LSB           */
    wr_reg(0x04, xe >>    8);   /* Column address end MSB             */
    wr_reg(0x05, xe &  0xFF);   /* Column address end LSB             */

    wr_reg(0x06, y  >>    8);   /* Row address start MSB              */
    wr_reg(0x07, y  &  0xFF);   /* Row address start LSB              */
    wr_reg(0x08, ye >>    8);   /* Row address end MSB                */
    wr_reg(0x09, ye &  0xFF);   /* Row address end LSB                */
}

void GLCD_WindowMax(void)
{
    GLCD_SetWindow (0, 0, GLCD_WIDTH, GLCD_HEIGHT);
}

void GLCD_SetTextColor(unsigned short color)
{
    Color[TXT_COLOR] = color;
}

void GLCD_SetBackColor(unsigned short color)
{
    Color[BG_COLOR] = color;
}

void GLCD_Clear(unsigned short color)
{
    unsigned int i;

    GLCD_WindowMax();
    wr_cmd(0x22);
    wr_dat_start();

    for(i = 0; i < (GLCD_WIDTH*GLCD_HEIGHT); ++i) {
        wr_dat_only(color);
    }
    wr_dat_stop();
}


void GLCD_DrawChar(
        unsigned int x, unsigned int y,
        unsigned int cw, unsigned int ch,
        unsigned char *c)
{
    unsigned int i, j, k, pixs;

    /* Heatlh check: out of bounds? */
    if ((x + cw) > GLCD_WIDTH || (y + ch) > GLCD_HEIGHT) {
        return;
    }

    GLCD_SetWindow(x, y, cw, ch);

    wr_cmd(0x22);
    wr_dat_start();

    k  = (cw + 7)/8;

    if (k == 1) {
        for (j = 0; j < ch; ++j) {
            pixs = *(unsigned char  *)c;
            c += 1;

            for (i = 0; i < cw; ++i) {
                wr_dat_only (Color[(pixs >> i) & 1]);
            }
        }
    }
    else if (k == 2) {
        for (j = 0; j < ch; ++j) {
            pixs = *(unsigned short *)c;
            c += 2;

            for (i = 0; i < cw; ++i) {
                wr_dat_only (Color[(pixs >> i) & 1]);
            }
        }
    }
    wr_dat_stop();
}

void GLCD_DisplayChar(
        unsigned int ln, unsigned int col,
        unsigned char fi, unsigned char c)
{
    c -= 32;
    switch (fi) {
        case 0: /* Font 9 x 15. */
            GLCD_DrawChar(col * 9, ln * 15, 9, 15,
                         (unsigned char *)&Font_9x15_h[c * 15]);
            break;
    }
}

void GLCD_DisplayString(
        unsigned int ln, unsigned int col,
        unsigned char fi, char *s)
{
  while (*s) {
    GLCD_DisplayChar(ln, col++, fi, *s++);
  }
}



void GLCD_ClearLn(unsigned int ln, unsigned char fi)
{
    unsigned char i;
    char buf[60];

    GLCD_WindowMax();
    switch (fi) {
        case 0:  /* Font 9x15*/
            for (i = 0; i < (GLCD_WIDTH+8)/9; ++i) {
                buf[i] = ' ';
            }
            buf[i+1] = 0;
            break;
    }
    GLCD_DisplayString (ln, 0, fi, buf);
}

void GLCD_Bitmap(unsigned int x, unsigned int y,
        unsigned int w, unsigned int h,
        unsigned short *bitmap)
{
    unsigned int i;
    unsigned short *bitmap_ptr = bitmap;

    GLCD_SetWindow (x, y, w, h);

    wr_cmd(0x22);
    wr_dat_start();

    for (i = 0; i < (w*h); ++i) {
        wr_dat_only (bitmap_ptr[i]);
    }
    wr_dat_stop();
}

void GLCD_Image(const void *data, const uint32_t width,
    const uint32_t height, const uint32_t channels,
    const uint32_t pos_x, const uint32_t pos_y,
    const uint32_t downsample_factor)
{
    uint32_t i, j = 0; /* for loops */
    const uint32_t x_incr = channels * downsample_factor; /* stride. */
    const uint32_t y_incr = channels * width * (downsample_factor - 1); /* skip rows. */
    uint8_t* src_unsigned = (uint8_t *)data; /* temporary pointer. */
    std_clr_2_lcd_clr_fn cvt_clr_fn = 0; /* colour conversion function. */

    /* Based on number of channels, we decide which of the above functions to use. */
    switch (channels) {
        case 1:
            cvt_clr_fn = _GLCD_Gray8_to_RGB565;
            break;

        case 3:
            cvt_clr_fn = _GLCD_RGB888_to_RGB565;
            break;

        default:
            printf_err("number of channels not supported by display\n");
            return;
    }

    /* Set the window position expected. Note: this is integer div. */
    GLCD_SetWindow(pos_x, pos_y,
        width/downsample_factor, height/downsample_factor);
    wr_cmd(0x22);
    wr_dat_start();

    /* Loop over the image. */
    for (j = height; j != 0; j -= downsample_factor) {
        for (i = width; i != 0; i -= downsample_factor) {
            wr_dat_only(cvt_clr_fn(src_unsigned));
            src_unsigned += x_incr;
        }

        /* Skip rows if needed. */
        src_unsigned += y_incr;
    }

    wr_dat_stop();
}

void GLCD_Box(
        unsigned int x, unsigned int y,
        unsigned int w, unsigned int h,
        unsigned short color)
{
    unsigned int i;

    GLCD_SetWindow (x, y, w, h);

    wr_cmd(0x22);
    wr_dat_start();
    for(i = 0; i < (w*h); ++i){
        wr_dat_only (color);
    }
    wr_dat_stop();
}


void GLCD_Initialize (void)
{
    /* CLCD screen setup (Default CLCD screen interface state) ------------- */
    LCD_CS(1);              /* deassert nCS0. */
    LCD_RST(1);             /* deassert Reset. */
    LCD_BL(0);              /* switch off backlight. */

    /* Reset CLCD screen --------------------------------------------------- */
    LCD_RST(0);             /* assert Reset. */
    delay(1);
    LCD_RST(1);             /* deassert Reset. */
    delay(10);

    /* Driving ability settings ----------------------------------------------*/
    wr_reg(0xEA, 0x00);     /* Power control internal used (1).    */
    wr_reg(0xEB, 0x20);     /* Power control internal used (2).    */
    wr_reg(0xEC, 0x0C);     /* Source control internal used (1).   */
    wr_reg(0xED, 0xC7);     /* Source control internal used (2).   */
    wr_reg(0xE8, 0x38);     /* Source output period Normal mode.   */
    wr_reg(0xE9, 0x10);     /* Source output period Idle mode.     */
    wr_reg(0xF1, 0x01);     /* RGB 18-bit interface ;0x0110.       */
    wr_reg(0xF2, 0x10);

    /* Adjust the Gamma Curve ------------------------------------------------*/
    wr_reg(0x40, 0x01);
    wr_reg(0x41, 0x00);
    wr_reg(0x42, 0x00);
    wr_reg(0x43, 0x10);
    wr_reg(0x44, 0x0E);
    wr_reg(0x45, 0x24);
    wr_reg(0x46, 0x04);
    wr_reg(0x47, 0x50);
    wr_reg(0x48, 0x02);
    wr_reg(0x49, 0x13);
    wr_reg(0x4A, 0x19);
    wr_reg(0x4B, 0x19);
    wr_reg(0x4C, 0x16);

    wr_reg(0x50, 0x1B);
    wr_reg(0x51, 0x31);
    wr_reg(0x52, 0x2F);
    wr_reg(0x53, 0x3F);
    wr_reg(0x54, 0x3F);
    wr_reg(0x55, 0x3E);
    wr_reg(0x56, 0x2F);
    wr_reg(0x57, 0x7B);
    wr_reg(0x58, 0x09);
    wr_reg(0x59, 0x06);
    wr_reg(0x5A, 0x06);
    wr_reg(0x5B, 0x0C);
    wr_reg(0x5C, 0x1D);
    wr_reg(0x5D, 0xCC);

    /* Power voltage setting -------------------------------------------------*/
    wr_reg(0x1B, 0x1B);
    wr_reg(0x1A, 0x01);
    wr_reg(0x24, 0x2F);
    wr_reg(0x25, 0x57);
    wr_reg(0x23, 0x88);

    /* Power on setting ------------------------------------------------------*/
    wr_reg(0x18, 0x36);    /* Internal oscillator frequency adj.  */
    wr_reg(0x19, 0x01);    /* Enable internal oscillator.         */
    wr_reg(0x01, 0x00);    /* Normal mode, no scroll.             */
    wr_reg(0x1F, 0x88);    /* Power control 6 - DDVDH Off.        */
    delay(20);
    wr_reg(0x1F, 0x82);    /* Power control 6 - Step-up: 3 x VCI. */
    delay(5);
    wr_reg(0x1F, 0x92);    /* Power control 6 - Step-up: On.      */
    delay(5);
    wr_reg(0x1F, 0xD2);    /* Power control 6 - VCOML active.     */
    delay(5);

    /* Color selection -------------------------------------------------------*/
    wr_reg(0x17, 0x55);    /* RGB, System interface: 16 Bit/Pixel. */
    wr_reg(0x00, 0x00);    /* Scrolling off, no standby.           */

    /* Interface config ------------------------------------------------------*/
    wr_reg(0x2F, 0x11);    /* LCD Drive: 1-line inversion.        */
    wr_reg(0x31, 0x00);
    wr_reg(0x32, 0x00);    /* DPL=0, HSPL=0, VSPL=0, EPL=0.       */

    /* Display on setting ----------------------------------------------------*/
    wr_reg(0x28, 0x38);    /* PT(0,0) active, VGL/VGL.            */
    delay(20);
    wr_reg(0x28, 0x3C);    /* Display active, VGL/VGL.            */

#if (LANDSCAPE == 1)
#if (ROTATE180 == 0)
    wr_reg (0x16, 0xA8);
#else /* (ROTATE180 == 0) */
    wr_reg (0x16, 0x68);
#endif /* (ROTATE180 == 0) */
#else /* (LANDSCAPE == 1) */
#if (ROTATE180 == 0)
    wr_reg (0x16, 0x08);
#else /* (ROTATE180 == 0) */
     wr_reg (0x16, 0xC8);
#endif /* (ROTATE180 == 0) */
#endif /* (LANDSCAPE == 1) */

    /* Display scrolling settings --------------------------------------------*/
    wr_reg(0x0E, 0x00);         /* TFA MSB */
    wr_reg(0x0F, 0x00);         /* TFA LSB */
    wr_reg(0x10, 320 >> 8);     /* VSA MSB */
    wr_reg(0x11, 320 &  0xFF);  /* VSA LSB */
    wr_reg(0x12, 0x00);         /* BFA MSB */
    wr_reg(0x13, 0x00);         /* BFA LSB */

    LCD_BL(1);                  /* turn on backlight                  */
}
