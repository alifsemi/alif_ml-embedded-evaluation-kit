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
#include "Classifier.hpp"           /* Classifier. */
#include "DsCnnModel.hpp"           /* Model class for running inference. */
#include "hal.h"                    /* Brings in platform definitions. */
#include "Labels.hpp"               /* For label strings. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */

using KwsClassifier = arm::app::Classifier;

enum opcodes
{
    MENU_OPT_RUN_INF_NEXT = 1,       /* Run on next vector. */
    MENU_OPT_RUN_INF_CHOSEN,         /* Run on a user provided vector index. */
    MENU_OPT_RUN_INF_ALL,            /* Run inference on all. */
    MENU_OPT_SHOW_MODEL_INFO,        /* Show model info. */
    MENU_OPT_LIST_AUDIO_CLIPS        /* List the current baked audio clips. */
};

static void DisplayMenu()
{
    printf("\n\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Classify next audio clip\n", MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Classify audio clip at chosen index\n", MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run classification on all audio clips\n", MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List audio clips\n\n", MENU_OPT_LIST_AUDIO_CLIPS);
    printf("  Choice: ");
    fflush(stdout);
}

void main_loop(hal_platform& platform)
{
    arm::app::DsCnnModel model;  /* Model wrapper object. */

    /* Load the model. */
    if (!model.Init()) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{&platform, "kws"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);

    caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("clipIndex", 0);
    caseContext.Set<int>("frameLength", g_FrameLength);
    caseContext.Set<int>("frameStride", g_FrameStride);
    caseContext.Set<float>("scoreThreshold", g_ScoreThreshold);  /* Normalised score threshold. */

    KwsClassifier classifier;  /* classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    std::vector <std::string> labels;
    GetLabelsVector(labels);

    caseContext.Set<const std::vector <std::string>&>("labels", labels);

    bool executionSuccessful = true;
    constexpr bool bUseMenu = NUMBER_OF_FILES > 1 ? true : false;

    /* Loop. */
    do {
        int menuOption = MENU_OPT_RUN_INF_NEXT;
        if (bUseMenu) {
            DisplayMenu();
            menuOption = arm::app::ReadUserInputAsInt(platform);
            printf("\n");
        }
        switch (menuOption) {
            case MENU_OPT_RUN_INF_NEXT:
                executionSuccessful = ClassifyAudioHandler(caseContext, caseContext.Get<uint32_t>("clipIndex"), false);
                break;
            case MENU_OPT_RUN_INF_CHOSEN: {
                printf("    Enter the audio clip index [0, %d]: ", NUMBER_OF_FILES-1);
                fflush(stdout);
                auto clipIndex = static_cast<uint32_t>(arm::app::ReadUserInputAsInt(platform));
                executionSuccessful = ClassifyAudioHandler(caseContext, clipIndex, false);
                break;
            }
            case MENU_OPT_RUN_INF_ALL:
                executionSuccessful = ClassifyAudioHandler(caseContext,caseContext.Get<uint32_t>("clipIndex"), true);
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