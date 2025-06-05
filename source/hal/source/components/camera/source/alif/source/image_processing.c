/* This file was ported to work on Alif Semiconductor devices. */

/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
* Use, distribution and modification of this code is permitted under the
* terms stated in the Alif Semiconductor Software License Agreement
*
* You should have received a copy of the Alif Semiconductor Software
* License Agreement with this file. If not, please write to:
* contact@alifsemi.com, or visit: https://alifsemi.com/license
*
*/

/* Edge Impulse ingestion SDK
 * Copyright (c) 2020 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <inttypes.h>
#include "image_processing.h"
#include "bayer.h"
#include "log_macros.h"

#include "timer_alif.h"
#include "RTE_Components.h"
#include "RTE_Device.h"

#if !defined(RTE_Drivers_CPI)
#define USE_FAKE_CAMERA 1
#endif

#if __ARM_FEATURE_MVE & 1
#include <arm_mve.h>
#endif

#if CIMAGE_SW_GAIN_CONTROL
#include "camera.h"
#endif

#define BAYER_FORMAT DC1394_COLOR_FILTER_GRBG

int frame_crop(const void *input_fb,
		       uint32_t ip_row_size,
			   uint32_t ip_col_size,
			   uint32_t row_start,
			   uint32_t col_start,
			   void *output_fb,
			   uint32_t op_row_size,
			   uint32_t op_col_size,
			   uint32_t bpp)
{
	uint8_t *op_fb;
	uint8_t *ip_fb;

	//Checking the boundary
	if(((row_start + op_row_size) > ip_row_size) || ((col_start + op_col_size) > ip_col_size))
		return FRAME_OUT_OF_RANGE;

	// Check for no cropping
	if (row_start == 0 && col_start == 0 && ip_row_size == op_row_size && ip_col_size == op_col_size) {
	    // No-op if cropping and in-place
	    if (input_fb != output_fb) {
	        memcpy(output_fb, input_fb, ip_row_size * op_row_size * (bpp / 8));
	    }
	    return 0;
	}

	//updating the input frame column start
	ip_fb = (uint8_t *)input_fb + (col_start * ip_row_size * (bpp / 8));

    op_fb = (uint8_t *)output_fb;

    for(uint32_t col_cnt = 0; col_cnt < op_col_size; ++col_cnt) {

        //update row address
        const uint8_t *ip_fb_row = ip_fb + row_start * (bpp / 8);

        memmove(op_fb, ip_fb_row, op_row_size * (bpp / 8));

        //update fb
        ip_fb += (ip_row_size * (bpp / 8));
        op_fb += (op_row_size * (bpp / 8));
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
#define FRAC_BITS 14
    const int FRAC_VAL = (1 << FRAC_BITS);
    const int FRAC_MASK = (FRAC_VAL - 1);

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
            uint32_t tx;
            // do indexing computations
            tx = (src_x_accum >> FRAC_BITS) * pixel_size_B;
            x_frac = src_x_accum & FRAC_MASK;
            nx_frac = FRAC_VAL - x_frac; // x fraction and 1.0 - x fraction
            src_x_accum += src_x_frac;
            __builtin_prefetch(&s[tx + 64]);
            __builtin_prefetch(&s[tx + srcWidth + 64]);

#if __ARM_FEATURE_MVE & 1
            uint32x4_t p00 = vldrbq_u32(&s[tx]);
            uint32x4_t p10 = vldrbq_u32(&s[tx + pixel_size_B]);
            uint32x4_t p01 = vldrbq_u32(&s[tx + srcWidth]);
            uint32x4_t p11 = vldrbq_u32(&s[tx + srcWidth + pixel_size_B]);
            p00 = vmulq(p00, nx_frac);
            p00 = vmlaq(p00, p10, x_frac);
            p00 = vrshrq(p00, FRAC_BITS);
            p01 = vmulq(p01, nx_frac);
            p01 = vmlaq(p01, p11, x_frac);
            p01 = vrshrq(p01, FRAC_BITS);
            p00 = vmulq(p00, ny_frac);
            p00 = vmlaq(p00, p01, y_frac);
            p00 = vrshrq(p00, FRAC_BITS);
            vstrbq_p_u32(d, p00, vctp32q(pixel_size_B));
            d += pixel_size_B;
#else
            //interpolate and write out
            for (int color = 0; color < pixel_size_B;
                 color++) // do pixel_size_B times for pixel_size_B colors
            {
                uint32_t p00, p01, p10, p11;
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
#endif
        } // for x
    } // for y
    return 0;
} // resizeImage()

#if __ARM_FEATURE_MVE & 1
#if defined(__clang__)
#define MVE_GOOD (1)
#elif defined (__GNUC__)
#define MVE_GOOD (__GNUC_PREREQ(12,2))
#else
#define MVE_GOOD (1)
#endif
#else
#define MVE_GOOD (0)
#endif //__ARM_FEATURE_MVE

int resize_image_RGB565(
    const uint8_t *srcImage,
    int srcWidth,
    int srcHeight,
    uint8_t *dstImage,
    int dstWidth,
    int dstHeight)
{
    const int FRAC_VAL = (1 << FRAC_BITS);
    const int FRAC_MASK = (FRAC_VAL - 1);

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

    srcWidth *= RGB565_BYTES;
    //srcHeight not used for indexing
    //dstWidth still needed as is
    //dstHeight shouldn't be scaled

#if MVE_GOOD
    const uint32_t corner_offset_vals[4] = { 0, RGB565_BYTES, srcWidth, srcWidth + RGB565_BYTES };
    const uint32x4_t corner_offsets = vldrwq_u32((uint32_t*)corner_offset_vals);
#endif

    const uint8_t *s;
    uint8_t *d;

    for (y = 0; y < dstHeight; y++) {
        // do indexing computations
        ty = src_y_accum >> FRAC_BITS; // src y
        y_frac = src_y_accum & FRAC_MASK;
        src_y_accum += src_y_frac;
        ny_frac = FRAC_VAL - y_frac; // y fraction and 1.0 - y fraction

        s = &srcImage[ty * srcWidth];
        d = &dstImage[y * dstWidth * RGB_BYTES]; //not scaled above
        // start at 1/2 pixel in to account for integer downsampling which might miss pixels
        src_x_accum = FRAC_VAL / 2;
        for (x = 0; x < dstWidth; x++) {
            // do indexing computations
            const uint32_t tx = (src_x_accum >> FRAC_BITS) * RGB565_BYTES;
            x_frac = src_x_accum & FRAC_MASK;
            nx_frac = FRAC_VAL - x_frac; // x fraction and 1.0 - x fraction
            src_x_accum += src_x_frac;
            __builtin_prefetch(&s[tx + 64]);
            __builtin_prefetch(&s[tx + srcWidth + 64]);

#if MVE_GOOD
            uint32x4_t corners = vldrhq_gather_offset_u32((const uint16_t *) (s + tx), corner_offsets);

            // Reinterpret to uint8 and fill LSB bits
            uint8x16_t blue = vreinterpretq_u8(corners);
            blue = vshlq_n(blue, 3);
            blue = vsriq(blue, blue, 5);

            corners = vshrq_n_u32(corners, 3);
            uint8x16_t green = vreinterpretq_u8(corners);
            green = vsriq(green, green, 6);

            corners = vshrq_n_u32(corners, 5);
            uint8x16_t red = vreinterpretq_u8(corners);
            red = vsriq(red, red, 5);

            // We now have 4 corners packed in separate red, green blue registers
            // Permute so we have red green blue packed in separate corner registers
            uint8x16x4_t rgbx = { red, green, blue, vuninitializedq_u8() };
            uint8_t swapbuf[64];
            vst4q_u8(swapbuf, rgbx);
            uint32x4_t p00 = vldrbq_u32(swapbuf + 0);
            uint32x4_t p10 = vldrbq_u32(swapbuf + 16);
            uint32x4_t p01 = vldrbq_u32(swapbuf + 32);
            uint32x4_t p11 = vldrbq_u32(swapbuf + 48);

            p00 = vmulq(p00, nx_frac);
            p00 = vmlaq(p00, p10, x_frac);
            p00 = vrshrq(p00, FRAC_BITS);
            p01 = vmulq(p01, nx_frac);
            p01 = vmlaq(p01, p11, x_frac);
            p01 = vrshrq(p01, FRAC_BITS);
            p00 = vmulq(p00, ny_frac);
            p00 = vmlaq(p00, p01, y_frac);
            p00 = vrshrq(p00, FRAC_BITS);
            vstrbq_p_u32(d, p00, vctp32q(RGB_BYTES));

            d += RGB_BYTES;
#else
            //interpolate and write out
            uint16_t rgb565_p00 = *((uint16_t*)&s[tx]);
            uint16_t rgb565_p10 = *((uint16_t*)&s[tx + RGB565_BYTES]);
            uint16_t rgb565_p01 = *((uint16_t*)&s[tx + srcWidth]);
            uint16_t rgb565_p11 = *((uint16_t*)&s[tx + srcWidth + RGB565_BYTES]);

            uint8_t rgb_p00[RGB_BYTES];
            uint8_t rgb_p10[RGB_BYTES];
            uint8_t rgb_p01[RGB_BYTES];
            uint8_t rgb_p11[RGB_BYTES];

            // blue
            rgb_p00[2] = ((rgb565_p00 & 0x1F) << 3) | ((rgb565_p00 & 0x1C) >> 2);
            rgb_p10[2] = ((rgb565_p10 & 0x1F) << 3) | ((rgb565_p10 & 0x1C) >> 2);
            rgb_p01[2] = ((rgb565_p01 & 0x1F) << 3) | ((rgb565_p01 & 0x1C) >> 2);
            rgb_p11[2] = ((rgb565_p11 & 0x1F) << 3) | ((rgb565_p11 & 0x1C) >> 2);
            rgb565_p00 >>= 3;
            rgb565_p10 >>= 3;
            rgb565_p01 >>= 3;
            rgb565_p11 >>= 3;
            // green
            rgb_p00[1] = (rgb565_p00 & 0xFC) | ((rgb565_p00 & 0xC0) >> 6);
            rgb_p10[1] = (rgb565_p10 & 0xFC) | ((rgb565_p10 & 0xC0) >> 6);
            rgb_p01[1] = (rgb565_p01 & 0xFC) | ((rgb565_p01 & 0xC0) >> 6);
            rgb_p11[1] = (rgb565_p11 & 0xFC) | ((rgb565_p11 & 0xC0) >> 6);
            rgb565_p00 >>= 5;
            rgb565_p10 >>= 5;
            rgb565_p01 >>= 5;
            rgb565_p11 >>= 5;
            // red
            rgb_p00[0] = (rgb565_p00 & 0xF8) | ((rgb565_p00 & 0xE0) >> 5);
            rgb_p10[0] = (rgb565_p10 & 0xF8) | ((rgb565_p10 & 0xE0) >> 5);
            rgb_p01[0] = (rgb565_p01 & 0xF8) | ((rgb565_p01 & 0xE0) >> 5);
            rgb_p11[0] = (rgb565_p11 & 0xF8) | ((rgb565_p11 & 0xE0) >> 5);

            for (int color = 0; color < RGB_BYTES; color++)
            {
                uint32_t p00, p01, p10, p11;
                p00 = rgb_p00[color];
                p10 = rgb_p10[color];
                p01 = rgb_p01[color];
                p11 = rgb_p11[color];

                p00 = ((p00 * nx_frac) + (p10 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // top line
                p01 = ((p01 * nx_frac) + (p11 * x_frac) + FRAC_VAL / 2) >> FRAC_BITS; // bottom line
                p00 = ((p00 * ny_frac) + (p01 * y_frac) + FRAC_VAL / 2) >> FRAC_BITS; //top + bottom
                *d++ = (uint8_t)p00; // store new pixel
            }
#endif
        } // for x
    } // for y
    return 0;
} // resizeImage()
#undef FRAC_BITS

int resize_image(
    const uint8_t *srcImage,
    int srcWidth,
    int srcHeight,
    uint8_t *dstImage,
    int dstWidth,
    int dstHeight,
    int pixel_size_src_B)
{
    if (pixel_size_src_B == 3) {
        return resize_image_A(srcImage, srcWidth, srcHeight,
                              dstImage, dstWidth, dstHeight,
                              pixel_size_src_B);
    }
    if (pixel_size_src_B == 2) {
        return resize_image_RGB565(srcImage, srcWidth, srcHeight,
                                   dstImage, dstWidth, dstHeight);
    }

    abort();
}

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

int crop_and_interpolate( uint8_t *image,
						  uint32_t srcWidth,
						  uint32_t srcHeight,
                          uint8_t *dstImage,
						  uint32_t dstWidth,
						  uint32_t dstHeight,
						  uint32_t bpp)
{
    uint32_t cropWidth, cropHeight;
    extern uint32_t tprof2, tprof3;
    if (bpp != 24 && bpp != 16) {
        abort();
    }
    tprof2 = Get_SysTick_Cycle_Count32();
    // What are dimensions that maintain aspect ratio?
    calculate_crop_dims(srcWidth, srcHeight, dstWidth, dstHeight, &cropWidth, &cropHeight);
    // Now crop to that dimension, in place
    int res = frame_crop(
        image,
        srcWidth,
        srcHeight,
        (srcWidth - cropWidth) / 2,
        (srcHeight - cropHeight) / 2,
        image,
        cropWidth,
        cropHeight,
		bpp);

    if( res < 0 ) { return res; }
    tprof2 = Get_SysTick_Cycle_Count32() - tprof2;

    tprof3 = Get_SysTick_Cycle_Count32();
    // Finally, interpolate down to desired dimensions, in place or to destination buffer
    int result = resize_image(image, cropWidth, cropHeight, dstImage, dstWidth, dstHeight, bpp/8);
    tprof3 = Get_SysTick_Cycle_Count32() - tprof3;
    return result;
}


static float current_log_gain = 0.0;
static int32_t current_api_gain = 0;
static int32_t last_requested_api_gain = 0;
static float minimum_log_gain = -INFINITY;
static float maximum_log_gain = +INFINITY;

float get_image_gain(void)
{
    return current_api_gain * 0x1p-16f;
}

static float api_gain_to_log(int32_t api)
{
    return logf(api * 0x1p-16f);
}

static int32_t log_gain_to_api(float gain)
{
    return expf(gain) * 0x1p16f;
}

#if CIMAGE_SW_GAIN_CONTROL
static void process_autogain(void)
{
    /* Simple "auto-exposure" algorithm. We work a single "gain" value
     * and leave it up to the camera driver how this is produced through
     * adjusting exposure time, analogue gain or digital gain.
     *
     * We us a discrete velocity form of a PI controller to adjust the
     * current gain to try to make the difference between high pixels
     * and low pixels hit a target.
     *
     * The definition of "low" and "high" pixels has quite an effect on the
     * end result - this is set over in bayer2rgb.c, which does the analysis.
     * It gives us 4 counts for high/low (>80% / <20%) pixels and over/under-
     * exposed (255 or 0).
     *
     * We reduce to 2 counts by using high|low + (weight * over|under),
     * effectively counting the over/under-exposed pixels weight more times.
     */
    const float target_highlow_difference = 0.0f;
    const float overunder_weight = 4.0f;

    // Control constants - output is to the logarithm of gain (as if
    // we're working in decibels), so 1 here would mean a factor of e
    // adjustment if every pixel was high.
    const float Kp = 1.0f * 0.45f;
    const float Ki = 1.0f * 0.54f / 2.0f;
    const float tiny_error = 0x1p-4f;

    static float previous_error = 0;

    if (current_api_gain <= 0) {
        if (current_api_gain < 0) {
            return;
        }
        current_api_gain = camera_gain(0); // Read initial gain
        if (current_api_gain < 0) {
            printf_err("camera_gain(0) returned error %" PRId32 "; disabling autogain\n", current_api_gain);
            return;
        }
        current_log_gain = api_gain_to_log(current_api_gain);
    }

    /* Rescale high-low difference in pixel counts so that it's
     * in range [-1..+1], regardless of image size */
    float high_proportion = (exposure_high_count + overunder_weight * exposure_over_count) * (1.0f / (CIMAGE_X * CIMAGE_Y));
    float low_proportion = (exposure_low_count + overunder_weight * exposure_under_count) * (1.0f / (CIMAGE_X * CIMAGE_Y));
    float highlow_difference = high_proportion - low_proportion;
    float error = highlow_difference - target_highlow_difference;

    /* Ignore small errors, so we don't oscillate */
    if (fabsf(error) < tiny_error * fmaxf(high_proportion, low_proportion)) {
        error = 0;
    }

    float delta_controller_output = (Kp + Ki) * error - Kp * previous_error;

    previous_error = error;
    current_log_gain = current_log_gain - delta_controller_output;

    /* Clamp according to limits we've found from the gain API */
    current_log_gain = fminf(current_log_gain, maximum_log_gain);
    current_log_gain = fmaxf(current_log_gain, minimum_log_gain);

    int32_t desired_api_gain = log_gain_to_api(current_log_gain);
    if (desired_api_gain <= 0) {
        desired_api_gain = 1;
    }

    /* Apply the gain, if it's a new request */
    if (desired_api_gain != last_requested_api_gain) {
        int32_t ret = camera_gain(desired_api_gain);
        if (ret < 0) {
            printf_err("Camera gain error %" PRId32 "\n", ret);
            return;
        }
        last_requested_api_gain = desired_api_gain;
        current_api_gain = ret;
        debug("Camera gain changed to %.3f\n", current_api_gain * 0x1p-16f);

        /* Check for saturation, and record it. Knowing our limits avoids
         * making pointless calls to the gain changing API.
         */
        float deviation = ((float) current_api_gain - desired_api_gain) / desired_api_gain;
        if (fabsf(deviation) > 0.25f) {
            if (deviation < 0) {
                maximum_log_gain = api_gain_to_log(current_api_gain);
                debug("Noted maximum gain %.3f\n", current_api_gain * 0x1p-16f);
            } else {
                minimum_log_gain = api_gain_to_log(current_api_gain);
                debug("Noted minimum gain %.3f\n", current_api_gain * 0x1p-16f);
            }
            current_log_gain = api_gain_to_log(current_api_gain);
        }
    }
}
#endif

const uint8_t *get_image_data(int ml_width, int ml_height, tiff_header_t tiff_header, uint8_t *image_data, int image_size, uint8_t *raw_image)
{
    extern uint32_t tprof1, tprof2, tprof3, tprof4;
#ifdef USE_FAKE_CAMERA
    static int roll = 0;
    for (int y = 0; y < CIMAGE_Y; y+=2) {
    	uint8_t *p = raw_image + y * CIMAGE_X;
    	int bar = (7 * ((y+roll) % CIMAGE_Y)) / CIMAGE_Y + 1;
    	float barb = bar & 1 ? 255 : 0;
    	float barr = bar & 2 ? 255 : 0;
    	float barg = bar & 4 ? 255 : 0;
    	for (int x = 0; x < CIMAGE_X; x+=2) {
    		float intensity = x * (1.0f/(CIMAGE_X-2));
    		float r = barr * intensity + 0.5f;
    		float g = barg * intensity + 0.5f;
    		float b = barb * intensity + 0.5f;
            if (BAYER_FORMAT == DC1394_COLOR_FILTER_BGGR) {
                p[0]        = b; p[1]            = g;
                p[CIMAGE_X] = g; p[CIMAGE_X + 1] = r;
            } else if (BAYER_FORMAT == DC1394_COLOR_FILTER_GRBG) {
                p[0]        = g; p[1]            = r;
                p[CIMAGE_X] = b; p[CIMAGE_X + 1] = g;
            }
    		p += 2;
    	}
    }
    roll = (roll + 1) % CIMAGE_Y;
#endif

#if !CIMAGE_USE_RGB565
    /* TIFF image can be dumped in Arm Development Studio using the command
     *
     *     dump value camera.tiff rgb_image
     *
     * while stopped at an appropriate breakpoint below.
     */
    write_tiff_header(&tiff_header, CIMAGE_X, CIMAGE_Y);
    tprof1 = Get_SysTick_Cycle_Count32();
    // RGB conversion and frame resize
    dc1394_bayer_Simple(raw_image, image_data, CIMAGE_X, CIMAGE_Y, BAYER_FORMAT);
    tprof1 = Get_SysTick_Cycle_Count32() - tprof1;
#endif

#ifndef USE_FAKE_CAMERA
#if CIMAGE_SW_GAIN_CONTROL
    // Use pixel analysis from bayer_to_RGB to adjust gain
    process_autogain();
#endif
#endif

    // Cropping and scaling
#if CIMAGE_USE_RGB565
    if (ml_width * ml_height * RGB_BYTES > image_size) {
        printf_err("Requested image does not fit to RGB buffer.\n");
        return NULL;
    }
    crop_and_interpolate(raw_image, CIMAGE_X, CIMAGE_Y,
                         image_data, ml_width, ml_height,
                         RGB565_BYTES * 8);
#else
    if (ml_width > CIMAGE_X || ml_height > CIMAGE_Y) {
        printf_err("Requested image can't be processed in place\n");
        return NULL;
    }
    crop_and_interpolate(image_data, CIMAGE_X, CIMAGE_Y,
                         image_data, ml_width, ml_height, RGB_BYTES * 8);
#endif
    // Rewrite the TIFF header for the new size
    write_tiff_header(&tiff_header, ml_width, ml_height);

#if CIMAGE_COLOR_CORRECTION
    tprof4 = Get_SysTick_Cycle_Count32();
    // Color correction for white balance
    white_balance(ml_width, ml_height, image_data, image_data);
    tprof4 = Get_SysTick_Cycle_Count32() - tprof4;
#endif
    return image_data;
}
