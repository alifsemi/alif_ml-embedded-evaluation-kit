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

#include "mlek/common/ImageUtils.hpp"
#include "mlek/fwk/tflm/TensorFlowLiteMicro.hpp"
#include "TestData_vww.hpp"
#include "mlek/fwk/tflm/VisualWakeWordModel.hpp"

#include <catch.hpp>

bool RunInference(arm::app::fwk::iface::Model& model, const int8_t* imageData)
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
void TestInference(int imageIdx, arm::app::fwk::iface::Model& model)
{
    auto image    = reinterpret_cast<const int8_t *>(test::GetIfmDataArray(imageIdx));
    auto goldenFV = reinterpret_cast<const int8_t *>(test::GetOfmDataArray(imageIdx));

    REQUIRE(RunInference(model, image));

    auto outputTensor = model.GetOutputTensor(0);

    REQUIRE(outputTensor);
    REQUIRE(outputTensor->Bytes() == OFM_0_DATA_SIZE);
    auto tensorData = tflite::GetTensorData<T>(outputTensor);
    REQUIRE(tensorData);

    for (size_t i = 0; i < outputTensor->Bytes(); i++) {
        auto testVal   = static_cast<int>(tensorData[i]);
        auto goldenVal = static_cast<int>(goldenFV[i]);
        CHECK(testVal == goldenVal);
    }
}
