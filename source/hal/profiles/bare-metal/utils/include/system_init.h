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
#ifndef BAREMETAL_SYSTEM_INIT_H
#define BAREMETAL_SYSTEM_INIT_H

#include "bsp.h"

/**
 * @brief   Initialises the platform (MPS3 FPGA board or Fixed Virtual Platform)
 *          Updates the system core clock and initialises the UART. It also
 *          verifies that the Cortex-M CPU variant being used matches the expected
 *          value if running on MPS3.
 * @return  0 if successful, error code otherwise.
*/
int system_init(void);

/**
 * @brief  Releases the platform (MPS3 FPGA board or Fixed Virtual Platform).
 **/
void system_release(void);

/**
 * @brief  Return the name the platform (MPS3 FPGA board or Fixed Virtual Platform).
 * @param[out]   name Platform name string.
 * @param[in]    size Name string length.
 **/
void system_name(char* name, size_t size);

#endif /* BAREMETAL_SYSTEM_INIT_H */
