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

#include "InputFiles.hpp"
#include "Classifier.hpp"
#include "DsCnnModel.hpp"
#include "hal.h"
#include "DsCnnMfcc.hpp"
#include "AudioUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "KwsResult.hpp"

#include <vector>
#include <functional>

using KwsClassifier = arm::app::Classifier;

namespace arm {
namespace app {

    /**
    * @brief            Helper function to increment current audio clip index.
    * @param[in,out]    ctx   Pointer to the application context object.
    **/
    static void IncrementAppCtxClipIdx(ApplicationContext& ctx);

    /**
     * @brief           Helper function to set the audio clip index.
     * @param[in,out]   ctx   Pointer to the application context object.
     * @param[in]       idx   Value to be set.
     * @return          true if index is set, false otherwise.
     **/
    static bool SetAppCtxClipIdx(ApplicationContext& ctx, uint32_t idx);

    /**
     * @brief           Presents inference results using the data presentation
     *                  object.
     * @param[in]       platform    Reference to the hal platform object.
     * @param[in]       results     Vector of classification results to be displayed.
     * @return          true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(hal_platform& platform,
                                       const std::vector<arm::app::kws::KwsResult>& results);

    /**
     * @brief Returns a function to perform feature calculation and populates input tensor data with
     * MFCC data.
     *
     * Input tensor data type check is performed to choose correct MFCC feature data type.
     * If tensor has an integer data type then original features are quantised.
     *
     * Warning: MFCC calculator provided as input must have the same life scope as returned function.
     *
     * @param[in]       mfcc          MFCC feature calculator.
     * @param[in,out]   inputTensor   Input tensor pointer to store calculated features.
     * @param[in]       cacheSize     Size of the feature vectors cache (number of feature vectors).
     * @return          Function to be called providing audio sample and sliding window index.
     */
    static std::function<void (std::vector<int16_t>&, int, bool, size_t)>
            GetFeatureCalculator(audio::DsCnnMFCC&  mfcc,
                                 TfLiteTensor*      inputTensor,
                                 size_t             cacheSize);

    /* Audio inference handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx, uint32_t clipIndex, bool runAll)
    {
        auto& platform = ctx.Get<hal_platform&>("platform");
        auto& profiler = ctx.Get<Profiler&>("profiler");

        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;
        constexpr int minTensorDims = static_cast<int>(
            (arm::app::DsCnnModel::ms_inputRowsIdx > arm::app::DsCnnModel::ms_inputColsIdx)?
             arm::app::DsCnnModel::ms_inputRowsIdx : arm::app::DsCnnModel::ms_inputColsIdx);

        platform.data_psn->clear(COLOR_BLACK);

        auto& model = ctx.Get<Model&>("model");

        /* If the request has a valid size, set the audio index. */
        if (clipIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxClipIdx(ctx, clipIndex)) {
                return false;
            }
        }
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        const auto frameLength = ctx.Get<int>("frameLength");
        const auto frameStride = ctx.Get<int>("frameStride");
        const auto scoreThreshold = ctx.Get<float>("scoreThreshold");
        auto startClipIdx = ctx.Get<uint32_t>("clipIndex");

        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        TfLiteTensor* inputTensor = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < minTensorDims) {
            printf_err("Input tensor dimension should be >= %d\n", minTensorDims);
            return false;
        }

        TfLiteIntArray* inputShape = model.GetInputShape(0);
        const uint32_t kNumCols = inputShape->data[arm::app::DsCnnModel::ms_inputColsIdx];
        const uint32_t kNumRows = inputShape->data[arm::app::DsCnnModel::ms_inputRowsIdx];

        audio::DsCnnMFCC mfcc = audio::DsCnnMFCC(kNumCols, frameLength);
        mfcc.Init();

        /* Deduce the data length required for 1 inference from the network parameters. */
        auto audioDataWindowSize = kNumRows * frameStride + (frameLength - frameStride);
        auto mfccWindowSize = frameLength;
        auto mfccWindowStride = frameStride;

        /* We choose to move by half the window size => for a 1 second window size
         * there is an overlap of 0.5 seconds. */
        auto audioDataStride = audioDataWindowSize / 2;

        /* To have the previously calculated features re-usable, stride must be multiple
         * of MFCC features window stride. */
        if (0 != audioDataStride % mfccWindowStride) {

            /* Reduce the stride. */
            audioDataStride -= audioDataStride % mfccWindowStride;
        }

        auto nMfccVectorsInAudioStride = audioDataStride/mfccWindowStride;

        /* We expect to be sampling 1 second worth of data at a time.
         * NOTE: This is only used for time stamp calculation. */
        const float secondsPerSample = 1.0/audio::DsCnnMFCC::ms_defaultSamplingFreq;

        do {
            auto currentIndex = ctx.Get<uint32_t>("clipIndex");

            /* Creating a mfcc features sliding window for the data required for 1 inference. */
            auto audioMFCCWindowSlider = audio::SlidingWindow<const int16_t>(
                                            get_audio_array(currentIndex),
                                            audioDataWindowSize, mfccWindowSize,
                                            mfccWindowStride);

            /* Creating a sliding window through the whole audio clip. */
            auto audioDataSlider = audio::SlidingWindow<const int16_t>(
                                        get_audio_array(currentIndex),
                                        get_audio_array_size(currentIndex),
                                        audioDataWindowSize, audioDataStride);

            /* Calculate number of the feature vectors in the window overlap region.
             * These feature vectors will be reused.*/
            auto numberOfReusedFeatureVectors = audioMFCCWindowSlider.TotalStrides() + 1
                                                - nMfccVectorsInAudioStride;

            /* Construct feature calculation function. */
            auto mfccFeatureCalc = GetFeatureCalculator(mfcc, inputTensor,
                                                        numberOfReusedFeatureVectors);

            if (!mfccFeatureCalc){
                return false;
            }

            /* Declare a container for results. */
            std::vector<arm::app::kws::KwsResult> results;

            /* Display message on the LCD - inference running. */
            std::string str_inf{"Running inference... "};
            platform.data_psn->present_data_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);
            info("Running inference on audio clip %" PRIu32 " => %s\n", currentIndex,
                 get_filename(currentIndex));

            /* Start sliding through audio clip. */
            while (audioDataSlider.HasNext()) {
                const int16_t *inferenceWindow = audioDataSlider.Next();

                /* We moved to the next window - set the features sliding to the new address. */
                audioMFCCWindowSlider.Reset(inferenceWindow);

                /* The first window does not have cache ready. */
                bool useCache = audioDataSlider.Index() > 0 && numberOfReusedFeatureVectors > 0;

                /* Start calculating features inside one audio sliding window. */
                while (audioMFCCWindowSlider.HasNext()) {
                    const int16_t *mfccWindow = audioMFCCWindowSlider.Next();
                    std::vector<int16_t> mfccAudioData = std::vector<int16_t>(mfccWindow,
                                                            mfccWindow + mfccWindowSize);
                    /* Compute features for this window and write them to input tensor. */
                    mfccFeatureCalc(mfccAudioData,
                                    audioMFCCWindowSlider.Index(),
                                    useCache,
                                    nMfccVectorsInAudioStride);
                }

                info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
                     audioDataSlider.TotalStrides() + 1);

                /* Run inference over this audio clip sliding window. */
                if (!RunInference(model, profiler)) {
                    return false;
                }

                std::vector<ClassificationResult> classificationResult;
                auto& classifier = ctx.Get<KwsClassifier&>("classifier");
                classifier.GetClassificationResults(outputTensor, classificationResult,
                                                    ctx.Get<std::vector<std::string>&>("labels"), 1);

                results.emplace_back(kws::KwsResult(classificationResult,
                    audioDataSlider.Index() * secondsPerSample * audioDataStride,
                    audioDataSlider.Index(), scoreThreshold));

#if VERIFY_TEST_OUTPUT
                arm::app::DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */
            } /* while (audioDataSlider.HasNext()) */

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            platform.data_psn->present_data_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

            ctx.Set<std::vector<arm::app::kws::KwsResult>>("results", results);

            if (!PresentInferenceResult(platform, results)) {
                return false;
            }

            profiler.PrintProfilingResult();

            IncrementAppCtxClipIdx(ctx);

        } while (runAll && ctx.Get<uint32_t>("clipIndex") != startClipIdx);

        return true;
    }

    static void IncrementAppCtxClipIdx(ApplicationContext& ctx)
    {
        auto curAudioIdx = ctx.Get<uint32_t>("clipIndex");

        if (curAudioIdx + 1 >= NUMBER_OF_FILES) {
            ctx.Set<uint32_t>("clipIndex", 0);
            return;
        }
        ++curAudioIdx;
        ctx.Set<uint32_t>("clipIndex", curAudioIdx);
    }

    static bool SetAppCtxClipIdx(ApplicationContext& ctx, uint32_t idx)
    {
        if (idx >= NUMBER_OF_FILES) {
            printf_err("Invalid idx %" PRIu32 " (expected less than %u)\n",
                       idx, NUMBER_OF_FILES);
            return false;
        }
        ctx.Set<uint32_t>("clipIndex", idx);
        return true;
    }

    static bool PresentInferenceResult(hal_platform& platform,
                                       const std::vector<arm::app::kws::KwsResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 30;
        constexpr uint32_t dataPsnTxtYIncr   = 16;  /* Row index increment. */

        platform.data_psn->set_text_color(COLOR_GREEN);
        info("Final results:\n");
        info("Total number of inferences: %zu\n", results.size());

        /* Display each result */
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
                                    dataPsnTxtStartX1, rowIdx1, false);
            rowIdx1 += dataPsnTxtYIncr;

            if (results[i].m_resultVec.empty()) {
                info("For timestamp: %f (inference #: %" PRIu32
                     "); label: %s; threshold: %f\n",
                     results[i].m_timeStamp, results[i].m_inferenceNumber,
                     topKeyword.c_str(),
                     results[i].m_threshold);
            } else {
                for (uint32_t j = 0; j < results[i].m_resultVec.size(); ++j) {
                    info("For timestamp: %f (inference #: %" PRIu32
                         "); label: %s, score: %f; threshold: %f\n",
                         results[i].m_timeStamp,
                         results[i].m_inferenceNumber,
                         results[i].m_resultVec[j].m_label.c_str(),
                         results[i].m_resultVec[j].m_normalisedVal,
                         results[i].m_threshold);
                }
            }
        }

        return true;
    }

    /**
     * @brief Generic feature calculator factory.
     *
     * Returns lambda function to compute features using features cache.
     * Real features math is done by a lambda function provided as a parameter.
     * Features are written to input tensor memory.
     *
     * @tparam T                Feature vector type.
     * @param[in] inputTensor   Model input tensor pointer.
     * @param[in] cacheSize     Number of feature vectors to cache. Defined by the sliding window overlap.
     * @param[in] compute       Features calculator function.
     * @return                  Lambda function to compute features.
     */
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
            T *tensorData = tflite::GetTensorData<T>(inputTensor);
            std::vector<T> features;

            /* Reuse features from cache if cache is ready and sliding windows overlap.
             * Overlap is in the beginning of sliding window with a size of a feature cache. */
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

            auto *quantParams = (TfLiteAffineQuantization *) quant.params;
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