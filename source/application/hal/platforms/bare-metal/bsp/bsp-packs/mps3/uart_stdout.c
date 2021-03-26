/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
#include "uart_stdout.h"

#include "device_mps3.h"

#include <stdio.h>

#define CNTLQ       0x11
#define CNTLS       0x13
#define DEL         0x7F
#define BACKSPACE   0x08
#define CR          0x0D
#define LF          0x0A
#define ESC         0x1B

void UartStdOutInit(void)
{
    /* NOTE: SystemCoreClock should have been set before initialising UART. */
    CMSDK_UART0->BAUDDIV = SystemCoreClock / 115200;   /* => (25 or 32 MHz) / (115200 bps). */
    CMSDK_UART0->CTRL    = ((1ul <<  0) |              /* TX enable. */
                            (1ul <<  1) );             /* RX enable. */
    return;
}

unsigned char UartPutc(unsigned char my_ch)
{
    while ((CMSDK_UART0->STATE & 1)); /* Wait if Transmit Holding register is full. */

    if (my_ch == '\n') {
        CMSDK_UART0->DATA  = '\r';
        while ((CMSDK_UART0->STATE & 1)); /* Wait if Transmit Holding register is full. */
    }

    CMSDK_UART0->DATA = my_ch; /* Write to transmit holding register. */
    return (my_ch);
}

unsigned char UartGetc(void)
{
    unsigned char my_ch;
    unsigned int  cnt;

    /* Wait if Receive Holding register is empty. */
    while (0 == (CMSDK_UART0->STATE & 2)) {
        cnt = MPS3_FPGAIO->CLK100HZ / 50;
        if (cnt & 0x8) {
            MPS3_FPGAIO->LED = 0x01 << (cnt & 0x7);
        }
        else {
            MPS3_FPGAIO->LED = 0x80 >> (cnt & 0x7);
        }
    }

    my_ch = CMSDK_UART0->DATA;

    /* Convert CR to LF. */
    if(my_ch == '\r') {
        my_ch = '\n';
    }

    return (my_ch);
}

bool GetLine(char *lp, unsigned int len)
{
    unsigned int cnt = 0;
    char c;

    do {
        c = UartGetc ();
        switch (c) {
            case CNTLQ:                       /* Ignore Control S/Q.            */
            case CNTLS:
                break;

            case BACKSPACE:
            case DEL:
                if (cnt == 0) {
                    break;
                }
                cnt--;                         /* Decrement count.               */
                lp--;                          /* Decrement line pointer.        */
                UartPutc (0x08);               /* Echo backspace.                */
                UartPutc (' ');
                UartPutc (0x08);
                fflush (stdout);
                break;

            case ESC:
            case 0:
                *lp = 0;                        /* ESC - stop editing line.       */
                return false;

            case CR:                            /* CR - done, stop editing line.  */
                *lp = c;
                lp++;                           /* Increment line pointer         */
                cnt++;                          /* and count.                     */
                c = LF;
            default:
                UartPutc (*lp = c);             /* Echo and store character.      */
                fflush (stdout);
                lp++;                           /* Increment line pointer         */
                    cnt++;                      /* and count.                     */
                break;
        }
    } while (cnt < len - 2  &&  c != LF);       /* Check limit and CR.            */
    *lp = 0;                                    /* Mark end of string.            */

    return true;
}

void UartEndSimulation(int code)
{
    UartPutc((char) 0x4);   /* End of simulation */
    UartPutc((char) code);  /* End of simulation */
    while(1);
}
