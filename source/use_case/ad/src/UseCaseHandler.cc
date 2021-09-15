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

#include "AdModel.hpp"
#include "InputFiles.hpp"
#include "Classifier.hpp"
#include "hal.h"
#include "AdMelSpectrogram.hpp"
#include "AudioUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "AdPostProcessing.hpp"

namespace arm {
namespace app {

    /**
     * @brief           Presents inference results using the data presentation
     *                  object.
     * @param[in]       platform    reference to the hal platform object
     * @param[in]       result      average sum of classification results
     * @param[in]       threshold   if larger than this value we have an anomaly
     * @return          true if successful, false otherwise
     **/
    static bool PresentInferenceResult(hal_platform& platform, float result, float threshold);

    /**
     * @brief Returns a function to perform feature calculation and populates input tensor data with
     * MelSpe data.
     *
     * Input tensor data type check is performed to choose correct MFCC feature data type.
     * If tensor has an integer data type then original features are quantised.
     *
     * Warning: mfcc calculator provided as input must have the same life scope as returned function.
     *
     * @param[in]           melSpec         MFCC feature calculator.
     * @param[in,out]       inputTensor     Input tensor pointer to store calculated features.
     * @param[in]           cacheSize       Size of the feture vectors cache (number of feature vectors).
     * @param[in]           trainingMean    Training mean.
     * @return function     function to be called providing audio sample and sliding window index.
     */
    static std::function<void (std::vector<int16_t>&, int, bool, size_t, size_t)>
    GetFeatureCalculator(audio::AdMelSpectrogram&  melSpec,
                         TfLiteTensor*             inputTensor,
                         size_t                    cacheSize,
                         float                     trainingMean);

    /* Vibration classification handler */
    bool ClassifyVibrationHandler(ApplicationContext& ctx, uint32_t clipIndex, bool runAll)
    {
        auto& platform = ctx.Get<hal_platform&>("platform");
        auto& profiler = ctx.Get<Profiler&>("profiler");

        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        platform.data_psn->clear(COLOR_BLACK);

        auto& model = ctx.Get<Model&>("model");

        /* If the request has a valid size, set the audio index */
        if (clipIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, clipIndex,"clipIndex")) {
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
        const auto trainingMean = ctx.Get<float>("trainingMean");
        auto startClipIdx = ctx.Get<uint32_t>("clipIndex");

        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        TfLiteTensor* inputTensor = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        }

        TfLiteIntArray* inputShape = model.GetInputShape(0);
        const uint32_t kNumRows = inputShape->data[1];
        const uint32_t kNumCols = inputShape->data[2];

        audio::AdMelSpectrogram melSpec = audio::AdMelSpectrogram(frameLength);
        melSpec.Init();

        /* Deduce the data length required for 1 inference from the network parameters. */
        const uint8_t inputResizeScale = 2;
        const uint32_t audioDataWindowSize = (((inputResizeScale * kNumCols) - 1) * frameStride) + frameLength;

        /* We are choosing to move by 20 frames across the audio for each inference. */
        const uint8_t nMelSpecVectorsInAudioStride = 20;

        auto audioDataStride = nMelSpecVectorsInAudioStride * frameStride;

        do {
            auto currentIndex = ctx.Get<uint32_t>("clipIndex");

            /* Get the output index to look at based on id in the filename. */
            int8_t machineOutputIndex = OutputIndexFromFileName(get_filename(currentIndex));
            if (machineOutputIndex == -1) {
                return false;
            }

            /* Creating a Mel Spectrogram sliding window for the data required for 1 inference.
             * "resizing" done here by multiplying stride by resize scale. */
            auto audioMelSpecWindowSlider = audio::SlidingWindow<const int16_t>(
                    get_audio_array(currentIndex),
                    audioDataWindowSize, frameLength,
                    frameStride * inputResizeScale);

            /* Creating a sliding window through the whole audio clip. */
            auto audioDataSlider = audio::SlidingWindow<const int16_t>(
                    get_audio_array(currentIndex),
                    get_audio_array_size(currentIndex),
                    audioDataWindowSize, audioDataStride);

            /* Calculate number of the feature vectors in the window overlap region taking into account resizing.
             * These feature vectors will be reused.*/
            auto numberOfReusedFeatureVectors = kNumRows - (nMelSpecVectorsInAudioStride / inputResizeScale);

            /* Construct feature calculation function. */
            auto melSpecFeatureCalc = GetFeatureCalculator(melSpec, inputTensor,
                                                           numberOfReusedFeatureVectors, trainingMean);
            if (!melSpecFeatureCalc){
                return false;
            }

            /* Result is an averaged sum over inferences. */
            float result = 0;

            /* Display message on the LCD - inference running. */
            std::string str_inf{"Running inference... "};
            platform.data_psn->present_data_text(
                    str_inf.c_str(), str_inf.size(),
                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);
            info("Running inference on audio clip %" PRIu32 " => %s\n", currentIndex, get_filename(currentIndex));

            /* Start sliding through audio clip. */
            while (audioDataSlider.HasNext()) {
                const int16_t *inferenceWindow = audioDataSlider.Next();

                /* We moved to the next window - set the features sliding to the new address. */
                audioMelSpecWindowSlider.Reset(inferenceWindow);

                /* The first window does not have cache ready. */
                bool useCache = audioDataSlider.Index() > 0 && numberOfReusedFeatureVectors > 0;

                /* Start calculating features inside one audio sliding window. */
                while (audioMelSpecWindowSlider.HasNext()) {
                    const int16_t *melSpecWindow = audioMelSpecWindowSlider.Next();
                    std::vector<int16_t> melSpecAudioData = std::vector<int16_t>(melSpecWindow,
                                                                                 melSpecWindow + frameLength);

                    /* Compute features for this window and write them to input tensor. */
                    melSpecFeatureCalc(melSpecAudioData, audioMelSpecWindowSlider.Index(),
                                       useCache, nMelSpecVectorsInAudioStride, inputResizeScale);
                }

                info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
                     audioDataSlider.TotalStrides() + 1);

                /* Run inference over this audio clip sliding window */
                if (!RunInference(model, profiler)) {
                    return false;
                }

                /* Use the negative softmax score of the corresponding index as the outlier score */
                std::vector<float> dequantOutput = Dequantize<int8_t>(outputTensor);
                Softmax(dequantOutput);
                result += -dequantOutput[machineOutputIndex];

#if VERIFY_TEST_OUTPUT
                arm::app::DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */
            } /* while (audioDataSlider.HasNext()) */

            /* Use average over whole clip as final score. */
            result /= (audioDataSlider.TotalStrides() + 1);

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            platform.data_psn->present_data_text(
                    str_inf.c_str(), str_inf.size(),
                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            ctx.Set<float>("result", result);
            if (!PresentInferenceResult(platform, result, scoreThreshold)) {
                return false;
            }

            profiler.PrintProfilingResult();

            IncrementAppCtxIfmIdx(ctx,"clipIndex");

        } while (runAll && ctx.Get<uint32_t>("clipIndex") != startClipIdx);

        return true;
    }


    static bool PresentInferenceResult(hal_platform& platform, float result, float threshold)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 30;
        constexpr uint32_t dataPsnTxtYIncr   = 16; /* Row index increment */

        platform.data_psn->set_text_color(COLOR_GREEN);

        /* Display each result */
        uint32_t rowIdx1 = dataPsnTxtStartY1 + 2 * dataPsnTxtYIncr;

        std::string anomalyScore = std::string{"Average anomaly score is: "} + std::to_string(result);
        std::string anomalyThreshold = std::string("Anomaly threshold is: ") + std::to_string(threshold);

        std::string anomalyResult;
        if (result > threshold) {
            anomalyResult += std::string("Anomaly detected!");
        } else {
            anomalyResult += std::string("Everything fine, no anomaly detected!");
        }

        platform.data_psn->present_data_text(
                anomalyScore.c_str(), anomalyScore.size(),
                dataPsnTxtStartX1, rowIdx1, false);

        info("%s\n", anomalyScore.c_str());
        info("%s\n", anomalyThreshold.c_str());
        info("%s\n", anomalyResult.c_str());

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
     */
    template<class T>
    std::function<void (std::vector<int16_t>&, size_t, bool, size_t, size_t)>
    FeatureCalc(TfLiteTensor* inputTensor, size_t cacheSize,
                std::function<std::vector<T> (std::vector<int16_t>& )> compute)
    {
        /* Feature cache to be captured by lambda function*/
        static std::vector<std::vector<T>> featureCache = std::vector<std::vector<T>>(cacheSize);

        return [=](std::vector<int16_t>& audioDataWindow,
                   size_t index,
                   bool useCache,
                   size_t featuresOverlapIndex,
                   size_t resizeScale)
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
            auto size = features.size() / resizeScale;
            auto sizeBytes = sizeof(T);

            /* Input should be transposed and "resized" by skipping elements. */
            for (size_t outIndex = 0; outIndex < size; outIndex++) {
                std::memcpy(tensorData + (outIndex*size) + index, &features[outIndex*resizeScale], sizeBytes);
            }

            /* Start renewing cache as soon iteration goes out of the windows overlap. */
            if (index >= featuresOverlapIndex / resizeScale) {
                featureCache[index - featuresOverlapIndex / resizeScale] = std::move(features);
            }
        };
    }

    template std::function<void (std::vector<int16_t>&, size_t , bool, size_t, size_t)>
    FeatureCalc<int8_t>(TfLiteTensor* inputTensor,
                        size_t cacheSize,
                        std::function<std::vector<int8_t> (std::vector<int16_t>&)> compute);

    template std::function<void (std::vector<int16_t>&, size_t , bool, size_t, size_t)>
    FeatureCalc<uint8_t>(TfLiteTensor* inputTensor,
                         size_t cacheSize,
                         std::function<std::vector<uint8_t> (std::vector<int16_t>&)> compute);

    template std::function<void (std::vector<int16_t>&, size_t , bool, size_t, size_t)>
    FeatureCalc<int16_t>(TfLiteTensor* inputTensor,
                         size_t cacheSize,
                         std::function<std::vector<int16_t> (std::vector<int16_t>&)> compute);

    template std::function<void(std::vector<int16_t>&, size_t, bool, size_t, size_t)>
    FeatureCalc<float>(TfLiteTensor *inputTensor,
                       size_t cacheSize,
                       std::function<std::vector<float>(std::vector<int16_t>&)> compute);


    static std::function<void (std::vector<int16_t>&, int, bool, size_t, size_t)>
    GetFeatureCalculator(audio::AdMelSpectrogram& melSpec, TfLiteTensor* inputTensor, size_t cacheSize, float trainingMean)
    {
        std::function<void (std::vector<int16_t>&, size_t, bool, size_t, size_t)> melSpecFeatureCalc;

        TfLiteQuantization quant = inputTensor->quantization;

        if (kTfLiteAffineQuantization == quant.type) {

            auto *quantParams = (TfLiteAffineQuantization *) quant.params;
            const float quantScale = quantParams->scale->data[0];
            const int quantOffset = quantParams->zero_point->data[0];

            switch (inputTensor->type) {
                case kTfLiteInt8: {
                    melSpecFeatureCalc = FeatureCalc<int8_t>(inputTensor,
                                                             cacheSize,
                                                             [=, &melSpec](std::vector<int16_t>& audioDataWindow) {
                                                                 return melSpec.MelSpecComputeQuant<int8_t>(
                                                                         audioDataWindow,
                                                                         quantScale,
                                                                         quantOffset,
                                                                         trainingMean);
                                                             }
                    );
                    break;
                }
                case kTfLiteUInt8: {
                    melSpecFeatureCalc = FeatureCalc<uint8_t>(inputTensor,
                                                              cacheSize,
                                                              [=, &melSpec](std::vector<int16_t>& audioDataWindow) {
                                                                  return melSpec.MelSpecComputeQuant<uint8_t>(
                                                                          audioDataWindow,
                                                                          quantScale,
                                                                          quantOffset,
                                                                          trainingMean);
                                                              }
                    );
                    break;
                }
                case kTfLiteInt16: {
                    melSpecFeatureCalc = FeatureCalc<int16_t>(inputTensor,
                                                              cacheSize,
                                                              [=, &melSpec](std::vector<int16_t>& audioDataWindow) {
                                                                  return melSpec.MelSpecComputeQuant<int16_t>(
                                                                          audioDataWindow,
                                                                          quantScale,
                                                                          quantOffset,
                                                                          trainingMean);
                                                              }
                    );
                    break;
                }
                default:
                printf_err("Tensor type %s not supported\n", TfLiteTypeGetName(inputTensor->type));
            }


        } else {
            melSpecFeatureCalc = melSpecFeatureCalc = FeatureCalc<float>(inputTensor,
                                                                         cacheSize,
                                                                         [=, &melSpec](
                                                                                 std::vector<int16_t>& audioDataWindow) {
                                                                             return melSpec.ComputeMelSpec(
                                                                                     audioDataWindow,
                                                                                     trainingMean);
                                                                         });
        }
        return melSpecFeatureCalc;
    }

} /* namespace app */
} /* namespace arm */
