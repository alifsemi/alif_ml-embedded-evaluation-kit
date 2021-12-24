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


template<typename T>
void test_classifier_result(std::vector<std::pair<uint32_t, T>>& selectedResults, T defaultTensorValue) {
    int dimArray[] = {1, 1001};
    std::vector <std::string> labels(1001);
    std::vector<T> outputVec(1001, defaultTensorValue);
    TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(outputVec.data(), dims, 1, 0);
    TfLiteTensor* outputTensor = &tfTensor;

    std::vector <arm::app::ClassificationResult> resultVec;

    for (auto& selectedResult : selectedResults) {
        outputVec[selectedResult.first] = selectedResult.second;
    }

    arm::app::Classifier classifier;
    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 5, true));
    REQUIRE(5 == resultVec.size());

    for (size_t i = 0; i < resultVec.size(); ++i) {
        REQUIRE(resultVec[i].m_labelIdx == selectedResults[i].first);
    }
}

TEST_CASE("Common classifier")
{
    SECTION("Test invalid classifier")
    {
        TfLiteTensor* outputTens = nullptr;
        std::vector <arm::app::ClassificationResult> resultVec;
        arm::app::Classifier classifier;
        REQUIRE(!classifier.GetClassificationResults(outputTens, resultVec, {}, 5, true));
    }

    SECTION("Test classification results")
    {
        SECTION("uint8") {
            /* Set the top five results <position, score>. */
            std::vector<std::pair<uint32_t, uint8_t>> selectedResults {
                    {1000, 10}, {15, 9}, {0, 8}, {20, 7}, {10, 7} };

            test_classifier_result(selectedResults, static_cast<uint8_t>(5));
        }

        SECTION("int8") {
            /* Set the top five results <position, score>. */
            std::vector<std::pair<uint32_t, int8_t>> selectedResults {
                    {1000, 10}, {15, 9}, {0, 8}, {20, -7}, {10, -7} };

            test_classifier_result(selectedResults, static_cast<int8_t>(-100));
        }

        SECTION("float") {
            /* Set the top five results <position, score>. */
            std::vector<std::pair<uint32_t, float>> selectedResults {
                    {1000, 10.9f}, {15, 9.8f}, {0, 8.7f}, {20, -7.0f}, {10, -7.1f} };

            test_classifier_result(selectedResults, -100.0f);
        }

    }
}
