#ifndef HAL_DATA_H
#define HAL_DATA_H
/**
 * This file is the top level abstraction for getting audio data
 **/

#include "audio_data.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


/**
 * @brief init audio
 *
 */
#define hal_audio_init()                audio_init()

#define hal_set_audio_callback(cb)          audio_set_callback(cb)
/**
 * @brief get audio data with Hal implementation.
 *
 * @param data  void* data input buffer for audio data. Data type depending on platform.
 * @param len   int len length of the input data buffer
 */
#define hal_get_audio_data(data, len)   get_audio_data(data, len)

#endif // HAL_DATA_H
