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
#include "timer_mps3.h"

#include "bsp_core_log.h"
#include "device_mps3.h"

void timer_reset(void)
{
    MPS3_FPGAIO->CLK1HZ   = 0;
    MPS3_FPGAIO->CLK100HZ = 0;
    MPS3_FPGAIO->COUNTER  = 0;

    if (0 != Init_SysTick()) {
        printf_err("Failed to initialise system tick config\n");
    }
    debug("system tick config ready\n");
}

mps3_time_counter get_time_counter(void)
{
    mps3_time_counter t = {
        .counter_1Hz        = MPS3_FPGAIO->CLK1HZ,
        .counter_100Hz      = MPS3_FPGAIO->CLK100HZ,
        .counter_fpga       = MPS3_FPGAIO->COUNTER,
        .counter_systick    = Get_SysTick_Cycle_Count()
    };
    debug("Timestamp:\
        \n\tCounter 1 Hz:   %u\
        \n\tCounter 100 Hz: %u\
        \n\tCounter FPGA:   %u\
        \n\tCounter CPU:    %llu\n",
        t.counter_1Hz, t.counter_100Hz, t.counter_fpga, t.counter_systick);
    return t;
}

/**
 * Please note, that there are no checks for overflow in this function => if
 * the time elapsed has been big (in days) this could happen and is currently
 * not handled.
 **/
uint32_t get_duration_milliseconds(mps3_time_counter *start,
                                   mps3_time_counter *end)
{
    uint32_t time_elapsed = 0;
    if (end->counter_100Hz > start->counter_100Hz) {
        time_elapsed = (end->counter_100Hz - start->counter_100Hz) * 10;
    } else {
        time_elapsed = (end->counter_1Hz - start->counter_1Hz) * 1000 +
            ((0xFFFFFFFF - start->counter_100Hz) + end->counter_100Hz + 1) * 10;
    }

    /* If the time elapsed is less than 100ms, use microseconds count to be
     * more precise */
    if (time_elapsed < 100) {
        debug("Using the microsecond function instead..\n");
        return get_duration_microseconds(start, end)/1000;
    }

    return time_elapsed;
}

/**
 * Like the microsecond counterpart, this function could return wrong results when
 * the counter (MAINCLK) overflows. There are no overflow counters available.
 **/
uint32_t get_duration_microseconds(mps3_time_counter *start,
                                   mps3_time_counter *end)
{
    const int divisor = GetMPS3CoreClock()/1000000;
    uint32_t time_elapsed = 0;
    if (end->counter_fpga > start->counter_fpga) {
        time_elapsed = (end->counter_fpga - start->counter_fpga)/divisor;
    } else {
        time_elapsed = ((0xFFFFFFFF - end->counter_fpga)
            + start->counter_fpga + 1)/divisor;
    }
    return time_elapsed;
}

uint64_t get_cycle_count_diff(mps3_time_counter *start,
                              mps3_time_counter *end)
{
    if (start->counter_systick > end->counter_systick) {
        warn("start > end; counter might have overflown\n");
    }
    return end->counter_systick - start->counter_systick;
}

void start_cycle_counter(void)
{
    /* Nothing to do for FPGA */
}

void stop_cycle_counter(void)
{
    /* Nothing to do for FPGA */
}
