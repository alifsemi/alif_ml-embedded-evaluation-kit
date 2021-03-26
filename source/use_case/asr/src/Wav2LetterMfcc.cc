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
#include "Wav2LetterMfcc.hpp"

#include "PlatformMath.hpp"

#include <cfloat>

namespace arm {
namespace app {
namespace audio {

    bool Wav2LetterMFCC::ApplyMelFilterBank(
            std::vector<float>&                 fftVec,
            std::vector<std::vector<float>>&    melFilterBank,
            std::vector<int32_t>&               filterBankFilterFirst,
            std::vector<int32_t>&               filterBankFilterLast,
            std::vector<float>&                 melEnergies)
    {
        const size_t numBanks = melEnergies.size();

        if (numBanks != filterBankFilterFirst.size() ||
                numBanks != filterBankFilterLast.size()) {
            printf_err("Unexpected filter bank lengths\n");
            return false;
        }

        for (size_t bin = 0; bin < numBanks; ++bin) {
            auto filterBankIter = melFilterBank[bin].begin();
            float melEnergy = 1e-10;  /* Avoid log of zero at later stages, same value used in librosa. */
            const int32_t firstIndex = filterBankFilterFirst[bin];
            const int32_t lastIndex = filterBankFilterLast[bin];

            for (int32_t i = firstIndex; i <= lastIndex; ++i) {
                melEnergy += (*filterBankIter++ * fftVec[i]);
            }

            melEnergies[bin] = melEnergy;
        }

        return true;
    }

    void Wav2LetterMFCC::ConvertToLogarithmicScale(
                            std::vector<float>& melEnergies)
    {
        float maxMelEnergy = -FLT_MAX;

        /* Container for natural logarithms of mel energies. */
        std::vector <float> vecLogEnergies(melEnergies.size(), 0.f);

        /* Because we are taking natural logs, we need to multiply by log10(e).
         * Also, for wav2letter model, we scale our log10 values by 10. */
        constexpr float multiplier = 10.0 *  /* Default scalar. */
                                      0.4342944819032518;  /* log10f(std::exp(1.0)) */

        /* Take log of the whole vector. */
        math::MathUtils::VecLogarithmF32(melEnergies, vecLogEnergies);

        /* Scale the log values and get the max. */
        for (auto iterM = melEnergies.begin(), iterL = vecLogEnergies.begin();
                  iterM != melEnergies.end(); ++iterM, ++iterL) {

            *iterM = *iterL * multiplier;

            /* Save the max mel energy. */
            if (*iterM > maxMelEnergy) {
                maxMelEnergy = *iterM;
            }
        }

        /* Clamp the mel energies. */
        constexpr float maxDb = 80.0;
        const float clampLevelLowdB = maxMelEnergy - maxDb;
        for (auto iter = melEnergies.begin(); iter != melEnergies.end(); ++iter) {
            *iter = std::max(*iter, clampLevelLowdB);
        }
    }

    std::vector<float> Wav2LetterMFCC::CreateDCTMatrix(
                                        const int32_t inputLength,
                                        const int32_t coefficientCount)
    {
        std::vector<float> dctMatix(inputLength * coefficientCount);

        /* Orthonormal normalization. */
        const float normalizerK0 = 2 * math::MathUtils::SqrtF32(1.0f /
                                        static_cast<float>(4*inputLength));
        const float normalizer = 2 * math::MathUtils::SqrtF32(1.0f /
                                        static_cast<float>(2*inputLength));

        const float angleIncr = M_PI / inputLength;
        float angle = angleIncr;  /* We start using it at k = 1 loop. */

        /* First row of DCT will use normalizer K0. */
        for (int32_t n = 0; n < inputLength; ++n) {
            dctMatix[n] = normalizerK0  /* cos(0) = 1 */;
        }

        /* Second row (index = 1) onwards, we use standard normalizer. */
        for (int32_t k = 1, m = inputLength; k < coefficientCount; ++k, m += inputLength) {
            for (int32_t n = 0; n < inputLength; ++n) {
                dctMatix[m+n] = normalizer *
                    math::MathUtils::CosineF32((n + 0.5f) * angle);
            }
            angle += angleIncr;
        }
        return dctMatix;
    }

    float Wav2LetterMFCC::GetMelFilterBankNormaliser(
                                    const float&    leftMel,
                                    const float&    rightMel,
                                    const bool      useHTKMethod)
    {
        /* Slaney normalization for mel weights. */
        return (2.0f / (MFCC::InverseMelScale(rightMel, useHTKMethod) -
                MFCC::InverseMelScale(leftMel, useHTKMethod)));
    }

} /* namespace audio */
} /* namespace app */
} /* namespace arm */
