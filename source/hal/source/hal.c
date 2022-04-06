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
#include "hal.h"                /* API */

#include "platform_drivers.h"   /* Platform drivers */
#include "log_macros.h"         /* Logging macros */

#include <stdio.h>
#include <assert.h>
#include <string.h>

int hal_init(hal_platform* platform, platform_timer* timer)
{
    platform->timer     = timer;
    platform->platform_init     = platform_init;
    platform->platform_release  = platform_release;
    platform_name(platform->plat_name, sizeof(platform->plat_name));

    return 0;
}

/**
 * @brief  Local helper function to clean the slate for current platform.
 **/
static void hal_platform_clear(hal_platform* platform)
{
    assert(platform);
    platform->inited = 0;
}

int hal_platform_init(hal_platform* platform)
{
    int state;
    assert(platform && platform->platform_init);
    hal_platform_clear(platform);

    /* Initialise platform */
    if (0 != (state = platform->platform_init())) {
        printf_err("Failed to initialise platform %s\n", platform->plat_name);
        return state;
    }

    /* Initialise LCD */
    if (0 != (state = hal_lcd_init())) {
        printf_err("hal_lcd_init failed\n");
        return state;
    }

    /* Initialise the timer module */
    init_timer(platform->timer);

    info("%s platform initialised\n", platform->plat_name);
    platform->inited = !state;
    return state;
}

void hal_platform_release(hal_platform *platform)
{
    assert(platform && platform->platform_release);

    hal_platform_clear(platform);
    info("Releasing platform %s\n", platform->plat_name);
    platform->platform_release();
}

bool hal_get_user_input(char* user_input, int size)
{
    if (1 != GetLine(user_input, size - 1)) {
        return true;
    }
    return false;
}
