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
#include "peripheral_memmap.h"      /* peripheral memory map definitions */

#include <stdio.h>
#include <stdint.h>

#define CNTLQ       0x11
#define CNTLS       0x13
#define DEL         0x7F
#define BACKSPACE   0x08
#define CR          0x0D
#define LF          0x0A
#define ESC         0x1B

#define UARTBASE    (PL011_UART0_BASE)

/*****************************************************************************/
/*  UART Control Register Locations                                          */
/*****************************************************************************/
#define UART0_DR   *((volatile unsigned *) UARTBASE)
#define UART0_RSR  *((volatile unsigned *)(UARTBASE + 0x04))
#define UART0_ECR  *((volatile unsigned *)(UARTBASE + 0x04))
#define UART0_LCRH *((volatile unsigned *)(UARTBASE + 0x2C))
#define UART0_LCRM *((volatile unsigned *)(UARTBASE + 0x28))
#define UART0_LCRL *((volatile unsigned *)(UARTBASE + 0x24))
#define UART0_CR   *((volatile unsigned *)(UARTBASE + 0x30))
#define UART0_FR   *((volatile unsigned *)(UARTBASE + 0x18))
#define UART0_IIR  *((volatile unsigned *)(UARTBASE + 0x1C))
#define UART0_ICR  *((volatile unsigned *)(UARTBASE + 0x44))

/*****************************************************************************/
/* Received Status Register - RSR                                            */
/*****************************************************************************/
#define RSR_OVERRUN_ERROR   0x08
#define RSR_BREAK_ERROR     0x04
#define RSR_PARITY_ERROR    0x02
#define RSR_FRAMING_ERROR   0x01

/*****************************************************************************/
/* Line Control High Byte Register - LCRH                                    */
/*****************************************************************************/
#define LCRH_WORD_LENGTH_8  0x60
#define LCRH_WORD_LENGTH_7  0x40
#define LCRH_WORD_LENGTH_6  0x20
#define LCRH_WORD_LENGTH_5  0x00
#define LCRH_FIFO_ENABLED   0x10
#define LCRH_2_STOP_BITS    0x08
#define LCRH_EVEN_PARITY    0x04
#define LCRH_PARITY_ENABLE  0x02
#define LCRH_SEND_BREAK     0x01

/*****************************************************************************/
/* Line Control Medium Byte Register - LCRM                                  */
/* This register specifies the high byte of the Baud rate divisor            */
/*****************************************************************************/
#define LCRM_BAUD_460800  0x00
#define LCRM_BAUD_230400  0x00
#define LCRM_BAUD_115200  0x00
#define LCRM_BAUD_76800   0x00
#define LCRM_BAUD_57600   0x00
#define LCRM_BAUD_38400   0x00
#define LCRM_BAUD_19200   0x00
#define LCRM_BAUD_14400   0x00
#define LCRM_BAUD_9600    0x00
#define LCRM_BAUD_2400    0x01
#define LCRM_BAUD_1200    0x02

/*****************************************************************************/
/* Line Control Low Byte Register - LCRL                                     */
/* This register specifies the low byte of the Baud rate divisor             */
/*****************************************************************************/
#define LCRL_BAUD_460800  0x01
#define LCRL_BAUD_230400  0x03
#define LCRL_BAUD_115200  0x07
#define LCRL_BAUD_76800   0x0B
#define LCRL_BAUD_57600   0x0F
#define LCRL_BAUD_38400   0xC
#define LCRL_BAUD_19200   0x2F
#define LCRL_BAUD_14400   0x3F
#define LCRL_BAUD_9600    0x5F
#define LCRL_BAUD_2400    0x7F
#define LCRL_BAUD_1200    0xFF

/*****************************************************************************/
/* Control Register - CR                                                     */
/*****************************************************************************/
#define CR_LOOP_BACK_EN   0x80
#define CR_TIMEOUT_INT_EN 0x40
#define CR_TX_INT_ENABLE  0x100
#define CR_RX_INT_ENABLE  0x200
#define CR_MODSTAT_INT_EN 0x08
#define CR_UART_ENABLE    0x01

/*****************************************************************************/
/* Flag Register - FR                                                        */
/*****************************************************************************/
#define FR_TX_FIFO_EMPTY  0x80
#define FR_RX_FIFO_FULL   0x40
#define FR_TX_FIFO_FULL   0x20
#define FR_RX_FIFO_EMPTY  0x10
#define FR_BUSY           0x08
#define FR_CARRIER_DETECT 0x04
#define FR_SET_READY      0x02
#define FR_CLEAR_TO_SEND  0x01

/*****************************************************************************/
/* Interrupt Identification Register - IIR                                   */
/*****************************************************************************/
#define IIR_RX_TIME_OUT   0x08
#define IIR_TX            0x04
#define IIR_RX            0x02
#define IIR_MODEM         0x01

void UartStdOutInit(void)
{
    /* Disable the serial port while setting the baud rate and word length. */
    UART0_CR = 0;

    /* Clear the receive status register. */
    UART0_ECR = 0;

    /* Set the correct baud rate and word length. */
    UART0_LCRL = LCRL_BAUD_115200;
    UART0_LCRM = LCRM_BAUD_115200;
    UART0_LCRH = LCRH_WORD_LENGTH_8;

    /* Explicitly disable FIFO's for char mode. */
    UART0_LCRH &= ~LCRH_FIFO_ENABLED;

    /* Enable UART0 (and RX/TX) without interrupts. */
    UART0_CR = CR_UART_ENABLE | CR_TX_INT_ENABLE | CR_RX_INT_ENABLE;
}

unsigned char UartPutc(unsigned char ch)
{
    if (ch == '\n') {
        (void) UartPutc('\r');
    }
    while (UART0_FR & FR_TX_FIFO_FULL)
        ;
    UART0_DR = ch;

    return ch;
}

unsigned char UartGetc(void)
{
    unsigned char c;
    while (UART0_FR & FR_RX_FIFO_EMPTY)
        ;
    c = UART0_DR;
    if (c == '\r') {
        c = '\n';
    }

    return c;
}

bool GetLine (char *lp, unsigned int len)
{
    unsigned int cnt = 0;
    char c;

    do {
        c = UartGetc();
        switch (c) {
            case CNTLQ:                       /* ignore Control S/Q.            */
            case CNTLS:
                break;
            case BACKSPACE:
            case DEL:
                if (cnt == 0) {
                    break;
                }
                cnt--;                         /* decrement count.               */
                lp--;                          /* and line pointer.              */
                UartPutc (0x08);               /* echo backspace.                */
                UartPutc (' ');
                UartPutc (0x08);
                fflush (stdout);
                break;
            case ESC:
            case 0:
                *lp = 0;                       /* ESC - stop editing line.       */
                return false;
            case CR:                            /* CR - done, stop editing line.  */
                UartPutc (*lp = c);             /* Echo and store character.      */
                lp++;                           /* Increment line pointer         */
                cnt++;                          /* and count.                     */
                c = LF;
                break;
            default:
                UartPutc (*lp = c);            /* echo and store character.      */
                fflush (stdout);
                lp++;                          /* increment line pointer.        */
                cnt++;                         /* and count.                     */
                break;
        }
    } while (cnt < len - 2  &&  c != LF);      /* check limit and CR.            */
    *lp = 0;                                   /* mark end of string.            */
    return true;
}

__attribute__((noreturn)) void UartEndSimulation(int code)
{
    UartPutc((char) 0x4);  // End of simulation
    UartPutc((char) code); // Exit code
    while(1);
}
