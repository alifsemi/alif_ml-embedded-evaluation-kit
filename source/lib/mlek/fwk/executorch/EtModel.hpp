/*
 * SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its affiliates
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
#ifndef MLEK_ET_MODEL_HPP
#define MLEK_ET_MODEL_HPP

#include "mlek/fwk/executorch/EtMemoryAllocator.hpp"
#include "mlek/fwk/executorch/ExecuTorch.hpp"
#include "mlek/fwk/iface/Model.hpp"
#include <functional>

namespace arm::app::fwk::et {

using executorch::runtime::HierarchicalAllocator;
using executorch::runtime::MemoryManager;
using executorch::runtime::Program;
using executorch::runtime::Result;

/**
 * Backend data for ExecuTorch framework.
 */
struct EtBackendData {
    std::shared_ptr<EtMemoryAllocator> m_methodAllocPtr{nullptr};
    std::shared_ptr<EtMemoryAllocator> m_tmpAllocPtr{nullptr};
    std::shared_ptr<HierarchicalAllocator> m_plannedMemAllocPtr{nullptr};
    std::shared_ptr<MemoryManager> m_memMgrPtr{nullptr};
    std::shared_ptr<Result<Program>> m_program{nullptr};
    std::string m_methodName{};
};

/**
 * @brief   NN model class wrapping the underlying TensorFlow-Lite-Micro API.
 */
class EtModel : public iface::Model {
public:
    /** @brief Constructor. */
    EtModel();

    /** @brief Destructor. */
    virtual ~EtModel();

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

    /** @brief Logs overall runtime memory usage to stdout. */
    void LogMemoryUsage() const;

    /** @brief Logs operators from the ExecuTorch program to stdout. */
    void LogOperatorInfo();

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
     * @brief       Gets ExecuTorch backend data.
     * @return      Backend data
     **/
    const EtBackendData& GetBackendData() const;

protected:
    /** @brief  Prepares input tensors. */
    virtual bool PrepareInputTensors();

private:
    std::function<executorch::runtime::Method&()> m_fnGetMethod; /**< Gets the forward inference
                                                                      method. */
    EtBackendData m_backendData{}; /**< Backend data object */
    bool m_inited{false}; /**< Indicates whether this object has been initialised. */
    bool m_hasEthosUDelegate{false}; /**< Indicates whether the model has NPU delegate. */
    iface::MemoryRegion m_computeBuffer{}; /**< Compute buffer region */
    iface::MemoryRegion m_modelBuffer{};   /**< Buffer where model is hosted */

    std::vector<std::shared_ptr<iface::TensorIface>>
        m_input{}; /**< Model's input tensor pointers. */
    std::vector<std::shared_ptr<iface::TensorIface>>
        m_output{}; /**< Model's output tensor pointers. */

    std::vector<std::pair<executorch::aten::TensorImpl, size_t>>
        m_inputTensorImplMap{}; /**< ExecuTorch's tensor implementation vector mapping to index of
                                     inputs */
    std::vector<executorch::aten::Tensor>
        m_outputTensor{}; /**< ExecuTorch output tensor (results are read from this space) */

    iface::TensorType m_type{iface::TensorType::INVALID}; /**< Model's data type. */
};
} /* namespace arm::app::fwk::et */

#endif /* MLEK_ET_MODEL_HPP */
