/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "image_processing.h"

#include "RTE_Components.h"

int frame_crop(const void * restrict input_fb,
		       uint32_t ip_row_size,
			   uint32_t ip_col_size,
			   uint32_t row_start,
			   uint32_t col_start,
			   void * restrict output_fb,
			   uint32_t op_row_size,
			   uint32_t op_col_size,
			   uint32_t bpp)
{
	uint32_t *op_fb = NULL, *ip_fb_row = NULL;
	uint8_t  *ip_fb = NULL, *ip8_fb_row = NULL, *op8_fb = NULL;
	uint32_t col_cnt, row_cnt;

	//Checking the boundary
	if(((row_start + op_row_size) > ip_row_size) || ((col_start + op_col_size) > ip_col_size))
		return FRAME_OUT_OF_RANGE;

	//updating the input frame column start
	ip_fb = (col_start * ip_row_size * (bpp / 8)) + (uint8_t *)input_fb ;


	if((32 % bpp) == 0) {
		//8bit and 16bit bpp will be handled here.
		op_fb = (uint32_t *)output_fb;


		for(col_cnt = 0; col_cnt < op_col_size; ++col_cnt) {

			//update row address
			ip_fb_row = (uint32_t *)((row_start * (bpp / 8)) + ip_fb);

			for(row_cnt = 0; row_cnt < (op_row_size / (32 / bpp)); ++row_cnt)
				op_fb[row_cnt] = ip_fb_row[row_cnt];

			//copy remaining pixels
			if(op_row_size % (32 / bpp))
				op_fb[row_cnt] = ip_fb_row[row_cnt];

			//update fb
			ip_fb = (ip_row_size * (bpp / 8)) + ip_fb;
			op_fb = (uint32_t *)((op_row_size * (bpp / 8)) + (uint8_t *)op_fb);
		}
	} else {
		//24bit bpp will be handled here.
		op8_fb = (uint8_t *)output_fb;


		for(col_cnt = 0; col_cnt < op_col_size; ++col_cnt) {

			//update row address
			ip8_fb_row = (row_start * (bpp / 8)) + ip_fb;

			//copy pixels
			for(row_cnt = 0; row_cnt < (op_row_size * (bpp / 8)); ++row_cnt)
				op8_fb[row_cnt] = ip8_fb_row[row_cnt];

			//update fb
			ip_fb = ((ip_row_size * (bpp / 8)) + ip_fb);
			op8_fb = ((op_row_size * (bpp / 8)) + op8_fb);
		}

	}

	return 0;
}




int resize_image_A(
    const uint8_t *srcImage,
    int srcWidth,
    int srcHeight,
    uint8_t *dstImage,
    int dstWidth,
    int dstHeight,
    int pixel_size_B)
{
    if (pixel_size_B != 3) {
        abort();
    }
// Copied from ei_camera.cpp in firmware-eta-compute
// Modified for RGB888
// This needs to be < 16 or it won't fit. Cortex-M4 only has SIMD for signed multiplies
    int FRAC_BITS = 14;
    int FRAC_VAL = (1 << FRAC_BITS);
    int FRAC_MASK = (FRAC_VAL - 1);

    uint32_t src_x_accum, src_y_accum; // accumulators and fractions for scaling the image
    uint32_t x_frac, nx_frac, y_frac, ny_frac;
    int x, y, ty;

    if (srcHeight < 2) {
        return FRAME_OUT_OF_RANGE;
    }

    // start at 1/2 pixel in to account for integer downsampling which might miss pixels
    src_y_accum = FRAC_VAL / 2;
    const uint32_t src_x_frac = (srcWidth * FRAC_VAL) / dstWidth;
    const uint32_t src_y_frac = (srcHeight * FRAC_VAL) / dstHeight;

    //from here out, *3 b/c RGB
    srcWidth *= pixel_size_B;
    //srcHeight not used for indexing
    //dstWidth still needed as is
    //dstHeight shouldn't be scaled

    const uint8_t *s;
    uint8_t *d;

    for (y = 0; y < dstHeight; y++) {
        // do indexing computations
        ty = src_y_accum >> FRAC_BITS; // src y
        y_frac = src_y_accum & FRAC_MASK;
        src_y_accum += src_y_frac;
        ny_frac = FRAC_VAL - y_frac; // y fraction and 1.0 - y fraction

        s = &srcImage[ty * srcWidth];
        d = &dstImage[y * dstWidth * pixel_size_B]; //not scaled above
        // start at 1/2 pixel in to account for integer downsampling which might miss pixels
        src_x_accum = FRAC_VAL / 2;
        for (x = 0; x < dstWidth; x++) {
            uint32_t tx, p00, p01, p10, p11;
            // do indexing computations
            tx = (src_x_accum >> FRAC_BITS) * pixel_size_B;
            x_frac = src_x_accum & FRAC_MASK;
            nx_frac = FRAC_VAL - x_frac; // x fraction and 1.0 - x fraction
            src_x_accum += src_x_frac;

            //interpolate and write out
            for (int color = 0; color < pixel_size_B;
                 color++) // do pixel_size_B times for pixel_size_B colors
            {
                p00 = s[tx];
                p10 = s[tx + pixel_size_B];
                p01 = s[tx + srcWidth];
                p11 = s[tx + srcWidth + pixel_size_B];
                p00 = ((p00 * nx_frac) + (p10 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // top line
                p01 = ((p01 * nx_frac) + (p11 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // bottom line
                p00 = ((p00 * ny_frac) + (p01 * y_frac) + FRAC_VAL / 2) >> FRAC_BITS; //top + bottom
                *d++ = (uint8_t)p00; // store new pixel
                //ready next loop
                tx++;
            }
        } // for x
    } // for y
    return 0;
} // resizeImage()

void calculate_crop_dims(uint32_t srcWidth,
						 uint32_t srcHeight,
						 uint32_t dstWidth,
						 uint32_t dstHeight,
						 uint32_t *cropWidth,
						 uint32_t *cropHeight)
{
    //first, trim the largest axis to match destination aspect ratio
    //calculate by fixing the smaller axis
    if (srcWidth > srcHeight) {
        *cropWidth = (uint32_t)(dstWidth * srcHeight) / dstHeight; //cast in case int is small
        *cropHeight = srcHeight;
    }
    else {
        *cropHeight = (uint32_t)(dstHeight * srcWidth) / dstWidth;
        *cropWidth = srcWidth;
    }
}

int crop_and_interpolate( uint8_t const * restrict srcImage,
						  uint32_t srcWidth,
						  uint32_t srcHeight,
						  uint8_t * restrict dstImage,
						  uint32_t dstWidth,
						  uint32_t dstHeight,
						  uint32_t bpp)
{
    uint32_t cropWidth, cropHeight;
    extern uint32_t tprof1, tprof2, tprof3, tprof4, tprof5;
    if (bpp != 24) {
        abort();
    }
    tprof2 = ARM_PMU_Get_CCNTR();
    // What are dimensions that maintain aspect ratio?
    calculate_crop_dims(srcWidth, srcHeight, dstWidth, dstHeight, &cropWidth, &cropHeight);
    // Now crop to that dimension
    int res = frame_crop(
        srcImage,
        srcWidth,
        srcHeight,
        (srcWidth - cropWidth) / 2,
        (srcHeight - cropHeight) / 2,
        dstImage,
        cropWidth,
        cropHeight,
		bpp);

    if( res < 0 ) { return res; }
    tprof2 = ARM_PMU_Get_CCNTR() - tprof2;

    tprof3 = ARM_PMU_Get_CCNTR();
    // Finally, interpolate down to desired dimensions, in place
    int result = resize_image_A(dstImage, cropWidth, cropHeight, dstImage, dstWidth, dstHeight, bpp/8);
    tprof3 = ARM_PMU_Get_CCNTR() - tprof3;
    return result;
}




