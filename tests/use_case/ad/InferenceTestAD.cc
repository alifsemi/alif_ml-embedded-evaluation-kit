/*
 * SPDX-FileCopyrightText: Copyright 2021, 2024-2025 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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
#include <random>

#include "mlek/fwk/tflm/AdModel.hpp"
#include "BufAttributes.hpp"
#include "mlek/fwk/tflm/TensorFlowLiteMicro.hpp"
#include "TestData_ad.hpp"
#include "mlek/log/log_macros.h"

#ifndef AD_FEATURE_VEC_DATA_SIZE
#define AD_IN_FEATURE_VEC_DATA_SIZE (1024)
#endif /* AD_FEATURE_VEC_DATA_SIZE */

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace ad {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace ad */
} /* namespace app */
} /* namespace arm */

using namespace test;

bool RunInference(arm::app::fwk::iface::Model& model, const int8_t vec[])
{
    auto inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    const size_t copySz = inputTensor->Bytes() < AD_IN_FEATURE_VEC_DATA_SIZE
                              ? inputTensor->Bytes()
                              : AD_IN_FEATURE_VEC_DATA_SIZE;

    memcpy(inputTensor->GetData(), vec, copySz);

    return model.RunInference();
}

bool RunInferenceRandom(arm::app::fwk::iface::Model& model)
{
    auto inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    std::random_device rndDevice;
    std::mt19937 mersenneGen{rndDevice()};
    std::uniform_int_distribution<short> dist{-128, 127};

    auto gen = [&dist, &mersenneGen]() { return dist(mersenneGen); };

    std::vector<int8_t> randomInput(inputTensor->Bytes());
    std::generate(std::begin(randomInput), std::end(randomInput), gen);

    REQUIRE(RunInference(model, randomInput.data()));
    return true;
}

template <typename T>
void TestInference(const T* input_goldenFV,
                   const T* output_goldenFV,
                   arm::app::fwk::iface::Model& model)
{
    REQUIRE(RunInference(model, static_cast<const T*>(input_goldenFV)));

    auto outputTensor = model.GetOutputTensor(0);

    REQUIRE(outputTensor);
    REQUIRE(outputTensor->Bytes() == OFM_0_DATA_SIZE);
    auto tensorData = outputTensor->GetData<T>();
    REQUIRE(tensorData);

    for (size_t i = 0; i < outputTensor->Bytes(); i++) {
        REQUIRE(static_cast<int>(tensorData[i]) == static_cast<int>(((T)output_goldenFV[i])));
    }
}

TEST_CASE("Running random inference with TensorFlow Lite Micro and AdModel Int8", "[AD]")
{
    arm::app::fwk::tflm::AdModel model{};
    REQUIRE_FALSE(model.IsInited());
    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::ad::GetModelPointer(),
                                                arm::app::ad::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                  sizeof(arm::app::tensorArena)};

    /* Load the model. */
    REQUIRE(model.Init(computeMem, modelMem));
    REQUIRE(model.IsInited());
    REQUIRE(RunInferenceRandom(model));
}

TEST_CASE("Running golden vector inference with TensorFlow Lite Micro and AdModel Int8", "[AD]")
{
    REQUIRE(NUMBER_OF_IFM_FILES == NUMBER_OF_IFM_FILES);
    for (uint32_t i = 0; i < NUMBER_OF_IFM_FILES; ++i) {
        auto input_goldenFV = reinterpret_cast<const int8_t*>(
                GetIfmDataArray(i));
        auto output_goldenFV = reinterpret_cast<const int8_t*>(
                GetOfmDataArray(i));

        DYNAMIC_SECTION("Executing inference with re-init")
        {
            arm::app::fwk::tflm::AdModel model{};
            REQUIRE_FALSE(model.IsInited());
            arm::app::fwk::iface::MemoryRegion modelMem{arm::app::ad::GetModelPointer(),
                                                        arm::app::ad::GetModelLen()};
            arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                          sizeof(arm::app::tensorArena)};

            /* Load the model. */
            REQUIRE(model.Init(computeMem, modelMem));
            REQUIRE(model.IsInited());

            TestInference<int8_t>(input_goldenFV, output_goldenFV, model);
        }
    }
}
