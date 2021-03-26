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
#include "Mfcc.hpp"

#include "PlatformMath.hpp"

#include <cfloat>

namespace arm {
namespace app {
namespace audio {

    MfccParams::MfccParams(
                    const float samplingFreq,
                    const uint32_t numFbankBins,
                    const float melLoFreq,
                    const float melHiFreq,
                    const uint32_t numMfccFeats,
                    const uint32_t frameLen,
                    const bool useHtkMethod):
                        m_samplingFreq(samplingFreq),
                        m_numFbankBins(numFbankBins),
                        m_melLoFreq(melLoFreq),
                        m_melHiFreq(melHiFreq),
                        m_numMfccFeatures(numMfccFeats),
                        m_frameLen(frameLen),

                        /* Smallest power of 2 >= frame length. */
                        m_frameLenPadded(pow(2, ceil((log(frameLen)/log(2))))),
                        m_useHtkMethod(useHtkMethod)
    {}

    std::string MfccParams::Str()
    {
        char strC[1024];
        snprintf(strC, sizeof(strC) - 1, "\n   \
            \n\t Sampling frequency:         %f\
            \n\t Number of filter banks:     %u\
            \n\t Mel frequency limit (low):  %f\
            \n\t Mel frequency limit (high): %f\
            \n\t Number of MFCC features:    %u\
            \n\t Frame length:               %u\
            \n\t Padded frame length:        %u\
            \n\t Using HTK for Mel scale:    %s\n",
                this->m_samplingFreq, this->m_numFbankBins, this->m_melLoFreq,
                this->m_melHiFreq, this->m_numMfccFeatures, this->m_frameLen,
                this->m_frameLenPadded, this->m_useHtkMethod ? "yes" : "no");
        return std::string{strC};
    }

    MFCC::MFCC(const MfccParams& params):
        _m_params(params),
        _m_filterBankInitialised(false)
    {
        this->_m_buffer = std::vector<float>(
                            this->_m_params.m_frameLenPadded, 0.0);
        this->_m_frame = std::vector<float>(
                            this->_m_params.m_frameLenPadded, 0.0);
        this->_m_melEnergies = std::vector<float>(
                                this->_m_params.m_numFbankBins, 0.0);

        this->_m_windowFunc = std::vector<float>(this->_m_params.m_frameLen);
        const float multiplier = 2 * M_PI / this->_m_params.m_frameLen;

        /* Create window function. */
        for (size_t i = 0; i < this->_m_params.m_frameLen; i++) {
            this->_m_windowFunc[i] = (0.5 - (0.5 *
                math::MathUtils::CosineF32(static_cast<float>(i) * multiplier)));
        }

        math::MathUtils::FftInitF32(this->_m_params.m_frameLenPadded, this->_m_fftInstance);
        debug("Instantiated MFCC object: %s\n", this->_m_params.Str().c_str());
    }

    void MFCC::Init()
    {
        this->_InitMelFilterBank();
    }

    float MFCC::MelScale(const float freq, const bool useHTKMethod)
    {
        if (useHTKMethod) {
            return 1127.0f * logf (1.0f + freq / 700.0f);
        } else {
            /* Slaney formula for mel scale. */

            float mel = freq / ms_freqStep;

            if (freq >= ms_minLogHz) {
                mel = ms_minLogMel + logf(freq / ms_minLogHz) / ms_logStep;
            }
            return mel;
        }
    }

    float MFCC::InverseMelScale(const float melFreq, const bool useHTKMethod)
    {
        if (useHTKMethod) {
            return 700.0f * (expf (melFreq / 1127.0f) - 1.0f);
        } else {
            /* Slaney formula for mel scale. */
            float freq = ms_freqStep * melFreq;

            if (melFreq >= ms_minLogMel) {
                freq = ms_minLogHz * expf(ms_logStep * (melFreq - ms_minLogMel));
            }
            return freq;
        }
    }


    bool MFCC::ApplyMelFilterBank(
            std::vector<float>&                 fftVec,
            std::vector<std::vector<float>>&    melFilterBank,
            std::vector<int32_t>&               filterBankFilterFirst,
            std::vector<int32_t>&               filterBankFilterLast,
            std::vector<float>&                 melEnergies)
    {
        const size_t numBanks = melEnergies.size();

        if (numBanks != filterBankFilterFirst.size() ||
                numBanks != filterBankFilterLast.size()) {
            printf_err("unexpected filter bank lengths\n");
            return false;
        }

        for (size_t bin = 0; bin < numBanks; ++bin) {
            auto filterBankIter = melFilterBank[bin].begin();
            float melEnergy = FLT_MIN;  /* Avoid log of zero at later stages */
            int32_t firstIndex = filterBankFilterFirst[bin];
            int32_t lastIndex = filterBankFilterLast[bin];

            for (int i = firstIndex; i <= lastIndex; i++) {
                float energyRep = math::MathUtils::SqrtF32(fftVec[i]);
                melEnergy += (*filterBankIter++ * energyRep);
            }

            melEnergies[bin] = melEnergy;
        }

        return true;
    }

    void MFCC::ConvertToLogarithmicScale(std::vector<float>& melEnergies)
    {
        for (size_t bin = 0; bin < melEnergies.size(); ++bin) {
            melEnergies[bin] = logf(melEnergies[bin]);
        }
    }

    void MFCC::_ConvertToPowerSpectrum()
    {
        const uint32_t halfDim = this->_m_params.m_frameLenPadded / 2;

        /* Handle this special case. */
        float firstEnergy = this->_m_buffer[0] * this->_m_buffer[0];
        float lastEnergy = this->_m_buffer[1] * this->_m_buffer[1];

        math::MathUtils::ComplexMagnitudeSquaredF32(
                            this->_m_buffer.data(),
                            this->_m_buffer.size(),
                            this->_m_buffer.data(),
                            this->_m_buffer.size()/2);

        this->_m_buffer[0] = firstEnergy;
        this->_m_buffer[halfDim] = lastEnergy;
    }

    std::vector<float> MFCC::CreateDCTMatrix(
                                const int32_t inputLength,
                                const int32_t coefficientCount)
    {
        std::vector<float> dctMatix(inputLength * coefficientCount);

        const float normalizer = math::MathUtils::SqrtF32(2.0f/inputLength);
        const float angleIncr = M_PI/inputLength;
        float angle = 0;

        for (int32_t k = 0, m = 0; k < coefficientCount; k++, m += inputLength) {
            for (int32_t n = 0; n < inputLength; n++) {
                dctMatix[m+n] = normalizer *
                    math::MathUtils::CosineF32((n + 0.5) * angle);
            }
            angle += angleIncr;
        }

        return dctMatix;
    }

    float MFCC::GetMelFilterBankNormaliser(
                    const float&    leftMel,
                    const float&    rightMel,
                    const bool      useHTKMethod)
    {
        UNUSED(leftMel);
        UNUSED(rightMel);
        UNUSED(useHTKMethod);

        /* By default, no normalisation => return 1 */
        return 1.f;
    }

    void MFCC::_InitMelFilterBank()
    {
        if (!this->_IsMelFilterBankInited()) {
            this->_m_melFilterBank = this->_CreateMelFilterBank();
            this->_m_dctMatrix = this->CreateDCTMatrix(
                                    this->_m_params.m_numFbankBins,
                                    this->_m_params.m_numMfccFeatures);
            this->_m_filterBankInitialised = true;
        }
    }

    bool MFCC::_IsMelFilterBankInited()
    {
        return this->_m_filterBankInitialised;
    }

    void MFCC::_MfccComputePreFeature(const std::vector<int16_t>& audioData)
    {
        this->_InitMelFilterBank();

        /* TensorFlow way of normalizing .wav data to (-1, 1). */
        constexpr float normaliser = 1.0/(1<<15);
        for (size_t i = 0; i < this->_m_params.m_frameLen; i++) {
            this->_m_frame[i] = static_cast<float>(audioData[i]) * normaliser;
        }

        /* Apply window function to input frame. */
        for(size_t i = 0; i < this->_m_params.m_frameLen; i++) {
            this->_m_frame[i] *= this->_m_windowFunc[i];
        }

        /* Set remaining frame values to 0. */
        std::fill(this->_m_frame.begin() + this->_m_params.m_frameLen,this->_m_frame.end(), 0);

        /* Compute FFT. */
        math::MathUtils::FftF32(this->_m_frame, this->_m_buffer, this->_m_fftInstance);

        /* Convert to power spectrum. */
        this->_ConvertToPowerSpectrum();

        /* Apply mel filterbanks. */
        if (!this->ApplyMelFilterBank(this->_m_buffer,
                                      this->_m_melFilterBank,
                                      this->_m_filterBankFilterFirst,
                                      this->_m_filterBankFilterLast,
                                      this->_m_melEnergies)) {
            printf_err("Failed to apply MEL filter banks\n");
        }

        /* Convert to logarithmic scale. */
        this->ConvertToLogarithmicScale(this->_m_melEnergies);
    }

    std::vector<float> MFCC::MfccCompute(const std::vector<int16_t>& audioData)
    {
        this->_MfccComputePreFeature(audioData);

        std::vector<float> mfccOut(this->_m_params.m_numMfccFeatures);

        float * ptrMel = this->_m_melEnergies.data();
        float * ptrDct = this->_m_dctMatrix.data();
        float * ptrMfcc = mfccOut.data();

        /* Take DCT. Uses matrix mul. */
        for (size_t i = 0, j = 0; i < mfccOut.size();
                    ++i, j += this->_m_params.m_numFbankBins) {
            *ptrMfcc++ = math::MathUtils::DotProductF32(
                                            ptrDct + j,
                                            ptrMel,
                                            this->_m_params.m_numFbankBins);
        }
        return mfccOut;
    }

    std::vector<std::vector<float>> MFCC::_CreateMelFilterBank()
    {
        size_t numFftBins = this->_m_params.m_frameLenPadded / 2;
        float fftBinWidth = static_cast<float>(this->_m_params.m_samplingFreq) / this->_m_params.m_frameLenPadded;

        float melLowFreq = MFCC::MelScale(this->_m_params.m_melLoFreq,
                                          this->_m_params.m_useHtkMethod);
        float melHighFreq = MFCC::MelScale(this->_m_params.m_melHiFreq,
                                           this->_m_params.m_useHtkMethod);
        float melFreqDelta = (melHighFreq - melLowFreq) / (this->_m_params.m_numFbankBins + 1);

        std::vector<float> thisBin = std::vector<float>(numFftBins);
        std::vector<std::vector<float>> melFilterBank(
                                            this->_m_params.m_numFbankBins);
        this->_m_filterBankFilterFirst =
                        std::vector<int32_t>(this->_m_params.m_numFbankBins);
        this->_m_filterBankFilterLast =
                        std::vector<int32_t>(this->_m_params.m_numFbankBins);

        for (size_t bin = 0; bin < this->_m_params.m_numFbankBins; bin++) {
            float leftMel = melLowFreq + bin * melFreqDelta;
            float centerMel = melLowFreq + (bin + 1) * melFreqDelta;
            float rightMel = melLowFreq + (bin + 2) * melFreqDelta;

            int32_t firstIndex = -1;
            int32_t lastIndex = -1;
            const float normaliser = this->GetMelFilterBankNormaliser(leftMel, rightMel, this->_m_params.m_useHtkMethod);

            for (size_t i = 0; i < numFftBins; i++) {
                float freq = (fftBinWidth * i);  /* Center freq of this fft bin. */
                float mel = MFCC::MelScale(freq, this->_m_params.m_useHtkMethod);
                thisBin[i] = 0.0;

                if (mel > leftMel && mel < rightMel) {
                    float weight;
                    if (mel <= centerMel) {
                        weight = (mel - leftMel) / (centerMel - leftMel);
                    } else {
                        weight = (rightMel - mel) / (rightMel - centerMel);
                    }

                    thisBin[i] = weight * normaliser;
                    if (firstIndex == -1) {
                        firstIndex = i;
                    }
                    lastIndex = i;
                }
            }

            this->_m_filterBankFilterFirst[bin] = firstIndex;
            this->_m_filterBankFilterLast[bin] = lastIndex;

            /* Copy the part we care about. */
            for (int32_t i = firstIndex; i <= lastIndex; i++) {
                melFilterBank[bin].push_back(thisBin[i]);
            }
        }

        return melFilterBank;
    }

} /* namespace audio */
} /* namespace app */
} /* namespace arm */
