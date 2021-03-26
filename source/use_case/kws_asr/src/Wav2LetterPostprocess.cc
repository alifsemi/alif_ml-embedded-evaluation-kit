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
        :   _m_contextLen(contextLen),
            _m_innerLen(innerLen),
            _m_totalLen(2 * this->_m_contextLen + this->_m_innerLen),
            _m_countIterations(0),
            _m_blankTokenIdx(blankTokenIdx)
    {}

    bool Postprocess::Invoke(TfLiteTensor*  tensor,
                            const uint32_t  axisIdx,
                            const bool      lastIteration)
    {
        /* Basic checks. */
        if (!this->_IsInputValid(tensor, axisIdx)) {
            return false;
        }

        /* Irrespective of tensor type, we use unsigned "byte" */
        uint8_t* ptrData = tflite::GetTensorData<uint8_t>(tensor);
        const uint32_t elemSz = this->_GetTensorElementSize(tensor);

        /* Other sanity checks. */
        if (0 == elemSz) {
            printf_err("Tensor type not supported for post processing\n");
            return false;
        } else if (elemSz * this->_m_totalLen > tensor->bytes) {
            printf_err("Insufficient number of tensor bytes\n");
            return false;
        }

        /* Which axis do we need to process? */
        switch (axisIdx) {
            case arm::app::Wav2LetterModel::ms_outputRowsIdx:
                return this->_EraseSectionsRowWise(ptrData,
                        elemSz * tensor->dims->data[arm::app::Wav2LetterModel::ms_outputColsIdx],
                        lastIteration);
            default:
                printf_err("Unsupported axis index: %u\n", axisIdx);
        }

        return false;
    }

    bool Postprocess::_IsInputValid(TfLiteTensor*  tensor,
                                    const uint32_t axisIdx) const
    {
        if (nullptr == tensor) {
            return false;
        }

        if (static_cast<int>(axisIdx) >= tensor->dims->size) {
            printf_err("Invalid axis index: %u; Max: %d\n",
                axisIdx, tensor->dims->size);
            return false;
        }

        if (static_cast<int>(this->_m_totalLen) !=
                             tensor->dims->data[axisIdx]) {
            printf_err("Unexpected tensor dimension for axis %d, \n",
                tensor->dims->data[axisIdx]);
            return false;
        }

        return true;
    }

    uint32_t Postprocess::_GetTensorElementSize(TfLiteTensor*  tensor)
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

    bool Postprocess::_EraseSectionsRowWise(
                        uint8_t*         ptrData,
                        const uint32_t   strideSzBytes,
                        const bool       lastIteration)
    {
        /* In this case, the "zero-ing" is quite simple as the region
         * to be zeroed sits in contiguous memory (row-major). */
        const uint32_t eraseLen = strideSzBytes * this->_m_contextLen;

        /* Erase left context? */
        if (this->_m_countIterations > 0) {
            /* Set output of each classification window to the blank token. */
            std::memset(ptrData, 0, eraseLen);
            for (size_t windowIdx = 0; windowIdx < this->_m_contextLen; windowIdx++) {
                ptrData[windowIdx*strideSzBytes + this->_m_blankTokenIdx] = 1;
            }
        }

        /* Erase right context? */
        if (false == lastIteration) {
            uint8_t * rightCtxPtr = ptrData + (strideSzBytes * (this->_m_contextLen + this->_m_innerLen));
            /* Set output of each classification window to the blank token. */
            std::memset(rightCtxPtr, 0, eraseLen);
            for (size_t windowIdx = 0; windowIdx < this->_m_contextLen; windowIdx++) {
                rightCtxPtr[windowIdx*strideSzBytes + this->_m_blankTokenIdx] = 1;
            }
        }

        if (lastIteration) {
            this->_m_countIterations = 0;
        } else {
            ++this->_m_countIterations;
        }

        return true;
    }

} /* namespace asr */
} /* namespace audio */
} /* namespace app */
} /* namespace arm */