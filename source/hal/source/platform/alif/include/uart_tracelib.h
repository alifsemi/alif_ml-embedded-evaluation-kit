/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef UART_TRACELIB_H_
#define UART_TRACELIB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int uart_init(void);

int uart_send(const char* str, uint32_t len);

char uart_getchar(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_TRACELIB_H_ */
