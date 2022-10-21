/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
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

int audio_init();

int get_audio_data(void *data, int len);

void audio_set_callback(audio_callback_t cb);

#endif // AUDIO_DATA_H
