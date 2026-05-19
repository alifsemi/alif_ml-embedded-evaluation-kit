/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef IMAGE_PROCESSING_H_
#define IMAGE_PROCESSING_H_

#include <stdint.h>
#include <RTE_Device.h>
#include "tiff.h"

#define RGB_BYTES 		3
#define RGBA_BYTES 		4
#define RGB565_BYTES	2
#define PIXEL_BYTES 	1


#if RTE_ISP
// Do not use CIMAGE_X, CIMAGE_Y as these dimensions are dynamically set at runtime for ISP mode (see hal_camera_configure() and camera_set_isp_resolution())
// ISP output buffer is allocated according to RTE_ISP_OUTPUT_WIDTH and RTE_ISP_OUTPUT_HEIGHT setting the maximum supported dimensions for hal_camera_configure().
// ISP output can be RGB planar or YUV depending on configuration, but will be converted to RGB888 in get_image_data() before ML inference
// The RGB buffer is allocated according to CIMAGE_RGB_WIDTH_MAX and CIMAGE_RGB_HEIGHT_MAX
#define CIMAGE_COLOR_CORRECTION (0) // Only apply SW color correction in SW pipeline
#define CIMAGE_SW_GAIN_CONTROL  (0)
#define CIMAGE_USE_RGB565       (0)
#define CIMAGE_RGB_WIDTH_MAX    (RTE_ISP_OUTPUT_WIDTH)
#define CIMAGE_RGB_HEIGHT_MAX   (RTE_ISP_OUTPUT_HEIGHT)

#else // Non-ISP SW pipeline dimensions, capture at full resolution and crop/rescale in software to desired dimensions

// Camera dimensions
#if RTE_MT9M114_CAMERA_SENSOR_MIPI_ENABLE
#if RTE_MT9M114_CAMERA_SENSOR_MIPI_IMAGE_CONFIG == 2
    #define CIMAGE_X            (1280)
    #define CIMAGE_Y            (720)
    #define CIMAGE_USE_RGB565   (1)
#else
    #error "Unsupported MT9M114 configuration"
#endif

#define CIMAGE_COLOR_CORRECTION (0)
#define CIMAGE_SW_GAIN_CONTROL  (0)
#if CIMAGE_USE_RGB565
#define CIMAGE_RGB_WIDTH_MAX    (320)
#define CIMAGE_RGB_HEIGHT_MAX   (320)
#else
#define CIMAGE_RGB_WIDTH_MAX    CIMAGE_X
#define CIMAGE_RGB_HEIGHT_MAX   CIMAGE_Y
#endif // CIMAGE_USE_RGB565
#elif RTE_OV5675_CAMERA_SENSOR_ENABLE
#define CIMAGE_X_ORIG           (RTE_OV5675_CAMERA_SENSOR_FRAME_WIDTH)
#define CIMAGE_Y_ORIG           (RTE_OV5675_CAMERA_SENSOR_FRAME_HEIGHT)
#define CIMAGE_X                (480)
#define CIMAGE_Y                (480)
#define CIMAGE_COLOR_CORRECTION (0)
#define CIMAGE_SW_GAIN_CONTROL  (0)
#define CIMAGE_USE_RGB565       (0)
#define CIMAGE_RGB_WIDTH_MAX    CIMAGE_X
#define CIMAGE_RGB_HEIGHT_MAX   CIMAGE_Y
#elif RTE_ARX3A0_CAMERA_SENSOR_CSI_ENABLE
#define CIMAGE_X                (RTE_ARX3A0_CAMERA_SENSOR_FRAME_WIDTH)
#define CIMAGE_Y                (RTE_ARX3A0_CAMERA_SENSOR_FRAME_HEIGHT)
#define CIMAGE_COLOR_CORRECTION (1)
#define CIMAGE_SW_GAIN_CONTROL  (1)
#define CIMAGE_USE_RGB565       (0)
#define CIMAGE_RGB_WIDTH_MAX    CIMAGE_X
#define CIMAGE_RGB_HEIGHT_MAX   CIMAGE_Y
#elif !defined(RTE_Device_CPI)
#define CIMAGE_X                (0)
#define CIMAGE_Y                (0)
#define CIMAGE_COLOR_CORRECTION (0)
#define CIMAGE_SW_GAIN_CONTROL  (0)
#define CIMAGE_USE_RGB565       (0)
#define CIMAGE_RGB_WIDTH_MAX    CIMAGE_X
#define CIMAGE_RGB_HEIGHT_MAX   CIMAGE_Y
#else
#error "Unsupported camera"
#endif

#endif 
/*error status*/
#define FRAME_FORMAT_NOT_SUPPORTED   -1
#define FRAME_OUT_OF_RANGE           -2


int frame_crop(const void *input_fb, uint32_t ip_row_size, uint32_t ip_col_size, uint32_t row_start, uint32_t col_start, void *output_fb, uint32_t op_row_size, uint32_t op_col_size, uint32_t bpp);
int crop_and_interpolate(uint8_t *image, uint32_t srcWidth, uint32_t srcHeight, uint8_t *dstImage, uint32_t dstWidth, uint32_t dstHeight, uint32_t bpp);
void white_balance(int width, int height, const uint8_t *sp, uint8_t *dp);
int bayer_to_RGB(uint8_t *src, uint8_t *dest);

const uint8_t *get_image_data(int ml_width, int ml_height, tiff_header_t tiff_header, uint8_t *image_data, int image_size, uint8_t *raw_image);
#endif /* IMAGE_PROCESSING_H_ */
