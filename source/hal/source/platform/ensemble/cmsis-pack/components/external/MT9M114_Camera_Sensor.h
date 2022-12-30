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
 * @file     MT9M114_Camera_Sensor.h
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     29-Sep-2021
 * @brief    ONsemi MT9M114 Camera Sensor driver User Configurations and defines.
 ******************************************************************************/

#ifndef MT9M114_CAMERA_SENSOR_H_
#define MT9M114_CAMERA_SENSOR_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/* System Includes */
#include "RTE_Device.h"

/* Proceed only if MT9M114 Camera Sensor is enabled. */
#if RTE_MT9M114_CAMERA_SENSOR_ENABLE

#include "Camera_Sensor.h"
#include "Camera_Sensor_i2c.h"

/* MT9M114 Camera Sensor Slave Address. */
#define MT9M114_CAMERA_SENSOR_SLAVE_ADDR                      0x48

/* Supported MT9M114 Camera Sensor Output Format.
 *  (Currently supports only RAW BAYER10 Format.)
 */
#define MT9M114_CAMERA_OUTPUT_FORMAT_RAW_BAYER10              0

/* User can select from supported MT9M114 Camera Sensor Output Format. */
#define MT9M114_USER_SELECT_CAMERA_OUTPUT_FORMAT              MT9M114_CAMERA_OUTPUT_FORMAT_RAW_BAYER10

/* As per data-sheet,
 * When using any of the RAW Bayer modes,
 *  it is essential that the user adds on the
 *  additional border pixels 8x8(WxH) for demosaic.
 */
#if (MT9M114_USER_SELECT_CAMERA_OUTPUT_FORMAT == MT9M114_CAMERA_OUTPUT_FORMAT_RAW_BAYER10)
#define MT9M114_RAW_BAYER_FORMAT_ADDITIONAL_BORDER_WIDTH      8
#define MT9M114_RAW_BAYER_FORMAT_ADDITIONAL_BORDER_HEIGHT     8

#define MT9M114_ADDITIONAL_WIDTH                              MT9M114_RAW_BAYER_FORMAT_ADDITIONAL_BORDER_WIDTH
#define MT9M114_ADDITIONAL_HEIGHT                             MT9M114_RAW_BAYER_FORMAT_ADDITIONAL_BORDER_HEIGHT
#else
#define MT9M114_ADDITIONAL_WIDTH                              0
#define MT9M114_ADDITIONAL_HEIGHT                             0
#endif

/* MT9M114 Camera Sensor CHIP-ID registers */
#define MT9M114_CHIP_ID_REGISTER                              0x0000
#define MT9M114_CHIP_ID_REGISTER_VALUE                        0x2481

/* MT9M114 Camera Sensor Command registers */
#define MT9M114_COMMAND_REGISTER                              0x0080
#define MT9M114_COMMAND_REGISTER_SET_STATE                   (1 << 1)
#define MT9M114_COMMAND_REGISTER_OK                          (1 << 15)

/* MT9M114 Camera Sensor Sysctl registers */
#define MT9M114_SYSCTL_REGISTER_RESET_AND_MISC_CONTROL        0x001A
#define MT9M114_SYSCTL_REGISTER_SLEW_RATE_CONTROL             0x001e

/* MT9M114 Camera Sensor Camera Output Format Control registers */
#define MT9M114_CAM_OUTPUT_FORMAT_REGISTER                        0xc86c
#define MT9M114_CAM_OUTPUT_FORMAT_REGISTER_FORMAT_BAYER          (2 << 8)
#define MT9M114_CAM_OUTPUT_FORMAT_REGISTER_BAYER_FORMAT_RAWR10   (0 << 10)

/* MT9M114 Camera Sensor System Manager registers */
#define MT9M114_SYSMGR_NEXT_STATE                             0xdc00

/* MT9M114 Camera Sensor System States */
#define MT9M114_SYS_STATE_ENTER_CONFIG_CHANGE                 0x28

#define MT9M114_SYS_STATE_STREAMING                           0x31
#define MT9M114_SYS_STATE_START_STREAMING                     0x34

#define MT9M114_SYS_STATE_ENTER_SUSPEND                       0x40
#define MT9M114_SYS_STATE_SUSPENDED                           0x41

#define MT9M114_SYS_STATE_ENTER_STANDBY                       0x50
#define MT9M114_SYS_STATE_STANDBY                             0x52
#define MT9M114_SYS_STATE_LEAVE_STANDBY                       0x54

/**
\brief MT9M114 Camera Sensor Register Array Structure
        used for Camera Resolution Configuration.
*/
typedef struct _MT9M114_REG {
  uint16_t reg_addr;             /* MT9M114 Camera Sensor Register Address                     */
  uint8_t  reg_size;             /* MT9M114 Camera Sensor Register Size: only valid 1/2/4 Byte */
  uint32_t reg_value;            /* MT9M114 Camera Sensor Register Value                       */
} MT9M114_REG;

/**
\brief MT9M114 Camera Sensor Resolution VGA 640x480
\note  Register Values are generated using
       MT9M114(SOC1040) Register Wizard Tool with
       below settings.
        - Image Timing :
          - Image                : VGA Binning
          - Frame Rate           : 5 Frame Per Second
          - Horizontal Mirror    : Enable
          - Vertical Flip        : Enable
        - PLL Setting :
          - Input Frequency      : 24 MHz
          - Target PLL Frequency : 96 MHz
          - Output mode          : Parallel
*/
const MT9M114_REG mt9m114_cam_resolution_VGA_640x480[] =
{
  { 0xC97E, 2, 0x01    }, /* cam_sysctl_pll_enable = 1 */

  { 0xC980, 2, 0x0120  }, /* cam_sysctl_pll_divider_m_n = 288 */
  { 0xC982, 2, 0x0700  }, /* cam_sysctl_pll_divider_p = 1792 */

  { 0xC984, 2, 0x8000  }, /* cam_port_output_control = 32768 (No pixel clock slow down) */

  { 0xC800, 2, 0x0000  }, /* cam_sensor_cfg_y_addr_start = 0 */
  { 0xC802, 2, 0x0000  }, /* cam_sensor_cfg_x_addr_start = 0 */
  { 0xC804, 2, 0x03CD  }, /* cam_sensor_cfg_y_addr_end = 973 */
  { 0xC806, 2, 0x050D  }, /* cam_sensor_cfg_x_addr_end = 1293 */

  { 0xC808, 4, 0x2DC6C00 }, /* cam_sensor_cfg_pixclk = 48000000 */

  { 0xC80C, 2, 0x0001  }, /* cam_sensor_cfg_row_speed = 1 */

  { 0xC80E, 2, 0x01C3  }, /* cam_sensor_cfg_fine_integ_min = 451 */
  { 0xC810, 2, 0x28F8  }, /* cam_sensor_cfg_fine_integ_max = 10488 */

  { 0xC812, 2, 0x036C  }, /* cam_sensor_cfg_frame_length_lines = 876 */

  { 0xC814, 2, 0x29E3  }, /* cam_sensor_cfg_line_length_pck = 10723 */

  { 0xC816, 2, 0x00E0  }, /* cam_sensor_cfg_fine_correction = 224 */

  { 0xC818, 2, 0x01E3  }, /* cam_sensor_cfg_cpipe_last_row = 483 */

  { 0xC826, 2, 0x0020  }, /* cam_sensor_cfg_reg_0_data = 32 */

  { 0xC834, 2, 0x0333  }, /* cam_sensor_control_read_mode = 819, H and V flip */

  { 0xC854, 2, 0x0000  }, /* cam_crop_window_xoffset = 0 */
  { 0xC856, 2, 0x0000  }, /* cam_crop_window_yoffset = 0 */

  { 0xC858, 2, 0x0280  }, /* cam_crop_window_width = 640 */
  { 0xC85A, 2, 0x01E0  }, /* cam_crop_window_height = 480 */

  { 0xC85C, 1, 0x03    }, /* cam_crop_cropmode = 3 */

  { 0xC868, 2, 0x0280  }, /* cam_output_width = 640 */
  { 0xC86A, 2, 0x01E0  }, /* cam_output_height = 480 */

  { 0xC878, 1, 0x00    }, /* cam_aet_aemode = 0 */

  { 0xC88C, 2, 0x051C  }, /* cam_aet_max_frame_rate = 1308, (5 fps) */
  { 0xC88E, 2, 0x051C  }, /* cam_aet_min_frame_rate = 1308, (5 fps) */

  { 0xC914, 2, 0x0000  }, /* cam_stat_awb_clip_window_xstart = 0 */
  { 0xC916, 2, 0x0000  }, /* cam_stat_awb_clip_window_ystart = 0 */

  { 0xC918, 2, 0x027F  }, /* cam_stat_awb_clip_window_xend = 639 */
  { 0xC91A, 2, 0x01DF  }, /* cam_stat_awb_clip_window_yend = 479 */

  { 0xC91C, 2, 0x0000  }, /* cam_stat_ae_initial_window_xstart = 0 */
  { 0xC91E, 2, 0x0000  }, /* cam_stat_ae_initial_window_ystart = 0 */

  { 0xC920, 2, 0x007F  }, /* cam_stat_ae_initial_window_xend = 127 */
  { 0xC922, 2, 0x005F  }, /* cam_stat_ae_initial_window_yend = 95 */

  { 0xA404, 2, 0x0003  }, /* Adaptive Weighted AE for lowlights
                             @NOTE: User can set as per Camera environment
                             refer data-sheet. */
};

/* For debugging,
 *  if required Enable MT9M114 Camera Sensor Test-Pattern.
 */
#define MT9M114_CAMERA_TEST_PATTERN_ENABLE     0

/* if MT9M114 Camera Sensor Test-Pattern is Enabled. */
#if MT9M114_CAMERA_TEST_PATTERN_ENABLE

/* Enable any one of Test-Pattern:
 *  - Color Bar
 *  - Walking 1's 08-bit
 *  - Walking 1's 10-bit
 */
#define MT9M114_CAMERA_TEST_PATTERN_COLOR_BAR_ENABLE            1   /* OR */
#define MT9M114_CAMERA_TEST_PATTERN_WALKING_1s_08_BIT_ENABLE    0   /* OR */
#define MT9M114_CAMERA_TEST_PATTERN_WALKING_1s_10_BIT_ENABLE    0
#endif

/* if MT9M114 Camera Sensor Test-Pattern is Enabled. */
#if MT9M114_CAMERA_TEST_PATTERN_ENABLE
const MT9M114_REG mt9m114_cam_testPattern[] =
{
  /* select operation mode as test-pattern generator. */
  {0xc84c, 1, 0x02},

  /* MT9M114 Camera Test Pattern: Color Bar */
#if MT9M114_CAMERA_TEST_PATTERN_COLOR_BAR_ENABLE
  {0xc84d, 1, 0x04},

  /* MT9M114 Camera Test Pattern: Walking 1's 08-bit */
#elif MT9M114_CAMERA_TEST_PATTERN_WALKING_1s_08_BIT_ENABLE
  {0xc84d, 1, 0x0B},

  /* MT9M114 Camera Test Pattern: Walking 1's 10-bit */
#elif MT9M114_CAMERA_TEST_PATTERN_WALKING_1s_10_BIT_ENABLE
  {0xc84d, 1, 0x0A},
#endif
};
#endif /* end of MT9M114_CAMERA_TEST_PATTERN_ENABLE */

/* Initialize MT9M114 Camera Sensor, also
 * initialize i2c using i3c driver instance */
int32_t mt9m114_Init(ARM_CAMERA_RESOLUTION cam_resolution);

/* Un-initialize MT9M114 Camera Sensor. */
int32_t mt9m114_Uninit(void);

/* Start MT9M114 Camera Sensor Streaming. */
int32_t mt9m114_Start(void);

/* Stop MT9M114 Camera Sensor Streaming. */
int32_t mt9m114_Stop(void);

/* Control MT9M114 Camera Sensor. */
int32_t mt9m114_Control(uint32_t control, uint32_t arg);

/**
\brief MT9M114 Camera Sensor slave i2c Configuration
        \ref CAMERA_SENSOR_SLAVE_I2C_CONFIG
*/
CAMERA_SENSOR_SLAVE_I2C_CONFIG mt9m114_camera_sensor_i2c_cnfg =
{
  .I3Cx_instance                  = RTE_MT9M114_CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE,
  .speed_mode                     = CAMERA_SENSOR_I2C_SPEED_SS_100_KBPS,
  .cam_sensor_slave_addr          = MT9M114_CAMERA_SENSOR_SLAVE_ADDR,
  .cam_sensor_slave_reg_addr_type = CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT,
};

/**
\brief MT9M114 Camera Sensor Information
        \ref CAMERA_SENSOR_INFO
*/
CAMERA_SENSOR_INFO mt9m114_info =
{
  .output_format     = MT9M114_USER_SELECT_CAMERA_OUTPUT_FORMAT,
  .additional_width  = MT9M114_ADDITIONAL_WIDTH,
  .additional_height = MT9M114_ADDITIONAL_HEIGHT,
};

/**
\brief MT9M114 Camera Sensor Configurations
        \ref CAMERA_SENSOR_CONFIG
*/
CAMERA_SENSOR_CONFIG mt9m114_config =
{
#if RTE_MT9M114_CAMERA_SENSOR_INTERFACE_PARALLEL_ENABLE
  .interface    = CAMERA_SENSOR_INTERFACE_PARALLEL,
#endif

  .clock_source = RTE_MT9M114_CAMERA_CLOCK_SOURCE,
  .pixelclk_pol = RTE_MT9M114_CAMERA_SENSOR_PIXEL_CLK_POL,
  .hsync_pol    = RTE_MT9M114_CAMERA_SENSOR_HSYNC_POL,
  .vsync_pol    = RTE_MT9M114_CAMERA_SENSOR_VSYNC_POL,
  .hsync_mode   = RTE_MT9M114_CAMERA_SENSOR_HSYNC_MODE,
  .data_mode    = RTE_MT9M114_CAMERA_SENSOR_DATA_MODE,
  .data_mask    = RTE_MT9M114_CAMERA_SENSOR_DATA_MASK,
};

/**
\brief MT9M114 Camera Sensor Operations
        \ref CAMERA_SENSOR_OPERATIONS
*/
CAMERA_SENSOR_OPERATIONS mt9m114_ops =
{
  .Init    = mt9m114_Init,
  .Uninit  = mt9m114_Uninit,
  .Start   = mt9m114_Start,
  .Stop    = mt9m114_Stop,
  .Control = mt9m114_Control,
};

/**
\brief MT9M114 Camera Sensor Device Structure
       Contains:
        - MT9M114 Camera Sensor Information
        - MT9M114 Camera Sensor Configurations
        - MT9M114 Camera Sensor Operations
        \ref CAMERA_SENSOR_DEVICE
*/
extern CAMERA_SENSOR_DEVICE mt9m114_camera_sensor;
CAMERA_SENSOR_DEVICE mt9m114_camera_sensor =
{
  .Info    = &mt9m114_info,
  .Config  = &mt9m114_config,
  .Ops     = &mt9m114_ops,
};

#endif /* RTE_MT9M114_CAMERA_SENSOR_ENABLE */


#ifdef  __cplusplus
}
#endif

#endif /* MT9M114_CAMERA_SENSOR_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
