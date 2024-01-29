/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
 
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
#define hal_audio_init(rate)          audio_init(rate)

#define hal_set_audio_callback(cb)          audio_set_callback(cb)
/**
 * @brief get audio data with Hal implementation.
 *
 * @param data  int16_t * data input buffer for audio data.
 * @param len   int number of samples
 */
#define hal_get_audio_data(data, len)   get_audio_data(data, len)

#define hal_wait_for_audio()            wait_for_audio()

#define hal_get_audio_samples_received() get_audio_samples_received()

#define hal_audio_preprocessing(data, len) audio_preprocessing(data, len)

#define hal_set_audio_gain(gain_db) set_audio_gain(gain_db)

#endif // HAL_DATA_H
