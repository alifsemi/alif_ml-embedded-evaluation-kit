/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "InputFiles.hpp"           /* For input data */
#include "AdModel.hpp"              /* Model class for running inference */
#include "UseCaseCommonUtils.hpp"   /* Utils functions */
#include "UseCaseHandler.hpp"       /* Handlers for different user options */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

namespace arm {
namespace app {
    namespace ad {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace ad */
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
} /* namespace app */
} /* namespace arm */


void main_loop()
{

    arm::app::AdModel model;  /* Model wrapper object. */

    /* Load the model. */
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::ad::GetModelPointer(),
                    arm::app::ad::GetModelLen())) {
        printf_err("failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"ad"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("index", 0);
    caseContext.Set<uint32_t>("result", 0);
    caseContext.Set<uint32_t>("frameLength", arm::app::ad::g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", arm::app::ad::g_FrameStride);
    caseContext.Set<float>("scoreThreshold", arm::app::ad::g_ScoreThreshold);
    caseContext.Set<float>("trainingMean", arm::app::ad::g_TrainingMean);
#if defined(MACHINE_GAIN)
    caseContext.Set<float>("machineGain", MACHINE_GAIN);
#endif

     /* Loop. */
    do {
        alif::app::ClassifyVibrationHandler(caseContext);
    } while (1);
}
