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

#include "ethosu_cpu_cache.h"

#include "RTE_Components.h"         /* For CPU related defintiions */
#include "ethosu_driver.h"          /* Arm Ethos-U driver header */
#include "log_macros.h"             /* Logging macros */

/** Structure to maintain data cache states. */
typedef struct _cpu_cache_state {
    uint32_t dcache_invalidated : 1;
    uint32_t dcache_cleaned : 1;
} cpu_cache_state;

/** Static CPU cache state object.
 * @note This logic around flipping these states is based on the driver
 *       calling the functions in this sequence:
 *
 *       Cache flush (ethosu_flush_dcache)
 *                  ↓
 *       Start inference (ethosu_inference_begin)
 *                  ↓
 *       Inference (ethosu_dev_run_command_stream)
 *                  ↓
 *       End inference (ethosu_inference_end)
 *                  ↓
 *       Cache invalidate (ethosu_dcache_invalidate)
 **/
static cpu_cache_state s_cache_state = {.dcache_cleaned = 0, .dcache_invalidated = 0};

/**
 * @brief   Gets the current CPU cache state.
 * @return  Pointer to the CPU cache state object.
 */
static cpu_cache_state* ethosu_get_cpu_cache_state(void)
{
    return &s_cache_state;
}

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

void ethosu_clear_cache_states(void)
{
    cpu_cache_state* const state = ethosu_get_cpu_cache_state();
    trace("Clearing cache state members\n");
    state->dcache_invalidated = 0;
    state->dcache_cleaned     = 0;
}

void ethosu_flush_dcache(uint32_t *p, size_t bytes)
{
    cpu_cache_state* const state = ethosu_get_cpu_cache_state();
    if (ethosu_area_needs_flush_dcache(p, bytes)) {

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

        /** Clean the cache if it hasn't been cleaned already  */
        if (!state->dcache_cleaned) {
            trace("Cleaning data cache\n");
            SCB_CleanDCache();

            /** Assert the cache cleaned state and clear the invalidation
             *  state. */
            state->dcache_cleaned     = 1;
            state->dcache_invalidated = 0;
        }
    } else {
        __DSB();
    }
}

void ethosu_invalidate_dcache(uint32_t *p, size_t bytes)
{
    cpu_cache_state* const state = ethosu_get_cpu_cache_state();
    if (ethosu_area_needs_invalidate_dcache(p, bytes)) {
        /**
         * See note in ethosu_flush_dcache function for why we clean the whole
         * cache instead of calling it for specific addresses.
         **/
        if (!state->dcache_invalidated) {
            trace("Invalidating data cache\n");
            /* Not safe to simply invalidate without cleaning unless we know there are no write-back areas in the system */
            SCB_CleanInvalidateDCache();

            /** Assert the cache invalidation state and clear the clean
             *  state. */
            state->dcache_invalidated = 1;
            state->dcache_cleaned     = 0;
        }
    } else {
        __DSB();
    }
}
