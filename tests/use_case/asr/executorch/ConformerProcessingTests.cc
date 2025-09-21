/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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

#include <catch.hpp>
#include <cstdio>
#include <filesystem>

#include "ConformerMelSpecFixtures.hpp"
#include "ConformerProcessing.hpp"
#include "EtTensor.hpp"
#include "executorch/runtime/core/exec_aten/exec_aten.h"
#include "executorch/runtime/core/exec_aten/testing_util/tensor_factory.h"

std::shared_ptr<arm::app::fwk::et::EtTensor> CreateTestTensor(
    executorch::runtime::testing::TensorFactory<executorch::aten::ScalarType::Float>* tensorFactory,
    const std::vector<int32_t>& shape)
{
    auto tensor = tensorFactory->zeros(shape);
    return std::make_shared<arm::app::fwk::et::EtTensor>(arm::app::fwk::et::EtTensor(tensor));
}

TEST_CASE("Conformer processing")
{
    auto tensorFactory =
        executorch::runtime::testing::TensorFactory<executorch::aten::ScalarType::Float>();
    auto chunkSize  = 1500;
    auto nMels      = 80;
    auto nFfts      = 512;
    auto windowSize = 400;
    auto hopSize    = 160;

    SECTION("Mel spectrogram generation")
    {
        auto inputTensorMelSpec   = CreateTestTensor(&tensorFactory, {1, chunkSize, nMels});
        auto inputTensorChunkSize = CreateTestTensor(&tensorFactory, {1});

        auto conformerPreProcess = arm::app::ConformerPreProcess<int16_t>(
            inputTensorMelSpec, inputTensorChunkSize, nFfts, hopSize, chunkSize);

        auto outputSize    = ((testDataWav.size() - windowSize) / hopSize) * nMels;
        auto actualMelSpec = std::vector<float>(outputSize);

        conformerPreProcess.GenerateMelSpectrogram(
            testDataWav.data(), testDataWav.size(), actualMelSpec.data(), outputSize);

        for (size_t i = 0; i < actualMelSpec.size(); ++i) {
            CHECK(actualMelSpec[i] == Approx(testDataMel[i]).epsilon(0.1).margin(1.0));
        }
    }

    SECTION("Populated input tensors with padded mel spectrogram and audio chunk size")
    {
        auto inputTensorMelSpec   = CreateTestTensor(&tensorFactory, {1, chunkSize, nMels});
        auto inputTensorChunkSize = CreateTestTensor(&tensorFactory, {1});

        auto conformerPreProcess = arm::app::ConformerPreProcess<int16_t>(
            inputTensorMelSpec, inputTensorChunkSize, nFfts, hopSize, chunkSize);
        conformerPreProcess.DoPreProcess(testDataWav.data(), testDataWav.size());

        auto inputTensorMelSpecData = static_cast<float*>(inputTensorMelSpec->GetData());
        std::vector actualMelSpec(inputTensorMelSpecData,
                                  inputTensorMelSpecData + inputTensorMelSpec->GetNumElements());
        auto expectedMelSpec = std::vector<float>(inputTensorMelSpec->GetNumElements());
        std::copy(
            testDataMel.begin(), testDataMel.end(), expectedMelSpec.data());
        std::fill_n(expectedMelSpec.begin() + testDataMel.size(),
                    expectedMelSpec.size() - testDataMel.size(),
                    -20.f);
        for (size_t i = 0; i < actualMelSpec.size(); ++i) {
            CHECK(actualMelSpec[i] == Approx(expectedMelSpec[i]).epsilon(0.1).margin(1.0));
        }

        float actualChunkSize = *static_cast<float*>(inputTensorChunkSize->GetData());
        REQUIRE(actualChunkSize == static_cast<float>(chunkSize));
    }
}
