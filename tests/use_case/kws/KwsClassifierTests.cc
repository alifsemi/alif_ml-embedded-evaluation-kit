/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "KwsClassifier.hpp"

#include <catch.hpp>

TEST_CASE("Test invalid classifier")
{
    TfLiteTensor* outputTens = nullptr;
    std::vector<arm::app::ClassificationResult> resultVec;
    arm::app::KwsClassifier classifier;
    std::vector<std::vector<float>> resultHistory;
    REQUIRE(!classifier.GetClassificationResults(outputTens, resultVec, {}, 5, true, resultHistory));
}

TEST_CASE("Test valid classifier, average=0 should be same as 1)")
{
    int dimArray[] = {1, 5};
    std::vector<std::string> labels(5);
    std::vector<uint8_t> outputVec = {0, 1, 2, 3, 4};
    std::vector<std::vector<float>> resultHistory = {};
    TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(
            outputVec.data(), dims, 1, 0);
    TfLiteTensor* outputTensor = &tfTensor;
    std::vector<arm::app::ClassificationResult> resultVec;
    arm::app::KwsClassifier classifier;

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1, false, resultHistory));
    REQUIRE(resultVec[0].m_labelIdx == 4);
    REQUIRE(resultVec[0].m_normalisedVal == 4);

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1, false, resultHistory));
    REQUIRE(resultVec[0].m_labelIdx == 4);
    REQUIRE(resultVec[0].m_normalisedVal == 4);

    std::vector<std::vector<float>> expectedHistory = {};
    REQUIRE(resultHistory == expectedHistory);
}

TEST_CASE("Test valid classifier UINT8, average=1, softmax=false")
{
    int dimArray[] = {1, 5};
    std::vector<std::string> labels(5);
    std::vector<uint8_t> outputVec = {0, 1, 2, 3, 4};
    std::vector<std::vector<float>> resultHistory = {{0, 0, 0, 0, 0}};
    TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(
            outputVec.data(), dims, 1, 0);
    TfLiteTensor* outputTensor = &tfTensor;
    std::vector<arm::app::ClassificationResult> resultVec;
    arm::app::KwsClassifier classifier;

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1, false, resultHistory));
    REQUIRE(resultVec[0].m_labelIdx == 4);
    REQUIRE(resultVec[0].m_normalisedVal == 4);

    /* We do not update history if not >1 in size. */
    std::vector<std::vector<float>> expectedHistory = {{0, 0, 0, 0, 0}};
    REQUIRE(resultHistory == expectedHistory);
}

TEST_CASE("Test valid classifier UINT8, average=2")
{
    int dimArray[] = {1, 5};
    std::vector<std::string> labels(5);
    std::vector<uint8_t> outputVec = {0, 1, 2, 3, 4};
    std::vector<std::vector<float>> resultHistory = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
    TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(
            outputVec.data(), dims, 1, 0);
    TfLiteTensor* outputTensor = &tfTensor;
    std::vector<arm::app::ClassificationResult> resultVec;
    arm::app::KwsClassifier classifier;

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1, false, resultHistory));
    REQUIRE(resultVec[0].m_labelIdx == 4);
    REQUIRE(resultVec[0].m_normalisedVal == 2);

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1, false, resultHistory));
    REQUIRE(resultVec[0].m_labelIdx == 4);
    REQUIRE(resultVec[0].m_normalisedVal == 4);

    std::vector<std::vector<float>> expectedHistory = {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}};
    REQUIRE(resultHistory == expectedHistory);
}

TEST_CASE("Test valid classifier int8, average=0")
{
    int dimArray[] = {1, 5};
    std::vector<std::string> labels(5);
    std::vector<int8_t> outputVec = {-2, -1, 0, 2, 1};
    std::vector<std::vector<float>> resultHistory = {};
    TfLiteIntArray* dims= tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(
            outputVec.data(), dims, 1, 0);
    TfLiteTensor* outputTensor = &tfTensor;
    std::vector<arm::app::ClassificationResult> resultVec;
    arm::app::KwsClassifier classifier;

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1, false, resultHistory));
    REQUIRE(resultVec[0].m_labelIdx == 3);
    REQUIRE(resultVec[0].m_normalisedVal == 2);

    REQUIRE(classifier.GetClassificationResults(outputTensor, resultVec, labels, 1, false, resultHistory));
    REQUIRE(resultVec[0].m_labelIdx == 3);
    REQUIRE(resultVec[0].m_normalisedVal == 2);

    std::vector<std::vector<float>> expectedHistory = {};
    REQUIRE(resultHistory == expectedHistory);
}