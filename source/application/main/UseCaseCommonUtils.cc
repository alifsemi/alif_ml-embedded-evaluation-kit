/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#include "ImageUtils.hpp"
#include "log_macros.h"

#include <cinttypes>

namespace arm {
namespace app {
    bool PresentInferenceResult(const std::vector<arm::app::ClassificationResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 150;
        constexpr uint32_t dataPsnTxtStartY1 = 30;

        constexpr uint32_t dataPsnTxtStartX2 = 10;
        constexpr uint32_t dataPsnTxtStartY2 = 150;

        constexpr uint32_t dataPsnTxtYIncr = 16; /* Row index increment. */

        hal_lcd_set_text_color(COLOR_GREEN);

        /* Display each result. */
        uint32_t rowIdx1 = dataPsnTxtStartY1 + 2 * dataPsnTxtYIncr;
        uint32_t rowIdx2 = dataPsnTxtStartY2;

        info("Final results:\n");
        info("Total number of inferences: 1\n");

        for (uint32_t i = 0; i < results.size(); ++i) {
            std::string resultStr = std::to_string(i + 1) + ") " +
                                    std::to_string(results[i].m_labelIdx) + " (" +
                                    std::to_string(results[i].m_normalisedVal) + ")";

            hal_lcd_display_text(
                resultStr.c_str(), resultStr.size(), dataPsnTxtStartX1, rowIdx1, false);
            rowIdx1 += dataPsnTxtYIncr;

            resultStr = std::to_string(i + 1) + ") " + results[i].m_label;
            hal_lcd_display_text(resultStr.c_str(), resultStr.size(), dataPsnTxtStartX2, rowIdx2, 0);
            rowIdx2 += dataPsnTxtYIncr;

            info("%" PRIu32 ") %" PRIu32 " (%f) -> %s\n",
                 i,
                 results[i].m_labelIdx,
                 results[i].m_normalisedVal,
                 results[i].m_label.c_str());
        }

        return true;
    }

    bool RunInference(arm::app::Model& model, Profiler& profiler)
    {
        profiler.StartProfiling("Inference");
        bool runInf = model.RunInference();
        profiler.StopProfiling();

        return runInf;
    }

    int ReadUserInputAsInt()
    {
        char chInput[128];
        memset(chInput, 0, sizeof(chInput));

        hal_get_user_input(chInput, sizeof(chInput));
        return atoi(chInput);
    }

    void DumpTensorData(const uint8_t* tensorData, size_t size, size_t lineBreakForNumElements)
    {
        char strhex[8];
        std::string strdump;

        for (size_t i = 0; i < size; ++i) {
            if (0 == i % lineBreakForNumElements) {
                printf("%s\n\t", strdump.c_str());
                strdump.clear();
            }
            snprintf(strhex, sizeof(strhex) - 1, "0x%02x, ", tensorData[i]);
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
        const auto* tensorData  = tflite::GetTensorData<uint8_t>(tensor);

        DumpTensorData(tensorData, tensorSz, lineBreakForNumElements);
    }
} /* namespace app */
} /* namespace arm */
