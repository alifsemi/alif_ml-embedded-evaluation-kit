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
#ifndef IMG_CLASS_PROCESSING_HPP
#define IMG_CLASS_PROCESSING_HPP

#include "BaseProcessing.hpp"
#include "Model.hpp"
#include "Classifier.hpp"

namespace arm {
namespace app {

    /**
     * @brief   Pre-processing class for Image Classification use case.
     *          Implements methods declared by BasePreProcess and anything else needed
     *          to populate input tensors ready for inference.
     */
    class ImgClassPreProcess : public BasePreProcess {

    public:
        explicit ImgClassPreProcess(Model* model);

        bool DoPreProcess(const void* input, size_t inputSize) override;
    };

    /**
     * @brief   Post-processing class for Image Classification use case.
     *          Implements methods declared by BasePostProcess and anything else needed
     *          to populate result vector.
     */
    class ImgClassPostProcess : public BasePostProcess {

    private:
        Classifier& m_imgClassifier;
        const std::vector<std::string>& m_labels;
        std::vector<ClassificationResult>& m_results;

    public:
        ImgClassPostProcess(Classifier& classifier, Model* model,
                            const std::vector<std::string>& labels,
                            std::vector<ClassificationResult>& results);

        bool DoPostProcess() override;
    };

} /* namespace app */
} /* namespace arm */

#endif /* IMG_CLASS_PROCESSING_HPP */