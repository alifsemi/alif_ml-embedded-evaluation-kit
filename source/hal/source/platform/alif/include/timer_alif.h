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
#ifndef timer_alif_H
#define timer_alif_H

#include "platform_pmu.h"

#include <stdint.h>
#include <stdbool.h>

#if defined (ARM_NPU)
    #include "ethosu_profiler.h"    /* Arm Ethos-U NPU profiling functions. */
#endif /* defined (ARM_NPU) */

/**
 * @brief   Initialises the counters.
 */
void platform_init_counters(void);

/**
 * @brief   Finalises the counters.
 */
void platform_final_counters(void);

/**
 * @brief   Resets the counters.
 */
void platform_reset_counters(void);

/**
 * @brief       Gets the current counter values.
 * @param[out]  Pointer to a pmu_counters object.
 **/
void platform_get_counters(pmu_counters* counters);

/**
 * @brief   System tick interrupt handler.
 **/
void SysTick_Handler(void);

int Init_SysTick(void);
uint32_t Get_SysTick_Count(void);
uint64_t Get_SysTick_Cycle_Count(void);

#define Get_SysTick_Cycle_Count32() ((uint32_t) Get_SysTick_Cycle_Count())

#endif /* timer_alif_H */
