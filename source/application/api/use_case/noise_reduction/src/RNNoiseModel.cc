/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "log_macros.h"

const tflite::MicroOpResolver& arm::app::RNNoiseModel::GetOpResolver()
{
    return this->m_opResolver;
}

bool arm::app::RNNoiseModel::EnlistOperations()
{
#ifndef ETHOS_U_NPU_ASSUMED
    this->m_opResolver.AddUnpack();
    this->m_opResolver.AddFullyConnected();
    this->m_opResolver.AddSplit();
    this->m_opResolver.AddSplitV();
    this->m_opResolver.AddAdd();
    this->m_opResolver.AddLogistic();
    this->m_opResolver.AddMul();
    this->m_opResolver.AddSub();
    this->m_opResolver.AddTanh();
    this->m_opResolver.AddPack();
    this->m_opResolver.AddReshape();
    this->m_opResolver.AddQuantize();
    this->m_opResolver.AddConcatenation();
    this->m_opResolver.AddRelu();
#endif

    if (kTfLiteOk == this->m_opResolver.AddEthosU()) {
        info("Added %s support to op resolver\n",
            tflite::GetString_ETHOSU());
    } else {
        printf_err("Failed to add Arm NPU support to op resolver.");
        return false;
    }
    return true;
}

bool arm::app::RNNoiseModel::RunInference()
{
    return Model::RunInference();
}

void arm::app::RNNoiseModel::ResetGruState()
{
    for (auto& stateMapping: this->m_gruStateMap) {
        TfLiteTensor* inputGruStateTensor = this->GetInputTensor(stateMapping.second);
        auto* inputGruState = tflite::GetTensorData<int8_t>(inputGruStateTensor);
        /* Initial value of states is 0, but this is affected by quantization zero point. */
        auto quantParams = arm::app::GetTensorQuantParams(inputGruStateTensor);
        memset(inputGruState, quantParams.offset, inputGruStateTensor->bytes);
    }
}

bool arm::app::RNNoiseModel::CopyGruStates()
{
    std::vector<std::pair<size_t, std::vector<int8_t>>> tempOutGruStates;
    /* Saving output states before copying them to input states to avoid output states modification in the tensor.
     * tflu shares input and output tensors memory, thus writing to input tensor can change output tensor values. */
    for (auto& stateMapping: this->m_gruStateMap) {
        TfLiteTensor* outputGruStateTensor = this->GetOutputTensor(stateMapping.first);
        std::vector<int8_t> tempOutGruState(outputGruStateTensor->bytes);
        auto* outGruState = tflite::GetTensorData<int8_t>(outputGruStateTensor);
        memcpy(tempOutGruState.data(), outGruState, outputGruStateTensor->bytes);
        /* Index of the input tensor and the data to copy. */
        tempOutGruStates.emplace_back(stateMapping.second, std::move(tempOutGruState));
    }
    /* Updating input GRU states with saved GRU output states. */
    for (auto& stateMapping: tempOutGruStates) {
        auto outputGruStateTensorData = stateMapping.second;
        TfLiteTensor* inputGruStateTensor = this->GetInputTensor(stateMapping.first);
        if (outputGruStateTensorData.size() != inputGruStateTensor->bytes) {
            printf_err("Unexpected number of bytes for GRU state mapping. Input = %zuz, output = %zuz.\n",
                       inputGruStateTensor->bytes,
                       outputGruStateTensorData.size());
            return false;
        }
        auto* inputGruState = tflite::GetTensorData<int8_t>(inputGruStateTensor);
        auto* outGruState = outputGruStateTensorData.data();
        memcpy(inputGruState, outGruState, inputGruStateTensor->bytes);
    }
    return true;
}
