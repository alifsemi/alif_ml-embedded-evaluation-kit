/*
 * SPDX-FileCopyrightText: Copyright 2022, 2025-2026 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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
#include "mlek/use_case/img_class/ImgClassProcessing.hpp"

#include "mlek/common/ImageUtils.hpp"
#include "mlek/log/log_macros.h"
#include <cassert>
#include <cstring>

namespace arm {
namespace app {

    ImgClassPreProcess::ImgClassPreProcess(
        const std::shared_ptr<fwk::iface::TensorIface> inputTensor,
        const std::array<float, kNumChannels> mean,
        const std::array<float, kNumChannels> stddev) :
        m_inputTensor{inputTensor}, m_mean{mean}, m_stddev{stddev}
    {}

    bool ImgClassPreProcess::DoPreProcess(const void* data, size_t inputSize)
    {
        if (data == nullptr) {
            printf_err("Data pointer is null");
            return false;
        }

        auto src = static_cast<const uint8_t*>(data);

        switch (this->m_inputTensor->Type()) {
        case fwk::iface::TensorType::INT8:
            assert(inputSize == this->m_inputTensor->Bytes());
            image::ConvertUint8ToInt8(this->m_inputTensor->GetData<int8_t>(),
                                      src,
                                      this->m_inputTensor->GetNumElements(),
                                      this->m_inputTensor->Layout(),
                                      this->m_inputTensor->GetQuantParams());
            break;
        case fwk::iface::TensorType::UINT8:
            assert(inputSize == this->m_inputTensor->Bytes());
            std::memcpy(this->m_inputTensor->GetData(), src, inputSize);
            break;
        case fwk::iface::TensorType::FP32:
            assert(inputSize * sizeof(float) == this->m_inputTensor->Bytes());
            return image::ConvertUint8ToFp32<kNumChannels>(this->m_inputTensor->GetData<float>(),
                                                           src,
                                                           this->m_inputTensor->GetNumElements(),
                                                           this->m_inputTensor->Layout(),
                                                           this->m_mean,
                                                           this->m_stddev);
        default:
            return false;
        }

        debug("Input tensor populated \n");
        return true;
    }

    ImgClassPostProcess::ImgClassPostProcess(
        const std::shared_ptr<fwk::iface::TensorIface> outputTensor,
        Classifier& classifier,
        const std::vector<std::string>& labels,
        std::vector<ClassificationResult>& results) :
        m_outputTensor{outputTensor}, m_imgClassifier{classifier}, m_labels{labels},
        m_results{results}
    {}

    bool ImgClassPostProcess::DoPostProcess()
    {
        return this->m_imgClassifier.GetClassificationResults(
            this->m_outputTensor, this->m_results, this->m_labels, 5, false);
    }

} /* namespace app */
} /* namespace arm */
