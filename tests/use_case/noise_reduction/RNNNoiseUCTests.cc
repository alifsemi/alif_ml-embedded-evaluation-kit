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
#include "RNNoiseModel.hpp"
#include "UseCaseHandler.hpp"
#include "InputFiles.hpp"
#include "RNNUCTestCaseData.hpp"
#include "UseCaseCommonUtils.hpp"

#include <catch.hpp>
#include <hal.h>
#include <Profiler.hpp>

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace rnn {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace rnn */
} /* namespace app */
} /* namespace arm */

#define PLATFORM    hal_platform_init();

#define CONTEXT \
arm::app::ApplicationContext caseContext; \
arm::app::Profiler profiler{"noise_reduction"}; \
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
    size_t sizeToWrite = 0;
    size_t lastTensorSize = model.GetOutputTensor(numOutputs - 1)->bytes;

    for (size_t i = 0; i < numOutputs; ++i) {
        TfLiteTensor* tensor = model.GetOutputTensor(i);
        auto* tData = tflite::GetTensorData<uint8_t>(tensor);

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
            auto* tData = tflite::GetTensorData<uint8_t>(tensor);

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

    caseContext.Set<uint32_t>("clipIndex", 0);
    caseContext.Set<uint32_t>("numInputFeatures", arm::app::rnn::g_NumInputFeatures);
    caseContext.Set<uint32_t>("frameLength", arm::app::rnn::g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", arm::app::rnn::g_FrameStride);

    /* Load the model. */
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::rnn::GetModelPointer(),
                       arm::app::rnn::GetModelLen()));

    REQUIRE(arm::app::NoiseReductionHandler(caseContext, true));
}

std::function<uint32_t(const uint32_t)> get_golden_input_p232_208_array_size(const uint32_t numberOfFeatures) {

    return [numberOfFeatures](const uint32_t) ->  uint32_t{
        return numberOfFeatures;
    };
}

const char* get_test_filename(const uint32_t idx) {
    auto name = get_filename(idx);
    REQUIRE(std::string("p232_208.wav") == name);
    return "p232_208.wav";
}

void testInfByIndex(std::vector<uint32_t>& numberOfInferences) {
    PLATFORM

    arm::app::RNNoiseModel model;

    CONTEXT

    caseContext.Set<std::function<const int16_t*(const uint32_t)>>("features", get_audio_array);
    caseContext.Set<std::function<const char* (const uint32_t)>>("featureFileNames", get_test_filename);
    caseContext.Set<uint32_t>("frameLength", arm::app::rnn::g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", arm::app::rnn::g_FrameStride);
    caseContext.Set<uint32_t>("numInputFeatures", arm::app::rnn::g_NumInputFeatures);
    /* Load the model. */
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::rnn::GetModelPointer(),
                       arm::app::rnn::GetModelLen()));

    size_t oneInferenceOutSizeBytes = arm::app::rnn::g_FrameLength * sizeof(int16_t);

    auto infIndex = 0;
    for (auto numInf: numberOfInferences) {
        DYNAMIC_SECTION("Number of features: "<< numInf) {
            caseContext.Set<uint32_t>("clipIndex", 1);  /* Only getting p232_208.wav for tests. */
            uint32_t audioSizeInput = numInf * arm::app::rnn::g_FrameLength;
            caseContext.Set<std::function<uint32_t(const uint32_t)>>("featureSizes",
                                                                     get_golden_input_p232_208_array_size(audioSizeInput));

            size_t headerNumBytes = 4 + 12 + 4;  /* Filename length, filename (12 for p232_208.wav), dump size. */
            size_t footerNumBytes = 4;  /* Eof value. */
            size_t memDumpMaxLenBytes = headerNumBytes + footerNumBytes + oneInferenceOutSizeBytes * numInf;

            std::vector<uint8_t > memDump(memDumpMaxLenBytes);
            size_t undefMemDumpBytesWritten = 0;
            caseContext.Set<size_t>("MEM_DUMP_LEN", memDumpMaxLenBytes);
            caseContext.Set<uint8_t*>("MEM_DUMP_BASE_ADDR", memDump.data());
            caseContext.Set<size_t*>("MEM_DUMP_BYTE_WRITTEN", &undefMemDumpBytesWritten);

            /* Inference. */
            REQUIRE(arm::app::NoiseReductionHandler(caseContext, false));

            /* The expected output after post-processing. */
            std::vector<int16_t> golden(&ofms[infIndex][0],
                                        &ofms[infIndex][0] + arm::app::rnn::g_FrameLength);

            size_t startOfLastInfOut = undefMemDumpBytesWritten - oneInferenceOutSizeBytes;

            /* The actual result from the usecase handler. */
            std::vector<int16_t> runtime(arm::app::rnn::g_FrameLength);
            std::memcpy(runtime.data(), &memDump[startOfLastInfOut], oneInferenceOutSizeBytes);

            /* Margin of 43 is 0.07% error. */
            REQUIRE_THAT(golden, Catch::Matchers::Approx(runtime).margin(43));
        }
        ++infIndex;
    }
}

TEST_CASE("Inference by index - one inference", "[RNNoise]")
{
    auto totalAudioSize = get_audio_array_size(1);
    REQUIRE(64757 == totalAudioSize);  /* Checking that the input file is as expected and has not changed. */

    /* Run 1 inference */
    std::vector<uint32_t> numberOfInferences = {1};
    testInfByIndex(numberOfInferences);
}

TEST_CASE("Inference by index - several inferences", "[RNNoise]")
{
    auto totalAudioSize = get_audio_array_size(1);
    REQUIRE(64757 == totalAudioSize);  /* Checking that the input file is as expected and has not changed. */

    /* 3 different inference amounts: 1, 2 and all inferences required to cover total feature set */
    uint32_t totalInferences                 = totalAudioSize / arm::app::rnn::g_FrameLength;
    std::vector<uint32_t> numberOfInferences = {1, 2, totalInferences};
    testInfByIndex(numberOfInferences);
}
