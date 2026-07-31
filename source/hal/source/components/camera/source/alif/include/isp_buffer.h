/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef ISP_BUFFER_H_
#define ISP_BUFFER_H_

#include "RTE_Device.h"
#if RTE_ISP
#include "Driver_ISP.h"
#include "Driver_CPI.h"
#include "vsi_comm_video.h"

enum {
    ISP_PLANAR = 1,
    ISP_SEMIPLANAR,
    ISP_INTERLEAVED,
    ISP_NONE,
};


/* ISP Output image width */
#define ISP_OUTPUT_X    RTE_ISP_OUTPUT_WIDTH

/* ISP output image height */
#define ISP_OUTPUT_Y    RTE_ISP_OUTPUT_HEIGHT

#if (RTE_ISP_OUTPUT_FORMAT == 32) // YUYV (YUV422 packed) output from ISP
#define ISP_AUX_BUFFER_TYPE  ISP_INTERLEAVED
#define ISP_OUTPUT_TOTAL_SIZE (ISP_OUTPUT_X * ISP_OUTPUT_Y * 2)
#elif (RTE_ISP_OUTPUT_FORMAT == 39) // RGB888 planar output from ISP
#define ISP_AUX_BUFFER_TYPE  ISP_PLANAR
#define ISP_OUTPUT_TOTAL_SIZE (ISP_OUTPUT_X * ISP_OUTPUT_Y * 3)
#else
#error "Unsupported ISP output format."
#endif

struct isp_buffers_t {
    uint8_t *y;
    uint8_t *cb;
    uint8_t *cr;
    uint8_t *cbcr;
};

int isp_buffer_configure(uint32_t width, uint32_t height);
int isp_buffer_deconfigure();

struct isp_buffers_t * isp_buffer_get(uint32_t index);

#endif  // RTE_ISP

#endif
