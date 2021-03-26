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
#ifndef KWS_ASR_WAV2LET_PREPROC_HPP
#define KWS_ASR_WAV2LET_PREPROC_HPP

#include "Wav2LetterModel.hpp"
#include "Wav2LetterMfcc.hpp"
#include "AudioUtils.hpp"
#include "DataStructures.hpp"

namespace arm {
namespace app {
namespace audio {
namespace asr {

    /* Class to facilitate pre-processing calculation for Wav2Letter model
     * for ASR. */
    using AudioWindow = SlidingWindow <const int16_t>;

    class Preprocess {
    public:
        /**
         * @brief       Constructor
         * @param[in]   numMfccFeatures   Number of MFCC features per window.
         * @param[in]   windowLen         Number of elements in a window.
         * @param[in]   windowStride      Stride (in number of elements) for
         *                                moving the window.
         * @param[in]   numMfccVectors    Number of MFCC vectors per window.
        */
        Preprocess(
            uint32_t  numMfccFeatures,
            uint32_t  windowLen,
            uint32_t  windowStride,
            uint32_t  numMfccVectors);
        Preprocess() = delete;
        ~Preprocess() = default;

        /**
         * @brief       Calculates the features required from audio data. This
         *              includes MFCC, first and second order deltas,
         *              normalisation and finally, quantisation. The tensor is
         *              populated with feature from a given window placed along
         *              in a single row.
         * @param[in]   audioData      Pointer to the first element of audio data.
         * @param[in]   audioDataLen   Number of elements in the audio data.
         * @param[in]   tensor         Tensor to be populated.
         * @return      true if successful, false in case of error.
         */
        bool Invoke(const int16_t * audioData,
                    uint32_t  audioDataLen,
                    TfLiteTensor *  tensor);

    protected:
         /**
          * @brief Computes the first and second order deltas for the
          *        MFCC buffers - they are assumed to be populated.
          *
          * @param[in]  mfcc     MFCC buffers.
          * @param[out] delta1   Result of the first diff computation.
          * @param[out] delta2   Result of the second diff computation.
          *
          * @return true if successful, false otherwise.
          */
         static bool _ComputeDeltas(Array2d<float>& mfcc,
                                    Array2d<float>& delta1,
                                    Array2d<float>& delta2);

        /**
         * @brief       Given a 2D vector of floats, computes the mean.
         * @param[in]   vec   Vector of vector of floats.
         * @return      Mean value.
         */
        static float _GetMean(Array2d<float>& vec);

        /**
         * @brief       Given a 2D vector of floats, computes the stddev.
         * @param[in]   vec    Vector of vector of floats.
         * @param[in]   mean   Mean value of the vector passed in.
         * @return      stddev value.
         */
        static float _GetStdDev(Array2d<float>& vec,
                                float mean);

        /**
         * @brief           Given a 2D vector of floats, normalises it using
         *                  the mean and the stddev
         * @param[in,out]   vec   Vector of vector of floats.
         */
        static void _NormaliseVec(Array2d<float>& vec);

        /**
         * @brief       Normalises the MFCC and delta buffers.
         */
        void _Normalise();

        /**
         * @brief       Given the quantisation and data type limits, computes
         *              the quantised values of a floating point input data.
         * @param[in]   elem            Element to be quantised.
         * @param[in]   quantScale      Scale.
         * @param[in]   quantOffset     Offset.
         * @param[in]   minVal          Numerical limit - minimum.
         * @param[in]   maxVal          Numerical limit - maximum.
         * @return      Floating point quantised value.
         */
        static float _GetQuantElem(
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
         * @param[in]   outputBuf       Pointer to the output buffer.
         * @param[in]   outputBufSz     Output buffer's size.
         * @param[in]   quantScale      Quantisation scale.
         * @param[in]   quantOffset     Quantisation offset.
         */
        template <typename T>
        bool _Quantise(
                T *             outputBuf,
                const uint32_t  outputBufSz,
                const float     quantScale,
                const int       quantOffset)
        {
            /* Check the output size will for everything. */
            if (outputBufSz < (this->_m_mfccBuf.size(0) * 3 * sizeof(T))) {
                printf_err("Tensor size too small for features\n");
                return false;
            }

            /* Populate. */
            T * outputBufMfcc = outputBuf;
            T * outputBufD1 = outputBuf + this->_m_numMfccFeats;
            T * outputBufD2 = outputBufD1 + this->_m_numMfccFeats;
            const uint32_t ptrIncr = this->_m_numMfccFeats * 2;  /* (3 vectors - 1 vector) */

            const float minVal = std::numeric_limits<T>::min();
            const float maxVal = std::numeric_limits<T>::max();

            /* We need to do a transpose while copying and concatenating
             * the tensor. */
            for (uint32_t j = 0; j < this->_m_numFeatVectors; ++j) {
                for (uint32_t i = 0; i < this->_m_numMfccFeats; ++i) {
                    *outputBufMfcc++ = static_cast<T>(this->_GetQuantElem(
                                        this->_m_mfccBuf(i, j), quantScale,
                                        quantOffset, minVal, maxVal));
                    *outputBufD1++ = static_cast<T>(this->_GetQuantElem(
                                        this->_m_delta1Buf(i, j), quantScale,
                                        quantOffset, minVal, maxVal));
                    *outputBufD2++ = static_cast<T>(this->_GetQuantElem(
                                        this->_m_delta2Buf(i, j), quantScale,
                                        quantOffset, minVal, maxVal));
                }
                outputBufMfcc += ptrIncr;
                outputBufD1 += ptrIncr;
                outputBufD2 += ptrIncr;
            }

            return true;
        }

    private:
        Wav2LetterMFCC      _m_mfcc;            /* MFCC instance. */

        /* Actual buffers to be populated. */
        Array2d<float>      _m_mfccBuf;         /* Contiguous buffer 1D: MFCC */
        Array2d<float>      _m_delta1Buf;       /* Contiguous buffer 1D: Delta 1 */
        Array2d<float>      _m_delta2Buf;       /* Contiguous buffer 1D: Delta 2 */

        uint32_t            _m_windowLen;       /* Window length for MFCC. */
        uint32_t            _m_windowStride;    /* Window stride len for MFCC. */
        uint32_t            _m_numMfccFeats;    /* Number of MFCC features per window. */
        uint32_t            _m_numFeatVectors;  /* Number of _m_numMfccFeats. */
        AudioWindow         _m_window;          /* Sliding window. */

    };

} /* namespace asr */
} /* namespace audio */
} /* namespace app */
} /* namespace arm */

#endif /* KWS_ASR_WAV2LET_PREPROC_HPP */