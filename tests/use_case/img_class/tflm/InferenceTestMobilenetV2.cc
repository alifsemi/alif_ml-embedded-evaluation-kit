/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024-2025 Arm Limited and/or its affiliates
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
#include "mlek/common/ImageUtils.hpp"
#include "mlek/fwk/tflm/MobileNetModel.hpp"
#include "mlek/fwk/tflm/TensorFlowLiteMicro.hpp"
#include "TestData_img_class.hpp"

#include <catch.hpp>

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace img_class {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace img_class */
} /* namespace app */
} /* namespace arm */

using namespace test;

bool RunInference(arm::app::fwk::iface::Model& model, const int8_t imageData[])
{
    auto inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    const size_t copySz =
        inputTensor->Bytes() < IFM_0_DATA_SIZE ? inputTensor->Bytes() : IFM_0_DATA_SIZE;
    memcpy(inputTensor->GetData(), imageData, copySz);

    if (model.IsDataSigned()) {
        arm::app::image::ConvertUint8ToInt8(inputTensor->GetData(), copySz);
    }

    return model.RunInference();
}

template <typename T>
void TestInference(int imageIdx, arm::app::fwk::iface::Model& model, T tolerance)
{
    auto image    = reinterpret_cast<const IFM_0_DATA_TYPE *>(GetIfmDataArray(imageIdx));
    auto goldenFV = reinterpret_cast<const OFM_0_DATA_TYPE *>(GetOfmDataArray(imageIdx));

    REQUIRE(RunInference(model, image));

    auto outputTensor = model.GetOutputTensor(0);

    REQUIRE(outputTensor);
    REQUIRE(outputTensor->Bytes() == OFM_0_DATA_SIZE);
    auto tensorData = outputTensor->GetData<T>();
    REQUIRE(tensorData);

    for (size_t i = 0; i < outputTensor->Bytes(); i++) {
        REQUIRE(static_cast<int>(tensorData[i]) ==
                Approx(static_cast<int>((T)goldenFV[i])).epsilon(tolerance));
    }
}

TEST_CASE("Running inference with TensorFlow Lite Micro and MobileNeV2 Uint8", "[MobileNetV2]")
{
    SECTION("Executing inferences sequentially")
    {
        arm::app::fwk::tflm::MobileNetModel model{};

        REQUIRE_FALSE(model.IsInited());

        arm::app::fwk::iface::MemoryRegion modelMem{arm::app::img_class::GetModelPointer(),
                                                    arm::app::img_class::GetModelLen()};
        arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                      sizeof(arm::app::tensorArena)};
        REQUIRE(model.Init(computeMem, modelMem));

        for (uint32_t i = 0; i < NUMBER_OF_IFM_FILES; ++i) {
            TestInference<uint8_t>(i, model, 1);
        }
    }

    for (uint32_t i = 0; i < NUMBER_OF_IFM_FILES; ++i) {
        DYNAMIC_SECTION("Executing inference with re-init")
        {
            arm::app::fwk::tflm::MobileNetModel model{};

            REQUIRE_FALSE(model.IsInited());
            arm::app::fwk::iface::MemoryRegion modelMem{arm::app::img_class::GetModelPointer(),
                                                        arm::app::img_class::GetModelLen()};
            arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                          sizeof(arm::app::tensorArena)};
            REQUIRE(model.Init(computeMem, modelMem));
            REQUIRE(model.IsInited());

            TestInference<uint8_t>(i, model, 1);
        }
    }
}
