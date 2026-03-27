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

#include "mlek/use_case/ad/AdMelSpectrogram.hpp"
#include "mlek/fwk/tflm/AdModel.hpp"
#include "mlek/use_case/ad/AdProcessing.hpp"
#include "mlek/common/AudioSlidingWindow.hpp"
#include "mlek/common/ImageUtils.hpp"
#include "hal.h"
#include "UseCaseCommonUtils.hpp"
#include "mlek/log/log_macros.h"

#include "services_lib_api.h"
#include "services_main.h"
#include "timer_alif.h"

#if defined(GPIO_PROFILING)
#include "board_utils.h"
#endif

#include <atomic>
#include <vector>


using arm::app::Profiler;
using arm::app::ClassificationResult;
using arm::app::ApplicationContext;
using arm::app::fwk::iface::Model;
using arm::app::AdPreProcess;
using arm::app::AdPostProcess;
using arm::app::fwk::tflm::AdModel;


#define AUDIO_SAMPLES 33280
#define AUDIO_STRIDE 10240
#define NEW_SAMPLE_START (AUDIO_SAMPLES - AUDIO_STRIDE)
#define RESULTS_MEMORY 8

static int16_t audio_inf[AUDIO_SAMPLES + AUDIO_STRIDE];


namespace alif {
namespace app {

namespace audio {
using namespace arm::app::audio;
}

namespace ad {
using namespace arm::app::ad;
}

    /**
     * @brief           Presents inference results using the data presentation
     *                  object.
     * @param[in]       avg_result      average sum of classification results
     * @param[in]       threshold   if larger than this value we have an anomaly
     * @return          true if successful, false otherwise
     **/
    static bool PresentInferenceResult(float result, float threshold);

    /* Anomaly Detection inference handler */
    bool ClassifyVibrationHandler(ApplicationContext& ctx)
    {
        auto& model = ctx.Get<Model&>("model");
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto index = ctx.Get<int>("index");
        auto result = ctx.Get<float>("result");
        const auto scoreThreshold = ctx.Get<float>("scoreThreshold");
        const auto melSpecFrameLength = ctx.Get<uint32_t>("frameLength");
        const auto melSpecFrameStride = ctx.Get<uint32_t>("frameStride");
        const auto trainingMean = ctx.Get<float>("trainingMean");
        const auto machineGain = ctx.Get<float>("machineGain");

        auto outputTensor = model.GetOutputTensor(0);
        auto inputTensor  = model.GetInputTensor(0);

        if (inputTensor->Shape().empty()) {
            printf_err("Invalid input tensor dims\n");
            return false;
        }

        AdPreProcess preProcess{inputTensor,
                        AdModel::ms_inputRowsIdx,
                        AdModel::ms_inputColsIdx,
                        melSpecFrameLength,
                        melSpecFrameStride,
                        trainingMean};

        AdPostProcess postProcess{outputTensor};

        int err;

        if (index == 0) {
            err = hal_audio_alif_init(audio::AdMelSpectrogram::ms_defaultSamplingFreq);
            if (err) {
                printf_err("hal_audio_alif_init failed with error: %d\n", err);
                return false;
            }
            // Start first fill of final stride section of buffer
            hal_get_audio_data(audio_inf + AUDIO_SAMPLES, AUDIO_STRIDE);
        }

        // Wait until stride buffer is full - initiated above or by previous call to ClassifyAudioHandler
        err = hal_wait_for_audio();
        if (err) {
            printf_err("hal_get_audio_data failed with error: %d\n", err);
            return false;
        }

        // move buffer down by one stride, clearing space at the end for the next stride
        memmove(audio_inf, audio_inf + AUDIO_STRIDE, AUDIO_SAMPLES * sizeof audio_inf[0]);

        // start receiving the next stride immediately before we start heavy processing, so as not to lose anything
        hal_get_audio_data(audio_inf + AUDIO_SAMPLES, AUDIO_STRIDE);
        hal_set_audio_gain(machineGain);
        hal_audio_alif_preprocessing(audio_inf + AUDIO_SAMPLES - AUDIO_STRIDE, AUDIO_STRIDE);

        const int16_t* inferenceWindow = audio_inf;

#if defined(GPIO_PROFILING)
        BOARD_LED1_BLUE_Control(BOARD_LED_STATE_TOGGLE);
#else
        uint32_t start = Get_SysTick_Cycle_Count32();
#endif

        /* Run the pre-processing, inference and post-processing. */
        if (!preProcess.DoPreProcess(inferenceWindow, preProcess.GetAudioWindowSize())) {
            printf_err("Pre-processing failed.");
            return false;
        }
#ifndef GPIO_PROFILING
        printf("Preprocessing time = %.3f ms\n", (double) (Get_SysTick_Cycle_Count32() - start) / SystemCoreClock * 1000);
        start = Get_SysTick_Cycle_Count32();
#endif

        if (!RunInference(model, profiler)) {
            printf_err("Inference failed.");
            return false;
        }

#ifndef GPIO_PROFILING
        printf("Inference time = %.3f ms\n", (double) (Get_SysTick_Cycle_Count32() - start) / SystemCoreClock * 1000);
        start = Get_SysTick_Cycle_Count32();
#endif

        if (!postProcess.DoPostProcess()) {
            printf_err("Post-processing failed.");
            return false;
        }

#if defined(GPIO_PROFILING)
        BOARD_LED1_BLUE_Control(BOARD_LED_STATE_TOGGLE);
        ctx.Set<int>("index", index + 1); /* Increment the audio window index for the next inference call so that audio is not initialized again */
#else
        printf("Postprocessing time = %.3f ms\n", (double) (Get_SysTick_Cycle_Count32() - start) / SystemCoreClock * 1000);

        /* Add results from this window to our final results vector. */
        result += 0 - postProcess.GetOutputValue(0);

    #if VERIFY_TEST_OUTPUT
        DumpTensor(outputTensor);
    #endif /* VERIFY_TEST_OUTPUT */

        if (!PresentInferenceResult(result / (index+1), scoreThreshold)) {
             return false;
        }
        ctx.Set<int>("index", index + 1);
        ctx.Set<float>("result", result);

        profiler.PrintProfilingResult();
#endif
        return true;
   }

    static bool PresentInferenceResult(float result, float threshold)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 10;
        constexpr uint32_t dataPsnTxtStartY1 = 30;
        constexpr uint32_t dataPsnTxtYIncr   = 16; /* Row index increment */

        hal_display_set_text_color(COLOR_GREEN);

        /* Display each result */
        uint32_t rowIdx1 = dataPsnTxtStartY1 + 2 * dataPsnTxtYIncr;
        uint32_t rowIdx2 = dataPsnTxtStartY1 + 15 + 2 * dataPsnTxtYIncr;
        uint32_t rowIdx3 = dataPsnTxtStartY1 + 30 + 2 * dataPsnTxtYIncr;

        std::string anomalyScore = std::string{"Average anomaly score: "} + std::to_string(result);
        std::string anomalyThreshold = std::string("Anomaly threshold: ") + std::to_string(threshold);

        std::string anomalyResult;
        if (result > threshold) {
            anomalyResult += std::string("Anomaly detected!");
        } else {
            anomalyResult += std::string("Everything fine, no anomaly!");
        }

        hal_display_show_text(
                anomalyThreshold.c_str(), anomalyThreshold.size(),
                dataPsnTxtStartX1, rowIdx1, false);
        hal_display_show_text(
                anomalyScore.c_str(), anomalyScore.size(),
                dataPsnTxtStartX1, rowIdx2, false);
        hal_display_show_text(
                anomalyResult.c_str(), anomalyResult.size(),
                dataPsnTxtStartX1, rowIdx3, false);

        info("%s\n", anomalyScore.c_str());
        info("%s\n", anomalyThreshold.c_str());
        info("%s\n", anomalyResult.c_str());

        return true;
    }

} /* namespace app */
} /* namespace alif */
