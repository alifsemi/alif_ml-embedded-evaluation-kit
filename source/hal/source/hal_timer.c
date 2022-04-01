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
#include "timer.h"
#include "log_macros.h"
#include "platform_drivers.h"

#include <assert.h>
#include <string.h>
#include <inttypes.h>

/**
 * @brief       Initialiser for HAL timer.
 * @param[in]   timer  Platform timer to initialize.
 **/
void init_timer(platform_timer* timer)
{
    assert(timer);
    memset(timer, 0, sizeof(*timer));

    timer->reset = platform_reset_counters;
    timer->get_counters = platform_get_counters;

    timer->reset();
    timer->inited = 1;
}
