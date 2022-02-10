/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "hal_config.h"
#include "platform_timer.h"

/** Struct for describing the capabilities available for
 * the timer provided by HAL */
typedef struct _platform_timer_capability {
    uint32_t npu_cycles:    1;
    uint32_t cpu_cycles:    1;
    uint32_t duration_ms:   1;
    uint32_t duration_us:   1;
} timer_capability;

/* Structure to hold a platform specific timer implementation */
typedef struct _platform_timer {
    int inited;                 /**< initialised or not */
    timer_capability cap;       /**< capability of this timer */

    /* reset the timer */
    void (* reset)(void);

    /* Gets the current time counter. */
    time_counter (* get_time_counter)(void);

    /* Gets the duration in milliseconds. */
    time_t (* get_duration_ms)(time_counter *start, time_counter *end);

    /* Gets duration in microseconds. */
    time_t (* get_duration_us)(time_counter *start, time_counter *end);

    /* Gets difference in CPU cycle counts. */
    uint64_t (* get_cpu_cycle_diff)(time_counter *start, time_counter *end);

    /* Gets the difference in terms of cycle counts for collected pmu counters. */
    int (* get_npu_cycles_diff)(time_counter *start, time_counter *end,
                                uint64_t* pmu_counters_values, size_t size);

    /* Wraps get_time_counter function with additional profiling
     * initialisation, if required. */
    time_counter (* start_profiling)(void);

    /* Wraps get_time_counter function along with additional instructions when
     * profiling ends, if required. */
    time_counter (* stop_profiling)(void);

} platform_timer;

/**
 * @brief   Initialise the timer available for the platform.
 **/
void init_timer(platform_timer *timer);

#endif /* HAL_TIMER_H */
