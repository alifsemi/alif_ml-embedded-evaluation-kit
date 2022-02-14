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

#include "InputFiles.hpp"
#include "AsrClassifier.hpp"
#include "Wav2LetterModel.hpp"
#include "hal.h"
#include "Wav2LetterMfcc.hpp"
#include "AudioUtils.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "AsrResult.hpp"
#include "Wav2LetterPreprocess.hpp"
#include "Wav2LetterPostprocess.hpp"
#include "OutputDecode.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    /**
     * @brief           Presents inference results using the data presentation
     *                  object.
     * @param[in]       platform    Reference to the hal platform object.
     * @param[in]       results     Vector of classification results to be displayed.
     * @return          true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(
                    hal_platform& platform,
                    const std::vector<arm::app::asr::AsrResult>& results);

    /* Audio inference classification handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx, uint32_t clipIndex, bool runAll)
    {
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        auto& platform = ctx.Get<hal_platform&>("platform");
        platform.data_psn->clear(COLOR_BLACK);

        auto& profiler = ctx.Get<Profiler&>("profiler");

        /* If the request has a valid size, set the audio index. */
        if (clipIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, clipIndex,"clipIndex")) {
                return false;
            }
        }

        /* Get model reference. */
        auto& model = ctx.Get<Model&>("model");
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        /* Get score threshold to be applied for the classifier (post-inference). */
        auto scoreThreshold = ctx.Get<float>("scoreThreshold");

        /* Get tensors. Dimensions of the tensor should have been verified by
         * the callee. */
        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        const uint32_t inputRows = inputTensor->dims->data[arm::app::Wav2LetterModel::ms_inputRowsIdx];

        /* Populate MFCC related parameters. */
        auto mfccParamsWinLen = ctx.Get<uint32_t>("frameLength");
        auto mfccParamsWinStride = ctx.Get<uint32_t>("frameStride");

        /* Populate ASR inference context and inner lengths for input. */
        auto inputCtxLen = ctx.Get<uint32_t>("ctxLen");
        const uint32_t inputInnerLen = inputRows - (2 * inputCtxLen);

        /* Audio data stride corresponds to inputInnerLen feature vectors. */
        const uint32_t audioParamsWinLen = (inputRows - 1) * mfccParamsWinStride + (mfccParamsWinLen);
        const uint32_t audioParamsWinStride = inputInnerLen * mfccParamsWinStride;
        const float audioParamsSecondsPerSample = (1.0/audio::Wav2LetterMFCC::ms_defaultSamplingFreq);

        /* Get pre/post-processing objects. */
        auto& prep = ctx.Get<audio::asr::Preprocess&>("preprocess");
        auto& postp = ctx.Get<audio::asr::Postprocess&>("postprocess");

        /* Set default reduction axis for post-processing. */
        const uint32_t reductionAxis = arm::app::Wav2LetterModel::ms_outputRowsIdx;

        /* Audio clip start index. */
        auto startClipIdx = ctx.Get<uint32_t>("clipIndex");

        /* Loop to process audio clips. */
        do {
            platform.data_psn->clear(COLOR_BLACK);

            /* Get current audio clip index. */
            auto currentIndex = ctx.Get<uint32_t>("clipIndex");

            /* Get the current audio buffer and respective size. */
            const int16_t* audioArr = get_audio_array(currentIndex);
            const uint32_t audioArrSize = get_audio_array_size(currentIndex);

            if (!audioArr) {
                printf_err("Invalid audio array pointer\n");
                return false;
            }

            /* Audio clip must have enough samples to produce 1 MFCC feature. */
            if (audioArrSize < mfccParamsWinLen) {
                printf_err("Not enough audio samples, minimum needed is %" PRIu32 "\n",
                    mfccParamsWinLen);
                return false;
            }

            /* Initialise an audio slider. */
            auto audioDataSlider = audio::FractionalSlidingWindow<const int16_t>(
                                        audioArr,
                                        audioArrSize,
                                        audioParamsWinLen,
                                        audioParamsWinStride);

            /* Declare a container for results. */
            std::vector<arm::app::asr::AsrResult> results;

            /* Display message on the LCD - inference running. */
            std::string str_inf{"Running inference... "};
            platform.data_psn->present_data_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            info("Running inference on audio clip %" PRIu32 " => %s\n", currentIndex,
                 get_filename(currentIndex));

            size_t inferenceWindowLen = audioParamsWinLen;

            /* Start sliding through audio clip. */
            while (audioDataSlider.HasNext()) {

                /* If not enough audio see how much can be sent for processing. */
                size_t nextStartIndex = audioDataSlider.NextWindowStartIndex();
                if (nextStartIndex + audioParamsWinLen > audioArrSize) {
                    inferenceWindowLen = audioArrSize - nextStartIndex;
                }

                const int16_t* inferenceWindow = audioDataSlider.Next();

                info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
                     static_cast<size_t>(ceilf(audioDataSlider.FractionalTotalStrides() + 1)));

                /* Calculate MFCCs, deltas and populate the input tensor. */
                prep.Invoke(inferenceWindow, inferenceWindowLen, inputTensor);

                /* Run inference over this audio clip sliding window. */
                if (!RunInference(model, profiler)) {
                    return false;
                }

                /* Post-process. */
                postp.Invoke(outputTensor, reductionAxis, !audioDataSlider.HasNext());

                /* Get results. */
                std::vector<ClassificationResult> classificationResult;
                auto& classifier = ctx.Get<AsrClassifier&>("classifier");
                classifier.GetClassificationResults(
                            outputTensor, classificationResult,
                            ctx.Get<std::vector<std::string>&>("labels"), 1);

                results.emplace_back(asr::AsrResult(classificationResult,
                                                    (audioDataSlider.Index() *
                                                    audioParamsSecondsPerSample *
                                                    audioParamsWinStride),
                                                    audioDataSlider.Index(), scoreThreshold));

#if VERIFY_TEST_OUTPUT
                arm::app::DumpTensor(outputTensor,
                    outputTensor->dims->data[arm::app::Wav2LetterModel::ms_outputColsIdx]);
#endif /* VERIFY_TEST_OUTPUT */

            }

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            platform.data_psn->present_data_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            ctx.Set<std::vector<arm::app::asr::AsrResult>>("results", results);

            if (!PresentInferenceResult(platform, results)) {
                return false;
            }

            profiler.PrintProfilingResult();

            IncrementAppCtxIfmIdx(ctx,"clipIndex");

        } while (runAll && ctx.Get<uint32_t>("clipIndex") != startClipIdx);

        return true;
    }


    static bool PresentInferenceResult(hal_platform& platform,
                                       const std::vector<arm::app::asr::AsrResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 60;
        constexpr bool allow_multiple_lines = true;

        platform.data_psn->set_text_color(COLOR_GREEN);

        info("Final results:\n");
        info("Total number of inferences: %zu\n", results.size());
        /* Results from multiple inferences should be combined before processing. */
        std::vector<arm::app::ClassificationResult> combinedResults;
        for (auto& result : results) {
            combinedResults.insert(combinedResults.end(),
                                   result.m_resultVec.begin(),
                                   result.m_resultVec.end());
        }

        /* Get each inference result string using the decoder. */
        for (const auto & result : results) {
            std::string infResultStr = audio::asr::DecodeOutput(result.m_resultVec);

            info("For timestamp: %f (inference #: %" PRIu32 "); label: %s\n",
                 result.m_timeStamp, result.m_inferenceNumber,
                 infResultStr.c_str());
        }

        /* Get the decoded result for the combined result. */
        std::string finalResultStr = audio::asr::DecodeOutput(combinedResults);

        platform.data_psn->present_data_text(
                            finalResultStr.c_str(), finalResultStr.size(),
                            dataPsnTxtStartX1, dataPsnTxtStartY1,
                            allow_multiple_lines);

        info("Complete recognition: %s\n", finalResultStr.c_str());
        return true;
    }

} /* namespace app */
} /* namespace arm */