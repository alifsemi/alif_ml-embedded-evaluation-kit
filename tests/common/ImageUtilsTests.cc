/*
 * SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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

#include "mlek/common/ImageUtils.hpp"

#include <array>
#include <cstdint>

#include <catch.hpp>

namespace {

using arm::app::fwk::iface::QuantParams;
using arm::app::fwk::iface::TensorLayout;

} // namespace

TEST_CASE("ConvertUint8ToInt8 uses centered conversion by default")
{
    const std::array<uint8_t, 3> src{0, 128, 255};
    std::array<int8_t, 3> dst{};

    arm::app::image::ConvertUint8ToInt8(dst.data(), src.data(), src.size(), TensorLayout::NHWC);

    REQUIRE(dst == std::array<int8_t, 3>{-128, 0, 127});
}

TEST_CASE("ConvertUint8ToInt8 quantizes [0,1] range")
{
    const std::array<uint8_t, 3> src{0, 128, 255};
    std::array<int8_t, 3> dst{};
    const QuantParams quantParams{1.0F / 255.0F, -128};

    arm::app::image::ConvertUint8ToInt8(
        dst.data(), src.data(), src.size(), TensorLayout::NHWC, quantParams);

    REQUIRE(dst == std::array<int8_t, 3>{-128, 0, 127});
}

TEST_CASE("ConvertUint8ToInt8 uses quantization for scale 1/255")
{
    const std::array<uint8_t, 5> src{0, 1, 127, 128, 255};
    std::array<int8_t, 5> dst{};
    const QuantParams quantParams{1.0F / 255.0F, -128};

    arm::app::image::ConvertUint8ToInt8(
        dst.data(), src.data(), src.size(), TensorLayout::NHWC, quantParams);

    REQUIRE(dst == std::array<int8_t, 5>{-128, -127, -1, 0, 127});
}

TEST_CASE("ConvertUint8ToInt8 quantizes centered [-1,1] range")
{
    const std::array<uint8_t, 7> src{0, 2, 4, 127, 128, 254, 255};
    std::array<int8_t, 7> dst{};
    const QuantParams quantParams{2.0F / 255.0F, -1};

    arm::app::image::ConvertUint8ToInt8(
        dst.data(), src.data(), src.size(), TensorLayout::NHWC, quantParams);

    REQUIRE(dst == std::array<int8_t, 7>{-128, -126, -124, -1, 0, 126, 127});
}

TEST_CASE("ConvertUint8ToInt8 saturates quantized values")
{
    const std::array<uint8_t, 3> src{0, 128, 255};
    std::array<int8_t, 3> dst{};
    const QuantParams quantParams{1.0F / 255.0F, -1};

    arm::app::image::ConvertUint8ToInt8(
        dst.data(), src.data(), src.size(), TensorLayout::NHWC, quantParams);

    REQUIRE(dst == std::array<int8_t, 3>{-1, 127, 127});
}
