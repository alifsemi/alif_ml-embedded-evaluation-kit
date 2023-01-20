/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * Copyright (c) 2019-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Uncomment this to disable traces to UART
//#define DISABLE_UART_TRACE

#include "Driver_USART.h"

#if !defined(DISABLE_UART_TRACE)
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdatomic.h>
#include "Driver_PINMUX_AND_PINPAD.h"
#include <RTE_Components.h>
#include CMSIS_device_header


#define CNTLQ     0x11
#define CNTLS     0x13
#define DEL       0x7F
#define BACKSPACE 0x08
#define CR        0x0D
#define LF        0x0A
#define ESC       0x1B

#define UNUSED(x) (void)(x)

/* Select used UART.
 *  Supported UARTs:
 *      UART2 (Pin P3_17)
 *      UART4 (Pin P3_2)
 */

/* UART Driver */
extern ARM_DRIVER_USART ARM_Driver_USART_(CONSOLE_UART);

/* UART Driver instance */
static ARM_DRIVER_USART *USARTdrv = &ARM_Driver_USART_(CONSOLE_UART);

static atomic_uint_fast32_t uart_event;
static bool initialized = false;
const char * tr_prefix = NULL;
uint16_t prefix_len;
#define MAX_TRACE_LEN 256

static int hardware_init(void)
{
    int32_t ret;

#if CONSOLE_UART == 2
    /* PINMUX UART2_B */

    /* Configure GPIO Pin : P3_16 as UART2_RX_B */
    ret = PINMUX_Config (PORT_NUMBER_3, PIN_NUMBER_16, PINMUX_ALTERNATE_FUNCTION_2);
    if(ret != ARM_DRIVER_OK)
    {
        return -1;
    }

    /* Configure GPIO Pin : P3_17 as UART2_TX_B */
    ret = PINMUX_Config (PORT_NUMBER_3, PIN_NUMBER_17, PINMUX_ALTERNATE_FUNCTION_2);
    if(ret != ARM_DRIVER_OK)
    {
        return -1;
    }
#elif CONSOLE_UART == 4
    /* PINMUX UART4_B */

    /* Configure GPIO Pin : P3_1 as UART4_RX_B */
    ret = PINMUX_Config (PORT_NUMBER_3, PIN_NUMBER_1, PINMUX_ALTERNATE_FUNCTION_1);
    if(ret != ARM_DRIVER_OK)
    {
        return -1;
    }

    /* Configure GPIO Pin : P3_2 as UART4_TX_B */
    ret = PINMUX_Config (PORT_NUMBER_3, PIN_NUMBER_2, PINMUX_ALTERNATE_FUNCTION_1);
    if(ret != ARM_DRIVER_OK)
    {
        return -1;
    }
#else
    #error "Unsupported UART!"
#endif

    return 0;
}

void myUART_callback(uint32_t event)
{
    uart_event = event;
}

int tracelib_init(const char * prefix)
{
    int32_t ret    = 0;

    tr_prefix = prefix;
    if (tr_prefix) {
        prefix_len = strlen(tr_prefix);
    } else {
        prefix_len = 0;
    }


    /* Initialize UART hardware pins using PinMux Driver. */
    ret = hardware_init();
    if(ret != 0)
    {
        return ret;
    }

    /* Initialize UART driver */
    ret = USARTdrv->Initialize(myUART_callback);
    if(ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    /* Power up UART peripheral */
    ret = USARTdrv->PowerControl(ARM_POWER_FULL);
    if(ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    /* Configure UART to 115200 Bits/sec */
    ret =  USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                             ARM_USART_DATA_BITS_8       |
                             ARM_USART_PARITY_NONE       |
                             ARM_USART_STOP_BITS_1       |
                             ARM_USART_FLOW_CONTROL_NONE, 115200);
    if(ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    /* Transmitter line */
    ret =  USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
    if(ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    /* Receiver line */
    ret =  USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
    if(ret != ARM_DRIVER_OK)
    {
        return ret;
    }

    initialized = true;
    return ret;
}

int send_str(const char* str, uint32_t len)
{
    int ret = 0;
    if (initialized)
    {
        uart_event = 0;
        int32_t ret = USARTdrv->Send(str, len);
        if(ret != ARM_DRIVER_OK)
        {
            return ret;
        }

        while (!uart_event) {
            __WFE();
        }
    }
    return ret;
}

void tracef(const char * format, ...)
{
    if (initialized)
    {
        static char buffer[MAX_TRACE_LEN];

        va_list args;
        va_start(args, format);
        if (prefix_len) {
            memcpy(buffer, tr_prefix, prefix_len);
        }
        vsnprintf(buffer + prefix_len, sizeof(buffer) - prefix_len, format, args);
        send_str(buffer, strlen(buffer));
        va_end(args);
    }
}

unsigned char UartPutc(unsigned char ch)
{
    if (ch == '\n') {
        (void)UartPutc('\r');
    }
    char c = ch;
    send_str(&c, 1);
    return ch;
}

unsigned char UartGetc(void)
{
    unsigned char c;
    uart_event = 0;
    if (initialized) {
        USARTdrv->Receive(&c, 1);
    }
    /* We'll just loop for ever if not initialized or anything goes wrong */
    while (!uart_event) {
        __WFE();
    }

    if (c == '\r') {
        c = '\n';
    }
    return c;
}



unsigned int GetLine(char *lp, unsigned int len)
{
    unsigned int cnt = 0;
    char c;

    do {
        c = UartGetc();
        switch (c) {
        case CNTLQ: /* ignore Control S/Q             */
        case CNTLS:
            break;
        case BACKSPACE:
        case DEL:
            if (cnt == 0) {
                break;
            }
            cnt--;          /* decrement count                */
            lp--;           /* and line pointer               */
            UartPutc(0x08); /* echo backspace                 */
            UartPutc(' ');
            UartPutc(0x08);
            fflush(stdout);
            break;
        case ESC:
        case 0:
            *lp = 0; /* ESC - stop editing line        */
            return 0;
        case CR: /* CR - done, stop editing line   */
            *lp = c;
            lp++;  /* increment line pointer         */
            cnt++; /* and count                      */
            c = LF;
            UartPutc(*lp = c); /* echo and store character       */
            fflush(stdout);
            lp++;  /* increment line pointer         */
            cnt++; /* and count                      */
            break;
        default:
            UartPutc(*lp = c); /* echo and store character       */
            fflush(stdout);
            lp++;  /* increment line pointer         */
            cnt++; /* and count                      */
            break;
        }
    } while (cnt < len - 2 && c != LF); /* check limit and CR             */
    *lp = 0;                            /* mark end of string             */
    return 1;
}

#else

int tracelib_init(const char * prefix)
{
    return 0;
}

int send_str(const char* str, uint32_t len)
{
    return 0;
}

void tracef(const char * format, ...)
{
}

unsigned int GetLine(char *user_input, unsigned int size)
{
    return 0;
}

#endif // DISABLE_UART_TRACE

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
