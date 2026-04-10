/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024-2025 Arm Limited and/or its
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
#include <catch.hpp>
#include "mlek/fwk/tflm/MicroNetKwsModel.hpp"
#include "hal.h"

#include "mlek/use_case/kws/KwsResult.hpp"
#include "Labels.hpp"
#include "UseCaseHandler.hpp"
#include "mlek/common/Classifier.hpp"
#include "UseCaseCommonUtils.hpp"
#include "BufAttributes.hpp"

namespace arm {
    namespace app {
        static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
        namespace kws {
            extern uint8_t* GetModelPointer();
            extern size_t GetModelLen();
        } /* namespace kws */
    } /* namespace app */
} /* namespace arm */

TEST_CASE("Model info")
{
    /* Model wrapper object. */
    arm::app::fwk::tflm::MicroNetKwsModel model{};

    REQUIRE_FALSE(model.IsInited());
    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::kws::GetModelPointer(),
                                                arm::app::kws::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                  sizeof(arm::app::tensorArena)};
    REQUIRE(model.Init(computeMem, modelMem));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::fwk::iface::Model&>("model", model);

    model.LogInterpreterInfo();
    REQUIRE(model.IsInited());
}

TEST_CASE("Inference run all clips")
{
    /* Initialise the HAL and platform. */
    hal_platform_init();

    /* Model wrapper object. */
    arm::app::fwk::tflm::MicroNetKwsModel model{};
    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::kws::GetModelPointer(),
                                                arm::app::kws::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                  sizeof(arm::app::tensorArena)};
    REQUIRE(model.Init(computeMem, modelMem));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"kws"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::fwk::iface::Model&>("model", model);
    caseContext.Set<int>("frameLength", arm::app::kws::g_FrameLength);  /* 640 sample length for MicroNet. */
    caseContext.Set<int>("frameStride", arm::app::kws::g_FrameStride);  /* 320 sample stride for MicroNet. */
    caseContext.Set<float>("scoreThreshold", 0.7);       /* Normalised score threshold. */
    arm::app::Classifier classifier;                     /* classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    std::vector <std::string> labels;
    GetLabelsVector(labels);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);
    REQUIRE(arm::app::ClassifyAudioHandler(caseContext));
}
