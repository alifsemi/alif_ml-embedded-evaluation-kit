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
 *           I2S2 is configured as master receiver SPH0645LM4H-1
 * @bug      None.
 * @Note     None
 ******************************************************************************/
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


#endif /* MIC_LISTENER_H_ */
