/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#include "log_macros.h"
#include "ImageUtils.hpp"
#include "YoloFastestModel.hpp"
#include "TensorFlowLiteMicro.hpp"
#include "DetectorPostProcessing.hpp"
#include "InputFiles.hpp"
#include "UseCaseCommonUtils.hpp"

namespace arm {
    namespace app {
        static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
        namespace object_detection {
            extern uint8_t* GetModelPointer();
            extern size_t GetModelLen();
        } /* namespace object_detection */
    } /* namespace app */
} /* namespace arm */

#include <catch.hpp>

void GetExpectedResults(std::vector<std::vector<arm::app::object_detection::DetectionResult>> &expected_results)
{
    /* Img1
    0)  (0.999246) -> Detection box: {x=89,y=17,w=41,h=56}
    1)  (0.995367) -> Detection box: {x=27,y=81,w=48,h=53}
    */
    expected_results.push_back({
        arm::app::object_detection::DetectionResult(0.99,89,17,41,56),
        arm::app::object_detection::DetectionResult(0.99,27,81,48,53)
    });
    /* Img2
    0)  (0.998107) -> Detection box: {x=87,y=35,w=53,h=64}
    */
    expected_results.push_back({
        arm::app::object_detection::DetectionResult(0.99,87,35,53,64)
    });
    /* Img3
    0)  (0.999244) -> Detection box: {x=105,y=73,w=58,h=66}
    1)  (0.985984) -> Detection box: {x=34,y=40,w=70,h=95}
    */
    expected_results.push_back({
        arm::app::object_detection::DetectionResult(0.99,105,73,58,66),
        arm::app::object_detection::DetectionResult(0.98,34,40,70,95)
    });
    /* Img4
    0)  (0.993294) -> Detection box: {x=22,y=43,w=39,h=53}
    1)  (0.992021) -> Detection box: {x=63,y=60,w=38,h=45}
    */
    expected_results.push_back({
        arm::app::object_detection::DetectionResult(0.99,22,43,39,53),
        arm::app::object_detection::DetectionResult(0.99,63,60,38,45)
    });
}

bool RunInference(arm::app::Model& model, const uint8_t imageData[])
{
    TfLiteTensor* inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    const size_t copySz = inputTensor->bytes < IMAGE_DATA_SIZE ?
                            inputTensor->bytes : IMAGE_DATA_SIZE;

    arm::app::image::RgbToGrayscale(imageData,inputTensor->data.uint8,copySz);

    if(model.IsDataSigned()){
        arm::app::image::ConvertImgToInt8(inputTensor->data.data, copySz);
    }

    return model.RunInference();
}

template<typename T>
void TestInferenceDetectionResults(int imageIdx, arm::app::Model& model, T tolerance) {

    std::vector<arm::app::object_detection::DetectionResult> results;
    auto image = get_img_array(imageIdx);

    TfLiteIntArray* inputShape = model.GetInputShape(0);
    auto nCols = inputShape->data[arm::app::YoloFastestModel::ms_inputColsIdx];
    auto nRows = inputShape->data[arm::app::YoloFastestModel::ms_inputRowsIdx];

    REQUIRE(RunInference(model, image));


    std::vector<TfLiteTensor*> output_arr{model.GetOutputTensor(0), model.GetOutputTensor(1)};
    for (size_t i =0; i < output_arr.size(); i++) {
        REQUIRE(output_arr[i]);
        REQUIRE(tflite::GetTensorData<T>(output_arr[i]));
    }

    const arm::app::object_detection::PostProcessParams postProcessParams {
            nRows, nCols, arm::app::object_detection::originalImageSize,
            arm::app::object_detection::anchor1, arm::app::object_detection::anchor2
    };
    arm::app::DetectorPostProcess postp{output_arr[0], output_arr[1], results, postProcessParams};
    postp.DoPostProcess();

    std::vector<std::vector<arm::app::object_detection::DetectionResult>> expected_results;
    GetExpectedResults(expected_results);

    /* Validate got the same number of boxes */
    REQUIRE(results.size() == expected_results[imageIdx].size());


    for (int i=0; i < (int)results.size(); i++) {
        /* Validate confidence and box dimensions */
        REQUIRE(std::abs(results[i].m_normalisedVal - expected_results[imageIdx][i].m_normalisedVal) < 0.1);
        REQUIRE(static_cast<int>(results[i].m_x0) == Approx(static_cast<int>((T)expected_results[imageIdx][i].m_x0)).epsilon(tolerance));
        REQUIRE(static_cast<int>(results[i].m_y0) == Approx(static_cast<int>((T)expected_results[imageIdx][i].m_y0)).epsilon(tolerance));
        REQUIRE(static_cast<int>(results[i].m_w) == Approx(static_cast<int>((T)expected_results[imageIdx][i].m_w)).epsilon(tolerance));
        REQUIRE(static_cast<int>(results[i].m_h) == Approx(static_cast<int>((T)expected_results[imageIdx][i].m_h)).epsilon(tolerance));
    }
}


TEST_CASE("Running inference with TensorFlow Lite Micro and YoloFastest", "[YoloFastest]")
{
    SECTION("Executing inferences sequentially")
    {
        arm::app::YoloFastestModel model{};

        REQUIRE_FALSE(model.IsInited());
        REQUIRE(model.Init(arm::app::tensorArena,
                           sizeof(arm::app::tensorArena),
                           arm::app::object_detection::GetModelPointer(),
                           arm::app::object_detection::GetModelLen()));
        REQUIRE(model.IsInited());

        for (uint32_t i = 0 ; i < NUMBER_OF_FILES; ++i) {
            TestInferenceDetectionResults<uint8_t>(i, model, 1);
        }
    }

    for (uint32_t i = 0 ; i < NUMBER_OF_FILES; ++i) {
        DYNAMIC_SECTION("Executing inference with re-init")
        {
            arm::app::YoloFastestModel model{};

            REQUIRE_FALSE(model.IsInited());
            REQUIRE(model.Init(arm::app::tensorArena,
                               sizeof(arm::app::tensorArena),
                               arm::app::object_detection::GetModelPointer(),
                               arm::app::object_detection::GetModelLen()));
            REQUIRE(model.IsInited());

            TestInferenceDetectionResults<uint8_t>(i, model, 1);
        }
    }
}
