/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdatomic.h>
#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "Driver_USART.h"

#include "uart_tracelib.h"

/* UART Driver */
extern ARM_DRIVER_USART ARM_Driver_USART_(CONSOLE_UART);

/* UART Driver instance */
static ARM_DRIVER_USART *USARTdrv = &ARM_Driver_USART_(CONSOLE_UART);

static atomic_uint_fast32_t uart_event;

void myUART_callback(uint32_t event)
{
    uart_event = event;
}

static bool initialized = false;

int uart_init(void)
{
    int32_t ret    = ARM_DRIVER_OK;

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

int uart_send(const char* str, uint32_t len)
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

char uart_getchar(void)
{
    char c;
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

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
