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
#include "device_mps3.h"

#include "bsp_core_log.h"
#include "smm_mps3.h"

#include <inttypes.h>

uint32_t GetMPS3CoreClock(void)
{
    const uint32_t default_clock = 32000000;
    static int warned_once = 0;
    if (0 != MPS3_SCC->CFG_ACLK) {
        return MPS3_SCC->CFG_ACLK;
    }

    if (!warned_once) {
        warn("MPS3_SCC->CFG_ACLK reads 0. Assuming default clock of %" PRIu32 "\n",
            default_clock);
        warned_once = 1;
    }
    return default_clock;
}
