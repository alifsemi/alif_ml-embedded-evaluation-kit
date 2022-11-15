/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "hal.h"                /* API */

#include "platform_drivers.h"   /* Platform drivers */
#include "log_macros.h"         /* Logging macros */

bool hal_platform_init(void)
{
    /* Initialise platform */
    if (0 != platform_init()) {
        printf_err("Failed to initialise platform %s\n", platform_name());
        return false;
    }

    /* Initialise LCD */
    if (0 != hal_lcd_init()) {
        printf_err("hal_lcd_init failed\n");
        return false;
    }

    /* Initialise PMU */
    hal_pmu_init();

    return true;
}

void hal_platform_release(void)
{
    platform_release();
}

bool hal_get_user_input(char* user_input, int size)
{
    if (1 != GetLine(user_input, size - 1)) {
        return true;
    }
    return false;
}
