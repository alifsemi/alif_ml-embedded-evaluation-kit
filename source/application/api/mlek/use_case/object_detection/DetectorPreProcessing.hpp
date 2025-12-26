/*
 * SPDX-FileCopyrightText: Copyright 2022 2025 Arm Limited and/or
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
#ifndef DETECTOR_PRE_PROCESSING_HPP
#define DETECTOR_PRE_PROCESSING_HPP

#include "mlek/common/BaseProcessing.hpp"
#include "mlek/common/Classifier.hpp"

namespace arm {
namespace app {

    /**
     * @brief   Pre-processing class for Object detection use case.
     *          Implements methods declared by BasePreProcess and anything else needed
     *          to populate input tensors ready for inference.
     */
    class DetectorPreProcess : public BasePreProcess {

    public:
        /**
         * @brief       Constructor
         * @param[in]   inputTensor     Shared pointer representing a tensor interface object.
         * @param[in]   rgb2Gray        Convert image from 3 channel RGB to 1 channel grayscale.
         * @param[in]   convertToInt8   Convert the image from uint8 to int8 range.
         **/
        explicit DetectorPreProcess(const std::shared_ptr<fwk::iface::TensorIface> inputTensor,
                                    bool rgb2Gray,
                                    bool convertToInt8);

        /**
         * @brief       Should perform pre-processing of 'raw' input image data and load it into
         *              input tensor ready for inference.
         * @param[in]   input      Pointer to the data that pre-processing will work on.
         * @param[in]   inputSize  Size of the input data.
         * @return      true if successful, false otherwise.
         **/
        bool DoPreProcess(const void* input, size_t inputSize) override;

    private:
        std::shared_ptr<fwk::iface::TensorIface> m_inputTensor;
        bool m_rgb2Gray;
        bool m_convertToInt8;
    };

} /* namespace app */
} /* namespace arm */

#endif /* DETECTOR_PRE_PROCESSING_HPP */
