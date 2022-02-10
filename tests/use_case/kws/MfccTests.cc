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
#include "MicroNetKwsMfcc.hpp"

#include <algorithm>
#include <catch.hpp>
#include <limits>

/* First 640 samples from yes.wav. */
const std::vector<int16_t> testWav = std::vector<int16_t>{
    139, 143, 164, 163, 157, 156, 151, 148, 172, 171,
    165, 169, 149, 142, 145, 147, 166, 146, 112, 132,
    132, 136, 165, 176, 176, 152, 138, 158, 179, 185,
    183, 148, 121, 130, 167, 204, 163, 132, 165, 184,
    193, 205, 210, 204, 195, 178, 168, 197, 207, 201,
    197, 177, 185, 196, 191, 198, 196, 183, 193, 181,
    157, 170, 167, 159, 164, 152, 146, 167, 180, 171,
    194, 232, 204, 173, 171, 172, 184, 169, 175, 199,
    200, 195, 185, 214, 214, 193, 196, 191, 204, 191,
    172, 187, 183, 192, 203, 172, 182, 228, 232, 205,
    177, 174, 191, 210, 210, 211, 197, 177, 198, 217,
    233, 236, 203, 191, 169, 145, 149, 161, 198, 206,
    176, 137, 142, 181, 200, 215, 201, 188, 166, 162,
    184, 155, 135, 132, 126, 142, 169, 184, 172, 156,
    132, 119, 150, 147, 154, 160, 125, 130, 137, 154,
    161, 168, 195, 182, 160, 134, 138, 146, 130, 120,
    101, 122, 137, 118, 117, 131, 145, 140, 146, 148,
    148, 168, 159, 134, 114, 114, 130, 147, 147, 134,
    125, 98, 107, 127, 99, 79, 84, 107, 117, 114,
    93, 92, 127, 112, 109, 110, 96, 118, 97, 87,
    110, 95, 128, 153, 147, 165, 146, 106, 101, 137,
    139, 96, 73, 90, 91, 51, 69, 102, 100, 103,
    96, 101, 123, 107, 82, 89, 118, 127, 99, 100,
    111, 97, 111, 123, 106, 121, 133, 103, 100, 88,
    85, 111, 114, 125, 102, 91, 97, 84, 139, 157,
    109, 66, 72, 129, 111, 90, 127, 126, 101, 109,
    142, 138, 129, 159, 140, 80, 74, 78, 76, 98,
    68, 42, 106, 143, 112, 102, 115, 114, 82, 75,
    92, 80, 110, 114, 66, 86, 119, 101, 101, 103,
    118, 145, 85, 40, 62, 88, 95, 87, 73, 64,
    86, 71, 71, 105, 80, 73, 96, 92, 85, 90,
    81, 86, 105, 100, 89, 78, 102, 114, 95, 98,
    69, 70, 108, 112, 111, 90, 104, 137, 143, 160,
    145, 121, 98, 86, 91, 87, 115, 123, 109, 99,
    85, 120, 131, 116, 125, 144, 153, 111, 98, 110,
    93, 89, 101, 137, 155, 142, 108, 94, 136, 145,
    129, 129, 122, 109, 90, 76, 81, 110, 119, 96,
    95, 102, 105, 111, 90, 89, 111, 115, 86, 51,
    107, 140, 105, 105, 110, 142, 125, 76, 75, 69,
    65, 52, 61, 69, 55, 42, 47, 58, 37, 35,
    24, 20, 44, 22, 16, 26, 6, 3, 4, 23,
    60, 51, 30, 12, 24, 31, -9, -16, -13, 13,
    19, 9, 37, 55, 70, 36, 23, 57, 45, 33,
    50, 59, 18, 11, 62, 74, 52, 8, -3, 26,
    51, 48, -5, -9, 12, -7, -12, -5, 28, 41,
    -2, -30, -13, 31, 33, -12, -22, -8, -15, -17,
    2, -6, -25, -27, -24, -8, 4, -9, -52, -47,
    -9, -32, -45, -5, 41, 15, -32, -14, 2, -1,
    -10, -30, -32, -25, -21, -17, -14, 8, -4, -13,
    34, 18, -36, -38, -18, -19, -28, -17, -14, -16,
    -2, -20, -27, 12, 11, -17, -33, -12, -22, -64,
    -42, -26, -23, -22, -37, -51, -53, -30, -18, -48,
    -69, -38, -54, -96, -72, -49, -50, -57, -41, -22,
    -43, -64, -54, -23, -49, -69, -41, -44, -42, -49,
    -40, -26, -54, -50, -38, -49, -70, -94, -89, -69,
    -56, -65, -71, -47, -39, -49, -79, -91, -56, -46,
    -62, -86, -64, -32, -47, -50, -71, -77, -65, -68,
    -52, -51, -61, -67, -61, -81, -93, -52, -59, -62,
    -51, -75, -76, -50, -32, -54, -68, -70, -43, 1,
    -42, -92, -80, -41, -38, -79, -69, -49, -82, -122,
    -93, -21, -24, -61, -70, -73, -62, -74, -69, -43,
    -25, -15, -43, -23, -26, -69, -44, -12, 1, -51,
    -78, -13, 3, -53, -105, -72, -24, -62, -66, -31,
    -40, -65, -86, -64, -44, -55, -63, -61, -37, -41,
};

/* Golden audio ops mfcc output for the above wav. */
const std::vector<float> testWavMfcc {
    -22.67135, -0.61615, 2.07233, 0.58137, 1.01655, 0.85816, 0.46039, 0.03393, 1.16511, 0.0072,
};

arm::app::audio::MicroNetKwsMFCC GetMFCCInstance() {
    const int sampFreq = arm::app::audio::MicroNetKwsMFCC::ms_defaultSamplingFreq;
    const int frameLenMs = 40;
    const int frameLenSamples = sampFreq * frameLenMs * 0.001;
    const int numMfccFeats = 10;

   return arm::app::audio::MicroNetKwsMFCC(numMfccFeats, frameLenSamples);
}

template <class T>
void TestQuantisedMFCC() {
    const float quantScale = 1.1088106632232666;
    const int quantOffset = 95;
    std::vector<T> mfccOutput = GetMFCCInstance().MfccComputeQuant<T>(testWav, quantScale, quantOffset);

    const long min_val = std::numeric_limits<T>::min();
    const long max_val = std::numeric_limits<T>::max();

    for (size_t i = 0; i < testWavMfcc.size(); ++i){
        long TestWavMfcc = (std::lround((testWavMfcc[i] / quantScale) + quantOffset));
        T quantizedTestWavMfcc = static_cast<T>(std::max(min_val, std::min(TestWavMfcc, max_val)));

        REQUIRE(quantizedTestWavMfcc  == Approx(mfccOutput[i]).margin(0));
    }
}
template void TestQuantisedMFCC<int8_t>();
template void TestQuantisedMFCC<uint8_t>();
template void TestQuantisedMFCC<int16_t>();


TEST_CASE("MFCC calculation test") {
    SECTION("FP32")
    {
        auto mfccOutput = GetMFCCInstance().MfccCompute(testWav);
        REQUIRE_THAT( mfccOutput, Catch::Approx(testWavMfcc).margin(0.0001) );
    }

    SECTION("int8_t")
    {
        TestQuantisedMFCC<int8_t>();
    }

    SECTION("uint8_t")
    {
        TestQuantisedMFCC<uint8_t>();
    }

    SECTION("MFCC quant calculation test - int16_t")
    {
        TestQuantisedMFCC<int16_t>();
    }
}