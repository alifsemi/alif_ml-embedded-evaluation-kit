/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
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
#include "hal.h"                      /* Brings in platform definitions. */
#include "YoloFastestModel.hpp"       /* Model class for running inference. */
#include "UseCaseHandler.hpp"         /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"     /* Utils functions. */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

#ifdef SE_SERVICES_SUPPORT
#include "services_lib_api.h"
extern run_profile_t default_runprof;
#endif

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace object_detection {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace object_detection */
} /* namespace app */
} /* namespace arm */


volatile bool obj_button_pressed = false;

void button2_cb(unsigned int event)
{
    (void)event;
    obj_button_pressed = true;
}

void MainLoop()
{
#ifdef SE_SERVICES_SUPPORT
    run_profile_t runprof   = default_runprof;
    runprof.power_domains   = PD_VBAT_AON_MASK | PD_SSE700_AON_MASK | PD_SYST_MASK | PD_SESS_MASK | PD_DBSS_MASK;
    runprof.ip_clock_gating = NPU_HP_MASK | CAMERA_MASK | MIPI_CSI_MASK;
#ifdef SHOW_UI
    runprof.ip_clock_gating |= MIPI_DSI_MASK | CDC200_MASK;
#endif
    if (set_power_run_profile(runprof)) {
        printf_err("Failed to set run profile\n");
    }
#endif

    BOARD_BUTTON2_Init(button2_cb);
    BOARD_BUTTON2_Control(BOARD_BUTTON_ENABLE_INTERRUPT);

    arm::app::YoloFastestModel model;  /* Model wrapper object. */

    if (!alif::app::ObjectDetectionInit()) {
        printf_err("Failed to initialise use case handler\n");
    }

    /* Load the model. */
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::object_detection::GetModelPointer(),
                    arm::app::object_detection::GetModelLen())) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"object_detection"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);

    /* Loop. */
    do {
        alif::app::ObjectDetectionHandler(caseContext);

        __disable_irq();
        while (obj_button_pressed) {
            info("Going to chip STOP mode...\n");
            pm_core_enter_deep_sleep_request_subsys_off();
            __enable_irq();
            __ISB();
            __disable_irq();
            info("Waiting for all IRQ are handled...\n");
        }
        __enable_irq();

    } while (1);
}
