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
#define hal_audio_alif_init(rate)          audio_init(rate)

#define hal_audio_alif_uninit()            audio_uninit()

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

#define hal_audio_alif_preprocessing(data, len) audio_preprocessing(data, len)

#define hal_set_audio_gain(gain_db) set_audio_gain(gain_db)

/*
 * Per-mic HAL wrappers. When both USE_I2S_MICS and USE_PDM_MICS are compiled
 * into audio_alif, each microphone can be driven independently by supplying
 * HAL_AUDIO_MIC_I2S or HAL_AUDIO_MIC_PDM.
 */
#define HAL_AUDIO_MIC_I2S                       AUDIO_MIC_I2S
#define HAL_AUDIO_MIC_PDM                       AUDIO_MIC_PDM

#define hal_audio_alif_init_ex(mic, rate)               audio_init_ex((mic), (rate))
#define hal_audio_alif_uninit_ex(mic)                   audio_uninit_ex(mic)
#define hal_set_audio_callback_ex(mic, cb)              audio_set_callback_ex((mic), (cb))
#define hal_get_audio_data_ex(mic, data, len)           get_audio_data_ex((mic), (data), (len))
#define hal_wait_for_audio_ex(mic)                      wait_for_audio_ex(mic)
#define hal_get_audio_samples_received_ex(mic)          get_audio_samples_received_ex(mic)
#define hal_audio_alif_preprocessing_ex(mic, data, len) audio_preprocessing_ex((mic), (data), (len))
#define hal_set_audio_gain_ex(mic, gain_db)             set_audio_gain_ex((mic), (gain_db))

#endif // HAL_DATA_H
