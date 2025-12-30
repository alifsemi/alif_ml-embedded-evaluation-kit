/*
 * SPDX-FileCopyrightText: Copyright 2021-2023, 2025 Arm Limited and/or
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
#include "mlek/common/Classifier.hpp"

#include "mlek/math/PlatformMath.hpp"
#include "mlek/log/log_macros.h"

#include <vector>
#include <string>
#include <set>
#include <cstdint>
#include <cinttypes>


namespace arm {
namespace app {

    void Classifier::SetVectorResults(std::set<std::pair<float, uint32_t>>& topNSet,
            std::vector<ClassificationResult>& vecResults,
            const std::vector <std::string>& labels)
    {
        /* Reset the iterator to the largest element - use reverse iterator. */

        auto topNIter = topNSet.rbegin();
        for (size_t i = 0; i < vecResults.size() && topNIter != topNSet.rend(); ++i, ++topNIter) {
            vecResults[i].m_normalisedVal = topNIter->first;
            vecResults[i].m_label = labels[topNIter->second];
            vecResults[i].m_labelIdx = topNIter->second;
        }
    }

    bool Classifier::GetTopNResults(const std::vector<float>& tensor,
            std::vector<ClassificationResult>& vecResults,
            uint32_t topNCount, const std::vector <std::string>& labels)
    {
        std::set<std::pair<float , uint32_t>> sortedSet;

        /* NOTE: inputVec's size verification against labels should be
         *       checked by the calling/public function. */

        /* Set initial elements. */
        for (uint32_t i = 0; i < topNCount; ++i) {
            sortedSet.insert({tensor[i], i});
        }

        /* Initialise iterator. */
        auto setFwdIter = sortedSet.begin();

        /* Scan through the rest of elements with compare operations. */
        for (uint32_t i = topNCount; i < tensor.size(); ++i) {
            if (setFwdIter->first < tensor[i]) {
                sortedSet.erase(*setFwdIter);
                sortedSet.insert({tensor[i], i});
                setFwdIter = sortedSet.begin();
            }
        }

        /* Final results' container. */
        vecResults = std::vector<ClassificationResult>(topNCount);
        SetVectorResults(sortedSet, vecResults, labels);

        return true;
    }

    bool Classifier::GetClassificationResults(
        const std::shared_ptr<fwk::iface::TensorIface> outputTensor,
        std::vector<ClassificationResult>& vecResults,
        const std::vector<std::string>& labels,
        uint32_t topNCount,
        bool useSoftmax)
    {
        if (outputTensor == nullptr) {
            printf_err("Output vector is null pointer.\n");
            return false;
        }

        const uint32_t nOutputElements = outputTensor->GetNumElements();

        /* Health check */
        if (nOutputElements < topNCount) {
            printf_err("Output vector is smaller than %" PRIu32 "\n", topNCount);
            return false;
        } else if (nOutputElements != labels.size()) {
            printf_err("Output size doesn't match the labels' size\n");
            return false;
        } else if (topNCount == 0) {
            printf_err("Top N results cannot be zero\n");
            return false;
        }

        vecResults.clear();

        /* De-Quantize Output Tensor */
        auto quantParams = outputTensor->GetQuantParams();

        /* Floating point tensor data to be populated
         * NOTE: The assumption here is that the output tensor size isn't too
         * big and therefore, there's neglibible impact on heap usage. */
        std::vector<float> tensorData(nOutputElements);

        /* Populate the floating point buffer */
        switch (outputTensor->Type()) {
        case fwk::iface::TensorType::UINT8: {
            uint8_t* tensor_buffer = outputTensor->GetData<uint8_t>();
            for (size_t i = 0; i < nOutputElements; ++i) {
                tensorData[i] =
                    quantParams.scale * (static_cast<float>(tensor_buffer[i]) - quantParams.offset);
            }
            break;
        }
        case fwk::iface::TensorType::INT8: {
            int8_t* tensor_buffer = outputTensor->GetData<int8_t>();
            for (size_t i = 0; i < nOutputElements; ++i) {
                tensorData[i] =
                    quantParams.scale * (static_cast<float>(tensor_buffer[i]) - quantParams.offset);
            }
            break;
        }
        case fwk::iface::TensorType::FP32: {
            float* tensor_buffer = outputTensor->GetData<float>();
            for (size_t i = 0; i < nOutputElements; ++i) {
                tensorData[i] = tensor_buffer[i];
            }
            break;
        }
            default:
                printf_err("Tensor type %s not supported by classifier\n",
                           fwk::iface::GetTensorDataTypeName(outputTensor->Type()));
                return false;
            }

        if (useSoftmax) {
            math::MathUtils::SoftmaxF32(tensorData);
        }

        /* Get the top N results. */
        bool resultState = GetTopNResults(tensorData, vecResults, topNCount, labels);

        if (!resultState) {
            printf_err("Failed to get top N results set\n");
        }

        return resultState;
    }
} /* namespace app */
} /* namespace arm */
