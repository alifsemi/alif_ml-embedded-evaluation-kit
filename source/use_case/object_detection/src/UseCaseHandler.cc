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
#include "UseCaseHandler.hpp"
#include "InputFiles.hpp"
#include "YoloFastestModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DetectionUseCaseUtils.hpp"
#include "DetectorPostProcessing.hpp"
#include "hal.h"

#include <inttypes.h>


/* used for presentation, original images are read-only"*/
static uint8_t g_image_buffer[INPUT_IMAGE_WIDTH*INPUT_IMAGE_HEIGHT*FORMAT_MULTIPLY_FACTOR] IFM_BUF_ATTRIBUTE = {}; 

namespace arm {
namespace app {


    /* Object detection classification handler. */
    bool ObjectDetectionHandler(ApplicationContext& ctx, uint32_t imgIndex, bool runAll)
    {
        auto& platform = ctx.Get<hal_platform&>("platform");
        auto& profiler = ctx.Get<Profiler&>("profiler");

        constexpr uint32_t dataPsnImgDownscaleFactor = 1;
        constexpr uint32_t dataPsnImgStartX = 10;
        constexpr uint32_t dataPsnImgStartY = 35;

        constexpr uint32_t dataPsnTxtInfStartX = 150;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        platform.data_psn->clear(COLOR_BLACK);

        auto& model = ctx.Get<Model&>("model");
        
        /* If the request has a valid size, set the image index. */
        if (imgIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, imgIndex, "imgIndex")) {
                return false;
            }
        }
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto curImIdx = ctx.Get<uint32_t>("imgIndex");

        TfLiteTensor* inputTensor = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 3) {
            printf_err("Input tensor dimension should be >= 3\n");
            return false;
        }

        TfLiteIntArray* inputShape = model.GetInputShape(0);

        const uint32_t nCols = inputShape->data[arm::app::YoloFastestModel::ms_inputColsIdx];
        const uint32_t nRows = inputShape->data[arm::app::YoloFastestModel::ms_inputRowsIdx];
        const uint32_t nPresentationChannels = FORMAT_MULTIPLY_FACTOR;

        std::vector<DetectionResult> results;

        do {
            /* Strings for presentation/logging. */
            std::string str_inf{"Running inference... "};

            const uint8_t* curr_image = get_img_array(ctx.Get<uint32_t>("imgIndex"));

            /* Copy over the data  and convert to gryscale */
#if DISPLAY_RGB_IMAGE
            memcpy(g_image_buffer,curr_image, INPUT_IMAGE_WIDTH*INPUT_IMAGE_HEIGHT*FORMAT_MULTIPLY_FACTOR);
#else 
            RgbToGrayscale(curr_image,g_image_buffer,INPUT_IMAGE_WIDTH,INPUT_IMAGE_HEIGHT);
#endif /*DISPLAY_RGB_IMAGE*/
            
            RgbToGrayscale(curr_image,inputTensor->data.uint8,INPUT_IMAGE_WIDTH,INPUT_IMAGE_HEIGHT);


            /* Display this image on the LCD. */
            platform.data_psn->present_data_image(
                g_image_buffer,
                nCols, nRows, nPresentationChannels,
                dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

            /* If the data is signed. */
            if (model.IsDataSigned()) {
                image::ConvertImgToInt8(inputTensor->data.data, inputTensor->bytes);
            }

            /* Display message on the LCD - inference running. */
            platform.data_psn->present_data_text(str_inf.c_str(), str_inf.size(),
                                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            /* Run inference over this image. */
            info("Running inference on image %" PRIu32 " => %s\n", ctx.Get<uint32_t>("imgIndex"),
                get_filename(ctx.Get<uint32_t>("imgIndex")));

            if (!RunInference(model, profiler)) {
                return false;
            }

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            platform.data_psn->present_data_text(str_inf.c_str(), str_inf.size(),
                                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            /* Detector post-processing*/
            TfLiteTensor* output_arr[2] = {nullptr,nullptr};
            output_arr[0] = model.GetOutputTensor(0);
            output_arr[1] = model.GetOutputTensor(1);
            RunPostProcessing(g_image_buffer,output_arr,results);

            platform.data_psn->present_data_image(
                g_image_buffer,
                nCols, nRows, nPresentationChannels,
                dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

            /*Detector post-processing*/


            /* Add results to context for access outside handler. */
            ctx.Set<std::vector<DetectionResult>>("results", results);

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */

            if (!image::PresentInferenceResult(platform, results)) {
                return false;
            }

            profiler.PrintProfilingResult();

            IncrementAppCtxIfmIdx(ctx,"imgIndex");

        } while (runAll && ctx.Get<uint32_t>("imgIndex") != curImIdx);

        return true;
    }

} /* namespace app */
} /* namespace arm */
