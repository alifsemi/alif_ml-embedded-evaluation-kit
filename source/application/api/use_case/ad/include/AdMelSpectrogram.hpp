/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef ADMELSPECTROGRAM_HPP
#define ADMELSPECTROGRAM_HPP

#include "MelSpectrogram.hpp"

namespace arm {
namespace app {
namespace audio {

    /* Class to provide anomaly detection specific Mel Spectrogram calculation requirements */
    class AdMelSpectrogram : public MelSpectrogram {

    public:
        static constexpr uint32_t  ms_defaultSamplingFreq = 16000;
        static constexpr uint32_t  ms_defaultNumFbankBins =    64;
        static constexpr uint32_t  ms_defaultMelLoFreq    =     0;
        static constexpr uint32_t  ms_defaultMelHiFreq    =  8000;
        static constexpr bool      ms_defaultUseHtkMethod = false;

        explicit AdMelSpectrogram(const size_t frameLen)
                :  MelSpectrogram(MelSpecParams(
                ms_defaultSamplingFreq, ms_defaultNumFbankBins,
                ms_defaultMelLoFreq, ms_defaultMelHiFreq,
                frameLen, ms_defaultUseHtkMethod))
        {}

        AdMelSpectrogram()  = delete;
        virtual ~AdMelSpectrogram() = default;

    protected:

        /**
         * @brief       Overrides base class implementation of this function.
         * @param[in]   fftVec                  Vector populated with FFT magnitudes
         * @param[in]   melFilterBank           2D Vector with filter bank weights
         * @param[in]   filterBankFilterFirst   Vector containing the first indices of filter bank
         *                                      to be used for each bin.
         * @param[in]   filterBankFilterLast    Vector containing the last indices of filter bank
         *                                      to be used for each bin.
         * @param[out]  melEnergies             Pre-allocated vector of MEL energies to be
         *                                      populated.
         * @return      true if successful, false otherwise
         */
        virtual bool ApplyMelFilterBank(
                std::vector<float>&                 fftVec,
                std::vector<std::vector<float>>&    melFilterBank,
                std::vector<uint32_t>&               filterBankFilterFirst,
                std::vector<uint32_t>&               filterBankFilterLast,
                std::vector<float>&                 melEnergies) override;

        /**
         * @brief       Override for the base class implementation convert mel
         *              energies to logarithmic scale. The difference from
         *              default behaviour is that the power is converted to dB
         *              and subsequently clamped.
         * @param[in,out]   melEnergies - 1D vector of Mel energies
         **/
        virtual void ConvertToLogarithmicScale(std::vector<float>& melEnergies) override;

        /**
         * @brief       Given the low and high Mel values, get the normaliser
         *              for weights to be applied when populating the filter
         *              bank. Override for the base class implementation.
         * @param[in]   leftMel - low Mel frequency value
         * @param[in]   rightMel - high Mel frequency value
         * @param[in]   useHTKMethod - bool to signal if HTK method is to be
         *              used for calculation
         * @return      Return float value to be applied 
         *              when populating the filter bank.
         */
        virtual float GetMelFilterBankNormaliser(
                const float&   leftMel,
                const float&   rightMel,
                const bool     useHTKMethod) override;
    };

} /* namespace audio */
} /* namespace app */
} /* namespace arm */

#endif /* ADMELSPECTROGRAM_HPP */
