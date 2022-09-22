/* Copyright (c) 2022 ALIF SEMICONDUCTOR

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

/**************************************************************************//**
 * @file     mic_listener.c
 * @author   Teppo Järvelin
 * @email    @alifsemi.com
 * @version
 * @date     6-May-2022
 * @brief    Test Application for I2S.
 *           I2S2 is configured as master receiver SPH0645LM4H-1.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

/*System Includes */
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Project Includes */
#include <Driver_SAI.h>
#include <Driver_PINMUX_AND_PINPAD.h>

#include "mic_listener.h"

#define I2S_BUS_NUMBER 2   // We use I2S Controller 2.

ARM_DRIVER_SAI       *i2s_drv;

extern ARM_DRIVER_SAI ARM_Driver_SAI_(I2S_BUS_NUMBER);

voice_callback_t rx_callback;

/**
  \fn          void sai_callback(uint32_t event)
  \brief       Callback routine from the i2s driver
  \param[in]   event Event for which the callback has been called
*/
void i2s_callback(uint32_t event)
{
    if (event & ARM_SAI_EVENT_RECEIVE_COMPLETE) {
        rx_callback(event);
    } else if (event & ARM_SAI_EVENT_RX_OVERFLOW) {
        printf("*** i2s_callback with event: ARM_SAI_EVENT_RX_OVERFLOW ***\n");
    } else if (event & ARM_SAI_EVENT_FRAME_ERROR) {
        printf("*** i2s_callback with event: ARM_SAI_EVENT_FRAME_ERROR ***\n");
    } else {
        printf("*** i2s_callback with event: %d ***\n", event);
    }
}

/**
  \fn          void i2s_pinmux_config(void)
  \brief       Initialize the pinmux for I2S RX
  \return      status
*/
void i2s_pinmux_config(void)
{
    /* Configure I2S2 WS */
    int32_t status = PINMUX_Config(PORT_NUMBER_2, PIN_NUMBER_4, PINMUX_ALTERNATE_FUNCTION_2);
    printf("i2s_pinmux_config(1) = %d\n", status);
    //assert(status == ARM_DRIVER_OK);

    /* Configure I2S2 SCLK */
    status = PINMUX_Config(PORT_NUMBER_2, PIN_NUMBER_3, PINMUX_ALTERNATE_FUNCTION_3);
    printf("i2s_pinmux_config(2) = %d\n", status);
    //assert(status == ARM_DRIVER_OK);

    /* Configure I2S2 SDI */
    status = PINMUX_Config(PORT_NUMBER_2, PIN_NUMBER_1, PINMUX_ALTERNATE_FUNCTION_3);
    printf("i2s_pinmux_config(3) = %d\n", status);
    //assert(status == ARM_DRIVER_OK);
}

int32_t init_microphone(uint32_t sampling_rate, uint32_t data_bit_len)
{
    ARM_SAI_CAPABILITIES cap;
    int32_t status;

    /* Configure the I2S pins */
    i2s_pinmux_config();

    /* Use the I2S2 as Receiver */
    i2s_drv = &ARM_Driver_SAI_(I2S_BUS_NUMBER);

    /* Verify the I2S API version for compatibility*/
    ARM_DRIVER_VERSION version = i2s_drv->GetVersion();
    printf("I2S API version = %d\n", version.api);

    /* Verify if I2S protocol is supported */
    cap = i2s_drv->GetCapabilities();
    if (!cap.protocol_i2s) {
        printf("I2S is not supported\n");
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* Initializes I2S2 interface */
    status = i2s_drv->Initialize(i2s_callback);
    if (status != ARM_DRIVER_OK) {
        printf("I2S Initialize failed status = %d\n", status);
        return status;
    }

    /* Enable the power for I2S2 */
    status = i2s_drv->PowerControl(ARM_POWER_FULL);
    if (status != ARM_DRIVER_OK) {
        printf("I2S Power failed status = %d\n", status);
        return status;
    }

    /* configure I2S2 Receiver to Asynchronous Master */
    status = i2s_drv->Control(ARM_SAI_CONFIGURE_RX |
                              ARM_SAI_MODE_MASTER  |
                              ARM_SAI_ASYNCHRONOUS |
                              ARM_SAI_PROTOCOL_I2S |
                              ARM_SAI_DATA_SIZE(data_bit_len), data_bit_len*2, sampling_rate);
                                // once bug fixed in i2s driver we can use above style
                                //ARM_SAI_DATA_SIZE(data_bit_len), ARM_SAI_FRAME_LENGTH(data_bit_len*2), sampling_rate);

    if (status != ARM_DRIVER_OK) {
        printf("I2S Control status = %d\n", status);
        i2s_drv->PowerControl(ARM_POWER_OFF);
    }

    printf(" (OUT) I2S Control status = %d\n", status);
    return ARM_DRIVER_OK;
}

int32_t enable_microphone(voice_callback_t callback)
{
    /* enable Receiver */
    int32_t status = i2s_drv->Control(ARM_SAI_CONTROL_RX, 1, 0);
    if (status != ARM_DRIVER_OK) {
        printf("I2S enabled failed = %d\n", status);
        i2s_drv->PowerControl(ARM_POWER_OFF);
        rx_callback = NULL;
        return status;
    }

    rx_callback = callback;

    return ARM_DRIVER_OK;
}

int32_t disable_microphone()
{
    /* Stop the RX */
    int32_t status = i2s_drv->Control(ARM_SAI_CONTROL_RX, 0, 0);
    if (status != ARM_DRIVER_OK) {
        printf("I2S disable failed status = %d\n", status);
    }

    return status;
}

int32_t receive_voice_data(void *data, uint32_t data_len)
{
    return i2s_drv->Receive(data, data_len);
}
