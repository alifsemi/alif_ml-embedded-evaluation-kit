/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024 Arm Limited and/or its
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
#include "BufAttributes.hpp"
#include "Profiler.hpp"
#include "RNNoiseModel.hpp"
#include "UseCaseHandler.hpp"
#include "hal.h"

#include <catch.hpp>

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace rnn {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace rnn */
} /* namespace app */
} /* namespace arm */

#define PLATFORM hal_platform_init();

#define CONTEXT                                                 \
    arm::app::ApplicationContext caseContext;                   \
    arm::app::Profiler profiler{"noise_reduction"};             \
    caseContext.Set<arm::app::Profiler&>("profiler", profiler); \
    caseContext.Set<arm::app::RNNoiseModel&>("model", model);

TEST_CASE("Verify output tensor memory dump")
{
    constexpr size_t maxMemDumpSz = 0x100000;   /* 1 MiB worth of space */
    std::vector<uint8_t> memPool(maxMemDumpSz); /* Memory pool */
    arm::app::RNNoiseModel model{};

    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::rnn::GetModelPointer(),
                       arm::app::rnn::GetModelLen()));
    REQUIRE(model.IsInited());

    /* Populate the output tensors */
    const size_t numOutputs = model.GetNumOutputs();
    size_t sizeToWrite      = 0;
    size_t lastTensorSize   = model.GetOutputTensor(numOutputs - 1)->bytes;

    for (size_t i = 0; i < numOutputs; ++i) {
        TfLiteTensor* tensor = model.GetOutputTensor(i);
        auto* tData          = tflite::GetTensorData<uint8_t>(tensor);

        if (tensor->bytes > 0) {
            memset(tData, static_cast<uint8_t>(i), tensor->bytes);
            sizeToWrite += tensor->bytes;
        }
    }

    SECTION("Positive use case")
    {
        /* Run the memory dump */
        auto bytesWritten = DumpOutputTensorsToMemory(model, memPool.data(), memPool.size());
        REQUIRE(sizeToWrite == bytesWritten);

        /* Verify the dump */
        size_t k = 0;
        for (size_t i = 0; i < numOutputs && k < memPool.size(); ++i) {
            TfLiteTensor* tensor = model.GetOutputTensor(i);
            auto* tData          = tflite::GetTensorData<uint8_t>(tensor);

            for (size_t j = 0; j < tensor->bytes && k < memPool.size(); ++j) {
                REQUIRE(tData[j] == memPool[k++]);
            }
        }
    }

    SECTION("Limited memory - skipping last tensor")
    {
        /* Run the memory dump */
        auto bytesWritten = DumpOutputTensorsToMemory(model, memPool.data(), sizeToWrite - 1);
        REQUIRE(lastTensorSize > 0);
        REQUIRE(bytesWritten == sizeToWrite - lastTensorSize);
    }

    SECTION("Zero memory")
    {
        /* Run the memory dump */
        auto bytesWritten = DumpOutputTensorsToMemory(model, memPool.data(), 0);
        REQUIRE(bytesWritten == 0);
    }
}

TEST_CASE("Inference run all clips", "[RNNoise]")
{
    PLATFORM

    arm::app::RNNoiseModel model;

    CONTEXT

    caseContext.Set<uint32_t>("numInputFeatures", arm::app::rnn::g_NumInputFeatures);
    caseContext.Set<uint32_t>("frameLength", arm::app::rnn::g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", arm::app::rnn::g_FrameStride);

    /* Load the model. */
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::rnn::GetModelPointer(),
                       arm::app::rnn::GetModelLen()));

    REQUIRE(arm::app::NoiseReductionHandler(caseContext));
}
