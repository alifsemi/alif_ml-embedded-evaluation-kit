/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
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
#include "hal.h"                /* API */
#include "log_macros.h"         /* Logging macros */

/* Simple test to check if platform initialises correctly */
int main()
{
    if (true != hal_platform_init()) {
        printf_err("Failed to initialise platform!\n");
    }

    info("HAL test: OK\n");
    hal_platform_release();
    return 0;
}
