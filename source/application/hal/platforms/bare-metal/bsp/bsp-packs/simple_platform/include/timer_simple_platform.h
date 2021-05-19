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
#ifndef TIMER_SIMPLE_PLATFORM_H
#define TIMER_SIMPLE_PLATFORM_H

#include "stubs_simple_platform.h"

/* Container for timestamp for simple platform. */
typedef struct _generic_time_counter {
    uint64_t    counter_systick;
} generic_time_counter;

/**
 * @brief   Resets the counters.
 */
void timer_reset(void);

/**
 * @brief   Gets the current counter values.
 * @returns counter struct.
 **/
generic_time_counter get_time_counter(void);

/**
 * @brief   Gets the cycle counts elapsed between start and end.
 * @return  difference in counter values as 32 bit unsigned integer.
 */
uint64_t get_cycle_count_diff(generic_time_counter *start, generic_time_counter *end);

/**
 * @brief   Enables or triggers cycle counting mechanism, if required
 *          by the platform.
 */
void start_cycle_counter(void);

/**
 * @brief   Stops cycle counting mechanism, if required by the platform.
 */
void stop_cycle_counter(void);

#endif /* TIMER_SIMPLE_PLATFORM_H */
