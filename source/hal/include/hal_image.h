/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef HAL_IMAGE_H
#define HAL_IMAGE_H
/**
 * This file is the top level abstraction for getting image data
 **/

#include "image_data.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief init audio
 *
 */
#define hal_image_init()                image_init()

/**
 * @brief get image data with Hal implementation.
 * @return pointer to RGB image data
 * */
#define hal_get_image_data(w, h)   get_image_data(w, h)

#endif // HAL_IMAGE_H
