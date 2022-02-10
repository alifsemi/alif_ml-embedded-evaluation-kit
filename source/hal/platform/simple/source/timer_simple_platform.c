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
#include "timer_simple_platform.h"

#include "irqs.h"
#include "log_macros.h"

#include <inttypes.h>

base_time_counter get_time_counter(void)
{
    base_time_counter t = {
        .counter_systick = Get_SysTick_Cycle_Count()
    };
    debug("counter_systick: %" PRIu64 "\n", t.counter_systick);
    return t;
}

void timer_reset(void)
{
    if (0 != Init_SysTick()) {
        printf_err("Failed to initialise system tick config\n");
    }
    debug("system tick config ready\n");
}

uint64_t get_cycle_count_diff(base_time_counter *start,
                              base_time_counter *end)
{
    if (start->counter_systick > end->counter_systick) {
        warn("start > end; counter might have overflown\n");
    }
    return end->counter_systick - start->counter_systick;
}

void start_cycle_counter(void)
{
    /* Add any custom requirement for this platform here */
}

void stop_cycle_counter(void)
{
    /* Add any custom requirement for this platform here */
}
