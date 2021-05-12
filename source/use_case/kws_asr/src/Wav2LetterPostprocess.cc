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
#include "Wav2LetterPostprocess.hpp"

#include "Wav2LetterModel.hpp"

namespace arm {
namespace app {
namespace audio {
namespace asr {

    Postprocess::Postprocess(const uint32_t contextLen,
                             const uint32_t innerLen,
                             const uint32_t blankTokenIdx)
        :   m_contextLen(contextLen),
            m_innerLen(innerLen),
            m_totalLen(2 * this->m_contextLen + this->m_innerLen),
            m_countIterations(0),
            m_blankTokenIdx(blankTokenIdx)
    {}

    bool Postprocess::Invoke(TfLiteTensor*  tensor,
                            const uint32_t  axisIdx,
                            const bool      lastIteration)
    {
        /* Basic checks. */
        if (!this->IsInputValid(tensor, axisIdx)) {
            return false;
        }

        /* Irrespective of tensor type, we use unsigned "byte" */
        uint8_t* ptrData = tflite::GetTensorData<uint8_t>(tensor);
        const uint32_t elemSz = this->GetTensorElementSize(tensor);

        /* Other sanity checks. */
        if (0 == elemSz) {
            printf_err("Tensor type not supported for post processing\n");
            return false;
        } else if (elemSz * this->m_totalLen > tensor->bytes) {
            printf_err("Insufficient number of tensor bytes\n");
            return false;
        }

        /* Which axis do we need to process? */
        switch (axisIdx) {
            case arm::app::Wav2LetterModel::ms_outputRowsIdx:
                return this->EraseSectionsRowWise(ptrData,
                                                  elemSz *
                                                  tensor->dims->data[arm::app::Wav2LetterModel::ms_outputColsIdx],
                                                  lastIteration);
            default:
                printf_err("Unsupported axis index: %" PRIu32 "\n", axisIdx);
        }

        return false;
    }

    bool Postprocess::IsInputValid(TfLiteTensor*  tensor,
                                   const uint32_t axisIdx) const
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
            printf_err("Unexpected tensor dimension for axis %d, \n",
                tensor->dims->data[axisIdx]);
            return false;
        }

        return true;
    }

    uint32_t Postprocess::GetTensorElementSize(TfLiteTensor*  tensor)
    {
        switch(tensor->type) {
            case kTfLiteUInt8:
                return 1;
            case kTfLiteInt8:
                return 1;
            case kTfLiteInt16:
                return 2;
            case kTfLiteInt32:
                return 4;
            case kTfLiteFloat32:
                return 4;
            default:
                printf_err("Unsupported tensor type %s\n",
                    TfLiteTypeGetName(tensor->type));
        }

        return 0;
    }

    bool Postprocess::EraseSectionsRowWise(
                        uint8_t*         ptrData,
                        const uint32_t   strideSzBytes,
                        const bool       lastIteration)
    {
        /* In this case, the "zero-ing" is quite simple as the region
         * to be zeroed sits in contiguous memory (row-major). */
        const uint32_t eraseLen = strideSzBytes * this->m_contextLen;

        /* Erase left context? */
        if (this->m_countIterations > 0) {
            /* Set output of each classification window to the blank token. */
            std::memset(ptrData, 0, eraseLen);
            for (size_t windowIdx = 0; windowIdx < this->m_contextLen; windowIdx++) {
                ptrData[windowIdx*strideSzBytes + this->m_blankTokenIdx] = 1;
            }
        }

        /* Erase right context? */
        if (false == lastIteration) {
            uint8_t * rightCtxPtr = ptrData + (strideSzBytes * (this->m_contextLen + this->m_innerLen));
            /* Set output of each classification window to the blank token. */
            std::memset(rightCtxPtr, 0, eraseLen);
            for (size_t windowIdx = 0; windowIdx < this->m_contextLen; windowIdx++) {
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

} /* namespace asr */
} /* namespace audio */
} /* namespace app */
} /* namespace arm */