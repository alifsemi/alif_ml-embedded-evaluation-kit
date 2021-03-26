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
    bool Classifier::_GetTopNResults(TfLiteTensor* tensor,
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

        /* For getting the floating point values, we need quantization parameters. */
        QuantParams quantParams = GetTensorQuantParams(tensor);

        /* Reset the iterator to the largest element - use reverse iterator. */
        auto setRevIter = sortedSet.rbegin();

        /* Populate results
         * Note: we could combine this loop with the loop above, but that
         *       would, involve more multiplications and other operations.
         **/
        for (size_t i = 0; i < vecResults.size(); ++i, ++setRevIter) {
            double score = static_cast<int> (setRevIter->first);
            vecResults[i].m_normalisedVal = quantParams.scale *
                                         (score - quantParams.offset);
            vecResults[i].m_label = labels[setRevIter->second];
            vecResults[i].m_labelIdx = setRevIter->second;
        }

        return true;
    }

    template<>
    bool Classifier::_GetTopNResults<float>(TfLiteTensor* tensor,
                                     std::vector<ClassificationResult>& vecResults,
                                     uint32_t topNCount,
                                     const std::vector <std::string>& labels)
    {
        std::set<std::pair<float, uint32_t>> sortedSet;

        /* NOTE: inputVec's size verification against labels should be
         *       checked by the calling/public function. */
        float* tensorData = tflite::GetTensorData<float>(tensor);

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

        /* Reset the iterator to the largest element - use reverse iterator. */
        auto setRevIter = sortedSet.rbegin();

        /* Populate results
         * Note: we could combine this loop with the loop above, but that
         *       would, involve more multiplications and other operations.
         **/
        for (size_t i = 0; i < vecResults.size(); ++i, ++setRevIter) {
            vecResults[i].m_normalisedVal = setRevIter->first;
            vecResults[i].m_label = labels[setRevIter->second];
            vecResults[i].m_labelIdx = setRevIter->second;
        }

        return true;
    }

    template bool  Classifier::_GetTopNResults<uint8_t>(TfLiteTensor* tensor,
                                           std::vector<ClassificationResult>& vecResults,
                                           uint32_t topNCount, const std::vector <std::string>& labels);

    template bool  Classifier::_GetTopNResults<int8_t>(TfLiteTensor* tensor,
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
        }

        bool resultState;
        vecResults.clear();

        /* Get the top N results. */
        switch (outputTensor->type) {
            case kTfLiteUInt8:
                resultState = _GetTopNResults<uint8_t>(outputTensor, vecResults, topNCount, labels);
                break;
            case kTfLiteInt8:
                resultState = _GetTopNResults<int8_t>(outputTensor, vecResults, topNCount, labels);
                break;
            case kTfLiteFloat32:
                resultState = _GetTopNResults<float>(outputTensor, vecResults, topNCount, labels);
                break;
            default:
                printf_err("Tensor type %s not supported by classifier\n", TfLiteTypeGetName(outputTensor->type));
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