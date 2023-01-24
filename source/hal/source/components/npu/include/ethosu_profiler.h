/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef ETHOS_U_PROFILER_H
#define ETHOS_U_PROFILER_H

#include "pmu_ethosu.h"

#define ETHOSU_DERIVED_NCOUNTERS     1      /**< Number of counters derived from event counters */
#define ETHOSU_PROFILER_NUM_COUNTERS ( \
            ETHOSU_DERIVED_NCOUNTERS + \
            ETHOSU_PMU_NCOUNTERS +     \
            1 /* total CCNT */)

typedef struct npu_event_counter_ {
    enum ethosu_pmu_event_type event_type;
    uint32_t event_mask;
    uint32_t counter_value;
    const char* unit;
    const char* name;
} npu_evt_counter;

typedef struct npu_derived_counter_ {
    uint32_t counter_value;
    const char* unit;
    const char* name;
} npu_derived_counter;

typedef struct ethosu_pmu_counters_ {
    uint64_t                npu_total_ccnt;     /**< Total NPU cycles */
    npu_evt_counter         npu_evt_counters[ETHOSU_PMU_NCOUNTERS];
    npu_derived_counter     npu_derived_counters[ETHOSU_DERIVED_NCOUNTERS];
    uint32_t                num_total_counters; /**< Total number of counters */
} ethosu_pmu_counters;

/**
 * @brief  Initialise the Arm Ethos-U NPU performance monitoring unit.
 */
void ethosu_pmu_init(void);

/**
 * @brief  Resets the Arm Ethos-U NPU PMU counters.
 */
void ethosu_pmu_reset_counters(void);

/**
 * @brief Get the Arm Ethos-U NPU PMU counters
 * @return ethosu_pmu_counters
 */
ethosu_pmu_counters ethosu_get_pmu_counters(void);

#endif /* ETHOS_U_PROFILER_H */
