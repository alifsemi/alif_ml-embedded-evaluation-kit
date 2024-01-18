/* Copyright (C) 2021-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef TRACELIB_H_
#define TRACELIB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* For Release build disable printf and semihosting */
//#define DISABLE_PRINTF

#ifdef DISABLE_PRINTF
#define printf tracef
#undef assert
#define assert 0
#endif

/**
 * @brief Initializes the trace lib.
 *
 * @param prefix string prepended to every trace message
 *
 * @note Max length is currently 256 - length of the prefix
 */
int tracelib_init(const char * prefix);

/**
 * @brief write trace to UART
 */
void tracef(const char * format, ...);

/**
 * @brief Send string to UART, no prefix is prepended.
 *
 * @param str string to send over UART
 * @param len length of the string
 */
int send_str(const char* str, uint32_t len);

unsigned int GetLine(char *user_input, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* TRACELIB_H_ */
