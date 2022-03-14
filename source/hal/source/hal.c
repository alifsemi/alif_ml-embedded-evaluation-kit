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

int hal_init(hal_platform* platform, data_acq_module* data_acq,
    data_psn_module* data_psn, platform_timer* timer)
{
    assert(platform && data_acq && data_psn);

    platform->data_acq  = data_acq;
    platform->data_psn  = data_psn;
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

    /* Initialise the data acquisition module */
    if (0 != (state = data_acq_channel_init(platform->data_acq))) {
        if (!platform->data_acq->inited) {
            printf_err("Failed to initialise data acq module: %s\n",
                platform->data_acq->system_name);
        }
        hal_platform_release(platform);
        return state;
    }

    /* Initialise the presentation module */
    if (0 != (state = data_psn_system_init(platform->data_psn))) {
        printf_err("Failed to initialise data psn module: %s\n",
            platform->data_psn->system_name);
        data_acq_channel_release(platform->data_acq);
        hal_platform_release(platform);
        return state;
    }

    /* Followed by the timer module */
    init_timer(platform->timer);

    info("%s platform initialised\n", platform->plat_name);
    debug("Using %s module for data acquisition\n",
            platform->data_acq->system_name);
    debug("Using %s module for data presentation\n",
        platform->data_psn->system_name);

    platform->inited = !state;

    return state;
}

void hal_platform_release(hal_platform *platform)
{
    assert(platform && platform->platform_release);
    data_acq_channel_release(platform->data_acq);
    data_psn_system_release(platform->data_psn);

    hal_platform_clear(platform);
    info("Releasing platform %s\n", platform->plat_name);
    platform->platform_release();
}
