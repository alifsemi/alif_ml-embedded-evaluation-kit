/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef RAM_TEST_H_
#define RAM_TEST_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ram_address_test(volatile uint8_t *ram, uint32_t size);
void ram_linear_test(uint8_t *ram);
void ram_random_test(uint8_t *test_ram);

#ifdef __cplusplus
}
#endif
#endif
