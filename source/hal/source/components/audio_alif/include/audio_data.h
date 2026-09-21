/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Callback for receiving ack when data is ready
 */
typedef void (*audio_callback_t)(uint32_t data);

/* Microphone selector for dual-mic builds. Matches mic_type_t. */
typedef enum {
    AUDIO_MIC_I2S = 0,
    AUDIO_MIC_PDM = 1,
} audio_mic_t;

int audio_init(int sampling_rate);

int audio_uninit();

/* Call is asynchronous - use wait call, audio_get_samples_received or callback to monitor progress. Data is not valid until preprocessing is run */
int get_audio_data(int16_t *data, int len);

void audio_set_callback(audio_callback_t cb);

/* Returns number of samples that have been received in the current transfer */
int get_audio_samples_received(void);

/* Returns error indication - 0 for success */
int wait_for_audio(void);

/* Separate foreground preprocessing stage - as it will likely be slow, initiate
 * the next asynchronous get into a separate buffer before running on the previous one. */
void audio_preprocessing(int16_t *data, int len);

/* Set fixed microphone gain */
void set_audio_gain(float gain_db);

/*
 * Per-mic variants. When both USE_I2S_MICS and USE_PDM_MICS are compiled in,
 * these let callers drive each microphone independently in parallel. Calls
 * against a microphone that is not compiled in return a non-zero error.
 */
int audio_init_ex(audio_mic_t mic, int sampling_rate);
int audio_uninit_ex(audio_mic_t mic);
int get_audio_data_ex(audio_mic_t mic, int16_t *data, int len);
void audio_set_callback_ex(audio_mic_t mic, audio_callback_t cb);
int get_audio_samples_received_ex(audio_mic_t mic);
int wait_for_audio_ex(audio_mic_t mic);
void audio_preprocessing_ex(audio_mic_t mic, int16_t *data, int len);
void set_audio_gain_ex(audio_mic_t mic, float gain_db);

#endif // AUDIO_DATA_H
