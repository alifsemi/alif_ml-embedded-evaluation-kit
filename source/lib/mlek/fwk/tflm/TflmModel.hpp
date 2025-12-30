/*
 * SPDX-FileCopyrightText: Copyright 2021-2023, 2025 Arm Limited and/or its affiliates
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
#ifndef MLEK_TFLM_MODEL_HPP
#define MLEK_TFLM_MODEL_HPP

#include "mlek/fwk/iface/Model.hpp"
#include "mlek/fwk/tflm/TensorFlowLiteMicro.hpp"
#include <cstdint>

namespace arm::app::fwk::tflm {

/**
 * Backend data for TensorFlow Lite Micro framework.
 */
struct TflmBackendData {
    const tflite::Model* m_pModel{nullptr};        /**< Tflite model pointer. */
    tflite::MicroAllocator* m_pAllocator{nullptr}; /**< Tflite micro allocator. */

    /** Tflite interpreter. */
    std::unique_ptr<tflite::MicroInterpreter> m_pInterpreter{nullptr};
};

/**
 * @brief   NN model class wrapping the underlying TensorFlow-Lite-Micro API.
 */
class TflmModel : public iface::Model {
public:
    /** @brief Constructor. */
    TflmModel();

    /** @brief Destructor. */
    virtual ~TflmModel() = default;

    /** @brief  Gets the pointer to the model's input tensor at given input index. */
    std::shared_ptr<iface::TensorIface> GetInputTensor(size_t index) const override;

    /** @brief  Gets the pointer to the model's output tensor at given output index. */
    std::shared_ptr<iface::TensorIface> GetOutputTensor(size_t index) const override;

    /** @brief  Gets the model's data type. */
    iface::TensorType GetType() const override;

    /** @brief  Gets the pointer to the model's input shape. */
    std::vector<size_t> GetInputShape(size_t index) const override;

    /** @brief  Gets the pointer to the model's output shape at given output index. */
    std::vector<size_t> GetOutputShape(size_t index) const override;

    /** @brief  Gets the number of input tensors the model has. */
    size_t GetNumInputs() const override;

    /** @brief  Gets the number of output tensors the model has. */
    size_t GetNumOutputs() const override;

    /** @brief  Logs the tensor information to stdout. */
    void LogTensorInfo(std::shared_ptr<iface::TensorIface> tensor) override;

    /** @brief  Logs the interpreter information to stdout. */
    void LogInterpreterInfo() override;

    /** @brief      Initialise the model class object.
     *  @return     true if initialisation succeeds, false otherwise.
     **/
    bool Init(iface::MemoryRegion& computeBuffer,
              iface::MemoryRegion& nnModel,
              const void* backendData = nullptr) override;

    /** @brief  Checks if this object has been initialised. */
    bool IsInited() const override;

    /** @brief  Checks if the model uses signed data. */
    bool IsDataSigned() const override;

    /** @brief Checks if the model uses Ethos-U operator */
    bool ContainsEthosUOperator() const override;

    /** @brief  Runs the inference (invokes the interpreter). */
    bool RunInference() override;

    /**
     * @brief   Gets memory region information for the backend's compute buffer.
     * @return  Const reference to MemoryRegion object.
     */
    const iface::MemoryRegion& GetComputeBuffer() const override;

    /**
     * @brief   Gets memory region information for neural network model.
     * @return  Const reference to MemoryRegion object.
     */
    const iface::MemoryRegion& GetModelBuffer() const override;

    /**
     * @brief       Gets TensorFlow Lite Micro backend data.
     * @return      Backend data, with pointers to the model, memory allocator and interpreter.
     **/
    const TflmBackendData& GetBackendData() const;

protected:
    /**
     * @brief       Gets the op resolver for the model instance.
     * @return      const reference to a tflite::MicroOpResolver object.
     **/
    virtual const tflite::MicroOpResolver& GetOpResolver() = 0;

    /**
     * @brief       Add all the operators required for the given model.
     *              Implementation of this should come from the use case.
     * @return      true is ops are successfully added, false otherwise.
     **/
    virtual bool EnlistOperations() = 0;

private:
    TflmBackendData m_backendData{};
    bool m_inited{false}; /* Indicates whether this object has been initialised. */
    iface::MemoryRegion m_computeBuffer{};
    iface::MemoryRegion m_modelBuffer{};

    std::vector<std::shared_ptr<iface::TensorIface>> m_input{}; /* Model's input tensor pointers. */
    std::vector<std::shared_ptr<iface::TensorIface>>
        m_output{};                                       /* Model's output tensor pointers. */
    iface::TensorType m_type{iface::TensorType::INVALID}; /* Model's data type. */
};
} /* namespace arm::app::fwk::tflm */

#endif /* MLEK_TFLM_MODEL_HPP */
