/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

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
    _port port_4[PORT4_MAX_PIN_NUMBER];         /**< GPIO PORT-1 PIN PAD register...>*/
} Pad_RegInfo;

#endif /* __PINMUX_AND_PINPAD_INTERNAL_H__ */
