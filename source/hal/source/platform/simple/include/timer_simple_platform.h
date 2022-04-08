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
#ifndef TIMER_SIMPLE_PLATFORM_H
#define TIMER_SIMPLE_PLATFORM_H

#include "platform_pmu.h"

#if defined (ARM_NPU)
    #include "ethosu_profiler.h"    /* Arm Ethos-U NPU profiling functions. */
#endif /* defined (ARM_NPU) */

#include <stdint.h>

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

#endif /* TIMER_SIMPLE_PLATFORM_H */
