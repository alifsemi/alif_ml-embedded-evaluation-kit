/*
 * SPDX-FileCopyrightText: Copyright 2022, 2024-2025 Arm Limited and/or its
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
#include "hal.h"                    /* Brings in platform definitions. */
#include "mlek/fwk/tflm/YoloFastestModel.hpp"     /* Model class for running inference. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "mlek/log/log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

namespace arm {
namespace app {
    static uint8_t activationBuf[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace object_detection {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace object_detection */
} /* namespace app */
} /* namespace arm */

void MainLoop()
{
    arm::app::fwk::tflm::YoloFastestModel model; /* Model wrapper object. */

    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::object_detection::GetModelPointer(),
                                                arm::app::object_detection::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::activationBuf,
                                                  sizeof(arm::app::activationBuf)};

    /* Load the model. */
    if (!model.Init(computeMem, modelMem)) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"object_detection"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::fwk::iface::Model&>("model", model);

    bool executionSuccessful = ObjectDetectionHandler(caseContext);
    info("Main loop terminated %s.\n",
        executionSuccessful ? "successfully" : "with failure");
}
