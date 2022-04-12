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
#ifdef __cplusplus
extern "C" {
#endif

#include "timer_native.h"

#include <time.h>
#include <string.h>

#define MILLISECONDS_IN_SECOND      1000
#define MICROSECONDS_IN_SECOND      1000000
#define NANOSECONDS_IN_MILLISECOND  1000000
#define NANOSECONDS_IN_MICROSECOND  1000

void platform_reset_counters() { /* Nothing to do */ }

void platform_get_counters(pmu_counters* counters)
{
    struct timespec current_time;
    counters->num_counters = 0;
    counters->initialised = true;
    clock_gettime(1, &current_time);
    uint64_t microseconds = (current_time.tv_sec * MICROSECONDS_IN_SECOND) +
                            (current_time.tv_nsec / NANOSECONDS_IN_MICROSECOND);

#if NUM_PMU_COUNTERS > 0
    counters->counters[0].value = microseconds;
    counters->counters[0].name = "Duration";
    counters->counters[0].unit = "microseconds";
    ++counters->num_counters;
#endif /* NUM_PMU_COUNTERS > 0 */
}

#ifdef __cplusplus
}
#endif
