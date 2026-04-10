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

#ifndef MLEK_TFLM_TENSOR_HPP
#define MLEK_TFLM_TENSOR_HPP

#include "mlek/fwk/iface/Tensor.hpp"
#include "mlek/fwk/tflm/TensorFlowLiteMicro.hpp"
#include <vector>

namespace arm::app::fwk::tflm {

class TflmTensor : public iface::TensorIface {
public:
    explicit TflmTensor(TfLiteTensor* tensor); /**< Constructor */
    ~TflmTensor() override = default;          /** Destructor */
    void* GetData() override;
    size_t Bytes() override;
    size_t GetNumElements() override;
    iface::TensorType Type() override;
    iface::TensorLayout Layout() override;
    std::vector<size_t> Shape() override;
    iface::QuantParams GetQuantParams() override;

private:
    TfLiteTensor* m_tensor{nullptr};
};

} // namespace arm::app::fwk::tflm

#endif /* MLEK_TFLM_TENSOR_HPP */
