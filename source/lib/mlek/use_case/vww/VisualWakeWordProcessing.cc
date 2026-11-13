/*
 * SPDX-FileCopyrightText: Copyright 2022, 2025-2026 Arm Limited and/or
 * its affiliates <open-source-office@arm.com>
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
#include "mlek/use_case/vww/VisualWakeWordProcessing.hpp"

#include "mlek/common/ImageUtils.hpp"
#include "mlek/log/log_macros.h"
#include <cstring>

namespace arm {
namespace app {

    VisualWakeWordPreProcess::VisualWakeWordPreProcess(
        std::shared_ptr<fwk::iface::TensorIface> inputTensor, bool rgb2Gray) :
        m_inputTensor{inputTensor}, m_rgb2Gray{rgb2Gray}
    {}

    bool VisualWakeWordPreProcess::DoPreProcess(const void* data, size_t inputSize)
    {
        if (data == nullptr) {
            printf_err("Data pointer is null");
        }

        auto input = static_cast<const uint8_t*>(data);

        auto* unsignedDstPtr = this->m_inputTensor->GetData<uint8_t>();

        if (this->m_rgb2Gray) {
            image::RgbToGrayscale(input, unsignedDstPtr, inputSize);
        } else {
            std::memcpy(unsignedDstPtr, input, inputSize);
        }

        auto inQuantParams = this->m_inputTensor->GetQuantParams();
        image::ConvertUint8ToInt8(unsignedDstPtr, this->m_inputTensor->Bytes(), inQuantParams);

        debug("Input tensor populated \n");

        return true;
    }

    VisualWakeWordPostProcess::VisualWakeWordPostProcess(
        std::shared_ptr<fwk::iface::TensorIface> outputTensor,
        Classifier& classifier,
        const std::vector<std::string>& labels,
        std::vector<ClassificationResult>& results) :
        m_outputTensor{outputTensor}, m_vwwClassifier{classifier}, m_labels{labels},
        m_results{results}
    {}

    bool VisualWakeWordPostProcess::DoPostProcess()
    {
        return this->m_vwwClassifier.GetClassificationResults(
            this->m_outputTensor, this->m_results, this->m_labels, 1, true);
    }

} /* namespace app */
} /* namespace arm */
