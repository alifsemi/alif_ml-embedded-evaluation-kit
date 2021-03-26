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
#include "Wav2LetterPreprocess.hpp"

#include "PlatformMath.hpp"
#include "TensorFlowLiteMicro.hpp"

#include <algorithm>
#include <cmath>

namespace arm {
namespace app {
namespace audio {
namespace asr {

    Preprocess::Preprocess(
        const uint32_t  numMfccFeatures,
        const uint32_t  windowLen,
        const uint32_t  windowStride,
        const uint32_t  numMfccVectors):
            _m_mfcc(numMfccFeatures, windowLen),
            _m_mfccBuf(numMfccFeatures, numMfccVectors),
            _m_delta1Buf(numMfccFeatures, numMfccVectors),
            _m_delta2Buf(numMfccFeatures, numMfccVectors),
            _m_windowLen(windowLen),
            _m_windowStride(windowStride),
            _m_numMfccFeats(numMfccFeatures),
            _m_numFeatVectors(numMfccVectors),
            _m_window()
    {
        if (numMfccFeatures > 0 && windowLen > 0) {
            this->_m_mfcc.Init();
        }
    }

    bool Preprocess::Invoke(
                const int16_t*  audioData,
                const uint32_t  audioDataLen,
                TfLiteTensor*   tensor)
    {
        this->_m_window = SlidingWindow<const int16_t>(
                            audioData, audioDataLen,
                            this->_m_windowLen, this->_m_windowStride);

        uint32_t mfccBufIdx = 0;

        std::fill(_m_mfccBuf.begin(), _m_mfccBuf.end(), 0.f);
        std::fill(_m_delta1Buf.begin(), _m_delta1Buf.end(), 0.f);
        std::fill(_m_delta2Buf.begin(), _m_delta2Buf.end(), 0.f);

        /* While we can slide over the window. */
        while (this->_m_window.HasNext()) {
            const int16_t*  mfccWindow = this->_m_window.Next();
            auto mfccAudioData = std::vector<int16_t>(
                                        mfccWindow,
                                        mfccWindow + this->_m_windowLen);
            auto mfcc = this->_m_mfcc.MfccCompute(mfccAudioData);
            for (size_t i = 0; i < this->_m_mfccBuf.size(0); ++i) {
                this->_m_mfccBuf(i, mfccBufIdx) = mfcc[i];
            }
            ++mfccBufIdx;
        }

        /* Pad MFCC if needed by adding MFCC for zeros. */
        if (mfccBufIdx != this->_m_numFeatVectors) {
            std::vector<int16_t> zerosWindow = std::vector<int16_t>(this->_m_windowLen, 0);
            std::vector<float> mfccZeros = this->_m_mfcc.MfccCompute(zerosWindow);

            while (mfccBufIdx != this->_m_numFeatVectors) {
                memcpy(&this->_m_mfccBuf(0, mfccBufIdx),
                       mfccZeros.data(), sizeof(float) * _m_numMfccFeats);
                ++mfccBufIdx;
            }
        }

        /* Compute first and second order deltas from MFCCs. */
        this->_ComputeDeltas(this->_m_mfccBuf,
                             this->_m_delta1Buf,
                             this->_m_delta2Buf);

        /* Normalise. */
        this->_Normalise();

        /* Quantise. */
        QuantParams quantParams = GetTensorQuantParams(tensor);

        if (0 == quantParams.scale) {
            printf_err("Quantisation scale can't be 0\n");
            return false;
        }

        switch(tensor->type) {
            case kTfLiteUInt8:
                return this->_Quantise<uint8_t>(
                        tflite::GetTensorData<uint8_t>(tensor), tensor->bytes,
                        quantParams.scale, quantParams.offset);
            case kTfLiteInt8:
                return this->_Quantise<int8_t>(
                        tflite::GetTensorData<int8_t>(tensor), tensor->bytes,
                        quantParams.scale, quantParams.offset);
            default:
                printf_err("Unsupported tensor type %s\n",
                    TfLiteTypeGetName(tensor->type));
        }

        return false;
    }

    bool Preprocess::_ComputeDeltas(Array2d<float>& mfcc,
                                    Array2d<float>& delta1,
                                    Array2d<float>& delta2)
    {
        const std::vector <float> delta1Coeffs =
            {6.66666667e-02,  5.00000000e-02,  3.33333333e-02,
             1.66666667e-02, -3.46944695e-18, -1.66666667e-02,
            -3.33333333e-02, -5.00000000e-02, -6.66666667e-02};

        const std::vector <float> delta2Coeffs =
            {0.06060606,      0.01515152,     -0.01731602,
            -0.03679654,     -0.04329004,     -0.03679654,
            -0.01731602,      0.01515152,      0.06060606};

        if (delta1.size(0) == 0 || delta2.size(0) != delta1.size(0) ||
            mfcc.size(0) == 0 || mfcc.size(1) == 0) {
            return false;
        }

        /* Get the middle index; coeff vec len should always be odd. */
        const size_t coeffLen = delta1Coeffs.size();
        const size_t fMidIdx = (coeffLen - 1)/2;
        const size_t numFeatures = mfcc.size(0);
        const size_t numFeatVectors = mfcc.size(1);

        /* Iterate through features in MFCC vector. */
        for (size_t i = 0; i < numFeatures; ++i) {
            /* For each feature, iterate through time (t) samples representing feature evolution and
             * calculate d/dt and d^2/dt^2, using 1D convolution with differential kernels.
             * Convolution padding = valid, result size is `time length - kernel length + 1`.
             * The result is padded with 0 from both sides to match the size of initial time samples data.
             *
             * For the small filter, conv1D implementation as a simple loop is efficient enough.
             * Filters of a greater size would need CMSIS-DSP functions to be used, like arm_fir_f32.
             */

            for (size_t j = fMidIdx; j < numFeatVectors - fMidIdx; ++j) {
                float d1 = 0;
                float d2 = 0;
                const size_t mfccStIdx = j - fMidIdx;

                for (size_t k = 0, m = coeffLen - 1; k < coeffLen; ++k, --m) {

                    d1 +=  mfcc(i,mfccStIdx + k) * delta1Coeffs[m];
                    d2 +=  mfcc(i,mfccStIdx + k) * delta2Coeffs[m];
                }

                delta1(i,j) = d1;
                delta2(i,j) = d2;
            }
        }

        return true;
    }

    float Preprocess::_GetMean(Array2d<float>& vec)
    {
        return math::MathUtils::MeanF32(vec.begin(), vec.totalSize());
    }

    float Preprocess::_GetStdDev(Array2d<float>& vec, const float mean)
    {
        return math::MathUtils::StdDevF32(vec.begin(), vec.totalSize(), mean);
    }

    void Preprocess::_NormaliseVec(Array2d<float>& vec)
    {
        auto mean = Preprocess::_GetMean(vec);
        auto stddev = Preprocess::_GetStdDev(vec, mean);

        debug("Mean: %f, Stddev: %f\n", mean, stddev);
        if (stddev == 0) {
            std::fill(vec.begin(), vec.end(), 0);
        } else {
            const float stddevInv = 1.f/stddev;
            const float normalisedMean = mean/stddev;

            auto NormalisingFunction = [=](float& value) {
                value = value * stddevInv - normalisedMean;
            };
            std::for_each(vec.begin(), vec.end(), NormalisingFunction);
        }
    }

    void Preprocess::_Normalise()
    {
        Preprocess::_NormaliseVec(this->_m_mfccBuf);
        Preprocess::_NormaliseVec(this->_m_delta1Buf);
        Preprocess::_NormaliseVec(this->_m_delta2Buf);
    }

    float Preprocess::_GetQuantElem(
                const float     elem,
                const float     quantScale,
                const int       quantOffset,
                const float     minVal,
                const float     maxVal)
    {
        float val = std::round((elem/quantScale) + quantOffset);
        return std::min<float>(std::max<float>(val, minVal), maxVal);
    }

} /* namespace asr */
} /* namespace audio */
} /* namespace app */
} /* namespace arm */