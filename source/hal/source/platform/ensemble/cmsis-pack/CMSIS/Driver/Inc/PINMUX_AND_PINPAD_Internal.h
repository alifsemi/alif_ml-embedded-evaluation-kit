/* Copyright (c) 2020 - 2021 ALIF SEMICONDUCTOR

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
 * @file     PINMUX_AND_PINPAD_Internal.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    Driver for PINMUX and PINPAD.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#ifndef __PINMUX_AND_PINPAD_INTERNAL_H__
#define __PINMUX_AND_PINPAD_INTERNAL_H__

#include "RTE_Device.h"
#include "RTE_Components.h"
#include "Driver_PINMUX_AND_PINPAD.h"
#include CMSIS_device_header
#include "Driver_Common.h"

#define PORT1_MAX_PIN_NUMBER                    (32)
#define PORT2_MAX_PIN_NUMBER                    (32)
#define PORT3_MAX_PIN_NUMBER                    (24)
#define PORT4_MAX_PIN_NUMBER                    (8)

typedef struct _PINMUX_RegInfo {
    const volatile uint32_t Reserved0[4];       /**< Reserved register...>*/
    volatile uint32_t port_1[4];                /**< GPIO PORT-1 PINMUX register...>*/
    volatile uint32_t port_2[4];                /**< GPIO PORT-2 PINMUX register...>*/
    volatile uint32_t port_3[3];                /**< GPIO PORT-3 PINMUX register...>*/
    const volatile uint32_t Reserved1;          /**< Reserved register...>*/
} PINMUX_RegInfo;

typedef struct __port{
    volatile uint8_t pad;                       /**< GPIO PORT PIN PAD register...>*/
    const volatile uint8_t Reserved0[3];        /**< Reserved register...>*/
} _port;

typedef struct _Pad_RegInfo {
    _port port_1[PORT1_MAX_PIN_NUMBER];         /**< GPIO PORT-1 PIN PAD register...>*/
    _port port_2[PORT2_MAX_PIN_NUMBER];         /**< GPIO PORT-2 PIN PAD register...>*/
    _port port_3[PORT3_MAX_PIN_NUMBER];         /**< GPIO PORT-3 PIN PAD register...>*/
    const _port Reserved1[8];                   /**< Reserved register...>*/
    _port port_4[PORT4_MAX_PIN_NUMBER];         /**< GPIO PORT-1 PIN PAD register...>*/
} Pad_RegInfo;

#endif /* __PINMUX_AND_PINPAD_INTERNAL_H__ */
