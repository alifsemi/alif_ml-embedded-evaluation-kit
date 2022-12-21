/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

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
#include <inttypes.h>

#include "RTE_Device.h"
#include "RTE_Components.h"
#include "Driver_PINMUX_AND_PINPAD.h"
#include "Driver_GPIO.h"
#include "uart_tracelib.h"
#include "services_lib_api.h"
#include "services_main.h"

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

#if CONSOLE_UART==2

static void MHU_msg_received(void* data);
extern ARM_DRIVER_GPIO Driver_GPIO1;
extern ARM_DRIVER_GPIO Driver_GPIO3;

#else

// IPC callback
void ipc_rx_callback(void *data)
{
    m55_data_payload_t* payload = (m55_data_payload_t*)data;
    char *st = (char*)payload->msg;
    uint16_t id = payload->id;
    printf("****** Got message from M55 HP CPU: %s, id: %d\n", st, id);
}
#endif

int platform_init(void)
{
    tracelib_init(NULL);

    int err = 0;
    info("Processor internal clock: %" PRIu32 "Hz\n", GetSystemCoreClock());

    info("%s: complete\n", __FUNCTION__);

#if CONSOLE_UART==2
    services_init(MHU_msg_received);

    // Initialize GPIOs to capture the buttons state

	Driver_GPIO1.Initialize(PIN_NUMBER_12, NULL);
	Driver_GPIO1.PowerControl(PIN_NUMBER_12, ARM_POWER_FULL);
	Driver_GPIO1.SetDirection(PIN_NUMBER_12, GPIO_PIN_DIRECTION_INPUT);
	PINMUX_Config(PORT_NUMBER_1, PIN_NUMBER_12, PINMUX_ALTERNATE_FUNCTION_0);
	PINPAD_Config(PORT_NUMBER_1, PIN_NUMBER_12, (PAD_FUNCTION_READ_ENABLE|PAD_FUNCTION_SCHMITT_TRIGGER_ENABLE|PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_HIGH_Z));

	Driver_GPIO3.Initialize(PIN_NUMBER_4, NULL);
	Driver_GPIO3.PowerControl(PIN_NUMBER_4, ARM_POWER_FULL);
	Driver_GPIO3.SetDirection(PIN_NUMBER_4, GPIO_PIN_DIRECTION_INPUT);
	PINMUX_Config(PORT_NUMBER_3, PIN_NUMBER_4, PINMUX_ALTERNATE_FUNCTION_0);
	PINPAD_Config(PORT_NUMBER_3, PIN_NUMBER_4, (PAD_FUNCTION_READ_ENABLE|PAD_FUNCTION_SCHMITT_TRIGGER_ENABLE|PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_HIGH_Z));
#else
    // init MHU Services communication
    services_init(ipc_rx_callback);
#endif


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

#if CONSOLE_UART==2
static bool do_inference_once = true;
static bool last_btn0 = false;
static bool last_btn1 = false;

void MHU_msg_received(void* data)
{
    m55_data_payload_t* payload = (m55_data_payload_t*)data;

    __DMB();

    switch(payload->id)
    {
        case 2:
            if (!strcmp(payload->msg, "go")) {
                // Switch single shot and continuous inference mode
               do_inference_once = false;
            }
            if (!strcmp(payload->msg, "stop")) {
                // Switch single shot and continuous inference mode
               do_inference_once = true;
            }
            break;
        case 3:
            break;
        default:
            break;
    }
}

extern bool run_requested(void)
{
    bool ret = true;
    bool new_btn0, new_btn1;
    uint32_t pin_state0, pin_state1;

    // Get new button state (active low)
    Driver_GPIO1.GetValue(PIN_NUMBER_12, &pin_state0);
    new_btn0 = pin_state0 == 0;
    Driver_GPIO3.GetValue(PIN_NUMBER_4, &pin_state1);
    new_btn1 = pin_state1 == 0;

    if (do_inference_once)
    {
        // Edge detector - run inference on the positive edge of the button pressed signal
        ret = !last_btn0 && new_btn0;
    }
    if (new_btn1 && last_btn1 != new_btn1)
    {
        // Switch single shot and continuous inference mode
        do_inference_once = !do_inference_once;
    }
    last_btn0 = new_btn0;
    last_btn1 = new_btn1;
    return ret;
}

#endif
