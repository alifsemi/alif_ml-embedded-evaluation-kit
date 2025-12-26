/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates
 * <open-source-office@arm.com>
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
#include "mlek/log/log_macros.h"

#include <cstdint>
#include <catch.hpp>

template <typename T>
static void TestQuantizedTensorFunctions()
{
    static_assert(std::is_integral_v<T>, "T must be integral");

    std::vector<T> vectorT(10, 0);
    int dimArray[]        = {2 /* n dims */, 1 /* dim0 */, 10 /* dim1 */};
    constexpr float scale = 0.5;
    constexpr int offset  = 1;
    TfLiteIntArray* dims  = tflite::testing::IntArrayFromInts(dimArray);
    TfLiteTensor tfTensor = tflite::testing::CreateQuantizedTensor(vectorT.data(),
                                                                   dims,
                                                                   scale,
                                                                   offset);
    const auto tensor     = std::make_shared<arm::app::fwk::tflm::TflmTensor>(&tfTensor);

    REQUIRE(tensor);
    const auto shape      = tensor->Shape();
    CHECK(shape.size() == 2);
    CHECK(shape[0] == 1);
    CHECK(shape[1] == 10);
    const auto quant = tensor->GetQuantParams();
    CHECK(quant.scale == Approx(scale).epsilon(0.1));
    CHECK(quant.offset == offset);
    CHECK(tensor->GetNumElements() == 10);
    CHECK(tensor->Bytes() == 10 * sizeof(T));

    arm::app::fwk::iface::TensorType type;
    switch(sizeof(T)) {
        case 1:
            if (std::is_signed_v<T>) {
                type = arm::app::fwk::iface::TensorType::INT8; break;
            } else {
                type = arm::app::fwk::iface::TensorType::UINT8; break;
            }
        case 2: type = arm::app::fwk::iface::TensorType::INT16; break;
        default: type = arm::app::fwk::iface::TensorType::INVALID;
    }
    info("Tests for tensor type %s: OK\n", arm::app::fwk::iface::GetTensorDataTypeName(type));
    CHECK(tensor->Type() == type);
}

TEST_CASE("TensorFlow Lite Micro Tensor wrapper")
{
    TestQuantizedTensorFunctions<uint8_t>();
    TestQuantizedTensorFunctions<int8_t>();
    TestQuantizedTensorFunctions<int16_t>();
}
