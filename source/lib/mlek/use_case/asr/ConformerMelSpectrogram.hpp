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
#ifndef CONFORMERMELSPECTROGRAM_HPP
#define CONFORMERMELSPECTROGRAM_HPP

#include "mlek/common/MelSpectrogram.hpp"

namespace arm::app::audio
{
    /* Mel spectrogram customised to match the Conformer training and evaluation scripts */
    class ConformerMelSpectrogram : public MelSpectrogram
    {
    public:
        static constexpr uint32_t ms_defaultSamplingFreq = 16000;
        static constexpr uint32_t ms_defaultNumFbankBins = 80;
        static constexpr uint32_t ms_defaultMelLoFreq = 0;
        static constexpr uint32_t ms_defaultMelHiFreq = 8000;
        static constexpr bool ms_defaultUseHtkMethod = true;
        static constexpr float ms_defaultMelEnergyMin = 1e-6;

        explicit ConformerMelSpectrogram(const size_t frameLen)
            : MelSpectrogram(MelSpecParams(
                ms_defaultSamplingFreq, ms_defaultNumFbankBins,
                ms_defaultMelLoFreq, ms_defaultMelHiFreq,
                frameLen, ms_defaultUseHtkMethod, ms_defaultMelEnergyMin))
        {
        }

        ConformerMelSpectrogram() = delete;
        virtual ~ConformerMelSpectrogram() = default;

    protected:
        /**
         * @brief       Project input frequency to Mel Scale.
         * @param[in]   freq          input frequency in floating point
         * @param[in]   useHTKMethod  bool to signal if HTK method is to be
         *                            used for calculation
         * @return      Mel transformed frequency in floating point
         **/
        float MelScale(const float freq, const bool useHTKMethod = true) override;

        /**
         * @brief       Inverse Mel transform - convert MEL warped frequency
         *              back to normal frequency
         * @param[in]   melFreq          Mel frequency in floating point
         * @param[in]   useHTKMethod  bool to signal if HTK method is to be
         *                            used for calculation
         * @return      Real world frequency in floating point
         **/
        float InverseMelScale(const float melFreq, const bool useHTKMethod = true) override;

        /**
         * @brief           Convert a FFT value to a mel energy
         * @param fftValue  FFT value
         * @return          Mel energy
         */
        float MelEnergy(float fftValue) override;

        /**
         * @brief       Create mel filter banks for Mel Spectrogram calculation.
         *              Looking to replicate implementation found in PyTorch:
         *              https://docs.pytorch.org/audio/2.7.0/_modules/torchaudio/functional/functional.html#melscale_fbanks
         * @return      2D vector of floats
         **/
        std::vector<std::vector<float>> CreateMelFilterBank() override;
    };
} // namespace arm::app::audio


#endif /* CONFORMERMELSPECTROGRAM_HPP */
