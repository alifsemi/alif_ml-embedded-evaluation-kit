/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

    camera_start(CAMERA_MODE_SNAPSHOT);
    camera_wait(100);
    // RGB conversion and frame resize
    bayer_to_RGB(raw_image+0x460, rgb_image);
    // Cropping and scaling
    crop_and_interpolate(rgb_image, CIMAGE_X, CIMAGE_Y, raw_image, MIMAGE_X, MIMAGE_Y, RGB_BYTES * 8);
    // Color correction for white balance
    white_balance(raw_image, ml_image);
    return 0;
}
