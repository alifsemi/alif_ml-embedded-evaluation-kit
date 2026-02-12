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
 * \file hdlcd_reg_map.h
 *
 * \brief Register map for HDLCD
 */

#ifndef __HDLCD_REG_MAP_H__
#define __HDLCD_REG_MAP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief HDLCD Register map structure
 */
struct hdlcd_reg_map_t {
    volatile const uint32_t version;                 /*!< Offset: 0x000 (R/ ) Version Register */
    volatile const uint32_t reserved1[3];            /*!< Offset: 0x004-0x00C Reserved */
    volatile       uint32_t raw_int_stat;            /*!< Offset: 0x010 (R/W) Raw Interrupt Status Register */
    volatile       uint32_t int_clr;                 /*!< Offset: 0x014 ( /W) Interrupt Clear Register */
    volatile       uint32_t int_mask;                /*!< Offset: 0x018 (R/W) Interrupt Clear Register */
    volatile       uint32_t int_stat;                /*!< Offset: 0x01C (R/W) Interrupt Status Register */
    volatile       uint32_t user_output;             /*!< Offset: 0x020 (R/W) User Output Register */
    volatile const uint32_t reserved2[55];           /*!< Offset: 0x024-0x0FC Reserved */
    volatile       uint32_t fb_base_addr;            /*!< Offset: 0x100 (R/W) Frame Buffer Base Address Register */
    volatile       uint32_t fb_line_length;          /*!< Offset: 0x104 (R/W) Frame Buffer Line Length Register */
    volatile       uint32_t fb_line_count;           /*!< Offset: 0x108 (R/W) Frame Buffer Line Count Register */
    volatile       uint32_t fb_line_pitch;           /*!< Offset: 0x10C (R/W) Frame Buffer Line Pitch Register */
    volatile       uint32_t bus_options;             /*!< Offset: 0x110 (R/W) Bus Options Register */
    volatile const uint32_t reserved3[59];           /*!< Offset: 0x114-0x1FC Reserved */
    volatile       uint32_t vsync_width;             /*!< Offset: 0x200 (R/W) Vertical Synch Register */
    volatile       uint32_t vsync_back_porch_width;  /*!< Offset: 0x204 (R/W) Vertical Back Porch Width Register */
    volatile       uint32_t vsync_data_width;        /*!< Offset: 0x208 (R/W) Vertical Data Width Register */
    volatile       uint32_t vsync_front_porch_width; /*!< Offset: 0x20C (R/W) Vertical Front Porch Width Register */
    volatile       uint32_t hsync_width;             /*!< Offset: 0x210 (R/W) Horizontal Synch Register */
    volatile       uint32_t hsync_back_porch_width;  /*!< Offset: 0x214 (R/W) Horizontal Back Porch Width Register */
    volatile       uint32_t hsync_data_width;        /*!< Offset: 0x218 (R/W) Horizontal Data Width Register */
    volatile       uint32_t hsync_front_porch_width; /*!< Offset: 0x21C (R/W) Horizontal Front Porch Width Register */
    volatile       uint32_t polarities;              /*!< Offset: 0x220 (R/W) Polarities Register */
    volatile const uint32_t reserved4[3];            /*!< Offset: 0x224-0x22C Reserved */
    volatile       uint32_t command;                 /*!< Offset: 0x230 (R/W) Command Register */
    volatile const uint32_t reserved5[3];            /*!< Offset: 0x234-0x23C Reserved */
    volatile       uint32_t pixel_format;            /*!< Offset: 0x240 (R/W) Pixel Format Register */
    volatile       uint32_t red_select;              /*!< Offset: 0x244 (R/W) Red Select Register */
    volatile       uint32_t green_select;            /*!< Offset: 0x248 (R/W) Green Select Register */
    volatile       uint32_t blue_select;             /*!< Offset: 0x24C (R/W) Blue Select Register */
};

/* Version Register Definitions */
#define VERSION_PRODUCT_ID_Pos          16U                                            /*!< VERSION: PRODUCT ID Position */
#define VERSION_PRODUCT_ID_Msk          (0xFFFFUL << VERSION_PRODUCT_ID_Pos)           /*!< VERSION: PRODUCT ID Mask */

#define VERSION_VERSION_MAJOR_Pos       8U                                             /*!< VERSION: VERSION MAJOR Position */
#define VERSION_VERSION_MAJOR_Msk       (0xFfUL << VERSION_VERSION_MAJOR_Pos)          /*!< VERSION: VERSION MAJOR Mask */

#define VERSION_VERSION_MINOR_Pos       0U                                             /*!< VERSION: VERSION MINOR Position */
#define VERSION_VERSION_MINOR_Msk       (0xFFUL /*<< VERSION_VERSION_MINOR_Pos*/)      /*!< VERSION: VERSION MINOR Mask */

/* Interrupt Register Definitions */
#define INT_UNDERRUN_Pos       3U                                             /*!< INT: UNDERRUN Position */
#define INT_UNDERRUN_Msk       (0x1UL << INT_UNDERRUN_Pos)                    /*!< INT: UNDERRUN Mask */

#define INT_VSYNC_Pos          2U                                             /*!< INT: VSYNC Position */
#define INT_VSYNC_Msk          (0x1UL << INT_VSYNC_Pos)                       /*!< INT: VSYNC Mask */

#define INT_BUS_ERROR_Pos      1U                                             /*!< INT: BUS ERROR Position */
#define INT_BUS_ERROR_Msk      (0x1UL << INT_BUS_ERROR_Pos)                   /*!< INT: BUS ERROR Mask */

#define INT_DMA_END_Pos        0U                                             /*!< INT: DMA END Position */
#define INT_DMA_END_Msk        (0x1UL /*<< INT_DMA_END_Pos*/)                 /*!< INT: DMA END Mask */

/* Frame Buffer Base Address Register Definitions */
#define FB_BASE_ADDR_FB_BASE_ADDR_Pos    3U                                               /*!< FB BASE ADDR: FB BASE ADDR Position */
#define FB_BASE_ADDR_FB_BASE_ADDR_Msk    (0x1FFFFFFFUL << FB_BASE_ADDR_FB_BASE_ADDR_Pos)  /*!< FB BASE ADDR: FB BASE ADDR Mask */

/* Frame Buffer Line Length Register Definitions */
#define FB_LINE_LENGTH_FB_LINE_LENGTH_Pos  3U                                                  /*!< FB LINE LENGTH: FB LINE LENGTH Position */
#define FB_LINE_LENGTH_FB_LINE_LENGTH_Msk  (0x1FFFFFFFUL << FB_LINE_LENGTH_FB_LINE_LENGTH_Pos) /*!< FB LINE LENGTH: FB LINE LENGTH Mask */

/* Frame Buffer Line Count Register Definitions */
#define FB_LINE_PITCH_FB_LINE_PITCH_Pos    3U                                                 /*!< FB LINE PITCH: FB LINE PITCH Position */
#define FB_LINE_PITCH_FB_LINE_PITCH_Msk    (0x1FFFFFFFUL << FB_LINE_PITCH_FB_LINE_PITCH_Pos)  /*!< FB LINE PITCH: FB LINE PITCH Mask */

/* Frame Buffer Line Count Register Definitions */
#define FB_LINE_PITCH_FB_LINE_PITCH_Pos    3U                                                 /*!< FB LINE PITCH: FB LINE PITCH Position */
#define FB_LINE_PITCH_FB_LINE_PITCH_Msk    (0x1FFFFFFFUL << FB_LINE_PITCH_FB_LINE_PITCH_Pos)  /*!< FB LINE PITCH: FB LINE PITCH Mask */

/* Bus Options Register Definitions */
#define BUS_OPTIONS_MAX_OUTSTANDING_Pos    8U                                                 /*!< BUS OPTIONS: MAX OUTSTANDING Position */
#define BUS_OPTIONS_MAX_OUTSTANDING_Msk    (0xFUL << BUS_OPTIONS_MAX_OUTSTANDING_Pos)         /*!< BUS OPTIONS: MAX OUTSTANDING Mask */

#define BUS_OPTIONS_BURST_16_Pos           4U                                                 /*!< BUS OPTIONS: BURST 16 Position */
#define BUS_OPTIONS_BURST_16_Msk           (0x1UL << BUS_OPTIONS_BURST_16_Pos)                /*!< BUS OPTIONS: BURST 16 Mask */

#define BUS_OPTIONS_BURST_8_Pos            3U                                                 /*!< BUS OPTIONS: BURST 8 Position */
#define BUS_OPTIONS_BURST_8_Msk            (0x1UL << BUS_OPTIONS_BURST_8_Pos)                 /*!< BUS OPTIONS: BURST 8 Mask */

#define BUS_OPTIONS_BURST_4_Pos            2U                                                 /*!< BUS OPTIONS: BURST 4 Position */
#define BUS_OPTIONS_BURST_4_Msk            (0x1UL << BUS_OPTIONS_BURST_4_Pos)                 /*!< BUS OPTIONS: BURST 4 Mask */

#define BUS_OPTIONS_BURST_2_Pos            1U                                                 /*!< BUS OPTIONS: BURST 2 Position */
#define BUS_OPTIONS_BURST_2_Msk            (0x1UL << BUS_OPTIONS_BURST_2_Pos)                 /*!< BUS OPTIONS: BURST 2 Mask */

#define BUS_OPTIONS_BURST_1_Pos            0U                                                 /*!< BUS OPTIONS: BURST 1 Position */
#define BUS_OPTIONS_BURST_1_Msk            (0x1UL /*<< BUS_OPTIONS_BURST_1_Pos*/)             /*!< BUS OPTIONS: BURST 1 Mask */

/* Vertical Sync Width Register Definitions */
#define VSYNC_WIDTH_V_SYNC_Pos             0U                                                 /*!< VSYNC WIDTH: V SYNC Position */
#define VSYNC_WIDTH_V_SYNC_Msk             (0xFFFUL << /*VSYNC_WIDTH_V_SYNC_Pos*/)            /*!< VSYNC WIDTH: V SYNC Mask */

/* Vertical Back Porch Register Definitions */
#define VSYNC_BACK_PORCH_WIDTH_V_BACK_PORCH_Pos   0U                                                         /*!< VSYNC BACK PORCH WIDTH: V BACK PORCH Position */
#define VSYNC_BACK_PORCH_WIDTH_V_BACK_PORCH_Msk   (0xFFFUL << /*VSYNC_BACK_PORCH_WIDTH_V_BACK_PORCH_Pos*/)   /*!< VSYNC BACK PORCH WIDTH: V BACK PORCH Mask */

/* Vertical Data Width Register Definitions */
#define VSYNC_DATA_WIDTH_V_DATA_Pos               0U                                                         /*!< VSYNC DATA WIDTH: V DATA Position */
#define VSYNC_DATA_WIDTH_V_DATA_Msk               (0xFFFUL << /*VSYNC_DATA_WIDTH_V_DATA_Pos*/)               /*!< VSYNC DATA WIDTH: V DATA Mask */

/* Vertical Front Porch Register Definitions */
#define VSYNC_FRONT_PORCH_WIDTH_V_FRONT_PORCH_Pos 0U                                                         /*!< VSYNC FRONT PORCH WIDTH: V FRONT PORCH Position */
#define VSYNC_FRONT_PORCH_WIDTH_V_FRONT_PORCH_Msk (0xFFFUL << /*VSYNC_FRONT_PORCH_WIDTH_V_FRONT_PORCH_Pos*/) /*!< VSYNC FRONT PORCH WIDTH: V FRONT PORCH Mask */

/* Horizontal Sync Width Register Definitions */
#define HSYNC_WIDTH_H_SYNC_Pos                    0U                                                 /*!< HSYNC WIDTH: H SYNC Position */
#define HSYNC_WIDTH_H_SYNC_Msk                    (0xFFFUL << /*HSYNC_WIDTH_H_SYNC_Pos*/)            /*!< HSYNC WIDTH: H SYNC Mask */

/* Horizontal Back Porch Register Definitions */
#define HSYNC_BACK_PORCH_WIDTH_H_BACK_PORCH_Pos   0U                                                         /*!< HSYNC BACK PORCH WIDTH: H BACK PORCH Position */
#define HSYNC_BACK_PORCH_WIDTH_H_BACK_PORCH_Msk   (0xFFFUL << /*HSYNC_BACK_PORCH_WIDTH_H_BACK_PORCH_Pos*/)   /*!< HSYNC BACK PORCH WIDTH: H BACK PORCH Mask */

/* Horizontal Data Width Register Definitions */
#define HSYNC_DATA_WIDTH_H_DATA_Pos               0U                                                         /*!< HSYNC DATA WIDTH: H DATA Position */
#define HSYNC_DATA_WIDTH_H_DATA_Msk               (0xFFFUL << /*HSYNC_DATA_WIDTH_H_DATA_Pos*/)               /*!< HSYNC DATA WIDTH: H DATA Mask */

/* Horizontal Front Porch Register Definitions */
#define HSYNC_FRONT_PORCH_WIDTH_H_FRONT_PORCH_Pos 0U                                                         /*!< HSYNC FRONT PORCH WIDTH: H FRONT PORCH Position */
#define HSYNC_FRONT_PORCH_WIDTH_H_FRONT_PORCH_Msk (0xFFFUL << /*HSYNC_FRONT_PORCH_WIDTH_H_FRONT_PORCH_Pos*/) /*!< HSYNC FRONT PORCH WIDTH: H FRONT PORCH Mask */

/* Polarities Register Definitions */
#define POLARITIES_PXLCLK_POLARITY_Pos            4U                                                 /*!< POLARITIES: PXLCLK POLARITY Position */
#define POLARITIES_PXLCLK_POLARITY_Msk            (0x1UL << POLARITIES_PXLCLK_POLARITY_Pos)          /*!< POLARITIES: PXLCLK POLARITY Mask */

#define POLARITIES_DATA_POLARITY_Pos              3U                                                 /*!< POLARITIES: DATA POLARITY Position */
#define POLARITIES_DATA_POLARITY_Msk              (0x1UL << POLARITIES_DATA_POLARITY_Pos)            /*!< POLARITIES: DATA POLARITY Mask */

#define POLARITIES_DATAEN_POLARITY_Pos            2U                                                 /*!< POLARITIES: DATAEN POLARITY Position */
#define POLARITIES_DATAEN_POLARITY_Msk            (0x1UL << POLARITIES_DATAEN_POLARITY_Pos)          /*!< POLARITIES: DATAEN POLARITY Mask */

#define POLARITIES_HSYNC_POLARITY_Pos             1U                                                 /*!< POLARITIES: HSYNC POLARITY Position */
#define POLARITIES_HSYNC_POLARITY_Msk             (0x1UL << POLARITIES_HSYNC_POLARITY_Pos)           /*!< POLARITIES: HSYNC POLARITY Mask */

#define POLARITIES_VSYNC_POLARITY_Pos             0U                                                 /*!< POLARITIES: VSYNC POLARITY Position */
#define POLARITIES_VSYNC_POLARITY_Msk             (0x1UL /*<< POLARITIES_VSYNC_POLARITY_Pos*/)       /*!< POLARITIES: VSYNC POLARITY Mask */

/* Command Register Definitions */
#define COMMAND_ENABLE_Pos                        0U                                                 /*!< COMMAND: ENABLE Position */
#define COMMAND_ENABLE_Msk                        (0x1UL /*<< COMMAND_ENABLE_Pos*/)                  /*!< COMMAND: ENABLE Mask */

/* Pixel Format Register Definitions */
#define PIXEL_FORMAT_BIG_ENDIAN_Pos               31U                                                /*!< PIXEL FORMAT: BIG ENDIAN Position */
#define PIXEL_FORMAT_BIG_ENDIAN_Msk               (0x1UL << PIXEL_FORMAT_BIG_ENDIAN_Pos)             /*!< PIXEL FORMAT: BIG ENDIAN Mask */

#define PIXEL_FORMAT_BYTES_PER_PIXEL_Pos          3U                                                 /*!< PIXEL FORMAT: BYTES PER PIXEL Position */
#define PIXEL_FORMAT_BYTES_PER_PIXEL_Msk          (0x3UL << PIXEL_FORMAT_BYTES_PER_PIXEL_Pos)        /*!< PIXEL FORMAT: BYTES PER PIXEL Mask */

/* Red Select Register Definitions */
#define COLOR_SELECT_DEFAULT_Pos                  16U                                                /*!< COLOR SELECT: DEFAULT Position */
#define COLOR_SELECT_DEFAULT_Msk                  (0xFFUL << COLOR_SELECT_DEFAULT_Pos)               /*!< COLOR SELECT: DEFAULT Mask */

#define COLOR_SELECT_SIZE_Pos                     8U                                                 /*!< COLOR SELECT: SIZE Position */
#define COLOR_SELECT_SIZE_Msk                     (0xFUL << COLOR_SELECT_SIZE_Pos)                   /*!< COLOR SELECT: SIZE Mask */

#define COLOR_SELECT_OFFSET_Pos                   0U                                                 /*!< COLOR SELECT: OFFSET Position */
#define COLOR_SELECT_OFFSET_Msk                   (0xFUL /*<< COLOR_SELECT_OFFSET_Pos*/)             /*!< COLOR SELECT: OFFSET Mask */

#ifdef __cplusplus
}
#endif

#endif /* __HDLCD_REG_MAP_H__ */
