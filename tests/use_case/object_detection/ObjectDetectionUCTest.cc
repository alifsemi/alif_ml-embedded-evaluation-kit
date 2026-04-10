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
#include "mlek/use_case/object_detection/DetectionResult.hpp"
#include "mlek/use_case/object_detection/DetectorPostProcessing.hpp"
#include "hal.h"
#include "mlek/fwk/tflm/YoloFastestModel.hpp"
#include "UseCaseHandler.hpp"
#include "UseCaseCommonUtils.hpp"
#include "BufAttributes.hpp"

#include <catch.hpp>

namespace arm {
    namespace app {
        static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
        namespace object_detection {
            extern uint8_t* GetModelPointer();
            extern size_t GetModelLen();
        } /* namespace object_detection */
    } /* namespace app */
} /* namespace arm */

TEST_CASE("Model info")
{
    /* Model wrapper object. */
    arm::app::fwk::tflm::YoloFastestModel model;

    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::object_detection::GetModelPointer(),
                                                arm::app::object_detection::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                  sizeof(arm::app::tensorArena)};

    /* Load the model. */
    REQUIRE(model.Init(computeMem, modelMem));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::fwk::iface::Model&>("model", model);

    model.LogInterpreterInfo();
    REQUIRE(model.IsInited());
}


TEST_CASE("Inference by index")
{
    /* Initialise the HAL and platform. */
    hal_platform_init();

    /* Model wrapper object. */
    arm::app::fwk::tflm::YoloFastestModel model;

    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::object_detection::GetModelPointer(),
                                                arm::app::object_detection::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                  sizeof(arm::app::tensorArena)};

    /* Load the model. */
    REQUIRE(model.Init(computeMem, modelMem));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"object_detection"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::fwk::iface::Model&>("model", model);

    REQUIRE(arm::app::ObjectDetectionHandler(caseContext));
}


TEST_CASE("Inference run all images")
{
    /* Initialise the HAL and platform. */
    hal_platform_init();

    /* Model wrapper object. */
    arm::app::fwk::tflm::YoloFastestModel model;
    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::object_detection::GetModelPointer(),
                                                arm::app::object_detection::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                  sizeof(arm::app::tensorArena)};

    /* Load the model. */
    REQUIRE(model.Init(computeMem, modelMem));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"object_detection"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::fwk::iface::Model&>("model", model);

    REQUIRE(arm::app::ObjectDetectionHandler(caseContext));
}
