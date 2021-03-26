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
#ifndef TIMER_MPS3_H
#define TIMER_MPS3_H

#include <stdint.h>
#include <time.h>

/* Container for timestamp up-counters. */
typedef struct _mps3_time_counter {
    uint32_t    counter_1Hz;
    uint32_t    counter_100Hz;

    /* Running at FPGA clock rate. See GetMPS3CoreClock(). */
    uint32_t    counter_fpga;

    /* Running at processor core's internal clock rate, triggered by SysTick. */
    uint64_t    counter_systick;
} mps3_time_counter;

/**
 * @brief   Resets the counters.
 */
void timer_reset(void);

/**
 * @brief   Gets the current counter values.
 * @returns Mps3 timer counter.
 **/
mps3_time_counter get_time_counter(void);

/**
 * @brief       Gets the duration elapsed between two counters in milliseconds.
 * @param[in]   start   Pointer to mps3_time_counter value at start time.
 * @param[in]   end     Pointer to mps3_time_counter value at end.
 * @returns     Difference in milliseconds between the two give counters 
 *              expressed as an unsigned integer.
 **/
uint32_t get_duration_milliseconds(mps3_time_counter *start,
                                   mps3_time_counter *end);

/**
 * @brief       Gets the duration elapsed between two counters in microseconds.
 * @param[in]   start   Pointer to mps3_time_counter value at start time.
 * @param[in]   end     Pointer to mps3_time_counter value at end.
 * @returns     Difference in microseconds between the two give counters 
 *              expressed as an unsigned integer.
 **/
uint32_t get_duration_microseconds(mps3_time_counter *start,
                                   mps3_time_counter *end);

/**
 * @brief       Gets the cycle counts elapsed between start and end.
 * @param[in]   start   Pointer to mps3_time_counter value at start time.
 * @param[in]   end     Pointer to mps3_time_counter value at end.
 * @return      Difference in counter values as 32 bit unsigned integer.
 **/
uint64_t get_cycle_count_diff(mps3_time_counter *start,
                              mps3_time_counter *end);

/**
 * @brief   Enables or triggers cycle counting mechanism, if required
 *          by the platform.
 **/
void start_cycle_counter(void);

/**
 * @brief   Stops cycle counting mechanism, if required by the platform.
 **/
void stop_cycle_counter(void);

#endif /* TIMER_MPS3_H */
