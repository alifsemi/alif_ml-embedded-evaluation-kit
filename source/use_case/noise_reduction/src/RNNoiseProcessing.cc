/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#include "RNNoiseProcessing.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    RNNoisePreProcess::RNNoisePreProcess(TfLiteTensor* inputTensor,
            std::shared_ptr<rnn::RNNoiseFeatureProcessor> featureProcessor, std::shared_ptr<rnn::FrameFeatures> frameFeatures)
    :   m_inputTensor{inputTensor},
        m_featureProcessor{featureProcessor},
        m_frameFeatures{frameFeatures}
    {}

    bool RNNoisePreProcess::DoPreProcess(const void* data, size_t inputSize)
    {
        if (data == nullptr) {
            printf_err("Data pointer is null");
            return false;
        }

        auto input = static_cast<const int16_t*>(data);
        this->m_audioFrame = rnn::vec1D32F(input, input + inputSize);
        m_featureProcessor->PreprocessFrame(this->m_audioFrame.data(), inputSize, *this->m_frameFeatures);

        QuantizeAndPopulateInput(this->m_frameFeatures->m_featuresVec,
                this->m_inputTensor->params.scale, this->m_inputTensor->params.zero_point,
                this->m_inputTensor);

        debug("Input tensor populated \n");

        return true;
    }

    void RNNoisePreProcess::QuantizeAndPopulateInput(rnn::vec1D32F& inputFeatures,
            const float quantScale, const int quantOffset,
            TfLiteTensor* inputTensor)
    {
        const float minVal = std::numeric_limits<int8_t>::min();
        const float maxVal = std::numeric_limits<int8_t>::max();

        auto* inputTensorData = tflite::GetTensorData<int8_t>(inputTensor);

        for (size_t i=0; i < inputFeatures.size(); ++i) {
            float quantValue = ((inputFeatures[i] / quantScale) + quantOffset);
            inputTensorData[i] = static_cast<int8_t>(std::min<float>(std::max<float>(quantValue, minVal), maxVal));
        }
    }

    RNNoisePostProcess::RNNoisePostProcess(TfLiteTensor* outputTensor,
            std::vector<int16_t>& denoisedAudioFrame,
            std::shared_ptr<rnn::RNNoiseFeatureProcessor> featureProcessor,
            std::shared_ptr<rnn::FrameFeatures> frameFeatures)
    :   m_outputTensor{outputTensor},
        m_denoisedAudioFrame{denoisedAudioFrame},
        m_featureProcessor{featureProcessor},
        m_frameFeatures{frameFeatures}
        {
            this->m_denoisedAudioFrameFloat.reserve(denoisedAudioFrame.size());
            this->m_modelOutputFloat.resize(outputTensor->bytes);
        }

    bool RNNoisePostProcess::DoPostProcess()
    {
        const auto* outputData = tflite::GetTensorData<int8_t>(this->m_outputTensor);
        auto outputQuantParams = GetTensorQuantParams(this->m_outputTensor);

        for (size_t i = 0; i < this->m_outputTensor->bytes; ++i) {
            this->m_modelOutputFloat[i] = (static_cast<float>(outputData[i]) - outputQuantParams.offset)
                                  * outputQuantParams.scale;
        }

        this->m_featureProcessor->PostProcessFrame(this->m_modelOutputFloat,
                *this->m_frameFeatures, this->m_denoisedAudioFrameFloat);

        for (size_t i = 0; i < this->m_denoisedAudioFrame.size(); ++i) {
            this->m_denoisedAudioFrame[i] = static_cast<int16_t>(
                    std::roundf(this->m_denoisedAudioFrameFloat[i]));
        }

        return true;
    }

} /* namespace app */
} /* namespace arm */