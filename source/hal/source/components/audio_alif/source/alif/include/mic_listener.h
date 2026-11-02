/* Copyright (C) 2022, 2026 Alif Semiconductor - All Rights Reserved.
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

/**
 * Microphone interfaces for the *_ex functions. Only the ones compiled in
 * (USE_I2S_MICS, USE_PDM_MICS, USE_LPPDM_MICS) are usable; others return -1.
 */
typedef enum {
    MIC_TYPE_I2S,
    MIC_TYPE_PDM,
    MIC_TYPE_LPPDM,
} mic_type_t;

/** @brief init_microphone() for the given microphone interface */
int32_t init_microphone_ex(mic_type_t mic, uint32_t sampling_rate, uint32_t data_bit_len);

/** @brief enable_microphone() for the given microphone interface */
int32_t enable_microphone_ex(mic_type_t mic, voice_callback_t callback);

/** @brief disable_microphone() for the given microphone interface */
int32_t disable_microphone_ex(mic_type_t mic);

/** @brief receive_voice_data() for the given microphone interface */
int32_t receive_voice_data_ex(mic_type_t mic, void *data, uint32_t data_len);

/** @brief Microphone interface driven by the single-mic API */
mic_type_t get_default_microphone(void);

/*
 * Run-time tuning of a PDM or LPPDM stream, for microphone calibration. Call
 * between captures. The next init_microphone*() restores the defaults
 * (PDM_CH_PHASE, PDM_CH_GAIN and pdm_mode() in mic_listener.c).
 */

/**
 * @brief Set phase and gain of one channel
 *
 * @param ch_index 0 = primary, 1 = secondary channel
 * @param phase    0..0x3F
 * @param gain     0..0xFFF
 */
int32_t pdm_mic_set_channel(mic_type_t mic, uint32_t ch_index, uint32_t phase, uint32_t gain);

/**
 * @brief Select the decimation ratio, i.e. the PDM clock (sampling_rate * decimation)
 *
 * @return 0, or -1 if the PDM block has no such mode (16 kHz: 32, 48 or 64)
 */
int32_t pdm_mic_set_decimation(mic_type_t mic, uint32_t sampling_rate, uint32_t decimation);


#endif /* MIC_LISTENER_H_ */
