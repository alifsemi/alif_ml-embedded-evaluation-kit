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

#include "log_macros.h"     /* Logging functions */
#include "peripheral_memmap.h"
#include <string.h>         /* For strncpy */

#include <time.h>
#include <string.h>

#include <string.h>
#include <inttypes.h>

#include "RTE_Device.h"
#include "RTE_Components.h"
#include "Driver_PINMUX_AND_PINPAD.h"
#include "Driver_GPIO.h"
#include "uart_tracelib.h"

#include CMSIS_device_header

#if defined(ARM_NPU)
#include "ethosu_npu_init.h"

#if defined(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
#include "ethosu_ta_init.h"
#endif /* ETHOS_U_NPU_TIMING_ADAPTER_ENABLED */

#if defined(ETHOS_U_BASE_ADDR)
    #if (ETHOS_U_NPU_BASE != ETHOS_U_BASE_ADDR) && (SEC_ETHOS_U_NPU_BASE != ETHOS_U_BASE_ADDR)
        #error "NPU component configured with incorrect NPU base address."
    #endif /* (ETHOS_U_NPU_BASE != ETHOS_U_BASE_ADDR) && (SEC_ETHOS_U_NPU_BASE == ETHOS_U_BASE_ADDR) */
#else
    #error "ETHOS_U_BASE_ADDR should have been defined by the NPU component."
#endif /* defined(ETHOS_U_BASE_ADDR) */

#endif /* ARM_NPU */


/** Platform name */
static const char* s_platform_name = DESIGN_NAME;

extern ARM_DRIVER_GPIO Driver_GPIO1;
extern ARM_DRIVER_GPIO Driver_GPIO3;

int platform_init(void)
{
    tracelib_init(NULL);

    int err = 0;
    info("Processor internal clock: %" PRIu32 "Hz\n", GetSystemCoreClock());

    info("%s: complete\n", __FUNCTION__);


#if defined(ARM_NPU)

#if defined(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
    /* If the platform has timing adapter blocks along with Ethos-U core
     * block, initialise them here. */
    if (0 != (err = arm_ethosu_timing_adapter_init())) {
        return err;
    }
#endif /* ETHOS_U_NPU_TIMING_ADAPTER_ENABLED */

    int state;

    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init())) {
        return state;
    }

#endif /* ARM_NPU */

    /* Print target design info */
    info("Target system design: %s\n", s_platform_name);
    return err;
}

void platform_release(void)
{
    __disable_irq();
}

const char* platform_name(void)
{
    return s_platform_name;
}
