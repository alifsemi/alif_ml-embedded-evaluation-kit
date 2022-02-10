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

#include <algorithm>
#include <catch.hpp>
#include <limits>

/* First 512 samples from itellyou.wav. */
const std::vector<int16_t> testWav1 = std::vector<int16_t> {
    -3,0,1,-1,2,3,-2,2,
    1,-2,0,3,-1,8,3,2,
    -1,-1,2,7,3,5,6,6,
    6,12,5,6,3,3,5,4,
    4,6,7,7,7,3,7,2,
    8,4,4,2,-4,-1,-1,-4,
    2,1,-1,-4,0,-7,-6,-2,
    -5,1,-5,-1,-7,-3,-3,-7,
    0,-3,3,-5,0,1,-2,-2,
    -3,-3,-7,-3,-2,-6,-5,-8,
    -2,-8,4,-9,-4,-9,-5,-5,
    -3,-9,-3,-9,-1,-7,-4,1,
    -3,2,-8,-4,-4,-5,1,-3,
    -1,0,-1,-2,-3,-2,-4,-1,
    1,-1,3,0,3,2,0,0,
    0,-3,1,1,0,8,3,4,
    1,5,6,4,7,3,3,0,
    3,6,7,6,4,5,9,9,
    5,5,8,1,6,9,6,6,
    7,1,8,1,5,0,5,5,
    0,3,2,7,2,-3,3,0,
    3,0,0,0,2,0,-1,-1,
    -2,-3,-8,0,1,0,-3,-3,
    -3,-2,-3,-3,-4,-6,-2,-8,
    -9,-4,-1,-5,-3,-3,-4,-3,
    -6,3,0,-1,-2,-9,-4,-2,
    2,-1,3,-5,-5,-2,0,-2,
    0,-1,-3,1,-2,9,4,5,
    2,2,1,0,-6,-2,0,0,
    0,-1,4,-4,3,-7,-1,5,
    -6,-1,-5,4,3,9,-2,1,
    3,0,0,-2,1,2,1,1,
    0,3,2,-1,3,-3,7,0,
    0,3,2,2,-2,3,-2,2,
    -3,4,-1,-1,-5,-1,-3,-2,
    1,-1,3,2,4,1,2,-2,
    0,2,7,0,8,-3,6,-3,
    6,1,2,-3,-1,-1,-1,1,
    -2,2,1,2,0,-2,3,-2,
    3,-2,1,0,-3,-1,-2,-4,
    -6,-5,-8,-1,-4,0,-3,-1,
    -1,-1,0,-2,-3,-7,-1,0,
    1,5,0,5,1,1,-3,0,
    -6,3,-8,4,-8,6,-6,1,
    -6,-2,-5,-6,0,-5,4,-1,
    4,-2,1,2,1,0,-2,0,
    0,2,-2,2,-5,2,0,-2,
    1,-2,0,5,1,0,1,5,
    0,8,3,2,2,0,5,-2,
    3,1,0,1,0,-2,-1,-3,
    1,-1,3,0,3,0,-2,-1,
    -4,-4,-4,-1,-4,-4,-3,-6,
    -3,-7,-3,-1,-2,0,-5,-4,
    -7,-3,-2,-2,1,2,2,8,
    5,4,2,4,3,5,0,3,
    3,6,4,2,2,-2,4,-2,
    3,3,2,1,1,4,-5,2,
    -3,0,-1,1,-2,2,5,1,
    4,2,3,1,-1,1,0,6,
    0,-2,-1,1,-1,2,-5,-1,
    -5,-1,-6,-3,-3,2,4,0,
    -1,-5,3,-4,-1,-3,-4,1,
    -4,1,-1,-1,0,-5,-4,-2,
    -1,-1,-3,-7,-3,-3,4,4,
};

const std::vector<int16_t> testWav2 = std::vector<int16_t> (512, 0);

/* Golden mfcc output for testwav1. */
const std::vector<float> golden_mfcc_output_testWav1 {
    -835.24603, 21.010452, 18.699404, 7.4338417, 19.028961, -5.401735, 6.4761047, -11.400679,
    8.392709, 12.202361, 8.403276, -13.508412, -18.307348
};

/* Golden mfcc output for the all zero wav. */
const std::vector<float> golden_mfcc_output_testWav2 {
    -1131.37085, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


arm::app::audio::Wav2LetterMFCC GetMFCCInstance()
{
    const auto sampFreq = arm::app::audio::Wav2LetterMFCC::ms_defaultSamplingFreq;
    const auto frameLenMs = 32;
    const auto numMfccFeats = 13;
    const auto frameLenSamples = sampFreq * frameLenMs * 0.001;
    return arm::app::audio::Wav2LetterMFCC(numMfccFeats, frameLenSamples);
}

template <class T>
void TestQuantisedMFCC()
{
    const auto quantScale = 0.1410219967365265;
    const auto quantOffset = 11;
    std::vector<T> mfccOutput = GetMFCCInstance().MfccComputeQuant<T>(testWav1, quantScale, quantOffset);

    long min_val = std::numeric_limits<T>::min();
    long max_val = std::numeric_limits<T>::max();

    for (size_t i = 0; i < golden_mfcc_output_testWav1.size(); i++){
        long TestWavMfcc = (std::lround((golden_mfcc_output_testWav1[i] / quantScale) + quantOffset));
        T quantizedTestWavMfcc = static_cast<T>(std::max(min_val, std::min(TestWavMfcc, max_val)));

        REQUIRE(quantizedTestWavMfcc  == Approx(mfccOutput[i]).margin(2));
    }
}

template void TestQuantisedMFCC<int8_t>();
template void TestQuantisedMFCC<uint8_t>();
template void TestQuantisedMFCC<int16_t>();

TEST_CASE("MFCC calculation")
{
    SECTION("FP32")
    {
        auto mfccOutput = GetMFCCInstance().MfccCompute(testWav1);
        REQUIRE_THAT( mfccOutput, Catch::Approx( golden_mfcc_output_testWav1 ).margin(0.3) );

        auto mfccOutput2 = GetMFCCInstance().MfccCompute(testWav2);
        REQUIRE_THAT( mfccOutput2, Catch::Approx( golden_mfcc_output_testWav2 ).margin(0.001) );
    }

    SECTION("int8_t")
    {
        TestQuantisedMFCC<int8_t>();
    }

    SECTION("uint8_t")
    {
        TestQuantisedMFCC<uint8_t>();
    }

    SECTION("int16_t")
    {
        TestQuantisedMFCC<int16_t>();
    }
}
