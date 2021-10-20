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
#include "UseCaseHandler.hpp"
#include "VisualWakeWordModel.hpp"
#include "Classifier.hpp"
#include "InputFiles.hpp"
#include "UseCaseCommonUtils.hpp"
#include "hal.h"

#include <algorithm>

namespace arm {
namespace app {

    /**
    * @brief            Helper function to load the current image into the input
    *                   tensor.
    * @param[in]        imIdx         Image index (from the pool of images available
    *                                 to the application).
    * @param[out]       inputTensor   Pointer to the input tensor to be populated.
    * @return           true if tensor is loaded, false otherwise.
    **/
    static bool LoadImageIntoTensor(uint32_t imIdx,
                                     TfLiteTensor *inputTensor);

    /* Image inference classification handler. */
    bool ClassifyImageHandler(ApplicationContext &ctx, uint32_t imgIndex, bool runAll)
    {
        auto& platform = ctx.Get<hal_platform &>("platform");
        auto& profiler = ctx.Get<Profiler&>("profiler");

        constexpr uint32_t dataPsnImgDownscaleFactor = 1;
        constexpr uint32_t dataPsnImgStartX = 10;
        constexpr uint32_t dataPsnImgStartY = 35;

        constexpr uint32_t dataPsnTxtInfStartX = 150;
        constexpr uint32_t dataPsnTxtInfStartY = 70;


        platform.data_psn->clear(COLOR_BLACK);
        time_t infTimeMs = 0;

        auto& model = ctx.Get<Model&>("model");

        /* If the request has a valid size, set the image index. */
        if (imgIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, imgIndex,"imgIndex")) {
                return false;
            }
        }
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto curImIdx = ctx.Get<uint32_t>("imgIndex");

        TfLiteTensor *outputTensor = model.GetOutputTensor(0);
        TfLiteTensor *inputTensor = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 3) {
            printf_err("Input tensor dimension should be >= 3\n");
            return false;
        }
        TfLiteIntArray* inputShape = model.GetInputShape(0);
        const uint32_t nCols = inputShape->data[2];
        const uint32_t nRows = inputShape->data[1];
        const uint32_t nChannels = (inputShape->size == 4) ? inputShape->data[3] : 1;

        std::vector<ClassificationResult> results;

        do {

            /* Strings for presentation/logging. */
            std::string str_inf{"Running inference... "};

            /* Copy over the data. */
            LoadImageIntoTensor(ctx.Get<uint32_t>("imgIndex"), inputTensor);

            /* Display this image on the LCD. */
            platform.data_psn->present_data_image(
                static_cast<uint8_t *>(inputTensor->data.data),
                nCols, nRows, nChannels,
                dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

            /* Vww model preprocessing is image conversion from uint8 to [0,1] float values,
             * then quantize them with input quantization info. */
            QuantParams inQuantParams = GetTensorQuantParams(inputTensor);

            auto* req_data = static_cast<uint8_t *>(inputTensor->data.data);
            auto* signed_req_data = static_cast<int8_t *>(inputTensor->data.data);
            for (size_t i = 0; i < inputTensor->bytes; i++) {
                auto i_data_int8 = static_cast<int8_t>(((static_cast<float>(req_data[i]) / 255.0f) / inQuantParams.scale) + inQuantParams.offset);
                signed_req_data[i] = std::min<int8_t>(INT8_MAX, std::max<int8_t>(i_data_int8, INT8_MIN));
            }

            /* Display message on the LCD - inference running. */
            platform.data_psn->present_data_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            /* Run inference over this image. */
            info("Running inference on image %" PRIu32 " => %s\n", ctx.Get<uint32_t>("imgIndex"),
                get_filename(ctx.Get<uint32_t>("imgIndex")));

            if (!RunInference(model, profiler)) {
                return false;
            }

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            platform.data_psn->present_data_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            auto& classifier = ctx.Get<Classifier&>("classifier");
            classifier.GetClassificationResults(outputTensor, results,
                                                ctx.Get<std::vector <std::string>&>("labels"), 1);

            /* Add results to context for access outside handler. */
            ctx.Set<std::vector<ClassificationResult>>("results", results);

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */

            if (!image::PresentInferenceResult(platform, results, infTimeMs)) {
                return false;
            }

            profiler.PrintProfilingResult();
            IncrementAppCtxIfmIdx(ctx,"imgIndex");

        } while (runAll && ctx.Get<uint32_t>("imgIndex") != curImIdx);

        return true;
    }

    static bool LoadImageIntoTensor(const uint32_t imIdx,
                                     TfLiteTensor *inputTensor)
    {
        const size_t copySz = inputTensor->bytes < IMAGE_DATA_SIZE ?
                                inputTensor->bytes : IMAGE_DATA_SIZE;
        if (imIdx >= NUMBER_OF_FILES) {
            printf_err("invalid image index %" PRIu32 " (max: %u)\n", imIdx,
                       NUMBER_OF_FILES - 1);
            return false;
        }

        const uint32_t nChannels = (inputTensor->dims->size == 4) ? inputTensor->dims->data[3] : 1;

        const uint8_t* srcPtr = get_img_array(imIdx);
        auto* dstPtr = static_cast<uint8_t *>(inputTensor->data.data);
        if (1 == nChannels) {
            /**
             * Visual Wake Word model accepts only one channel =>
             * Convert image to grayscale here
             **/
            for (size_t i = 0; i < copySz; ++i, srcPtr += 3) {
                *dstPtr++ = 0.2989*(*srcPtr) +
                            0.587*(*(srcPtr+1)) +
                            0.114*(*(srcPtr+2));
            }
        } else {
            memcpy(inputTensor->data.data, srcPtr, copySz);
        }

        debug("Image %" PRIu32 " loaded\n", imIdx);
        return true;
    }

} /* namespace app */
} /* namespace arm */   