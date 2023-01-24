/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "InputFiles.hpp"           /* For input data */
#include "AdModel.hpp"              /* Model class for running inference */
#include "UseCaseCommonUtils.hpp"   /* Utils functions */
#include "UseCaseHandler.hpp"       /* Handlers for different user options */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

namespace arm {
namespace app {
    namespace ad {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace ad */
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
} /* namespace app */
} /* namespace arm */

enum opcodes
{
    MENU_OPT_RUN_INF_NEXT = 1,       /* Run on next vector */
    MENU_OPT_RUN_INF_CHOSEN,         /* Run on a user provided vector index */
    MENU_OPT_RUN_INF_ALL,            /* Run inference on all */
    MENU_OPT_SHOW_MODEL_INFO,        /* Show model info */
    MENU_OPT_LIST_AUDIO_CLIPS        /* List the current baked audio signals */
};

static void DisplayMenu()
{
    printf("\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Classify next audio signal\n", MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Classify audio signal at chosen index\n", MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run classification on all audio signals\n", MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List audio signals\n\n", MENU_OPT_LIST_AUDIO_CLIPS);
    printf("  Choice: ");
    fflush(stdout);
}

void main_loop()
{
    arm::app::AdModel model;  /* Model wrapper object. */

    /* Load the model. */
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::ad::GetModelPointer(),
                    arm::app::ad::GetModelLen())) {
        printf_err("failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"ad"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("clipIndex", 0);
    caseContext.Set<uint32_t>("frameLength", arm::app::ad::g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", arm::app::ad::g_FrameStride);
    caseContext.Set<float>("scoreThreshold", arm::app::ad::g_ScoreThreshold);
    caseContext.Set<float>("trainingMean", arm::app::ad::g_TrainingMean);

    /* Main program loop. */
    bool executionSuccessful = true;
    constexpr bool bUseMenu = NUMBER_OF_FILES > 1 ? true : false;

    /* Loop. */
    do {
        int menuOption = MENU_OPT_RUN_INF_NEXT;
        if (bUseMenu) {
            DisplayMenu();
            menuOption = arm::app::ReadUserInputAsInt();
            printf("\n");
        }
        switch (menuOption) {
            case MENU_OPT_RUN_INF_NEXT:
                executionSuccessful = ClassifyVibrationHandler(
                        caseContext,
                        caseContext.Get<uint32_t>("clipIndex"),
                        false);
                break;
            case MENU_OPT_RUN_INF_CHOSEN: {
                printf("    Enter the data index [0, %d]: ",
                       NUMBER_OF_FILES-1);
                fflush(stdout);
                auto audioIndex = static_cast<uint32_t>(
                        arm::app::ReadUserInputAsInt());
                executionSuccessful = ClassifyVibrationHandler(caseContext,
                                                           audioIndex,
                                                           false);
                break;
            }
            case MENU_OPT_RUN_INF_ALL:
                executionSuccessful = ClassifyVibrationHandler(
                    caseContext,
                    caseContext.Get<uint32_t>("clipIndex"),
                    true);
                break;
            case MENU_OPT_SHOW_MODEL_INFO:
                executionSuccessful = model.ShowModelInfoHandler();
                break;
            case MENU_OPT_LIST_AUDIO_CLIPS:
                executionSuccessful = ListFilesHandler(caseContext);
                break;
            default:
                printf("Incorrect choice, try again.");
                break;
        }
    } while (executionSuccessful && bUseMenu);
    info("Main loop terminated.\n");
}
