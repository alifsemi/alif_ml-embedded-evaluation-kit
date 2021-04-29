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
#include "Classifier.hpp"

#include "hal.h"
#include "TensorFlowLiteMicro.hpp"

#include <vector>
#include <string>
#include <set>
#include <cstdint>

namespace arm {
namespace app {

    template<typename T>
    void SetVectorResults(std::set<std::pair<T, uint32_t>>& topNSet,
                          std::vector<ClassificationResult>& vecResults,
                          TfLiteTensor* tensor,
                          const std::vector <std::string>& labels) {

        /* For getting the floating point values, we need quantization parameters. */
        QuantParams quantParams = GetTensorQuantParams(tensor);

        /* Reset the iterator to the largest element - use reverse iterator. */
        auto topNIter = topNSet.rbegin();
        for (size_t i = 0; i < vecResults.size() && topNIter != topNSet.rend(); ++i, ++topNIter) {
            T score = topNIter->first;
            vecResults[i].m_normalisedVal = quantParams.scale * (score - quantParams.offset);
            vecResults[i].m_label = labels[topNIter->second];
            vecResults[i].m_labelIdx = topNIter->second;
        }

    }

    template<>
    void SetVectorResults<float>(std::set<std::pair<float, uint32_t>>& topNSet,
                          std::vector<ClassificationResult>& vecResults,
                          TfLiteTensor* tensor,
                          const std::vector <std::string>& labels) {
        UNUSED(tensor);
        /* Reset the iterator to the largest element - use reverse iterator. */
        auto topNIter = topNSet.rbegin();
        for (size_t i = 0; i < vecResults.size() && topNIter != topNSet.rend(); ++i, ++topNIter) {
            vecResults[i].m_normalisedVal = topNIter->first;
            vecResults[i].m_label = labels[topNIter->second];
            vecResults[i].m_labelIdx = topNIter->second;
        }

    }

    template<typename T>
    bool Classifier::GetTopNResults(TfLiteTensor* tensor,
                                    std::vector<ClassificationResult>& vecResults,
                                    uint32_t topNCount,
                                    const std::vector <std::string>& labels)
    {
        std::set<std::pair<T, uint32_t>> sortedSet;

        /* NOTE: inputVec's size verification against labels should be
         *       checked by the calling/public function. */
        T* tensorData = tflite::GetTensorData<T>(tensor);

        /* Set initial elements. */
        for (uint32_t i = 0; i < topNCount; ++i) {
            sortedSet.insert({tensorData[i], i});
        }

        /* Initialise iterator. */
        auto setFwdIter = sortedSet.begin();

        /* Scan through the rest of elements with compare operations. */
        for (uint32_t i = topNCount; i < labels.size(); ++i) {
            if (setFwdIter->first < tensorData[i]) {
                sortedSet.erase(*setFwdIter);
                sortedSet.insert({tensorData[i], i});
                setFwdIter = sortedSet.begin();
            }
        }

        /* Final results' container. */
        vecResults = std::vector<ClassificationResult>(topNCount);

        SetVectorResults<T>(sortedSet, vecResults, tensor, labels);

        return true;
    }

    template bool  Classifier::GetTopNResults<uint8_t>(TfLiteTensor* tensor,
                                                       std::vector<ClassificationResult>& vecResults,
                                                       uint32_t topNCount, const std::vector <std::string>& labels);

    template bool  Classifier::GetTopNResults<int8_t>(TfLiteTensor* tensor,
                                                      std::vector<ClassificationResult>& vecResults,
                                                      uint32_t topNCount, const std::vector <std::string>& labels);

    bool  Classifier::GetClassificationResults(
        TfLiteTensor* outputTensor,
        std::vector<ClassificationResult>& vecResults,
        const std::vector <std::string>& labels, uint32_t topNCount)
    {
        if (outputTensor == nullptr) {
            printf_err("Output vector is null pointer.\n");
            return false;
        }

        uint32_t totalOutputSize = 1;
        for (int inputDim = 0; inputDim < outputTensor->dims->size; inputDim++){
            totalOutputSize *= outputTensor->dims->data[inputDim];
        }

        /* Sanity checks. */
        if (totalOutputSize < topNCount) {
            printf_err("Output vector is smaller than %u\n", topNCount);
            return false;
        } else if (totalOutputSize != labels.size()) {
            printf_err("Output size doesn't match the labels' size\n");
            return false;
        } else if (topNCount == 0) {
            printf_err("Top N results cannot be zero\n");
            return false;
        }

        bool resultState;
        vecResults.clear();

        /* Get the top N results. */
        switch (outputTensor->type) {
            case kTfLiteUInt8:
                resultState = GetTopNResults<uint8_t>(outputTensor, vecResults, topNCount, labels);
                break;
            case kTfLiteInt8:
                resultState = GetTopNResults<int8_t>(outputTensor, vecResults, topNCount, labels);
                break;
            case kTfLiteFloat32:
                resultState = GetTopNResults<float>(outputTensor, vecResults, topNCount, labels);
                break;
            default:
                printf_err("Tensor type %s not supported by classifier\n", TfLiteTypeGetName(outputTensor->type));
                return false;
        }

        if (!resultState) {
            printf_err("Failed to get top N results set\n");
            return false;
        }

        return true;
    }

} /* namespace app */
} /* namespace arm */