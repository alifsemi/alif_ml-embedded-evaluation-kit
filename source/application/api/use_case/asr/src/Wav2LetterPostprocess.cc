/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
#include "Wav2LetterPostprocess.hpp"

#include "Wav2LetterModel.hpp"
#include "log_macros.h"

#include <cmath>

namespace arm {
namespace app {

    AsrPostProcess::AsrPostProcess(TfLiteTensor* outputTensor, AsrClassifier& classifier,
            const std::vector<std::string>& labels, std::vector<ClassificationResult>& results,
            const uint32_t outputContextLen,
            const uint32_t blankTokenIdx, const uint32_t reductionAxisIdx
            ):
            m_classifier(classifier),
            m_outputTensor(outputTensor),
            m_labels{labels},
            m_results(results),
            m_outputContextLen(outputContextLen),
            m_countIterations(0),
            m_blankTokenIdx(blankTokenIdx),
            m_reductionAxisIdx(reductionAxisIdx)
    {
        this->m_outputInnerLen = AsrPostProcess::GetOutputInnerLen(this->m_outputTensor, this->m_outputContextLen);
        this->m_totalLen = (2 * this->m_outputContextLen + this->m_outputInnerLen);
    }

    bool AsrPostProcess::DoPostProcess()
    {
        /* Basic checks. */
        if (!this->IsInputValid(this->m_outputTensor, this->m_reductionAxisIdx)) {
            return false;
        }

        /* Irrespective of tensor type, we use unsigned "byte" */
        auto* ptrData = tflite::GetTensorData<uint8_t>(this->m_outputTensor);
        const uint32_t elemSz = AsrPostProcess::GetTensorElementSize(this->m_outputTensor);

        /* Other sanity checks. */
        if (0 == elemSz) {
            printf_err("Tensor type not supported for post processing\n");
            return false;
        } else if (elemSz * this->m_totalLen > this->m_outputTensor->bytes) {
            printf_err("Insufficient number of tensor bytes\n");
            return false;
        }

        /* Which axis do we need to process? */
        switch (this->m_reductionAxisIdx) {
            case Wav2LetterModel::ms_outputRowsIdx:
                this->EraseSectionsRowWise(
                        ptrData, elemSz * this->m_outputTensor->dims->data[Wav2LetterModel::ms_outputColsIdx],
                        this->m_lastIteration);
                break;
            default:
                printf_err("Unsupported axis index: %" PRIu32 "\n", this->m_reductionAxisIdx);
                return false;
        }
        this->m_classifier.GetClassificationResults(this->m_outputTensor,
                this->m_results, this->m_labels, 1);

        return true;
    }

    bool AsrPostProcess::IsInputValid(TfLiteTensor* tensor, const uint32_t axisIdx) const
    {
        if (nullptr == tensor) {
            return false;
        }

        if (static_cast<int>(axisIdx) >= tensor->dims->size) {
            printf_err("Invalid axis index: %" PRIu32 "; Max: %d\n",
                axisIdx, tensor->dims->size);
            return false;
        }

        if (static_cast<int>(this->m_totalLen) !=
                             tensor->dims->data[axisIdx]) {
            printf_err("Unexpected tensor dimension for axis %" PRIu32", got %d.\n",
                axisIdx, tensor->dims->data[axisIdx]);
            return false;
        }

        return true;
    }

    uint32_t AsrPostProcess::GetTensorElementSize(TfLiteTensor* tensor)
    {
        switch(tensor->type) {
            case kTfLiteUInt8:
            case kTfLiteInt8:
                return 1;
            case kTfLiteInt16:
                return 2;
            case kTfLiteInt32:
            case kTfLiteFloat32:
                return 4;
            default:
                printf_err("Unsupported tensor type %s\n",
                    TfLiteTypeGetName(tensor->type));
        }

        return 0;
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

    uint32_t AsrPostProcess::GetNumFeatureVectors(const Model& model)
    {
        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        const int inputRows = std::max(inputTensor->dims->data[Wav2LetterModel::ms_inputRowsIdx], 0);
        if (inputRows == 0) {
            printf_err("Error getting number of input rows for axis: %" PRIu32 "\n",
                    Wav2LetterModel::ms_inputRowsIdx);
        }
        return inputRows;
    }

    uint32_t AsrPostProcess::GetOutputInnerLen(const TfLiteTensor* outputTensor, const uint32_t outputCtxLen)
    {
        const uint32_t outputRows = std::max(outputTensor->dims->data[Wav2LetterModel::ms_outputRowsIdx], 0);
        if (outputRows == 0) {
            printf_err("Error getting number of output rows for axis: %" PRIu32 "\n",
                    Wav2LetterModel::ms_outputRowsIdx);
        }

        /* Watching for underflow. */
        int innerLen = (outputRows - (2 * outputCtxLen));

        return std::max(innerLen, 0);
    }

    uint32_t AsrPostProcess::GetOutputContextLen(const Model& model, const uint32_t inputCtxLen)
    {
        const uint32_t inputRows = AsrPostProcess::GetNumFeatureVectors(model);
        const uint32_t inputInnerLen = inputRows - (2 * inputCtxLen);
        constexpr uint32_t ms_outputRowsIdx = Wav2LetterModel::ms_outputRowsIdx;

        /* Check to make sure that the input tensor supports the above
         * context and inner lengths. */
        if (inputRows <= 2 * inputCtxLen || inputRows <= inputInnerLen) {
            printf_err("Input rows not compatible with ctx of %" PRIu32 "\n",
                       inputCtxLen);
            return 0;
        }

        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        const uint32_t outputRows = std::max(outputTensor->dims->data[ms_outputRowsIdx], 0);
        if (outputRows == 0) {
            printf_err("Error getting number of output rows for axis: %" PRIu32 "\n",
                       Wav2LetterModel::ms_outputRowsIdx);
            return 0;
        }

        const float inOutRowRatio = static_cast<float>(inputRows) /
                                     static_cast<float>(outputRows);

        return std::round(static_cast<float>(inputCtxLen) / inOutRowRatio);
    }

} /* namespace app */
} /* namespace arm */
