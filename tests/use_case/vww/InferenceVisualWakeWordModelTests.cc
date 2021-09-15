/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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

#include <catch.hpp>
#include <random>
#include "hal.h"
#include "InputFiles.hpp"
#include "ImageUtils.hpp"
#include "TestData_vww.hpp"
#include "VisualWakeWordModel.hpp"
#include "TensorFlowLiteMicro.hpp"


bool RunInference(arm::app::Model& model, const int8_t* imageData)
{
    TfLiteTensor* inputTensor = model.GetInputTensor(0);
    REQUIRE(inputTensor);

    return model.RunInference();
}

template<typename T>
void TestInference(int imageIdx,arm::app::Model& model) {

    auto image = test::get_ifm_data_array(imageIdx);
    auto goldenFV = test::get_ofm_data_array(imageIdx);

    REQUIRE(RunInference(model, image));

    TfLiteTensor* outputTensor = model.GetOutputTensor(0);

    REQUIRE(outputTensor);
    REQUIRE(outputTensor->bytes == OFM_DATA_SIZE);
    auto tensorData = tflite::GetTensorData<T>(outputTensor);
    REQUIRE(tensorData);

    for (size_t i = 0; i < outputTensor->bytes; i++) {
        auto testVal = static_cast<int>(tensorData[i]);
        auto goldenVal = static_cast<int>(goldenFV[i]);
        CHECK(testVal == goldenVal);
    }
}


/**
 * @brief       Given an image name, get its index
 * @param[in]   imageName   Name of the image expected
 * @return      index of the image if valid and (-1) if not found
 */
static int _GetImageIdx(std::string &imageName)
{
    int imgIdx = -1;
    for (uint32_t i = 0 ; i < NUMBER_OF_FILES; ++i) {
        if (imageName == std::string(get_filename(i))) {
            info("Image %s exists at index %u\n", get_filename(i), i);
            imgIdx = static_cast<int>(i);
            break;
        }
    }

    if (-1 == imgIdx) {
        warn("Image %s not found!\n", imageName.c_str());
    }

    return imgIdx;
}

