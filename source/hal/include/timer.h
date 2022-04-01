/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include "platform_drivers.h"  /* Platform package API */
#include "user_input.h"        /* PMU structs and API */

/* Structure to hold a platform specific timer implementation */
typedef struct _platform_timer {
    int inited;                           /**< Initialised or not. */
    void (* reset)(void);                 /**< Reset the timer. */
    pmu_counters (* get_counters)(void);  /**< Gets the current time counter. */

} platform_timer;

/**
 * @brief   Initialise the timer available for the platform.
 **/
void init_timer(platform_timer* timer);

#endif /* HAL_TIMER_H */
