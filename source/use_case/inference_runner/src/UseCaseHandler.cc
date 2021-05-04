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

#include "TestModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "hal.h"

#include <cstdlib>

namespace arm {
namespace app {

    bool RunInferenceHandler(ApplicationContext& ctx)
    {
        auto& platform = ctx.Get<hal_platform&>("platform");
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model = ctx.Get<Model&>("model");

        constexpr uint32_t dataPsnTxtInfStartX = 150;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        const size_t numInputs = model.GetNumInputs();

        /* Populate each input tensor with random data. */
        for (size_t inputIndex = 0; inputIndex < numInputs; inputIndex++) {

            TfLiteTensor* inputTensor = model.GetInputTensor(inputIndex);

            debug("Populating input tensor %zu@%p\n", inputIndex, inputTensor);
            debug("Total input size to be populated: %zu\n", inputTensor->bytes);

            /* Create a random input. */
            if (inputTensor->bytes > 0) {

                uint8_t* tData = tflite::GetTensorData<uint8_t>(inputTensor);

                for (size_t j = 0; j < inputTensor->bytes; ++j) {
                    tData[j] = static_cast<uint8_t>(std::rand() & 0xFF);
                }
            }
        }

        /* Strings for presentation/logging. */
        std::string str_inf{"Running inference... "};

        /* Display message on the LCD - inference running. */
        platform.data_psn->present_data_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

        if (!RunInference(model, profiler)) {
            return false;
        }

        /* Erase. */
        str_inf = std::string(str_inf.size(), ' ');
        platform.data_psn->present_data_text(
                                str_inf.c_str(), str_inf.size(),
                                dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

        info("Final results:\n");
        profiler.PrintProfilingResult();

#if VERIFY_TEST_OUTPUT
        for (size_t outputIndex = 0; outputIndex < model.GetNumOutputs(); outputIndex++) {
            arm::app::DumpTensor(model.GetOutputTensor(outputIndex));
        }
#endif /* VERIFY_TEST_OUTPUT */

        return true;
    }

} /* namespace app */
} /* namespace arm */
