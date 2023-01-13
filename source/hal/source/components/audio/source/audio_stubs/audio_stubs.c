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
#include <string.h>

int audio_init(int sampling_rate, int wlen)
{
    (void) sampling_rate;
    (void) wlen;
    return 0;
}

int get_audio_data(void *data, int len)
{
    memset(data, 0, len);
    return 0;
}
