/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef ITM_TRACELIB_H_
#define ITM_TRACELIB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void itm_init(uint8_t channel);

void itm_send(uint8_t channel, const char* str, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* ITM_TRACELIB_H_ */
