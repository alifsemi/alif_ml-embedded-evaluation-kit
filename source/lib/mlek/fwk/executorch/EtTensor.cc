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

#include "mlek/fwk/executorch/EtTensor.hpp"

namespace arm::app::fwk::et {

EtTensor::EtTensor(executorch::aten::TensorImpl* tensorImpl) : m_tensor(tensorImpl) {}

EtTensor::EtTensor(executorch::aten::Tensor& tensor) : m_tensor(tensor) {}

void* EtTensor::GetData()
{
    return this->m_tensor.mutable_data_ptr<void>();
}

size_t EtTensor::Bytes()
{
    return this->m_tensor.nbytes();
}

size_t EtTensor::GetNumElements()
{
    return this->m_tensor.numel();
}

std::vector<size_t> EtTensor::Shape()
{
    auto sz = this->m_tensor.sizes();
    return std::vector<size_t>(sz.begin(), sz.end());
}

iface::TensorType EtTensor::Type()
{
    switch (this->m_tensor.dtype()) {
    case executorch::aten::ScalarType::QInt8:
        return iface::TensorType::INT8;
    case executorch::aten::ScalarType::QUInt8:
        return iface::TensorType::UINT8;
    case executorch::aten::ScalarType::Int:
        return iface::TensorType::INT32;
    case executorch::aten::ScalarType::Float:
        return iface::TensorType::FP32;
    default:
        return iface::TensorType::INVALID;
    }
}

iface::TensorLayout EtTensor::Layout()
{
    return iface::TensorLayout::NCHW;
}

iface::QuantParams EtTensor::GetQuantParams()
{
    iface::QuantParams params{0.f, 0};
    /**
     * TODO: work out how to get quant params from ExecuTorch Tensor.
     */
    return params;
}

} // namespace arm::app::fwk::et
