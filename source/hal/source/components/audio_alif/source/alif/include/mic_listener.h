/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef MIC_LISTENER_H_
#define MIC_LISTENER_H_

#include <stdint.h>

/**
 * Callback for receiving ack when data is ready
 */
typedef void (*voice_callback_t)(uint32_t data);

/* Microphone selector for dual-mic builds. */
typedef enum {
    MIC_TYPE_I2S = 0,
    MIC_TYPE_PDM = 1,
} mic_type_t;

/**
 * @brief Initialize microphone with wanted values
 *
 * @param sampling_rate Audio frequency for sampling
 * @param data_bit_len xx-bit data length
 */
int32_t init_microphone(uint32_t sampling_rate, uint32_t data_bit_len);

/**
 * @brief Enables microphone
 *
 * @param callback Callback which is called when data is ready
 *
 * @note receive_voice_data must be called to get data from microphone
 */
int32_t enable_microphone(voice_callback_t callback);

/**
 * @brief Disables microphone
 */
int32_t disable_microphone();

/**
 * @brief Get data from microphone. Once data is ready, callback given in start_mic_listening is called.
 *        Must be called again after data is read.
 *
 * @param data Buffer for voice data
 * @param length of the data buffer
 */
int32_t receive_voice_data(void *data, uint32_t data_len);

/*
 * Per-mic variants used when both USE_I2S_MICS and USE_PDM_MICS are defined,
 * so that the two microphones can be driven independently in parallel.
 * When only one mic type is compiled in, calls on the disabled mic return an
 * error and calls on the enabled mic forward to the single-mic implementation.
 */
int32_t init_microphone_ex(mic_type_t mic, uint32_t sampling_rate, uint32_t data_bit_len);
int32_t enable_microphone_ex(mic_type_t mic, voice_callback_t callback);
int32_t disable_microphone_ex(mic_type_t mic);
int32_t receive_voice_data_ex(mic_type_t mic, void *data, uint32_t data_len);

#endif /* MIC_LISTENER_H_ */
