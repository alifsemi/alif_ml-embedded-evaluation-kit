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
 * @file     Camera_Sensor.h
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     01-Sep-2021
 * @brief    Camera Sensor Device definitions.
 ******************************************************************************/

#ifndef CAMERA_SENSOR_H_
#define CAMERA_SENSOR_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "Camera_Common.h"

/**
\brief Camera Clock Source
*/
typedef enum _CAMERA_CLOCK_SOURCE {
  CAMERA_CLOCK_SOURCE_INTERNAL_CAMERA_CONTROLLER = 0,   /* Clock Source: Internal from Camera Controller    */
  CAMERA_CLOCK_SOURCE_EXTERNAL_CAMERA_SENSOR,           /* Clock Source: External from Camera Sensor Device */
} CAMERA_CLOCK_SOURCE;

/**
\brief Camera Sensor Interface
*/
typedef enum _CAMERA_SENSOR_INTERFACE {
  CAMERA_SENSOR_INTERFACE_PARALLEL = 0,          /* Camera Sensor Interface: Parallel */
  CAMERA_SENSOR_INTERFACE_MIPI_CSI,              /* Camera Sensor Interface: MIPI_CSI */
} CAMERA_SENSOR_INTERFACE;

/**
\brief Camera Sensor Polarity
*/
typedef enum _CAMERA_SENSOR_POLARITY {
  CAMERA_SENSOR_POLARITY_NOT_INVERT = 0,         /* Camera Sensor Polarity: Not Invert */
  CAMERA_SENSOR_POLARITY_INVERT,                 /* Camera Sensor Polarity: Invert     */
} CAMERA_SENSOR_POLARITY;

/**
\brief Camera Sensor HSYNC Mode
*/
typedef enum _CAMERA_SENSOR_HSYNC_MODE {
  CAMERA_SENSOR_HSYNC_SIG_IS_HYNC = 0,           /* Camera Sensor HSYNC MODE: HSYNC signal is HYNC        */
  CAMERA_SENSOR_HSYNC_SIG_IS_DATA_EN,            /* Camera Sensor HSYNC MODE: HSYNC signal is Data Enable */
} CAMERA_SENSOR_HSYNC_MODE;

/**
\brief Camera Sensor Data Mode
*/
typedef enum _CAMERA_SENSOR_DATA_MODE {
  CAMERA_SENSOR_DATA_MODE_8_BIT = 0,             /* Camera Sensor Data Mode:  8-bit */
  CAMERA_SENSOR_DATA_MODE_16_BIT,                /* Camera Sensor Data Mode: 16-bit */
  CAMERA_SENSOR_DATA_MODE_32_BIT,                /* Camera Sensor Data Mode: 32-bit */
  CAMERA_SENSOR_DATA_MODE_64_BIT                 /* Camera Sensor Data Mode: 64-bit */
} CAMERA_SENSOR_DATA_MODE;

/**
\brief Camera Sensor Data Mask
 @Note No impact on 8-bit mode
*/
typedef enum _CAMERA_SENSOR_DATA_MASK {
  CAMERA_SENSOR_DATA_MASK_10_BIT = 0,            /* Camera Sensor Data Mask: 10-bit */
  CAMERA_SENSOR_DATA_MASK_12_BIT,                /* Camera Sensor Data Mask: 12-bit */
  CAMERA_SENSOR_DATA_MASK_14_BIT,                /* Camera Sensor Data Mask: 14-bit */
  CAMERA_SENSOR_DATA_MASK_16_BIT                 /* Camera Sensor Data Mask: 16-bit */
} CAMERA_SENSOR_DATA_MASK;

/**
\brief CAMERA Sensor Device Information
*/
typedef struct _CAMERA_SENSOR_INFO {
  uint8_t output_format;     /* Camera Sensor Image Output Format Mono/RAW-Bayer/RGB/YUV */
  uint8_t additional_width;  /* if Camera Sensor requires additional width  (ex: in case of border pixel for demosaic.) */
  uint8_t additional_height; /* if Camera Sensor requires additional height (ex: in case of border pixel for demosaic.) */
} CAMERA_SENSOR_INFO;

/**
\brief CAMERA Sensor Device Configurations
*/
typedef struct _CAMERA_SENSOR_CONFIG {
  CAMERA_CLOCK_SOURCE      clock_source;         /* Camera Clock Source                 */
  CAMERA_SENSOR_INTERFACE  interface;            /* Camera Sensor Interface             */
  CAMERA_SENSOR_POLARITY   pixelclk_pol;         /* Camera Sensor Pixel Clock Polarity  */
  CAMERA_SENSOR_POLARITY   hsync_pol;            /* Camera Sensor HSYNC Polarity        */
  CAMERA_SENSOR_POLARITY   vsync_pol;            /* Camera Sensor VSYNC Polarity        */
  CAMERA_SENSOR_HSYNC_MODE hsync_mode;           /* Camera Sensor HSYNC Mode            */
  CAMERA_SENSOR_DATA_MODE  data_mode;            /* Camera Sensor Data Mode             */
  CAMERA_SENSOR_DATA_MASK  data_mask;            /* Camera Sensor Data Mask             */
} CAMERA_SENSOR_CONFIG;

/**
\brief CAMERA Sensor Device Operations
*/
typedef struct _CAMERA_SENSOR_OPERATIONS {
  int32_t (*Init)    (ARM_CAMERA_RESOLUTION camera_resolution);   /* Initialize Camera Sensor device    */
  int32_t (*Uninit)  (void);                                      /* De-initialize Camera Sensor device */
  int32_t (*Start)   (void);                                      /* Start Camera Sensor device         */
  int32_t (*Stop)    (void);                                      /* Stop Camera Sensor device          */
  int32_t (*Control) (uint32_t control, uint32_t arg);            /* Control Camera Sensor device       */
} CAMERA_SENSOR_OPERATIONS;

/**
\brief CAMERA Sensor Device
*/
typedef struct _CAMERA_SENSOR_DEVICE {
  CAMERA_SENSOR_INFO       *Info;                /* Camera Sensor device Information    */
  CAMERA_SENSOR_CONFIG     *Config;              /* Camera Sensor device Configurations */
  CAMERA_SENSOR_OPERATIONS *Ops;                 /* Camera Sensor device Operations     */
} CAMERA_SENSOR_DEVICE;

#ifdef  __cplusplus
}
#endif

#endif /* CAMERA_SENSOR_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
