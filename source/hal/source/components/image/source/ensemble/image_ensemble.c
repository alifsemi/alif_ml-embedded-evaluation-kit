/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "RTE_Components.h"

#include "image_data.h"
#include "image_processing.h"
#include "Driver_CPI.h"
#include "Driver_PINMUX_AND_PINPAD.h"
#include "Driver_GPIO.h"
#include "base_def.h"
#include "delay.h"

extern uint8_t raw_image[CIMAGE_X*CIMAGE_Y*RGB_BYTES + 0x460];
extern uint8_t rgb_image[CIMAGE_X*CIMAGE_Y*RGB_BYTES];
extern uint8_t lcd_image[DIMAGE_Y][DIMAGE_X][RGB_BYTES];
extern ARM_DRIVER_GPIO Driver_GPIO1;

int image_init()
{
    DEBUG_PRINTF("image_init(IN)\n");
    int err = camera_init(raw_image);
    DEBUG_PRINTF("image_init(), camera_init: %d\n", err);
	if (err != 0) {
		while(1) {
			Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_LOW);
			sleep_or_wait_msec(300);
			Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_HIGH);
			sleep_or_wait_msec(300);
		}
	}
	DEBUG_PRINTF("Camera initialized... \n");
	Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_HIGH);

    return err;
}

int get_image_data(void *data)
{
    uint8_t *ml_image = (uint8_t *)data;
    extern uint32_t tprof1, tprof2, tprof3, tprof4, tprof5;

    camera_start(CAMERA_MODE_SNAPSHOT);
    SCB_InvalidateDCache_by_Addr(raw_image, sizeof raw_image);
    camera_wait(100);
    tprof1 = ARM_PMU_Get_CCNTR();
    // RGB conversion and frame resize
    bayer_to_RGB(raw_image+0x460, rgb_image);
    tprof1 = ARM_PMU_Get_CCNTR() - tprof1;
    // Cropping and scaling
    crop_and_interpolate(rgb_image, CIMAGE_X, CIMAGE_Y, raw_image, MIMAGE_X, MIMAGE_Y, RGB_BYTES * 8);
    tprof4 = ARM_PMU_Get_CCNTR();
    // Color correction for white balance
    white_balance(raw_image, ml_image);
    tprof4 = ARM_PMU_Get_CCNTR() - tprof4;
    return 0;
}
