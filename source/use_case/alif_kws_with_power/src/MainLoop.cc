/* This file was ported to work on Alif Semiconductor devices. */

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
#include "KwsClassifier.hpp"        /* Classifier. */
#include "MicroNetKwsModel.hpp"     /* Model class for running inference. */
#include "hal.h"                    /* Brings in platform definitions. */
#include "Labels.hpp"               /* For label strings. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */
#include "board_utils.h"

#ifdef SE_SERVICES_SUPPORT
extern run_profile_t default_runprof;
#endif

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace kws {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
        extern const int g_AudioRate;
    } /* namespace kws */
} /* namespace app */
} /* namespace arm */

volatile bool kws_button_pressed = false;

void button2_cb(uint32_t event)
{
    (void)event;
    kws_button_pressed = true;
}

void MainLoop()
{
#ifdef SE_SERVICES_SUPPORT
    init_trigger_tx();

    run_profile_t runprof       = default_runprof;
    runprof.power_domains       = PD_VBAT_AON_MASK | PD_SSE700_AON_MASK | PD_SYST_MASK | PD_SESS_MASK;
    runprof.memory_blocks       = SERAM_MASK | MRAM_MASK | FWRAM_MASK | BACKUP4K_MASK;
    runprof.ip_clock_gating     = NPU_HE_MASK;
    runprof.phy_pwr_gating      = 0;

    if (set_power_run_profile(runprof)) {
        printf_err("Failed to set run profile\n");
    }
#endif

    BOARD_BUTTON2_Init(button2_cb);
    BOARD_BUTTON2_Control(BOARD_BUTTON_ENABLE_INTERRUPT);

    arm::app::MicroNetKwsModel model;  /* Model wrapper object. */

    /* Load the model. */
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::kws::GetModelPointer(),
                    arm::app::kws::GetModelLen())) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"kws"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<int>("frameLength", arm::app::kws::g_FrameLength);
    caseContext.Set<int>("frameStride", arm::app::kws::g_FrameStride);
    caseContext.Set<int>("audioRate", arm::app::kws::g_AudioRate);
    caseContext.Set<float>("scoreThreshold", arm::app::kws::g_ScoreThreshold);  /* Normalised score threshold. */

    arm::app::KwsClassifier classifier;  /* classifier wrapper object. */
    caseContext.Set<arm::app::KwsClassifier&>("classifier", classifier);

    std::vector <std::string> labels;
    GetLabelsVector(labels);

    caseContext.Set<const std::vector <std::string>&>("labels", labels);

    bool executionSuccessful = true;

    /* Loop. */
    do {
        executionSuccessful = alif::app::ClassifyAudioHandler(caseContext, false);
        info("Going to chip STOP mode...\n");
        __disable_irq();
        while(1) {
            pm_core_enter_deep_sleep_request_subsys_off();
            __enable_irq();
            __ISB();
            __disable_irq();
            info("Waiting for all IRQ are handled...\n");
        }

    } while (1);

    info("Main loop terminated.\n");
}
