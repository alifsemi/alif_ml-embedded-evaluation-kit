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


#endif /* MIC_LISTENER_H_ */
