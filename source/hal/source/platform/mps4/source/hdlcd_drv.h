/*
 * SPDX-FileCopyrightText: Copyright 2023, 2025 Arm Limited and/or
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
 * \file hdlcd_drv.h
 *
 * \brief Driver for HDLCD
 */

#ifndef __HDLCD_DRV_H__
#define __HDLCD_DRV_H__

#include <stdbool.h>
#include <stdint.h>
#include "hdlcd_reg_map.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PRODUCT_ID 0x1CDC
#define BYTESPERPIXEL4 3
#define BYTESPERPIXEL3 2
#define BYTESPERPIXEL2 1
#define BYTESPERPIXEL1 0

enum hdlcd_error_t {
    HDLCD_ERR_NONE,         /*!< No error */
    HDLCD_ERR               /*!< Generic error */
};

enum hdlcd_color_t {
    HDLCD_COLOR_RED,
    HDLCD_COLOR_GREEN,
    HDLCD_COLOR_BLUE
};

/**
 *  \brief Supported HDLCD resolutions enum
 */
enum hdlcd_resolution_t {
    /* 4:3 */
    HDLCD_RES_VGA = 0,
    HDLCD_RES_SVGA,
    HDLCD_RES_XGA,
    HDLCD_RES_UXGA,
    /* Non-standard */
    HDLCD_RES_320x240,
    /* 5:4 */
    HDLCD_RES_SXGA,
    /* 16:9 */
    HDLCD_RES_HD720,
    HDLCD_RES_HD768,
    HDLCD_RES_HD1080,
    /* Non-standard */
    HDLCD_RES_640x360,
    HDLCD_RES_NUM
};

/**
 *  \brief HDLCD device configuration structure
 */
struct hdlcd_resolution_cfg_t {
    uint32_t h_data;
    uint32_t h_front_porch;
    uint32_t h_sync;
    uint32_t h_back_porch;
    uint32_t v_data;
    uint32_t v_front_porch;
    uint32_t v_sync;
    uint32_t v_back_porch;
};

/**
 *  \brief Supported HDLCD resolutions config
 */
extern const struct hdlcd_resolution_cfg_t HDLCD_RES[HDLCD_RES_NUM];

/**
 *  \brief HDLCD device configuration structure
 */
struct hdlcd_dev_cfg_t {
    /*!< HDLCD device base address */
    const uint32_t base;
    /*!< Polarities of the sync signals for the LCD panel */
    const uint32_t polarities;
    /*!< Aspects of how the LCD controller accesses the bus */
    const uint32_t bus_options;
};

/**
 * \brief HDLCD device data structure
 */
struct hdlcd_dev_data_t {
    bool is_initialized;
};

/**
 * \brief HDLCD buffer structure
 */
struct hdlcd_buffer_cfg_t {
    /*!< Address of the first pixel of the first line in the frame buffer */
    uint32_t base_address;
    /*!< The length of each frame buffer line in bytes */
    uint32_t line_length;
    /*!< The number of lines to read from the frame buffer */
    uint32_t line_count;
    /*!< The number of bytes between the start of one line in the frame buffer and the start of the next line */
    uint32_t line_pitch;
    /*!< Endianness and number of bytes per pixel */
    uint32_t pixel_format;
};

/**
 * \brief HDLCD color structure
 */
struct hdlcd_color_cfg_t {
    /*!< Defaulr color */
    uint32_t default_color;
    /*!< Number of bits to exract */
    uint32_t bit_size;
    /*!< Index of the lowest bit to extract */
    uint32_t offset;
};

/**
 * \brief HDLCD pixel structure
 */
struct hdlcd_pixel_cfg_t {
    struct hdlcd_color_cfg_t red;
    struct hdlcd_color_cfg_t green;
    struct hdlcd_color_cfg_t blue;
};

/**
 * \brief HDLCD device structure
 */
struct hdlcd_dev_t {
    /*!< HDLCD configuration structure */
    const struct hdlcd_dev_cfg_t* const cfg;
    /*!< HDLCD data structure */
    struct hdlcd_dev_data_t* const data;
};

/**
 * \brief Initializes HDLCD
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_init(struct hdlcd_dev_t* dev);

/**
 * \brief Uninitializes HDLCD
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_uninit(struct hdlcd_dev_t* dev);

/**
 * \brief Static configuration of HDLCD
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_static_config(struct hdlcd_dev_t* dev);

/**
* \brief Set resolution of HDLCD from pre-configured options
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] resolution HDLCD resolution to set \ref hdlcd_resolution_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_set_resolution(struct hdlcd_dev_t* dev, enum hdlcd_resolution_t resolution);

/**
* \brief Configure the resolution of HDLCD
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] resolution_cfg HDLCD resolution config to set \ref hdlcd_resolution_cfg_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_set_custom_resolution(struct hdlcd_dev_t* dev, const struct hdlcd_resolution_cfg_t* resolution_cfg);

/**
 * \brief Get configured resolution of HDLCD
 *
 * \param[in]  dev HDLCD device struct \ref hdlcd_dev_t
 * \param[out] resolution_cfg configured HDLCD resolution \ref hdlcd_resolution_cfg_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_get_custom_resolution(struct hdlcd_dev_t* dev, struct hdlcd_resolution_cfg_t* resolution_cfg);

/**
 * \brief Buffer configuration of HDLCD
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] buff HDLC buffer struct \ref hdlcd_buffer_cfg_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_buffer_config(struct hdlcd_dev_t* dev, struct hdlcd_buffer_cfg_t* buff);

/**
 * \brief Pixel condfiguration of HDLCD
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] pixel HDLCD pixel struct \ref hdlcd_pixel_cfg_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_pixel_config(struct hdlcd_dev_t* dev, const struct hdlcd_pixel_cfg_t* pixel);

/**
 * \brief Enable HDLCD
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_enable(struct hdlcd_dev_t* dev);

/**
 * \brief Disable HDLCD
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_disable(struct hdlcd_dev_t* dev);

/**
 * \brief Enable HDLCD IRQs
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] mask IRQ mask
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_enable_irq(struct hdlcd_dev_t* dev, uint32_t mask);

/**
 * \brief Disable HDLCD IRQs
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] mask IRQ mask
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_disable_irq(struct hdlcd_dev_t* dev, uint32_t mask);

/**
 * \brief Generate HDLCD IRQs
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] mask IRQ mask
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_generate_irq(struct hdlcd_dev_t* dev, uint32_t mask);

/**
 * \brief Clear HDLCD IRQs
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] mask IRQ mask
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_clear_irq(struct hdlcd_dev_t* dev, uint32_t mask);

/**
 * \brief Get HDLCD IRQ state
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns INT_STATE register content
 */
uint32_t hdlcd_get_irq_state(struct hdlcd_dev_t* dev);

/**
 * \brief Set user output register
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] value Desired user output value
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_set_user_output(struct hdlcd_dev_t* dev, uint32_t value);

/**
 * \brief Get user output register content
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns user output register content
 */
uint32_t hdlcd_get_user_output(struct hdlcd_dev_t* dev);

/**
 * \brief Set frame buffer base address
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] value Frame buffer base address
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_set_frame_buffer_base_address(struct hdlcd_dev_t* dev, uint32_t value);

/**
 * \brief Get frame bufer base address value
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns frame bufer base address register content
 */
uint32_t hdlcd_get_frame_buffer_base_address(struct hdlcd_dev_t* dev);

/**
 * \brief Set frame buffer line length
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] value Frame buffer line length
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_set_frame_buffer_line_length(struct hdlcd_dev_t* dev, uint32_t value);

/**
 * \brief Get frame bufer line length value
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns frame bufer line length register content
 */
uint32_t hdlcd_get_frame_buffer_line_length(struct hdlcd_dev_t* dev);

/**
 * \brief Set frame buffer line count
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] value Frame buffer line count
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_set_frame_buffer_line_count(struct hdlcd_dev_t* dev, uint32_t value);

/**
 * \brief Get frame bufer line count value
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns frame bufer line count register content
 */
uint32_t hdlcd_get_frame_buffer_line_count(struct hdlcd_dev_t* dev);

/**
 * \brief Set frame buffer line pitch
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] value Frame buffer line pitch
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t hdlcd_set_frame_buffer_line_pitch(struct hdlcd_dev_t* dev, uint32_t value);

/**
 * \brief Get frame bufer line pitch value
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 *
 * \return Returns frame bufer line pitch register content
 */
uint32_t hdlcd_get_frame_buffer_line_pitch(struct hdlcd_dev_t* dev);

/**
 * \brief Set color select register
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] color Selected color \ref hdlcd_color_t
 * \param[in] color_cfg Color configuration \ref hdlcd_color_cfg_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t set_select_register(struct hdlcd_dev_t* dev,
     enum hdlcd_color_t color, struct hdlcd_color_cfg_t* color_cfg);

/**
 * \brief get color select register content
 *
 * \param[in] dev HDLCD device struct \ref hdlcd_dev_t
 * \param[in] color Selected color \ref hdlcd_color_t
 * \param[out] color_cfg Color configuration \ref hdlcd_color_cfg_t
 *
 * \return Returns error code as specified in \ref hdlcd_error_t
 */
enum hdlcd_error_t get_select_register(struct hdlcd_dev_t* dev,
     enum hdlcd_color_t color, struct hdlcd_color_cfg_t* color_cfg);

#ifdef __cplusplus
}
#endif
#endif /* __HDLCD_DRV_H__ */
