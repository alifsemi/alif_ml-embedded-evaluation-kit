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
#include "bsp.h"
#include "timer.h"

#include <assert.h>
#include <string.h>

#if defined (ARM_NPU)

#include "pmu_ethosu.h"

/**
 * @brief Initialises the PMU and enables the cycle counter.
 **/
static void _init_ethosu_cyclecounter(void);

/**
 * @brief       Gets the difference of total NPU cycle counts.
 *              (includes active and idle)
 * @param[in]   st      Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      Total NPU cycle counts difference between the arguments expressed
 *              as unsigned 64 bit integer.
 **/
static uint64_t bm_get_npu_total_cycle_diff(time_counter *st,
                                            time_counter *end);

/** 
 * @brief       Gets the difference in active NPU cycle counts.
 * @param[in]   st      Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      Active NPU cycle counts difference between the arguments expressed
 *              as unsigned 64 bit integer.
 **/
static uint64_t bm_get_npu_active_cycle_diff(time_counter *st,
                                             time_counter *end);

#endif /* defined (ARM_NPU) */

#if defined(MPS3_PLATFORM)
/** 
 * @brief       Wrapper for getting milliseconds duration between time counters
 * @param[in]   st      Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      Difference in milliseconds between given time counters.
 **/
static time_t bm_get_duration_ms(time_counter *st, time_counter *end);

/**
 * @brief       Wrapper for getting microseconds duration between time counters
 * @param[in]   st      Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      Difference in microseconds between given time counters.
 **/
static time_t bm_get_duration_us(time_counter *st, time_counter *end);
#endif /* defined(MPS3_PLATFORM) */

/**
 * @brief Wrapper for resetting timer.
 **/
static void bm_timer_reset(void);

/**
 * @brief   Wrapper for getting the current timer counter.
 * @return  Current time counter value.
 **/
static time_counter bm_get_time_counter(void);

/**
 * @brief   Wrapper for profiler start.
 * @return  Current profiler start timer counter.
 **/
static time_counter bm_start_profiling(void);

/**
 * @brief   Wrapper for profiler end.
 * @return  Current profiler end timer counter.
 **/
static time_counter bm_stop_profiling(void);

/**
 * @brief   Wrapper for getting CPU cycle difference between time counters.
 * @return  CPU cycle difference between given time counters expressed
 *          as unsigned 32 bit integer.
 **/
static uint32_t bm_get_cpu_cycles_diff(time_counter *st, time_counter *end);

/**
 * @brief       Initialiser for bare metal timer. 
 * @param[in]   timer  Platform timer to initialize.
 **/
void init_timer(platform_timer *timer)
{
    assert(timer);
    memset(timer, 0, sizeof(*timer));

    timer->reset            = bm_timer_reset;
    timer->get_time_counter = bm_get_time_counter;
    timer->start_profiling  = bm_start_profiling;
    timer->stop_profiling   = bm_stop_profiling;
    timer->get_cpu_cycle_diff = bm_get_cpu_cycles_diff;
    timer->cap.cpu_cycles = 1;

#if defined (MPS3_PLATFORM)
    timer->cap.duration_ms  = 1;
    timer->cap.duration_us  = 1;
    timer->get_duration_ms  = bm_get_duration_ms;
    timer->get_duration_us  = bm_get_duration_us;
#endif  /* defined (MPS3_PLATFORM) */

#if defined (ARM_NPU)
    /* We are capable of reporting npu cycle counts. */
    timer->cap.npu_cycles   = 1;
    timer->get_npu_total_cycle_diff = bm_get_npu_total_cycle_diff;
    timer->get_npu_active_cycle_diff = bm_get_npu_active_cycle_diff;
    _init_ethosu_cyclecounter();
#endif /* defined (ARM_NPU) */

    timer->reset();
    timer->inited = 1;
}

#if defined (ARM_NPU)

static void _reset_ethosu_counters(void)
{
    /* Reset all cycle and event counters. */
    ETHOSU_PMU_CYCCNT_Reset();
    ETHOSU_PMU_EVCNTR_ALL_Reset();
}

static void _init_ethosu_cyclecounter(void)
{
    /* Reset overflow status. */
    ETHOSU_PMU_Set_CNTR_OVS(ETHOSU_PMU_CNT1_Msk | ETHOSU_PMU_CCNT_Msk);

    /* Set the counter #0 to count idle cycles. */
    ETHOSU_PMU_Set_EVTYPER(0, ETHOSU_PMU_NPU_IDLE);

    /* Enable PMU. */
    ETHOSU_PMU_Enable();

    /* Enable counters for cycle and counter# 0. */
    ETHOSU_PMU_CNTR_Enable(ETHOSU_PMU_CNT1_Msk | ETHOSU_PMU_CCNT_Msk);

    _reset_ethosu_counters();
}

static uint64_t bm_get_npu_total_cycle_diff(time_counter *st, time_counter *end)
{
    return end->npu_total_ccnt - st->npu_total_ccnt;
}

static uint64_t bm_get_npu_active_cycle_diff(time_counter *st, time_counter *end)
{
    /* Check for overflow: The idle counter is 32 bit while the
       total cycle count is 64 bit. */
    const uint32_t overflow_status = ETHOSU_PMU_Get_CNTR_OVS();

    if (ETHOSU_PMU_CNT1_Msk & overflow_status) {
        printf_err("EthosU PMU idle counter overflow.\n");
        return 0;
    }

    /* Active NPU time = total time - idle time */
    return (bm_get_npu_total_cycle_diff(st, end) +
           (uint64_t)(st->npu_idle_ccnt)) - (uint64_t)(end->npu_idle_ccnt);
}

#endif /* defined (ARM_NPU) */

static void bm_timer_reset(void)
{
#if defined (ARM_NPU)
    _init_ethosu_cyclecounter();
#endif /* defined (ARM_NPU) */

    timer_reset();
}

static time_counter bm_get_time_counter(void)
{
    time_counter t = {
        .counter = get_time_counter(),

#if defined (ARM_NPU)
        .npu_idle_ccnt = ETHOSU_PMU_Get_EVCNTR(0),
        .npu_total_ccnt = ETHOSU_PMU_Get_CCNTR()
#endif /* defined (ARM_NPU) */

    };

#if defined (ARM_NPU)
    debug("NPU total cc: %llu; NPU idle cc: %u\n",
        t.npu_total_ccnt, t.npu_idle_ccnt);
#endif /* defined (ARM_NPU) */

    return t;
}

static time_counter bm_start_profiling(void)
{
    start_cycle_counter();
    return bm_get_time_counter();
}

static time_counter bm_stop_profiling(void)
{
    stop_cycle_counter();
    return bm_get_time_counter();
}

static uint32_t bm_get_cpu_cycles_diff(time_counter *st, time_counter *end)
{
    return get_cycle_count_diff(&(st->counter), &(end->counter));
}

#if defined(MPS3_PLATFORM)
static time_t bm_get_duration_ms(time_counter *st, time_counter *end)
{
    return get_duration_milliseconds(&(st->counter), &(end->counter));
}

static time_t bm_get_duration_us(time_counter *st, time_counter *end)
{
    return get_duration_microseconds(&(st->counter), &(end->counter));
}
#endif /* defined(MPS3_PLATFORM) */
