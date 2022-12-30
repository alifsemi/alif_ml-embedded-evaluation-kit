/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     DSI_LCD_Panels.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     19-July-2022
 * @brief    DSI LCD Panel Specific Header file for MIPI DSI Driver.
 ******************************************************************************/

#ifndef DSI_LCD_PANELS_H_
#define DSI_LCD_PANELS_H_

#if (RTE_MIPI_DSI)

/* Check ILI9806E Display Panel is enabled */
#if (RTE_MIPI_DSI_ILI9806E_PANEL)
  extern DISPLAY_PANEL_DEVICE ILI9806E_display_panel;
  DISPLAY_PANEL_DEVICE *display_panel = &ILI9806E_display_panel;
#else
#error "MIPI DSI LCD Panel is not Selected"
#endif

#endif /*RTE_MIPI_DSI*/

#endif /* DSI_LCD_PANELS_H_ */
