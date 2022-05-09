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
#include "TensorFlowLiteMicro.hpp"
#include "RNNoiseModel.hpp"
#include "TestData_noise_reduction.hpp"
#include "BufAttributes.hpp"

#include <catch.hpp>
#include <random>

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace rnn {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace rnn */
} /* namespace app */
} /* namespace arm */

namespace test {
namespace noise_reduction {

    bool RunInference(arm::app::Model& model, const std::vector<std::vector<int8_t>> inData)
    {
        for (size_t i = 0; i < model.GetNumInputs(); ++i) {
            TfLiteTensor* inputTensor = model.GetInputTensor(i);
            REQUIRE(inputTensor);
            memcpy(inputTensor->data.data, inData[i].data(), inData[i].size());
        }

        return model.RunInference();
    }

    bool RunInferenceRandom(arm::app::Model& model)
    {
        std::random_device rndDevice;
        std::mt19937 mersenneGen{rndDevice()};
        std::uniform_int_distribution<short> dist {-128, 127};

        auto gen = [&dist, &mersenneGen](){
            return dist(mersenneGen);
        };

        std::vector<std::vector<int8_t>> randomInput{NUMBER_OF_IFM_FILES};
        for (size_t i = 0; i < model.GetNumInputs(); ++i) {
            TfLiteTensor *inputTensor = model.GetInputTensor(i);
            REQUIRE(inputTensor);
            randomInput[i].resize(inputTensor->bytes);
            std::generate(std::begin(randomInput[i]), std::end(randomInput[i]), gen);
        }

        REQUIRE(RunInference(model, randomInput));
        return true;
    }

    TEST_CASE("Running random inference with Tflu and RNNoise Int8", "[RNNoise]")
    {
        arm::app::RNNoiseModel model{};

        REQUIRE_FALSE(model.IsInited());
        REQUIRE(model.Init(arm::app::tensorArena,
                           sizeof(arm::app::tensorArena),
                           arm::app::rnn::GetModelPointer(),
                           arm::app::rnn::GetModelLen()));
        REQUIRE(model.IsInited());

        REQUIRE(RunInferenceRandom(model));
    }

    template<typename T>
    void TestInference(const std::vector<std::vector<T>> input_goldenFV, const std::vector<std::vector<T>> output_goldenFV, arm::app::Model& model)
    {
        for (size_t i = 0; i < model.GetNumInputs(); ++i) {
            TfLiteTensor* inputTensor = model.GetInputTensor(i);
            REQUIRE(inputTensor);
        }

        REQUIRE(RunInference(model, input_goldenFV));

        for (size_t i = 0; i < model.GetNumOutputs(); ++i) {
            TfLiteTensor *outputTensor = model.GetOutputTensor(i);

            REQUIRE(outputTensor);
            auto tensorData = tflite::GetTensorData<T>(outputTensor);
            REQUIRE(tensorData);

            for (size_t j = 0; j < outputTensor->bytes; j++) {
                REQUIRE(static_cast<int>(tensorData[j]) == static_cast<int>((output_goldenFV[i][j])));
            }
        }
    }

    TEST_CASE("Running inference with Tflu and RNNoise Int8", "[RNNoise]")
    {
        std::vector<std::vector<int8_t>> goldenInputFV {NUMBER_OF_IFM_FILES};
        std::vector<std::vector<int8_t>> goldenOutputFV {NUMBER_OF_OFM_FILES};

        std::array<size_t, NUMBER_OF_IFM_FILES> inputSizes = {IFM_0_DATA_SIZE,
                                                              IFM_1_DATA_SIZE,
                                                              IFM_2_DATA_SIZE,
                                                              IFM_3_DATA_SIZE};

        std::array<size_t, NUMBER_OF_OFM_FILES> outputSizes = {OFM_0_DATA_SIZE,
                                                               OFM_1_DATA_SIZE,
                                                               OFM_2_DATA_SIZE,
                                                               OFM_3_DATA_SIZE,
                                                               OFM_4_DATA_SIZE};

        for (uint32_t i = 0 ; i < NUMBER_OF_IFM_FILES; ++i) {
            goldenInputFV[i].resize(inputSizes[i]);
            std::memcpy(goldenInputFV[i].data(), get_ifm_data_array(i), inputSizes[i]);
        }
        for (uint32_t i = 0 ; i < NUMBER_OF_OFM_FILES; ++i) {
            goldenOutputFV[i].resize(outputSizes[i]);
            std::memcpy(goldenOutputFV[i].data(), get_ofm_data_array(i), outputSizes[i]);
        }

        DYNAMIC_SECTION("Executing inference with re-init")
        {
            arm::app::RNNoiseModel model{};

            REQUIRE_FALSE(model.IsInited());
            REQUIRE(model.Init(arm::app::tensorArena,
                               sizeof(arm::app::tensorArena),
                               arm::app::rnn::GetModelPointer(),
                               arm::app::rnn::GetModelLen()));
            REQUIRE(model.IsInited());

            TestInference<int8_t>(goldenInputFV, goldenOutputFV, model);
        }
    }

} /* namespace noise_reduction */
}  /* namespace test */
