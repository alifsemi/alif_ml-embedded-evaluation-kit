/*
 * SPDX-FileCopyrightText: Copyright 2023, 2024-2026 Arm Limited and/or
 * its affiliates <open-source-office@arm.com>
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
/**
 * \file hdlcd_drv.c
 *
 * \brief Driver for HDLCD
 */

#include "hdlcd_drv.h"
#include "hdlcd_reg_map.h"

const struct hdlcd_resolution_cfg_t HDLCD_RES[HDLCD_RES_NUM] = {
    [HDLCD_RES_VGA] = {.h_data = 640, .h_front_porch = 16, .h_sync = 64, .h_back_porch = 80,
                       .v_data = 480, .v_front_porch = 3, .v_sync = 4, .v_back_porch = 13},
    [HDLCD_RES_SVGA] = {.h_data = 800, .h_front_porch = 32, .h_sync = 80, .h_back_porch = 112,
                        .v_data = 600, .v_front_porch = 3, .v_sync = 4, .v_back_porch = 17},
    [HDLCD_RES_XGA] = {.h_data = 1024, .h_front_porch = 48, .h_sync = 104, .h_back_porch = 152,
                       .v_data = 768, .v_front_porch = 3, .v_sync = 4, .v_back_porch = 23},
    [HDLCD_RES_UXGA] = {.h_data = 1600, .h_front_porch = 112, .h_sync = 168, .h_back_porch = 280,
                        .v_data = 1200, .v_front_porch = 3, .v_sync = 4, .v_back_porch = 38},
    [HDLCD_RES_320x240] = {.h_data = 320, .h_front_porch = 8, .h_sync = 32, .h_back_porch = 40,
                           .v_data = 240, .v_front_porch = 3, .v_sync = 4, .v_back_porch = 6},
    [HDLCD_RES_SXGA] = {.h_data = 1280, .h_front_porch = 80, .h_sync = 136, .h_back_porch = 216,
                        .v_data = 1024, .v_front_porch = 3, .v_sync = 7, .v_back_porch = 29},
    [HDLCD_RES_HD720] = {.h_data = 1280, .h_front_porch = 64, .h_sync = 128, .h_back_porch = 192,
                         .v_data = 720, .v_front_porch = 3, .v_sync = 5, .v_back_porch = 20},
    [HDLCD_RES_HD768] = {.h_data = 1366, .h_front_porch = 72, .h_sync = 136, .h_back_porch = 208,
                         .v_data = 768, .v_front_porch = 3, .v_sync = 10, .v_back_porch = 17},
    [HDLCD_RES_HD1080] = {.h_data = 1920, .h_front_porch = 128, .h_sync = 200, .h_back_porch = 328,
                          .v_data = 1080, .v_front_porch = 3, .v_sync = 5, .v_back_porch = 32},
    [HDLCD_RES_640x360] = {.h_data = 640, .h_front_porch = 16, .h_sync = 64, .h_back_porch = 80,
                           .v_data = 360, .v_front_porch = 3, .v_sync = 5, .v_back_porch = 8}
};

enum hdlcd_error_t hdlcd_init(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if(!dev) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    if((p_hdlcd->version & VERSION_PRODUCT_ID_Msk) !=
       (PRODUCT_ID << VERSION_PRODUCT_ID_Pos)) {
        return HDLCD_ERR;
    }

    if (dev->data->is_initialized == false) {
        dev->data->is_initialized = true;
    }
    p_hdlcd->int_mask &= ~(INT_UNDERRUN_Msk | INT_VSYNC_Msk | INT_BUS_ERROR_Msk | INT_DMA_END_Msk);
    p_hdlcd->command &= ~COMMAND_ENABLE_Msk;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_uninit(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    if (dev->data->is_initialized == true) {
        dev->data->is_initialized = false;
    }

    p_hdlcd->int_mask &= ~(INT_UNDERRUN_Msk | INT_VSYNC_Msk | INT_BUS_ERROR_Msk | INT_DMA_END_Msk);
    p_hdlcd->command &= ~COMMAND_ENABLE_Msk;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_static_config(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd;
    const struct hdlcd_dev_cfg_t* p_cfg;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;
    p_cfg = dev->cfg;

    p_hdlcd->polarities = p_cfg->polarities;
    p_hdlcd->bus_options = p_cfg->bus_options;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_set_resolution(struct hdlcd_dev_t* dev, enum hdlcd_resolution_t resolution)
{
    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }
    if (resolution >= HDLCD_RES_NUM) {
        return HDLCD_ERR;
    }
    return hdlcd_set_custom_resolution(dev, &HDLCD_RES[resolution]);
}

enum hdlcd_error_t hdlcd_set_custom_resolution(struct hdlcd_dev_t* dev, const struct hdlcd_resolution_cfg_t* resolution_cfg)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->hsync_data_width = resolution_cfg->h_data - 1;
    p_hdlcd->hsync_front_porch_width = resolution_cfg->h_front_porch - 1;
    p_hdlcd->hsync_width = resolution_cfg->h_sync - 1;
    p_hdlcd->hsync_back_porch_width = resolution_cfg->h_back_porch - 1;

    /* FIXME: Current FVP requires VSYNC_DATA to be 3 pixel greater */
    p_hdlcd->vsync_data_width = resolution_cfg->v_data - 1;
    p_hdlcd->vsync_front_porch_width = resolution_cfg->v_front_porch - 1;
    p_hdlcd->vsync_width = resolution_cfg->v_sync - 1;
    p_hdlcd->vsync_back_porch_width = resolution_cfg->v_back_porch - 1;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_get_custom_resolution(struct hdlcd_dev_t* dev, struct hdlcd_resolution_cfg_t* resolution_cfg)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    resolution_cfg->h_data = p_hdlcd->hsync_data_width + 1;
    resolution_cfg->h_front_porch = p_hdlcd->hsync_front_porch_width + 1;
    resolution_cfg->h_sync = p_hdlcd->hsync_width + 1;
    resolution_cfg->h_back_porch = p_hdlcd->hsync_back_porch_width + 1;

    /* FIXME: Current FVP requires VSYNC_DATA to be 3 pixel greater */
    resolution_cfg->v_data = p_hdlcd->vsync_data_width + 1;
    resolution_cfg->v_front_porch = p_hdlcd->vsync_front_porch_width + 1;
    resolution_cfg->v_sync = p_hdlcd->vsync_width + 1;
    resolution_cfg->v_back_porch = p_hdlcd->vsync_back_porch_width + 1;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_buffer_config(struct hdlcd_dev_t* dev, struct hdlcd_buffer_cfg_t* buff)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }
    if(!buff) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->fb_base_addr = buff->base_address;
    p_hdlcd->fb_line_length = buff->line_length;
    p_hdlcd->fb_line_count = buff->line_count;
    p_hdlcd->fb_line_pitch = buff->line_pitch;

    p_hdlcd->pixel_format = buff->pixel_format;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_pixel_config(struct hdlcd_dev_t* dev, const struct hdlcd_pixel_cfg_t* pixel)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }
    if(!pixel) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->red_select = pixel->red.default_color << COLOR_SELECT_DEFAULT_Pos |
                          pixel->red.bit_size << COLOR_SELECT_SIZE_Pos |
                          pixel->red.offset << COLOR_SELECT_OFFSET_Pos;

    p_hdlcd->green_select = pixel->green.default_color << COLOR_SELECT_DEFAULT_Pos |
                            pixel->green.bit_size << COLOR_SELECT_SIZE_Pos |
                            pixel->green.offset << COLOR_SELECT_OFFSET_Pos;

    p_hdlcd->blue_select = pixel->blue.default_color << COLOR_SELECT_DEFAULT_Pos |
                           pixel->blue.bit_size << COLOR_SELECT_SIZE_Pos |
                           pixel->blue.offset << COLOR_SELECT_OFFSET_Pos;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_enable(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->command |=  COMMAND_ENABLE_Msk;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_disable(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->command &= ~COMMAND_ENABLE_Msk;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_enable_irq(struct hdlcd_dev_t* dev, uint32_t mask)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->int_mask |= mask;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_disable_irq(struct hdlcd_dev_t* dev, uint32_t mask)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->int_mask &= ~mask;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_generate_irq(struct hdlcd_dev_t* dev, uint32_t mask)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->raw_int_stat = mask;

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t hdlcd_clear_irq(struct hdlcd_dev_t* dev, uint32_t mask)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->int_clr = mask;

    return HDLCD_ERR_NONE;
}

uint32_t hdlcd_get_irq_state(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    return p_hdlcd->int_stat;
}

enum hdlcd_error_t hdlcd_set_user_output(struct hdlcd_dev_t* dev, uint32_t value)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->user_output = value;

    return HDLCD_ERR_NONE;
}

uint32_t hdlcd_get_user_output(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    return p_hdlcd->user_output;
}

enum hdlcd_error_t hdlcd_set_frame_buffer_base_address(struct hdlcd_dev_t* dev, uint32_t value)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->fb_base_addr = value;

    return HDLCD_ERR_NONE;
}

 uint32_t hdlcd_get_frame_buffer_base_address(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    return p_hdlcd->fb_base_addr;
}

enum hdlcd_error_t hdlcd_set_frame_buffer_line_length(struct hdlcd_dev_t* dev, uint32_t value)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->fb_line_length = value;

    return HDLCD_ERR_NONE;
}

 uint32_t hdlcd_get_frame_buffer_line_length(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    return p_hdlcd->fb_line_length;
}

enum hdlcd_error_t hdlcd_set_frame_buffer_line_count(struct hdlcd_dev_t* dev, uint32_t value)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->fb_line_count = value;

    return HDLCD_ERR_NONE;
}

 uint32_t hdlcd_get_frame_buffer_line_count(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    return p_hdlcd->fb_line_count;
}

enum hdlcd_error_t hdlcd_set_frame_buffer_line_pitch(struct hdlcd_dev_t* dev, uint32_t value)
{
    struct hdlcd_reg_map_t* p_hdlcd;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    p_hdlcd->fb_line_pitch = value;

    return HDLCD_ERR_NONE;
}

 uint32_t hdlcd_get_frame_buffer_line_pitch(struct hdlcd_dev_t* dev)
{
    struct hdlcd_reg_map_t* p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    return p_hdlcd->fb_line_pitch;
}

enum hdlcd_error_t set_select_register(struct hdlcd_dev_t* dev,
     enum hdlcd_color_t color, struct hdlcd_color_cfg_t* color_cfg)
{

    struct hdlcd_reg_map_t* p_hdlcd;
    uint32_t reg_value = 0;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    reg_value = ((color_cfg->default_color << COLOR_SELECT_DEFAULT_Pos) & COLOR_SELECT_DEFAULT_Msk);
    reg_value |= ((color_cfg->bit_size << COLOR_SELECT_SIZE_Pos) & COLOR_SELECT_SIZE_Msk);
    reg_value |= ((color_cfg->offset << COLOR_SELECT_OFFSET_Pos) & COLOR_SELECT_OFFSET_Msk);

    switch(color) {
        case HDLCD_COLOR_RED:
            p_hdlcd->red_select = reg_value;
            break;
        case HDLCD_COLOR_GREEN:
            p_hdlcd->green_select = reg_value;
            break;
        case HDLCD_COLOR_BLUE:
            p_hdlcd->blue_select = reg_value;
            break;
        default:
            return HDLCD_ERR;
    }

    return HDLCD_ERR_NONE;
}

enum hdlcd_error_t get_select_register(struct hdlcd_dev_t* dev,
     enum hdlcd_color_t color, struct hdlcd_color_cfg_t* color_cfg)
{
    struct hdlcd_reg_map_t* p_hdlcd;
    uint32_t reg_value = 0;

    if (dev->data->is_initialized == false) {
        return HDLCD_ERR;
    }

    p_hdlcd = (struct hdlcd_reg_map_t*)dev->cfg->base;

    switch(color) {
        case HDLCD_COLOR_RED:
            reg_value = p_hdlcd->red_select;
            break;
        case HDLCD_COLOR_GREEN:
            reg_value = p_hdlcd->green_select;
            break;
        case HDLCD_COLOR_BLUE:
            reg_value = p_hdlcd->blue_select;
            break;
        default:
            return HDLCD_ERR;
    }

    color_cfg->default_color = ((reg_value >> COLOR_SELECT_DEFAULT_Pos) & COLOR_SELECT_DEFAULT_Msk);
    color_cfg->bit_size = ((reg_value >> COLOR_SELECT_SIZE_Pos) & COLOR_SELECT_SIZE_Msk);
    color_cfg->offset = ((reg_value >> COLOR_SELECT_OFFSET_Pos) & COLOR_SELECT_OFFSET_Msk);

    return HDLCD_ERR_NONE;
}
