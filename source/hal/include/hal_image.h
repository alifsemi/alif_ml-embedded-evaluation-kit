
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
 *
 * @param data  void* data input buffer for image data. Data type depending on platform.
 */
#define hal_get_image_data(data)   get_image_data(data)

#endif // HAL_IMAGE_H
