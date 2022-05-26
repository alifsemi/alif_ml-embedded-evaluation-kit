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
 * @file     Driver_PINMUX_AND_PINPAD.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    Driver for PINMUX and PINPAD.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/
#ifndef __DRIVER_PINMUX_AND_PINPAD_H__
#define __DRIVER_PINMUX_AND_PINPAD_H__

#include<stdint.h>

#define PORT_NUMBER_1                                       (0)
#define PORT_NUMBER_2                                       (1)
#define PORT_NUMBER_3                                       (2)
#define PORT_NUMBER_4                                       (3)

#define PIN_NUMBER_0                                        (0)
#define PIN_NUMBER_1                                        (1)
#define PIN_NUMBER_2                                        (2)
#define PIN_NUMBER_3                                        (3)
#define PIN_NUMBER_4                                        (4)
#define PIN_NUMBER_5                                        (5)
#define PIN_NUMBER_6                                        (6)
#define PIN_NUMBER_7                                        (7)
#define PIN_NUMBER_8                                        (8)
#define PIN_NUMBER_9                                        (9)
#define PIN_NUMBER_10                                       (10)
#define PIN_NUMBER_11                                       (11)
#define PIN_NUMBER_12                                       (12)
#define PIN_NUMBER_13                                       (13)
#define PIN_NUMBER_14                                       (14)
#define PIN_NUMBER_15                                       (15)
#define PIN_NUMBER_16                                       (16)
#define PIN_NUMBER_17                                       (17)
#define PIN_NUMBER_18                                       (18)
#define PIN_NUMBER_19                                       (19)
#define PIN_NUMBER_20                                       (20)
#define PIN_NUMBER_21                                       (21)
#define PIN_NUMBER_22                                       (22)
#define PIN_NUMBER_23                                       (23)
#define PIN_NUMBER_24                                       (24)
#define PIN_NUMBER_25                                       (25)
#define PIN_NUMBER_26                                       (26)
#define PIN_NUMBER_27                                       (27)
#define PIN_NUMBER_28                                       (28)
#define PIN_NUMBER_29                                       (29)
#define PIN_NUMBER_30                                       (30)
#define PIN_NUMBER_31                                       (31)

#define PINMUX_ALTERNATE_FUNCTION_0                         (0)
#define PINMUX_ALTERNATE_FUNCTION_1                         (1)
#define PINMUX_ALTERNATE_FUNCTION_2                         (2)
#define PINMUX_ALTERNATE_FUNCTION_3                         (3)
#define PINMUX_ALTERNATE_FUNCTION_4                         (4)
#define PINMUX_ALTERNATE_FUNCTION_5                         (5)
#define PINMUX_ALTERNATE_FUNCTION_6                         (6)
#define PINMUX_ALTERNATE_FUNCTION_7                         (7)

#define PAD_FUNCTION_READ_ENABLE                            (0x01)
#define PAD_FUNCTION_SCHMITT_TRIGGER_ENABLE                 (0x02)
#define PAD_FUNCTION_SLEW_RATE_FAST                         (0x04)
#define PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_HIGH_Z       (0x00)
#define PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_PULL_UP      (0x08)
#define PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_PULL_DOWN    (0x10)
#define PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_BUS_REPEATER (0x18)
#define PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_02_MILI_AMPS     (0x00)
#define PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_04_MILI_AMPS     (0x20)
#define PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_08_MILI_AMPS     (0x40)
#define PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_12_MILI_AMPS     (0x60)
#define PAD_FUNCTION_DRIVER_OPEN_DRAIN                      (0x80)
#define PAD_FUNCTION_DISABLE                                (0x00)

int32_t PINMUX_Config (uint8_t port, uint8_t pin_no, uint8_t AF_number);
int32_t PINMUX_read   (uint8_t port, uint8_t pin_no, uint32_t *AF_number);
int32_t PINPAD_Config (uint8_t port, uint8_t pin_no, uint8_t function);
int32_t PINPAD_read   (uint8_t port, uint8_t pin_no, uint32_t *function);

#endif /* __DRIVER_PINMUX_AND_PINPAD_H__ */
