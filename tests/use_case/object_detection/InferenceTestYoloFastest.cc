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
#include "hal.h"
#include "ImageUtils.hpp"
#include "YoloFastestModel.hpp"
#include "TensorFlowLiteMicro.hpp"
#include "DetectorPostProcessing.hpp"
#include "InputFiles.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DetectionUseCaseUtils.hpp"
#include "ExpectedObjectDetectionResults.hpp"

#include <catch.hpp>


bool RunInference(arm::app::Model& model, const uint8_t imageData[])
{
    TfLiteTensor* inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    const size_t copySz = inputTensor->bytes < (INPUT_IMAGE_WIDTH*INPUT_IMAGE_HEIGHT) ?
                            inputTensor->bytes :
                            (INPUT_IMAGE_WIDTH*INPUT_IMAGE_HEIGHT);

    arm::app::RgbToGrayscale(imageData,inputTensor->data.uint8,INPUT_IMAGE_WIDTH,INPUT_IMAGE_HEIGHT);            

    if(model.IsDataSigned()){
        convertImgIoInt8(inputTensor->data.data, copySz);
    }

    return model.RunInference();
}

template<typename T>
void TestInference(int imageIdx, arm::app::Model& model, T tolerance) {

    info("Entering TestInference for image %d \n", imageIdx);

    std::vector<arm::app::DetectionResult> results;
    auto image = get_img_array(imageIdx);

    REQUIRE(RunInference(model, image));


    TfLiteTensor* output_arr[2] = {nullptr,nullptr};
    output_arr[0] = model.GetOutputTensor(0);
    output_arr[1] = model.GetOutputTensor(1);
    
    for (int i =0; i < 2; i++) {
        REQUIRE(output_arr[i]);    
        REQUIRE(tflite::GetTensorData<T>(output_arr[i]));
    }

    RunPostProcessing(NULL,output_arr,results);
    
    info("Got %ld boxes \n",results.size());
      
    std::vector<std::vector<arm::app::DetectionResult>> expected_results;
    get_expected_ut_results(expected_results);
    
    /*validate got the same number of boxes */
    REQUIRE(results.size() == expected_results[imageIdx].size());
    
    
    for (int i=0; i < (int)results.size(); i++) {
    
        info("%" PRIu32 ")  (%f) -> %s {x=%d,y=%d,w=%d,h=%d}\n", (int)i, 
                 results[i].m_normalisedVal, "Detection box:",
               results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h );

        /*validate confidence and box dimensions */
        REQUIRE(fabs(results[i].m_normalisedVal - expected_results[imageIdx][i].m_normalisedVal) < 0.1);
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
        REQUIRE(model.Init());
        REQUIRE(model.IsInited());

        for (uint32_t i = 0 ; i < NUMBER_OF_FILES; ++i) {
            TestInference<uint8_t>(i, model, 1);
        }
    }

    for (uint32_t i = 0 ; i < NUMBER_OF_FILES; ++i) {
        DYNAMIC_SECTION("Executing inference with re-init")
        {
            arm::app::YoloFastestModel model{};

            REQUIRE_FALSE(model.IsInited());
            REQUIRE(model.Init());
            REQUIRE(model.IsInited());

            TestInference<uint8_t>(i, model, 1);
        }
    }
}
