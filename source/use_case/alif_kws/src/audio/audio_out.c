
/* Copyright (c) 2024 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <stdatomic.h>

#include "Driver_SAI.h"
#include "audio_out.h"
#include "wm8904_driver.h"

#include "RTE_Components.h"
#include CMSIS_device_header

/* I2S_Driver */
#define I2S_DAC 2                    /* DAC I2S Controller 2 */

/* Callback events */
#define DAC_SEND_COMPLETE_EVENT    (1U << 0)

static uint32_t wlen = 16;

/* I2S_DAC Driver */
extern ARM_DRIVER_SAI ARM_Driver_SAI_(I2S_DAC);
/* Driver Instance */
static ARM_DRIVER_SAI *i2s_dac = &ARM_Driver_SAI_(I2S_DAC);

static atomic_bool running = false;
static atomic_bool data_ready = false;
static const void *next_data = NULL;
static uint32_t next_samples = 0;

static void DAC_Callback(uint32_t event)
{
    if (event != ARM_SAI_EVENT_SEND_COMPLETE) {
        printf("DAC_Callback: %" PRIu32 "\n", event);
    }
    if(event & ARM_SAI_EVENT_SEND_COMPLETE) {
        running = false;

        if (!data_ready) {
            printf("audio out underflow\n");
            return;
        }

        int32_t ret = i2s_dac->Send(next_data, next_samples);
        if(ret != ARM_DRIVER_OK) {
            printf("I2S send failed: %" PRId32 "\n", ret);
            return;
        }

        data_ready = false;
        running = true;
    }
}

int32_t audio_out_init(uint32_t sample_rate)
{
    ARM_DRIVER_VERSION   version;
    ARM_SAI_CAPABILITIES cap;
    int32_t              status;

    /* Verify the I2S API version for compatibility */
    version = i2s_dac->GetVersion();
    printf("I2S API version = %" PRIu16 "\n", version.api);

    /* Verify if I2S protocol is supported */
    cap = i2s_dac->GetCapabilities();
    if(!cap.protocol_i2s) {
        printf("I2S is not supported\n");
        return -1;
    }

    /* Initializes I2S0 interface */
    status = i2s_dac->Initialize(DAC_Callback);
    if(status) {
        printf("DAC Init failed status = %" PRId32 "\n", status);
        return -1;
    }

    /* Enable the power for I2S0 */
    status = i2s_dac->PowerControl(ARM_POWER_FULL);
    if(status) {
        printf("DAC Power Failed status = %" PRId32 "\n", status);
        return -1;
    }

    /* configure I2S0 Transmitter to Asynchronous Master */
    status = i2s_dac->Control(ARM_SAI_CONFIGURE_TX |
                                ARM_SAI_MODE_MASTER  |
                                ARM_SAI_ASYNCHRONOUS |
                                ARM_SAI_PROTOCOL_I2S |
                                ARM_SAI_MONO_MODE |
                                ARM_SAI_DATA_SIZE(wlen), wlen*2, sample_rate);
    if(status) {
        printf("DAC Control status = %" PRId32 "\n", status);
        return -1;
    }

    /* enable Transmitter */
    status = i2s_dac->Control(ARM_SAI_CONTROL_TX, 1, 0);
    if(status) {
        printf("DAC TX status = %" PRId32 "\n", status);
        return -1;
    }

    status = WM8904_Codec_Init();

    return status;
}

int32_t audio_out_transmit(const void *data, uint32_t num)
{
    if (running && data_ready) {
        printf("audio out overflow\n");
        return -1;
    }

    if (running) {
        next_data = data;
        next_samples = num;
        data_ready = true;
        return ARM_DRIVER_OK;
    }

    int32_t ret = i2s_dac->Send(data, num);
    if(ret != ARM_DRIVER_OK) {
        printf("I2S send failed: %" PRId32 "\n", ret);
        return ret;
    }

    running = true;

    return ret;
}

void audio_out_set_volume(uint8_t volume)
{
    WM8904_Set_Volume(volume);
}