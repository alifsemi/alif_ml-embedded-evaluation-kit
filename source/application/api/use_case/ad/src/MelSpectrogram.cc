/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "MelSpectrogram.hpp"

#include "PlatformMath.hpp"
#include "log_macros.h"

#include <cfloat>
#include <cinttypes>

namespace arm {
namespace app {
namespace audio {

    MelSpecParams::MelSpecParams(
            const float samplingFreq,
            const uint32_t numFbankBins,
            const float melLoFreq,
            const float melHiFreq,
            const uint32_t frameLen,
            const bool useHtkMethod):
            m_samplingFreq(samplingFreq),
            m_numFbankBins(numFbankBins),
            m_melLoFreq(melLoFreq),
            m_melHiFreq(melHiFreq),
            m_frameLen(frameLen),

            /* Smallest power of 2 >= frame length. */
            m_frameLenPadded(pow(2, ceil((log(frameLen)/log(2))))),
            m_useHtkMethod(useHtkMethod)
    {}

    std::string MelSpecParams::Str() const
    {
        char strC[1024];
        snprintf(strC, sizeof(strC) - 1, "\n   \
            \n\t Sampling frequency:         %f\
            \n\t Number of filter banks:     %" PRIu32 "\
            \n\t Mel frequency limit (low):  %f\
            \n\t Mel frequency limit (high): %f\
            \n\t Frame length:               %" PRIu32 "\
            \n\t Padded frame length:        %" PRIu32 "\
            \n\t Using HTK for Mel scale:    %s\n",
            this->m_samplingFreq, this->m_numFbankBins, this->m_melLoFreq,
            this->m_melHiFreq, this->m_frameLen,
            this->m_frameLenPadded, this->m_useHtkMethod ? "yes" : "no");
        return std::string{strC};
    }

    MelSpectrogram::MelSpectrogram(const MelSpecParams& params):
            m_params(params),
            m_filterBankInitialised(false)
    {
        this->m_buffer = std::vector<float>(
                this->m_params.m_frameLenPadded, 0.0);
        this->m_frame = std::vector<float>(
                this->m_params.m_frameLenPadded, 0.0);
        this->m_melEnergies = std::vector<float>(
                this->m_params.m_numFbankBins, 0.0);

        this->m_windowFunc = std::vector<float>(this->m_params.m_frameLen);
        const auto multiplier = static_cast<float>(2 * M_PI / this->m_params.m_frameLen);

        /* Create window function. */
        for (size_t i = 0; i < this->m_params.m_frameLen; ++i) {
            this->m_windowFunc[i] = (0.5 - (0.5 *
                                             math::MathUtils::CosineF32(static_cast<float>(i) * multiplier)));
        }

        math::MathUtils::FftInitF32(this->m_params.m_frameLenPadded, this->m_fftInstance);
        debug("Instantiated Mel Spectrogram object: %s\n", this->m_params.Str().c_str());
    }

    void MelSpectrogram::Init()
    {
        this->InitMelFilterBank();
    }

    float MelSpectrogram::MelScale(const float freq, const bool useHTKMethod)
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

    float MelSpectrogram::InverseMelScale(const float melFreq, const bool useHTKMethod)
    {
        if (useHTKMethod) {
            return 700.0f * (expf (melFreq / 1127.0f) - 1.0f);
        } else {
            /* Slaney formula for inverse mel scale. */
            float freq = ms_freqStep * melFreq;

            if (melFreq >= ms_minLogMel) {
                freq = ms_minLogHz * expf(ms_logStep * (melFreq - ms_minLogMel));
            }
            return freq;
        }
    }

    bool MelSpectrogram::ApplyMelFilterBank(
            std::vector<float>&                 fftVec,
            std::vector<std::vector<float>>&    melFilterBank,
            std::vector<uint32_t>&               filterBankFilterFirst,
            std::vector<uint32_t>&               filterBankFilterLast,
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
            auto end = melFilterBank[bin].end();
            float melEnergy = FLT_MIN; /* Avoid log of zero at later stages */
            const uint32_t firstIndex = filterBankFilterFirst[bin];
            const uint32_t lastIndex = std::min<int32_t>(filterBankFilterLast[bin], fftVec.size() - 1);

            for (uint32_t i = firstIndex; i <= lastIndex && filterBankIter != end; ++i) {
                float energyRep = math::MathUtils::SqrtF32(fftVec[i]);
                melEnergy += (*filterBankIter++ * energyRep);
            }

            melEnergies[bin] = melEnergy;
        }

        return true;
    }

    void MelSpectrogram::ConvertToLogarithmicScale(std::vector<float>& melEnergies)
    {
        math::MathUtils::VecLogarithmF32(melEnergies, melEnergies);
    }

    void MelSpectrogram::ConvertToPowerSpectrum()
    {
        const uint32_t halfDim = this->m_buffer.size() / 2;

        /* Handle this special case. */
        float firstEnergy = this->m_buffer[0] * this->m_buffer[0];
        float lastEnergy = this->m_buffer[1] * this->m_buffer[1];

        math::MathUtils::ComplexMagnitudeSquaredF32(
                this->m_buffer.data(),
                this->m_buffer.size(),
                this->m_buffer.data(),
                this->m_buffer.size()/2);

        this->m_buffer[0] = firstEnergy;
        this->m_buffer[halfDim] = lastEnergy;
    }

    float MelSpectrogram::GetMelFilterBankNormaliser(
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

    void MelSpectrogram::InitMelFilterBank()
    {
        if (!this->IsMelFilterBankInited()) {
            this->m_melFilterBank = this->CreateMelFilterBank();
            this->m_filterBankInitialised = true;
        }
    }

    bool MelSpectrogram::IsMelFilterBankInited() const
    {
        return this->m_filterBankInitialised;
    }

    std::vector<float> MelSpectrogram::ComputeMelSpec(const std::vector<int16_t>& audioData, float trainingMean)
    {
        this->InitMelFilterBank();

        /* TensorFlow way of normalizing .wav data to (-1, 1). */
        constexpr float normaliser = 1.0/(1<<15);
        for (size_t i = 0; i < this->m_params.m_frameLen; ++i) {
            this->m_frame[i] = static_cast<float>(audioData[i]) * normaliser;
        }

        /* Apply window function to input frame. */
        for(size_t i = 0; i < this->m_params.m_frameLen; ++i) {
            this->m_frame[i] *= this->m_windowFunc[i];
        }

        /* Set remaining frame values to 0. */
        std::fill(this->m_frame.begin() + this->m_params.m_frameLen,this->m_frame.end(), 0);

        /* Compute FFT. */
        math::MathUtils::FftF32(this->m_frame, this->m_buffer, this->m_fftInstance);

        /* Convert to power spectrum. */
        this->ConvertToPowerSpectrum();

        /* Apply mel filterbanks. */
        if (!this->ApplyMelFilterBank(this->m_buffer,
                                      this->m_melFilterBank,
                                      this->m_filterBankFilterFirst,
                                      this->m_filterBankFilterLast,
                                      this->m_melEnergies)) {
            printf_err("Failed to apply MEL filter banks\n");
        }

        /* Convert to logarithmic scale */
        this->ConvertToLogarithmicScale(this->m_melEnergies);

        /* Perform mean subtraction. */
        for (auto& energy:this->m_melEnergies) {
            energy -= trainingMean;
        }

        return this->m_melEnergies;
    }

    std::vector<std::vector<float>> MelSpectrogram::CreateMelFilterBank()
    {
        size_t numFftBins = this->m_params.m_frameLenPadded / 2;
        float fftBinWidth = static_cast<float>(this->m_params.m_samplingFreq) / this->m_params.m_frameLenPadded;

        float melLowFreq = MelSpectrogram::MelScale(this->m_params.m_melLoFreq,
                                          this->m_params.m_useHtkMethod);
        float melHighFreq = MelSpectrogram::MelScale(this->m_params.m_melHiFreq,
                                           this->m_params.m_useHtkMethod);
        float melFreqDelta = (melHighFreq - melLowFreq) / (this->m_params.m_numFbankBins + 1);

        std::vector<float> thisBin = std::vector<float>(numFftBins);
        std::vector<std::vector<float>> melFilterBank(
                this->m_params.m_numFbankBins);
        this->m_filterBankFilterFirst =
                std::vector<uint32_t>(this->m_params.m_numFbankBins);
        this->m_filterBankFilterLast =
                std::vector<uint32_t>(this->m_params.m_numFbankBins);

        for (size_t bin = 0; bin < this->m_params.m_numFbankBins; bin++) {
            float leftMel = melLowFreq + bin * melFreqDelta;
            float centerMel = melLowFreq + (bin + 1) * melFreqDelta;
            float rightMel = melLowFreq + (bin + 2) * melFreqDelta;

            uint32_t firstIndex = 0;
            uint32_t lastIndex = 0;
            bool firstIndexFound = false;
            const float normaliser = this->GetMelFilterBankNormaliser(leftMel, rightMel, this->m_params.m_useHtkMethod);

            for (size_t i = 0; i < numFftBins; ++i) {
                float freq = (fftBinWidth * i); /* Center freq of this fft bin. */
                float mel = MelSpectrogram::MelScale(freq, this->m_params.m_useHtkMethod);
                thisBin[i] = 0.0;

                if (mel > leftMel && mel < rightMel) {
                    float weight;
                    if (mel <= centerMel) {
                        weight = (mel - leftMel) / (centerMel - leftMel);
                    } else {
                        weight = (rightMel - mel) / (rightMel - centerMel);
                    }

                    thisBin[i] = weight * normaliser;
                    if (!firstIndexFound) {
                        firstIndex = i;
                        firstIndexFound = true;
                    }
                    lastIndex = i;
                }
            }

            this->m_filterBankFilterFirst[bin] = firstIndex;
            this->m_filterBankFilterLast[bin] = lastIndex;

            /* Copy the part we care about. */
            for (uint32_t i = firstIndex; i <= lastIndex; ++i) {
                melFilterBank[bin].push_back(thisBin[i]);
            }
        }

        return melFilterBank;
    }

} /* namespace audio */
} /* namespace app */
} /* namespace arm */
