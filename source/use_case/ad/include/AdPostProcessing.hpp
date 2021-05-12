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
#ifndef ADPOSTPROCESSING_HPP
#define ADPOSTPROCESSING_HPP

#include "TensorFlowLiteMicro.hpp"

#include <vector>

namespace arm {
namespace app {

    /** @brief      Dequantize TensorFlow Lite Micro tensor.
     *  @param[in]  tensor Pointer to the TensorFlow Lite Micro tensor to be dequantized.
     *  @return     Vector with the dequantized tensor values.
    **/
    template<typename T>
    std::vector<float> Dequantize(TfLiteTensor* tensor);

    /**
     * @brief   Calculates the softmax of vector in place. **/
    void Softmax(std::vector<float>& inputVector);


    /** @brief      Given a wav file name return AD model output index.
     *  @param[in]  wavFileName Audio WAV filename.
     *                          File name should be in format anything_goes_XX_here.wav
     *                          where XX is the machine ID e.g. 00, 02, 04 or 06
     *  @return     AD model output index as 8 bit integer.
    **/
    int8_t OutputIndexFromFileName(std::string wavFileName);

} /* namespace app */
} /* namespace arm */

#endif /* ADPOSTPROCESSING_HPP */
