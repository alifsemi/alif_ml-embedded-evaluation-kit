/*
 * SPDX-FileCopyrightText: Copyright 2021-2026 Arm Limited and/or its affiliates
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
#include "mlek/fwk/tflm/TflmModel.hpp"
#include "mlek/fwk/iface/Model.hpp"
#include "mlek/fwk/tflm/TflmResourceVariables.hpp"
#include "mlek/fwk/tflm/TflmTensor.hpp"
#include "mlek/log/log_macros.h"

#include <cinttypes>
#include <cstring>
#include <memory>
#include <string>

namespace arm::app::fwk::tflm {
namespace {
    /**
     * @brief   Infer Arm Ethos-U NPU memory mode from Ethos-U operator inputs.
     * @param[in] model     Pointer the the model object.
     * @return  Memory mode string if detected, or empty string otherwise.
     */
    std::string InferNpuMemoryModeFromEthosUOp(const tflite::Model* model)
    {
        if (!model || !model->subgraphs() || model->subgraphs()->size() == 0 ||
            !model->operator_codes()) {
            return {};
        }

        const tflite::SubGraph* subgraph = model->subgraphs()->Get(0);
        if (!subgraph || !subgraph->operators()) {
            return {};
        }

        int ethos_u_opcode = -1;
        for (uint32_t i = 0; i < model->operator_codes()->size(); ++i) {
            const tflite::OperatorCode* opcode = model->operator_codes()->Get(i);
            if (!opcode || opcode->builtin_code() != tflite::BuiltinOperator_CUSTOM) {
                continue;
            }
            const auto* custom = opcode->custom_code();
            if (custom && (std::strcmp(custom->c_str(), "ethos-u") == 0)) {
                ethos_u_opcode = static_cast<int>(i);
                break;
            }
        }

        if (ethos_u_opcode < 0) {
            return {};
        }

        for (uint32_t i = 0; i < subgraph->operators()->size(); ++i) {
            const tflite::Operator* op = subgraph->operators()->Get(i);
            if (!op || op->opcode_index() != ethos_u_opcode) {
                continue;
            }
            const auto* inputs = op->inputs();
            if (!inputs || inputs->size() <= 3) {
                continue;
            }

            /* Vela places tensor arena at index 2 and fast scratch buffer at index 3. */
            const int32_t arena_idx   = inputs->Get(2);
            const int32_t scratch_idx = inputs->Get(3);
            if (arena_idx < 0 || scratch_idx < 0) {
                continue;
            }

            const tflite::Tensor* arena   = subgraph->tensors()->Get(arena_idx);
            const tflite::Tensor* scratch = subgraph->tensors()->Get(scratch_idx);
            if (!arena || !scratch) {
                continue;
            }

            const auto arena_shape   = arena->shape();
            const auto scratch_shape = scratch->shape();
            if (!arena_shape || !scratch_shape || arena_shape->size() < 1 ||
                scratch_shape->size() < 1 || arena_shape->Get(0) < 1) {
                continue;
            }

            if (arena_shape->Get(0) == scratch_shape->Get(0)) {
                return "Sram_Only/Shared_Sram";
            }
            return "Dedicated_Sram";
        }

        return {};
    }
} /* anonymous namespace */

TflmModel::TflmModel() {}

/* Initialise the model */
bool TflmModel::Init(iface::MemoryRegion& computeBuffer,
                     iface::MemoryRegion& nnModel,
                     const void* backendData)
{
    /* Following tf lite micro example:
     * Map the model into a usable data structure. This doesn't involve any
     * copying or parsing, it's a very lightweight operation. */
    debug("loading model from @ 0x%p\n", nnModel.data);
    debug("model size: %" PRIu32 " bytes.\n", nnModel.size);

    /* Enable logging before we start interacting with the framework. */
    EnableTFLMLog();

    this->m_backendData.m_pModel = ::tflite::GetModel(nnModel.data);

    if (this->m_backendData.m_pModel->version() != TFLITE_SCHEMA_VERSION) {
        printf_err("Model's schema version %" PRIu32 " is not equal "
                   "to supported version %d.",
                   this->m_backendData.m_pModel->version(),
                   TFLITE_SCHEMA_VERSION);
        return false;
    }

    this->m_computeBuffer = computeBuffer;
    this->m_modelBuffer   = nnModel;

    /* Pull in only the operation implementations we need.
     * This relies on a complete list of all the ops needed by this graph.
     * An easier approach is to just use the AllOpsResolver, but this will
     * incur some penalty in code space for op implementations that are not
     * needed by this graph.
     * static ::tflite::ops::micro::AllOpsResolver resolver; */
    /* NOLINTNEXTLINE(runtime-global-variables) */
    debug("loading op resolver\n");

    this->EnlistOperations();

    /** If backend data has been provided, re-use the allocator */
    if (backendData) {
        this->m_backendData.m_pAllocator =
            reinterpret_cast<const TflmBackendData*>(backendData)->m_pAllocator;
    }

    /* Create allocator instance, if it doesn't exist */
    if (!this->m_backendData.m_pAllocator) {
        /* Create an allocator instance */
        info("Creating allocator using tensor arena at 0x%p\n", computeBuffer.data);

        this->m_backendData.m_pAllocator =
            tflite::MicroAllocator::Create(computeBuffer.data, computeBuffer.size);

        if (!this->m_backendData.m_pAllocator) {
            printf_err("Failed to create allocator\n");
            return false;
        }
        debug("Created new allocator @ 0x%p\n", this->m_backendData.m_pAllocator);
    } else {
        debug("Using existing allocator @ 0x%p\n", this->m_backendData.m_pAllocator);
    }

    const int resourceVariableCount = CountResourceVariables(this->m_backendData.m_pModel);
    tflite::MicroResourceVariables* resourceVariables =
        CreateResourceVariables(resourceVariableCount, this->m_backendData.m_pAllocator);
    if (resourceVariableCount > 0 && !resourceVariables) {
        printf_err("Failed to create resource variables\n");
        return false;
    }

    this->m_backendData.m_pInterpreter =
        std::make_unique<tflite::MicroInterpreter>(this->m_backendData.m_pModel,
                                                   this->GetOpResolver(),
                                                   this->m_backendData.m_pAllocator,
                                                   resourceVariables);

    if (!this->m_backendData.m_pInterpreter) {
        printf_err("Failed to allocate interpreter\n");
        return false;
    }

    /* Allocate memory from the tensor_arena for the model's tensors. */
    info("Allocating tensors\n");
    TfLiteStatus allocate_status = this->m_backendData.m_pInterpreter->AllocateTensors();

    if (allocate_status != kTfLiteOk) {
        printf_err("tensor allocation failed!\n");
        return false;
    }

    /* Get information about the memory area to use for the model's input. */
    this->m_input.resize(this->GetNumInputs());
    for (size_t inIndex = 0; inIndex < this->GetNumInputs(); inIndex++) {
        this->m_input[inIndex] =
            std::make_shared<TflmTensor>(this->m_backendData.m_pInterpreter->input(inIndex));
    }

    this->m_output.resize(this->GetNumOutputs());
    for (size_t outIndex = 0; outIndex < this->GetNumOutputs(); outIndex++) {
        this->m_output[outIndex] =
            std::make_shared<TflmTensor>(this->m_backendData.m_pInterpreter->output(outIndex));
    }

    if (this->m_input.empty() || this->m_output.empty()) {
        printf_err("failed to get tensors\n");
        return false;
    } else {
        this->m_type = this->m_input[0]->Type(); /* Input 0 should be the main input */

        /* Clear the input & output tensors */
        for (size_t inIndex = 0; inIndex < this->GetNumInputs(); inIndex++) {
            std::memset(this->m_input[inIndex]->GetData(), 0, this->m_input[inIndex]->Bytes());
        }
        for (size_t outIndex = 0; outIndex < this->GetNumOutputs(); outIndex++) {
            std::memset(this->m_output[outIndex]->GetData(), 0, this->m_output[outIndex]->Bytes());
        }

        this->LogInterpreterInfo();
    }

    this->m_inited = true;
    return true;
}

std::shared_ptr<iface::TensorIface> TflmModel::GetInputTensor(size_t index) const
{
    if (index < this->GetNumInputs()) {
        return this->m_input.at(index);
    }
    return {};
}

std::shared_ptr<iface::TensorIface> TflmModel::GetOutputTensor(size_t index) const
{
    if (index < this->GetNumOutputs()) {
        return this->m_output.at(index);
    }
    return nullptr;
}

void TflmModel::LogTensorInfo(std::shared_ptr<iface::TensorIface> tensor)
{
    if (!tensor) {
        printf_err("Invalid tensor\n");
        assert(tensor);
        return;
    }

    debug("\ttensor is assigned to 0x%p\n", tensor.get());
    info("\ttensor type is %s\n", GetTensorDataTypeName(tensor->Type()));
    info("\ttensor occupies %zu bytes with dimensions\n", tensor->Bytes());
    const auto shape = tensor->Shape();
    for (size_t i = 0; i < shape.size(); ++i) {
        info("\t\t%zu: %3d\n", i, static_cast<int>(shape[i]));
    }

    const auto quant = tensor->GetQuantParams();
    info("Scale = %f\n", quant.scale);
    info("ZeroPoint = %d\n", quant.offset);
}

void TflmModel::LogInterpreterInfo()
{
    if (!this->m_backendData.m_pInterpreter) {
        printf_err("Invalid interpreter\n");
        return;
    }

    info("Model INPUT tensors: \n");
    for (auto& input : this->m_input) {
        this->LogTensorInfo(input);
    }

    info("Model OUTPUT tensors: \n");
    for (auto& output : this->m_output) {
        this->LogTensorInfo(output);
    }

    info("Activation buffer (a.k.a tensor arena) size used: %zu\n",
         this->m_backendData.m_pInterpreter->arena_used_bytes());

    /* We expect there to be only one subgraph. */
    const uint32_t nOperators = tflite::NumSubgraphOperators(this->m_backendData.m_pModel, 0);
    info("Number of operators: %" PRIu32 "\n", nOperators);

    const tflite::SubGraph* subgraph = this->m_backendData.m_pModel->subgraphs()->Get(0);

    auto* opcodes = this->m_backendData.m_pModel->operator_codes();

    /* For each operator, display registration information. */
    for (size_t i = 0; i < nOperators; ++i) {
        const tflite::Operator* op         = subgraph->operators()->Get(i);
        const tflite::OperatorCode* opcode = opcodes->Get(op->opcode_index());
        const TFLMRegistration* reg        = nullptr;

        tflite::GetRegistrationFromOpCode(opcode, this->GetOpResolver(), &reg);
        std::string opName;

        if (reg) {
            if (tflite::BuiltinOperator_CUSTOM == reg->builtin_code) {
                opName = std::string(reg->custom_name);
            } else {
                opName = std::string(
                    EnumNameBuiltinOperator(tflite::BuiltinOperator(reg->builtin_code)));
            }
        }
        info("\tOperator %zu: %s\n", i, opName.c_str());
    }

    if (this->ContainsEthosUOperator()) {
        const std::string mode = InferNpuMemoryModeFromEthosUOp(this->m_backendData.m_pModel);
        if (!mode.empty()) {
            info("NPU memory mode likely to be: %s\n", mode.c_str());
        } else {
            warn("Unable to infer NPU memory mode.\n");
        }
    }
}

bool TflmModel::IsInited() const
{ return this->m_inited; }

bool TflmModel::IsDataSigned() const
{
    switch (this->GetType()) {
    case iface::TensorType::INT8:
        [[fallthrough]];
    case iface::TensorType::INT16:
        [[fallthrough]];
    case iface::TensorType::INT32:
        return true;
    default:
        return false;
    }
}

bool TflmModel::ContainsEthosUOperator() const
{
    /* We expect there to be only one subgraph. */
    const uint32_t nOperators = tflite::NumSubgraphOperators(this->m_backendData.m_pModel, 0);
    const tflite::SubGraph* subgraph = this->m_backendData.m_pModel->subgraphs()->Get(0);
    const auto* opcodes              = this->m_backendData.m_pModel->operator_codes();

    /* check for custom operators */
    for (size_t i = 0; (i < nOperators); ++i) {
        const tflite::Operator* op         = subgraph->operators()->Get(i);
        const tflite::OperatorCode* opcode = opcodes->Get(op->opcode_index());

        auto builtin_code = tflite::GetBuiltinCode(opcode);
        if ((builtin_code == tflite::BuiltinOperator_CUSTOM) &&
            (nullptr != opcode->custom_code()) &&
            ("ethos-u" == std::string(opcode->custom_code()->c_str()))) {
            return true;
        }
    }
    return false;
}

bool TflmModel::RunInference()
{
    bool inference_state = false;
    if (this->m_backendData.m_pModel && this->m_backendData.m_pInterpreter) {
        if (kTfLiteOk != this->m_backendData.m_pInterpreter->Invoke()) {
            printf_err("Invoke failed.\n");
        } else {
            inference_state = true;
        }
    } else {
        printf_err("Error: No interpreter!\n");
    }
    return inference_state;
}

size_t TflmModel::GetNumInputs() const
{
    if (this->m_backendData.m_pModel && this->m_backendData.m_pInterpreter) {
        return this->m_backendData.m_pInterpreter->inputs_size();
    }
    return 0;
}

size_t TflmModel::GetNumOutputs() const
{
    if (this->m_backendData.m_pModel && this->m_backendData.m_pInterpreter) {
        return this->m_backendData.m_pInterpreter->outputs_size();
    }
    return 0;
}

iface::TensorType TflmModel::GetType() const
{ return this->m_type; }

std::vector<size_t> TflmModel::GetInputShape(size_t index) const
{
    if (index < this->GetNumInputs()) {
        return this->m_input.at(index)->Shape();
    }
    return std::vector<size_t>();
}

std::vector<size_t> TflmModel::GetOutputShape(size_t index) const
{
    if (index < this->GetNumOutputs()) {
        return this->m_output.at(index)->Shape();
    }
    return std::vector<size_t>();
}

/** @brief   Gets a pointer to the backend's compute buffer. */
const iface::MemoryRegion& TflmModel::GetComputeBuffer() const
{ return std::ref(this->m_computeBuffer); }

/** @brief      Gets the pointer to the NN model data array. */
const iface::MemoryRegion& TflmModel::GetModelBuffer() const
{ return std::ref(this->m_modelBuffer); }

const TflmBackendData& TflmModel::GetBackendData() const
{ return std::ref(this->m_backendData); }

} /* namespace arm::app::fwk::tflm */
