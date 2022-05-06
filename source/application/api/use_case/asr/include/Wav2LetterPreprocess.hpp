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
#ifndef ASR_WAV2LETTER_PREPROCESS_HPP
#define ASR_WAV2LETTER_PREPROCESS_HPP

#include "TensorFlowLiteMicro.hpp"
#include "Wav2LetterMfcc.hpp"
#include "AudioUtils.hpp"
#include "DataStructures.hpp"
#include "BaseProcessing.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    /* Class to facilitate pre-processing calculation for Wav2Letter model
     * for ASR. */
    using AudioWindow = audio::SlidingWindow<const int16_t>;

    class AsrPreProcess : public BasePreProcess {
    public:
        /**
         * @brief       Constructor.
         * @param[in]   inputTensor        Pointer to the TFLite Micro input Tensor.
         * @param[in]   numMfccFeatures    Number of MFCC features per window.
         * @param[in]   numFeatureFrames   Number of MFCC vectors that need to be calculated
         *                                 for an inference.
         * @param[in]   mfccWindowLen      Number of audio elements to calculate MFCC features per window.
         * @param[in]   mfccWindowStride   Stride (in number of elements) for moving the MFCC window.
         */
        AsrPreProcess(TfLiteTensor* inputTensor,
                      uint32_t  numMfccFeatures,
                      uint32_t  numFeatureFrames,
                      uint32_t  mfccWindowLen,
                      uint32_t  mfccWindowStride);

        /**
         * @brief       Calculates the features required from audio data. This
         *              includes MFCC, first and second order deltas,
         *              normalisation and finally, quantisation. The tensor is
         *              populated with features from a given window placed along
         *              in a single row.
         * @param[in]   audioData      Pointer to the first element of audio data.
         * @param[in]   audioDataLen   Number of elements in the audio data.
         * @return      true if successful, false in case of error.
         */
        bool DoPreProcess(const void* audioData, size_t audioDataLen) override;

    protected:
         /**
          * @brief Computes the first and second order deltas for the
          *        MFCC buffers - they are assumed to be populated.
          *
          * @param[in]  mfcc     MFCC buffers.
          * @param[out] delta1   Result of the first diff computation.
          * @param[out] delta2   Result of the second diff computation.
          * @return     true if successful, false otherwise.
          */
         static bool ComputeDeltas(Array2d<float>& mfcc,
                                   Array2d<float>& delta1,
                                   Array2d<float>& delta2);

        /**
         * @brief           Given a 2D vector of floats, rescale it to have mean of 0 and
        *                   standard deviation of 1.
         * @param[in,out]   vec   Vector of vector of floats.
         */
        static void StandardizeVecF32(Array2d<float>& vec);

        /**
         * @brief   Standardizes all the MFCC and delta buffers to have mean 0 and std. dev 1.
         */
        void Standarize();

        /**
         * @brief       Given the quantisation and data type limits, computes
         *              the quantised values of a floating point input data.
         * @param[in]   elem          Element to be quantised.
         * @param[in]   quantScale    Scale.
         * @param[in]   quantOffset   Offset.
         * @param[in]   minVal        Numerical limit - minimum.
         * @param[in]   maxVal        Numerical limit - maximum.
         * @return      Floating point quantised value.
         */
        static float GetQuantElem(
                float     elem,
                float     quantScale,
                int       quantOffset,
                float     minVal,
                float     maxVal);

        /**
         * @brief       Quantises the MFCC and delta buffers, and places them
         *              in the output buffer. While doing so, it transposes
         *              the data. Reason: Buffers in this class are arranged
         *              for "time" axis to be row major. Primary reason for
         *              this being the convolution speed up (as we can use
         *              contiguous memory). The output, however, requires the
         *              time axis to be in column major arrangement.
         * @param[in]   outputBuf     Pointer to the output buffer.
         * @param[in]   outputBufSz   Output buffer's size.
         * @param[in]   quantScale    Quantisation scale.
         * @param[in]   quantOffset   Quantisation offset.
         */
        template <typename T>
        bool Quantise(
                T*              outputBuf,
                const uint32_t  outputBufSz,
                const float     quantScale,
                const int       quantOffset)
        {
            /* Check the output size will fit everything. */
            if (outputBufSz < (this->m_mfccBuf.size(0) * 3 * sizeof(T))) {
                printf_err("Tensor size too small for features\n");
                return false;
            }

            /* Populate. */
            T* outputBufMfcc = outputBuf;
            T* outputBufD1 = outputBuf + this->m_numMfccFeats;
            T* outputBufD2 = outputBufD1 + this->m_numMfccFeats;
            const uint32_t ptrIncr = this->m_numMfccFeats * 2;  /* (3 vectors - 1 vector) */

            const float minVal = std::numeric_limits<T>::min();
            const float maxVal = std::numeric_limits<T>::max();

            /* Need to transpose while copying and concatenating the tensor. */
            for (uint32_t j = 0; j < this->m_numFeatureFrames; ++j) {
                for (uint32_t i = 0; i < this->m_numMfccFeats; ++i) {
                    *outputBufMfcc++ = static_cast<T>(AsrPreProcess::GetQuantElem(
                            this->m_mfccBuf(i, j), quantScale,
                            quantOffset, minVal, maxVal));
                    *outputBufD1++ = static_cast<T>(AsrPreProcess::GetQuantElem(
                            this->m_delta1Buf(i, j), quantScale,
                            quantOffset, minVal, maxVal));
                    *outputBufD2++ = static_cast<T>(AsrPreProcess::GetQuantElem(
                            this->m_delta2Buf(i, j), quantScale,
                            quantOffset, minVal, maxVal));
                }
                outputBufMfcc += ptrIncr;
                outputBufD1 += ptrIncr;
                outputBufD2 += ptrIncr;
            }

            return true;
        }

    private:
        audio::Wav2LetterMFCC   m_mfcc;          /* MFCC instance. */
        TfLiteTensor*           m_inputTensor;   /* Model input tensor. */

        /* Actual buffers to be populated. */
        Array2d<float>   m_mfccBuf;              /* Contiguous buffer 1D: MFCC */
        Array2d<float>   m_delta1Buf;            /* Contiguous buffer 1D: Delta 1 */
        Array2d<float>   m_delta2Buf;            /* Contiguous buffer 1D: Delta 2 */

        uint32_t         m_mfccWindowLen;        /* Window length for MFCC. */
        uint32_t         m_mfccWindowStride;     /* Window stride len for MFCC. */
        uint32_t         m_numMfccFeats;         /* Number of MFCC features per window. */
        uint32_t         m_numFeatureFrames;     /* How many sets of m_numMfccFeats. */
        AudioWindow      m_mfccSlidingWindow;    /* Sliding window to calculate MFCCs. */

    };

} /* namespace app */
} /* namespace arm */

#endif /* ASR_WAV2LETTER_PREPROCESS_HPP */