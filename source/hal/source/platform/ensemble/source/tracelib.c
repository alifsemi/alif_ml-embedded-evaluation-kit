/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
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

#include <stdio.h>
#include <stdbool.h>

#include "uart_tracelib.h"
#include "itm_tracelib.h"
#include "stm_tracelib.h"

#include <stdarg.h>
#include <string.h>
#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

// CMake passes in USE_UART, USE_ITM, USE_STM and USE_SEMIHOSTING
// Any or all can be set

#define CNTLQ     0x11
#define CNTLS     0x13
#define DEL       0x7F
#define BACKSPACE 0x08
#define CR        0x0D
#define LF        0x0A
#define ESC       0x1B

#define UNUSED(x) (void)(x)

static bool initialized = false;
const char * tr_prefix = NULL;
uint16_t prefix_len;
#define MAX_TRACE_LEN 256

#ifdef M55_HP
#define STM_CHANNEL 0
#define ITM_CHANNEL 0
#else
#define STM_CHANNEL 1
#define ITM_CHANNEL 1
#endif

int tracelib_init(const char * prefix)
{
    int32_t ret    = 0;

    tr_prefix = prefix;
    if (tr_prefix) {
        prefix_len = strlen(tr_prefix);
    } else {
        prefix_len = 0;
    }

#ifdef USE_STM
    stm_init(STM_CHANNEL);
#endif
#ifdef USE_ITM
    itm_init(ITM_CHANNEL);
#endif

#ifdef USE_UART
    ret = uart_init();
    if(ret != 0)
    {
        return ret;
    }
#endif

    initialized = true;
    return ret;
}

int send_str(const char* str, uint32_t len)
{
    int ret = 0;
#ifdef USE_STM
    stm_send(STM_CHANNEL, str, len);
#endif
#ifdef USE_ITM
    itm_send(ITM_CHANNEL, str, len);
#endif
#ifdef USE_UART
    ret = uart_send(str, len);
    if(ret != 0)
    {
        return ret;
    }
#endif
#ifdef USE_SEMIHOSTING
    ret = fwrite(str, 1, len, stdout) == len ? 0 : -1;
#endif
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

#if defined USE_UART || defined USE_SEMIHOSTING
static char UartPutc(char ch)
{
    if (ch == '\n') {
        (void)UartPutc('\r');
    }
    char c = ch;
    send_str(&c, 1);
    return ch;
}

static char UartGetc(void)
{
    char c;
#ifdef USE_UART
    c = uart_getchar();
#elif defined USE_SEMIHOSTING
    c = (char) getchar();
#endif
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
#else // USE_UART || USE_SEMIHOSTING
unsigned int GetLine(char *user_input, unsigned int size)
{
    (void) user_input;
    (void) size;

    return 0;
}
#endif // USE_UART || USE_SEMIHOSTING

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
