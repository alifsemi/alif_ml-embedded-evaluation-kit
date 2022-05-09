/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
#include "ImageUtils.hpp"
#include "MobileNetModel.hpp"
#include "TensorFlowLiteMicro.hpp"
#include "TestData_img_class.hpp"
#include "BufAttributes.hpp"

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

bool RunInference(arm::app::Model& model, const int8_t imageData[])
{
    TfLiteTensor* inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    const size_t copySz = inputTensor->bytes < IFM_0_DATA_SIZE ?
                            inputTensor->bytes :
                            IFM_0_DATA_SIZE;
    memcpy(inputTensor->data.data, imageData, copySz);

    if(model.IsDataSigned()){
        arm::app::image::ConvertImgToInt8(inputTensor->data.data, copySz);
    }

    return model.RunInference();
}

template<typename T>
void TestInference(int imageIdx, arm::app::Model& model, T tolerance) {
    auto image = get_ifm_data_array(imageIdx);
    auto goldenFV = get_ofm_data_array(imageIdx);

    REQUIRE(RunInference(model, image));

    TfLiteTensor* outputTensor = model.GetOutputTensor(0);

    REQUIRE(outputTensor);
    REQUIRE(outputTensor->bytes == OFM_0_DATA_SIZE);
    auto tensorData = tflite::GetTensorData<T>(outputTensor);
    REQUIRE(tensorData);

    for (size_t i = 0; i < outputTensor->bytes; i++) {
        REQUIRE(static_cast<int>(tensorData[i]) == Approx(static_cast<int>((T)goldenFV[i])).epsilon(tolerance));
    }
}


TEST_CASE("Running inference with TensorFlow Lite Micro and MobileNeV2 Uint8", "[MobileNetV2]")
{
    SECTION("Executing inferences sequentially")
    {
        arm::app::MobileNetModel model{};

        REQUIRE_FALSE(model.IsInited());
        REQUIRE(model.Init(arm::app::tensorArena,
                           sizeof(arm::app::tensorArena),
                           arm::app::img_class::GetModelPointer(),
                           arm::app::img_class::GetModelLen()));
        REQUIRE(model.IsInited());

        for (uint32_t i = 0 ; i < NUMBER_OF_IFM_FILES; ++i) {
            TestInference<uint8_t>(i, model, 1);
        }
    }

    for (uint32_t i = 0 ; i < NUMBER_OF_IFM_FILES; ++i) {
        DYNAMIC_SECTION("Executing inference with re-init")
        {
            arm::app::MobileNetModel model{};

            REQUIRE_FALSE(model.IsInited());
            REQUIRE(model.Init(arm::app::tensorArena,
                               sizeof(arm::app::tensorArena),
                               arm::app::img_class::GetModelPointer(),
                               arm::app::img_class::GetModelLen()));
            REQUIRE(model.IsInited());

            TestInference<uint8_t>(i, model, 1);
        }
    }
}
