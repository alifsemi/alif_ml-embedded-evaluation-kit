/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024-2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#ifndef PLATFORM_HAL_H
#define PLATFORM_HAL_H

/**
 * This file should present a C API for the main application logic to use
 * and be indifferent to the lower level platform. In addition to this it
 * will also need to be aware of the API exposed by data acquisition and
 * data presentation modules.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "platform_drivers.h"   /* Platform drivers */
#include "hal_pmu.h"            /* Timer/profiler API */
#include "hal_display.h"        /* Display interface */
#include "hal_audio.h"          /* Audio interface */
#include "hal_camera.h"         /* Camera interface */

#include <inttypes.h>
#include <stdbool.h>

/**
 * @brief       Initialise the HAL platform. This will go and initialise all the
 *              modules on the platform the application requires to run.
 * @return      True if successful, false otherwise.
 **/
bool hal_platform_init(void);


/**
 * @brief       Release the HAL platform. This should release resources acquired.
 *                          platform structure.
 **/
void hal_platform_release(void);

/**
 * @brief       Wait for any user input from the stdin interface.
 */
void hal_await_user_input();

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_HAL_H */
