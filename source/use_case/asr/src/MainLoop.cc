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
#include "hal.h"                     /* Brings in platform definitions. */
#include "Labels.hpp"                /* For label strings. */
#include "UseCaseHandler.hpp"        /* Handlers for different user options. */
#include "Wav2LetterModel.hpp"       /* Model class for running inference. */
#include "UseCaseCommonUtils.hpp"    /* Utils functions. */
#include "AsrClassifier.hpp"         /* Classifier. */
#include "InputFiles.hpp"            /* Generated audio clip header. */
#include "Wav2LetterPreprocess.hpp"  /* Pre-processing class. */
#include "Wav2LetterPostprocess.hpp" /* Post-processing class. */
#include "log_macros.h"

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

/** @brief Verify input and output tensor are of certain min dimensions. */
static bool VerifyTensorDimensions(const arm::app::Model& model);

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

void main_loop()
{
    arm::app::Wav2LetterModel model;  /* Model wrapper object. */

    /* Load the model. */
    if (!model.Init()) {
        printf_err("Failed to initialise model\n");
        return;
    } else if (!VerifyTensorDimensions(model)) {
        printf_err("Model's input or output dimension verification failed\n");
        return;
    }

    /* Initialise pre-processing. */
    arm::app::audio::asr::Preprocess prep(
                                GetNumMfccFeatures(model),
                                g_FrameLength,
                                g_FrameStride,
                                GetNumMfccFeatureVectors(model));

    /* Initialise post-processing. */
    const uint32_t outputCtxLen = GetOutputContextLen(model, g_ctxLen);
    const uint32_t blankTokenIdx = 28;
    arm::app::audio::asr::Postprocess postp(
                                outputCtxLen,
                                GetOutputInnerLen(model, outputCtxLen),
                                blankTokenIdx);

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;
    std::vector <std::string> labels;
    GetLabelsVector(labels);
    arm::app::AsrClassifier classifier;  /* Classifier wrapper object. */

    arm::app::Profiler profiler{"asr"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("clipIndex", 0);
    caseContext.Set<uint32_t>("frameLength", g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", g_FrameStride);
    caseContext.Set<float>("scoreThreshold", g_ScoreThreshold);  /* Score threshold. */
    caseContext.Set<uint32_t>("ctxLen", g_ctxLen);  /* Left and right context length (MFCC feat vectors). */
    caseContext.Set<const std::vector <std::string>&>("labels", labels);
    caseContext.Set<arm::app::AsrClassifier&>("classifier", classifier);
    caseContext.Set<arm::app::audio::asr::Preprocess&>("preprocess", prep);
    caseContext.Set<arm::app::audio::asr::Postprocess&>("postprocess", postp);

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

    /* Check to make sure that the input tensor supports the above
     * context and inner lengths. */
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
