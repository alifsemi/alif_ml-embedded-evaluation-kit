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
#include "VisualWakeWordProcessing.hpp"

#include "ImageUtils.hpp"
#include "VisualWakeWordModel.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    VisualWakeWordPreProcess::VisualWakeWordPreProcess(TfLiteTensor* inputTensor, bool rgb2Gray)
    :m_inputTensor{inputTensor},
     m_rgb2Gray{rgb2Gray}
    {}

    bool VisualWakeWordPreProcess::DoPreProcess(const void* data, size_t inputSize)
    {
        if (data == nullptr) {
            printf_err("Data pointer is null");
        }

        auto input = static_cast<const uint8_t*>(data);

        uint8_t* unsignedDstPtr = this->m_inputTensor->data.uint8;

        if (this->m_rgb2Gray) {
            image::RgbToGrayscale(input, unsignedDstPtr, inputSize);
        } else {
            std::memcpy(unsignedDstPtr, input, inputSize);
        }

        /* VWW model pre-processing is image conversion from uint8 to [0,1] float values,
         * then quantize them with input quantization info. */
        QuantParams inQuantParams = GetTensorQuantParams(this->m_inputTensor);

        int8_t* signedDstPtr = this->m_inputTensor->data.int8;
        for (size_t i = 0; i < this->m_inputTensor->bytes; i++) {
            auto i_data_int8 = static_cast<int8_t>(
                    ((static_cast<float>(unsignedDstPtr[i]) / 255.0f) / inQuantParams.scale) + inQuantParams.offset
                    );
            signedDstPtr[i] = std::min<int8_t>(INT8_MAX, std::max<int8_t>(i_data_int8, INT8_MIN));
        }

        debug("Input tensor populated \n");

        return true;
    }

    VisualWakeWordPostProcess::VisualWakeWordPostProcess(TfLiteTensor* outputTensor, Classifier& classifier,
            const std::vector<std::string>& labels, std::vector<ClassificationResult>& results)
            :m_outputTensor{outputTensor},
             m_vwwClassifier{classifier},
             m_labels{labels},
             m_results{results}
    {}

    bool VisualWakeWordPostProcess::DoPostProcess()
    {
        return this->m_vwwClassifier.GetClassificationResults(
                this->m_outputTensor, this->m_results,
                this->m_labels, 1, true);
    }

} /* namespace app */
} /* namespace arm */