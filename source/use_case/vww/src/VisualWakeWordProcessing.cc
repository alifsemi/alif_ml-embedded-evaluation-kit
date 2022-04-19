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
#include "VisualWakeWordProcessing.hpp"
#include "ImageUtils.hpp"
#include "VisualWakeWordModel.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    VisualWakeWordPreProcess::VisualWakeWordPreProcess(Model* model)
    {
        if (!model->IsInited()) {
            printf_err("Model is not initialised!.\n");
        }
        this->m_model = model;
    }

    bool VisualWakeWordPreProcess::DoPreProcess(const void* data, size_t inputSize)
    {
        if (data == nullptr) {
            printf_err("Data pointer is null");
        }

        auto input = static_cast<const uint8_t*>(data);
        TfLiteTensor* inputTensor = this->m_model->GetInputTensor(0);

        auto unsignedDstPtr = static_cast<uint8_t*>(inputTensor->data.data);

        /* VWW model has one channel input => Convert image to grayscale here.
         * We expect images to always be RGB. */
        image::RgbToGrayscale(input, unsignedDstPtr, inputSize);

        /* VWW model pre-processing is image conversion from uint8 to [0,1] float values,
         * then quantize them with input quantization info. */
        QuantParams inQuantParams = GetTensorQuantParams(inputTensor);

        auto signedDstPtr = static_cast<int8_t*>(inputTensor->data.data);
        for (size_t i = 0; i < inputTensor->bytes; i++) {
            auto i_data_int8 = static_cast<int8_t>(
                    ((static_cast<float>(unsignedDstPtr[i]) / 255.0f) / inQuantParams.scale) + inQuantParams.offset
                    );
            signedDstPtr[i] = std::min<int8_t>(INT8_MAX, std::max<int8_t>(i_data_int8, INT8_MIN));
        }

        debug("Input tensor populated \n");

        return true;
    }

    VisualWakeWordPostProcess::VisualWakeWordPostProcess(Classifier& classifier, Model* model,
            const std::vector<std::string>& labels, std::vector<ClassificationResult>& results)
            :m_vwwClassifier{classifier},
             m_labels{labels},
             m_results{results}
    {
        if (!model->IsInited()) {
            printf_err("Model is not initialised!.\n");
        }
        this->m_model = model;
    }

    bool VisualWakeWordPostProcess::DoPostProcess()
    {
        return this->m_vwwClassifier.GetClassificationResults(
                this->m_model->GetOutputTensor(0), this->m_results,
                this->m_labels, 1, true);
    }

} /* namespace app */
} /* namespace arm */