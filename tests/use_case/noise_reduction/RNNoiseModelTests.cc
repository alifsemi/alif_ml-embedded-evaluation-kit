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
#include "TensorFlowLiteMicro.hpp"
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

bool RunInference(arm::app::Model& model, std::vector<int8_t> vec,
                    const size_t sizeRequired, const size_t dataInputIndex)
{
    TfLiteTensor* inputTensor = model.GetInputTensor(dataInputIndex);
    REQUIRE(inputTensor);
    size_t copySz = inputTensor->bytes < sizeRequired ? inputTensor->bytes : sizeRequired;
    const int8_t* vecData = vec.data();
    memcpy(inputTensor->data.data, vecData, copySz);
    return model.RunInference();
}

void genRandom(size_t bytes, std::vector<int8_t>& randomAudio)
{
    randomAudio.resize(bytes);
    std::random_device rndDevice;
    std::mt19937 mersenneGen{rndDevice()};
    std::uniform_int_distribution<short> dist {-128, 127};
    auto gen = [&dist, &mersenneGen](){
        return dist(mersenneGen);
    };
    std::generate(std::begin(randomAudio), std::end(randomAudio), gen);
}

bool RunInferenceRandom(arm::app::Model& model, const size_t dataInputIndex)
{
    std::array<size_t, 4> inputSizes = {IFM_0_DATA_SIZE, IFM_1_DATA_SIZE, IFM_2_DATA_SIZE, IFM_3_DATA_SIZE};
    std::vector<int8_t> randomAudio;
    TfLiteTensor* inputTensor = model.GetInputTensor(dataInputIndex);
    REQUIRE(inputTensor);
    genRandom(inputTensor->bytes, randomAudio);

    REQUIRE(RunInference(model, randomAudio, inputSizes[dataInputIndex], dataInputIndex));
    return true;
}

TEST_CASE("Running random inference with TensorFlow Lite Micro and RNNoiseModel Int8", "[RNNoise]")
{
    arm::app::RNNoiseModel model{};

    REQUIRE_FALSE(model.IsInited());
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::rnn::GetModelPointer(),
                       arm::app::rnn::GetModelLen()));
    REQUIRE(model.IsInited());

    model.ResetGruState();

    for (int i = 1; i < 4; i++ ) {
        TfLiteTensor* inputGruStateTensor = model.GetInputTensor(i);
        auto* inputGruState = tflite::GetTensorData<int8_t>(inputGruStateTensor);
        for (size_t tIndex = 0;  tIndex < inputGruStateTensor->bytes; tIndex++) {
            REQUIRE(inputGruState[tIndex] == arm::app::GetTensorQuantParams(inputGruStateTensor).offset);
        }
    }

    REQUIRE(RunInferenceRandom(model, 0));
}

class TestRNNoiseModel : public arm::app::RNNoiseModel
{
public:
    bool CopyGruStatesTest() {
        return RNNoiseModel::CopyGruStates();
    }

    std::vector<std::pair<size_t, size_t>> GetStateMap() {
        return  m_gruStateMap;
    }

};

template <class T>
void printArray(size_t dataSz, T data){
    char strhex[8];
    std::string strdump;

    for (size_t i = 0; i < dataSz; ++i) {
        if (0 == i % 8) {
            printf("%s\n\t", strdump.c_str());
            strdump.clear();
        }
        snprintf(strhex, sizeof(strhex) - 1,
                 "0x%02x, ", data[i]);
        strdump += std::string(strhex);
    }

    if (!strdump.empty()) {
        printf("%s\n", strdump.c_str());
    }
}

/* This is true for gcc x86 platform, not guaranteed for other compilers and platforms. */
TEST_CASE("Test initial GRU out state is 0", "[RNNoise]")
{
    TestRNNoiseModel model{};
    model.Init(arm::app::tensorArena,
               sizeof(arm::app::tensorArena),
               arm::app::rnn::GetModelPointer(),
               arm::app::rnn::GetModelLen());

    auto map = model.GetStateMap();

    for(auto& mapping: map) {
        TfLiteTensor* gruOut = model.GetOutputTensor(mapping.first);
        auto* outGruState = tflite::GetTensorData<uint8_t>(gruOut);

        printf("gru out state:");
        printArray(gruOut->bytes, outGruState);

        for (size_t tIndex = 0;  tIndex < gruOut->bytes; tIndex++) {
            REQUIRE(outGruState[tIndex] == 0);
        }
    }

}

TEST_CASE("Test GRU state copy", "[RNNoise]")
{
    TestRNNoiseModel model{};
    model.Init(arm::app::tensorArena,
               sizeof(arm::app::tensorArena),
               arm::app::rnn::GetModelPointer(),
               arm::app::rnn::GetModelLen());
    REQUIRE(RunInferenceRandom(model, 0));

    auto map = model.GetStateMap();

    std::vector<std::vector<uint8_t>> oldStates;
    for(auto& mapping: map) {

        TfLiteTensor* gruOut = model.GetOutputTensor(mapping.first);
        auto* outGruState = tflite::GetTensorData<uint8_t>(gruOut);
        /* Save old output state. */
        std::vector<uint8_t> oldState(gruOut->bytes);
        memcpy(oldState.data(), outGruState, gruOut->bytes);
        oldStates.push_back(oldState);
    }

    model.CopyGruStatesTest();
    auto statesIter = oldStates.begin();
    for(auto& mapping: map) {
        TfLiteTensor* gruInput = model.GetInputTensor(mapping.second);
        auto* inGruState = tflite::GetTensorData<uint8_t>(gruInput);
        for (size_t tIndex = 0;  tIndex < gruInput->bytes; tIndex++) {
            REQUIRE((*statesIter)[tIndex] == inGruState[tIndex]);
        }
        statesIter++;
    }

}
