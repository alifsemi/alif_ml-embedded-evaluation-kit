/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "audio_data.h"
#include "mic_listener.h"

const uint32_t wlen = 16;
const uint32_t sampling_rate = 16000;

volatile bool audio_received;
audio_callback_t audio_callback =  NULL;

void voice_data_cb(uint32_t data)
{
    audio_received = true;
    if (audio_callback) {
        audio_callback(data);
    }
}

void audio_set_callback(audio_callback_t callback)
{
    audio_callback = callback;
}

int audio_init()
{
    int32_t err = init_microphone(sampling_rate, wlen);

    if (err == 0) {
        // Enable microphone
        err = enable_microphone(voice_data_cb);
    }
    audio_received = false;

    return err;
}

int get_audio_data(void *data, int len)
{
    int status = receive_voice_data(data, len);
    audio_received = false;

    if (audio_callback == NULL && status == 0) {
        while (audio_received == false) {
            // get data synchronously so loop here. Hope that compiler won't optimize this away as we use volatile variable
        }
    }

    return status;
}
