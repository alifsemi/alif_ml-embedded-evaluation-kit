/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024 Arm Limited and/or its
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
#include "UseCaseHandler.hpp"

#include "AdMelSpectrogram.hpp"
#include "AdProcessing.hpp"
#include "AudioUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "ImageUtils.hpp"
#include "hal.h"
#include "log_macros.h"

namespace arm {
namespace app {

    /**
     * @brief           Presents inference results using the data presentation
     *                  object.
     * @param[in]       result      average sum of classification results
     * @param[in]       threshold   if larger than this value we have an anomaly
     * @return          true if successful, false otherwise
     **/
    static bool PresentInferenceResult(float result, float threshold);

    /* Anomaly Detection inference handler */
    bool ClassifyVibrationHandler(ApplicationContext& ctx)
    {
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        auto& model = ctx.Get<Model&>("model");
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto& profiler                = ctx.Get<Profiler&>("profiler");
        const auto melSpecFrameLength = ctx.Get<uint32_t>("frameLength");
        const auto melSpecFrameStride = ctx.Get<uint32_t>("frameStride");
        const auto scoreThreshold     = ctx.Get<float>("scoreThreshold");
        const auto trainingMean       = ctx.Get<float>("trainingMean");

        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        TfLiteTensor* inputTensor  = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        }

        AdPreProcess preProcess{inputTensor, melSpecFrameLength, melSpecFrameStride, trainingMean};
        AdPostProcess postProcess{outputTensor};
        uint32_t machineOutputIndex = 0; /* default sample */

        hal_audio_init();
        if (!hal_audio_configure(HAL_AUDIO_MODE_SINGLE_BURST,
                                 HAL_AUDIO_FORMAT_16KHZ_MONO_16BIT)) {
            printf_err("Failed to configure audio\n");
            return false;
        }

        while (true) {
            hal_lcd_clear(COLOR_BLACK);
            uint32_t nElements = 0;
            hal_audio_start();
            auto audioData = hal_audio_get_captured_frame(&nElements);
            if (!nElements || !audioData) {
                debug("End of stream\n");
                break;
            }

            /* Creating a sliding window through the whole audio clip. */
            auto audioDataSlider =
                audio::SlidingWindow<const int16_t>(audioData,
                                                    nElements,
                                                    preProcess.GetAudioWindowSize(),
                                                    preProcess.GetAudioDataStride());

            /* Result is an averaged sum over inferences. */
            float result = 0;

            /* Display message on the LCD - inference running. */
            std::string str_inf{"Running inference... "};
            hal_lcd_display_text(
                str_inf.c_str(), str_inf.size(), dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            /* Start sliding through audio clip. */
            while (audioDataSlider.HasNext()) {
                const int16_t* inferenceWindow = audioDataSlider.Next();

                preProcess.SetAudioWindowIndex(audioDataSlider.Index());
                preProcess.DoPreProcess(inferenceWindow, preProcess.GetAudioWindowSize());

                info("Inference %zu/%zu\n",
                     audioDataSlider.Index() + 1,
                     audioDataSlider.TotalStrides() + 1);

                /* Run inference over this audio clip sliding window */
                if (!RunInference(model, profiler)) {
                    return false;
                }

                postProcess.DoPostProcess();
                result += 0 - postProcess.GetOutputValue(machineOutputIndex);

#if VERIFY_TEST_OUTPUT
                DumpTensor(outputTensor);
#endif        /* VERIFY_TEST_OUTPUT */
            } /* while (audioDataSlider.HasNext()) */

            /* Use average over whole clip as final score. */
            result /= (audioDataSlider.TotalStrides() + 1);

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            hal_lcd_display_text(
                str_inf.c_str(), str_inf.size(), dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            ctx.Set<float>("result", result);
            if (!PresentInferenceResult(result, scoreThreshold)) {
                return false;
            }

            profiler.PrintProfilingResult();
        }

        return true;
    }

    static bool PresentInferenceResult(float result, float threshold)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 30;
        constexpr uint32_t dataPsnTxtYIncr   = 16; /* Row index increment */

        hal_lcd_set_text_color(COLOR_GREEN);

        /* Display each result */
        uint32_t rowIdx1 = dataPsnTxtStartY1 + 2 * dataPsnTxtYIncr;

        std::string anomalyScore =
            std::string{"Average anomaly score is: "} + std::to_string(result);
        std::string anomalyThreshold =
            std::string("Anomaly threshold is: ") + std::to_string(threshold);

        std::string anomalyResult;
        if (result > threshold) {
            anomalyResult += std::string("Anomaly detected!");
        } else {
            anomalyResult += std::string("Everything fine, no anomaly detected!");
        }

        hal_lcd_display_text(
            anomalyScore.c_str(), anomalyScore.size(), dataPsnTxtStartX1, rowIdx1, false);

        info("%s\n", anomalyScore.c_str());
        info("%s\n", anomalyThreshold.c_str());
        info("%s\n", anomalyResult.c_str());

        return true;
    }
} /* namespace app */
} /* namespace arm */
