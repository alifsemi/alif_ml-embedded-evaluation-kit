/*
 * color_correction.c
 *
 *  Created on: Feb 19, 2022
 *      Author: Eugene Bonev
 */

#include <stdint.h>
#include "base_def.h"
#include "image_processing.h"

#ifdef HELIUM
#include "dsp/matrix_functions.h"

static arm_matrix_instance_f32 ccm =
{
	.numRows = 3,
	.numCols = 3,
	.pData = ccm_data
};

arm_matrix_instance_f32 spixel =
{
	.numRows = 3,
	.numCols = 1,
	.pData = NULL
};

arm_matrix_instance_f32 dpixel =
{
	.numRows = 3,
	.numCols = 1,
	.pData = NULL
};

#endif

#ifndef float32_t
#define float32_t float
#endif


static float32_t ccm_data[9] =
{ 2.092, -0.369, -0.636,
 -0.492,  1.315,  0.162,
 -0.139, -0.664,  3.017};


void color_correction(uint8_t *sp, uint8_t *dp)
{
	static float32_t dpixel_data[3];
//	volatile static uint32_t t0, ts;
//	ts = PMU_GetCounter();
#ifdef HELIUM
	static float32_t spixel_data[3];

	spixel.pData = spixel_data;
	dpixel.pData = dpixel_data;

	spixel.pData[0] = (float32_t)sp[2];
	spixel.pData[1] = (float32_t)sp[1];
	spixel.pData[2] = (float32_t)sp[0];
	arm_mat_mult_f32(&ccm, &spixel, &dpixel);
#else
#if 0
	dpixel_data[0] = sp[2];
	dpixel_data[1] = sp[1];
	dpixel_data[2] = sp[0];
#else

	dpixel_data[0] =  2.092*sp[2] - 0.369*sp[1] - 0.636*sp[0];
	dpixel_data[1] = -0.492*sp[2] + 1.315*sp[1] + 0.162*sp[0];
	dpixel_data[2] = -0.139*sp[2] - 0.664*sp[1] + 3.017*sp[0];

#endif
#endif
//	t0 = PMU_GetCounter() - ts;
	if (dpixel_data[0] < 0) dpixel_data[0] = 0;
	if (dpixel_data[1] < 0) dpixel_data[1] = 0;
	if (dpixel_data[2] < 0) dpixel_data[2] = 0;

	if (dpixel_data[0] > 255) dpixel_data[0] = 255;
	if (dpixel_data[1] > 255) dpixel_data[1] = 255;
	if (dpixel_data[2] > 255) dpixel_data[2] = 255;
	dp[2] = (uint8_t)dpixel_data[0]; // 2 = RED
	dp[1] = (uint8_t)dpixel_data[1]; // 1 = GREEN
	dp[0] = (uint8_t)dpixel_data[2]; // 0 = BLUE
}

#define OFFS_X 120
#define OFFS_Y 200

void white_balance(uint8_t *sp, uint8_t *dp)
{
	uint32_t x2 = 0, y2 = 0, index2 = 0;

	for (y2 = OFFS_Y; y2 < MIMAGE_Y+OFFS_Y; y2++) {  				//height
		//uint32_t location2 = y2 * DIMAGE_X * RGB_BYTES;

		for (x2 = OFFS_X; x2 < MIMAGE_X+OFFS_X; x2++) { 			//width
#if 1
			color_correction(&sp[index2], &dp[index2]);
			index2 += RGB_BYTES;
#elif 0
		color_correction(&sp[index2], &dp[location2 + (x2 * RGB_BYTES) ]);
		index2 += RGB_BYTES;
#else
		dp[location2 + (x2 * 3) ] = sp[index2++]; //R
		dp[location2 + (x2 * 3) + 1 ] = sp[index2++]; //G
		dp[location2 + (x2 * 3) + 2 ] = sp[index2++]; //B
#endif
		}
	}
}
