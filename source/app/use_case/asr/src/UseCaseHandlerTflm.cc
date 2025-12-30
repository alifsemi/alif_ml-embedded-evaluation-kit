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
#if defined(MLEK_FWK_TFLM)

#include "UseCaseHandler.hpp"

#include "mlek/use_case/asr/AsrClassifier.hpp"
#include "mlek/use_case/asr/AsrResult.hpp"
#include "mlek/common/AudioSlidingWindow.hpp"
#include "mlek/common/ImageUtils.hpp"
#include "mlek/use_case/asr/OutputDecode.hpp"
#include "UseCaseCommonUtils.hpp"
#include "mlek/fwk/tflm/Wav2LetterModel.hpp"
#include "mlek/use_case/asr/Wav2LetterPostprocess.hpp"
#include "mlek/use_case/asr/Wav2LetterPreprocess.hpp"
#include "hal.h"
#include "mlek/log/log_macros.h"

namespace arm {
namespace app {

    /**
     * @brief       Presents ASR inference results.
     * @param[in]   results   Vector of ASR classification results to be displayed.
     * @return      true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(const std::vector<asr::AsrResult>& results);

    /* ASR inference handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx)
    {
        auto& model          = ctx.Get<fwk::iface::Model&>("model");
        auto& profiler       = ctx.Get<Profiler&>("profiler");
        auto mfccFrameLen    = ctx.Get<uint32_t>("frameLength");
        auto mfccFrameStride = ctx.Get<uint32_t>("frameStride");
        auto scoreThreshold  = ctx.Get<float>("scoreThreshold");
        auto inputCtxLen     = ctx.Get<uint32_t>("ctxLen");
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        const auto inputTensor  = model.GetInputTensor(0);
        const auto outputTensor = model.GetOutputTensor(0);

        /* Get input shape. Dimensions of the tensor should have been verified by
         * the callee. */
        auto inputShape = model.GetInputShape(0);

        const uint32_t inputRowsSize = inputShape[fwk::tflm::Wav2LetterModel::ms_inputRowsIdx];
        const uint32_t inputInnerLen = inputRowsSize - (2 * inputCtxLen);

        /* Audio data stride corresponds to inputInnerLen feature vectors. */
        const uint32_t audioDataWindowLen = (inputRowsSize - 1) * mfccFrameStride + (mfccFrameLen);
        const uint32_t audioDataWindowStride = inputInnerLen * mfccFrameStride;

        /* NOTE: This is only used for time stamp calculation. */
        const float secondsPerSample = (1.0 / audio::Wav2LetterMFCC::ms_defaultSamplingFreq);

        /* Set up pre and post-processing objects. */
        AsrPreProcess preProcess =
            AsrPreProcess(inputTensor,
                          fwk::tflm::Wav2LetterModel::ms_numMfccFeatures,
                          inputShape[fwk::tflm::Wav2LetterModel::ms_inputRowsIdx],
                          mfccFrameLen,
                          mfccFrameStride);

        std::vector<ClassificationResult> singleInfResult;
        AsrPostProcess postProcess = AsrPostProcess(model,
                                                    ctx.Get<AsrClassifier&>("classifier"),
                                                    ctx.Get<std::vector<std::string>&>("labels"),
                                                    singleInfResult,
                                                    inputCtxLen,
                                                    fwk::tflm::Wav2LetterModel::ms_blankTokenIdx,
                                                    fwk::tflm::Wav2LetterModel::ms_outputRowsIdx);

        hal_audio_init();
        if (!hal_audio_configure(HAL_AUDIO_MODE_SINGLE_BURST,
                                 HAL_AUDIO_FORMAT_16KHZ_MONO_16BIT)) {
            printf_err("Failed to configure audio\n");
            return false;
        }

        /* Loop to process audio clips. */
        while (true) {
#ifdef INTERACTIVE_MODE
            AwaitUserInput(); // Wait for user input before moving forward.
#endif /* INTERACTIVE_MODE */

            hal_display_clear(COLOR_BLACK);

            /* Get the current audio buffer and respective size. */
            uint32_t audioArrSize = 0;
            hal_audio_start();
            auto audioArr = hal_audio_get_captured_frame(&audioArrSize);
            if (!audioArrSize || !audioArr) {
                debug("End of stream\n");
                break;
            }

            /* Audio clip needs enough samples to produce at least 1 MFCC feature. */
            if (audioArrSize < mfccFrameLen) {
                printf_err("Not enough audio samples, minimum needed is %" PRIu32 "\n",
                           mfccFrameLen);
                return false;
            }

            /* Creating a sliding window through the whole audio clip. */
            auto audioDataSlider = audio::FractionalSlidingWindow<const int16_t>(
                audioArr, audioArrSize, audioDataWindowLen, audioDataWindowStride);

            /* Declare a container for final results. */
            std::vector<asr::AsrResult> finalResults;

            /* Display message on the LCD - inference running. */
            std::string str_inf{"Running inference... "};
            hal_display_show_text(
                str_inf.c_str(), str_inf.size(), dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            size_t inferenceWindowLen = audioDataWindowLen;

            /* Start sliding through audio clip. */
            while (audioDataSlider.HasNext()) {

                /* If not enough audio, see how much can be sent for processing. */
                size_t nextStartIndex = audioDataSlider.NextWindowStartIndex();
                if (nextStartIndex + audioDataWindowLen > audioArrSize) {
                    inferenceWindowLen = audioArrSize - nextStartIndex;
                }

                const int16_t* inferenceWindow = audioDataSlider.Next();

                info("Inference %zu/%zu\n",
                     audioDataSlider.Index() + 1,
                     static_cast<size_t>(ceilf(audioDataSlider.FractionalTotalStrides() + 1)));

                /* Run the pre-processing, inference and post-processing. */
                if (!preProcess.DoPreProcess(inferenceWindow, inferenceWindowLen)) {
                    printf_err("Pre-processing failed.");
                    return false;
                }

                if (!RunInference(model, profiler)) {
                    printf_err("Inference failed.");
                    return false;
                }

                /* Post processing needs to know if we are on the last audio window. */
                postProcess.m_lastIteration = !audioDataSlider.HasNext();
                if (!postProcess.DoPostProcess()) {
                    printf_err("Post-processing failed.");
                    return false;
                }

                /* Add results from this window to our final results vector. */
                finalResults.emplace_back(asr::AsrResult(
                    singleInfResult,
                    (audioDataSlider.Index() * secondsPerSample * audioDataWindowStride),
                    audioDataSlider.Index(),
                    scoreThreshold));

#if VERIFY_TEST_OUTPUT
                armDumpTensor(outputTensor,
                              outputTensor->dims->data[Wav2LetterModel::ms_outputColsIdx]);
#endif        /* VERIFY_TEST_OUTPUT */
            } /* while (audioDataSlider.HasNext()) */

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            hal_display_show_text(
                str_inf.c_str(), str_inf.size(), dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            ctx.Set<std::vector<asr::AsrResult>>("results", finalResults);

            if (!PresentInferenceResult(finalResults)) {
                return false;
            }

            profiler.PrintProfilingResult();
        }

        return true;
    }

    static bool PresentInferenceResult(const std::vector<asr::AsrResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 60;
        constexpr bool allow_multiple_lines  = true;

        hal_display_set_text_color(COLOR_GREEN);

        info("Final results:\n");
        info("Total number of inferences: %zu\n", results.size());
        /* Results from multiple inferences should be combined before processing. */
        std::vector<ClassificationResult> combinedResults;
        for (const auto& result : results) {
            combinedResults.insert(
                combinedResults.end(), result.m_resultVec.begin(), result.m_resultVec.end());
        }

        /* Get each inference result string using the decoder. */
        for (const auto& result : results) {
            std::string infResultStr = audio::asr::DecodeOutput(result.m_resultVec);

            info("For timestamp: %f (inference #: %" PRIu32 "); label: %s\n",
                 result.m_timeStamp,
                 result.m_inferenceNumber,
                 infResultStr.c_str());
        }

        /* Get the decoded result for the combined result. */
        std::string finalResultStr = audio::asr::DecodeOutput(combinedResults);

        hal_display_show_text(finalResultStr.c_str(),
                              finalResultStr.size(),
                              dataPsnTxtStartX1,
                              dataPsnTxtStartY1,
                              allow_multiple_lines);

        info("Complete recognition: %s\n", finalResultStr.c_str());
        return true;
    }

} /* namespace app */
} /* namespace arm */

#endif /* defined(MLEK_FWK_TFLM) */
