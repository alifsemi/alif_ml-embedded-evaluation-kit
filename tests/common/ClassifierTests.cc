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
#include "Classifier.hpp"

#include <catch.hpp>

TEST_CASE("Common classifier")
{
    SECTION("Test invalid classifier")
    {
        TfLiteTensor* outputTens = nullptr;
        std::vector <arm::app::ClassificationResult> resultVec;
        arm::app::Classifier classifier;
        REQUIRE(!classifier.GetClassificationResults(outputTens, resultVec, {}, 5));
    }

    SECTION("Test valid classifier UINT8")
    {
        const int dimArray[] = {1, 1001};
        std::vector <std::string> labels(1001);
        std::vector <uint8_t> outputVec(1001);
        TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
        TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(
                outputVec.data(), dims, 1, 0, "test");
        TfLiteTensor* outputTensor = &tfTensor;
        std::vector <arm::app::ClassificationResult> resultVec;
        arm::app::Classifier classifier;
        REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 5));
        REQUIRE(5 == resultVec.size());
    }

    SECTION("Get classification results")
    {
        const int dimArray[] = {1, 1001};
        std::vector <std::string> labels(1001);
        std::vector<uint8_t> outputVec(1001, static_cast<uint8_t>(5));
        TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
        TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(
                outputVec.data(), dims, 1, 0, "test");
        TfLiteTensor* outputTensor = &tfTensor;

        std::vector <arm::app::ClassificationResult> resultVec;

        /* Set the top five results. */
        std::vector<std::pair<uint32_t, uint8_t>> selectedResults {
                {0, 8}, {20, 7}, {10, 7}, {15, 9}, {1000, 10}};

        for (size_t i = 0; i < selectedResults.size(); ++i) {
            outputVec[selectedResults[i].first] = selectedResults[i].second;
        }

        arm::app::Classifier classifier;
        REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 5));
        REQUIRE(5 == resultVec.size());

        REQUIRE(resultVec[0].m_labelIdx == 1000);
        REQUIRE(resultVec[1].m_labelIdx == 15);
        REQUIRE(resultVec[2].m_labelIdx == 0);
        REQUIRE(resultVec[3].m_labelIdx == 20);
        REQUIRE(resultVec[4].m_labelIdx == 10);
    }
}
