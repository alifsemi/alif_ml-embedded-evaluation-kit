/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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

#include "log_macros.h"     /* Logging macros */
#include "cmsis.h"          /* For CPU related defintiions */

#include <inttypes.h>

static uint64_t cpu_cycle_count = 0;    /* 64-bit cpu cycle counter */
extern uint32_t SystemCoreClock;        /* Expected to come from the cmsis-device lib */

/**
 * @brief   Gets the system tick triggered cycle counter for the CPU.
 * @return  64-bit counter value.
 **/
static uint64_t Get_SysTick_Cycle_Count(void);

/**
 * SysTick initialisation
 */
static int Init_SysTick(void);


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


void SysTick_Handler(void)
{
    /* Increment the cycle counter based on load value. */
    cpu_cycle_count += SysTick->LOAD + 1;
}

/**
 * Gets the current SysTick derived counter value
 */
static uint64_t Get_SysTick_Cycle_Count(void)
{
    uint32_t systick_val;

    NVIC_DisableIRQ(SysTick_IRQn);
    systick_val = SysTick->VAL & SysTick_VAL_CURRENT_Msk;
    NVIC_EnableIRQ(SysTick_IRQn);

    return cpu_cycle_count + (SysTick->LOAD - systick_val);
}

/**
 * SysTick initialisation
 */
static int Init_SysTick(void)
{
    const uint32_t ticks_10ms = SystemCoreClock/100 + 1;
    int err = 0;

    /* Reset CPU cycle count value. */
    cpu_cycle_count = 0;

    /* Changing configuration for sys tick => guard from being
     * interrupted. */
    NVIC_DisableIRQ(SysTick_IRQn);

    /* SysTick init - this will enable interrupt too. */
    err = SysTick_Config(ticks_10ms);

    /* Enable interrupt again. */
    NVIC_EnableIRQ(SysTick_IRQn);

    /* Wait for SysTick to kick off */
    while (!err && !SysTick->VAL) {
        __NOP();
    }

    return err;
}