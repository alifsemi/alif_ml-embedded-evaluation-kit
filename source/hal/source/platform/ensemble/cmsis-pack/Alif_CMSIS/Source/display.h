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
 * @file     display.h
 * @author   Girish BN and Prasanna Ravi
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     30-Sep-2021
 * @brief    display driver header.
 * @bug      None.
 * @Note     None
 ******************************************************************************/
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef  __cplusplus
extern "C"
{
#endif

/* Display Panel Control Codes */
#define DISPALY_PANEL_CONFIG  (0x01UL)    ///< Display Panel configure.

/**
  \brief Display Panel Device Operations.
  */
typedef struct {
	int32_t (*Init)    (void);              ///< Initialize Display Panel device.
	int32_t (*Uninit)  (void);              ///< De-initialize Display Panel device.
	int32_t (*Start)   (void);              ///< Start Display Panel device.
	int32_t (*Stop)    (void);              ///< Stop Display Panel device.
	int32_t (*Control) (uint32_t control);  ///< Control Display Panel device.
} DISPLAY_PANEL_OPERATIONS;

/**
  \brief PANEL DPI frame info
  */
typedef struct {
	uint32_t hsync_time;                    ///< Frame hsync time
	uint32_t hbp_time;                      ///< Frame hbp time
	uint32_t hfp_time;                      ///< Frame hfp time
	uint32_t hactive_time;                  ///< Frame hact time
	uint32_t vsync_line;                    ///< Frame vsync line
	uint32_t vbp_line;                      ///< Frame vbp line
	uint32_t vfp_line;                      ///< Frame vfp line
	uint32_t vactive_line;                  ///< Frame vact line
}DISPLAY_PANEL_FRAME_INFO;

/**
  \brief PANEL info
  */
typedef struct {
	uint32_t max_bitrate;                   ///< Maximum bitrate Panel supports
}DISPLAY_PANEL_INFO;
/**
  \brief display panel device structure.
  */
typedef struct
{
	DISPLAY_PANEL_FRAME_INFO *Frame_info;   ///< Display Panel frame information
	DISPLAY_PANEL_INFO       *Info;         ///< Display Panel information
	DISPLAY_PANEL_OPERATIONS *Ops;          ///< Display Panel operations.
}DISPLAY_PANEL_DEVICE;

#ifdef  __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */
