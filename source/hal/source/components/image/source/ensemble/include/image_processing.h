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

#define RGB_BYTES 		3
#define RGBA_BYTES 		4
#define RGB565_BYTES	2
#define PIXEL_BYTES 	1

// Camera dimensions
#if RTE_MT9M114_CAMERA_SENSOR_MIPI_ENABLE
#define CIMAGE_X                (RTE_MT9M114_CAMERA_SENSOR_MIPI_FRAME_WIDTH)
#define CIMAGE_Y                (RTE_MT9M114_CAMERA_SENSOR_MIPI_FRAME_HEIGHT)
#define CIMAGE_COLOR_CORRECTION (0)
#define CIMAGE_SW_GAIN_CONTROL  (0)
#define CIMAGE_USE_RGB565       (RTE_MT9M114_CAMERA_SENSOR_MIPI_CSI_DATA_TYPE == 0x22)
#if CIMAGE_USE_RGB565
#define CIMAGE_RGB_WIDTH_MAX    (320)
#define CIMAGE_RGB_HEIGHT_MAX   (320)
#else
#define CIMAGE_RGB_WIDTH_MAX    CIMAGE_X
#define CIMAGE_RGB_HEIGHT_MAX   CIMAGE_Y
#endif // CIMAGE_USE_RGB565
#elif RTE_ARX3A0_CAMERA_SENSOR_CSI_ENABLE
#define CIMAGE_X                (RTE_ARX3A0_CAMERA_SENSOR_FRAME_WIDTH)
#define CIMAGE_Y                (RTE_ARX3A0_CAMERA_SENSOR_FRAME_HEIGHT)
#define CIMAGE_COLOR_CORRECTION (1)
#define CIMAGE_SW_GAIN_CONTROL  (1)
#define CIMAGE_USE_RGB565       (0)
#define CIMAGE_RGB_WIDTH_MAX    CIMAGE_X
#define CIMAGE_RGB_HEIGHT_MAX   CIMAGE_Y
#elif RTE_ARX3A0_CAMERA_SENSOR_ENABLE // REV A RTE
#define CIMAGE_X                (560)
#define CIMAGE_Y                (560)
#define CIMAGE_COLOR_CORRECTION (1)
#define CIMAGE_SW_GAIN_CONTROL  (1)
#define CIMAGE_USE_RGB565       (0)
#define CIMAGE_RGB_WIDTH_MAX    CIMAGE_X
#define CIMAGE_RGB_HEIGHT_MAX   CIMAGE_Y
#else
#error "Unsupported camera"
#endif

/*error status*/
#define FRAME_FORMAT_NOT_SUPPORTED   -1
#define FRAME_OUT_OF_RANGE           -2


extern uint32_t exposure_under_count, exposure_low_count, exposure_high_count, exposure_over_count;

int frame_crop(const void *input_fb, uint32_t ip_row_size, uint32_t ip_col_size, uint32_t row_start, uint32_t col_start, void *output_fb, uint32_t op_row_size, uint32_t op_col_size, uint32_t bpp);
int crop_and_interpolate(uint8_t *image, uint32_t srcWidth, uint32_t srcHeight, uint8_t *dstImage, uint32_t dstWidth, uint32_t dstHeight, uint32_t bpp);
void white_balance(int width, int height, const uint8_t *sp, uint8_t *dp);
int bayer_to_RGB(uint8_t *src, uint8_t *dest);

#endif /* IMAGE_PROCESSING_H_ */
