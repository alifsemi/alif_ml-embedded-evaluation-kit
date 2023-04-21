/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <inttypes.h>

int image_init(void)
{
    return 0;
}

const uint8_t *get_image_data(int width, int height)
{
    (void) width;
    (void) height;
    return 0;
}

float get_image_gain(void)
{
    return 1.0f;
}
