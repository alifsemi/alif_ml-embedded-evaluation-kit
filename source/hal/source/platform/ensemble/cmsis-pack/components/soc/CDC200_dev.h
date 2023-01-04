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
 * @file     CDC200_dev.h
 * @author   Girish BN and Prasanna Ravi
 * @email    girish.bn@alifsemi.com and prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     04-July-2022
 * @brief    Device Specific Header file for CDC200 Driver.
 ******************************************************************************/

#ifndef CDC200_DEV_H_
#define CDC200_DEV_H_

#include "RTE_Components.h"
#include CMSIS_device_header
#include "Driver_CDC200.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
  \brief Register description.
  */
typedef struct {
	__IOM uint32_t hw_version;
	__IOM uint32_t layer_count;
	__IOM uint32_t sync_size;
	__IOM uint32_t back_porch;
	__IOM uint32_t active_width;
	__IOM uint32_t total_width;
	__IOM uint32_t global_control;
	__IOM uint32_t global_configuration_1;
	__IOM uint32_t global_configuration_2;
	__IOM uint32_t shadow_reload_control;
	__IOM uint32_t gamma_correction;
	__IOM uint32_t background_color;
	__IM  uint32_t reserved0;
	__IOM uint32_t irq_enable;
	__IOM uint32_t irq_status;
	__IOM uint32_t irq_clear;
	__IOM uint32_t line_irq_position_control;
	__IOM uint32_t position_status;
	__IOM uint32_t sync_status;
	__IOM uint32_t background_layer_base;
	__IOM uint32_t background_layer_increments;
	__IOM uint32_t background_layer_ram_address;
	__IOM uint32_t background_layer_data;
	__IOM uint32_t slave_timing_mode_status;
	__IOM uint32_t external_display_control;
	__IOM uint32_t secondary_irq_enable;
	__IOM uint32_t secondary_irq_status;
	__IOM uint32_t secondary_irq_clear;
	__IOM uint32_t secure_line_irq_position_control;
	__IM  uint32_t reserved1;
	__IOM uint32_t crc_reference;
	__IOM uint32_t crc_result;
	__IOM uint32_t rotation_buffer_0_address;
	__IOM uint32_t rotation_buffer_1_address;
	__IOM uint32_t rotation_buffer_pitch;
	__IOM uint32_t rotation_blend_color;
	__IOM uint32_t underrun_threshold;
} CDC200_GlobalRegInfo;

typedef struct {
	__IOM uint32_t config_1;
	__IOM uint32_t config_2;
	__IOM uint32_t reload;
	__IOM uint32_t control;
	__IOM uint32_t window_h;
	__IOM uint32_t window_v;
	__IOM uint32_t color_key;
	__IOM uint32_t pixel_format;
	__IOM uint32_t alpha;
	__IOM uint32_t color;
	__IOM uint32_t blending;
	__IOM uint32_t fb_bus_control;
	__IOM uint32_t aux_fb_control;
	__IOM uint32_t fb_start;
	__IOM uint32_t fb_length;
	__IOM uint32_t fb_lines;
	__IOM uint32_t aux0_fb_start;
	__IOM uint32_t aux1_fb_start;
	__IOM uint32_t aux_fb_length;
	__IOM uint32_t aux_fb_lines;
	__IOM uint32_t clut;
	__IOM uint32_t scaler_input_size;
	__IOM uint32_t scaler_output_size;
	__IOM uint32_t scaler_v_scaling_factor;
	__IOM uint32_t scaler_v_scaling_phase;
	__IOM uint32_t scaler_h_scaling_factor;
	__IOM uint32_t scaler_h_scaling_phase;
	__IOM uint32_t ycbcr_scale_1;
	__IOM uint32_t ycbcr_scale_2;
} CDC200_LayerRegInfo;

typedef struct {
	CDC200_GlobalRegInfo global_reg;
    uint32_t const reserved0[27];
	CDC200_LayerRegInfo layer1_reg;
    uint32_t const reserved1[35];
	CDC200_LayerRegInfo layer2_reg;
    uint32_t const reserved2[35];
	CDC200_LayerRegInfo layer3_reg;
} CDC200_Type;

#define CDC200                         ((CDC200_Type *)CDC200_BASE)

/*CDC200_PIXCLK_CTRL register bits parameters*/
#define CDC200_CLK_EN_Pos              0U
#define CDC200_CLK_EN_Msk              (1U << CDC200_CLK_EN_Pos)
#define CDC200_CLK_DIV_Pos             16U
#define CDC200_CLK_DIV_Msk             MASK(24,16)

/*Global Enable bit parameters*/
#define CDC200_GLOBAL_EN_Pos           0U

/*CDC200 helper macros*/
#define CDC200_SET_BIT(REG, BIT)       ((REG) |= (BIT))
#define CDC200_CLEAR_BIT(REG, BIT)     ((REG) &= ~(BIT))

/*CDC configurations*/
#define BUS_WIDTH                      (7)
#define ALPHA_CONSTANT                 (0x000000FF)

/**
  \brief CDC200 Driver flags
  */
typedef enum {
	CDC200_FLAG_DRV_INIT_DONE     = (1U << 0),
	CDC200_FLAG_POWER_ENABLED     = (1U << 1),
	CDC200_FLAG_CONFIGURED        = (1U << 2),
	CDC200_FLAG_STARTED           = (1U << 3)
}CDC200_FLAG_Type;

/**
  \brief CDC200 Supported Pixel format.
  */
typedef enum {
	CDC200_ARGB8888 = 0,
	CDC200_RGB888,
	CDC200_RGB565,
	CDC200_RGBA8888,
	CDC200_AL44,
	CDC200_AL8,
	CDC200_ARGB1555,
	CDC200_ARGB4444
} CDC200_PIXEL_FORMAT;

/**
  \brief CDC200 DPI frame info
  */
typedef struct {
	uint32_t hsync_time;
	uint32_t hbp_time;
	uint32_t hfp_time;
	uint32_t hactive_time;
	uint32_t vsync_line;
	uint32_t vbp_line;
	uint32_t vfp_line;
	uint32_t vactive_line;
}CDC200_FRAME_INFO;

/**
  \brief CDC200 driver info
  */
typedef struct{
	CDC200_Type               *reg_base;
	ARM_CDC200_SignalEvent_t  cb_event;
	CDC200_FRAME_INFO         *frame_info;
	CDC200_PIXEL_FORMAT       pixel_format;
	CDC200_FLAG_Type          flags;
}CDC200_DRV_INFO;

#ifdef  __cplusplus
}
#endif

#endif /* CDC200_DEV_H_ */
