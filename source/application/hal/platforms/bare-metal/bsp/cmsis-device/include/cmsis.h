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
#ifndef BAREMETAL_CMSIS_H
#define BAREMETAL_CMSIS_H

#include "ARMCM55.h"  /* Cortex M system header file from CMSIS. */
#include "irqs.h"     /* Interrupt definitions file. */

/* Addition to template functions should be mentioned here. */

/**
 * @brief   Gets the internal processor clock.
 * @return  Clock frequency as unsigned 32 bit value.
 **/
uint32_t GetSystemCoreClock(void);

#endif  /* BAREMETAL_CMSIS_H */
