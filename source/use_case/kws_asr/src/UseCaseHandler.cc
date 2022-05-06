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

#include "hal.h"
#include "InputFiles.hpp"
#include "AudioUtils.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "MicroNetKwsModel.hpp"
#include "MicroNetKwsMfcc.hpp"
#include "Classifier.hpp"
#include "KwsResult.hpp"
#include "Wav2LetterModel.hpp"
#include "Wav2LetterMfcc.hpp"
#include "Wav2LetterPreprocess.hpp"
#include "Wav2LetterPostprocess.hpp"
#include "KwsProcessing.hpp"
#include "AsrResult.hpp"
#include "AsrClassifier.hpp"
#include "OutputDecode.hpp"
#include "log_macros.h"


using KwsClassifier = arm::app::Classifier;

namespace arm {
namespace app {

    struct KWSOutput {
        bool executionSuccess = false;
        const int16_t* asrAudioStart = nullptr;
        int32_t asrAudioSamples = 0;
    };

    /**
     * @brief       Presents KWS inference results.
     * @param[in]   results   Vector of KWS classification results to be displayed.
     * @return      true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(std::vector<kws::KwsResult>& results);

    /**
     * @brief       Presents ASR inference results.
     * @param[in]   results   Vector of ASR classification results to be displayed.
     * @return      true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(std::vector<asr::AsrResult>& results);

    /**
     * @brief           Performs the KWS pipeline.
     * @param[in,out]   ctx   pointer to the application context object
     * @return          struct containing pointer to audio data where ASR should begin
     *                  and how much data to process.
     **/
    static KWSOutput doKws(ApplicationContext& ctx)
    {
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& kwsModel = ctx.Get<Model&>("kwsModel");
        const auto kwsMfccFrameLength = ctx.Get<int>("kwsFrameLength");
        const auto kwsMfccFrameStride = ctx.Get<int>("kwsFrameStride");
        const auto kwsScoreThreshold = ctx.Get<float>("kwsScoreThreshold");

        auto currentIndex = ctx.Get<uint32_t>("clipIndex");

        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        constexpr int minTensorDims = static_cast<int>(
            (MicroNetKwsModel::ms_inputRowsIdx > MicroNetKwsModel::ms_inputColsIdx)?
             MicroNetKwsModel::ms_inputRowsIdx : MicroNetKwsModel::ms_inputColsIdx);

        /* Output struct from doing KWS. */
        KWSOutput output {};

        if (!kwsModel.IsInited()) {
            printf_err("KWS model has not been initialised\n");
            return output;
        }

        /* Get Input and Output tensors for pre/post processing. */
        TfLiteTensor* kwsInputTensor = kwsModel.GetInputTensor(0);
        TfLiteTensor* kwsOutputTensor = kwsModel.GetOutputTensor(0);
        if (!kwsInputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return output;
        } else if (kwsInputTensor->dims->size < minTensorDims) {
            printf_err("Input tensor dimension should be >= %d\n", minTensorDims);
            return output;
        }

        /* Get input shape for feature extraction. */
        TfLiteIntArray* inputShape = kwsModel.GetInputShape(0);
        const uint32_t numMfccFeatures = inputShape->data[MicroNetKwsModel::ms_inputColsIdx];
        const uint32_t numMfccFrames = inputShape->data[MicroNetKwsModel::ms_inputRowsIdx];

        /* We expect to be sampling 1 second worth of data at a time
         * NOTE: This is only used for time stamp calculation. */
        const float kwsAudioParamsSecondsPerSample = 1.0 / audio::MicroNetKwsMFCC::ms_defaultSamplingFreq;

        /* Set up pre and post-processing. */
        KwsPreProcess preProcess = KwsPreProcess(kwsInputTensor, numMfccFeatures, numMfccFrames,
                                                 kwsMfccFrameLength, kwsMfccFrameStride);

        std::vector<ClassificationResult> singleInfResult;
        KwsPostProcess postProcess = KwsPostProcess(kwsOutputTensor, ctx.Get<KwsClassifier &>("kwsClassifier"),
                                                    ctx.Get<std::vector<std::string>&>("kwsLabels"),
                                                    singleInfResult);

        /* Creating a sliding window through the whole audio clip. */
        auto audioDataSlider = audio::SlidingWindow<const int16_t>(
                get_audio_array(currentIndex),
                get_audio_array_size(currentIndex),
                preProcess.m_audioDataWindowSize, preProcess.m_audioDataStride);

        /* Declare a container to hold kws results from across the whole audio clip. */
        std::vector<kws::KwsResult> finalResults;

        /* Display message on the LCD - inference running. */
        std::string str_inf{"Running KWS inference... "};
        hal_lcd_display_text(
                            str_inf.c_str(), str_inf.size(),
                            dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

        info("Running KWS inference on audio clip %" PRIu32 " => %s\n",
             currentIndex, get_filename(currentIndex));

        /* Start sliding through audio clip. */
        while (audioDataSlider.HasNext()) {
            const int16_t* inferenceWindow = audioDataSlider.Next();

            /* The first window does not have cache ready. */
            preProcess.m_audioWindowIndex = audioDataSlider.Index();

            /* Run the pre-processing, inference and post-processing. */
            if (!preProcess.DoPreProcess(inferenceWindow, audio::MicroNetKwsMFCC::ms_defaultSamplingFreq)) {
                printf_err("KWS Pre-processing failed.");
                return output;
            }

            if (!RunInference(kwsModel, profiler)) {
                printf_err("KWS Inference failed.");
                return output;
            }

            if (!postProcess.DoPostProcess()) {
                printf_err("KWS Post-processing failed.");
                return output;
            }

            info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
                 audioDataSlider.TotalStrides() + 1);

            /* Add results from this window to our final results vector. */
            finalResults.emplace_back(
                    kws::KwsResult(singleInfResult,
                            audioDataSlider.Index() * kwsAudioParamsSecondsPerSample * preProcess.m_audioDataStride,
                            audioDataSlider.Index(), kwsScoreThreshold));

            /* Break out when trigger keyword is detected. */
            if (singleInfResult[0].m_label == ctx.Get<const std::string&>("triggerKeyword")
                    && singleInfResult[0].m_normalisedVal > kwsScoreThreshold) {
                output.asrAudioStart = inferenceWindow + preProcess.m_audioDataWindowSize;
                output.asrAudioSamples = get_audio_array_size(currentIndex) -
                                        (audioDataSlider.NextWindowStartIndex() -
                                        preProcess.m_audioDataStride + preProcess.m_audioDataWindowSize);
                break;
            }

#if VERIFY_TEST_OUTPUT
            DumpTensor(kwsOutputTensor);
#endif /* VERIFY_TEST_OUTPUT */

        } /* while (audioDataSlider.HasNext()) */

        /* Erase. */
        str_inf = std::string(str_inf.size(), ' ');
        hal_lcd_display_text(str_inf.c_str(), str_inf.size(),
                dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

        if (!PresentInferenceResult(finalResults)) {
            return output;
        }

        profiler.PrintProfilingResult();

        output.executionSuccess = true;
        return output;
    }

    /**
     * @brief           Performs the ASR pipeline.
     * @param[in,out]   ctx         Pointer to the application context object.
     * @param[in]       kwsOutput   Struct containing pointer to audio data where ASR should begin
     *                              and how much data to process.
     * @return          true if pipeline executed without failure.
     **/
    static bool doAsr(ApplicationContext& ctx, const KWSOutput& kwsOutput)
    {
        auto& asrModel = ctx.Get<Model&>("asrModel");
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto asrMfccFrameLen = ctx.Get<uint32_t>("asrFrameLength");
        auto asrMfccFrameStride = ctx.Get<uint32_t>("asrFrameStride");
        auto asrScoreThreshold = ctx.Get<float>("asrScoreThreshold");
        auto asrInputCtxLen = ctx.Get<uint32_t>("ctxLen");

        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        if (!asrModel.IsInited()) {
            printf_err("ASR model has not been initialised\n");
            return false;
        }

        hal_lcd_clear(COLOR_BLACK);

        /* Get Input and Output tensors for pre/post processing. */
        TfLiteTensor* asrInputTensor = asrModel.GetInputTensor(0);
        TfLiteTensor* asrOutputTensor = asrModel.GetOutputTensor(0);

        /* Get input shape. Dimensions of the tensor should have been verified by
        * the callee. */
        TfLiteIntArray* inputShape = asrModel.GetInputShape(0);


        const uint32_t asrInputRows = asrInputTensor->dims->data[Wav2LetterModel::ms_inputRowsIdx];
        const uint32_t asrInputInnerLen = asrInputRows - (2 * asrInputCtxLen);

        /* Make sure the input tensor supports the above context and inner lengths. */
        if (asrInputRows <= 2 * asrInputCtxLen || asrInputRows <= asrInputInnerLen) {
            printf_err("ASR input rows not compatible with ctx length %" PRIu32 "\n",
                asrInputCtxLen);
            return false;
        }

        /* Audio data stride corresponds to inputInnerLen feature vectors. */
        const uint32_t asrAudioDataWindowLen = (asrInputRows - 1) * asrMfccFrameStride + (asrMfccFrameLen);
        const uint32_t asrAudioDataWindowStride = asrInputInnerLen * asrMfccFrameStride;
        const float asrAudioParamsSecondsPerSample = 1.0 / audio::Wav2LetterMFCC::ms_defaultSamplingFreq;

        /* Get the remaining audio buffer and respective size from KWS results. */
        const int16_t* audioArr = kwsOutput.asrAudioStart;
        const uint32_t audioArrSize = kwsOutput.asrAudioSamples;

        /* Audio clip must have enough samples to produce 1 MFCC feature. */
        std::vector<int16_t> audioBuffer = std::vector<int16_t>(audioArr, audioArr + audioArrSize);
        if (audioArrSize < asrMfccFrameLen) {
            printf_err("Not enough audio samples, minimum needed is %" PRIu32 "\n",
                       asrMfccFrameLen);
            return false;
        }

        /* Initialise an audio slider. */
        auto audioDataSlider = audio::FractionalSlidingWindow<const int16_t>(
                audioBuffer.data(),
                audioBuffer.size(),
                asrAudioDataWindowLen,
                asrAudioDataWindowStride);

        /* Declare a container for results. */
        std::vector<asr::AsrResult> asrResults;

        /* Display message on the LCD - inference running. */
        std::string str_inf{"Running ASR inference... "};
        hal_lcd_display_text(str_inf.c_str(), str_inf.size(),
                dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

        size_t asrInferenceWindowLen = asrAudioDataWindowLen;

        /* Set up pre and post-processing objects. */
        AsrPreProcess asrPreProcess = AsrPreProcess(asrInputTensor, arm::app::Wav2LetterModel::ms_numMfccFeatures,
                                              inputShape->data[Wav2LetterModel::ms_inputRowsIdx],
                                              asrMfccFrameLen, asrMfccFrameStride);

        std::vector<ClassificationResult> singleInfResult;
        const uint32_t outputCtxLen = AsrPostProcess::GetOutputContextLen(asrModel, asrInputCtxLen);
        AsrPostProcess asrPostProcess = AsrPostProcess(
                asrOutputTensor, ctx.Get<AsrClassifier&>("asrClassifier"),
                ctx.Get<std::vector<std::string>&>("asrLabels"),
                singleInfResult, outputCtxLen,
                Wav2LetterModel::ms_blankTokenIdx, Wav2LetterModel::ms_outputRowsIdx
        );
        /* Start sliding through audio clip. */
        while (audioDataSlider.HasNext()) {

            /* If not enough audio see how much can be sent for processing. */
            size_t nextStartIndex = audioDataSlider.NextWindowStartIndex();
            if (nextStartIndex + asrAudioDataWindowLen > audioBuffer.size()) {
                asrInferenceWindowLen = audioBuffer.size() - nextStartIndex;
            }

            const int16_t* asrInferenceWindow = audioDataSlider.Next();

            info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
                static_cast<size_t>(ceilf(audioDataSlider.FractionalTotalStrides() + 1)));

            /* Run the pre-processing, inference and post-processing. */
            if (!asrPreProcess.DoPreProcess(asrInferenceWindow, asrInferenceWindowLen)) {
                printf_err("ASR pre-processing failed.");
                return false;
            }

            /* Run inference over this audio clip sliding window. */
            if (!RunInference(asrModel, profiler)) {
                printf_err("ASR inference failed\n");
                return false;
            }

            /* Post processing needs to know if we are on the last audio window. */
            asrPostProcess.m_lastIteration = !audioDataSlider.HasNext();
            if (!asrPostProcess.DoPostProcess()) {
                printf_err("ASR post-processing failed.");
                return false;
            }

            /* Get results. */
            std::vector<ClassificationResult> asrClassificationResult;
            auto& asrClassifier = ctx.Get<AsrClassifier&>("asrClassifier");
            asrClassifier.GetClassificationResults(
                    asrOutputTensor, asrClassificationResult,
                    ctx.Get<std::vector<std::string>&>("asrLabels"), 1);

            asrResults.emplace_back(asr::AsrResult(asrClassificationResult,
                                                (audioDataSlider.Index() *
                                                 asrAudioParamsSecondsPerSample *
                                                 asrAudioDataWindowStride),
                                                 audioDataSlider.Index(), asrScoreThreshold));

#if VERIFY_TEST_OUTPUT
            armDumpTensor(asrOutputTensor, asrOutputTensor->dims->data[Wav2LetterModel::ms_outputColsIdx]);
#endif /* VERIFY_TEST_OUTPUT */

            /* Erase */
            str_inf = std::string(str_inf.size(), ' ');
            hal_lcd_display_text(
                        str_inf.c_str(), str_inf.size(),
                        dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);
        }
        if (!PresentInferenceResult(asrResults)) {
            return false;
        }

        profiler.PrintProfilingResult();

        return true;
    }

    /* KWS and ASR inference handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx, uint32_t clipIndex, bool runAll)
    {
        hal_lcd_clear(COLOR_BLACK);

        /* If the request has a valid size, set the audio index. */
        if (clipIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, clipIndex,"kws_asr")) {
                return false;
            }
        }

        auto startClipIdx = ctx.Get<uint32_t>("clipIndex");

        do {
            KWSOutput kwsOutput = doKws(ctx);
            if (!kwsOutput.executionSuccess) {
                printf_err("KWS failed\n");
                return false;
            }

            if (kwsOutput.asrAudioStart != nullptr && kwsOutput.asrAudioSamples > 0) {
                info("Trigger keyword spotted\n");
                if(!doAsr(ctx, kwsOutput)) {
                    printf_err("ASR failed\n");
                    return false;
                }
            }

            IncrementAppCtxIfmIdx(ctx,"kws_asr");

        } while (runAll && ctx.Get<uint32_t>("clipIndex") != startClipIdx);

        return true;
    }

    static bool PresentInferenceResult(std::vector<arm::app::kws::KwsResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 30;
        constexpr uint32_t dataPsnTxtYIncr   = 16;  /* Row index increment. */

        hal_lcd_set_text_color(COLOR_GREEN);

        /* Display each result. */
        uint32_t rowIdx1 = dataPsnTxtStartY1 + 2 * dataPsnTxtYIncr;

        for (auto & result : results) {
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
                    dataPsnTxtStartX1, rowIdx1, 0);
            rowIdx1 += dataPsnTxtYIncr;

            info("For timestamp: %f (inference #: %" PRIu32 "); threshold: %f\n",
                 result.m_timeStamp, result.m_inferenceNumber,
                 result.m_threshold);
            for (uint32_t j = 0; j < result.m_resultVec.size(); ++j) {
                info("\t\tlabel @ %" PRIu32 ": %s, score: %f\n", j,
                     result.m_resultVec[j].m_label.c_str(),
                     result.m_resultVec[j].m_normalisedVal);
            }
        }

        return true;
    }

    static bool PresentInferenceResult(std::vector<arm::app::asr::AsrResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 80;
        constexpr bool allow_multiple_lines = true;

        hal_lcd_set_text_color(COLOR_GREEN);

        /* Results from multiple inferences should be combined before processing. */
        std::vector<arm::app::ClassificationResult> combinedResults;
        for (auto& result : results) {
            combinedResults.insert(combinedResults.end(),
                                   result.m_resultVec.begin(),
                                   result.m_resultVec.end());
        }

        for (auto& result : results) {
            /* Get the final result string using the decoder. */
            std::string infResultStr = audio::asr::DecodeOutput(result.m_resultVec);

            info("Result for inf %" PRIu32 ": %s\n", result.m_inferenceNumber,
                 infResultStr.c_str());
        }

        std::string finalResultStr = audio::asr::DecodeOutput(combinedResults);

        hal_lcd_display_text(finalResultStr.c_str(), finalResultStr.size(),
                dataPsnTxtStartX1, dataPsnTxtStartY1, allow_multiple_lines);

        info("Final result: %s\n", finalResultStr.c_str());
        return true;
    }

} /* namespace app */
} /* namespace arm */
