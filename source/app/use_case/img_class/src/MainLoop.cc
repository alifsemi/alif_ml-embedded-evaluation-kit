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

#include "BufAttributes.hpp"      /* Buffer attributes to be applied */
#include "mlek/common/Classifier.hpp"         /* Classifier. */
#include "Labels.hpp"             /* For label strings. */
#include "UseCaseCommonUtils.hpp" /* Utils functions. */
#include "UseCaseHandler.hpp"     /* Handlers for different user options. */

#if defined(MLEK_FWK_TFLM)
#include "mlek/fwk/tflm/MobileNetModel.hpp"   /* Model class for running inference. */
#elif defined(MLEK_FWK_EXECUTORCH)
#include "mlek/fwk/executorch/MobileNetModel.hpp"
#endif

namespace arm {
namespace app {
    static uint8_t activationBuf[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace img_class {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace img_class */
} /* namespace app */
} /* namespace arm */

using ImgClassClassifier = arm::app::Classifier;

/** Based on ML framework, set up the model namespace. */
#if defined(MLEK_FWK_TFLM)
using arm::app::fwk::tflm::MobileNetModel;
#elif defined(MLEK_FWK_EXECUTORCH)
using arm::app::fwk::et::MobileNetModel;
#endif /** MLEK_FWK_TFLM or MLEK_FWK_EXECUTORCH */

void MainLoop()
{
    MobileNetModel model; /* Model wrapper object. */
    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::img_class::GetModelPointer(),
                                                arm::app::img_class::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::activationBuf,
                                                  sizeof(arm::app::activationBuf)};

    /* Load the model. */
    if (!model.Init(computeMem, modelMem)) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"img_class"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::fwk::iface::Model&>("model", model);

    ImgClassClassifier classifier;  /* Classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    std::vector<std::string> labels;
    GetLabelsVector(labels);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);

    /* Loop. */
    bool executionSuccessful = ClassifyImageHandler(caseContext);
    info("Main loop terminated %s.\n",
        executionSuccessful ? "successfully" : "with failure");
}
