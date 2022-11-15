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
#ifndef PLATFORM_PMU_INTERFACE_H
#define PLATFORM_PMU_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define NUM_PMU_COUNTERS     (10)     /**< Maximum number of available counters. */

/**
 * @brief   Container for a single unit for a PMU counter.
 */
typedef struct _pmu_counter_unit {
    uint64_t value;     /**< Value of the counter expressed as 64 bits unsigned integer. */
    const char* name;   /**< Name for the counter. */
    const char* unit;   /**< Unit that the counter value represents (like cycles, beats, or milliseconds). */
} pmu_counter_unit;

/**
 * @brief   Container for a an array of counters
 */
typedef struct _pmu_counters {
    pmu_counter_unit counters[NUM_PMU_COUNTERS]; /**< Counter array. */
    uint32_t num_counters;                       /**< Number of valid counters. */
    bool initialised;                            /**< Initialised or not. */
} pmu_counters;

/**
 * @brief   Resets the counters.
 */
void platform_reset_counters(void);

/**
 * @brief       Gets the current counter values.
 * @param[out]  Pointer to a pmu_counters object.
 **/
void platform_get_counters(pmu_counters* counters);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_PMU_INTERFACE_H */
