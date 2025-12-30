/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024-2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#include "Labels_micronetkws.hpp"   /* For MicroNetKws label strings. */
#include "Labels_wav2letter.hpp"    /* For Wav2Letter label strings. */
#include "mlek/use_case/kws/KwsClassifier.hpp"        /* KWS classifier. */
#include "mlek/use_case/asr/AsrClassifier.hpp"        /* ASR classifier. */
#include "mlek/fwk/tflm/MicroNetKwsModel.hpp"     /* KWS model class for running inference. */
#include "mlek/fwk/tflm/Wav2LetterModel.hpp"      /* ASR model class for running inference. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "mlek/log/log_macros.h"             /* Logging functions */
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
    static uint8_t activationBuf[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
} /* namespace app */
} /* namespace arm */

/** @brief   Verify input and output tensor are of certain min dimensions. */
static bool VerifyTensorDimensions(const arm::app::fwk::iface::Model& model);

void MainLoop()
{
    /* Model wrapper objects. */
    arm::app::fwk::tflm::MicroNetKwsModel kwsModel;
    arm::app::fwk::tflm::Wav2LetterModel asrModel;

    arm::app::fwk::iface::MemoryRegion kwsModelMem{arm::app::kws::GetModelPointer(),
                                                   arm::app::kws::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion asrModelMem{arm::app::asr::GetModelPointer(),
                                                   arm::app::asr::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::activationBuf,
                                                  sizeof(arm::app::activationBuf)};

    /* Load the model. */
    if (!kwsModel.Init(computeMem, kwsModelMem)) {
        printf_err("Failed to initialise KWS model\n");
        return;
    }

    /* Initialise the asr model using the same allocator from KWS
     * to re-use the tensor arena. */
    if (!asrModel.Init(computeMem, asrModelMem, &kwsModel.GetBackendData())) {
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
    caseContext.Set<arm::app::fwk::iface::Model&>("kwsModel", kwsModel);
    caseContext.Set<arm::app::fwk::iface::Model&>("asrModel", asrModel);
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
    arm::app::AsrClassifier asrClassifier{
        arm::app::fwk::tflm::Wav2LetterModel::ms_inputRowsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_inputColsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_outputColsIdx}; /* Classifier wrapper object. */
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
    bool executionSuccessful = ClassifyAudioHandler(caseContext);
    info("Main loop terminated %s.\n",
        executionSuccessful ? "successfully" : "with failure");
}

static bool VerifyTensorDimensions(const arm::app::fwk::iface::Model& model)
{
    /* Populate tensor related parameters. */
    auto inputTensor = model.GetInputTensor(0);
    if (inputTensor->Shape().empty()) {
        printf_err("Invalid input tensor dims\n");
        return false;
    }
    if (inputTensor->Shape().size() < 3) {
        printf_err("Input tensor dimension should be >= 3\n");
        return false;
    }

    auto outputTensor = model.GetOutputTensor(0);
    if (outputTensor->Shape().empty()) {
        printf_err("Invalid output tensor dims\n");
        return false;
    }

    if (outputTensor->Shape().size() < 3) {
        printf_err("Output tensor dimension should be >= 3\n");
        return false;
    }

    return true;
}
