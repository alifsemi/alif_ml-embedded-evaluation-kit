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
#ifndef ASR_WAV2LETTER_MFCC_HPP
#define ASR_WAV2LETTER_MFCC_HPP

#include "Mfcc.hpp"

namespace arm {
namespace app {
namespace audio {

    /* Class to provide Wav2Letter specific MFCC calculation requirements. */
    class Wav2LetterMFCC : public MFCC {

    public:
        static constexpr uint32_t  ms_defaultSamplingFreq = 16000;
        static constexpr uint32_t  ms_defaultNumFbankBins =   128;
        static constexpr uint32_t  ms_defaultMelLoFreq    =     0;
        static constexpr uint32_t  ms_defaultMelHiFreq    =  8000;
        static constexpr bool      ms_defaultUseHtkMethod = false;

        explicit Wav2LetterMFCC(const size_t numFeats, const size_t frameLen)
            :  MFCC(MfccParams(
                        ms_defaultSamplingFreq, ms_defaultNumFbankBins,
                        ms_defaultMelLoFreq, ms_defaultMelHiFreq,
                        numFeats, frameLen, ms_defaultUseHtkMethod))
        {}

        Wav2LetterMFCC()  = delete;
        ~Wav2LetterMFCC() = default;

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
        bool ApplyMelFilterBank(
            std::vector<float>&                 fftVec,
            std::vector<std::vector<float>>&    melFilterBank,
            std::vector<uint32_t>&              filterBankFilterFirst,
            std::vector<uint32_t>&              filterBankFilterLast,
            std::vector<float>&                 melEnergies) override;

        /**
         * @brief           Override for the base class implementation convert mel
         *                  energies to logarithmic scale. The difference from
         *                  default behaviour is that the power is converted to dB
         *                  and subsequently clamped.
         * @param[in,out]   melEnergies   1D vector of Mel energies
         **/
        void ConvertToLogarithmicScale(std::vector<float>& melEnergies) override;

        /**
         * @brief       Create a matrix used to calculate Discrete Cosine
         *              Transform. Override for the base class' default
         *              implementation as the first and last elements
         *              use a different normaliser.
         * @param[in]   inputLength        input length of the buffer on which
         *                                 DCT will be performed
         * @param[in]   coefficientCount   Total coefficients per input length.
         * @return      1D vector with inputLength x coefficientCount elements
         *              populated with DCT coefficients.
         */
        std::vector<float> CreateDCTMatrix(int32_t inputLength,
                                           int32_t coefficientCount) override;

        /**
         * @brief       Given the low and high Mel values, get the normaliser
         *              for weights to be applied when populating the filter
         *              bank. Override for the base class implementation.
         * @param[in]   leftMel        Low Mel frequency value.
         * @param[in]   rightMel       High Mel frequency value.
         * @param[in]   useHTKMethod   bool to signal if HTK method is to be
         *                             used for calculation.
         * @return      Value to use for normalising.
         */
        float GetMelFilterBankNormaliser(const float&   leftMel,
                                         const float&   rightMel,
                                         bool     useHTKMethod) override;
    };

} /* namespace audio */
} /* namespace app */
} /* namespace arm */

#endif /* ASR_WAV2LETTER_MFCC_HPP */