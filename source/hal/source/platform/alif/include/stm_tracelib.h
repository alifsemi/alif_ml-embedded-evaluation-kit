/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef STM_TRACELIB_H_
#define STM_TRACELIB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void stm_init(uint16_t channel);

void stm_send(uint16_t channel, const char* str, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* STM_TRACELIB_H_ */
