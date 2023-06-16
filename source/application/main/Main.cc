/*
 * SPDX-FileCopyrightText: Copyright 2021-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

/****************************************************************************\
 *               Main application file for ARM NPU on MPS3 board             *
\****************************************************************************/

#include "hal.h"                    /* our hardware abstraction api */
#include "log_macros.h"
#include "TensorFlowLiteMicro.hpp"  /* our inference logic api */

#include <cstdio>
#include <new>
#include <exception>

extern void main_loop();

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__ASM(" .global __ARM_use_no_argv\n");
#endif

/* Print application information. */
static void print_application_intro()
{
    info("%s\n", PRJ_DES_STR);
    info("Version %s Build date: " __DATE__ " @ " __TIME__ "\n", PRJ_VER_STR);
    info("Copyright 2021-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>\n\n");
}

static void out_of_heap()
{
    warn("Out of heap\n");
    std::terminate();
}

int main ()
{
    if (hal_platform_init()) {
        /* Application information, UART should have been initialised. */
        print_application_intro();

        /* Check the version of TensorFlow Lite Micro. */
        PrintTensorFlowVersion();

        std::set_new_handler(out_of_heap);

        /* Run the application. */
        main_loop();
    }

    /* This is unreachable without errors. */
    info("program terminating...\n");

    /* Release platform. */
    hal_platform_release();
    return 0;
}
