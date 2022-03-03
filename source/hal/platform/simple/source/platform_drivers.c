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

#include "platform_drivers.h"

#include "uart_stdout.h"
#include <string.h>

#if defined(ARM_NPU)
#include "ethosu_npu_init.h"

#if defined(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
#include "ethosu_ta_init.h"
#endif /* ETHOS_U_NPU_TIMING_ADAPTER_ENABLED */

#endif /* ARM_NPU */

int platform_init(void)
{
    SystemCoreClockUpdate();    /* From start up code */

    /* UART init - will enable valid use of printf (stdout
     * re-directed at this UART (UART0) */
    UartStdOutInit();

    info("%s: complete\n", __FUNCTION__);

#if defined(ARM_NPU)

    int state;

    /* If the platform has timing adapter blocks along with Ethos-U core
     * block, initialise them here. */
#if defined(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
    int err;

    if (0 != (err = arm_ethosu_timing_adapter_init())) {
        return err;
    }
#endif /* ETHOS_U_NPU_TIMING_ADAPTER_ENABLED */

    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init())) {
        return state;
    }

#endif /* ARM_NPU */

    /* Print target design info */
    info("Target system design: %s\n", DESIGN_NAME);

    return 0;
}

void platform_release(void)
{
    __disable_irq();
}

void platform_name(char* name, size_t size)
{
    strncpy(name, DESIGN_NAME, size);
}
