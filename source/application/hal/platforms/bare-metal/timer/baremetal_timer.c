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
#include <inttypes.h>

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

/** @brief  Gets the difference in idle NPU cycle counts
 * @param[in]   st      Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      Idle NPU cycle counts difference between the arguments expressed
 *              as unsigned 64 bit integer.
 **/
static uint64_t bm_get_npu_idle_cycle_diff(time_counter *st,
                                           time_counter *end);

/** @brief  Gets the difference in axi0 bus reads cycle counts
 * @param[in]   st      Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      NPU AXI0 read cycle counts  difference between the arguments expressed
 *              as unsigned 64 bit integer.
 **/
static uint64_t bm_get_npu_axi0_read_cycle_diff(time_counter *st,
                                                time_counter *end);

/** @brief  Gets the difference in axi0 bus writes cycle counts
 * @param[in]   st      Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      NPU AXI0 write cycle counts difference between the arguments expressed
 *              as unsigned 64 bit integer.
 **/
static uint64_t bm_get_npu_axi0_write_cycle_diff(time_counter *st,
                                                 time_counter *end);

/** @brief  Gets the difference in axi1 bus reads cycle counts
 * @param[in]   st      Pointer to time_counter value at start time.
 * @param[in]   end     Pointer to time_counter value at end.
 * @return      NPU AXI1 read cycle counts difference between the arguments expressed
 *              as unsigned 64 bit integer.
 **/
static uint64_t bm_get_npu_axi1_read_cycle_diff(time_counter *st,
                                                time_counter *end);

/** @brief  Gets the difference for 6 collected cycle counts:
 * 1) total NPU
 * 2) active NPU
 * 3) idle NPU
 * 4) axi0 read
 * 5) axi0 write
 * 6) axi1 read
 * */
static int bm_get_npu_cycle_diff(time_counter *st, time_counter *end,
                                  uint64_t* pmu_counters_values, const size_t size);

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
    timer->get_npu_cycles_diff = bm_get_npu_cycle_diff;
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

    /* We can retrieve only 4 PMU counters: */
    ETHOSU_PMU_Set_EVTYPER(0, ETHOSU_PMU_NPU_IDLE);
    ETHOSU_PMU_Set_EVTYPER(1, ETHOSU_PMU_AXI0_RD_DATA_BEAT_RECEIVED);
    ETHOSU_PMU_Set_EVTYPER(2, ETHOSU_PMU_AXI0_WR_DATA_BEAT_WRITTEN);
    ETHOSU_PMU_Set_EVTYPER(3, ETHOSU_PMU_AXI1_RD_DATA_BEAT_RECEIVED);

    /* Enable PMU. */
    ETHOSU_PMU_Enable();

    /* Enable counters for cycle and counter# 0. */
    ETHOSU_PMU_CNTR_Enable(ETHOSU_PMU_CNT1_Msk | ETHOSU_PMU_CNT2_Msk | ETHOSU_PMU_CNT3_Msk | ETHOSU_PMU_CNT4_Msk| ETHOSU_PMU_CCNT_Msk);
    _reset_ethosu_counters();
}

static int bm_get_npu_cycle_diff(time_counter *st, time_counter *end,
                                  uint64_t* pmu_counters_values, const size_t size)
{
    if (size == 6) {
        pmu_counters_values[0] = bm_get_npu_total_cycle_diff(st, end);
        pmu_counters_values[1] = bm_get_npu_active_cycle_diff(st, end);
        pmu_counters_values[2] = bm_get_npu_idle_cycle_diff(st, end);
        pmu_counters_values[3] = bm_get_npu_axi0_read_cycle_diff(st, end);
        pmu_counters_values[4] = bm_get_npu_axi0_write_cycle_diff(st, end);
        pmu_counters_values[5] = bm_get_npu_axi1_read_cycle_diff(st, end);
        return 0;
    } else {
        return 1;
    }
}

static uint64_t bm_get_npu_total_cycle_diff(time_counter *st, time_counter *end)
{
    return end->npu_total_ccnt - st->npu_total_ccnt;
}

static uint32_t counter_overflow(uint32_t pmu_counter_mask)
{
    /* Check for overflow: The idle counter is 32 bit while the
       total cycle count is 64 bit. */
    const uint32_t overflow_status = ETHOSU_PMU_Get_CNTR_OVS();
    return pmu_counter_mask & overflow_status;
}

static uint64_t bm_get_npu_idle_cycle_diff(time_counter *st, time_counter *end)
{
    if (counter_overflow(ETHOSU_PMU_CNT1_Msk)) {
        printf_err("EthosU PMU idle counter overflow.\n");
        return 0;
    }
    return (uint64_t)(end->npu_idle_ccnt - st->npu_idle_ccnt);
}

static uint64_t bm_get_npu_active_cycle_diff(time_counter *st, time_counter *end)
{
    /* Active NPU time = total time - idle time */
    return bm_get_npu_total_cycle_diff(st, end) - bm_get_npu_idle_cycle_diff(st, end);
}

static uint64_t bm_get_npu_axi0_read_cycle_diff(time_counter *st, time_counter *end)
{
    if (counter_overflow(ETHOSU_PMU_CNT2_Msk)) {
        printf_err("EthosU PMU axi0 read counter overflow.\n");
        return 0;
    }
    return (uint64_t)(end->npu_axi0_read_ccnt - st->npu_axi0_read_ccnt);
}

static uint64_t bm_get_npu_axi0_write_cycle_diff(time_counter *st, time_counter *end)
{
    if (counter_overflow(ETHOSU_PMU_CNT3_Msk)) {
        printf_err("EthosU PMU axi0 write counter overflow.\n");
        return 0;
    }
    return (uint64_t)(end->npu_axi0_write_ccnt - st->npu_axi0_write_ccnt);
}

static uint64_t bm_get_npu_axi1_read_cycle_diff(time_counter *st, time_counter *end)
{
    if (counter_overflow(ETHOSU_PMU_CNT4_Msk)) {
        printf_err("EthosU PMU axi1 read counter overflow.\n");
        return 0;
    }
    return (uint64_t)(end->npu_axi1_read_ccnt - st->npu_axi1_read_ccnt);
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
            .npu_total_ccnt = ETHOSU_PMU_Get_CCNTR(),
            .npu_idle_ccnt = ETHOSU_PMU_Get_EVCNTR(0),
            .npu_axi0_read_ccnt = ETHOSU_PMU_Get_EVCNTR(1),
            .npu_axi0_write_ccnt = ETHOSU_PMU_Get_EVCNTR(2),
            .npu_axi1_read_ccnt = ETHOSU_PMU_Get_EVCNTR(3)
#endif /* defined (ARM_NPU) */

    };

#if defined (ARM_NPU)
    debug("NPU total cc: %" PRIu64
        "; NPU idle cc: %" PRIu32
        "; NPU axi0 read cc: %" PRIu32
        "; NPU axi0 write cc: %" PRIu32
        "; NPU axi1 read cc: %" PRIu32 "\n",
        t.npu_total_ccnt,
        t.npu_idle_ccnt,
        t.npu_axi0_read_ccnt,
        t.npu_axi0_write_ccnt,
        t.npu_axi1_read_ccnt);
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
