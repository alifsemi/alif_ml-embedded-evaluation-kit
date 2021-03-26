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
#include "data_acq.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief   Initialize the acuisition.
 * @return  0 if successful, error code otherwise.
 **/
static int acquisition_init(void)
{
    return 0;
}

/**
 * @brief           Get the user input from stdin.
 * @param[out]      user_input  String read from the stdin.
 * @param[in,out]   size        String read length.
 * @return          0 if successful, error code otherwise.
 **/
static int get_user_input(char* user_input, int size)
{
    fgets(user_input, size, stdin);
    return 0;
}

int data_acq_channel_init(data_acq_module *module)
{
    assert(module);

    module->system_init = acquisition_init;
    module->get_input = get_user_input;
    strncpy(module->system_name, "native",
            sizeof(module->system_name));
    module->inited = !module->system_init();
    return !module->inited;
}

int data_acq_channel_release(data_acq_module *module)
{
    assert(module);
    module->inited = 0;
    return 0;
}
