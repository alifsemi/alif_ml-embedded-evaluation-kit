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
#include "DataStructures.hpp"
#include "AsrGoldenFeatures.hpp"
#include "hal.h"
#include "TensorFlowLiteMicro.hpp"
#include "Wav2LetterPreprocess.hpp"

#include <catch.hpp>
#include <random>

class TestPreprocess : public arm::app::audio::asr::Preprocess {
public:
    TestPreprocess()
    : arm::app::audio::asr::Preprocess(0,0,0,0)
    {}

    bool ComputeDeltas(arm::app::Array2d<float>& mfcc,
                       arm::app::Array2d<float>& delta1,
                       arm::app::Array2d<float>& delta2)
    {
        return this->_ComputeDeltas(mfcc, delta1, delta2);
    }

    float GetMean(arm::app::Array2d<float>& vec)
    {
        return this->_GetMean(vec);
    }

    float GetStdDev(arm::app::Array2d<float>& vec, const float mean)
    {
       return this->_GetStdDev(vec, mean);
    }

    void NormaliseVec(arm::app::Array2d<float>& vec)
    {
        return this->_NormaliseVec(vec);
    }
};

template<class T>
void CheckOutputs(const std::vector<T> goldenOutput, std::vector<T> output)
{
    const size_t goldenSize = goldenOutput.size();
    const size_t realSize = output.size();

    REQUIRE(realSize == goldenSize);
    REQUIRE_THAT(output, Catch::Approx( goldenOutput ).margin(0.0001));
}
template void CheckOutputs<float>(const std::vector<float> goldenOutput, std::vector<float> output);

void populateBuffer(const float* input, size_t size, size_t numMfccFeats, std::vector<std::vector<float>>& buf)
{
    size_t time = 0;
    for (size_t i = 0; i < size; ++i) {
        if (i > 0 && i % numMfccFeats == 0) {
            ++time;
        }
        float featureValue = *(input + i);
        buf[i % numMfccFeats][time] = featureValue;
    }
}

void populateArray2dWithVectorOfVector(std::vector<std::vector<float>> vec, arm::app::Array2d<float>& buf)
{
    for (size_t i = 0; i < vec.size(); ++i) {
        for (size_t j = 0; j < vec[i].size(); ++j) {
            buf(i, j) = vec[i][j];
        }
    }
}

TEST_CASE("Floating point asr features calculation", "[ASR]")
{
    TestPreprocess tp;

    SECTION("First and second diff")
    {
        constexpr uint32_t numMfccFeats = 13;
        constexpr uint32_t numFeatVectors = 296;

        arm::app::Array2d<float> mfccBuf(numMfccFeats, numFeatVectors);
        arm::app::Array2d<float> delta1Buf(numMfccFeats, numFeatVectors);
        arm::app::Array2d<float> delta2Buf(numMfccFeats, numFeatVectors);

        std::vector<std::vector<float>> goldenMfccBuf(numMfccFeats, std::vector<float>(numFeatVectors));
        std::vector<std::vector<float>> goldenDelta1Buf(numMfccFeats, std::vector<float>(numFeatVectors));
        std::vector<std::vector<float>> goldenDelta2Buf(numMfccFeats, std::vector<float>(numFeatVectors));

        populateBuffer(golden_asr_mfcc, golden_asr_mfcc_len, numMfccFeats, goldenMfccBuf);
        populateBuffer(golden_diff1_features, golden_diff1_len, numMfccFeats, goldenDelta1Buf);
        populateBuffer(golden_diff2_features, golden_diff2_len, numMfccFeats, goldenDelta2Buf);

        populateArray2dWithVectorOfVector(goldenMfccBuf, mfccBuf);
        std::fill(delta1Buf.begin(), delta1Buf.end(), 0.f);
        std::fill(delta2Buf.begin(), delta2Buf.end(), 0.f);

        tp.ComputeDeltas(mfccBuf, delta1Buf, delta2Buf);

        /* First 4 and last 4 values are different because we pad AFTER diff calculated. */
        for (size_t i = 0; i < numMfccFeats; ++i) {
            const float* start_goldenDelta1Buf = goldenDelta1Buf[i].data() + 4;
            const float* start_delta1 = delta1Buf.begin() + i * delta1Buf.size(1) + 4;
            std::vector<float> goldenDataDelta1(start_goldenDelta1Buf, start_goldenDelta1Buf + numFeatVectors - 8);
            std::vector<float> tensorDataDelta1(start_delta1, start_delta1 + numFeatVectors - 8);

            CheckOutputs<float>(goldenDataDelta1,tensorDataDelta1);

            const float* start_goldenDelta2Buf = goldenDelta2Buf[i].data() + 4;
            const float* start_delta2 = delta2Buf.begin() + i * delta2Buf.size(1) + 4;
            std::vector<float> goldenDataDelta2(start_goldenDelta2Buf, start_goldenDelta2Buf + numFeatVectors - 8);
            std::vector<float> tensorDataDelta2(start_delta2, start_delta2 + numFeatVectors - 8);

            CheckOutputs<float>(goldenDataDelta2,tensorDataDelta2);
        }

    }

    SECTION("Mean")
    {
        std::vector<std::vector<float>> mean1vec{{1, 2},
                                                {-1, -2}};
        arm::app::Array2d<float> mean1(2,2); /* {{1, 2},{-1, -2}} */
        populateArray2dWithVectorOfVector(mean1vec, mean1);
        REQUIRE(0 == Approx(tp.GetMean(mean1)));

        arm::app::Array2d<float> mean2(2, 2);
        std::fill(mean2.begin(), mean2.end(), 0.f);
        REQUIRE(0 == Approx(tp.GetMean(mean2)));

        arm::app::Array2d<float> mean3(3,3);
        std::fill(mean3.begin(), mean3.end(), 1.f);
        REQUIRE(1 == Approx(tp.GetMean(mean3)));
    }

    SECTION("Std")
    {
        arm::app::Array2d<float> std1(2, 2);
        std::fill(std1.begin(), std1.end(), 0.f); /* {{0, 0}, {0, 0}} */
        REQUIRE(0 == Approx(tp.GetStdDev(std1, 0)));

        std::vector<std::vector<float>> std2vec{{1, 2, 3, 4, 5}, {6, 7, 8, 9, 0}};
        arm::app::Array2d<float> std2(2,5);
        populateArray2dWithVectorOfVector(std2vec, std2);
        const float mean = tp.GetMean(std2);
        REQUIRE(2.872281323 == Approx(tp.GetStdDev(std2, mean)));

        arm::app::Array2d<float> std3(2,2);
        std::fill(std3.begin(), std3.end(), 1.f); /* std3{{1, 1}, {1, 1}}; */
        REQUIRE(0 == Approx(tp.GetStdDev(std3, 1)));
    }

    SECTION("Norm") {
        auto checker = [&](arm::app::Array2d<float>& d, std::vector<float>& g) {
            tp.NormaliseVec(d);
            std::vector<float> d_vec(d.begin(), d.end());
            REQUIRE_THAT(g, Catch::Approx(d_vec));
        };

        std::vector<std::vector<float>> norm0vec{{1, 1}, {1, 1}};
        std::vector<float> goldenNorm0 {0, 0, 0, 0};
        arm::app::Array2d<float> norm0(2, 2);
        populateArray2dWithVectorOfVector(norm0vec, norm0);
        checker(norm0, goldenNorm0);

        std::vector<std::vector<float>> norm1vec{{1, 2, 3, 4, 5}, {6, 7, 8, 9, 0}};
        std::vector<float> goldenNorm1 {
            -1.218543592, -0.87038828, -0.522232968, -0.174077656, 0.174077656,
             0.522232968,  0.87038828,  1.218543592,  1.566698904, -1.566698904};
        arm::app::Array2d<float> norm1(2, 5);
        populateArray2dWithVectorOfVector(norm1vec, norm1);
        checker(norm1, goldenNorm1);
    }
}
