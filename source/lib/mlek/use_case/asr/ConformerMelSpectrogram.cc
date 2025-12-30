/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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

#include <algorithm>
#include <cassert>

#include "mlek/use_case/asr/ConformerMelSpectrogram.hpp"
#include "mlek/log/log_macros.h"

namespace arm::app::audio
{
    float ConformerMelSpectrogram::MelScale(const float freq, const bool useHTKMethod)
    {
        if (useHTKMethod)
        {
            return 2595.0f * log10f(1.0f + freq / 700.0f);
        }
        // Fall back to base class implementation
        return MelSpectrogram::MelScale(freq, useHTKMethod);
    }

    float ConformerMelSpectrogram::InverseMelScale(const float melFreq, const bool useHTKMethod)
    {
        if (useHTKMethod)
        {
            return 700.0f * (powf(10.0, melFreq / 2595.0f) - 1.0f);
        }
        // Fall back to base class implementation
        return MelSpectrogram::InverseMelScale(melFreq, useHTKMethod);
    }

    float ConformerMelSpectrogram::MelEnergy(float fftValue)
    {
        return fftValue;
    }

    std::vector<std::vector<float>> ConformerMelSpectrogram::CreateMelFilterBank()
    {
        size_t numFftBins = this->m_frameLenPadded / 2 + 1;
        float fftBinWidth = this->m_params.m_samplingFreq
            / static_cast<float>(this->m_frameLenPadded);

        float melLowFreq = MelScale(this->m_params.m_melLoFreq,
                                    this->m_params.m_useHtkMethod);
        float melHighFreq = MelScale(this->m_params.m_melHiFreq, this->m_params.m_useHtkMethod);
        float melFreqDelta = (melHighFreq - melLowFreq)
            / static_cast<float>(this->m_params.m_numFbankBins + 1);

        std::vector<float> thisBin = std::vector<float>(numFftBins);
        std::vector<std::vector<float>> melFilterBank(this->m_params.m_numFbankBins);
        this->m_filterBankFilterFirst = std::vector<uint32_t>(this->m_params.m_numFbankBins);
        this->m_filterBankFilterLast = std::vector<uint32_t>(this->m_params.m_numFbankBins);

        for (size_t bin = 0; bin < this->m_params.m_numFbankBins; bin++)
        {
            float leftMel = melLowFreq + bin * melFreqDelta;
            float centerMel = melLowFreq + (bin + 1) * melFreqDelta;
            float rightMel = melLowFreq + (bin + 2) * melFreqDelta;

            float leftFreq = InverseMelScale(leftMel);
            float centerFreq = InverseMelScale(centerMel);
            float rightFreq = InverseMelScale(rightMel);

            uint32_t firstIndex = 0;
            uint32_t lastIndex = 0;
            bool firstIndexFound = false;
            const float normaliser = this->GetMelFilterBankNormaliser(leftMel, rightMel, this->m_params.m_useHtkMethod);

            for (size_t i = 0; i < numFftBins; ++i)
            {
                float freq = fftBinWidth * i; /* Center freq of this fft bin. */
                float leftSlope = leftFreq - freq;
                float rightSlope = rightFreq - freq;
                float downSlope = -leftSlope / (centerFreq - leftFreq);
                float upSlope = rightSlope / (rightFreq - centerFreq);
                float fb = std::max(0.f, std::min(downSlope, upSlope));
                thisBin[i] = fb;
                if (fb > 0)
                {
                    thisBin[i] = fb * normaliser;
                    if (!firstIndexFound)
                    {
                        firstIndex = i;
                        firstIndexFound = true;
                    }
                    lastIndex = i;
                }
            }

            this->m_filterBankFilterFirst[bin] = firstIndex;
            this->m_filterBankFilterLast[bin] = lastIndex;

            /* Copy the part we care about. */
            for (uint32_t i = firstIndex; i <= lastIndex; ++i)
            {
                melFilterBank[bin].push_back(thisBin[i]);
            }
        }

        return melFilterBank;
    }
} // namespace arm::app::audio
