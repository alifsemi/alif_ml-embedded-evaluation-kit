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
#include "AdMelSpectrogram.hpp"
#include "PlatformMath.hpp"
#include "log_macros.h"

#include <cfloat>

namespace arm {
namespace app {
namespace audio {

    bool AdMelSpectrogram::ApplyMelFilterBank(
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
            float melEnergy = FLT_MIN; /* Avoid log of zero at later stages. */
            const uint32_t firstIndex = filterBankFilterFirst[bin];
            const uint32_t lastIndex = std::min<int32_t>(filterBankFilterLast[bin], fftVec.size() - 1);

            for (uint32_t i = firstIndex; i <= lastIndex && filterBankIter != end; ++i) {
                melEnergy += (*filterBankIter++ * fftVec[i]);
            }

            melEnergies[bin] = melEnergy;
        }

        return true;
    }

    void AdMelSpectrogram::ConvertToLogarithmicScale(
            std::vector<float>& melEnergies)
    {
        /* Container for natural logarithms of mel energies */
        std::vector <float> vecLogEnergies(melEnergies.size(), 0.f);

        /* Because we are taking natural logs, we need to multiply by log10(e).
         * Also, for wav2letter model, we scale our log10 values by 10 */
        constexpr float multiplier = 10.0 * /* default scalar */
                                     0.4342944819032518; /* log10f(std::exp(1.0))*/

        /* Take log of the whole vector */
        math::MathUtils::VecLogarithmF32(melEnergies, vecLogEnergies);

        /* Scale the log values. */
        for (auto iterM = melEnergies.begin(), iterL = vecLogEnergies.begin();
             iterM != melEnergies.end() && iterL != vecLogEnergies.end(); ++iterM, ++iterL) {

            *iterM = *iterL * multiplier;
        }
    }

    float AdMelSpectrogram::GetMelFilterBankNormaliser(
            const float&    leftMel,
            const float&    rightMel,
            const bool      useHTKMethod)
    {
        /* Slaney normalization for mel weights. */
        return (2.0f / (AdMelSpectrogram::InverseMelScale(rightMel, useHTKMethod) -
                        AdMelSpectrogram::InverseMelScale(leftMel, useHTKMethod)));
    }

} /* namespace audio */
} /* namespace app */
} /* namespace arm */
