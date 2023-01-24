/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef AD_PROCESSING_HPP
#define AD_PROCESSING_HPP

#include "BaseProcessing.hpp"
#include "TensorFlowLiteMicro.hpp"
#include "AudioUtils.hpp"
#include "AdMelSpectrogram.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    /**
     * @brief   Pre-processing class for anomaly detection use case.
     *          Implements methods declared by BasePreProcess and anything else needed
     *          to populate input tensors ready for inference.
     */
    class AdPreProcess : public BasePreProcess {

    public:
        /**
         * @brief Constructor for AdPreProcess class objects
         * @param[in] inputTensor  input tensor pointer from the tensor arena.
         * @param[in] melSpectrogramFrameLen MEL spectrogram's frame length
         * @param[in] melSpectrogramFrameStride MEL spectrogram's frame stride
         * @param[in] adModelTrainingMean Training mean for the Anomaly detection model being used.
         */
        explicit AdPreProcess(TfLiteTensor* inputTensor,
                              uint32_t melSpectrogramFrameLen,
                              uint32_t melSpectrogramFrameStride,
                              float adModelTrainingMean);

        ~AdPreProcess() = default;

        /**
         * @brief Function to invoke pre-processing and populate the input vector
         * @param input pointer to input data. For anomaly detection, this is the pointer to
         *              the audio data.
         * @param inputSize Size of the data being passed in for pre-processing.
         * @return True if successful, false otherwise.
         */
        bool DoPreProcess(const void* input, size_t inputSize) override;

        /**
         * @brief Getter function for audio window size computed when constructing
         *        the class object.
         * @return Audio window size as 32 bit unsigned integer.
         */
        uint32_t GetAudioWindowSize();

        /**
         * @brief Getter function for audio window stride computed when constructing
         *        the class object.
         * @return Audio window stride as 32 bit unsigned integer.
         */
        uint32_t GetAudioDataStride();

        /**
         * @brief Setter function for current audio index. This is only used for evaluating
         *        if previously computed features can be re-used from cache.
         */
        void SetAudioWindowIndex(uint32_t idx);

    private:
        bool        m_validInstance{false}; /**< Indicates the current object is valid. */
        uint32_t    m_melSpectrogramFrameLen{}; /**< MEL spectrogram's window frame length */
        uint32_t    m_melSpectrogramFrameStride{}; /**< MEL spectrogram's window frame stride */
        uint8_t     m_inputResizeScale{}; /**< Downscaling factor for the MEL energy matrix. */
        uint32_t    m_numMelSpecVectorsInAudioStride{};  /**< Number of frames to move across the audio. */
        uint32_t    m_audioDataWindowSize{}; /**< Audio window size computed based on other parameters. */
        uint32_t    m_audioDataStride{}; /**< Audio window stride computed. */
        uint32_t    m_numReusedFeatureVectors{}; /**< Number of MEL vectors that can be re-used */
        uint32_t    m_audioWindowIndex{}; /**< Current audio window index (from audio's sliding window) */

        audio::SlidingWindow<const int16_t> m_melWindowSlider; /**< Internal MEL spectrogram window slider */
        audio::AdMelSpectrogram m_melSpec; /**< MEL spectrogram computation object */
        std::function<void
            (std::vector<int16_t>&, int, bool, size_t, size_t)> m_featureCalc; /**< Feature calculator object */
    };

    class AdPostProcess : public BasePostProcess {
    public:
        /**
         * @brief Constructor for AdPostProcess object.
         * @param[in] outputTensor Output tensor pointer.
         */
        explicit AdPostProcess(TfLiteTensor* outputTensor);

        ~AdPostProcess() = default;

        /**
         * @brief Function to do the post-processing on the output tensor.
         * @return True if successful, false otherwise.
         */
        bool DoPostProcess() override;

        /**
         * @brief Getter function for an element from the de-quantised output vector.
         * @param index Index of the element to be retrieved.
         * @return index represented as a 32 bit floating point number.
         */
        float GetOutputValue(uint32_t index);

    private:
        TfLiteTensor* m_outputTensor{}; /**< Output tensor pointer */
        std::vector<float> m_dequantizedOutputVec{}; /**< Internal output vector */

        /**
         * @brief De-quantizes and flattens the output tensor into a vector.
         * @tparam T template parameter to indicate data type.
         * @return True if successful, false otherwise.
         */
        template<typename T>
        bool Dequantize()
        {
            TfLiteTensor* tensor = this->m_outputTensor;
            if (tensor == nullptr) {
                printf_err("Invalid output tensor.\n");
                return false;
            }
            T* tensorData = tflite::GetTensorData<T>(tensor);

            uint32_t totalOutputSize = 1;
            for (int inputDim = 0; inputDim < tensor->dims->size; inputDim++){
                totalOutputSize *= tensor->dims->data[inputDim];
            }

            /* For getting the floating point values, we need quantization parameters */
            QuantParams quantParams = GetTensorQuantParams(tensor);

            this->m_dequantizedOutputVec = std::vector<float>(totalOutputSize, 0);

            for (size_t i = 0; i < totalOutputSize; ++i) {
                this->m_dequantizedOutputVec[i] = quantParams.scale * (tensorData[i] - quantParams.offset);
            }

            return true;
        }
    };

    /* Templated instances available: */
    template bool AdPostProcess::Dequantize<int8_t>();

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
            T* tensorData = tflite::GetTensorData<T>(inputTensor);
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

    template std::function<void(std::vector<int16_t>&, size_t, bool, size_t, size_t)>
    FeatureCalc<float>(TfLiteTensor *inputTensor,
                       size_t cacheSize,
                       std::function<std::vector<float>(std::vector<int16_t>&)> compute);

    std::function<void (std::vector<int16_t>&, int, bool, size_t, size_t)>
    GetFeatureCalculator(audio::AdMelSpectrogram& melSpec,
                         TfLiteTensor* inputTensor,
                         size_t cacheSize,
                         float trainingMean);

} /* namespace app */
} /* namespace arm */

#endif /* AD_PROCESSING_HPP */
