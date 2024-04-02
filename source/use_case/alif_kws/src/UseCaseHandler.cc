/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2023-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
#include "UseCaseHandler.hpp"

#include "KwsClassifier.hpp"
#include "MicroNetKwsModel.hpp"
#include "hal.h"
#include "timer_ensemble.h"
#include "AudioUtils.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "KwsResult.hpp"
#include "log_macros.h"
#include "KwsProcessing.hpp"
#include "services_lib_api.h"
#include "services_main.h"

#include <vector>

extern uint32_t m55_comms_handle;
m55_data_payload_t mhu_data;

using arm::app::KwsClassifier;
using arm::app::Profiler;
using arm::app::ClassificationResult;
using arm::app::ApplicationContext;
using arm::app::Model;
using arm::app::KwsPreProcess;
using arm::app::KwsPostProcess;
using arm::app::MicroNetKwsModel;

#define AUDIO_SAMPLES 16000 // 16k samples/sec, 1sec sample
#define AUDIO_STRIDE 8000 // 0.5 seconds
#define RESULTS_MEMORY 8

static int16_t audio_inf[AUDIO_SAMPLES + AUDIO_STRIDE];

namespace alif {
namespace app {

namespace audio {
using namespace arm::app::audio;
}

namespace kws {
using namespace arm::app::kws;
}

 /**
 * @brief           Presents KWS inference results.
 * @param[in]       results     Vector of KWS classification results to be displayed.
 * @return          true if successful, false otherwise.
 **/
static bool PresentInferenceResult(const std::vector<arm::app::kws::KwsResult>& results);

static std::string last_label;

static void send_msg_if_needed(arm::app::kws::KwsResult &result)
{
    mhu_data.id = 2; // id for M55_HE
    if (result.m_resultVec.empty()) {
        last_label.clear();
        return;
    }

    arm::app::ClassificationResult classification = result.m_resultVec[0];

    if (classification.m_label != last_label) {
        if (classification.m_label == "go" || classification.m_label == "stop") {
            info("******************* send_msg_if_needed, FOUND \"%s\", copy data end send! ******************\n", classification.m_label.c_str());
            strcpy(mhu_data.msg, classification.m_label.c_str());
            __DMB();
            SERVICES_send_msg(m55_comms_handle, &mhu_data);
        }
        last_label = classification.m_label;
    }
}


    /* KWS inference handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx, bool oneshot)
    {
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model = ctx.Get<Model&>("model");
        const auto mfccFrameLength = ctx.Get<int>("frameLength");
        const auto mfccFrameStride = ctx.Get<int>("frameStride");
        const auto audioRate = ctx.Get<int>("audioRate");
        const auto scoreThreshold = ctx.Get<float>("scoreThreshold");

        constexpr int minTensorDims = static_cast<int>(
            (MicroNetKwsModel::ms_inputRowsIdx > MicroNetKwsModel::ms_inputColsIdx)?
             MicroNetKwsModel::ms_inputRowsIdx : MicroNetKwsModel::ms_inputColsIdx);

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        /* Get Input and Output tensors for pre/post processing. */
        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < minTensorDims) {
            printf_err("Input tensor dimension should be >= %d\n", minTensorDims);
            return false;
        }

        /* Get input shape for feature extraction. */
        TfLiteIntArray* inputShape = model.GetInputShape(0);
        const uint32_t numMfccFeatures = inputShape->data[MicroNetKwsModel::ms_inputColsIdx];
        const uint32_t numMfccFrames = inputShape->data[arm::app::MicroNetKwsModel::ms_inputRowsIdx];

        /* We expect to be sampling 1 second worth of data at a time.
        *  NOTE: This is only used for time stamp calculation. */
        const float secondsPerSample = 1.0f / audioRate;

        /* Set up pre and post-processing. */
        KwsPreProcess preProcess = KwsPreProcess(inputTensor, numMfccFeatures, numMfccFrames,
                                                 mfccFrameLength, mfccFrameStride);

        std::vector<ClassificationResult> singleInfResult;
        KwsPostProcess postProcess = KwsPostProcess(outputTensor, ctx.Get<KwsClassifier &>("classifier"),
                                                    ctx.Get<std::vector<std::string>&>("labels"),
                                                    singleInfResult);

        int index = 0;
        std::vector<kws::KwsResult> infResults;
        static bool audio_inited;
        if (!audio_inited) {
            int err = hal_audio_init(audioRate);
            if (err) {
                printf_err("hal_audio_init failed with error: %d\n", err);
                return false;
            }
            audio_inited = true;
        }

        // Start first fill of final stride section of buffer
        hal_get_audio_data(audio_inf + AUDIO_SAMPLES, AUDIO_STRIDE);

        do {
            // Wait until stride buffer is full - initiated above or by previous interation of loop
            int err = hal_wait_for_audio();
            if (err) {
                printf_err("hal_get_audio_data failed with error: %d\n", err);
                return false;
            }

            // move buffer down by one stride, clearing space at the end for the next stride
            std::copy(audio_inf + AUDIO_STRIDE, audio_inf + AUDIO_STRIDE + AUDIO_SAMPLES, audio_inf);

            // start receiving the next stride immediately before we start heavy processing, so as not to lose anything
            hal_get_audio_data(audio_inf + AUDIO_SAMPLES, AUDIO_STRIDE);

            hal_audio_preprocessing(audio_inf + AUDIO_SAMPLES - AUDIO_STRIDE, AUDIO_STRIDE);

            const int16_t* inferenceWindow = audio_inf;

            uint32_t start = Get_SysTick_Cycle_Count32();
            /* Run the pre-processing, inference and post-processing. */
            if (!preProcess.DoPreProcess(inferenceWindow, index)) {
                printf_err("Pre-processing failed.");
                return false;
            }
            printf("Preprocessing time = %.3f ms\n", (double) (Get_SysTick_Cycle_Count32() - start) / SystemCoreClock * 1000);

            start = Get_SysTick_Cycle_Count32();
            if (!RunInference(model, profiler)) {
                printf_err("Inference failed.");
                return false;
            }
            printf("Inference time = %.3f ms\n", (double) (Get_SysTick_Cycle_Count32() - start) / SystemCoreClock * 1000);

            start = Get_SysTick_Cycle_Count32();
            if (!postProcess.DoPostProcess()) {
                printf_err("Post-processing failed.");
                return false;
            }
            printf("Postprocessing time = %.3f ms\n", (double) (Get_SysTick_Cycle_Count32() - start) / SystemCoreClock * 1000);

            /* Add results from this window to our final results vector. */
            if (infResults.size() == RESULTS_MEMORY) {
                infResults.erase(infResults.begin());
            }
            infResults.emplace_back(kws::KwsResult(singleInfResult,
                    index * secondsPerSample * preProcess.m_audioDataStride,
                    index, scoreThreshold));

            send_msg_if_needed(infResults.back());

#if VERIFY_TEST_OUTPUT
            DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */

            hal_lcd_clear(COLOR_BLACK);

            if (!PresentInferenceResult(infResults)) {
                return false;
            }

            profiler.PrintProfilingResult();

            ++index;
        } while (!oneshot);
        return true;
    }

    static bool PresentInferenceResult(const std::vector<kws::KwsResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 30;
        constexpr uint32_t dataPsnTxtYIncr   = 16;  /* Row index increment. */

        hal_lcd_set_text_color(COLOR_GREEN);
        info("Final results:\n");
        info("Total number of inferences: %zu\n", results.size());

        /* Display each result */
        uint32_t rowIdx1 = dataPsnTxtStartY1 + 2 * dataPsnTxtYIncr;

        for (const auto& result : results) {

            std::string topKeyword{"<none>"};
            float score = 0.f;
            if (!result.m_resultVec.empty()) {
                topKeyword = result.m_resultVec[0].m_label;
                score = result.m_resultVec[0].m_normalisedVal;
            }

            std::string resultStr =
                    std::string{"@"} + std::to_string(result.m_timeStamp) +
                    std::string{"s: "} + topKeyword + std::string{" ("} +
                    std::to_string(static_cast<int>(score * 100)) + std::string{"%)"};

            hal_lcd_display_text(resultStr.c_str(), resultStr.size(),
                    dataPsnTxtStartX1, rowIdx1, false);
            rowIdx1 += dataPsnTxtYIncr;

            if (result.m_resultVec.empty()) {
                info("For timestamp: %f (inference #: %" PRIu32
                             "); label: %s; threshold: %f\n",
                     result.m_timeStamp, result.m_inferenceNumber,
                     topKeyword.c_str(),
                     result.m_threshold);
            } else {
                for (uint32_t j = 0; j < result.m_resultVec.size(); ++j) {
                    info("For timestamp: %f (inference #: %" PRIu32
                                 "); label: %s, score: %f; threshold: %f\n",
                         result.m_timeStamp,
                         result.m_inferenceNumber,
                         result.m_resultVec[j].m_label.c_str(),
                         result.m_resultVec[j].m_normalisedVal,
                         result.m_threshold);
                }
            }
        }

        return true;
    }

} /* namespace app */
} /* namespace alif */
