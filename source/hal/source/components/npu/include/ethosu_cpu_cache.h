/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#ifndef ETHOSU_CPU_CACHE
#define ETHOSU_CPU_CACHE

#include <stdint.h>
#include <stddef.h>

/**
 * @brief   Flush/clean the data cache by address and size. Passing NULL as p argument
 *          expects the whole cache to be flushed.
 * @param[in]   p       Pointer to the start address.
 * @param[in]   bytes   Number of bytes to flush beginning at start address.
 */
void ethosu_flush_dcache(uint32_t *p, size_t bytes);

/**
 * @brief   Invalidate the data cache by address and size. Passing NULL as p argument
 *          expects the whole cache to be invalidated.
 * @param[in]   p       Pointer to the start address.
 * @param[in]   bytes   Number of bytes to flush beginning at start address.
 */
void ethosu_invalidate_dcache(uint32_t *p, size_t bytes);

#endif /* ETHOSU_CPU_CACHE */
