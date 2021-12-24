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
#include "hal.h"                    /* Brings in platform definitions. */
#include "InputFiles.hpp"           /* For input images. */
#include "Labels_micronetkws.hpp"   /* For MicroNetKws label strings. */
#include "Labels_wav2letter.hpp"    /* For Wav2Letter label strings. */
#include "Classifier.hpp"           /* KWS classifier. */
#include "AsrClassifier.hpp"        /* ASR classifier. */
#include "MicroNetKwsModel.hpp"     /* KWS model class for running inference. */
#include "Wav2LetterModel.hpp"      /* ASR model class for running inference. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "Wav2LetterPreprocess.hpp" /* ASR pre-processing class. */
#include "Wav2LetterPostprocess.hpp"/* ASR post-processing class. */

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

/** @brief Gets the number of MFCC features for a single window. */
static uint32_t GetNumMfccFeatures(const arm::app::Model& model);

/** @brief Gets the number of MFCC feature vectors to be computed. */
static uint32_t GetNumMfccFeatureVectors(const arm::app::Model& model);

/** @brief Gets the output context length (left and right) for post-processing. */
static uint32_t GetOutputContextLen(const arm::app::Model& model,
                                    uint32_t inputCtxLen);

/** @brief Gets the output inner length for post-processing. */
static uint32_t GetOutputInnerLen(const arm::app::Model& model,
                                  uint32_t outputCtxLen);

void main_loop(hal_platform& platform)
{
    /* Model wrapper objects. */
    arm::app::MicroNetKwsModel kwsModel;
    arm::app::Wav2LetterModel asrModel;

    /* Load the models. */
    if (!kwsModel.Init()) {
        printf_err("Failed to initialise KWS model\n");
        return;
    }

    /* Initialise the asr model using the same allocator from KWS
     * to re-use the tensor arena. */
    if (!asrModel.Init(kwsModel.GetAllocator())) {
        printf_err("Failed to initialise ASR model\n");
        return;
    }

    /* Initialise ASR pre-processing. */
    arm::app::audio::asr::Preprocess prep(
            GetNumMfccFeatures(asrModel),
            arm::app::asr::g_FrameLength,
            arm::app::asr::g_FrameStride,
            GetNumMfccFeatureVectors(asrModel));

    /* Initialise ASR post-processing. */
    const uint32_t outputCtxLen = GetOutputContextLen(asrModel, arm::app::asr::g_ctxLen);
    const uint32_t blankTokenIdx = 28;
    arm::app::audio::asr::Postprocess postp(
            outputCtxLen,
            GetOutputInnerLen(asrModel, outputCtxLen),
            blankTokenIdx);

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{&platform, "kws_asr"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);

    caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<arm::app::Model&>("kwsmodel", kwsModel);
    caseContext.Set<arm::app::Model&>("asrmodel", asrModel);
    caseContext.Set<uint32_t>("clipIndex", 0);
    caseContext.Set<uint32_t>("ctxLen", arm::app::asr::g_ctxLen);  /* Left and right context length (MFCC feat vectors). */
    caseContext.Set<int>("kwsframeLength", arm::app::kws::g_FrameLength);
    caseContext.Set<int>("kwsframeStride", arm::app::kws::g_FrameStride);
    caseContext.Set<float>("kwsscoreThreshold", arm::app::kws::g_ScoreThreshold);  /* Normalised score threshold. */
    caseContext.Set<uint32_t >("kwsNumMfcc", arm::app::kws::g_NumMfcc);
    caseContext.Set<uint32_t >("kwsNumAudioWins", arm::app::kws::g_NumAudioWins);

    caseContext.Set<int>("asrframeLength", arm::app::asr::g_FrameLength);
    caseContext.Set<int>("asrframeStride", arm::app::asr::g_FrameStride);
    caseContext.Set<float>("asrscoreThreshold", arm::app::asr::g_ScoreThreshold);  /* Normalised score threshold. */

    KwsClassifier kwsClassifier;  /* Classifier wrapper object. */
    arm::app::AsrClassifier asrClassifier;  /* Classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("kwsclassifier", kwsClassifier);
    caseContext.Set<arm::app::AsrClassifier&>("asrclassifier", asrClassifier);

    caseContext.Set<arm::app::audio::asr::Preprocess&>("preprocess", prep);
    caseContext.Set<arm::app::audio::asr::Postprocess&>("postprocess", postp);

    std::vector<std::string> asrLabels;
    arm::app::asr::GetLabelsVector(asrLabels);
    std::vector<std::string> kwsLabels;
    arm::app::kws::GetLabelsVector(kwsLabels);
    caseContext.Set<const std::vector <std::string>&>("asrlabels", asrLabels);
    caseContext.Set<const std::vector <std::string>&>("kwslabels", kwsLabels);

    /* Index of the kws outputs we trigger ASR on. */
    caseContext.Set<uint32_t>("keywordindex", 9 );

    /* Loop. */
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
                        arm::app::ReadUserInputAsInt(platform));
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

static uint32_t GetNumMfccFeatures(const arm::app::Model& model)
{
    TfLiteTensor* inputTensor = model.GetInputTensor(0);
    const int inputCols = inputTensor->dims->data[arm::app::Wav2LetterModel::ms_inputColsIdx];
    if (0 != inputCols % 3) {
        printf_err("Number of input columns is not a multiple of 3\n");
    }
    return std::max(inputCols/3, 0);
}

static uint32_t GetNumMfccFeatureVectors(const arm::app::Model& model)
{
    TfLiteTensor* inputTensor = model.GetInputTensor(0);
    const int inputRows = inputTensor->dims->data[arm::app::Wav2LetterModel::ms_inputRowsIdx];
    return std::max(inputRows, 0);
}

static uint32_t GetOutputContextLen(const arm::app::Model& model, const uint32_t inputCtxLen)
{
    const uint32_t inputRows = GetNumMfccFeatureVectors(model);
    const uint32_t inputInnerLen = inputRows - (2 * inputCtxLen);
    constexpr uint32_t ms_outputRowsIdx = arm::app::Wav2LetterModel::ms_outputRowsIdx;

    /* Check to make sure that the input tensor supports the above context and inner lengths. */
    if (inputRows <= 2 * inputCtxLen || inputRows <= inputInnerLen) {
        printf_err("Input rows not compatible with ctx of %" PRIu32 "\n",
                   inputCtxLen);
        return 0;
    }

    TfLiteTensor* outputTensor = model.GetOutputTensor(0);
    const uint32_t outputRows = std::max(outputTensor->dims->data[ms_outputRowsIdx], 0);

    const float tensorColRatio = static_cast<float>(inputRows)/
                                 static_cast<float>(outputRows);

    return std::round(static_cast<float>(inputCtxLen)/tensorColRatio);
}

static uint32_t GetOutputInnerLen(const arm::app::Model& model,
                                  const uint32_t outputCtxLen)
{
    constexpr uint32_t ms_outputRowsIdx = arm::app::Wav2LetterModel::ms_outputRowsIdx;
    TfLiteTensor* outputTensor = model.GetOutputTensor(0);
    const uint32_t outputRows = std::max(outputTensor->dims->data[ms_outputRowsIdx], 0);
    return (outputRows - (2 * outputCtxLen));
}
