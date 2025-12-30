/*
 * SPDX-FileCopyrightText: Copyright 2022, 2025 Arm Limited and/or its affiliates
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
#ifndef IMG_CLASS_PROCESSING_HPP
#define IMG_CLASS_PROCESSING_HPP

#include "mlek/common/BaseProcessing.hpp"
#include "mlek/common/Classifier.hpp"
#include <memory>
#include <array>

namespace arm {
namespace app {

    /**
     * @brief   Pre-processing class for Image Classification use case.
     *          Implements methods declared by BasePreProcess and anything else needed
     *          to populate input tensors ready for inference.
     */
    class ImgClassPreProcess : public BasePreProcess {

    public:
        static constexpr size_t kNumChannels = 3;

        /**
         * @brief       Constructor
         * @param[in]   inputTensor     Shared pointer representing a tensor interface object.
         * @param[in]   mean            Optional array representing per-channel mean value to be
         *                              used for normalisation. Default is set to values expected
         *                              by ImageNet dataset.
         * @param[in]   stddev          Optional array representing per-channel standard deviation
         *                              value to be used for normalisation. Default is set to values
         *                              expected by ImageNet dataset.
         **/
        explicit ImgClassPreProcess(const std::shared_ptr<fwk::iface::TensorIface> inputTensor,
                                    const std::array<float, kNumChannels> mean = {0.485, 0.456, 0.406},
                                    const std::array<float, kNumChannels> stddev = {0.229, 0.224, 0.225});

        /**
         * @brief       Should perform pre-processing of 'raw' input image data and load it into
         *              input tensors ready for inference
         * @param[in]   input      Pointer to the data that pre-processing will work on.
         * @param[in]   inputSize  Size of the input data.
         * @return      true if successful, false otherwise.
         **/
        bool DoPreProcess(const void* input, size_t inputSize) override;

    private:
        std::shared_ptr<fwk::iface::TensorIface> m_inputTensor;
        std::array<float, kNumChannels> m_mean{0.f, 0.f, 0.f};
        std::array<float, kNumChannels> m_stddev{1.f, 1.f, 1.f};
    };

    /**
     * @brief   Post-processing class for Image Classification use case.
     *          Implements methods declared by BasePostProcess and anything else needed
     *          to populate result vector.
     */
    class ImgClassPostProcess : public BasePostProcess {

    public:
        /**
         * @brief       Constructor
         * @param[in]   outputTensor  Shared pointer representing a tensor interface object
         * @param[in]   classifier    Classifier object used to get top N results from classification.
         * @param[in]   labels        Vector of string labels to identify each output of the model.
         * @param[in]   results       Vector of classification results to store decoded outputs.
         **/
        ImgClassPostProcess(const std::shared_ptr<fwk::iface::TensorIface> outputTensor,
                            Classifier& classifier,
                            const std::vector<std::string>& labels,
                            std::vector<ClassificationResult>& results);

        /**
         * @brief       Should perform post-processing of the result of inference then
         *              populate classification result data for any later use.
         * @return      true if successful, false otherwise.
         **/
        bool DoPostProcess() override;

    private:
        std::shared_ptr<fwk::iface::TensorIface> m_outputTensor{nullptr};
        Classifier& m_imgClassifier;
        const std::vector<std::string>& m_labels;
        std::vector<ClassificationResult>& m_results;
    };

} /* namespace app */
} /* namespace arm */

#endif /* IMG_CLASS_PROCESSING_HPP */
