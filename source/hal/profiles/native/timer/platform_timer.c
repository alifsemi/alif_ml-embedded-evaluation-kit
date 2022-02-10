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

#include "timer.h"

#include <assert.h>
#include <time.h>
#include <string.h>

#define MILLISECONDS_IN_SECOND      1000
#define MICROSECONDS_IN_SECOND      1000000
#define NANOSECONDS_IN_MILLISECOND  1000000
#define NANOSECONDS_IN_MICROSECOND  1000

/**
 * @brief   Gets the current time counter value.
 * @return  Counter value expressed in terms of time_counter struct.
 **/
static time_counter get_time_counter(void)
{
    struct timespec current_time;
    clock_gettime(1, &current_time);
    time_counter t = {
        .current_secs = current_time.tv_sec,
        .current_nsecs = current_time.tv_nsec
    };

    return t;
}

/**
 * @brief       Gets the time duration elapsed between start and end.
 * @param[in]   start   Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      Difference in milliseconds between the arguments expressed
 *              as unsigned 32 bit integer.
 **/
static time_t get_duration_milliseconds(time_counter *start, time_counter *end)
{
    /* Convert both parts of time struct to ms then add for complete time. */
    time_t seconds_part =
        (end->current_secs - start->current_secs) * MILLISECONDS_IN_SECOND;
    time_t nanoseconds_part =
        (end->current_nsecs - start->current_nsecs) / NANOSECONDS_IN_MILLISECOND;

    return seconds_part + nanoseconds_part;
}

/**
 * @brief       Gets the time duration elapsed between start and end.
 * @param[in]   start   Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      Difference in microseconds between the arguments expressed
 *              as unsigned 32 bit integer.
 **/
static time_t get_duration_microseconds(time_counter *start, time_counter *end)
{
    /* Convert both parts of time struct to us then add for complete time. */
    time_t seconds_part =
        (end->current_secs - start->current_secs) * MICROSECONDS_IN_SECOND;
    time_t nanoseconds_part =
        (end->current_nsecs - start->current_nsecs) / NANOSECONDS_IN_MICROSECOND;

    return seconds_part + nanoseconds_part;
}

/**
 * @brief Stub for timer reset.
 **/
void reset_timer() {}

/**
 * @brief Initialise the timer for this platform.
 **/
void init_timer(platform_timer *timer)
{
    assert(timer);
    memset(timer, 0, sizeof(*timer));

    timer->get_time_counter = get_time_counter;
    timer->start_profiling = get_time_counter;
    timer->stop_profiling = get_time_counter;
    timer->get_duration_ms = get_duration_milliseconds;
    timer->cap.duration_ms = 1;
    timer->get_duration_us = get_duration_microseconds;
    timer->cap.duration_us = 1;
    timer->reset = reset_timer;
    timer->inited = 1;
}

#ifdef __cplusplus
}
#endif
