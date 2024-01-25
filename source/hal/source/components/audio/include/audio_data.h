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

int audio_init(int sampling_rate);

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

#endif // AUDIO_DATA_H
