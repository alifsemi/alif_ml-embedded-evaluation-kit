/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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

#include "ethosu_profiler.h"
#include "ethosu_cpu_cache.h"
#include "log_macros.h"

#include <string.h>

extern struct ethosu_driver ethosu_drv;    /* Default Arm Ethos-U NPU device driver object */
static ethosu_pmu_counters s_npu_counters; /* NPU counter local instance */
static uint32_t s_evt_mask = 0;            /* PMU event mask */

static const char* unit_beats  = "beats";
static const char* unit_cycles = "cycles";

/**
 * @brief Gets the npu counter instance to be used.
 * @return Pointer to the npu counter instance.
 */
static ethosu_pmu_counters* get_counter_instance(void)
{
    return &s_npu_counters;
}

/**
 * @brief Gets the enabled event mask from the PMU driver.
 * @return Event mask as an unsigned 32 bit integer.
 */
static uint32_t get_event_mask(void)
{
    return s_evt_mask;
}

/**
 * @brief Sets the enabled event mask for the PMU driver.
 * @param[in] mask  event mask as an unsigned 32 bit integer.
 * @return none.
 */
static void set_event_mask(uint32_t mask)
{
    s_evt_mask = mask;
}

/**
 * @brief Checks if the counter has overflown.
 * @param pmu_counter_mask  Mask for the event counter.
 * @return  true if overflow is detected, false otherwise.
 */
static bool counter_overflow(uint32_t pmu_counter_mask)
{
    /* Check for overflow: The idle counter is 32 bit while the
       total cycle count is 64 bit. */
    const uint32_t overflow_status = ETHOSU_PMU_Get_CNTR_OVS(&ethosu_drv);
    return pmu_counter_mask & overflow_status ? true : false;
}

void ethosu_pmu_init(void)
{
    uint32_t i                    = 0;
    uint32_t evt_mask             = ETHOSU_PMU_CCNT_Msk;
    ethosu_pmu_counters* counters = get_counter_instance();
    memset(counters, 0, sizeof(*counters));

    /* Total counters = event counters + derived counters + total cycle count */
    counters->num_total_counters = ETHOSU_PROFILER_NUM_COUNTERS;

#if ETHOSU_PMU_NCOUNTERS >= 4
    counters->npu_evt_counters[0].event_type = ETHOSU_PMU_NPU_IDLE;
    counters->npu_evt_counters[0].event_mask = ETHOSU_PMU_CNT1_Msk;
    counters->npu_evt_counters[0].name       = "NPU IDLE";
    counters->npu_evt_counters[0].unit       = unit_cycles;

    counters->npu_evt_counters[1].event_type = ETHOSU_PMU_AXI0_RD_DATA_BEAT_RECEIVED;
    counters->npu_evt_counters[1].event_mask = ETHOSU_PMU_CNT2_Msk;
    counters->npu_evt_counters[1].name       = "NPU AXI0_RD_DATA_BEAT_RECEIVED";
    counters->npu_evt_counters[1].unit       = unit_beats;

    counters->npu_evt_counters[2].event_type = ETHOSU_PMU_AXI0_WR_DATA_BEAT_WRITTEN;
    counters->npu_evt_counters[2].event_mask = ETHOSU_PMU_CNT3_Msk;
    counters->npu_evt_counters[2].name       = "NPU AXI0_WR_DATA_BEAT_WRITTEN";
    counters->npu_evt_counters[2].unit       = unit_beats;

    counters->npu_evt_counters[3].event_type = ETHOSU_PMU_AXI1_RD_DATA_BEAT_RECEIVED;
    counters->npu_evt_counters[3].event_mask = ETHOSU_PMU_CNT4_Msk;
    counters->npu_evt_counters[3].name       = "NPU AXI1_RD_DATA_BEAT_RECEIVED";
    counters->npu_evt_counters[3].unit       = unit_beats;
#else /* ETHOSU_PMU_NCOUNTERS >= 4 */
#error "NPU PMU expects a minimum of 4 available event triggered counters!"
#endif /* ETHOSU_PMU_NCOUNTERS >= 4 */

#if ETHOSU_DERIVED_NCOUNTERS >= 1
    counters->npu_derived_counters[0].name = "NPU ACTIVE";
    counters->npu_derived_counters[0].unit = unit_cycles;
#endif /* ETHOSU_DERIVED_NCOUNTERS >= 1 */

    for (i = 0; i < ETHOSU_PMU_NCOUNTERS; ++i) {
        ETHOSU_PMU_Set_EVTYPER(&ethosu_drv, i, counters->npu_evt_counters[i].event_type);
        evt_mask |= counters->npu_evt_counters[i].event_mask;
    }

    set_event_mask(evt_mask);

    /* Reset overflow status. */
    ETHOSU_PMU_Set_CNTR_OVS(&ethosu_drv, get_event_mask());

    /* Enable PMU. */
    ETHOSU_PMU_Enable(&ethosu_drv);

    /* Enable counters for cycle and event counters. */
    ETHOSU_PMU_CNTR_Disable(&ethosu_drv, get_event_mask());
    ethosu_pmu_reset_counters();
}

/**
 * @brief  Resets the Arm Ethos-U NPU PMU counters.
 */
void ethosu_pmu_reset_counters(void)
{
    /* Reset all cycle and event counters. */
    ETHOSU_PMU_CYCCNT_Reset(&ethosu_drv);
    ETHOSU_PMU_EVCNTR_ALL_Reset(&ethosu_drv);
}

/**
 * @brief Get the Arm Ethos-U NPU PMU counters
 * @return ethosu_pmu_counters
 */
ethosu_pmu_counters ethosu_get_pmu_counters(void)
{
    ethosu_pmu_counters* counters = get_counter_instance();
    uint32_t i                    = 0;

    /* Event counters */
    for (i = 0; i < ETHOSU_PMU_NCOUNTERS; ++i) {
        if (counter_overflow(counters->npu_evt_counters[i].event_mask)) {
            warn("Counter overflow detected for %s.\n", counters->npu_evt_counters[i].name);
        }
        counters->npu_evt_counters[i].counter_value = ETHOSU_PMU_Get_EVCNTR(&ethosu_drv, i);
    }

    /* Total cycle count */
    counters->npu_total_ccnt = ETHOSU_PMU_Get_CCNTR(&ethosu_drv);

    /* Derived counters */
#if ETHOSU_DERIVED_NCOUNTERS >= 1
    if (counters->npu_evt_counters[0].event_type == ETHOSU_PMU_NPU_IDLE) {

        /* Compute the active count */
        counters->npu_derived_counters[0].counter_value =
            counters->npu_total_ccnt - counters->npu_evt_counters[0].counter_value;
    }
#endif /* ETHOSU_DERIVED_NCOUNTERS >= 1 */

    return *counters;
}

void ethosu_inference_begin(struct ethosu_driver* drv, void* userArg)
{
    UNUSED(userArg);
    ethosu_clear_cache_states();
    ETHOSU_PMU_CNTR_Disable(drv, get_event_mask());
    ETHOSU_PMU_CNTR_Enable(drv, get_event_mask());
}

void ethosu_inference_end(struct ethosu_driver* drv, void* userArg)
{
    UNUSED(userArg);
    ETHOSU_PMU_CNTR_Disable(drv, get_event_mask());
}
