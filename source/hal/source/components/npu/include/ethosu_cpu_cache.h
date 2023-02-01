/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include <stdbool.h>

/**
 * @brief   Clears all the cache state members.
 */
void ethosu_clear_cache_states(void);

/**
 * @brief   Precheck hook for ethosu_flush_dcache. Default weak implementation
 *          returns true if the data cache is enabled. Can be overridden to
 *          skip flush/clean for certain regions (eg TCM, or write-through cached areas).
 * @param[in]   p       Pointer to the start address.
 * @param[in]   bytes   Number of bytes to flush beginning at start address.
 * @return      true if a flush/clean is required
 */
bool ethosu_area_needs_flush_dcache(const uint32_t *p, size_t bytes);

/**
 * @brief   Precheck hook for ethosu_invalidate_dcache. Default weak implementation
 *          returns true if the data cache is enabled. Can be overridden to
 *          skip invalidates for certain regions (eg TCM).
 * @param[in]   p       Pointer to the start address (or NULL).
 * @param[in]   bytes   Number of bytes to flush beginning at start address.
 * @return      true if an invalidate is required
 */
bool ethosu_area_needs_invalidate_dcache(const uint32_t *p, size_t bytes);

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
