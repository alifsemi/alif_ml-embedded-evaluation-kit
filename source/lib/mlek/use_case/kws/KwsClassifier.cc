/*
 * SPDX-FileCopyrightText: Copyright 2022-2023, 2025 Arm Limited and/or
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
#include "mlek/use_case/kws/KwsClassifier.hpp"

#include "mlek/math/PlatformMath.hpp"
#include "mlek/fwk/iface/Tensor.hpp"
#include "mlek/log/log_macros.h"

#include <algorithm>
#include <cinttypes>
#include <string>
#include <vector>

namespace arm {
namespace app {

    bool KwsClassifier::GetClassificationResults(
        const std::shared_ptr<fwk::iface::TensorIface> outputTensor,
        std::vector<ClassificationResult>& vecResults,
        const std::vector<std::string>& labels,
        uint32_t topNCount,
        bool useSoftmax,
        std::vector<std::vector<float>>& resultHistory)
    {
        if (outputTensor == nullptr) {
            printf_err("Output vector is null pointer.\n");
            return false;
        }

        const uint32_t totalOutputSize = outputTensor->GetNumElements();

        /* Health check */
        if (totalOutputSize < topNCount) {
            printf_err("Output vector is smaller than %" PRIu32 "\n", topNCount);
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

        /* De-Quantize Output Tensor */
        auto quantParams = outputTensor->GetQuantParams();

        /* Floating point tensor data to be populated
         * NOTE: The assumption here is that the output tensor size isn't too
         * big and therefore, there's neglibible impact on heap usage. */
        std::vector<float> resultData(totalOutputSize);
        resultData.resize(totalOutputSize);

        /* Populate the floating point buffer */
        switch (outputTensor->Type()) {
        case fwk::iface::TensorType::UINT8: {
            uint8_t* tensor_buffer = outputTensor->GetData<uint8_t>();
            for (size_t i = 0; i < totalOutputSize; ++i) {
                resultData[i] =
                    quantParams.scale * (static_cast<float>(tensor_buffer[i]) - quantParams.offset);
            }
            break;
        }
        case fwk::iface::TensorType::INT8: {
            int8_t* tensor_buffer = outputTensor->GetData<int8_t>();
            for (size_t i = 0; i < totalOutputSize; ++i) {
                resultData[i] =
                    quantParams.scale * (static_cast<float>(tensor_buffer[i]) - quantParams.offset);
            }
            break;
        }
        case fwk::iface::TensorType::FP32: {
            float* tensor_buffer = outputTensor->GetData<float>();
            for (size_t i = 0; i < totalOutputSize; ++i) {
                resultData[i] = tensor_buffer[i];
            }
            break;
        }
        default:
            printf_err("Tensor type %s not supported by classifier\n",
                       fwk::iface::GetTensorDataTypeName(outputTensor->Type()));
            return false;
        }

        if (useSoftmax) {
            math::MathUtils::SoftmaxF32(resultData);
        }

        /* If keeping track of recent results, update and take an average. */
        if (resultHistory.size() > 1) {
            std::rotate(resultHistory.begin(), resultHistory.begin() + 1, resultHistory.end());
            resultHistory.back() = resultData;
            AveragResults(resultHistory, resultData);
        }

        /* Get the top N results. */
        resultState = GetTopNResults(resultData, vecResults, topNCount, labels);

        if (!resultState) {
            printf_err("Failed to get top N results set\n");
            return false;
        }

        return true;
    }

    void app::KwsClassifier::AveragResults(const std::vector<std::vector<float>>& resultHistory,
            std::vector<float>& averageResult)
    {
        /* Compute averages of each class across the window length. */
        float sum;
        for (size_t j = 0; j < averageResult.size(); j++) {
            sum = 0;
            for (size_t i = 0; i < resultHistory.size(); i++) {
                sum += resultHistory[i][j];
            }
            averageResult[j] = (sum / resultHistory.size());
        }
    }

} /* namespace app */
} /* namespace arm */
