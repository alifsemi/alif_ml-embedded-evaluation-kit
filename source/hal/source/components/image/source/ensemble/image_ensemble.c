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

#define FAKE_CAMERA 0

int get_image_data(void *data)
{
    uint8_t *ml_image = (uint8_t *)data;
    extern uint32_t tprof1, tprof2, tprof3, tprof4, tprof5;

#if !FAKE_CAMERA
    camera_start(CAMERA_MODE_SNAPSHOT);
    // It's a huge buffer (941920 bytes) - actually doing it by address can take 0.5ms, while
    // a global clean+invalidate is 0.023ms. (Although there will be a reload cost
    // on stuff we lost).
    // Notably, just invalidate is faster at 0.015ms, but we'd have to be sure
    // there were no writeback cacheable areas.
    // From that Breakeven point for ranged invalidate time = global clean+invalidate would be 43Kbyte.
    // So maybe go to global if >128K, considering cost of refills?
    //SCB_InvalidateDCache_by_Addr(raw_image, sizeof raw_image);
    SCB_CleanInvalidateDCache();
    camera_wait(100);
#else
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
    		p[0]        = b; p[1]            = g;
    		p[CIMAGE_X] = g; p[CIMAGE_X + 1] = r;
    		p += 2;
    	}
    }
    roll = (roll + 1) % CIMAGE_Y;
#endif
    tprof1 = ARM_PMU_Get_CCNTR();
    // RGB conversion and frame resize
    bayer_to_RGB(raw_image, rgb_image);
    tprof1 = ARM_PMU_Get_CCNTR() - tprof1;
    // Cropping and scaling
    crop_and_interpolate(rgb_image, CIMAGE_X, CIMAGE_Y, raw_image, MIMAGE_X, MIMAGE_Y, RGB_BYTES * 8);
    tprof4 = ARM_PMU_Get_CCNTR();
    // Color correction for white balance
    white_balance(raw_image, ml_image);
    tprof4 = ARM_PMU_Get_CCNTR() - tprof4;
    return 0;
}
