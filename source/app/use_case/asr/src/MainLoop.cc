/*
 * SPDX-FileCopyrightText: Copyright 2021, 2024-2025 Arm Limited and/or its
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
#include "Labels.hpp"                /* For label strings. */
#include "UseCaseHandler.hpp"        /* Handlers for different user options. */
#include "mlek/log/log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */
#include "UseCaseCommonUtils.hpp"    /* Utils functions. */

/** Based on ML framework, set up the model namespace. */
#if defined(MLEK_FWK_TFLM)
#include "mlek/fwk/tflm/Wav2LetterModel.hpp"       /* Model class for running inference. */
#include "mlek/use_case/asr/AsrClassifier.hpp"         /* Classifier. */

using AsrModel = arm::app::fwk::tflm::Wav2LetterModel;
#elif defined(MLEK_FWK_EXECUTORCH)
#include "mlek/fwk/executorch/ConformerModel.hpp"

using AsrModel = arm::app::fwk::et::ConformerModel;
#endif /** MLEK_FWK_TFLM or MLEK_FWK_EXECUTORCH */

namespace arm {
namespace app {
    static uint8_t activationBuf[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

    namespace asr {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace asr */

} /* namespace app */
} /* namespace arm */

/** @brief   Verify input and output tensor are of certain min dimensions. */
static bool VerifyTensorDimensions(const arm::app::fwk::iface::Model& model);

void MainLoop()
{
    AsrModel model; /* Model wrapper object. */

    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::asr::GetModelPointer(),
                                                arm::app::asr::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::activationBuf,
                                                  sizeof(arm::app::activationBuf)};

    /* Load the model. */
    if (!model.Init(computeMem, modelMem)) {
        printf_err("Failed to initialise model\n");
        return;
    }

    if (!VerifyTensorDimensions(model)) {
        printf_err("Model's input or output dimension verification failed\n");
        return;
    }
    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;
    std::vector <std::string> labels;
    GetLabelsVector(labels);

    arm::app::Profiler profiler{"asr"};

#if defined(MLEK_FWK_TFLM)
    arm::app::AsrClassifier classifier{
        arm::app::fwk::tflm::Wav2LetterModel::ms_inputRowsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_inputColsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_outputColsIdx}; /* Classifier wrapper object. */
    caseContext.Set<uint32_t>("frameLength", arm::app::asr::g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", arm::app::asr::g_FrameStride);
    caseContext.Set<float>("scoreThreshold", arm::app::asr::g_ScoreThreshold);  /* Score threshold. */
    caseContext.Set<uint32_t>("ctxLen", arm::app::asr::g_ctxLen);  /* Left and right context length (MFCC feat vectors). */
    caseContext.Set<arm::app::AsrClassifier&>("classifier", classifier);
#elif defined(MLEK_FWK_EXECUTORCH)
    caseContext.Set<uint32_t>("melSpecWindowSize", arm::app::asr::g_melSpecWindowSize);
    caseContext.Set<uint32_t>("melSpecHopSize", arm::app::asr::g_melSpecHopSize);
    caseContext.Set<uint32_t>("chunkSize", arm::app::asr::g_chunkSize);
#endif /** MLEK_FWK_TFLM or MLEK_FWK_EXECUTORCH */
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::fwk::iface::Model&>("model", model);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);

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
