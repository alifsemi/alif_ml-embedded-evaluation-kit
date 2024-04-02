/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

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

#ifndef PLATFORM_DRIVERS_H
#define PLATFORM_DRIVERS_H

#include "log_macros.h"     /* Logging related helpers. */

/* Platform components */
#include "RTE_Components.h" /* For CPU related defintiions */
#include "timer_ensemble.h"     /* Timer functions. */
#include "tracelib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialises the platform components.
 * @return  0 if successful, error code otherwise.
 */
int platform_init(void);

/**
 * @brief   Teardown for platform components.
 */
void platform_release(void);

/**
 * @brief   Gets the platform name.
 * @return  Pointer to the name
 */
const char* platform_name(void);

extern bool run_requested(void);
extern void init_trigger_rx(void);
extern void init_trigger_tx(void);

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DRIVERS_H */
