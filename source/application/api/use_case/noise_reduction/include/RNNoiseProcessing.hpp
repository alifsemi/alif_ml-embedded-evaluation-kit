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
#ifndef RNNOISE_PROCESSING_HPP
#define RNNOISE_PROCESSING_HPP

#include "BaseProcessing.hpp"
#include "Model.hpp"
#include "RNNoiseFeatureProcessor.hpp"

namespace arm {
namespace app {

    /**
     * @brief   Pre-processing class for Noise Reduction use case.
     *          Implements methods declared by BasePreProcess and anything else needed
     *          to populate input tensors ready for inference.
     */
    class RNNoisePreProcess : public BasePreProcess {

    public:
        /**
         * @brief           Constructor
         * @param[in]       inputTensor        Pointer to the TFLite Micro input Tensor.
         * @param[in/out]   featureProcessor   RNNoise specific feature extractor object.
         * @param[in/out]   frameFeatures      RNNoise specific features shared between pre & post-processing.
         *
         **/
        explicit RNNoisePreProcess(TfLiteTensor* inputTensor,
                                   std::shared_ptr<rnn::RNNoiseFeatureProcessor> featureProcessor,
                                   std::shared_ptr<rnn::FrameFeatures> frameFeatures);

        /**
         * @brief       Should perform pre-processing of 'raw' input audio data and load it into
         *              TFLite Micro input tensors ready for inference
         * @param[in]   input      Pointer to the data that pre-processing will work on.
         * @param[in]   inputSize  Size of the input data.
         * @return      true if successful, false otherwise.
         **/
        bool DoPreProcess(const void* input, size_t inputSize) override;

    private:
        TfLiteTensor* m_inputTensor;                        /* Model input tensor. */
        std::shared_ptr<rnn::RNNoiseFeatureProcessor> m_featureProcessor;   /* RNNoise feature processor shared between pre & post-processing. */
        std::shared_ptr<rnn::FrameFeatures> m_frameFeatures;                /* RNNoise features shared between pre & post-processing. */
        rnn::vec1D32F m_audioFrame;                         /* Audio frame cast to FP32 */

        /**
         * @brief            Quantize the given features and populate the input Tensor.
         * @param[in]        inputFeatures   Vector of floating point features to quantize.
         * @param[in]        quantScale      Quantization scale for the inputTensor.
         * @param[in]        quantOffset     Quantization offset for the inputTensor.
         * @param[in,out]    inputTensor     TFLite micro tensor to populate.
         **/
        static void QuantizeAndPopulateInput(rnn::vec1D32F& inputFeatures,
                float quantScale, int quantOffset,
                TfLiteTensor* inputTensor);
    };

    /**
     * @brief   Post-processing class for Noise Reduction use case.
     *          Implements methods declared by BasePostProcess and anything else needed
     *          to populate result vector.
     */
    class RNNoisePostProcess : public BasePostProcess {

    public:
        /**
         * @brief           Constructor
         * @param[in]       outputTensor         Pointer to the TFLite Micro output Tensor.
         * @param[out]      denoisedAudioFrame   Vector to store the final denoised audio frame.
         * @param[in/out]   featureProcessor     RNNoise specific feature extractor object.
         * @param[in/out]   frameFeatures        RNNoise specific features shared between pre & post-processing.
         **/
        RNNoisePostProcess(TfLiteTensor* outputTensor,
                           std::vector<int16_t>& denoisedAudioFrame,
                           std::shared_ptr<rnn::RNNoiseFeatureProcessor> featureProcessor,
                           std::shared_ptr<rnn::FrameFeatures> frameFeatures);

        /**
         * @brief       Should perform post-processing of the result of inference then
         *              populate result data for any later use.
         * @return      true if successful, false otherwise.
         **/
        bool DoPostProcess() override;

    private:
        TfLiteTensor* m_outputTensor;                       /* Model output tensor. */
        std::vector<int16_t>& m_denoisedAudioFrame;         /* Vector to store the final denoised frame. */
        rnn::vec1D32F m_denoisedAudioFrameFloat;            /* Internal vector to store the final denoised frame (FP32). */
        std::shared_ptr<rnn::RNNoiseFeatureProcessor> m_featureProcessor;   /* RNNoise feature processor shared between pre & post-processing. */
        std::shared_ptr<rnn::FrameFeatures> m_frameFeatures;                /* RNNoise features shared between pre & post-processing. */
        std::vector<float> m_modelOutputFloat;              /* Internal vector to store de-quantized model output. */

    };

} /* namespace app */
} /* namespace arm */

#endif /* RNNOISE_PROCESSING_HPP */