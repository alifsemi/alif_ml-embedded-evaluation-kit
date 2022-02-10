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
#include "AdPostProcessing.hpp"
#include "log_macros.h"

#include <numeric>
#include <cmath>
#include <string>

namespace arm {
namespace app {

    template<typename T>
    std::vector<float> Dequantize(TfLiteTensor* tensor) {

        if (tensor == nullptr) {
            printf_err("Tensor is null pointer can not dequantize.\n");
            return std::vector<float>();
        }
        T* tensorData = tflite::GetTensorData<T>(tensor);

        uint32_t totalOutputSize = 1;
        for (int inputDim = 0; inputDim < tensor->dims->size; inputDim++){
            totalOutputSize *= tensor->dims->data[inputDim];
        }

        /* For getting the floating point values, we need quantization parameters */
        QuantParams quantParams = GetTensorQuantParams(tensor);

        std::vector<float> dequantizedOutput(totalOutputSize);

        for (size_t i = 0; i < totalOutputSize; ++i) {
            dequantizedOutput[i] = quantParams.scale * (tensorData[i] - quantParams.offset);
        }

        return dequantizedOutput;
    }

    void Softmax(std::vector<float>& inputVector) {
        auto start = inputVector.begin();
        auto end = inputVector.end();

        /* Fix for numerical stability and apply exp. */
        float maxValue = *std::max_element(start, end);
        for (auto it = start; it!=end; ++it) {
            *it = std::exp((*it) - maxValue);
        }

        float sumExp = std::accumulate(start, end, 0.0f);

        for (auto it = start; it!=end; ++it) {
            *it = (*it)/sumExp;
        }
    }

    int8_t OutputIndexFromFileName(std::string wavFileName) {
        /* Filename is assumed in the form machine_id_00.wav */
        std::string delimiter = "_";  /* First character used to split the file name up. */
        size_t delimiterStart;
        std::string subString;
        size_t machineIdxInString = 3;  /* Which part of the file name the machine id should be at. */

        for (size_t i = 0; i < machineIdxInString; ++i) {
            delimiterStart = wavFileName.find(delimiter);
            subString = wavFileName.substr(0, delimiterStart);
            wavFileName.erase(0, delimiterStart + delimiter.length());
        }

        /* At this point substring should be 00.wav */
        delimiter = ".";  /* Second character used to split the file name up. */
        delimiterStart = subString.find(delimiter);
        subString = (delimiterStart != std::string::npos) ? subString.substr(0, delimiterStart) : subString;

        auto is_number = [](const std::string& str) ->  bool
        {
            std::string::const_iterator it = str.begin();
            while (it != str.end() && std::isdigit(*it)) ++it;
            return !str.empty() && it == str.end();
        };

        const int8_t machineIdx = is_number(subString) ? std::stoi(subString) : -1;

        /* Return corresponding index in the output vector. */
        if (machineIdx == 0) {
            return 0;
        } else if (machineIdx == 2) {
            return 1;
        } else if (machineIdx == 4) {
            return 2;
        } else if (machineIdx == 6) {
            return 3;
        } else {
            printf_err("%d is an invalid machine index \n", machineIdx);
            return -1;
        }
    }

    template std::vector<float> Dequantize<uint8_t>(TfLiteTensor* tensor);
    template std::vector<float> Dequantize<int8_t>(TfLiteTensor* tensor);
} /* namespace app */
} /* namespace arm */