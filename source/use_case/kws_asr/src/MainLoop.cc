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
#include "InputFiles.hpp"           /* For input images. */
#include "Labels_micronetkws.hpp"   /* For MicroNetKws label strings. */
#include "Labels_wav2letter.hpp"    /* For Wav2Letter label strings. */
#include "KwsClassifier.hpp"        /* KWS classifier. */
#include "AsrClassifier.hpp"        /* ASR classifier. */
#include "MicroNetKwsModel.hpp"     /* KWS model class for running inference. */
#include "Wav2LetterModel.hpp"      /* ASR model class for running inference. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

namespace arm {
namespace app {

    namespace asr {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace asr */

    namespace kws {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace kws */
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
} /* namespace app */
} /* namespace arm */

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

/** @brief   Verify input and output tensor are of certain min dimensions. */
static bool VerifyTensorDimensions(const arm::app::Model& model);

void main_loop()
{
    /* Model wrapper objects. */
    arm::app::MicroNetKwsModel kwsModel;
    arm::app::Wav2LetterModel asrModel;

    /* Load the models. */
    if (!kwsModel.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::kws::GetModelPointer(),
                       arm::app::kws::GetModelLen())) {
        printf_err("Failed to initialise KWS model\n");
        return;
    }

    /* Initialise the asr model using the same allocator from KWS
     * to re-use the tensor arena. */
    if (!asrModel.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::asr::GetModelPointer(),
                       arm::app::asr::GetModelLen(),
                       kwsModel.GetAllocator())) {
        printf_err("Failed to initialise ASR model\n");
        return;
    } else if (!VerifyTensorDimensions(asrModel)) {
        printf_err("Model's input or output dimension verification failed\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"kws_asr"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("kwsModel", kwsModel);
    caseContext.Set<arm::app::Model&>("asrModel", asrModel);
    caseContext.Set<uint32_t>("clipIndex", 0);
    caseContext.Set<uint32_t>("ctxLen", arm::app::asr::g_ctxLen);  /* Left and right context length (MFCC feat vectors). */
    caseContext.Set<int>("kwsFrameLength", arm::app::kws::g_FrameLength);
    caseContext.Set<int>("kwsFrameStride", arm::app::kws::g_FrameStride);
    caseContext.Set<float>("kwsScoreThreshold", arm::app::kws::g_ScoreThreshold);  /* Normalised score threshold. */
    caseContext.Set<uint32_t >("kwsNumMfcc", arm::app::kws::g_NumMfcc);
    caseContext.Set<uint32_t >("kwsNumAudioWins", arm::app::kws::g_NumAudioWins);

    caseContext.Set<int>("asrFrameLength", arm::app::asr::g_FrameLength);
    caseContext.Set<int>("asrFrameStride", arm::app::asr::g_FrameStride);
    caseContext.Set<float>("asrScoreThreshold", arm::app::asr::g_ScoreThreshold);  /* Normalised score threshold. */

    arm::app::KwsClassifier kwsClassifier;  /* Classifier wrapper object. */
    arm::app::AsrClassifier asrClassifier;  /* Classifier wrapper object. */
    caseContext.Set<arm::app::KwsClassifier&>("kwsClassifier", kwsClassifier);
    caseContext.Set<arm::app::AsrClassifier&>("asrClassifier", asrClassifier);

    std::vector<std::string> asrLabels;
    arm::app::asr::GetLabelsVector(asrLabels);
    std::vector<std::string> kwsLabels;
    arm::app::kws::GetLabelsVector(kwsLabels);
    caseContext.Set<const std::vector <std::string>&>("asrLabels", asrLabels);
    caseContext.Set<const std::vector <std::string>&>("kwsLabels", kwsLabels);

    /* KWS keyword that triggers ASR and associated checks */
    std::string triggerKeyword = std::string("no");
    if (std::find(kwsLabels.begin(), kwsLabels.end(), triggerKeyword) != kwsLabels.end()) {
        caseContext.Set<const std::string &>("triggerKeyword", triggerKeyword);
    }
    else {
        printf_err("Selected trigger keyword not found in labels file\n");
        return;
    }

    /* Loop. */
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
                executionSuccessful = ClassifyAudioHandler(
                        caseContext,
                        caseContext.Get<uint32_t>("clipIndex"),
                        false);
                break;
            case MENU_OPT_RUN_INF_CHOSEN: {
                printf("    Enter the audio clip index [0, %d]: ",
                       NUMBER_OF_FILES-1);
                fflush(stdout);
                auto clipIndex = static_cast<uint32_t>(
                        arm::app::ReadUserInputAsInt());
                executionSuccessful = ClassifyAudioHandler(caseContext,
                                                           clipIndex,
                                                           false);
                break;
            }
            case MENU_OPT_RUN_INF_ALL:
                executionSuccessful = ClassifyAudioHandler(
                        caseContext,
                        caseContext.Get<uint32_t>("clipIndex"),
                        true);
                break;
            case MENU_OPT_SHOW_MODEL_INFO:
                executionSuccessful = kwsModel.ShowModelInfoHandler();
                executionSuccessful = asrModel.ShowModelInfoHandler();
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

static bool VerifyTensorDimensions(const arm::app::Model& model)
{
    /* Populate tensor related parameters. */
    TfLiteTensor* inputTensor = model.GetInputTensor(0);
    if (!inputTensor->dims) {
        printf_err("Invalid input tensor dims\n");
        return false;
    } else if (inputTensor->dims->size < 3) {
        printf_err("Input tensor dimension should be >= 3\n");
        return false;
    }

    TfLiteTensor* outputTensor = model.GetOutputTensor(0);
    if (!outputTensor->dims) {
        printf_err("Invalid output tensor dims\n");
        return false;
    } else if (outputTensor->dims->size < 3) {
        printf_err("Output tensor dimension should be >= 3\n");
        return false;
    }

    return true;
}
