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
#include <map>

#include "ConformerLogitsFixtures.hpp"
#include "ConformerMelSpecFixtures.hpp"
#include "mlek/use_case/asr/ConformerProcessing.hpp"
#include "mlek/fwk/executorch/EtTensor.hpp"
#include "executorch/runtime/core/exec_aten/exec_aten.h"
#include "executorch/runtime/core/exec_aten/testing_util/tensor_factory.h"

std::shared_ptr<arm::app::fwk::et::EtTensor> CreateTestTensor(
    executorch::runtime::testing::TensorFactory<executorch::aten::ScalarType::Float>* tensorFactory,
    const std::vector<int32_t>& shape
)
{
    auto tensor = tensorFactory->zeros(shape);
    return std::make_shared<arm::app::fwk::et::EtTensor>(arm::app::fwk::et::EtTensor(tensor));
}

template <std::size_t DIMS>
std::shared_ptr<arm::app::fwk::et::EtTensor> CreateTestTensor(
    executorch::runtime::testing::TensorFactory<executorch::aten::ScalarType::Float>* tensorFactory,
    const std::array<size_t, DIMS>& shape,
    const float* data,
    const size_t dataSize
)
{
    const size_t shapeProduct = std::accumulate(
        shape.begin(),
        shape.end(),
        1,
        std::multiplies<size_t>()
    );
    REQUIRE(dataSize <= shapeProduct);

    auto sizes = std::vector<int32_t>(shape.begin(), shape.end());
    auto tensor = tensorFactory->zeros(sizes);
    auto etTensor =
        std::make_shared<arm::app::fwk::et::EtTensor>(arm::app::fwk::et::EtTensor(tensor));
    memcpy(etTensor->GetData(), data, dataSize * sizeof(float));
    return etTensor;
}

TEST_CASE("Conformer pre-processing")
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

TEST_CASE("Conformer post-processing")
{
    SECTION("Arg max 1D")
    {
        auto fixtures = std::map<std::vector<float>, uint32_t>{
            {{0.1, 0.2, 0.3, 0.4, 0.5, 0.4, 0.3, 0.2, 0.1}, 4},
            {{0.4, 0.5, 0.5, 0.5, 0.5, 0.5}, 1},
            {{-0.4, -0.5, -0.5, -0.5, -0.5, -0.5}, 0},
            {{-0.4, -0.2, 0.5, -0.5}, 2}
        };

        for (const auto& fixture : fixtures) {
            auto testLogits     = fixture.first;
            auto expectedArgMax = fixture.second;
            auto actualArgMax =
                arm::app::ConformerPostProcess::ArgMax1D(testLogits.data(), testLogits.size());
            REQUIRE(actualArgMax == expectedArgMax);
        }
    }

    SECTION("Arg max 2D")
    {
        auto input = anotherDoorInt8Logits;
        auto actualArgMax = arm::app::ConformerPostProcess::ArgMax2D(
            input.data(), input.size(), anotherDoorInt8LogitsShape
        );
        REQUIRE(std::equal(
            actualArgMax.begin(),
            actualArgMax.end(),
            anotherDoorInt8ArgMax.begin(),
            anotherDoorInt8ArgMax.end()
            )
        );
    }

    SECTION("Decode")
    {
        auto input = anotherDoorInt8Logits;
        std::array<size_t, 2> inputShape;
        std::copy(
            anotherDoorInt8LogitsShape.begin(),
            anotherDoorInt8LogitsShape.end(),
            inputShape.data()
        );
        std::string actualDecoded;

        arm::app::ConformerPostProcess::Decode(
            input.data(), input.size(), inputShape, sentencePieceVocab, actualDecoded
        );
        REQUIRE(actualDecoded == anotherDoorInt8Decoded);
    }

    SECTION("Decoded output from output tensors")
    {
        auto tensorFactory =
            executorch::runtime::testing::TensorFactory<executorch::aten::ScalarType::Float>();
        auto outputTensorLogits = CreateTestTensor(
            &tensorFactory,
            anotherDoorInt8LogitsShape,
            anotherDoorInt8Logits.data(),
            anotherDoorInt8Logits.size()
        );
        const auto chunkSize = static_cast<float>(anotherDoorInt8LogitsShape[0]);
        std::array<size_t, 1> shape = {1};
        auto outputTensorChunkSize = CreateTestTensor(
            &tensorFactory,
            shape,
            &chunkSize,
            1
        );
        std::string actualDecoded;
        auto conformerPostProcess = arm::app::ConformerPostProcess(
            outputTensorLogits, outputTensorChunkSize, sentencePieceVocab, actualDecoded
        );
        conformerPostProcess.DoPostProcess();
        REQUIRE(actualDecoded == anotherDoorInt8Decoded);
    }
}
