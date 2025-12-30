/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates
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
#ifndef MLEK_MODEL_HPP
#define MLEK_MODEL_HPP

#include "mlek/fwk/iface/Tensor.hpp"
#include <cstdint>
#include <memory>

namespace arm::app::fwk::iface {

/**
 * @brief   Memory region used by underlying framework
 */
struct MemoryRegion {
    uint8_t* data{nullptr};
    size_t size{0};
    explicit MemoryRegion(uint8_t* addr, size_t bytes) : data(addr), size(bytes) {}
    MemoryRegion() = default;
};

/**
 * @brief   NN model class wrapping the underlying TensorFlow-Lite-Micro API.
 */
class Model {
public:
    /** @brief Constructor. */
    Model() = default;

    /** @brief Destructor. */
    virtual ~Model() = 0;

    /** @brief  Gets the pointer to the model's input tensor at given input index. */
    virtual std::shared_ptr<TensorIface> GetInputTensor(size_t index) const = 0;

    /** @brief  Gets the pointer to the model's output tensor at given output index. */
    virtual std::shared_ptr<TensorIface> GetOutputTensor(size_t index) const = 0;

    /** @brief  Gets the model's data type. */
    virtual TensorType GetType() const = 0;

    /** @brief  Gets the pointer to the model's input shape. */
    virtual std::vector<size_t> GetInputShape(size_t index) const = 0;

    /** @brief  Gets the pointer to the model's output shape at given output index. */
    virtual std::vector<size_t> GetOutputShape(size_t index) const = 0;

    /** @brief  Gets the number of input tensors the model has. */
    virtual size_t GetNumInputs() const = 0;

    /** @brief  Gets the number of output tensors the model has. */
    virtual size_t GetNumOutputs() const = 0;

    /** @brief  Logs the tensor information to stdout. */
    virtual void LogTensorInfo(std::shared_ptr<TensorIface> tensor) = 0;

    /** @brief  Logs the interpreter information to stdout. */
    virtual void LogInterpreterInfo() = 0;

    /** @brief      Initialise the model class object.
     *  @return     true if initialisation succeeds, false otherwise.
     **/
    virtual bool
    Init(MemoryRegion& computeBuffer, MemoryRegion& nnModel, const void* backendData) = 0;

    /** @brief  Checks if this object has been initialised. */
    virtual bool IsInited() const = 0;

    /** @brief  Checks if the model uses signed data. */
    virtual bool IsDataSigned() const = 0;

    /** @brief Checks if the model uses Ethos-U operator */
    virtual bool ContainsEthosUOperator() const = 0;

    /** @brief  Runs the inference (invokes the interpreter). */
    virtual bool RunInference() = 0;

    /**
     * @brief   Gets memory region information for the backend's compute buffer.
     * @return  Const reference to MemoryRegion object.
     */
    virtual const MemoryRegion& GetComputeBuffer() const = 0;

    /**
     * @brief   Gets memory region information for neural network model.
     * @return  Const reference to MemoryRegion object.
     */
    virtual const MemoryRegion& GetModelBuffer() const = 0;
};

inline Model::~Model() {}

} /* namespace arm::app::fwk::iface */

#endif /* MLEK_MODEL_HPP */
