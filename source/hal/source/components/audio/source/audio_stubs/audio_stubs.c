/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "audio_data.h"
#include <string.h>

int audio_init(int sampling_rate)
{
    (void) sampling_rate;
    return -1;
}

int get_audio_data(int16_t *data, int len)
{
    return -1;
}

void audio_set_callback(audio_callback_t cb)
{
    (void) cb;
}

int get_audio_samples_received(void)
{
    return 0;
}

int wait_for_audio(void)
{
    return -1;
}

void audio_preprocessing(int16_t *data, int len)
{
    (void) data;
    (void) len;
}

void set_audio_gain(float gain_db)
{
    (void) gain_db;
}
