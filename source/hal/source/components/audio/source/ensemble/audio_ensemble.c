/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
