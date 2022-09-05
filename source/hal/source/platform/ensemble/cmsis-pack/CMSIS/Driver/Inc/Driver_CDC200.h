/* Copyright (c) 2021 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/**************************************************************************//**
 * @file     Driver_CDC200.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     30-Sep-2021
 * @brief    Display controller driver header.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef __DISPLAY_CONTROLLER_DRIVER_H__
#define __DISPLAY_CONTROLLER_DRIVER_H__

#include <stdint.h>

#define EXPSLV1_CDC200_PIXCLK_CTRL                  (0x04)
#define EXPSLV1_PIXEL_CLOCK_ENABLE                  (0x01)
#define EXPSLV1_PIXEL_CLOCK_DIVISOR                 (0x0E << 16)
#define CFGMST0_PIXEL_CLOCK_ENABLE                  (0x01 << 1)
#define CFGMST0_IP_CLOCK_ENABLE                     (0x01 << 4)

#define DRIVER_OK                                   (0)
#define DRIVER_ERROR                                (-1)

#define BUS_WIDTH                                   (7)
#define ALPHA_CONSTANT                              (0x000000FF)

typedef struct {
	volatile uint32_t hw_version;
	volatile uint32_t layer_count;
	volatile uint32_t sync_size;
	volatile uint32_t back_porch;
	volatile uint32_t active_width;
	volatile uint32_t total_width;
	volatile uint32_t global_control;
	volatile uint32_t global_configuration_1;
	volatile uint32_t global_configuration_2;
	volatile uint32_t shadow_reload_control;
	volatile uint32_t gamma_correction;
	volatile uint32_t background_color;
	volatile const uint32_t reserved0;
	volatile uint32_t irq_enable;
	volatile uint32_t irq_status;
	volatile uint32_t irq_clear;
	volatile uint32_t line_irq_position_control;
	volatile uint32_t position_status;
	volatile uint32_t sync_status;
	volatile uint32_t background_layer_base;
	volatile uint32_t background_layer_increments;
	volatile uint32_t background_layer_ram_address;
	volatile uint32_t background_layer_data;
	volatile uint32_t slave_timing_mode_status;
	volatile uint32_t external_display_control;
	volatile uint32_t secondary_irq_enable;
	volatile uint32_t secondary_irq_status;
	volatile uint32_t secondary_irq_clear;
	volatile uint32_t secure_line_irq_position_control;
	volatile const uint32_t reserved1;
	volatile uint32_t crc_reference;
	volatile uint32_t crc_result;
	volatile uint32_t rotation_buffer_0_address;
	volatile uint32_t rotation_buffer_1_address;
	volatile uint32_t rotation_buffer_pitch;
	volatile uint32_t rotation_blend_color;
	volatile uint32_t underrun_threshold;
} CDC200_GlobalRegInfo;

typedef struct {
	volatile uint32_t config_1;
	volatile uint32_t config_2;
	volatile uint32_t reload;
	volatile uint32_t control;
	volatile uint32_t window_h;
	volatile uint32_t window_v;
	volatile uint32_t color_key;
	volatile uint32_t pixel_format;
	volatile uint32_t alpha;
	volatile uint32_t color;
	volatile uint32_t blending;
	volatile uint32_t fb_bus_control;
	volatile uint32_t aux_fb_control;
	volatile uint32_t fb_start;
	volatile uint32_t fb_length;
	volatile uint32_t fb_lines;
	volatile uint32_t aux0_fb_start;
	volatile uint32_t aux1_fb_start;
	volatile uint32_t aux_fb_length;
	volatile uint32_t aux_fb_lines;
	volatile uint32_t clut;
	volatile uint32_t scaler_input_size;
	volatile uint32_t scaler_output_size;
	volatile uint32_t scaler_v_scaling_factor;
	volatile uint32_t scaler_v_scaling_phase;
	volatile uint32_t scaler_h_scaling_factor;
	volatile uint32_t scaler_h_scaling_phase;
	volatile uint32_t ycbcr_scale_1;
	volatile uint32_t ycbcr_scale_2;
} CDC200_LayerRegInfo;

typedef struct {
	CDC200_GlobalRegInfo global_reg;
    uint32_t const reserved0[27];
	CDC200_LayerRegInfo layer1_reg;
    uint32_t const reserved1[35];
	CDC200_LayerRegInfo layer2_reg;
    uint32_t const reserved2[35];
	CDC200_LayerRegInfo layer3_reg;
} CDC200_RegInfo;

typedef struct {
	uint16_t height;
	uint16_t width;
	uint16_t hsync;
	uint16_t vsync;
	uint16_t h_back_porch;
	uint16_t v_back_porch;
	uint16_t h_front_porch;
	uint16_t v_front_porch;
} display_panel;

typedef enum {
	ARGB8888 = 0,
	RGB888,
	RGB565
} image_format;

void display_controller_enable(uint8_t enable);
int32_t display_controller_setup (uint32_t image_buff_address, uint32_t image_format, const display_panel *panel);

#endif /* __DISPLAY_CONTROLLER_DRIVER_H__ */
