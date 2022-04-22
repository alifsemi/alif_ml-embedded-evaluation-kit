/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
#include "Wav2LetterPreprocess.hpp"

#include <limits>
#include <catch.hpp>

constexpr uint32_t numMfccFeatures = 13;
constexpr uint32_t numMfccVectors  = 10;

/* Test vector output: generated using test-asr-preprocessing.py. */
int8_t expectedResult[numMfccVectors][numMfccFeatures * 3] = {
        /* Feature vec 0. */
        {-32,   4,  -9,  -8, -10, -10, -11, -11, -11, -11, -12, -11, -11,    /* MFCCs.   */
        -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,     /* Delta 1. */
        -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10},    /* Delta 2. */
        /* Feature vec 1. */
        {-31,   4,  -9,  -8, -10, -10, -11, -11, -11, -11, -12, -11, -11,
        -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
        -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10},
        /* Feature vec 2. */
        {-31,   4,  -9,  -9, -10, -10, -11, -11, -11, -11, -12, -12, -12,
        -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
        -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10},
        /* Feature vec 3. */
        {-31,   4,  -9,  -9, -10, -10, -11, -11, -11, -11, -11, -12, -12,
        -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
        -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10},
        /* Feature vec 4 : this should have valid delta 1 and delta 2. */
        {-31,   4,  -9,  -9, -10, -10, -11, -11, -11, -11, -11, -12, -12,
        -38, -29,  -9,   1,  -2,  -7,  -8,  -8, -12, -16, -14,  -5,   5,
        -68, -50, -13,   5,   0,  -9,  -9,  -8, -13, -20, -19,  -3,  15},
        /* Feature vec 5 : this should have valid delta 1 and delta 2. */
        {-31,   4,  -9,  -8, -10, -10, -11, -11, -11, -11, -11, -12, -12,
        -62, -45, -11,   5,   0,  -8,  -9,  -8, -12, -19, -17,  -3,  13,
        -27, -22, -13,  -9, -11, -12, -12, -11, -11, -13, -13, -10,  -6},
        /* Feature vec 6. */
        {-31,   4,  -9,  -8, -10, -10, -11, -11, -11, -11, -12, -11, -11,
        -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
        -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10},
        /* Feature vec 7. */
        {-32,   4,  -9,  -8, -10, -10, -11, -11, -11, -12, -12, -11, -11,
        -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
        -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10},
        /* Feature vec 8. */
        {-32,   4,  -9,  -8, -10, -10, -11, -11, -11, -12, -12, -11, -11,
        -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
        -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10},
        /* Feature vec 9. */
        {-31,   4,  -9,  -8, -10, -10, -11, -11, -11, -11, -12, -11, -11,
        -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11, -11,
        -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10, -10}
};

void PopulateTestWavVector(std::vector<int16_t>& vec)
{
    constexpr int int16max = std::numeric_limits<int16_t>::max();
    int val = 0;
    for (size_t i = 0; i < vec.size(); ++i, ++val) {

        /* We want a differential filter response from both - order 1
         * and 2 => Don't have a linear signal here - we use a signal
         * using squares for example. Alternate sign flips might work
         * just as well and will be computationally less work! */
        int valsq = val * val;
        if (valsq > int16max) {
            val = 0;
            valsq = 0;
        }
        vec[i] = valsq;
    }
}

TEST_CASE("Preprocessing calculation INT8")
{
    /* Constants. */
    const uint32_t  mfccWindowLen      = 512;
    const uint32_t  mfccWindowStride   = 160;
    int             dimArray[]         = {3, 1, numMfccFeatures * 3, numMfccVectors};
    const float     quantScale         = 0.1410219967365265;
    const int       quantOffset        = -11;

    /* Test wav memory. */
    std::vector<int16_t> testWav((mfccWindowStride * numMfccVectors) +
                                  (mfccWindowLen - mfccWindowStride)
                                 );

    /* Populate with dummy input. */
    PopulateTestWavVector(testWav);

    /* Allocate mem for tensor. */
    std::vector<int8_t> tensorVec(dimArray[1]*dimArray[2]*dimArray[3]);

    /* Initialise dimensions and the test tensor. */
    TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor inputTensor = tflite::testing::CreateQuantizedTensor(
            tensorVec.data(), dims, quantScale, quantOffset, "preprocessedInput");

    /* Initialise pre-processing module. */
    arm::app::AsrPreProcess prep{&inputTensor,
                                 numMfccFeatures, numMfccVectors, mfccWindowLen, mfccWindowStride};

    /* Invoke pre-processing. */
    REQUIRE(prep.DoPreProcess(testWav.data(), testWav.size()));

    /* Wrap the tensor with a std::vector for ease. */
    auto* tensorData = tflite::GetTensorData<int8_t>(&inputTensor);
    std::vector <int8_t> vecResults =
            std::vector<int8_t>(tensorData, tensorData + inputTensor.bytes);

    /* Check sizes. */
    REQUIRE(vecResults.size() == sizeof(expectedResult));

    /* Check that the elements have been calculated correctly. */
    for (uint32_t j = 0; j < numMfccVectors; ++j) {
        for (uint32_t i = 0; i < numMfccFeatures * 3; ++i) {
            size_t tensorIdx = (j * numMfccFeatures * 3) + i;
            CHECK(vecResults[tensorIdx] == expectedResult[j][i]);
        }
    }
}
