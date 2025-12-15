/*
 * SPDX-FileCopyrightText: Copyright 2021-2025 Arm Limited and/or its affiliates
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
#include "EtModel.hpp"

#include "EtTensor.hpp"
#include "Model.hpp"
#include "log_macros.h"

#include <memory>

#if !(defined(ML_FWK_TMP_MEM_SIZE))
#error "ML_FWK_TMP_MEM_SIZE should be defined."
#endif /* ML_FWK_TMP_MEM_SIZE */

constexpr size_t sTmpAllocationPoolSz = ML_FWK_TMP_MEM_SIZE; /**< Temp allocation pool size. */

#if !(defined(ML_FWK_TMP_MEM_BASE))
static uint8_t __attribute__((aligned(16), section("ifm")))
    sTmpAllocationPool[sTmpAllocationPoolSz]; /**< temp allocation buffer */
#else /* !(defined(ML_FWK_TMP_MEM_BASE)) */
static uint8_t* sTmpAllocationPool = reinterpret_cast<uint8_t *>(ML_FWK_TMP_MEM_BASE);
#endif /* !(defined(ML_FWK_TMP_MEM_BASE)) */

namespace arm::app::fwk::et {

EtModel::EtModel()
{
    info("Initialising ExecuTorch runtime\n");
    executorch::runtime::runtime_init();
}

EtModel::~EtModel()
{
    this->LogMemoryUsage();
}

/* Initialise the model */
bool EtModel::Init(iface::MemoryRegion& computeBuffer,
                   iface::MemoryRegion& nnModel,
                   const void* backendData)
{
    debug("loading model from @ 0x%p\n", nnModel.data);
    debug("model size: %" PRIu32 " bytes.\n", nnModel.size);

    if (backendData) {
        this->m_backendData = *static_cast<const EtBackendData*>(backendData);
    }

    info("Model PTE file loaded. Size: %zu bytes.\n", nnModel.size);
    auto loader = executorch::extension::BufferDataLoader(nnModel.data, nnModel.size);

    this->m_backendData.m_program = std::make_shared<Result<Program>>(Program::load(&loader));

    if (!this->m_backendData.m_program->ok()) {
        printf_err("Program loading failed @ 0x%p: 0x%" PRIx32 "\n",
                   nnModel.data,
                   static_cast<uint32_t>(this->m_backendData.m_program->error()));
        return false;
    }
    info("Model buffer loaded, has %zu methods\n",
         this->m_backendData.m_program->get().num_methods());

    if (this->m_backendData.m_methodName.empty()) {
        const auto methodNameResult = this->m_backendData.m_program->get().get_method_name(0);
        ET_CHECK_MSG(methodNameResult.ok(), "Program has no methods");
        this->m_backendData.m_methodName = std::string(*methodNameResult);
    }

    info("Method found \"%s\"\n", this->m_backendData.m_methodName.c_str());

    auto methodMeta =
        this->m_backendData.m_program->get().method_meta(this->m_backendData.m_methodName.c_str());
    if (!methodMeta.ok()) {
        printf_err("Failed to get method_meta for %s: 0x%" PRIx32 "\n",
                   this->m_backendData.m_methodName.c_str(),
                   static_cast<uint32_t>(methodMeta.error()));
        return false;
    }

    info("Setting up method allocator pool. Base: %p, size: %zu bytes.\n",
         computeBuffer.data, computeBuffer.size);

    if (!this->m_backendData.m_methodAllocPtr) {
        this->m_backendData.m_methodAllocPtr =
            std::make_shared<EtMemoryAllocator>(computeBuffer.size, computeBuffer.data);
    }

    std::vector<executorch::runtime::Span<uint8_t>> plannedSpans; // Passed to the allocator
    const size_t numMemoryPlannedBuffers = methodMeta->num_memory_planned_buffers();
    size_t plannedMemBeforeMark          = this->m_backendData.m_methodAllocPtr->UsedSizeCurrent();

    for (size_t id = 0; id < numMemoryPlannedBuffers; ++id) {
        size_t bufferSize = static_cast<size_t>(methodMeta->memory_planned_buffer_size(id).get());
        info("Setting up planned buffer %zu, size %zu.\n", id, bufferSize);

        /* Move to its own allocator when MemoryPlanner is in place. */
        uint8_t* buffer =
            reinterpret_cast<uint8_t*>(this->m_backendData.m_methodAllocPtr->allocate(bufferSize));
        plannedSpans.push_back(executorch::runtime::Span{buffer, bufferSize});
    }

    info("Total planned memory allocated: %zu bytes\n",
         this->m_backendData.m_methodAllocPtr->UsedSizeCurrent() - plannedMemBeforeMark);

    if (!this->m_backendData.m_plannedMemAllocPtr) {
        this->m_backendData.m_plannedMemAllocPtr = std::make_shared<HierarchicalAllocator>(
            executorch::runtime::Span{plannedSpans.data(), plannedSpans.size()});
    }

    if (!this->m_backendData.m_tmpAllocPtr) {
         info("Setting up temporary allocator pool. Base: %p, size: %zu bytes.\n",
            sTmpAllocationPool, sTmpAllocationPoolSz);

        this->m_backendData.m_tmpAllocPtr =
            std::make_shared<EtMemoryAllocator>(sTmpAllocationPoolSz, sTmpAllocationPool);
    }

    if (!this->m_backendData.m_memMgrPtr) {
        this->m_backendData.m_memMgrPtr =
            std::make_shared<MemoryManager>(this->m_backendData.m_methodAllocPtr.get(),
                                            this->m_backendData.m_plannedMemAllocPtr.get(),
                                            this->m_backendData.m_tmpAllocPtr.get());
    }

    size_t methodMemBeforeMark = this->m_backendData.m_methodAllocPtr->UsedSizeCurrent();

    executorch::runtime::EventTracer* eventTracerPtr = nullptr;
    static auto method =
        this->m_backendData.m_program->get().load_method(this->m_backendData.m_methodName.c_str(),
                                                         this->m_backendData.m_memMgrPtr.get(),
                                                         eventTracerPtr);

    if (!method.ok()) {
        info("Loading of method \"%s\" failed with status 0x%" PRIx32 "\n",
             this->m_backendData.m_methodName.c_str(),
             static_cast<uint32_t>(method.error()));
        return false;
    }

    /**
     * Bypass the method class restrictions by creating a lambda
     * function to get reference. This function can then be used
     * by other member functions to get a reference to the method
     * (static/bss) we created above
     **/
    this->m_fnGetMethod = [&]() { return std::ref(method.get()); };

    info("Method memory allocated: %zu bytes\n",
         this->m_backendData.m_methodAllocPtr->UsedSizeCurrent() - methodMemBeforeMark);
    info("Method %s loaded.\n", this->m_backendData.m_methodName.c_str());

    info("Preparing inputs...\n");
    size_t input_membase = this->m_backendData.m_methodAllocPtr->UsedSizeCurrent();

    std::vector<std::pair<char*, size_t>> inputBuffers;
    if (!this->PrepareInputTensors()) {
        printf_err("Failed to prepare inputs\n");
        return false;
    }

    info("Inputs allocated: %zu bytes.\n",
         this->m_backendData.m_methodAllocPtr->UsedSizeCurrent() - input_membase);

    std::vector<executorch::runtime::EValue> outputs(method->outputs_size());
    info("Preparing outputs; count: %zu\n", outputs.size());
    auto status = method->get_outputs(outputs.data(), outputs.size());
    if (status != executorch::runtime::Error::Ok) {
        printf_err("Failed to get output tensors\n");
        return false;
    }

    for (size_t i = 0; i < outputs.size(); ++i) {
        this->m_outputTensor.emplace_back(outputs[i].toTensor());
        this->m_output.emplace_back(
            std::make_shared<EtTensor>(this->m_outputTensor[this->m_outputTensor.size() - 1]));
    }
    info("Outputs prepared\n");
    this->m_inited = true;
    this->LogInterpreterInfo();

    info("Model initialisation complete\n");
    return true;
}

std::shared_ptr<iface::TensorIface> EtModel::GetInputTensor(size_t index) const
{
    if (index < this->GetNumInputs()) {
        return this->m_input.at(index);
    }
    return nullptr;
}

std::shared_ptr<iface::TensorIface> EtModel::GetOutputTensor(size_t index) const
{
    if (index < this->GetNumOutputs()) {
        return this->m_output.at(index);
    }
    return nullptr;
}

void EtModel::LogTensorInfo(std::shared_ptr<iface::TensorIface> tensor)
{
    if (!tensor) {
        printf_err("Invalid tensor\n");
        return;
    }

    info("Tensor: %zu bytes; %zu elements\n", tensor->Bytes(), tensor->GetNumElements());
    auto shape = tensor->Shape();
    std::string shapeStr("[");
    for (auto s : shape) {
        shapeStr += std::to_string(s) + ", ";
    }
    shapeStr.erase(shapeStr.size() - 2, shapeStr.size());
    shapeStr += "]";
    info("\tShape:\t%s\n", shapeStr.c_str());
    info("\tType:\t%s\n", fwk::iface::GetTensorDataTypeName(tensor->Type()));
}

void EtModel::LogInterpreterInfo()
{
    info("Number of input tensors:  %zu\n", this->GetNumInputs());
    for (auto i : this->m_input) {
        this->LogTensorInfo(i);
    }
    info("Number of output tensors: %zu\n", this->GetNumOutputs());
    for (auto o : this->m_output) {
        this->LogTensorInfo(o);
    }

    this->LogMemoryUsage();
}

void EtModel::LogMemoryUsage() const
{
    info("Total memory usage: \n");
    info("\tMethod memory: Used: %zu; Peak: %zu; Available: %" PRId32 "\n",
        this->m_backendData.m_methodAllocPtr->UsedSizeCurrent(),
        this->m_backendData.m_methodAllocPtr->UsedSizePeak(),
        this->m_backendData.m_methodAllocPtr->size());
    info("\tTemp memory: Used: %zu; Peak: %zu; Available: %" PRId32 "\n",
        this->m_backendData.m_tmpAllocPtr->UsedSizeCurrent(),
        this->m_backendData.m_tmpAllocPtr->UsedSizePeak(),
        this->m_backendData.m_tmpAllocPtr->size());
}

bool EtModel::IsInited() const
{
    return this->m_inited;
}

bool EtModel::IsDataSigned() const
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

bool EtModel::ContainsEthosUOperator() const
{
    return false;
}

bool EtModel::RunInference()
{
    assert(this->m_inited);
    trace("Running inference\n");
    auto& method = this->m_fnGetMethod();
    executorch::runtime::Error err = executorch::runtime::Error::Ok;

    /** Populate input tensors */
    for (size_t i = 0; i < this->m_inputTensorImplMap.size(); ++i) {
        executorch::aten::Tensor t(&this->m_inputTensorImplMap[i].first);
        err = method.set_input(t, this->m_inputTensorImplMap[i].second);
        if (executorch::runtime::Error::Ok != err) {
            printf_err("Failed to set input tensor. Error: %d\n", static_cast<int>(err));
            return false;
        }
        trace("Input tensor %zu registered with the model; address: %p\n",
            i, t.const_data_ptr());
    }
    err = method.execute();
    trace("Inference done; status = %d\n", static_cast<int>(err));
    return err == executorch::runtime::Error::Ok;
}

size_t EtModel::GetNumInputs() const
{
    assert(this->m_inited);
    return this->m_input.size();
}

size_t EtModel::GetNumOutputs() const
{
    assert(this->m_inited);
    return this->m_output.size();
}

iface::TensorType EtModel::GetType() const
{
    return this->m_type;
}

std::vector<size_t> EtModel::GetInputShape(size_t index) const
{
    assert(index < this->GetNumInputs());
    return this->m_input[index]->Shape();
}

std::vector<size_t> EtModel::GetOutputShape(size_t index) const
{
    assert(index < this->GetNumOutputs());
    return this->m_output[index]->Shape();
}

const iface::MemoryRegion& EtModel::GetComputeBuffer() const
{
    return std::ref(this->m_computeBuffer);
}

const iface::MemoryRegion& EtModel::GetModelBuffer() const
{
    return std::ref(this->m_modelBuffer);
}

const EtBackendData& EtModel::GetBackendData() const
{
    return std::ref(this->m_backendData);
}

bool EtModel::PrepareInputTensors()
{
    auto& method        = this->m_fnGetMethod();
    auto methodMeta     = method.method_meta();
    size_t numInputs    = methodMeta.num_inputs();
    size_t numAllocated = 0;

    void** inputs = static_cast<void**>(
        this->m_backendData.m_methodAllocPtr->allocate(numInputs * sizeof(void*)));

    ET_CHECK_MSG(inputs != nullptr, "Could not allocate memory for pointers to input buffers.");

    for (size_t i = 0; i < numInputs; i++) {
        auto tag = methodMeta.input_tag(i);
        if (!tag.ok()) {
            printf_err("Failed to retrieve tag\n");
            return false;
        }
        if (tag.get() != executorch::runtime::Tag::Tensor) {
            debug("Skipping non-tensor input %zu", i);
            continue;
        }
        auto tensorMeta = methodMeta.input_tensor_meta(i);
        if (!tensorMeta.ok()) {
            printf_err("Failed to retrieve input tensor meta\n");
            return false;
        }

        // Input is a tensor. Allocate a buffer for it.
        void* dataPtr = this->m_backendData.m_methodAllocPtr->allocate(tensorMeta->nbytes());
        ET_CHECK_MSG(dataPtr != nullptr, "Could not allocate memory for input buffers.");
        inputs[numAllocated++] = dataPtr;

        auto p = std::make_pair(executorch::runtime::etensor::TensorImpl(
            tensorMeta.get().scalar_type(),
            tensorMeta.get().sizes().size(),
            const_cast<executorch::aten::TensorImpl::SizesType*>(tensorMeta.get().sizes().data()),
            dataPtr,
            const_cast<executorch::aten::TensorImpl::DimOrderType*>(
                tensorMeta.get().dim_order().data())), i);
        this->m_inputTensorImplMap.push_back(p);
    }

    this->m_input = std::vector<std::shared_ptr<iface::TensorIface>>(
        this->m_inputTensorImplMap.size(), nullptr);
    for (size_t i = 0; i < this->m_inputTensorImplMap.size(); i++) {
        this->m_input[this->m_inputTensorImplMap[i].second] =
            std::make_shared<EtTensor>(&this->m_inputTensorImplMap[i].first);
    }
    return true;
}
} /* namespace arm::app::fwk::et */
