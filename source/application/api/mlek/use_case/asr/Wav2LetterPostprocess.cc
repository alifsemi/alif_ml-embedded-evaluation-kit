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
#include "mlek/use_case/asr/Wav2LetterPostprocess.hpp"
#include "mlek/log/log_macros.h"

#include <cmath>
#include <cstring>

namespace arm {
namespace app {

    AsrPostProcess::AsrPostProcess(const fwk::iface::Model& model,
                                   AsrClassifier& classifier,
                                   const std::vector<std::string>& labels,
                                   std::vector<ClassificationResult>& results,
                                   const uint32_t inputContextLen,
                                   const uint32_t blankTokenIdx,
                                   const uint32_t reductionAxisIdx) :
        m_classifier(classifier), m_model(model), m_labels{labels}, m_results(results),
        m_inputContextLen(inputContextLen), m_countIterations(0), m_blankTokenIdx(blankTokenIdx),
        m_reductionAxisIdx(reductionAxisIdx)
    {
        this->m_outputTensor     = model.GetOutputTensor(0);
        this->m_outputContextLen = this->GetOutputContextLen();
        this->m_outputInnerLen   = this->GetOutputInnerLen();
        this->m_totalLen = (2 * this->m_outputContextLen + this->m_outputInnerLen);
    }

    bool AsrPostProcess::DoPostProcess()
    {
        /* Basic checks. */
        if (!this->IsInputValid(this->m_outputTensor, this->m_reductionAxisIdx)) {
            return false;
        }

        /* Irrespective of tensor type, we use unsigned "byte" */
        const auto ptrData    = this->m_outputTensor->GetData<uint8_t>();
        const uint32_t elemSz = fwk::iface::GetTensorDataTypeSize(this->m_outputTensor->Type());

        /* Other health checks. */
        if (0 == elemSz) {
            printf_err("Tensor type not supported for post processing\n");
            return false;
        } else if (elemSz * this->m_totalLen > this->m_outputTensor->Bytes()) {
            printf_err("Insufficient number of tensor bytes\n");
            return false;
        }

        /* Which axis do we need to process? */
        if (this->m_reductionAxisIdx == this->m_classifier.m_outputTensorRowsIdx) {
            this->EraseSectionsRowWise(
                ptrData,
                elemSz * this->m_outputTensor->Shape()[this->m_classifier.m_outputTensorColsIdx],
                this->m_lastIteration);
        } else {
            printf_err("Unsupported axis index: %" PRIu32 "\n", this->m_reductionAxisIdx);
            return false;
        }
        this->m_classifier.GetClassificationResults(
            this->m_outputTensor, this->m_results, this->m_labels, 1);
        return true;
    }

    bool AsrPostProcess::IsInputValid(const std::shared_ptr<fwk::iface::TensorIface> tensor,
                                      const uint32_t axisIdx) const
    {
        if (nullptr == tensor) {
            return false;
        }

        const auto shape = tensor->Shape();

        if (axisIdx >= shape.size()) {
            printf_err("Invalid axis index: %" PRIu32 "; Max: %zu\n", axisIdx, shape.size());
            return false;
        }

        if (this->m_totalLen != shape[axisIdx]) {
            printf_err("Unexpected tensor dimension for axis %" PRIu32 ", got %zu.\n",
                       axisIdx,
                       shape[axisIdx]);
            return false;
        }

        return true;
    }

    bool AsrPostProcess::EraseSectionsRowWise(
            uint8_t*         ptrData,
            const uint32_t   strideSzBytes,
            const bool       lastIteration)
    {
        /* In this case, the "zero-ing" is quite simple as the region
         * to be zeroed sits in contiguous memory (row-major). */
        const uint32_t eraseLen = strideSzBytes * this->m_outputContextLen;

        /* Erase left context? */
        if (this->m_countIterations > 0) {
            /* Set output of each classification window to the blank token. */
            std::memset(ptrData, 0, eraseLen);
            for (size_t windowIdx = 0; windowIdx < this->m_outputContextLen; windowIdx++) {
                ptrData[windowIdx*strideSzBytes + this->m_blankTokenIdx] = 1;
            }
        }

        /* Erase right context? */
        if (false == lastIteration) {
            uint8_t* rightCtxPtr = ptrData + (strideSzBytes * (this->m_outputContextLen + this->m_outputInnerLen));
            /* Set output of each classification window to the blank token. */
            std::memset(rightCtxPtr, 0, eraseLen);
            for (size_t windowIdx = 0; windowIdx < this->m_outputContextLen; windowIdx++) {
                rightCtxPtr[windowIdx*strideSzBytes + this->m_blankTokenIdx] = 1;
            }
        }

        if (lastIteration) {
            this->m_countIterations = 0;
        } else {
            ++this->m_countIterations;
        }

        return true;
    }

    uint32_t AsrPostProcess::GetNumFeatureVectors() const
    {
        const auto inputTensor = this->m_model.GetInputTensor(0);
        const size_t inputRows = inputTensor->Shape()[this->m_classifier.m_inputTensorRowsIdx];
        if (inputRows == 0) {
            printf_err("Error getting number of input rows for axis: %" PRIu32 "\n",
                       this->m_classifier.m_inputTensorRowsIdx);
        }
        return inputRows;
    }

    uint32_t AsrPostProcess::GetOutputInnerLen() const
    {
        const uint32_t outputRows =
            this->m_outputTensor->Shape()[this->m_classifier.m_outputTensorRowsIdx];
        if (outputRows == 0) {
            printf_err("Error getting number of output rows for axis: %" PRIu32 "\n",
                       this->m_classifier.m_outputTensorRowsIdx);
        }

        /* Watching for underflow. */
        if (this->m_outputContextLen * 2 > outputRows) {
            printf_err("Invalid context length.\n");
            return 0;
        }

        return (outputRows - (2 * this->m_outputContextLen));
    }

    uint32_t AsrPostProcess::GetOutputContextLen() const
    {
        const uint32_t inputRows     = this->GetNumFeatureVectors();
        const uint32_t inputInnerLen = inputRows - (2 * this->m_inputContextLen);
        const uint32_t outputRowsIdx = this->m_classifier.m_outputTensorRowsIdx;

        /* Check to make sure that the input tensor supports the above
         * context and inner lengths. */
        if (inputRows <= 2 * this->m_inputContextLen || inputRows <= inputInnerLen) {
            printf_err("Input rows not compatible with ctx of %" PRIu32 "\n",
                       this->m_inputContextLen);
            return 0;
        }

        const auto outputTensor   = this->m_model.GetOutputTensor(0);
        const uint32_t outputRows = outputTensor->Shape()[outputRowsIdx];
        if (outputRows == 0) {
            printf_err("Error getting number of output rows for axis: %" PRIu32 "\n",
                       this->m_classifier.m_outputTensorRowsIdx);
            return 0;
        }

        const float inOutRowRatio = static_cast<float>(inputRows) /
                                     static_cast<float>(outputRows);

        return std::round(static_cast<float>(this->m_inputContextLen) / inOutRowRatio);
    }

} /* namespace app */
} /* namespace arm */
