/*
 * resize_frame.h
 *
 *  Created on: 24-Jan-2022
 *      Author: prasanna
 */

#ifndef IMAGE_PROCESSING_H_
#define IMAGE_PROCESSING_H_

#include <stdint.h>
#include "RTE_Device.h"

#define RGB_BYTES 		3
#define RGBA_BYTES 		4
#define RGB565_BYTES	2
#define PIXEL_BYTES 	1

// Camera dimensions
#define CIMAGE_X		560
#define CIMAGE_Y		560

// ML scaled image dimensions
#define MIMAGE_X		224
#define MIMAGE_Y		224

// Do we get LVGL to zoom the camera image, or do we double it up?
//#define USE_LVGL_ZOOM

#ifdef USE_LVGL_ZOOM
#define LIMAGE_X		MIMAGE_X
#define LIMAGE_Y		MIMAGE_Y
#else
#define LIMAGE_X		(MIMAGE_X * 2)
#define LIMAGE_Y		(MIMAGE_Y * 2)
#endif

// Display dimensions
#define DIMAGE_X        RTE_PANEL_HACTIVE_TIME
#define DIMAGE_Y        RTE_PANEL_VACTIVE_LINE
#define DISPLAY_BUFFER_SIZE  (DIMAGE_X*DIMAGE_Y*RGB_BYTES)

#define TIFF_HDR_NUM_ENTRY 8
#define TIFF_HDR_SIZE 10+TIFF_HDR_NUM_ENTRY*12

/*error status*/
#define FRAME_FORMAT_NOT_SUPPORTED   -1
#define FRAME_OUT_OF_RANGE           -2

#define IMAGE_DATA_SIZE  ((MIMAGE_X*MIMAGE_Y*PIXEL_BYTES*RGB_BYTES)+ TIFF_HDR_SIZE)


typedef struct {
	uint8_t tiff_header[TIFF_HDR_SIZE];
	union {
		uint8_t		bytes[MIMAGE_X*MIMAGE_Y*RGB_BYTES];
		uint32_t  	words[(MIMAGE_X*MIMAGE_Y*RGB_BYTES + 2) / 4];
	} buffer;
} ml_image_t;

int frame_crop(const void * restrict input_fb, uint32_t ip_row_size, uint32_t ip_col_size, uint32_t row_start, uint32_t col_start, void * restrict output_fb, uint32_t op_row_size, uint32_t op_col_size, uint32_t bpp);
int crop_and_interpolate( uint8_t const * restrict srcImage, uint32_t srcWidth, uint32_t srcHeight, uint8_t * restrict dstImage, uint32_t dstWidth, uint32_t dstHeight, uint32_t bpp);
void color_correction(const uint8_t sp[static 3], uint8_t dp[static 3]);
void white_balance(const uint8_t *sp, uint8_t *dp);
uint8_t *put_tiff(uint8_t * rgb, uint32_t width, uint32_t height, uint16_t bpp);
int bayer_to_RGB(uint8_t * restrict src, uint8_t * restrict dest);

#endif /* IMAGE_PROCESSING_H_ */
