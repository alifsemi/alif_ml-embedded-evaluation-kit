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
 * @file     Camera_Controller_dev.h
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     10-Sep-2021
 * @brief    Device Specific Header file for Camera Controller Driver.
 ******************************************************************************/

#ifndef CAMERA_CONTROLLER_DEV_H_
#define CAMERA_CONTROLLER_DEV_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/* System Includes */
#include "RTE_Components.h"
#include CMSIS_device_header

/* Project Includes */
#include "Driver_Camera_Controller.h"

/* common helpers */
#define GENMASK(h,l)                                  (((~(0U)) - ((1U) << (l)) + 1) & \
                                                      (~(0U) >> (32 - 1 - (h))))

/* Camera Controller Register definitions */

/* Camera Controller Configurations from Camera Sensor */

/* Camera Controller Clock source */
#define CAMERA_CTRL_CLOCK_SOURCE_INTERNAL             (0x00)     /* Clock Source: Internal from Camera Controller    */
#define CAMERA_CTRL_CLOCK_SOURCE_EXTERNAL             (BIT(0))   /* Clock Source: External from Camera Sensor Device */

/* Camera Controller CAMERA interface */
#define CAMERA_CTRL_CAMERA_INTERFACE_PARALLEL         (0x00)     /* Camera Controller Interface: Parallel */
#define CAMERA_CTRL_CAMERA_INTERFACE_MIPI_CSI         (BIT(4))   /* Camera Controller Interface: MIPI_CSI */

/* Camera Controller HYSNC Mode */
#define CAMERA_CTRL_HSYNC_SIG_IS_HYNC                 (0x00)     /* Camera Controller HSYNC MODE: HSYNC signal is HYNC        */
#define CAMERA_CTRL_HSYNC_SIG_IS_DATA_EN              (BIT(8))   /* Camera Controller HSYNC MODE: HSYNC signal is Data Enable */

/* Camera Controller Pixel Clock Polarity */
#define CAMERA_CTRL_PIXEL_CLK_POL_NOT_INVERT          (0x00)     /* Camera Controller Pixel Clock Polarity: Not Invert */
#define CAMERA_CTRL_PIXEL_CLK_POL_INVERT              (BIT(12))  /* Camera Controller Pixel Clock Polarity: Invert     */

/* Camera Controller HSYNC Polarity */
#define CAMERA_CTRL_HSYNC_POL_NOT_INVERT              (0x00)     /* Camera Controller HSYNC Polarity: Not Invert */
#define CAMERA_CTRL_HSYNC_POL_INVERT                  (BIT(16))  /* Camera Controller HSYNC Polarity: Invert     */

/* Camera Controller VSYNC Polarity */
#define CAMERA_CTRL_VSYNC_POL_NOT_INVERT              (0x00)     /* Camera Controller VSYNC Polarity: Not Invert */
#define CAMERA_CTRL_VSYNC_POL_INVERT                  (BIT(20))  /* Camera Controller VSYNC Polarity: Invert     */

/* Camera Controller Data Mask
 * @Note: No impact on 8-bit mode */
#define CAMERA_CTRL_DATA_MASK_10_BIT                  (0x00)                /* Camera Controller Data Mask: 10-bit */
#define CAMERA_CTRL_DATA_MASK_12_BIT                  (BIT(24))             /* Camera Controller Data Mask: 12-bit */
#define CAMERA_CTRL_DATA_MASK_14_BIT                  (BIT(25))             /* Camera Controller Data Mask: 14-bit */
#define CAMERA_CTRL_DATA_MASK_16_BIT                  (BIT(24) | BIT(25))   /* Camera Controller Data Mask: 16-bit */

/* Camera Controller Data Mode */
#define CAMERA_CTRL_DATA_MODE_8_BIT                   (0x00)                /* Camera Controller Data Mode:  8-bit */
#define CAMERA_CTRL_DATA_MODE_16_BIT                  (BIT(28))             /* Camera Controller Data Mode: 16-bit */
#define CAMERA_CTRL_DATA_MODE_32_BIT                  (BIT(29))             /* Camera Controller Data Mode: 32-bit */
#define CAMERA_CTRL_DATA_MODE_64_BIT                  (BIT(28) | BIT(29))   /* Camera Controller Data Mode: 32-bit */
/* End of Camera Controller Configurations from Camera Sensor */

/* Camera Controller Frame Configure Register */
#define CAMERA_CTRL_FRAME_CONFIG_WIDTH_MASK           (GENMASK(13,0))
#define CAMERA_CTRL_FRAME_CONFIG_WIDTH(x)             (((x) << 0))
#define CAMERA_CTRL_FRAME_CONFIG_HEIGHT_MASK          (GENMASK(11,0))
#define CAMERA_CTRL_FRAME_CONFIG_HEIGHT(x)            (((x) << 16))
#define CAMERA_CTRL_FRAME_CONFIG_DATA_CNT_EN          (BIT(15))

/* Camera Controller Horizontal Configure Register */
#define CAMERA_CTRL_HORI_CONFIG_MASK                  (GENMASK(13,0))
#define CAMERA_CTRL_HORI_CONFIG_HBP(x)                (((x) << 0))
#define CAMERA_CTRL_HORI_CONFIG_HFP(x)                (((x) << 16))
#define CAMERA_CTRL_HORI_CONFIG_HFP_EN                (BIT(31))

/* Camera Controller Vertical Configure Register */
#define CAMERA_CTRL_VERTI_CONFIG_MASK                 (GENMASK(11,0))
#define CAMERA_CTRL_VERTI_CONFIG_VBP(x)               (((x) << 0))
#define CAMERA_CTRL_VERTI_CONFIG_VFP(x)               (((x) << 16))

/* Camera Controller FIFO Configure Register */
#define CAMERA_CTRL_FIFO_WATERMARK_MASK               (GENMASK(4,0))
#define CAMERA_CTRL_FIFO_READ_WATERMARK(x)            (((x) << 0))
#define CAMERA_CTRL_FIFO_WRITE_WATERMARK(x)           (((x) << 8))

/* Camera Controller Color Mode Configure Register */
#define CAMERA_CTRL_COLOR_CONFIG_8BIT_DATA_NOT_SWAP   (0x00)
#define CAMERA_CTRL_COLOR_CONFIG_8BIT_DATA_SWAP       (BIT(0))
#define CAMERA_CTRL_COLOR_CONFIG_16BIT_ENCODE_MASK    (GENMASK(2,0))
#define CAMERA_CTRL_COLOR_CONFIG_16BIT_ENCODE(x)      (((x) << 4))
#define CAMERA_CTRL_COLOR_CONFIG_32BIT_ENCODE_MASK    (GENMASK(1,0))
#define CAMERA_CTRL_COLOR_CONFIG_32BIT_ENCODE(x)      (((x) << 8))

/* Camera Controller Interrupt Enable/Status Register */
#define CAMERA_CTRL_IRQ_CAPTURE_STOP                  (BIT(0))
#define CAMERA_CTRL_IRQ_FIFO_OVERRUN                  (BIT(1))
#define CAMERA_CTRL_IRQ_FIFO_UNDERRUN                 (BIT(2))
#define CAMERA_CTRL_IRQ_BUS_ERR                       (BIT(3))
#define CAMERA_CTRL_IRQ_HBP_ERR                       (BIT(4))
#define CAMERA_CTRL_IRQ_HFP_ERR                       (BIT(5))
#define CAMERA_CTRL_IRQ_VSYNC_DETECT                  (BIT(8))

/* Camera Controller Start Capture */
#define CAMERA_CTRL_START_CAPTURE                     (BIT(0))

/* Camera Controller Capture Status */
#define CAMERA_CTRL_CAPTURE_STATUS_BUSY_MASK          (BIT(1))
#define CAMERA_CTRL_CAPTURE_STATUS_NOT_CAPTURING      (0x00)
#define CAMERA_CTRL_CAPTURE_STATUS_CAPTURING          (0x01)

/* Camera Controller Capture Mode */
#define CAMERA_CTRL_CAPTURE_MODE_SNAPSHOT_EN          (BIT(4))

/* Camera Controller Software Reset */
#define CAMERA_CTRL_SOFT_RESET                        (BIT(8))

/* Camera Controller driver flags */
#define CAMERA_CTRL_FLAG_INIT                         (1U << 0)   /* Camera Controller initialized    */
#define CAMERA_CTRL_FLAG_POWER                        (1U << 1)   /* Camera Controller powered        */

/**
\brief Camera Controller Capture Mode
        Snapshot Mode : Camera Controller will
                        capture one frame then it gets stop.
*/
typedef enum _CAMERA_CTRL_CAPTURE_MODE {
  CAMERA_CTRL_CAPTURE_MODE_SNAPSHOT = 0,              /* Camera Controller Capture Mode: Snapshot Mode */
  CAMERA_CTRL_CAPTURE_MODE_VIDEO,                     /* Camera Controller Capture Mode: Video Mode */
} CAMERA_CTRL_CAPTURE_MODE;

/*
 \brief Camera Controller Register Memory Map Structure
 */
typedef struct _CAMERA_CTRL_REG_MAP {
  __IOM uint32_t cam_ctrl;          /* Offset: 0x00 (R/W)   CAMERA Control Register              */
  __IOM uint32_t intr_stat;         /* Offset: 0x04 (R/W1C) CAMERA Interrupt Status Register     */
  __IOM uint32_t intr_ena;          /* Offset: 0x08 (R/W)   CAMERA Interrupt Enable Register     */
        uint32_t reserved0;         /* Offset: 0x0C         Reserved                             */
  __IOM uint32_t cam_cfg;           /* Offset: 0x10 (R/W)   CAMERA Configure Register            */
  __IOM uint32_t fifo_ctrl;         /* Offset: 0x14 (R/W )  FIFO Control Register                */
  __IM  uint32_t axi_err_stat;      /* Offset: 0x18 (R/ )   AXI Error Status Register            */
        uint32_t reserved1;         /* Offset: 0x1C         Reserved                             */
  __IOM uint32_t video_hcfg;        /* Offset: 0x20 (R/W)   Video Horizontal Configure Register  */
  __IOM uint32_t video_vcfg;        /* Offset: 0x24 (R/W)   Video Vertical Configure Register    */
  __IOM uint32_t video_fcfg;        /* Offset: 0x28 (R/W)   Video Frame Configure Register       */
  __IOM uint32_t video_cmcfg;       /* Offset: 0x2C (R/W)   Video Color Mode Configure Register  */
  __IOM uint32_t frame_addrA;       /* Offset: 0x30 (R/W)   Video Frame Buffer A Start Address   */
  __IOM uint32_t frame_addrB;       /* Offset: 0x34 (R/W)   Video Frame Buffer B Start Address   */
  __IOM uint32_t frame_addrC;       /* Offset: 0x38 (R/W)   Video Frame Buffer C Start Address   */
  __IOM uint32_t frame_addrD;       /* Offset: 0x3C (R/W)   Video Frame Buffer D Start Address   */
}CAMERA_CTRL_REG_MAP;

/**
\brief Camera Controller Horizontal Configuration
*/
typedef struct _CAMERA_CTRL_HORIZONTAL_CONFIG {
  uint16_t hbp;                                         /* Horizontal Back  Porch */
  uint16_t hfp;                                         /* Horizontal Front Porch */
} CAMERA_CTRL_HORIZONTAL_CONFIG;

/**
\brief Camera Controller Vertical Configuration
*/
typedef struct _CAMERA_CTRL_VERTICAL_CONFIG {
  uint16_t vbp;                                         /* Vertical Back  Porch */
  uint16_t vfp;                                         /* Vertical Front Porch */
} CAMERA_CTRL_VERTICAL_CONFIG;

/**
\brief Camera Controller Frame Configuration
*/
typedef struct _CAMERA_CTRL_FRAME_CONFIG {
  uint16_t width;                                       /* Frame Width(Column)                     */
  uint16_t height;                                      /* Frame Height(Row)                       */
  uint8_t  data_cnt_en;                                 /* Enable counting number of data in a Row */
} CAMERA_CTRL_FRAME_CONFIG;

/**
\brief Camera Controller Frame Buffer Configuration
*/
typedef struct _CAMERA_CTRL_FRAMEBUFF_START_ADDR_CONFIG {
  uint32_t fbuff_A;                                     /* Frame Buffer A Start Address */
  uint32_t fbuff_B;                                     /* Frame Buffer B Start Address */
  uint32_t fbuff_C;                                     /* Frame Buffer C Start Address */
  uint32_t fbuff_D;                                     /* Frame Buffer D Start Address */
} CAMERA_CTRL_FRAMEBUFF_START_ADDR_CONFIG;

/**
\brief Camera Controller FIFO Configuration
*/
typedef struct _CAMERA_CTRL_FIFO_CONFIG {
  uint8_t read_watermark;                               /* FIFO Read  Water mark 0,1: illegal */
  uint8_t write_watermark;                              /* FIFO Write Water mark 0,1: illegal */
} CAMERA_CTRL_FIFO_CONFIG;

/**
\brief Camera Controller Color Mode Configuration
*/
typedef struct _CAMERA_CTRL_COLOR_MODE_CONFIG {
  uint8_t data_swap_8bit_en;                            /* Camera 8-bit data swap mode; 0:not swap,1:swap */
  uint8_t encode_16bit;                                 /* 16-bit color encoding                          */
  uint8_t encode_32bit;                                 /* 32-bit color encoding                          */
} CAMERA_CTRL_COLOR_MODE_CONFIG;

/**
\brief Camera Controller Configurations
*/
typedef struct _CAMERA_CTRL_CONFIG {
  uint32_t                                 sensor_cnfg;       /* Camera Sensor Configuration              */
  CAMERA_CTRL_HORIZONTAL_CONFIG            hori;              /* Horizontal Configuration                 */
  CAMERA_CTRL_VERTICAL_CONFIG              verti;             /* Vertical Configuration                   */
  CAMERA_CTRL_FRAME_CONFIG                 frame;             /* Frame Configuration                      */
  CAMERA_CTRL_FRAMEBUFF_START_ADDR_CONFIG  framebuff_saddr;   /* Frame Buffer Start Address Configuration */
  CAMERA_CTRL_FIFO_CONFIG                  fifo;              /* FIFO Configuration                       */
  CAMERA_CTRL_COLOR_MODE_CONFIG            color;             /* Color Mode Configuration                 */
}CAMERA_CTRL_CONFIG;

/**
\brief Camera Controller Status
*/
typedef struct _CAMERA_CTRL_STATUS {
  uint8_t  busy;                                        /* Camera capture busy flag           */
  uint8_t  flags;                                       /* Camera Controller Driver Flags     */
  uint32_t cam_ctrl_status;                             /* Camera Controller Status           */
}CAMERA_CTRL_STATUS;

/**
\brief Camera Controller Device Resource Structure
*/
typedef struct _CAMERA_CTRL_DEV {
  ARM_CAMERA_CONTROLLER_SignalEvent_t   cb_event;       /* Camera Controller Application Event Callback                  */
  CAMERA_CTRL_REG_MAP                  *reg_base;       /* Camera Controller Register Base Address                       */
  IRQn_Type                             irq_num;        /* Camera Controller Interrupt Vector Number                     */
  uint32_t                              irq_priority;   /* Camera Controller Interrupt Priority                          */
  CAMERA_CTRL_CAPTURE_MODE              capture_mode;   /* Camera Controller Capture Mode @ref CAMERA_CTRL_CAPTURE_MODE  */
  CAMERA_CTRL_CONFIG                    cnfg;           /* Camera Controller Configurations                              */
  CAMERA_CTRL_STATUS                    status;         /* Camera Controller Status                                      */
}CAMERA_CTRL_DEV;


#ifdef  __cplusplus
}
#endif

#endif /* CAMERA_CONTROLLER_DEV_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
