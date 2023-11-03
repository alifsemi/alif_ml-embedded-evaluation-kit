/*
 * SPDX-FileCopyrightText: Copyright 2023 Arm Limited and/or its affiliates
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
#include "BufAttributes.hpp"
#include "TensorFlowLiteMicro.hpp"
#include "TestModel.hpp" /* Model class for running inference. */

#include <catch.hpp>
#include <random>

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace inference_runner {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace inference_runner */
} /* namespace app */
} /* namespace arm */

TEST_CASE("Testing Init failure due to insufficient tensor arena inf runner", "[inf runner]")
{
    arm::app::TestModel model{};
    REQUIRE_FALSE(model.IsInited());
    size_t insufficientTensorArenaSz = 1000;
    REQUIRE_FALSE(model.Init(arm::app::tensorArena,
                             insufficientTensorArenaSz,
                             arm::app::inference_runner::GetModelPointer(),
                             arm::app::inference_runner::GetModelLen()));
    REQUIRE_FALSE(model.IsInited());
}
