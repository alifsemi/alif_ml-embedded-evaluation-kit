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
#include "log_macros.h"

#include <cstdlib>

namespace arm {
namespace app {

static void PopulateInputTensor(const Model& model)
{
    const size_t numInputs = model.GetNumInputs();

#if defined(DYNAMIC_IFM_BASE) && defined(DYNAMIC_IFM_SIZE)
    size_t curInputIdx = 0;
#endif /* defined(DYNAMIC_IFM_BASE) && defined(DYNAMIC_IFM_SIZE) */

    /* Populate each input tensor with random data. */
    for (size_t inputIndex = 0; inputIndex < numInputs; inputIndex++) {

        TfLiteTensor* inputTensor = model.GetInputTensor(inputIndex);

        debug("Populating input tensor %zu@%p\n", inputIndex, inputTensor);
        debug("Total input size to be populated: %zu\n", inputTensor->bytes);

        if (inputTensor->bytes > 0) {

            uint8_t* tData = tflite::GetTensorData<uint8_t>(inputTensor);

#if defined(DYNAMIC_IFM_BASE) && defined(DYNAMIC_IFM_SIZE)
            if (curInputIdx + inputTensor->bytes > DYNAMIC_IFM_SIZE) {
                printf_err("IFM reserved buffer size insufficient\n");
                return;
            }
            memcpy(tData, reinterpret_cast<void *>(DYNAMIC_IFM_BASE + curInputIdx),
                    inputTensor->bytes);
            curInputIdx += inputTensor->bytes;
#else /* defined(DYNAMIC_IFM_BASE) */
            /* Create a random input. */
            for (size_t j = 0; j < inputTensor->bytes; ++j) {
                tData[j] = static_cast<uint8_t>(std::rand() & 0xFF);
            }
#endif /* defined(DYNAMIC_IFM_BASE) && defined(DYNAMIC_IFM_SIZE) */
        }
    }

#if defined(DYNAMIC_IFM_BASE)
    info("%d input tensor/s populated with %d bytes with data read from 0x%08x\n",
        numInputs, curInputIdx, DYNAMIC_IFM_BASE);
#endif /* defined(DYNAMIC_IFM_BASE) */
}

#if defined (DYNAMIC_OFM_BASE) && defined(DYNAMIC_OFM_SIZE)
static void PopulateDynamicOfm(const Model& model)
{
    /* Dump the output to a known memory location */
    const size_t numOutputs = model.GetNumOutputs();
    size_t curCopyIdx = 0;
    uint8_t* const dstPtr = reinterpret_cast<uint8_t *>(DYNAMIC_OFM_BASE);

    for (size_t outputIdx = 0; outputIdx < numOutputs; ++outputIdx) {
        TfLiteTensor* outputTensor = model.GetOutputTensor(outputIdx);
        uint8_t* const tData = tflite::GetTensorData<uint8_t>(outputTensor);

        if (tData && outputTensor->bytes > 0) {
            if (curCopyIdx + outputTensor->bytes > DYNAMIC_OFM_SIZE) {
                printf_err("OFM reserved buffer size insufficient\n");
                return;
            }
            memcpy(dstPtr + curCopyIdx, tData, outputTensor->bytes);
            curCopyIdx += outputTensor->bytes;
        }
    }

    info("%d output tensor/s worth %d bytes copied to 0x%08x\n",
        numOutputs, curCopyIdx, DYNAMIC_OFM_BASE);
}
#endif /* defined (DYNAMIC_OFM_BASE) && defined(DYNAMIC_OFM_SIZE) */

#if VERIFY_TEST_OUTPUT
static void DumpInputs(const Model& model, const char* message)
{
    info("%s\n", message);
    for (size_t inputIndex = 0; inputIndex < model.GetNumInputs(); inputIndex++) {
        arm::app::DumpTensor(model.GetInputTensor(inputIndex));
    }
}

static void DumpOutputs(const Model& model, const char* message)
{
    info("%s\n", message);
    for (size_t outputIndex = 0; outputIndex < model.GetNumOutputs(); outputIndex++) {
        arm::app::DumpTensor(model.GetOutputTensor(outputIndex));
    }
}
#endif /* VERIFY_TEST_OUTPUT */

bool RunInferenceHandler(ApplicationContext& ctx)
{
    auto& profiler = ctx.Get<Profiler&>("profiler");
    auto& model = ctx.Get<Model&>("model");

    constexpr uint32_t dataPsnTxtInfStartX = 150;
    constexpr uint32_t dataPsnTxtInfStartY = 40;

    if (!model.IsInited()) {
        printf_err("Model is not initialised! Terminating processing.\n");
        return false;
    }

#if VERIFY_TEST_OUTPUT
    DumpInputs(model, "Initial input tensors values");
    DumpOutputs(model, "Initial output tensors values");
#endif /* VERIFY_TEST_OUTPUT */

    PopulateInputTensor(model);

#if VERIFY_TEST_OUTPUT
    DumpInputs(model, "input tensors populated");
#endif /* VERIFY_TEST_OUTPUT */

    /* Strings for presentation/logging. */
    std::string str_inf{"Running inference... "};

    /* Display message on the LCD - inference running. */
    hal_lcd_display_text(str_inf.c_str(), str_inf.size(),
                         dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

    if (!RunInference(model, profiler)) {
        return false;
    }

    /* Erase. */
    str_inf = std::string(str_inf.size(), ' ');
    hal_lcd_display_text(
                            str_inf.c_str(), str_inf.size(),
                            dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

    info("Final results:\n");
    info("Total number of inferences: 1\n");
    profiler.PrintProfilingResult();

#if VERIFY_TEST_OUTPUT
    DumpOutputs(model, "output tensors post inference");
#endif /* VERIFY_TEST_OUTPUT */

#if defined (DYNAMIC_OFM_BASE) && defined(DYNAMIC_OFM_SIZE)
    PopulateDynamicOfm(model);
#endif /* defined (DYNAMIC_OFM_BASE) && defined(DYNAMIC_OFM_SIZE) */

    return true;
}

} /* namespace app */
} /* namespace arm */
