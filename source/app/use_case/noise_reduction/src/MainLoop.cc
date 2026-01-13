/*
 * SPDX-FileCopyrightText: Copyright 2021-2025 Arm Limited and/or its
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
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "mlek/fwk/tflm/RNNoiseModel.hpp"         /* Model class for running inference. */
#include "mlek/log/log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

namespace arm {
namespace app {
    static uint8_t activationBuf[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace rnn {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace rnn */
} /* namespace app */
} /* namespace arm */

void MainLoop()
{
    arm::app::fwk::tflm::RNNoiseModel model; /* Model wrapper object. */
    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::rnn::GetModelPointer(),
                                                arm::app::rnn::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::activationBuf,
                                                  sizeof(arm::app::activationBuf)};

    /* Load the model. */
    if (!model.Init(computeMem, modelMem)) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"noise_reduction"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<uint32_t>("numInputFeatures", arm::app::rnn::g_NumInputFeatures);
    caseContext.Set<uint32_t>("frameLength", arm::app::rnn::g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", arm::app::rnn::g_FrameStride);
    caseContext.Set<arm::app::fwk::tflm::RNNoiseModel&>("model", model);

#if defined(MEM_DUMP_BASE_ADDR)
    /* For this use case, for valid targets, we dump contents
     * of the output tensor to a certain location in memory to
     * allow offline tools to pick this data up. */
    constexpr size_t memDumpMaxLen = MEM_DUMP_LEN;
    uint8_t* memDumpBaseAddr = reinterpret_cast<uint8_t *>(MEM_DUMP_BASE_ADDR);
    size_t memDumpBytesWritten = 0;
    caseContext.Set<size_t>("MEM_DUMP_LEN", memDumpMaxLen);
    caseContext.Set<uint8_t*>("MEM_DUMP_BASE_ADDR", memDumpBaseAddr);
    caseContext.Set<size_t*>("MEM_DUMP_BYTE_WRITTEN", &memDumpBytesWritten);
#endif /* defined(MEM_DUMP_BASE_ADDR) */

    /* Loop. */
    bool executionSuccessful = arm::app::NoiseReductionHandler(caseContext);
    info("Main loop terminated %s.\n",
        executionSuccessful ? "successfully" : "with failure");
}
