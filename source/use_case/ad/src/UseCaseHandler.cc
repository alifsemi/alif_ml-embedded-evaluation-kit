/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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
#include "AdModel.hpp"
#include "AdProcessing.hpp"
#include "AudioUtils.hpp"
#include "Classifier.hpp"
#include "ImageUtils.hpp"
#include "InputFiles.hpp"
#include "UseCaseCommonUtils.hpp"
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

    /** @brief      Given a wav file name return AD model output index.
     *  @param[in]  wavFileName Audio WAV filename.
     *                          File name should be in format anything_goes_XX_here.wav
     *                          where XX is the machine ID e.g. 00, 02, 04 or 06
     *  @return     AD model output index as 8 bit integer.
     **/
    static int8_t OutputIndexFromFileName(std::string wavFileName);

    /* Anomaly Detection inference handler */
    bool ClassifyVibrationHandler(ApplicationContext& ctx, uint32_t clipIndex, bool runAll)
    {
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        auto& model = ctx.Get<Model&>("model");

        /* If the request has a valid size, set the audio index */
        if (clipIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, clipIndex, "clipIndex")) {
                return false;
            }
        }
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto& profiler                = ctx.Get<Profiler&>("profiler");
        const auto melSpecFrameLength = ctx.Get<uint32_t>("frameLength");
        const auto melSpecFrameStride = ctx.Get<uint32_t>("frameStride");
        const auto scoreThreshold     = ctx.Get<float>("scoreThreshold");
        const auto trainingMean       = ctx.Get<float>("trainingMean");
        auto startClipIdx             = ctx.Get<uint32_t>("clipIndex");

        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        TfLiteTensor* inputTensor  = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        }

        AdPreProcess preProcess{inputTensor, melSpecFrameLength, melSpecFrameStride, trainingMean};

        AdPostProcess postProcess{outputTensor};

        do {
            hal_lcd_clear(COLOR_BLACK);

            auto currentIndex = ctx.Get<uint32_t>("clipIndex");

            /* Get the output index to look at based on id in the filename. */
            int8_t machineOutputIndex = OutputIndexFromFileName(GetFilename(currentIndex));
            if (machineOutputIndex == -1) {
                return false;
            }

            /* Creating a sliding window through the whole audio clip. */
            auto audioDataSlider =
                audio::SlidingWindow<const int16_t>(GetAudioArray(currentIndex),
                                                    GetAudioArraySize(currentIndex),
                                                    preProcess.GetAudioWindowSize(),
                                                    preProcess.GetAudioDataStride());

            /* Result is an averaged sum over inferences. */
            float result = 0;

            /* Display message on the LCD - inference running. */
            std::string str_inf{"Running inference... "};
            hal_lcd_display_text(
                str_inf.c_str(), str_inf.size(), dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            info("Running inference on audio clip %" PRIu32 " => %s\n",
                 currentIndex,
                 GetFilename(currentIndex));

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

            IncrementAppCtxIfmIdx(ctx, "clipIndex");

        } while (runAll && ctx.Get<uint32_t>("clipIndex") != startClipIdx);

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

    static int8_t OutputIndexFromFileName(std::string wavFileName)
    {
        /* Filename is assumed in the form machine_id_00.wav */
        std::string delimiter = "_"; /* First character used to split the file name up. */
        size_t delimiterStart;
        std::string subString;
        size_t machineIdxInString =
            3; /* Which part of the file name the machine id should be at. */

        for (size_t i = 0; i < machineIdxInString; ++i) {
            delimiterStart = wavFileName.find(delimiter);
            subString      = wavFileName.substr(0, delimiterStart);
            wavFileName.erase(0, delimiterStart + delimiter.length());
        }

        /* At this point substring should be 00.wav */
        delimiter      = "."; /* Second character used to split the file name up. */
        delimiterStart = subString.find(delimiter);
        subString =
            (delimiterStart != std::string::npos) ? subString.substr(0, delimiterStart) : subString;

        auto is_number = [](const std::string& str) -> bool {
            std::string::const_iterator it = str.begin();
            while (it != str.end() && std::isdigit(*it))
                ++it;
            return !str.empty() && it == str.end();
        };

        const int8_t machineIdx = is_number(subString) ? std::stoi(subString) : -1;

        /* Return corresponding index in the output vector. */
        if (machineIdx == 0) {
            return 0;
        } else if (machineIdx == 2) {
            return 1;
        } else if (machineIdx == 4) {
            return 2;
        } else if (machineIdx == 6) {
            return 3;
        } else {
            printf_err("%d is an invalid machine index \n", machineIdx);
            return -1;
        }
    }

} /* namespace app */
} /* namespace arm */
