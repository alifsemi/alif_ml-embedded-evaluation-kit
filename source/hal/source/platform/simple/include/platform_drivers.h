/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#include "log_macros.h"   /* Logging related helpers. */

/* Platform components */
#include "RTE_Components.h"         /* For CPU related defintiions */
#include "timer_simple_platform.h"  /* timer implementation */
#include "platform_pmu.h"           /* PMU definitions and API */
#include "user_input.h"             /* User input functions */
#include "lcd_img.h"                /* LCD functions */

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

#endif /* PLATFORM_DRIVERS_H */
