/* This file was ported to work on Alif Semiconductor devices. */

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

#include "ethosu_cpu_cache.h"

#include "RTE_Components.h"         /* For CPU related defintiions */
#include "ethosu_driver.h"          /* Arm Ethos-U driver header */
#include "log_macros.h"             /* Logging macros */

bool __attribute__((weak)) ethosu_area_needs_flush_dcache(const uint32_t *p, size_t bytes)
{
    UNUSED(p);
    UNUSED(bytes);
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    return SCB->CCR & SCB_CCR_DC_Msk;
#else
    return false;
#endif

}

bool __attribute__((weak)) ethosu_area_needs_invalidate_dcache(const uint32_t *p, size_t bytes)
{
    UNUSED(p);
    UNUSED(bytes);
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    return SCB->CCR & SCB_CCR_DC_Msk;
#else
    return false;
#endif
}

void ethosu_flush_dcache(const uint64_t *base_addr, const size_t *base_addr_size, int num_base_addr)
{
    bool need_flush = false;
    for (int i = 0; i < num_base_addr; i++) {
        if (ethosu_area_needs_flush_dcache((void *)base_addr[i], base_addr_size[i])) {
            need_flush = true;
            break;
        }
    }

    if (need_flush) {

        /**
         * @note We could choose to call the `SCB_CleanDCache_by_Addr` function
         *       here, but the sizes which this function is called for, can
         *       cause unnecessary delays. It's worth noting that this function
         *       is called from the Arm Ethos-U NPU drive repeatedly for each
         *       region it accesses. This could even be RO memory which does
         *       not need cache maintenance, along with parts of the input and
         *       output tensors which rightly need to be cleaned. Therefore, to
         *       reduce overhead of repeated calls for large memory sizes, we
         *       call the clean and invalidation functions for whole cache.
         *
         *       If the neural network to be executed is completely falling
         *       onto the NPU, consider disabling the data cache altogether
         *       for the duration of the inference to further reduce the cache
         *       maintenance burden in these functions.
         */

        /** Clean the cache */
        trace("Cleaning data cache\n");
        SCB_CleanDCache();
    } else {
        __DSB();
    }
}

void ethosu_invalidate_dcache(const uint64_t *base_addr, const size_t *base_addr_size, int num_base_addr)
{
    bool need_flush = false;
    for (int i = 0; i < num_base_addr; i++) {
        if (ethosu_area_needs_invalidate_dcache((void *)base_addr[i], base_addr_size[i])) {
            need_flush = true;
            break;
        }
    }

    if (need_flush) {
        /**
         * See note in ethosu_flush_dcache function for why we clean the whole
         * cache instead of calling it for specific addresses.
         **/
        trace("Invalidating data cache\n");
        /* Not safe to simply invalidate without cleaning unless we know there are no write-back areas in the system */
        SCB_CleanInvalidateDCache();

    } else {
        __DSB();
    }
}
