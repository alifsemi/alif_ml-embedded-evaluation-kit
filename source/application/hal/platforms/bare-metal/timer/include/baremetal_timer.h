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
#ifndef BAREMETAL_TIMER_H
#define BAREMETAL_TIMER_H

#include <stdint.h>
#include <time.h>

#if defined (MPS3_PLATFORM)
    #include "timer_mps3.h"
    typedef mps3_time_counter   base_time_counter;
#else /* defined (MPS3_PLATFORM) */
    #include "timer_fvp.h"
    typedef fvp_time_counter    base_time_counter;
#endif  /* defined (MPS3_PLATFORM) */

typedef struct bm_time_counter {
    base_time_counter       counter;

#if defined (ARM_NPU)
    uint64_t                npu_total_ccnt;
    uint32_t                npu_idle_ccnt;
#endif /* ARM_NPU */

} time_counter;

#endif /* BAREMETAL_TIMER_H */
