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
#ifndef ASR_CLASSIFIER_HPP
#define ASR_CLASSIFIER_HPP

#include "Classifier.hpp"

namespace arm {
namespace app {

    class AsrClassifier : public Classifier {
    public:
        /**
         * @brief       Gets the top N classification results from the
         *              output vector.
         * @param[in]   outputTensor   Inference output tensor from an NN model.
         * @param[out]  vecResults     A vector of classification results
         *                             populated by this function.
         * @param[in]   labels         Labels vector to match classified classes
         * @param[in]   topNCount      Number of top classifications to pick.
         * @param[in]   use_softmax    Whether softmax scaling should be applied to model output.
         * @return      true if successful, false otherwise.
         **/
        bool GetClassificationResults(TfLiteTensor* outputTensor,
                                      std::vector<ClassificationResult>& vecResults,
                                      const std::vector<std::string>& labels,
                                      uint32_t topNCount, bool use_softmax = false) override;

    private:
        /**
         * @brief       Utility function that gets the top 1 classification results from the
         *              output tensor (vector of vector).
         * @param[in]   tensor       Inference output tensor from an NN model.
         * @param[out]  vecResults   Vector of classification results populated by this function.
         * @param[in]   labels       Labels vector to match classified classes.
         * @param[in]   scale        Quantization scale.
         * @param[in]   zeroPoint    Quantization zero point.
         * @return      true if successful, false otherwise.
         **/
        template<typename T>
        bool GetTopResults(TfLiteTensor* tensor,
                           std::vector<ClassificationResult>& vecResults,
                           const std::vector<std::string>& labels, double scale, double zeroPoint);
    };

} /* namespace app */
} /* namespace arm */

#endif /* ASR_CLASSIFIER_HPP */