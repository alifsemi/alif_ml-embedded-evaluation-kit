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

#include "mlek/fwk/tflm/TflmTensor.hpp"
#include <cassert>

namespace arm::app::fwk::tflm {

TflmTensor::TflmTensor(TfLiteTensor* tensor) : m_tensor(tensor) {}

void* TflmTensor::GetData()
{
    assert(this->m_tensor);
    return this->m_tensor->data.data;
}

size_t TflmTensor::Bytes()
{
    assert(this->m_tensor);
    return this->m_tensor->bytes;
}

size_t TflmTensor::GetNumElements()
{
    assert(this->m_tensor);
    const auto shape = this->Shape();
    size_t nElements = 1;
    for (auto& dim : shape) {
        nElements *= dim;
    }
    return nElements;
}

std::vector<size_t> TflmTensor::Shape()
{
    assert(this->m_tensor);
    assert(this->m_tensor->dims);
    return std::vector<size_t>(this->m_tensor->dims->data,
                               this->m_tensor->dims->data + this->m_tensor->dims->size);
}

iface::TensorType TflmTensor::Type()
{
    assert(this->m_tensor);

    switch (this->m_tensor->type) {
    case kTfLiteUInt8:
        return iface::TensorType::UINT8;
    case kTfLiteInt8:
        return iface::TensorType::INT8;
    case kTfLiteInt16:
        return iface::TensorType::INT16;
    case kTfLiteInt32:
        return iface::TensorType::INT32;
    case kTfLiteFloat16:
        return iface::TensorType::FP16;
    case kTfLiteFloat32:
        return iface::TensorType::FP32;
    default:
        return iface::TensorType::INVALID;
    }
}

iface::TensorLayout TflmTensor::Layout()
{
    return iface::TensorLayout::NHWC;
}

iface::QuantParams TflmTensor::GetQuantParams()
{
    iface::QuantParams params{0.f, 0};
    assert(this->m_tensor);
    if (kTfLiteAffineQuantization == this->m_tensor->quantization.type) {
        auto* quantParams =
            reinterpret_cast<TfLiteAffineQuantization*>(this->m_tensor->quantization.params);
        if (quantParams && 0 == quantParams->quantized_dimension) {
            if (quantParams->scale->size) {
                params.scale = quantParams->scale->data[0];
            }
            if (quantParams->zero_point->size) {
                params.offset = quantParams->zero_point->data[0];
            }
        } else if (this->m_tensor->params.scale != 0.0) {
            /* Legacy tensorflow quantisation parameters */
            params.scale  = this->m_tensor->params.scale;
            params.offset = this->m_tensor->params.zero_point;
        }
    }
    return params;
}

} // namespace arm::app::fwk::tflm
