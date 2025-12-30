/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or
 * its affiliates <open-source-office@arm.com>
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

#ifndef MLEK_ET_TENSOR_HPP
#define MLEK_ET_TENSOR_HPP

#include "mlek/fwk/executorch/ExecuTorch.hpp"
#include "mlek/fwk/iface/Tensor.hpp"
#include <cstdint>
#include <vector>

namespace arm::app::fwk::et {

class EtTensor : public iface::TensorIface {
public:
    explicit EtTensor(executorch::aten::TensorImpl* tensorImpl); /**< Constructor */
    explicit EtTensor(executorch::aten::Tensor& tensor);         /** Constructor */
    ~EtTensor() override = default;                              /** Destructor */
    void* GetData() override;
    size_t Bytes() override;
    size_t GetNumElements() override;
    iface::TensorType Type() override;
    iface::TensorLayout Layout() override;
    std::vector<size_t> Shape() override;
    iface::QuantParams GetQuantParams() override;

private:
    executorch::aten::Tensor m_tensor{nullptr};
};

} // namespace arm::app::fwk::et

#endif /* MLEK_ET_TENSOR_HPP */
