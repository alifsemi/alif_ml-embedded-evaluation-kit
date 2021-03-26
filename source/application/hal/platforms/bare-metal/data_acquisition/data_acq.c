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

#include "bsp.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief       Get the user input from USART.
 * @param[out]  user_input  String read from the UART block.
 * @param[in]   size        String read length.
 * @return      0 if successful, error code otherwise.
 **/
static int get_uart_user_input(char* user_input, int size)
{
    if (true != GetLine(user_input, size - 1)) {
        printf_err("invalid input\n");
        return 1;
    }
    return 0;
}

int data_acq_channel_init(data_acq_module* module)
{
    assert(module);

    /* UART should have been initialised with low level initialisation
     * routines. */
    module->system_init = NULL;

    strncpy(module->system_name, "UART", sizeof(module->system_name));
    module->get_input = get_uart_user_input;
    module->inited = 1;

    return !(module->inited);
}

int data_acq_channel_release(data_acq_module* module)
{
    assert(module);
    module->inited = 0;
    module->get_input = NULL;
    return 0;
}
