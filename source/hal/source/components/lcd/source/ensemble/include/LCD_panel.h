/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef ILI9806E_LCD_PANEL_LCD_PANEL_H_
#define ILI9806E_LCD_PANEL_LCD_PANEL_H_


int Display_reset(void);
int Display_BL_LED_EN(void);
int Display_initialization(uint8_t *buffer);


#endif /* ILI9806E_LCD_PANEL_LCD_PANEL_H_ */
