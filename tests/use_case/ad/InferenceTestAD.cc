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

#include <catch.hpp>
#include <random>

#include "AdModel.hpp"
#include "TestData_ad.hpp"
#include "log_macros.h"
#include "TensorFlowLiteMicro.hpp"
#include "BufAttributes.hpp"

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

bool RunInference(arm::app::Model& model, const int8_t vec[])
{
    TfLiteTensor *inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    const size_t copySz = inputTensor->bytes < AD_IN_FEATURE_VEC_DATA_SIZE ? inputTensor->bytes : AD_IN_FEATURE_VEC_DATA_SIZE;

    memcpy(inputTensor->data.data, vec, copySz);

    return model.RunInference();
}

bool RunInferenceRandom(arm::app::Model& model)
{
    TfLiteTensor *inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    std::random_device rndDevice;
    std::mt19937 mersenneGen{rndDevice()};
    std::uniform_int_distribution<short> dist{-128, 127};

    auto gen = [&dist, &mersenneGen]() {
        return dist(mersenneGen);
    };

    std::vector<int8_t> randomInput(inputTensor->bytes);
    std::generate(std::begin(randomInput), std::end(randomInput), gen);

    REQUIRE(RunInference(model, randomInput.data()));
    return true;
}

template <typename T>
void TestInference(const T *input_goldenFV, const T *output_goldenFV, arm::app::Model& model)
{
    REQUIRE(RunInference(model, static_cast<const T*>(input_goldenFV)));

    TfLiteTensor *outputTensor = model.GetOutputTensor(0);

    REQUIRE(outputTensor);
    REQUIRE(outputTensor->bytes == OFM_0_DATA_SIZE);
    auto tensorData = tflite::GetTensorData<T>(outputTensor);
    REQUIRE(tensorData);

    for (size_t i = 0; i < outputTensor->bytes; i++)
    {
        REQUIRE(static_cast<int>(tensorData[i]) == static_cast<int>(((T)output_goldenFV[i])));
    }
}

TEST_CASE("Running random inference with TensorFlow Lite Micro and AdModel Int8", "[AD]")
{
    arm::app::AdModel model{};

    REQUIRE_FALSE(model.IsInited());
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::ad::GetModelPointer(),
                       arm::app::ad::GetModelLen()));
    REQUIRE(model.IsInited());

    REQUIRE(RunInferenceRandom(model));
}

TEST_CASE("Running golden vector inference with TensorFlow Lite Micro and AdModel Int8", "[AD]")
{
    REQUIRE(NUMBER_OF_IFM_FILES == NUMBER_OF_IFM_FILES);
    for (uint32_t i = 0 ; i < NUMBER_OF_IFM_FILES; ++i) {
        auto input_goldenFV = get_ifm_data_array(i);;
        auto output_goldenFV = get_ofm_data_array(i);

        DYNAMIC_SECTION("Executing inference with re-init")
        {
            arm::app::AdModel model{};

            REQUIRE_FALSE(model.IsInited());
            REQUIRE(model.Init(arm::app::tensorArena,
                               sizeof(arm::app::tensorArena),
                               arm::app::ad::GetModelPointer(),
                               arm::app::ad::GetModelLen()));
            REQUIRE(model.IsInited());

            TestInference<int8_t>(input_goldenFV, output_goldenFV, model);

        }
    }
}
