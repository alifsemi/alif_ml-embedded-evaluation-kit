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
#include "InputFiles.hpp"           /* For input audio clips. */
#include "KwsClassifier.hpp"        /* Classifier. */
#include "MicroNetKwsModel.hpp"     /* Model class for running inference. */
#include "hal.h"                    /* Brings in platform definitions. */
#include "Labels.hpp"               /* For label strings. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

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

enum opcodes
{
    MENU_OPT_RUN_ONCE = 1,
    MENU_OPT_RUN_CONTINUOUS,
};

static void DisplayMenu()
{
    printf("\n\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Run classification on one audio window\n", MENU_OPT_RUN_ONCE);
    printf("  %u. Run classification continuously\n\n", MENU_OPT_RUN_CONTINUOUS);
    printf("  Choice: ");
    fflush(stdout);
}

void main_loop()
{
    init_trigger_tx();

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

#if USE_APP_MENU
    constexpr bool bUseMenu = true;
#else
    constexpr bool bUseMenu = false;
#endif

    /* Loop. */
    do {
        int menuOption = MENU_OPT_RUN_CONTINUOUS;
        if (bUseMenu) {
            DisplayMenu();
            menuOption = arm::app::ReadUserInputAsInt();
            printf("\n");
        }
        switch (menuOption) {
            case MENU_OPT_RUN_ONCE:
                executionSuccessful = alif::app::ClassifyAudioHandler(caseContext, true);
                break;
            case MENU_OPT_RUN_CONTINUOUS:
                executionSuccessful = alif::app::ClassifyAudioHandler(caseContext, false);
                break;
            default:
                printf("Incorrect choice, try again.");
                break;
        }
    } while (executionSuccessful || bUseMenu);
    info("Main loop terminated.\n");
}
