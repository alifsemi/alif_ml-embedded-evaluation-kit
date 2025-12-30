/*
 * SPDX-FileCopyrightText: Copyright 2021,2023, 2025 Arm Limited and/or
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
#include "mlek/use_case/asr/AsrClassifier.hpp"

#include "mlek/fwk/iface/Tensor.hpp"
#include "mlek/log/log_macros.h"

namespace arm {
namespace app {

    AsrClassifier::AsrClassifier(uint32_t inputRowsIdx,
                                 uint32_t inputColsIdx,
                                 uint32_t outputRowsIdx,
                                 uint32_t outputColsIdx) :
        m_inputTensorRowsIdx(inputRowsIdx), m_inputTensorColsIdx(inputColsIdx),
        m_outputTensorRowsIdx(outputRowsIdx), m_outputTensorColsIdx(outputColsIdx)
    {}

    template <typename T>
    bool AsrClassifier::GetTopResults(const std::shared_ptr<fwk::iface::TensorIface> tensor,
                                      std::vector<ClassificationResult>& vecResults,
                                      const std::vector<std::string>& labels,
                                      double scale,
                                      double zeroPoint)
    {
        const uint32_t nElems   = tensor->Shape()[this->m_outputTensorRowsIdx];
        const uint32_t nLetters = tensor->Shape()[this->m_outputTensorColsIdx];

        if (nLetters != labels.size()) {
            printf("Output size doesn't match the labels' size\n");
            return false;
        }

        /* NOTE: tensor's size verification against labels should be
         *       checked by the calling/public function. */
        if (nLetters < 1) {
            return false;
        }

        /* Final results' container. */
        vecResults = std::vector<ClassificationResult>(nElems);

        T* tensorData = tensor->GetData<T>();

        /* Get the top 1 results. */
        for (uint32_t i = 0, row = 0; i < nElems; ++i, row+=nLetters) {
            std::pair<T, uint32_t> top_1 = std::make_pair(tensorData[row + 0], 0);

            for (uint32_t j = 1; j < nLetters; ++j) {
                if (top_1.first < tensorData[row + j]) {
                    top_1.first = tensorData[row + j];
                    top_1.second = j;
                }
            }

            double score = static_cast<int> (top_1.first);
            vecResults[i].m_normalisedVal = scale * (score - zeroPoint);
            vecResults[i].m_label = labels[top_1.second];
            vecResults[i].m_labelIdx = top_1.second;
        }

        return true;
    }
    template bool
    AsrClassifier::GetTopResults<uint8_t>(const std::shared_ptr<fwk::iface::TensorIface> tensor,
                                          std::vector<ClassificationResult>& vecResults,
                                          const std::vector<std::string>& labels,
                                          double scale,
                                          double zeroPoint);
    template bool
    AsrClassifier::GetTopResults<int8_t>(const std::shared_ptr<fwk::iface::TensorIface> tensor,
                                         std::vector<ClassificationResult>& vecResults,
                                         const std::vector<std::string>& labels,
                                         double scale,
                                         double zeroPoint);

    bool AsrClassifier::GetClassificationResults(
        const std::shared_ptr<fwk::iface::TensorIface> outputTensor,
        std::vector<ClassificationResult>& vecResults,
        const std::vector<std::string>& labels,
        uint32_t topNCount,
        bool use_softmax)
    {
            UNUSED(use_softmax);
            vecResults.clear();

            const auto minTensorDims = (this->m_outputTensorRowsIdx > this->m_outputTensorColsIdx)
                                           ? this->m_outputTensorRowsIdx
                                           : this->m_outputTensorColsIdx;

            const uint32_t outColsIdx = this->m_outputTensorColsIdx;
            auto outputShape          = outputTensor->Shape();

            /* Health checks. */
            if (outputTensor == nullptr) {
                printf_err("Output vector is null pointer.\n");
                return false;
            } else if (outputShape.size() < minTensorDims) {
                printf_err("Output tensor expected to be %dD\n", minTensorDims);
                return false;
            } else if (outputShape[outColsIdx] < topNCount) {
                printf_err("Output vectors are smaller than %" PRIu32 "\n", topNCount);
                return false;
            } else if (outputShape[outColsIdx] != labels.size()) {
                printf("Output size doesn't match the labels' size\n");
                return false;
            }

            if (topNCount != 1) {
                warn("TopNCount value ignored in this implementation\n");
            }

            /* To return the floating point values, we need quantization parameters. */
            auto quantParams = outputTensor->GetQuantParams();

            bool resultState;

            switch (outputTensor->Type()) {
            case fwk::iface::TensorType::UINT8:
                resultState = this->GetTopResults<uint8_t>(
                    outputTensor, vecResults, labels, quantParams.scale, quantParams.offset);
                break;
            case fwk::iface::TensorType::INT8:
                resultState = this->GetTopResults<int8_t>(
                    outputTensor, vecResults, labels, quantParams.scale, quantParams.offset);
                break;
            default:
                printf_err("Tensor type %s not supported by classifier\n",
                           fwk::iface::GetTensorDataTypeName(outputTensor->Type()));
                return false;
            }

            if (!resultState) {
                printf_err("Failed to get sorted set\n");
                return false;
            }

            return true;
    }

} /* namespace app */
} /* namespace arm */
