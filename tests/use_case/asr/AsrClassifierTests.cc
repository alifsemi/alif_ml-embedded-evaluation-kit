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
#include "AsrClassifier.hpp"
#include "Wav2LetterModel.hpp"

#include <catch.hpp>

TEST_CASE("Test invalid classifier")
{
    TfLiteTensor* outputTens = nullptr;
    std::vector <arm::app::ClassificationResult> resultVec;
    arm::app::AsrClassifier classifier;

    REQUIRE(!classifier.GetClassificationResults(outputTens, resultVec, {}, 1));
}


TEST_CASE("Test valid classifier UINT8") {
    int dimArray[] = {4, 1, 1, 246, 29};
    std::vector <std::string> labels(29);
    std::vector <uint8_t> outputVec(7134);
    TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(
                                outputVec.data(), dims, 1, 0);
    TfLiteTensor* outputTensor = &tfTensor;
    std::vector <arm::app::ClassificationResult> resultVec;
    arm::app::AsrClassifier classifier;

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1));
    REQUIRE(246 == resultVec.size());
}


TEST_CASE("Get classification results") {
    int dimArray[] = {4, 1, 1, 10, 15};
    std::vector <std::string> labels(15);
    std::vector<uint8_t> outputVec(150, static_cast<uint8_t>(1));
    TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(
                                outputVec.data(), dims, 1, 0);
    TfLiteTensor* outputTensor = &tfTensor;

    std::vector <arm::app::ClassificationResult> resultVec(10);

    /* set the top five results: */
    std::vector<std::pair<uint32_t, std::pair<uint32_t, uint8_t>>> selectedResults {
        {0, {3, 23}},
        {0, {9, 15}},
        {1, {5, 24}},
        {1, {7, 4}},
        {2, {9, 5}},
        {3, {8, 6}},
        {4, {13, 10}},
        {4, {6, 18}},
        {5, {3, 15}},
        {5, {4, 115}},
        {6, {6, 25}},
        {7, {1, 7}},
        {8, {11, 9}},
        {9, {1, 10}}
    };

    const uint32_t nCols = outputTensor->dims->data[arm::app::Wav2LetterModel::ms_outputColsIdx];
    for (size_t i = 0; i < selectedResults.size(); ++i) {
        uint32_t rIndex = selectedResults[i].first;
        uint32_t cIndex = selectedResults[i].second.first;
        uint8_t   value = selectedResults[i].second.second;
        outputVec[rIndex * nCols + cIndex] = value;
    }

    arm::app::AsrClassifier classifier;

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1));
    REQUIRE(resultVec[0].m_labelIdx == 3);
    REQUIRE(resultVec[1].m_labelIdx == 5);
    REQUIRE(resultVec[2].m_labelIdx == 9);
    REQUIRE(resultVec[3].m_labelIdx == 8);
    REQUIRE(resultVec[4].m_labelIdx == 6);
    REQUIRE(resultVec[5].m_labelIdx == 4);
    REQUIRE(resultVec[6].m_labelIdx == 6);
    REQUIRE(resultVec[7].m_labelIdx == 1);
    REQUIRE(resultVec[8].m_labelIdx == 11);
    REQUIRE(resultVec[9].m_labelIdx == 1);
}
