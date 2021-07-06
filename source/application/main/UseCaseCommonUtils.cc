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
#include "UseCaseCommonUtils.hpp"

#include "InputFiles.hpp"

#include <inttypes.h>

namespace arm {
namespace app {

    bool RunInference(arm::app::Model& model, Profiler& profiler)
    {
        profiler.StartProfiling("Inference");
        bool runInf = model.RunInference();
        profiler.StopProfiling();

        return runInf;
    }

    int ReadUserInputAsInt(hal_platform& platform)
    {
        char chInput[128];
        memset(chInput, 0, sizeof(chInput));

        platform.data_acq->get_input(chInput, sizeof(chInput));
        return atoi(chInput);
    }

    void DumpTensorData(const uint8_t* tensorData,
                        size_t size,
                        size_t lineBreakForNumElements)
        {
            char strhex[8];
            std::string strdump;

            for (size_t i = 0; i < size; ++i) {
                if (0 == i % lineBreakForNumElements) {
                    printf("%s\n\t", strdump.c_str());
                    strdump.clear();
                }
                snprintf(strhex, sizeof(strhex) - 1,
                         "0x%02x, ", tensorData[i]);
                strdump += std::string(strhex);
            }

            if (!strdump.empty()) {
                printf("%s\n", strdump.c_str());
            }
        }

    void DumpTensor(const TfLiteTensor* tensor, const size_t lineBreakForNumElements)
    {
        if (!tensor) {
            printf_err("invalid tensor\n");
            return;
        }

        const uint32_t tensorSz = tensor->bytes;
        const uint8_t* tensorData = tflite::GetTensorData<uint8_t>(tensor);

        DumpTensorData(tensorData, tensorSz, lineBreakForNumElements);
    }

    bool ListFilesHandler(ApplicationContext& ctx)
    {
        auto& model = ctx.Get<Model&>("model");
        auto& platform = ctx.Get<hal_platform&>("platform");

        constexpr uint32_t dataPsnTxtStartX = 20;
        constexpr uint32_t dataPsnTxtStartY = 40;

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        /* Clear the LCD */
        platform.data_psn->clear(COLOR_BLACK);

        /* Show the total number of embedded files. */
        std::string strNumFiles = std::string{"Total Number of Files: "} +
                                   std::to_string(NUMBER_OF_FILES);
        platform.data_psn->present_data_text(strNumFiles.c_str(),
                                             strNumFiles.size(),
                                             dataPsnTxtStartX,
                                             dataPsnTxtStartY,
                                             false);

#if NUMBER_OF_FILES > 0
        constexpr uint32_t dataPsnTxtYIncr = 16;
        info("List of Files:\n");
        uint32_t yVal = dataPsnTxtStartY + dataPsnTxtYIncr;
        for (uint32_t i = 0; i < NUMBER_OF_FILES; ++i, yVal += dataPsnTxtYIncr) {

            std::string currentFilename{get_filename(i)};
            platform.data_psn->present_data_text(currentFilename.c_str(),
                                                 currentFilename.size(),
                                                 dataPsnTxtStartX, yVal, false);

            info("\t%" PRIu32 " => %s\n", i, currentFilename.c_str());
        }
#endif /* NUMBER_OF_FILES > 0 */

        return true;
    }

} /* namespace app */
} /* namespace arm */