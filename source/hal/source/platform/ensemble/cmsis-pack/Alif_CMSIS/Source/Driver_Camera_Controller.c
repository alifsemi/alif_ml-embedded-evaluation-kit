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
 * @file     Driver_Camera_Controller.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     10-Sep-2021
 * @brief    CMSIS-Driver for Camera Controller
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* System Includes */
#include "RTE_Device.h"

/* Project Includes */
#include "Camera_Sensor.h"
#include "Camera_Controller_dev.h"

#if !(RTE_CAMERA0)
#error "CAMERA is not enabled in the RTE_Device.h"
#endif

#if (!defined(RTE_Drivers_CAMERA0))
#error "CAMERA0 not configured in RTE_Components.h!"
#endif

#if (RTE_MIPI_CSI2)
#include "Driver_MIPI_CSI2.h"
extern ARM_DRIVER_MIPI_CSI2 Driver_MIPI_CSI2;
#endif

#define ARM_CAMERA_CONTROLLER_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /* driver version */

/* CAMERA0 Driver Instance */
#if (RTE_CAMERA0)

/* CAMERA0 Device Resource */
static CAMERA_CTRL_DEV camera0_ctrl =
{
  .reg_base     = (CAMERA_CTRL_REG_MAP *) CAMERA0_BASE,
  .irq_num      = (IRQn_Type) CAMERA0_IRQ,
  .irq_priority = (uint32_t) RTE_CAMERA0_IRQ_PRI,
  .cnfg         = {0},
};
#endif /*End of RTE_CAMERA0 */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_CAMERA_CONTROLLER_API_VERSION,
  ARM_CAMERA_CONTROLLER_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_CAMERA_CONTROLLER_CAPABILITIES DriverCapabilities = {
  1,  /* Supports Camera Controller Snapshot mode,
         In this mode Camera Controller will capture one frame
         then it gets stop.      */
  0   /* Reserved (must be zero) */
};

/**
  \fn      ARM_DRIVER_VERSION CAMERA_GetVersion(void)
  \brief   get Camera version
  \return  driver version
*/
static ARM_DRIVER_VERSION CAMERA_GetVersion(void)
{
  return DriverVersion;
}

/**
  \fn      ARM_CAMERA_CONTROLLER_CAPABILITIES CAMERA_GetCapabilities(void)
  \brief   get Camera Controller capabilites
  \return  driver capabilites
*/
static ARM_CAMERA_CONTROLLER_CAPABILITIES CAMERA_GetCapabilities(void)
{
  return DriverCapabilities;
}

/**
  \fn           void cam_ctrl_disable_interrupt(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        disable Camera Controller interrupt.
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       none
*/
__STATIC_INLINE void cam_ctrl_disable_interrupt(CAMERA_CTRL_DEV *cam_ctrl)
{
  /* Disable Interrupt. */
  cam_ctrl->reg_base->intr_ena = DISABLE;
}

/**
  \fn           void cam_ctrl_enable_interrupt(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        enable Camera Controller interrupt.
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       none
*/
static void cam_ctrl_enable_interrupt(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val = 0;

#if !(RTE_MIPI_DSI)
  /* Enable all Interrupt. */
  reg_val = CAMERA_CTRL_IRQ_CAPTURE_STOP    | \
            CAMERA_CTRL_IRQ_VSYNC_DETECT    | \
            CAMERA_CTRL_IRQ_FIFO_OVERRUN    | \
            CAMERA_CTRL_IRQ_FIFO_UNDERRUN   | \
            CAMERA_CTRL_IRQ_BUS_ERR         | \
            CAMERA_CTRL_IRQ_HBP_ERR         | \
            CAMERA_CTRL_IRQ_HFP_ERR;
#else
  reg_val = CAMERA_CTRL_IRQ_FIFO_OVERRUN    | \
            CAMERA_CTRL_IRQ_FIFO_UNDERRUN   | \
            CAMERA_CTRL_IRQ_VSYNC_DETECT;
#endif

  cam_ctrl->reg_base->intr_ena = reg_val;
}

/**
  \fn           void cam_ctrl_softreset(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        Camera Controller software reset.
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       none
*/
static void cam_ctrl_softreset(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val = 0;

  /* Set Camera Controller Software Reset bit. */
  reg_val = CAMERA_CTRL_SOFT_RESET;
  cam_ctrl->reg_base->cam_ctrl |= reg_val;

  /* Clear Camera Controller Software Reset bit. */
  reg_val = (~CAMERA_CTRL_SOFT_RESET);
  cam_ctrl->reg_base->cam_ctrl &= reg_val;
}

/**
  \fn           uint8_t cam_ctrl_get_capture_status(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        get Camera Controller capture status.
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       capture status
                 0 : CAMERA_CTRL_CAPTURE_STATUS_NOT_CAPTURING
                 1 : CAMERA_CTRL_CAPTURE_STATUS_CAPTURING
*/
static uint8_t cam_ctrl_get_capture_status(CAMERA_CTRL_DEV *cam_ctrl)
{
  /* Check Camera Controller is busy in capturing? */
  cam_ctrl->status.busy = \
    ( (cam_ctrl->reg_base->cam_ctrl & CAMERA_CTRL_CAPTURE_STATUS_BUSY_MASK) \
             ? CAMERA_CTRL_CAPTURE_STATUS_CAPTURING : CAMERA_CTRL_CAPTURE_STATUS_NOT_CAPTURING );

  return cam_ctrl->status.busy;
}

/**
  \fn           int32_t cam_ctrl_get_sensor_config(CAMERA_CTRL_DEV      *cam_ctrl,
                                                   CAMERA_SENSOR_DEVICE *cam_sensor)
  \brief        get Camera Sensor Configurations for Camera Controller.
                 - Interface/Clock Source/
                 - Pixel Clock/HSYNC/VSYC Polarity/
                 - Data Mode/Data Mask/HSYNC Mode
  \param[in]    cam_sensor  : Pointer to Camera Sensor Device resources structure
  \return       camera sensor configuration for camera controller.
*/
static int32_t cam_ctrl_get_sensor_config(CAMERA_CTRL_DEV      *cam_ctrl,
                                          CAMERA_SENSOR_DEVICE *cam_sensor)
{
  uint32_t sensor_cnfg = 0;

  /* Get Camera Clock Source. */
  if(cam_sensor->Config->clock_source == CAMERA_CLOCK_SOURCE_INTERNAL_CAMERA_CONTROLLER)
    sensor_cnfg |= CAMERA_CTRL_CLOCK_SOURCE_INTERNAL;

  else if(cam_sensor->Config->clock_source == CAMERA_CLOCK_SOURCE_EXTERNAL_CAMERA_SENSOR)
    sensor_cnfg |= CAMERA_CTRL_CLOCK_SOURCE_EXTERNAL;

  else
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Get Camera Sensor Interface. */
  if(cam_sensor->Config->interface == CAMERA_SENSOR_INTERFACE_PARALLEL)
    sensor_cnfg |= CAMERA_CTRL_CAMERA_INTERFACE_PARALLEL;

  else if(cam_sensor->Config->interface == CAMERA_SENSOR_INTERFACE_MIPI_CSI)
    sensor_cnfg |= CAMERA_CTRL_CAMERA_INTERFACE_MIPI_CSI;

  else
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Get Camera Sensor Pixel Clock Polarity. */
  if(cam_sensor->Config->pixelclk_pol == CAMERA_SENSOR_POLARITY_NOT_INVERT)
    sensor_cnfg |= CAMERA_CTRL_PIXEL_CLK_POL_NOT_INVERT;

  else if(cam_sensor->Config->pixelclk_pol == CAMERA_SENSOR_POLARITY_INVERT)
    sensor_cnfg |= CAMERA_CTRL_PIXEL_CLK_POL_INVERT;

  else
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Get Camera Sensor HSYNC Polarity. */
  if(cam_sensor->Config->hsync_pol == CAMERA_SENSOR_POLARITY_NOT_INVERT)
    sensor_cnfg |= CAMERA_CTRL_HSYNC_POL_NOT_INVERT;

  else if(cam_sensor->Config->hsync_pol == CAMERA_SENSOR_POLARITY_INVERT)
    sensor_cnfg |= CAMERA_CTRL_HSYNC_POL_INVERT;

  else
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Get Camera Sensor VSYNC Polarity. */
  if(cam_sensor->Config->vsync_pol == CAMERA_SENSOR_POLARITY_NOT_INVERT)
    sensor_cnfg |= CAMERA_CTRL_VSYNC_POL_NOT_INVERT;

  else if(cam_sensor->Config->vsync_pol == CAMERA_SENSOR_POLARITY_INVERT)
    sensor_cnfg |= CAMERA_CTRL_VSYNC_POL_INVERT;

  else
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Get Camera Sensor HSYNC Mode. */
  if(cam_sensor->Config->hsync_mode == CAMERA_SENSOR_HSYNC_SIG_IS_HYNC)
    sensor_cnfg |= CAMERA_CTRL_HSYNC_SIG_IS_HYNC;

  else if(cam_sensor->Config->hsync_mode == CAMERA_SENSOR_HSYNC_SIG_IS_DATA_EN)
    sensor_cnfg |= CAMERA_CTRL_HSYNC_SIG_IS_DATA_EN;

  else
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Get Camera Sensor Data Mode. */
  if(cam_sensor->Config->data_mode == CAMERA_SENSOR_DATA_MODE_8_BIT)
    sensor_cnfg |= CAMERA_CTRL_DATA_MODE_8_BIT;

  else if(cam_sensor->Config->data_mode == CAMERA_SENSOR_DATA_MODE_16_BIT)
    sensor_cnfg |= CAMERA_CTRL_DATA_MODE_16_BIT;

  else if(cam_sensor->Config->data_mode == CAMERA_SENSOR_DATA_MODE_32_BIT)
    sensor_cnfg |= CAMERA_CTRL_DATA_MODE_32_BIT;

  else if(cam_sensor->Config->data_mode == CAMERA_SENSOR_DATA_MODE_64_BIT)
    sensor_cnfg |= CAMERA_CTRL_DATA_MODE_64_BIT;

  else
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Get Camera Sensor Data Mask.
   * @Note: No impact on 8-bit mode */
  if(cam_sensor->Config->data_mode == CAMERA_SENSOR_DATA_MODE_8_BIT)
    sensor_cnfg |= 0x00;

  else if(cam_sensor->Config->data_mask == CAMERA_SENSOR_DATA_MASK_10_BIT)
    sensor_cnfg |= CAMERA_CTRL_DATA_MASK_10_BIT;

  else if(cam_sensor->Config->data_mask == CAMERA_SENSOR_DATA_MASK_12_BIT)
    sensor_cnfg |= CAMERA_CTRL_DATA_MASK_12_BIT;

  else if(cam_sensor->Config->data_mask == CAMERA_SENSOR_DATA_MASK_14_BIT)
    sensor_cnfg |= CAMERA_CTRL_DATA_MASK_14_BIT;

  else if(cam_sensor->Config->data_mask == CAMERA_SENSOR_DATA_MASK_16_BIT)
    sensor_cnfg |= CAMERA_CTRL_DATA_MASK_16_BIT;

  else
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Update Camera Sensor configuration for Camera Controller */
  cam_ctrl->cnfg.sensor_cnfg = sensor_cnfg;

  return ARM_DRIVER_OK;
}

/**
  \fn           void cam_ctrl_set_sensor_config(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        set Camera Sensor configurations for Camera Controller
                 - Interface/Clock Source/
                 - Pixel Clock/HSYNC/VSYC Polarity/
                 - Data Mode/Data Mask/HSYNC Mode
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       none
*/
__STATIC_INLINE void cam_ctrl_set_sensor_config(CAMERA_CTRL_DEV *cam_ctrl)
{
  /* Set Camera Sensor configurations for Camera Controller. */
  cam_ctrl->reg_base->cam_cfg = cam_ctrl->cnfg.sensor_cnfg;
}

/**
  \fn           int32_t cam_ctrl_set_frame_config(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        set Camera Controller Frame Configurations:
                 - Frame Width(Column)
                 - Frame Height(Row)
                 - Enable counting number of data in a Row
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_set_frame_config(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val      = 0;
  uint16_t frame_width  = 0;
  uint16_t frame_height = 0;
  uint8_t  data_cnt_en  = 0;

  /* How Camera Controller Hardware is Capturing Frame?
   * Camera Controller Hardware logic calculations for Frame Width:
   * Currently, Camera Controller Hardware is aligned to 8 Bytes(64 Bits).
   * Lets take example for Frame Width 648:
   * Camera Controller Captures:
   *   648/8 = 81  Bytes
   *   81 + 1 = 82 Bytes //add 1 for alignment
   *  Total Capture data per Row:
   *   82*8 = 656 Bytes
   *  Correct data per Row:
   *   648 + 1 = 649 Bytes
   *  Junk data per Row = Total Capture data - Correct data = 656 - 649 = 7 Bytes.
   *
   * Lets arrange Frame Width for correctly getting data Frame
   * Frame Width = Frame Width - 1 = 648 - 1 = 647
   * Camera Controller Captures:
   *   647/8 = 80.87 Bytes
   *   80 + 1 = 81   Bytes //add 1
   *  Total Capture data per Row:
   *   81*8 = 648 Bytes
   *  Correct data per Row:
   *   647 + 1 = 648 Bytes
   *  Junk data per Row = Total Capture data - Correct data = 0 Bytes.
   */

  if(cam_ctrl->cnfg.frame.width)
    frame_width  = (cam_ctrl->cnfg.frame.width - 1);
  frame_height = cam_ctrl->cnfg.frame.height;
  data_cnt_en  = cam_ctrl->cnfg.frame.data_cnt_en;

  if( (frame_width  > CAMERA_CTRL_FRAME_CONFIG_WIDTH_MASK) || \
      (frame_height > CAMERA_CTRL_FRAME_CONFIG_HEIGHT_MASK) )
    return ARM_DRIVER_ERROR_PARAMETER;

  reg_val = CAMERA_CTRL_FRAME_CONFIG_WIDTH(frame_width)  | \
            CAMERA_CTRL_FRAME_CONFIG_HEIGHT(frame_height);

  if(data_cnt_en)
  {
    reg_val |= CAMERA_CTRL_FRAME_CONFIG_DATA_CNT_EN;
  }

  /* Set Frame Configuration Register. */
  cam_ctrl->reg_base->video_fcfg = reg_val;
  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_set_framebuff_start_addr_config(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        set Camera Controller FrameBuffer Start Address Configurations:
                 - four frame buffers are available
                 - Frame Buffer A/B/C/D
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_set_framebuff_start_addr_config(CAMERA_CTRL_DEV *cam_ctrl)
{
  /* Set Frame Buffer Start Address Register if its changed */
  if(cam_ctrl->reg_base->frame_addrA != cam_ctrl->cnfg.framebuff_saddr.fbuff_A)
    cam_ctrl->reg_base->frame_addrA = cam_ctrl->cnfg.framebuff_saddr.fbuff_A;

  if(cam_ctrl->reg_base->frame_addrB != cam_ctrl->cnfg.framebuff_saddr.fbuff_B)
    cam_ctrl->reg_base->frame_addrB = cam_ctrl->cnfg.framebuff_saddr.fbuff_B;

  if(cam_ctrl->reg_base->frame_addrC != cam_ctrl->cnfg.framebuff_saddr.fbuff_C)
    cam_ctrl->reg_base->frame_addrC = cam_ctrl->cnfg.framebuff_saddr.fbuff_C;

  if(cam_ctrl->reg_base->frame_addrD != cam_ctrl->cnfg.framebuff_saddr.fbuff_D)
    cam_ctrl->reg_base->frame_addrD = cam_ctrl->cnfg.framebuff_saddr.fbuff_D;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_set_horizontal_config(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        set Camera Controller Horizontal Configurations:
                 - Horizontal Back  Porch (HBP)
                 - Horizontal Front Porch (HFP)
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_set_horizontal_config(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val = 0;
  uint16_t hbp     = cam_ctrl->cnfg.hori.hbp;
  uint16_t hfp     = cam_ctrl->cnfg.hori.hfp;

  if( (hbp > CAMERA_CTRL_HORI_CONFIG_MASK) || \
      (hfp > CAMERA_CTRL_HORI_CONFIG_MASK) )
    return ARM_DRIVER_ERROR_PARAMETER;

  reg_val = CAMERA_CTRL_HORI_CONFIG_HBP(hbp) | \
            CAMERA_CTRL_HORI_CONFIG_HFP(hfp);

  if(hfp)
  {
    reg_val |= CAMERA_CTRL_HORI_CONFIG_HFP_EN;
  }

  /* Set Horizontal Configuration Register. */
  cam_ctrl->reg_base->video_hcfg = reg_val;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_set_vertical_config(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        set Camera Controller Vertical Configurations:
                 - Vertical Back  Porch (VBP)
                 - Vertical Front Porch (VFP)
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_set_vertical_config(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val = 0;
  uint16_t vbp     = cam_ctrl->cnfg.verti.vbp;
  uint16_t vfp     = cam_ctrl->cnfg.verti.vfp;

  if( (vbp > CAMERA_CTRL_VERTI_CONFIG_MASK) || \
      (vfp > CAMERA_CTRL_VERTI_CONFIG_MASK) )
    return ARM_DRIVER_ERROR_PARAMETER;

  reg_val = CAMERA_CTRL_VERTI_CONFIG_VBP(vbp) | \
            CAMERA_CTRL_VERTI_CONFIG_VFP(vfp);

  /* Set Vertical Configuration Register. */
  cam_ctrl->reg_base->video_vcfg = reg_val;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_set_fifo_config(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        set Camera Controller FIFO Configurations:
                 - FIFO Read  Water Mark
                 - FIFO Write Water Mark
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_set_fifo_config(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val = 0;
  uint8_t  r_wmark = cam_ctrl->cnfg.fifo.read_watermark;
  uint8_t  w_wmark = cam_ctrl->cnfg.fifo.write_watermark;

  if( (r_wmark > CAMERA_CTRL_FIFO_WATERMARK_MASK) || \
      (w_wmark > CAMERA_CTRL_FIFO_WATERMARK_MASK) )
    return ARM_DRIVER_ERROR_PARAMETER;

  reg_val = CAMERA_CTRL_FIFO_READ_WATERMARK(r_wmark) | \
            CAMERA_CTRL_FIFO_WRITE_WATERMARK(w_wmark);

  /* Set FIFO Configuration Register. */
  cam_ctrl->reg_base->fifo_ctrl = reg_val;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_set_color_config(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        set Camera Controller Color Configurations:
                 -  8-bit data swap mode
                 - 16-bit color encoding
                 - 32-bit color encoding
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_set_color_config(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val            = 0;
  uint8_t  data_swap_8bit_en  = cam_ctrl->cnfg.color.data_swap_8bit_en;
  uint8_t  color_encode_16bit = cam_ctrl->cnfg.color.encode_16bit;
  uint8_t  color_encode_32bit = cam_ctrl->cnfg.color.encode_32bit;

  if( (color_encode_16bit > CAMERA_CTRL_COLOR_CONFIG_16BIT_ENCODE_MASK) || \
      (color_encode_32bit > CAMERA_CTRL_COLOR_CONFIG_32BIT_ENCODE_MASK) )
    return ARM_DRIVER_ERROR_PARAMETER;

  reg_val = CAMERA_CTRL_COLOR_CONFIG_16BIT_ENCODE(color_encode_16bit)  | \
            CAMERA_CTRL_COLOR_CONFIG_32BIT_ENCODE(color_encode_32bit);

  if(data_swap_8bit_en)
  {
    reg_val |= CAMERA_CTRL_COLOR_CONFIG_8BIT_DATA_SWAP;
  }

  /* Set Color Mode Configuration Register. */
  cam_ctrl->reg_base->video_cmcfg = reg_val;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_set_all_config(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        set all Camera Controller Configurations to respective registers
                 - set Camera Sensor Configuration
                    - Interface/Clock Source/
                    - Pixel Clock/HSYNC/VSYC Polarity/
                    - Data Mode/Data Mask/HSYNC Mode
                 - set Camera Controller Configurations:
                    - Frame/Frame Buffer Start Address/
                    - Horizontal/Vertical/FIFO/Color mode
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_set_all_config(CAMERA_CTRL_DEV *cam_ctrl)
{
  int32_t ret = ARM_DRIVER_OK;

  /* Set Camera Sensor configurations for Camera Controller. */
  cam_ctrl_set_sensor_config(cam_ctrl);

  /* Camera Controller set frame configuration */
  ret = cam_ctrl_set_frame_config(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  /* Camera Controller set frame buffer address  */
  ret = cam_ctrl_set_framebuff_start_addr_config(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  /* Camera Controller set horizontal configuration */
  ret = cam_ctrl_set_horizontal_config(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  /* Camera Controller set vertical configuration */
  ret = cam_ctrl_set_vertical_config(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  /* Camera Controller set fifo watermark */
  ret = cam_ctrl_set_fifo_config(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  /* Camera Controller set color mode configuration */
  ret = cam_ctrl_set_color_config(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_init(CAMERA_CTRL_DEV       *cam_ctrl,
                                      CAMERA_SENSOR_DEVICE  *cam_sensor,
                                      ARM_CAMERA_RESOLUTION  cam_resolution)
  \brief        initialize Camera Controller with all the configurations.
                 this function will
                  - disable camera interrupt
                  - get and set Camera Sensor Configuration
                    - Interface/Clock Source/
                    - Pixel Clock/HSYNC/VSYC Polarity/
                    - Data Mode/Data Mask/HSYNC Mode
                  - set Camera Controller Configurations:
                    - Frame/Frame Buffer Start Address/
                    - Horizontal/Vertical/FIFO/Color mode
  \param[in]    cam_ctrl       : Pointer to Camera Controller resources structure
  \param[in]    cam_sensor     : Pointer to Camera Sensor Device resources structure
  \param[in]    cam_resolution : Camera Resolution \ref ARM_CAMERA_RESOLUTION
  \return       \ref execution_status
*/
static int32_t cam_ctrl_init(CAMERA_CTRL_DEV       *cam_ctrl,
                             CAMERA_SENSOR_DEVICE  *cam_sensor,
                             ARM_CAMERA_RESOLUTION  cam_resolution)
{
  uint16_t frame_width  = 0;
  uint16_t frame_height = 0;
  int32_t  ret          = ARM_DRIVER_OK;

  /* Get Camera Sensor specific Configuration. */
  ret = cam_ctrl_get_sensor_config(cam_ctrl,cam_sensor);
  if(ret != ARM_DRIVER_OK)
    return ret;

  switch(cam_resolution)
  {
  case CAMERA_RESOLUTION_VGA_640x480:
    frame_width  = 640;
    frame_height = 480;
    break;
  case CAMERA_RESOLUTION_560x560:
    frame_width  = 560;
    frame_height = 560;
    break;
  case CAMERA_RESOLUTION_480x480:
    frame_width  = 480;
    frame_height = 480;
    break;
  default:
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Check if Camera Sensor requires any additional width and height? */
  if(cam_sensor->Info->additional_width)
    frame_width += cam_sensor->Info->additional_width;

  if(cam_sensor->Info->additional_height)
    frame_height += cam_sensor->Info->additional_height;

  /* Camera Controller Frame Configuration. */
  cam_ctrl->cnfg.frame.width       = frame_width;
  cam_ctrl->cnfg.frame.height      = frame_height;
  cam_ctrl->cnfg.frame.data_cnt_en = ENABLE;

  /* Camera Controller Horizontal Configuration. */
  cam_ctrl->cnfg.hori.hbp = 0x00;
  cam_ctrl->cnfg.hori.hfp = 0x00;

  /* Camera Controller Vertical Configuration. */
  cam_ctrl->cnfg.verti.vbp = 0x00;
  cam_ctrl->cnfg.verti.vfp = 0x00;

  /* Camera Controller FIFO Water mark Configuration. */
  cam_ctrl->cnfg.fifo.read_watermark  = 0x0F;
  cam_ctrl->cnfg.fifo.write_watermark = 0x00;

  /* Camera Controller Color Mode Configuration. */
  cam_ctrl->cnfg.color.data_swap_8bit_en = DISABLE;
  cam_ctrl->cnfg.color.encode_16bit      = 0x00;
  cam_ctrl->cnfg.color.encode_32bit      = 0x00;

  /* Camera Controller Frame Buffer Start Address Configuration. */
  cam_ctrl->cnfg.framebuff_saddr.fbuff_A = 0x00;
  cam_ctrl->cnfg.framebuff_saddr.fbuff_B = 0x00;
  cam_ctrl->cnfg.framebuff_saddr.fbuff_C = 0x00;
  cam_ctrl->cnfg.framebuff_saddr.fbuff_D = 0x00;

  /* Disable Camera Controller Interrupt. */
  cam_ctrl_disable_interrupt(cam_ctrl);

  /* Set all Camera Controller Configurations. */
  ret = cam_ctrl_set_all_config(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_start(CAMERA_CTRL_DEV *cam_ctrl,
  \brief        start Camera Controller
                 this function will
                  - check Camera Controller capture status
                  - set frame buffer start address
                  - enable Camera Controller interrupt
                  - software reset Camera Controller
                  - start capture in Snapshot mode
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_start(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val = 0;
  int32_t  ret = ARM_DRIVER_OK;

  if (!((cam_ctrl->capture_mode == CAMERA_CTRL_CAPTURE_MODE_SNAPSHOT) || (cam_ctrl->capture_mode == CAMERA_CTRL_CAPTURE_MODE_VIDEO)))
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Check Camera Controller is busy in capturing? */
  if(cam_ctrl_get_capture_status(cam_ctrl) != CAMERA_CTRL_CAPTURE_STATUS_NOT_CAPTURING)
    return ARM_DRIVER_ERROR_BUSY;

  /* Set Frame Buffer Start Address. */
  ret = cam_ctrl_set_framebuff_start_addr_config(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  /* Clear Camera Controller status variable. */
  cam_ctrl->status.cam_ctrl_status = 0;

  /* Enable Camera Controller Interrupt. */
  cam_ctrl_enable_interrupt(cam_ctrl);

  /* Clear Camera Controller control register*/
  cam_ctrl->reg_base->cam_ctrl = 0;

  /* Camera Controller Software Reset */
  cam_ctrl_softreset(cam_ctrl);

  /* Start Camera Capture in Snapshot mode */
  if(cam_ctrl->capture_mode == CAMERA_CTRL_CAPTURE_MODE_SNAPSHOT)
  {
    reg_val = (CAMERA_CTRL_CAPTURE_MODE_SNAPSHOT_EN | CAMERA_CTRL_START_CAPTURE);
    cam_ctrl->reg_base->cam_ctrl |= reg_val;
  }
  else
  {
    /* Start Camera Capture in Video mode */
    reg_val = (CAMERA_CTRL_START_CAPTURE);
    cam_ctrl->reg_base->cam_ctrl |= reg_val;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t cam_ctrl_stop(CAMERA_CTRL_DEV *cam_ctrl,
  \brief        stop Camera Controller
                 this function will
                  - disable Camera Controller interrupt.
                  - stop Camera Controller capture.
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static int32_t cam_ctrl_stop(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val = 0;

  /* Disable Camera Controller Interrupt. */
  cam_ctrl_disable_interrupt(cam_ctrl);

  /* Stop Camera Controller Capture */
  reg_val = (~CAMERA_CTRL_START_CAPTURE);
  cam_ctrl->reg_base->cam_ctrl &= reg_val;

  return ARM_DRIVER_OK;
}

#if (RTE_MIPI_CSI2)
/**
  \fn          void ARM_MIPI_CSI2_Event_Callback (uint32_t int_event)
  \brief       Signal MIPI CSI2 Events.
  \param[in]   int_event  \ref MIPI CSI2 event types.
  \return      none.
*/
void ARM_MIPI_CSI2_Event_Callback (uint32_t int_event)
{
	camera0_ctrl.cb_event (ARM_CAMERA_CONTROLLER_EVENT_MIPI_CSI2_ERROR);
}
#endif

/**
  \fn           int32_t CAMERAx_Initialize(CAMERA_CTRL_DEV      *cam_ctrl,
                                           CAMERA_SENSOR_DEVICE *cam_sensor,
                                           ARM_CAMERA_RESOLUTION cam_resolution)
  \brief        initialize Camera Sensor and Camera Controller.
                 this function will
                  - set the user callback event
                  - call Camera Sensor initialize
                  - initialize Camera Controller with all the configurations
                    - Camera Sensor Configurations:
                      - Interface/Clock Source/
                      - Pixel Clock/HSYNC/VSYC Polarity/
                      - Data Mode/Data Mask/HSYNC Mode
                    - Camera Controller Configurations:
                      - Frame/Frame Buffer Start Address/
                      - Horizontal/Vertical/FIFO/Color mode
  \param[in]    cam_ctrl       : Pointer to Camera Controller resources structure
  \param[in]    cam_sensor     : Pointer to Camera Sensor Device resources structure
  \param[in]    cam_resolution : Camera Resolution \ref ARM_CAMERA_RESOLUTION
  \param[in]    cb_event       : Pointer to Camera Event \ref ARM_CAMERA_CONTROLLER_SignalEvent_t
  \return       \ref execution_status
*/
static int32_t CAMERAx_Initialize(CAMERA_CTRL_DEV                     *cam_ctrl,
                                  CAMERA_SENSOR_DEVICE                *cam_sensor,
                                  ARM_CAMERA_RESOLUTION                cam_resolution,
                                  ARM_CAMERA_CONTROLLER_SignalEvent_t  cb_event)
{
  int32_t ret = ARM_DRIVER_OK;

  if (cam_ctrl->status.flags & CAMERA_CTRL_FLAG_INIT)
  {
    /* Driver is already initialized */
    return ARM_DRIVER_OK;
  }

  if (!cb_event)
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Set the user callback event. */
  cam_ctrl->cb_event = cb_event;

  /* Call Camera Sensor specific init */
  ret = cam_sensor->Ops->Init(cam_resolution);
  if(ret != ARM_DRIVER_OK)
    return ret;

#if (RTE_MIPI_CSI2)
  /*Initializing MIPI CSI2 if the sensor is MIPI CSI2 sensor*/
  ret = Driver_MIPI_CSI2.Initialize(ARM_MIPI_CSI2_Event_Callback, RTE_ARX3A0_CAMERA_SENSOR_FREQ);
  if(ret != ARM_DRIVER_OK)
    return ret;
#endif

  /* Camera Controller init */
  ret = cam_ctrl_init(cam_ctrl,cam_sensor,cam_resolution);
  if(ret != ARM_DRIVER_OK)
    return ret;

  /* Set the driver flag as initialized. */
  cam_ctrl->status.flags = CAMERA_CTRL_FLAG_INIT;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t CAMERAx_Uninitialize(CAMERA_CTRL_DEV      *cam_ctrl,
                                             CAMERA_SENSOR_DEVICE *cam_sensor,
  \brief        un-initialize Camera Sensor and Camera Controller
  \param[in]    cam_ctrl   : Pointer to Camera Controller resources structure
  \param[in]    cam_sensor : Pointer to Camera Sensor Device resources structure
  \return       \ref execution_status
*/
static int32_t CAMERAx_Uninitialize(CAMERA_CTRL_DEV      *cam_ctrl,
                                    CAMERA_SENSOR_DEVICE *cam_sensor)
{
  int32_t ret = ARM_DRIVER_OK;

  /* Call Camera Sensor specific uninit */
  cam_sensor->Ops->Uninit();

#if (RTE_MIPI_CSI2)
  /*Uninitializing MIPI CSI2 if the sensor is MIPI CSI2 sensor*/
  ret = Driver_MIPI_CSI2.Uninitialize();
  if(ret != ARM_DRIVER_OK)
    return ret;
#endif
  /* Reset driver flags. */
  cam_ctrl->status.flags = 0U;

  return ret;
}

/**
  \fn           int32_t CAMERAx_PowerControl(CAMERA_CTRL_DEV *cam_ctrl,
                                             ARM_POWER_STATE  state)
  \brief        camera power control.
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \param[in]    state     : Power state
  \return       \ref execution_status
*/
static int32_t CAMERAx_PowerControl(CAMERA_CTRL_DEV *cam_ctrl,
                                    ARM_POWER_STATE  state)
{
  int32_t ret = ARM_DRIVER_OK;

  switch (state)
  {
    case ARM_POWER_OFF:

      /* Disable Camera IRQ */
      NVIC_DisableIRQ(cam_ctrl->irq_num);

      /* Clear Any Pending Camera IRQ */
      NVIC_ClearPendingIRQ(cam_ctrl->irq_num);

#if (RTE_MIPI_CSI2)
      /*Disable MIPI CSI2*/
      ret = Driver_MIPI_CSI2.PowerControl(ARM_POWER_OFF);
      if(ret != ARM_DRIVER_OK)
        return ret;
#endif

      /* Reset the power status of Camera. */
      cam_ctrl->status.flags &= ~CAMERA_CTRL_FLAG_POWER;
      break;

    case ARM_POWER_FULL:

      if (!(cam_ctrl->status.flags & CAMERA_CTRL_FLAG_INIT))
        return ARM_DRIVER_ERROR;

      if (cam_ctrl->status.flags & CAMERA_CTRL_FLAG_POWER)
        break;

      /* Enable Camera IRQ */
      NVIC_ClearPendingIRQ(cam_ctrl->irq_num);
      NVIC_SetPriority(cam_ctrl->irq_num, cam_ctrl->irq_priority);
      NVIC_EnableIRQ(cam_ctrl->irq_num);

#if (RTE_MIPI_CSI2)
      /*Enable MIPI CSI2*/
      ret = Driver_MIPI_CSI2.PowerControl(ARM_POWER_FULL);
      if(ret != ARM_DRIVER_OK)
        return ret;
#endif

      /* Set the power flag enabled */
      cam_ctrl->status.flags |= CAMERA_CTRL_FLAG_POWER;
      break;

    case ARM_POWER_LOW:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
   }

  return ret;
}

/**
  \fn           int32_t CAMERAx_CaptureFrame(CAMERA_CTRL_DEV      *cam_ctrl,
                                             CAMERA_SENSOR_DEVICE *cam_sensor,
                                             uint8_t              *framebuffer_startaddr)
  \brief        start Camera Sensor and Camera Controller (in Snapshot mode).
                 in Snapshot mode, Camera Controller will capture one frame then it gets stop.
                 this function will
                  - call Camera Sensor Start
                  - set frame buffer start address in Camera Controller
                  - set Camera Controller Capture mode as Snapshot mode
                  - start capturing
  \param[in]    cam_ctrl              : Pointer to Camera Controller resources structure
  \param[in]    cam_sensor            : Pointer to Camera Sensor Device resources structure
  \param[in]    framebuffer_startaddr : Pointer to frame buffer start address,
                                         where camera captured image will be stored.
  \return       \ref execution_status
*/
static int32_t CAMERAx_CaptureFrame(CAMERA_CTRL_DEV       *cam_ctrl,
                                    CAMERA_SENSOR_DEVICE  *cam_sensor,
                                    uint8_t               *framebuffer_startaddr)
{
  int32_t ret = ARM_DRIVER_OK;

#if (RTE_MIPI_CSI2)
  uint32_t lp_count = 0;
#endif

  if(!framebuffer_startaddr)
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Check Camera Controller is busy in capturing? */
  if(cam_ctrl_get_capture_status(cam_ctrl) != CAMERA_CTRL_CAPTURE_STATUS_NOT_CAPTURING)
    return ARM_DRIVER_ERROR_BUSY;

#if (RTE_MIPI_CSI2)
  ret = Driver_MIPI_CSI2.StartIPI();
  if(ret != ARM_DRIVER_OK)
    return ret;
#endif

  /* Call Camera Sensor specific Start */
  cam_sensor->Ops->Start();

  /* Update Frame Buffer Start Address */
  cam_ctrl->cnfg.framebuff_saddr.fbuff_A = (uint32_t)framebuffer_startaddr;

  /* Set Capture Mode as Snapshot, Capture one frame then stop. */
  cam_ctrl->capture_mode = CAMERA_CTRL_CAPTURE_MODE_SNAPSHOT;

  /* Camera Controller start capturing */
  ret = cam_ctrl_start(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

#if (RTE_MIPI_CSI2)
  /*Wait till frame completes capturing and check for any error occurred while capturing*/
  while(cam_ctrl_get_capture_status(cam_ctrl) == CAMERA_CTRL_CAPTURE_STATUS_CAPTURING)
  {
    if(lp_count++ < 1000000)
      PMU_delay_loop_us(1);
    else
      return ARM_DRIVER_ERROR;
  }
#endif

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t CAMERAx_CaptureVideo (CAMERA_CTRL_DEV      *cam_ctrl,
                                              CAMERA_SENSOR_DEVICE *cam_sensor,
                                              uint8_t              *framebuffer_startaddr)
  \brief        start Camera Sensor and Camera Controller (in Video mode).
                 in Video mode, Camera Controller will capture video data continuously.
                 this function will
                  - call Camera Sensor Start
                  - set frame buffer start address in Camera Controller
                  - set Camera Controller Capture mode as Video mode
                  - start capturing
  \param[in]    cam_ctrl              : Pointer to Camera Controller resources structure
  \param[in]    cam_sensor            : Pointer to Camera Sensor Device resources structure
  \param[in]    framebuffer_startaddr : Pointer to frame buffer start address,
                                        where camera captured video data will be stored.
  \return       \ref execution_status
*/
static int32_t CAMERAx_CaptureVideo(CAMERA_CTRL_DEV       *cam_ctrl,
                                    CAMERA_SENSOR_DEVICE  *cam_sensor,
                                    uint8_t               *framebuffer_startaddr)
{
  int32_t ret = ARM_DRIVER_OK;

  if(!framebuffer_startaddr)
    return ARM_DRIVER_ERROR_PARAMETER;

  /* Check Camera Controller is busy in capturing? */
  if(cam_ctrl_get_capture_status(cam_ctrl) != CAMERA_CTRL_CAPTURE_STATUS_NOT_CAPTURING)
    return ARM_DRIVER_ERROR_BUSY;

#if (RTE_MIPI_CSI2)
  ret = Driver_MIPI_CSI2.StartIPI();
  if(ret != ARM_DRIVER_OK)
    return ret;
#endif

  /* Call Camera Sensor specific Start */
  cam_sensor->Ops->Start();

  /* Update Frame Buffer Start Address */
  cam_ctrl->cnfg.framebuff_saddr.fbuff_A = (uint32_t)framebuffer_startaddr;

  /* Set Capture Mode as Video Mode, Capture video data continuously. */
  cam_ctrl->capture_mode = CAMERA_CTRL_CAPTURE_MODE_VIDEO;

  /* Camera Controller start capturing */
  ret = cam_ctrl_start(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  return ARM_DRIVER_OK;
}


/**
  \fn           int32_t CAMERAx_Stop(CAMERA_CTRL_DEV      *cam_ctrl,
                                     CAMERA_SENSOR_DEVICE *cam_sensor)
  \brief        stop Camera Sensor and Camera Controller.
  \param[in]    cam_ctrl              : Pointer to Camera Controller resources structure
  \param[in]    cam_sensor            : Pointer to Camera Sensor Device resources structure
  \return       \ref execution_status
*/
static int32_t CAMERAx_Stop(CAMERA_CTRL_DEV       *cam_ctrl,
                            CAMERA_SENSOR_DEVICE  *cam_sensor)
{
  int32_t ret = ARM_DRIVER_OK;

  /* Call Camera Sensor specific Stop */
  cam_sensor->Ops->Stop();

#if (RTE_MIPI_CSI2)
  /*Stop MIPI CSI2 IPI interface*/
  ret = Driver_MIPI_CSI2.StopIPI();
  if(ret != ARM_DRIVER_OK)
    return ret;
#endif

  /* Stop Camera Controller */
  ret = cam_ctrl_stop(cam_ctrl);
  if(ret != ARM_DRIVER_OK)
    return ret;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t CAMERAx_Control(CAMERA_CTRL_DEV       *cam_ctrl,
                                        CAMERA_SENSOR_DEVICE  *cam_sensor,
                                        uint32_t               control,
                                        uint32_t               arg)
  \brief        control Camera Controller and Camera Sensor.
  \param[in]    cam_ctrl    : Pointer to Camera Controller resources structure
  \param[in]    cam_sensor  : Pointer to Camera Sensor Device resources structure
  \param[in]    control     : Operation
  \param[in]    arg         : Argument of operation
  \return       \ref execution_status
*/
static int32_t CAMERAx_Control(CAMERA_CTRL_DEV       *cam_ctrl,
                               CAMERA_SENSOR_DEVICE  *cam_sensor,
                               uint32_t               control,
                               uint32_t               arg)
{
  int32_t ret = ARM_DRIVER_OK;
  uint32_t cam_sensor_control = 0;

  if (!(cam_ctrl->status.flags & CAMERA_CTRL_FLAG_INIT))
    return ARM_DRIVER_ERROR;

  switch(control)
  {
    case CAMERA_CONTROLLER_SOFTRESET:
      /* Camera Controller software reset */
      cam_ctrl_softreset(cam_ctrl);
      break;
    case CAMERA_SENSOR_CONFIGURE:
      /* Camera Sensor configure*/
      cam_sensor_control = 1;
      break;
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  /* Call Camera Sensor specific Control if required. */
  if(cam_sensor_control)
  {

#if (RTE_MIPI_CSI2)
    /*Configure MIPI CSI2 host and IPI interface*/
    ret = Driver_MIPI_CSI2.ConfigureHost(CSI2_EVENT_PHY_FATAL
                                         | CSI2_EVENT_PKT_FATAL
                                         | CSI2_EVENT_PHY
                                         | CSI2_EVENT_LINE
                                         | CSI2_EVENT_IPI_FATAL
                                         | CSI2_EVENT_BNDRY_FRAME_FATAL
                                         | CSI2_EVENT_SEQ_FRAME_FATAL
                                         | CSI2_EVENT_CRC_FRAME_FATAL
                                         | CSI2_EVENT_PLD_CRC_FATAL
                                         | CSI2_EVENT_DATA_ID
                                         | CSI2_EVENT_ECC_CORRECT);
    if(ret != ARM_DRIVER_OK)
      return ret;

    ret = Driver_MIPI_CSI2.ConfigureIPI();
    if(ret != ARM_DRIVER_OK)
      return ret;
#endif
    cam_sensor->Ops->Control(control, arg);
  }

  return ret;
}

/**
  \fn           int32_t CAMERAx_IRQHandler(CAMERA_CTRL_DEV *cam_ctrl)
  \brief        camera interrupt handler.
                 this function will
                  - check Camera Controller received interrupt status.
                  - update events based on interrupt status.
                  - call the user callback function if any event occurs.
                  - clear interrupt status.
  \param[in]    cam_ctrl  : Pointer to Camera Controller resources structure
  \return       \ref execution_status
*/
static void CAMERAx_IRQHandler(CAMERA_CTRL_DEV *cam_ctrl)
{
  uint32_t reg_val       = 0U;
  uint32_t event         = 0U;   /* callback event */
  uint32_t intrpt_status = 0U;

  intrpt_status = cam_ctrl->reg_base->intr_stat;

  /* received capture stop interrupt? */
  if(intrpt_status & CAMERA_CTRL_IRQ_CAPTURE_STOP)
  {
    reg_val |= CAMERA_CTRL_IRQ_CAPTURE_STOP;
    event |= ARM_CAMERA_CONTROLLER_EVENT_CAMERA_CAPTURE_STOPPED;
  }

  /* received vsync detected interrupt? */
  if(intrpt_status & CAMERA_CTRL_IRQ_VSYNC_DETECT)
  {
    reg_val |= CAMERA_CTRL_IRQ_VSYNC_DETECT;
    event |= ARM_CAMERA_CONTROLLER_EVENT_CAMERA_FRAME_VSYNC_DETECTED;
  }

  /* received fifo over-run interrupt? */
  if(intrpt_status & CAMERA_CTRL_IRQ_FIFO_OVERRUN)
  {
    reg_val |= CAMERA_CTRL_IRQ_FIFO_OVERRUN;
    event |= ARM_CAMERA_CONTROLLER_EVENT_ERR_CAMERA_FIFO_OVERRUN;
  }

  /* received fifo under-run interrupt? */
  if(intrpt_status & CAMERA_CTRL_IRQ_FIFO_UNDERRUN)
  {
    reg_val |= CAMERA_CTRL_IRQ_FIFO_UNDERRUN;
    event |= ARM_CAMERA_CONTROLLER_EVENT_ERR_CAMERA_FIFO_UNDERRUN;
  }

  /* received bus error interrupt? */
  if(intrpt_status & CAMERA_CTRL_IRQ_BUS_ERR)
  {
    reg_val |= CAMERA_CTRL_IRQ_BUS_ERR;
    event |= ARM_CAMERA_CONTROLLER_EVENT_ERR_HARDWARE;
  }

  /* received HBP error interrupt? */
  if(intrpt_status & CAMERA_CTRL_IRQ_HBP_ERR)
  {
    reg_val |= CAMERA_CTRL_IRQ_HBP_ERR;
  }

  /* received HFP error interrupt? */
  if(intrpt_status & CAMERA_CTRL_IRQ_HFP_ERR)
  {
    reg_val |= CAMERA_CTRL_IRQ_HFP_ERR;
  }

  /* call the user callback if any event occurs */
  if ((event != 0U) && (cam_ctrl->cb_event != NULL) )
  {
    cam_ctrl->cb_event(event);
  }

  /* Copy Camera Controller status. */
  cam_ctrl->status.cam_ctrl_status = reg_val;

  /* clear interrupt by writing one(W1C) */
  cam_ctrl->reg_base->intr_stat = reg_val;

  (void)cam_ctrl->reg_base->intr_stat;

}


/* CAMERA0 Driver Instance */
#if (RTE_CAMERA0)

/* Check MT9M114 Camera Sensor is enable? */
#if (RTE_MT9M114_CAMERA_SENSOR_ENABLE)
  extern CAMERA_SENSOR_DEVICE mt9m114_camera_sensor;
  CAMERA_SENSOR_DEVICE *camera0_sensor = (CAMERA_SENSOR_DEVICE *)&mt9m114_camera_sensor;
#elif (RTE_ARX3A0_CAMERA_SENSOR_ENABLE)
  extern CAMERA_SENSOR_DEVICE arx3a0_camera_sensor;
  CAMERA_SENSOR_DEVICE *camera0_sensor = (CAMERA_SENSOR_DEVICE *)&arx3a0_camera_sensor;
#endif


/* wrapper functions for CAMERA0 */
static int32_t CAMERA0_Initialize(ARM_CAMERA_RESOLUTION               cam_resolution,
                                  ARM_CAMERA_CONTROLLER_SignalEvent_t cb_event)
{
  return (CAMERAx_Initialize(&camera0_ctrl, camera0_sensor, cam_resolution, cb_event));
}

static int32_t CAMERA0_Uninitialize(void)
{
  return (CAMERAx_Uninitialize(&camera0_ctrl, camera0_sensor));
}

static int32_t CAMERA0_PowerControl(ARM_POWER_STATE state)
{
  return (CAMERAx_PowerControl(&camera0_ctrl, state));
}

static int32_t CAMERA0_CaptureFrame(uint8_t *framebuffer_startaddr)
{
  return (CAMERAx_CaptureFrame(&camera0_ctrl, camera0_sensor, framebuffer_startaddr));
}

static int32_t CAMERA0_CaptureVideo(uint8_t *framebuffer_startaddr)
{
  return (CAMERAx_CaptureVideo(&camera0_ctrl, camera0_sensor, framebuffer_startaddr));
}

static int32_t CAMERA0_Stop(void)
{
  return (CAMERAx_Stop(&camera0_ctrl, camera0_sensor));
}

static int32_t CAMERA0_Control(uint32_t control, uint32_t arg)
{
  return (CAMERAx_Control(&camera0_ctrl, camera0_sensor, control, arg));
}

void CAMERA0_IRQHandler(void)
{
  CAMERAx_IRQHandler(&camera0_ctrl);
}

extern ARM_DRIVER_CAMERA_CONTROLLER Driver_CAMERA0;
ARM_DRIVER_CAMERA_CONTROLLER Driver_CAMERA0 =
{
  CAMERA_GetVersion,
  CAMERA_GetCapabilities,
  CAMERA0_Initialize,
  CAMERA0_Uninitialize,
  CAMERA0_PowerControl,
  CAMERA0_CaptureFrame,
  CAMERA0_CaptureVideo,
  CAMERA0_Stop,
  CAMERA0_Control,
};
#endif /* End of RTE_CAMERA0 */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
