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
#include "UseCaseHandler.hpp"

#include "hal.h"
#include "InputFiles.hpp"
#include "AudioUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DsCnnModel.hpp"
#include "DsCnnMfcc.hpp"
#include "Classifier.hpp"
#include "KwsResult.hpp"
#include "Wav2LetterMfcc.hpp"
#include "Wav2LetterPreprocess.hpp"
#include "Wav2LetterPostprocess.hpp"
#include "AsrResult.hpp"
#include "AsrClassifier.hpp"
#include "OutputDecode.hpp"


using KwsClassifier = arm::app::Classifier;

namespace arm {
namespace app {

    enum AsrOutputReductionAxis {
        AxisRow = 1,
        AxisCol = 2
    };

    struct KWSOutput {
        bool executionSuccess = false;
        const int16_t* asrAudioStart = nullptr;
        int32_t asrAudioSamples = 0;
    };

    /**
     * @brief           Presents kws inference results using the data presentation
     *                  object.
     * @param[in]       platform    reference to the hal platform object
     * @param[in]       results     vector of classification results to be displayed
     * @return          true if successful, false otherwise
     **/
    static bool PresentInferenceResult(hal_platform& platform, std::vector<arm::app::kws::KwsResult>& results);

    /**
     * @brief           Presents asr inference results using the data presentation
     *                  object.
     * @param[in]       platform    reference to the hal platform object
     * @param[in]       results     vector of classification results to be displayed
     * @return          true if successful, false otherwise
     **/
    static bool PresentInferenceResult(hal_platform& platform, std::vector<arm::app::asr::AsrResult>& results);

    /**
     * @brief Returns a function to perform feature calculation and populates input tensor data with
     * MFCC data.
     *
     * Input tensor data type check is performed to choose correct MFCC feature data type.
     * If tensor has an integer data type then original features are quantised.
     *
     * Warning: mfcc calculator provided as input must have the same life scope as returned function.
     *
     * @param[in]           mfcc            MFCC feature calculator.
     * @param[in,out]       inputTensor     Input tensor pointer to store calculated features.
     * @param[in]            cacheSize      Size of the feture vectors cache (number of feature vectors).
     *
     * @return function     function to be called providing audio sample and sliding window index.
     **/
    static std::function<void (std::vector<int16_t>&, int, bool, size_t)>
    GetFeatureCalculator(audio::DsCnnMFCC&  mfcc,
                         TfLiteTensor*      inputTensor,
                         size_t             cacheSize);

    /**
     * @brief Performs the KWS pipeline.
     * @param[in,out]   ctx pointer to the application context object
     *
     * @return KWSOutput    struct containing pointer to audio data where ASR should begin
     *                      and how much data to process.
     */
    static KWSOutput doKws(ApplicationContext& ctx) {
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        constexpr int minTensorDims = static_cast<int>(
            (arm::app::DsCnnModel::ms_inputRowsIdx > arm::app::DsCnnModel::ms_inputColsIdx)?
             arm::app::DsCnnModel::ms_inputRowsIdx : arm::app::DsCnnModel::ms_inputColsIdx);

        KWSOutput output;

        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& kwsModel = ctx.Get<Model&>("kwsmodel");
        if (!kwsModel.IsInited()) {
            printf_err("KWS model has not been initialised\n");
            return output;
        }

        const int kwsFrameLength = ctx.Get<int>("kwsframeLength");
        const int kwsFrameStride = ctx.Get<int>("kwsframeStride");
        const float kwsScoreThreshold = ctx.Get<float>("kwsscoreThreshold");

        TfLiteTensor* kwsOutputTensor = kwsModel.GetOutputTensor(0);
        TfLiteTensor* kwsInputTensor = kwsModel.GetInputTensor(0);

        if (!kwsInputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return output;
        } else if (kwsInputTensor->dims->size < minTensorDims) {
            printf_err("Input tensor dimension should be >= %d\n", minTensorDims);
            return output;
        }

        const uint32_t kwsNumMfccFeats = ctx.Get<uint32_t>("kwsNumMfcc");
        const uint32_t kwsNumAudioWindows = ctx.Get<uint32_t>("kwsNumAudioWins");

        audio::DsCnnMFCC kwsMfcc = audio::DsCnnMFCC(kwsNumMfccFeats, kwsFrameLength);
        kwsMfcc.Init();

        /* Deduce the data length required for 1 KWS inference from the network parameters. */
        auto kwsAudioDataWindowSize = kwsNumAudioWindows * kwsFrameStride +
                                        (kwsFrameLength - kwsFrameStride);
        auto kwsMfccWindowSize = kwsFrameLength;
        auto kwsMfccWindowStride = kwsFrameStride;

        /* We are choosing to move by half the window size => for a 1 second window size,
         * this means an overlap of 0.5 seconds. */
        auto kwsAudioDataStride = kwsAudioDataWindowSize / 2;

        info("KWS audio data window size %" PRIu32 "\n", kwsAudioDataWindowSize);

        /* Stride must be multiple of mfcc features window stride to re-use features. */
        if (0 != kwsAudioDataStride % kwsMfccWindowStride) {
            kwsAudioDataStride -= kwsAudioDataStride % kwsMfccWindowStride;
        }

        auto kwsMfccVectorsInAudioStride = kwsAudioDataStride/kwsMfccWindowStride;

        /* We expect to be sampling 1 second worth of data at a time
         * NOTE: This is only used for time stamp calculation. */
        const float kwsAudioParamsSecondsPerSample = 1.0/audio::DsCnnMFCC::ms_defaultSamplingFreq;

        auto currentIndex = ctx.Get<uint32_t>("clipIndex");

        /* Creating a mfcc features sliding window for the data required for 1 inference. */
        auto kwsAudioMFCCWindowSlider = audio::SlidingWindow<const int16_t>(
                get_audio_array(currentIndex),
                kwsAudioDataWindowSize, kwsMfccWindowSize,
                kwsMfccWindowStride);

        /* Creating a sliding window through the whole audio clip. */
        auto audioDataSlider = audio::SlidingWindow<const int16_t>(
                get_audio_array(currentIndex),
                get_audio_array_size(currentIndex),
                kwsAudioDataWindowSize, kwsAudioDataStride);

        /* Calculate number of the feature vectors in the window overlap region.
         * These feature vectors will be reused.*/
        size_t numberOfReusedFeatureVectors = kwsAudioMFCCWindowSlider.TotalStrides() + 1
                                              - kwsMfccVectorsInAudioStride;

        auto kwsMfccFeatureCalc = GetFeatureCalculator(kwsMfcc, kwsInputTensor,
                                                       numberOfReusedFeatureVectors);

        if (!kwsMfccFeatureCalc){
            return output;
        }

        /* Container for KWS results. */
        std::vector<arm::app::kws::KwsResult> kwsResults;

        /* Display message on the LCD - inference running. */
        auto& platform = ctx.Get<hal_platform&>("platform");
        std::string str_inf{"Running KWS inference... "};
        platform.data_psn->present_data_text(
                            str_inf.c_str(), str_inf.size(),
                            dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

        info("Running KWS inference on audio clip %" PRIu32 " => %s\n",
             currentIndex, get_filename(currentIndex));

        /* Start sliding through audio clip. */
        while (audioDataSlider.HasNext()) {
            const int16_t* inferenceWindow = audioDataSlider.Next();

            /* We moved to the next window - set the features sliding to the new address. */
            kwsAudioMFCCWindowSlider.Reset(inferenceWindow);

            /* The first window does not have cache ready. */
            bool useCache = audioDataSlider.Index() > 0 && numberOfReusedFeatureVectors > 0;

            /* Start calculating features inside one audio sliding window. */
            while (kwsAudioMFCCWindowSlider.HasNext()) {
                const int16_t* kwsMfccWindow = kwsAudioMFCCWindowSlider.Next();
                std::vector<int16_t> kwsMfccAudioData =
                    std::vector<int16_t>(kwsMfccWindow, kwsMfccWindow + kwsMfccWindowSize);

                /* Compute features for this window and write them to input tensor. */
                kwsMfccFeatureCalc(kwsMfccAudioData,
                                   kwsAudioMFCCWindowSlider.Index(),
                                   useCache,
                                   kwsMfccVectorsInAudioStride);
            }

            info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
                 audioDataSlider.TotalStrides() + 1);

            /* Run inference over this audio clip sliding window. */
            if (!RunInference(kwsModel, profiler)) {
                printf_err("KWS inference failed\n");
                return output;
            }

            std::vector<ClassificationResult> kwsClassificationResult;
            auto& kwsClassifier = ctx.Get<KwsClassifier&>("kwsclassifier");

            kwsClassifier.GetClassificationResults(
                            kwsOutputTensor, kwsClassificationResult,
                            ctx.Get<std::vector<std::string>&>("kwslabels"), 1);

            kwsResults.emplace_back(
                kws::KwsResult(
                    kwsClassificationResult,
                    audioDataSlider.Index() * kwsAudioParamsSecondsPerSample * kwsAudioDataStride,
                    audioDataSlider.Index(), kwsScoreThreshold)
                );

            /* Keyword detected. */
            if (kwsClassificationResult[0].m_labelIdx == ctx.Get<uint32_t>("keywordindex")) {
                output.asrAudioStart = inferenceWindow + kwsAudioDataWindowSize;
                output.asrAudioSamples = get_audio_array_size(currentIndex) -
                                        (audioDataSlider.NextWindowStartIndex() -
                                        kwsAudioDataStride + kwsAudioDataWindowSize);
                break;
            }

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(kwsOutputTensor);
#endif /* VERIFY_TEST_OUTPUT */

        } /* while (audioDataSlider.HasNext()) */

        /* Erase. */
        str_inf = std::string(str_inf.size(), ' ');
        platform.data_psn->present_data_text(
                            str_inf.c_str(), str_inf.size(),
                            dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

        if (!PresentInferenceResult(platform, kwsResults)) {
            return output;
        }

        profiler.PrintProfilingResult();

        output.executionSuccess = true;
        return output;
    }

    /**
     * @brief Performs the ASR pipeline.
     *
     * @param[in,out] ctx   pointer to the application context object
     * @param[in] kwsOutput struct containing pointer to audio data where ASR should begin
     *                      and how much data to process
     * @return bool         true if pipeline executed without failure
     */
    static bool doAsr(ApplicationContext& ctx, const KWSOutput& kwsOutput) {
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& platform = ctx.Get<hal_platform&>("platform");
        platform.data_psn->clear(COLOR_BLACK);

        /* Get model reference. */
        auto& asrModel = ctx.Get<Model&>("asrmodel");
        if (!asrModel.IsInited()) {
            printf_err("ASR model has not been initialised\n");
            return false;
        }

        /* Get score threshold to be applied for the classifier (post-inference). */
        auto asrScoreThreshold = ctx.Get<float>("asrscoreThreshold");

        /* Dimensions of the tensor should have been verified by the callee. */
        TfLiteTensor* asrInputTensor = asrModel.GetInputTensor(0);
        TfLiteTensor* asrOutputTensor = asrModel.GetOutputTensor(0);
        const uint32_t asrInputRows = asrInputTensor->dims->data[arm::app::Wav2LetterModel::ms_inputRowsIdx];

        /* Populate ASR MFCC related parameters. */
        auto asrMfccParamsWinLen = ctx.Get<uint32_t>("asrframeLength");
        auto asrMfccParamsWinStride = ctx.Get<uint32_t>("asrframeStride");

        /* Populate ASR inference context and inner lengths for input. */
        auto asrInputCtxLen = ctx.Get<uint32_t>("ctxLen");
        const uint32_t asrInputInnerLen = asrInputRows - (2 * asrInputCtxLen);

        /* Make sure the input tensor supports the above context and inner lengths. */
        if (asrInputRows <= 2 * asrInputCtxLen || asrInputRows <= asrInputInnerLen) {
            printf_err("ASR input rows not compatible with ctx length %" PRIu32 "\n",
                asrInputCtxLen);
            return false;
        }

        /* Audio data stride corresponds to inputInnerLen feature vectors. */
        const uint32_t asrAudioParamsWinLen = (asrInputRows - 1) *
                                              asrMfccParamsWinStride + (asrMfccParamsWinLen);
        const uint32_t asrAudioParamsWinStride = asrInputInnerLen * asrMfccParamsWinStride;
        const float asrAudioParamsSecondsPerSample =
                                        (1.0/audio::Wav2LetterMFCC::ms_defaultSamplingFreq);

        /* Get pre/post-processing objects */
        auto& asrPrep = ctx.Get<audio::asr::Preprocess&>("preprocess");
        auto& asrPostp = ctx.Get<audio::asr::Postprocess&>("postprocess");

        /* Set default reduction axis for post-processing. */
        const uint32_t reductionAxis = arm::app::Wav2LetterModel::ms_outputRowsIdx;

        /* Get the remaining audio buffer and respective size from KWS results. */
        const int16_t* audioArr = kwsOutput.asrAudioStart;
        const uint32_t audioArrSize = kwsOutput.asrAudioSamples;

        /* Audio clip must have enough samples to produce 1 MFCC feature. */
        std::vector<int16_t> audioBuffer = std::vector<int16_t>(audioArr, audioArr + audioArrSize);
        if (audioArrSize < asrMfccParamsWinLen) {
            printf_err("Not enough audio samples, minimum needed is %" PRIu32 "\n",
                asrMfccParamsWinLen);
            return false;
        }

        /* Initialise an audio slider. */
        auto audioDataSlider = audio::FractionalSlidingWindow<const int16_t>(
                audioBuffer.data(),
                audioBuffer.size(),
                asrAudioParamsWinLen,
                asrAudioParamsWinStride);

        /* Declare a container for results. */
        std::vector<arm::app::asr::AsrResult> asrResults;

        /* Display message on the LCD - inference running. */
        std::string str_inf{"Running ASR inference... "};
        platform.data_psn->present_data_text(
                str_inf.c_str(), str_inf.size(),
                dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

        size_t asrInferenceWindowLen = asrAudioParamsWinLen;

        /* Start sliding through audio clip. */
        while (audioDataSlider.HasNext()) {

            /* If not enough audio see how much can be sent for processing. */
            size_t nextStartIndex = audioDataSlider.NextWindowStartIndex();
            if (nextStartIndex + asrAudioParamsWinLen > audioBuffer.size()) {
                asrInferenceWindowLen = audioBuffer.size() - nextStartIndex;
            }

            const int16_t* asrInferenceWindow = audioDataSlider.Next();

            info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
                static_cast<size_t>(ceilf(audioDataSlider.FractionalTotalStrides() + 1)));

            /* Calculate MFCCs, deltas and populate the input tensor. */
            asrPrep.Invoke(asrInferenceWindow, asrInferenceWindowLen, asrInputTensor);

            /* Run inference over this audio clip sliding window. */
            if (!RunInference(asrModel, profiler)) {
                printf_err("ASR inference failed\n");
                return false;
            }

            /* Post-process. */
            asrPostp.Invoke(asrOutputTensor, reductionAxis, !audioDataSlider.HasNext());

            /* Get results. */
            std::vector<ClassificationResult> asrClassificationResult;
            auto& asrClassifier = ctx.Get<AsrClassifier&>("asrclassifier");
            asrClassifier.GetClassificationResults(
                    asrOutputTensor, asrClassificationResult,
                    ctx.Get<std::vector<std::string>&>("asrlabels"), 1);

            asrResults.emplace_back(asr::AsrResult(asrClassificationResult,
                                                (audioDataSlider.Index() *
                                                 asrAudioParamsSecondsPerSample *
                                                 asrAudioParamsWinStride),
                                                 audioDataSlider.Index(), asrScoreThreshold));

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(asrOutputTensor, asrOutputTensor->dims->data[arm::app::Wav2LetterModel::ms_outputColsIdx]);
#endif /* VERIFY_TEST_OUTPUT */

            /* Erase */
            str_inf = std::string(str_inf.size(), ' ');
            platform.data_psn->present_data_text(
                        str_inf.c_str(), str_inf.size(),
                        dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);
        }
        if (!PresentInferenceResult(platform, asrResults)) {
            return false;
        }

        profiler.PrintProfilingResult();

        return true;
    }

    /* Audio inference classification handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx, uint32_t clipIndex, bool runAll)
    {
        auto& platform = ctx.Get<hal_platform&>("platform");
        platform.data_psn->clear(COLOR_BLACK);

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
                return false;
            }

            if (kwsOutput.asrAudioStart != nullptr && kwsOutput.asrAudioSamples > 0) {
                info("Keyword spotted\n");
                if(!doAsr(ctx, kwsOutput)) {
                    printf_err("ASR failed");
                    return false;
                }
            }

            IncrementAppCtxIfmIdx(ctx,"kws_asr");

        } while (runAll && ctx.Get<uint32_t>("clipIndex") != startClipIdx);

        return true;
    }


    static bool PresentInferenceResult(hal_platform& platform,
                                       std::vector<arm::app::kws::KwsResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 30;
        constexpr uint32_t dataPsnTxtYIncr   = 16;  /* Row index increment. */

        platform.data_psn->set_text_color(COLOR_GREEN);

        /* Display each result. */
        uint32_t rowIdx1 = dataPsnTxtStartY1 + 2 * dataPsnTxtYIncr;

        for (uint32_t i = 0; i < results.size(); ++i) {

            std::string topKeyword{"<none>"};
            float score = 0.f;

            if (!results[i].m_resultVec.empty()) {
                topKeyword = results[i].m_resultVec[0].m_label;
                score = results[i].m_resultVec[0].m_normalisedVal;
            }

            std::string resultStr =
                    std::string{"@"} + std::to_string(results[i].m_timeStamp) +
                    std::string{"s: "} + topKeyword + std::string{" ("} +
                    std::to_string(static_cast<int>(score * 100)) + std::string{"%)"};

            platform.data_psn->present_data_text(
                        resultStr.c_str(), resultStr.size(),
                        dataPsnTxtStartX1, rowIdx1, 0);
            rowIdx1 += dataPsnTxtYIncr;

            info("For timestamp: %f (inference #: %" PRIu32 "); threshold: %f\n",
                 results[i].m_timeStamp, results[i].m_inferenceNumber,
                 results[i].m_threshold);
            for (uint32_t j = 0; j < results[i].m_resultVec.size(); ++j) {
                info("\t\tlabel @ %" PRIu32 ": %s, score: %f\n", j,
                     results[i].m_resultVec[j].m_label.c_str(),
                     results[i].m_resultVec[j].m_normalisedVal);
            }
        }

        return true;
    }

    static bool PresentInferenceResult(hal_platform& platform, std::vector<arm::app::asr::AsrResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 80;
        constexpr bool allow_multiple_lines = true;

        platform.data_psn->set_text_color(COLOR_GREEN);

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

        platform.data_psn->present_data_text(
                    finalResultStr.c_str(), finalResultStr.size(),
                    dataPsnTxtStartX1, dataPsnTxtStartY1, allow_multiple_lines);

        info("Final result: %s\n", finalResultStr.c_str());
        return true;
    }

    /**
     * @brief Generic feature calculator factory.
     *
     * Returns lambda function to compute features using features cache.
     * Real features math is done by a lambda function provided as a parameter.
     * Features are written to input tensor memory.
     *
     * @tparam T            feature vector type.
     * @param inputTensor   model input tensor pointer.
     * @param cacheSize     number of feature vectors to cache. Defined by the sliding window overlap.
     * @param compute       features calculator function.
     * @return              lambda function to compute features.
     **/
    template<class T>
    std::function<void (std::vector<int16_t>&, size_t, bool, size_t)>
    FeatureCalc(TfLiteTensor* inputTensor, size_t cacheSize,
                std::function<std::vector<T> (std::vector<int16_t>& )> compute)
    {
        /* Feature cache to be captured by lambda function. */
        static std::vector<std::vector<T>> featureCache = std::vector<std::vector<T>>(cacheSize);

        return [=](std::vector<int16_t>& audioDataWindow,
                   size_t index,
                   bool useCache,
                   size_t featuresOverlapIndex)
        {
            T* tensorData = tflite::GetTensorData<T>(inputTensor);
            std::vector<T> features;

            /* Reuse features from cache if cache is ready and sliding windows overlap.
             * Overlap is in the beginning of sliding window with a size of a feature cache.
             */
            if (useCache && index < featureCache.size()) {
                features = std::move(featureCache[index]);
            } else {
                features = std::move(compute(audioDataWindow));
            }
            auto size = features.size();
            auto sizeBytes = sizeof(T) * size;
            std::memcpy(tensorData + (index * size), features.data(), sizeBytes);

            /* Start renewing cache as soon iteration goes out of the windows overlap. */
            if (index >= featuresOverlapIndex) {
                featureCache[index - featuresOverlapIndex] = std::move(features);
            }
        };
    }

    template std::function<void (std::vector<int16_t>&, size_t , bool, size_t)>
    FeatureCalc<int8_t>(TfLiteTensor* inputTensor,
                        size_t cacheSize,
                        std::function<std::vector<int8_t> (std::vector<int16_t>& )> compute);

    template std::function<void (std::vector<int16_t>&, size_t , bool, size_t)>
    FeatureCalc<uint8_t>(TfLiteTensor* inputTensor,
                         size_t cacheSize,
                         std::function<std::vector<uint8_t> (std::vector<int16_t>& )> compute);

    template std::function<void (std::vector<int16_t>&, size_t , bool, size_t)>
    FeatureCalc<int16_t>(TfLiteTensor* inputTensor,
                         size_t cacheSize,
                         std::function<std::vector<int16_t> (std::vector<int16_t>& )> compute);

    template std::function<void(std::vector<int16_t>&, size_t, bool, size_t)>
    FeatureCalc<float>(TfLiteTensor* inputTensor,
                       size_t cacheSize,
                       std::function<std::vector<float>(std::vector<int16_t>&)> compute);


    static std::function<void (std::vector<int16_t>&, int, bool, size_t)>
    GetFeatureCalculator(audio::DsCnnMFCC& mfcc, TfLiteTensor* inputTensor, size_t cacheSize)
    {
        std::function<void (std::vector<int16_t>&, size_t, bool, size_t)> mfccFeatureCalc;

        TfLiteQuantization quant = inputTensor->quantization;

        if (kTfLiteAffineQuantization == quant.type) {

            auto* quantParams = (TfLiteAffineQuantization*) quant.params;
            const float quantScale = quantParams->scale->data[0];
            const int quantOffset = quantParams->zero_point->data[0];

            switch (inputTensor->type) {
                case kTfLiteInt8: {
                    mfccFeatureCalc = FeatureCalc<int8_t>(inputTensor,
                                                          cacheSize,
                                                          [=, &mfcc](std::vector<int16_t>& audioDataWindow) {
                                                              return mfcc.MfccComputeQuant<int8_t>(audioDataWindow,
                                                                                                   quantScale,
                                                                                                   quantOffset);
                                                          }
                    );
                    break;
                }
                case kTfLiteUInt8: {
                    mfccFeatureCalc = FeatureCalc<uint8_t>(inputTensor,
                                                           cacheSize,
                                                           [=, &mfcc](std::vector<int16_t>& audioDataWindow) {
                                                               return mfcc.MfccComputeQuant<uint8_t>(audioDataWindow,
                                                                                                     quantScale,
                                                                                                     quantOffset);
                                                           }
                    );
                    break;
                }
                case kTfLiteInt16: {
                    mfccFeatureCalc = FeatureCalc<int16_t>(inputTensor,
                                                           cacheSize,
                                                           [=, &mfcc](std::vector<int16_t>& audioDataWindow) {
                                                               return mfcc.MfccComputeQuant<int16_t>(audioDataWindow,
                                                                                                     quantScale,
                                                                                                     quantOffset);
                                                           }
                    );
                    break;
                }
                default:
                printf_err("Tensor type %s not supported\n", TfLiteTypeGetName(inputTensor->type));
            }


        } else {
            mfccFeatureCalc = mfccFeatureCalc = FeatureCalc<float>(inputTensor,
                                                                   cacheSize,
                                                                   [&mfcc](std::vector<int16_t>& audioDataWindow) {
                                                                       return mfcc.MfccCompute(audioDataWindow);
                                                                   });
        }
        return mfccFeatureCalc;
    }
} /* namespace app */
} /* namespace arm */