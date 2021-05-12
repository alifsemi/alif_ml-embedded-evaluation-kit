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
#include "Model.hpp"

#include "hal.h"

#include <cstdint>
#include <inttypes.h>

/* Initialise the model */
arm::app::Model::~Model()
{
    if (this->m_pInterpreter) {
        delete this->m_pInterpreter;
    }

    /**
     * No clean-up function available for allocator in TensorFlow Lite Micro yet.
     **/
}

arm::app::Model::Model() :
    m_inited (false),
    m_type(kTfLiteNoType)
{
    this->m_pErrorReporter = &this->m_uErrorReporter;
}

bool arm::app::Model::Init(tflite::MicroAllocator* allocator)
{
    /* Following tf lite micro example:
     * Map the model into a usable data structure. This doesn't involve any
     * copying or parsing, it's a very lightweight operation. */
    const uint8_t* model_addr = ModelPointer();
    debug("loading model from @ 0x%p\n", model_addr);
    this->m_pModel = ::tflite::GetModel(model_addr);

    if (this->m_pModel->version() != TFLITE_SCHEMA_VERSION) {
        this->m_pErrorReporter->Report(
            "[ERROR] model's schema version %d is not equal "
            "to supported version %d.",
            this->m_pModel->version(), TFLITE_SCHEMA_VERSION);
        return false;
    }

    /* Pull in only the operation implementations we need.
     * This relies on a complete list of all the ops needed by this graph.
     * An easier approach is to just use the AllOpsResolver, but this will
     * incur some penalty in code space for op implementations that are not
     * needed by this graph.
     * static ::tflite::ops::micro::AllOpsResolver resolver; */
    /* NOLINTNEXTLINE(runtime-global-variables) */
    debug("loading op resolver\n");

    this->EnlistOperations();

    /* Create allocator instance, if it doesn't exist */
    this->m_pAllocator = allocator;
    if (!this->m_pAllocator) {
        /* Create an allocator instance */
        info("Creating allocator using tensor arena in %s\n",
            ACTIVATION_BUF_SECTION_NAME);

        this->m_pAllocator = tflite::MicroAllocator::Create(
                                        this->GetTensorArena(),
                                        this->GetActivationBufferSize(),
                                        this->m_pErrorReporter);

        if (!this->m_pAllocator) {
            printf_err("Failed to create allocator\n");
            return false;
        }
        debug("Created new allocator @ 0x%p\n", this->m_pAllocator);
    } else {
        debug("Using existing allocator @ 0x%p\n", this->m_pAllocator);
    }

    this->m_pInterpreter = new ::tflite::MicroInterpreter(
        this->m_pModel, this->GetOpResolver(),
        this->m_pAllocator, this->m_pErrorReporter);

    if (!this->m_pInterpreter) {
        printf_err("Failed to allocate interpreter\n");
        return false;
    }

    /* Allocate memory from the tensor_arena for the model's tensors. */
    info("Allocating tensors\n");
    TfLiteStatus allocate_status = this->m_pInterpreter->AllocateTensors();

    if (allocate_status != kTfLiteOk) {
        this->m_pErrorReporter->Report("[ERROR] allocateTensors() failed");
        printf_err("tensor allocation failed!\n");
        delete this->m_pInterpreter;
        return false;
    }

    /* Get information about the memory area to use for the model's input. */
    this->m_input.resize(this->GetNumInputs());
    for (size_t inIndex = 0; inIndex < this->GetNumInputs(); inIndex++)
        this->m_input[inIndex] = this->m_pInterpreter->input(inIndex);

    this->m_output.resize(this->GetNumOutputs());
    for (size_t outIndex = 0; outIndex < this->GetNumOutputs(); outIndex++)
        this->m_output[outIndex] = this->m_pInterpreter->output(outIndex);

    if (this->m_input.empty() || this->m_output.empty()) {
        printf_err("failed to get tensors\n");
        return false;
    } else {
        this->m_type = this->m_input[0]->type;  /* Input 0 should be the main input */

        /* Clear the input & output tensors */
        for (size_t inIndex = 0; inIndex < this->GetNumInputs(); inIndex++) {
            std::memset(this->m_input[inIndex]->data.data, 0, this->m_input[inIndex]->bytes);
        }
        for (size_t outIndex = 0; outIndex < this->GetNumOutputs(); outIndex++) {
            std::memset(this->m_output[outIndex]->data.data, 0, this->m_output[outIndex]->bytes);
        }

        this->LogInterpreterInfo();
    }

    this->m_inited = true;
    return true;
}

tflite::MicroAllocator* arm::app::Model::GetAllocator()
{
    if (this->IsInited()) {
        return this->m_pAllocator;
    }
    return nullptr;
}

void arm::app::Model::LogTensorInfo(TfLiteTensor* tensor)
{
    if (!tensor) {
        printf_err("Invalid tensor\n");
        assert(tensor);
        return;
    }

    debug("\ttensor is assigned to 0x%p\n", tensor);
    info("\ttensor type is %s\n", TfLiteTypeGetName(tensor->type));
    info("\ttensor occupies %zu bytes with dimensions\n",
         tensor->bytes);
    for (int i = 0 ; i < tensor->dims->size; ++i) {
        info ("\t\t%d: %3d\n", i, tensor->dims->data[i]);
    }

    TfLiteQuantization quant = tensor->quantization;
    if (kTfLiteAffineQuantization == quant.type) {
        auto* quantParams = (TfLiteAffineQuantization*)quant.params;
        info("Quant dimension: %" PRIi32 "\n", quantParams->quantized_dimension);
        for (int i = 0; i < quantParams->scale->size; ++i) {
            info("Scale[%d] = %f\n", i, quantParams->scale->data[i]);
        }
        for (int i = 0; i < quantParams->zero_point->size; ++i) {
            info("ZeroPoint[%d] = %d\n", i, quantParams->zero_point->data[i]);
        }
    }
}

void arm::app::Model::LogInterpreterInfo()
{
    if (!this->m_pInterpreter) {
        printf_err("Invalid interpreter\n");
        return;
    }

    info("Model INPUT tensors: \n");
    for (auto input : this->m_input) {
        this->LogTensorInfo(input);
    }

    info("Model OUTPUT tensors: \n");
    for (auto output : this->m_output) {
        this->LogTensorInfo(output);
    }

    info("Activation buffer (a.k.a tensor arena) size used: %zu\n",
        this->m_pInterpreter->arena_used_bytes());

    const size_t nOperators = this->m_pInterpreter->operators_size();
    info("Number of operators: %zu\n", nOperators);

    /* For each operator, display registration information */
    for (size_t i = 0 ; i < nOperators; ++i) {
        const tflite::NodeAndRegistration nodeReg =
            this->m_pInterpreter->node_and_registration(i);
        const TfLiteRegistration* reg = nodeReg.registration;
        std::string opName{""};

        if (reg) {
            if (tflite::BuiltinOperator_CUSTOM == reg->builtin_code) {
                opName = std::string(reg->custom_name);
            } else {
                opName = std::string(EnumNameBuiltinOperator(
                            tflite::BuiltinOperator(reg->builtin_code)));
            }
        }
        info("\tOperator %zu: %s\n", i, opName.c_str());
    }
}

bool arm::app::Model::IsInited() const
{
    return this->m_inited;
}

bool arm::app::Model::IsDataSigned() const
{
    return this->GetType() == kTfLiteInt8;
}

bool arm::app::Model::RunInference()
{
    bool inference_state = false;
    if (this->m_pModel && this->m_pInterpreter) {
        if (kTfLiteOk != this->m_pInterpreter->Invoke()) {
            printf_err("Invoke failed.\n");
        } else {
            inference_state = true;
        }
    } else {
        printf_err("Error: No interpreter!\n");
    }
    return inference_state;
}

TfLiteTensor* arm::app::Model::GetInputTensor(size_t index) const
{
    if (index < this->GetNumInputs()) {
        return this->m_input.at(index);
    }
    return nullptr;
}

TfLiteTensor* arm::app::Model::GetOutputTensor(size_t index) const
{
    if (index < this->GetNumOutputs()) {
        return this->m_output.at(index);
    }
    return nullptr;
}

size_t arm::app::Model::GetNumInputs() const
{
    if (this->m_pModel && this->m_pInterpreter) {
        return this->m_pInterpreter->inputs_size();
    }
    return 0;
}

size_t arm::app::Model::GetNumOutputs() const
{
    if (this->m_pModel && this->m_pInterpreter) {
        return this->m_pInterpreter->outputs_size();
    }
    return 0;
}


TfLiteType arm::app::Model::GetType() const
{
    return this->m_type;
}

TfLiteIntArray* arm::app::Model::GetInputShape(size_t index) const
{
    if (index < this->GetNumInputs()) {
        return this->m_input.at(index)->dims;
    }
    return nullptr;
}

TfLiteIntArray* arm::app::Model::GetOutputShape(size_t index) const
{
    if (index < this->GetNumOutputs()) {
        return this->m_output.at(index)->dims;
    }
    return nullptr;
}

bool arm::app::Model::ShowModelInfoHandler()
{
    if (!this->IsInited()) {
        printf_err("Model is not initialised! Terminating processing.\n");
        return false;
    }

    PrintTensorFlowVersion();
    info("Model info:\n");
    this->LogInterpreterInfo();

#if defined(ARM_NPU)
    info("Use of Arm uNPU is enabled\n");
#else   /* ARM_NPU */
    info("Use of Arm uNPU is disabled\n");
#endif  /* ARM_NPU */

    return true;
}
namespace arm {
namespace app {
    static uint8_t  tensor_arena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
} /* namespace app */
} /* namespace arm */

size_t arm::app::Model::GetActivationBufferSize()
{
    return ACTIVATION_BUF_SZ;
}

uint8_t *arm::app::Model::GetTensorArena()
{
    return tensor_arena;
}