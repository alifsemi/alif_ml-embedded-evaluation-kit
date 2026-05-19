/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include "isp_buffer.h"

#if RTE_ISP

#include <stdio.h>

static struct isp_buffers_t isp_buffers[RTE_ISP_BUFFER_COUNT];

static uint8_t isp_buffer_data[RTE_ISP_BUFFER_COUNT][ISP_OUTPUT_TOTAL_SIZE]
    __attribute__((section(".bss.camera_frame_buf"), aligned(32)));

static VIDEO_BUF_S isp_buffer_array[RTE_ISP_BUFFER_COUNT];

extern ARM_DRIVER_ISP Driver_ISP;

/**
 * @brief Configure ISP output buffers for image capture.
 *
 * Initialises the ISP buffer pool based on the requested output dimensions
 * and the compile-time buffer format (planar or interleaved). Each buffer
 * is registered with the ISP driver via ISP_CONTROL_QBUF.
 *
 * @param[in] width  Desired output image width in pixels.
 *                    Must not exceed ISP_OUTPUT_X.
 * @param[in] height Desired output image height in pixels.
 *                    Must not exceed ISP_OUTPUT_Y.
 * @return 0 on success, -1 if dimensions are out of range or the format is
 *         unsupported, or a driver error code if ISP_CONTROL_QBUF fails.
 */
int isp_buffer_configure(uint32_t width, uint32_t height) {

    if (width > ISP_OUTPUT_X || height > ISP_OUTPUT_Y) {
        printf("Error: ISP output dimensions exceed configured (RTE_ISP_OUTPUT_WIDTH x RTE_ISP_OUTPUT_HEIGHT).\n");
        return -1;
    }

    for (int i = 0; i < RTE_ISP_BUFFER_COUNT; i++) {
        isp_buffers[i] = (struct isp_buffers_t) {0};
        isp_buffer_array[i] = (VIDEO_BUF_S) {0};

        isp_buffer_array[i].index = i;
        switch (ISP_AUX_BUFFER_TYPE) {
        case ISP_PLANAR:
            isp_buffers[i].y = isp_buffer_data[i];
            isp_buffers[i].cb = isp_buffer_data[i] + width * height;
            isp_buffers[i].cr = isp_buffer_data[i] + width * height * 2;

            isp_buffer_array[i].numPlanes = 3;
            isp_buffer_array[i].imageSize = width * height * 3;
            isp_buffer_array[i].planes[0].dmaPhyAddr = (vsi_dma_t)isp_buffers[i].y;
            isp_buffer_array[i].planes[1].dmaPhyAddr = (vsi_dma_t)isp_buffers[i].cb;
            isp_buffer_array[i].planes[2].dmaPhyAddr = (vsi_dma_t)isp_buffers[i].cr;
            break;
        case ISP_INTERLEAVED:
            isp_buffers[i].y = isp_buffer_data[i];

            isp_buffer_array[i].numPlanes = 1;
            isp_buffer_array[i].imageSize = width * height * 2;
            isp_buffer_array[i].planes[0].dmaPhyAddr = (vsi_dma_t)isp_buffers[i].y;
            break;

        case ISP_SEMIPLANAR: // Unsupported currently in ALif MLEK, but could be added in future if needed
        case ISP_NONE:
        default:
            printf("Error: Unsupported ISP buffer format.\n");
            return -1;
        }

        int res = Driver_ISP.Control(ISP_CONTROL_QBUF, (uint32_t)&isp_buffer_array[i]);
        if (res != ARM_DRIVER_OK) {
            printf("Error: ISP buffer configuration failed.\n");
            return res;
        }
    }

    return 0;
}

/**
 * @brief Release all ISP buffers previously registered with the driver.
 *
 * Dequeues every buffer in the pool from the ISP driver using
 * ISP_CONTROL_DQBUF.
 *
 * @return 0 on success, or a driver error code if ISP_CONTROL_DQBUF fails.
 */
int isp_buffer_deconfigure() {
    for (int i = 0; i < RTE_ISP_BUFFER_COUNT; i++) {
        int res = Driver_ISP.Control(ISP_CONTROL_DQBUF, (uint32_t)&isp_buffer_array[i]);
        if (res != ARM_DRIVER_OK) {
            return res;
        }
    }

    return 0;
}

/**
 * @brief Retrieve a pointer to an ISP buffer by index.
 *
 * @param[in] index Zero-based index of the buffer to retrieve.
 *                  Must be less than RTE_ISP_BUFFER_COUNT.
 * @return Pointer to the requested isp_buffers_t structure,
 *         or NULL if @p index is out of range.
 */
struct isp_buffers_t * isp_buffer_get(uint32_t index) {
    if (index >= RTE_ISP_BUFFER_COUNT) {
        return NULL;
    }
    return &isp_buffers[index];
}

#endif /* RTE_ISP */
