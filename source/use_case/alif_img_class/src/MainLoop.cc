/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * Copyright (c) 2021 - 2022 Arm Limited. All rights reserved.
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
#include "Classifier.hpp"           /* Classifier. */
#include "Labels.hpp"               /* For label strings. */
#include "MobileNetModel.hpp"       /* Model class for running inference. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace img_class {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace img_class */
} /* namespace app */
} /* namespace arm */

using ImgClassClassifier = arm::app::Classifier;

void main_loop()
{
    init_trigger_rx();

    arm::app::MobileNetModel model;  /* Model wrapper object. */

    if (!alif::app::ClassifyImageInit()) {
        printf_err("Failed to initialise use case handler\n");
    }

#if !SKIP_MODEL
    /* Load the model. */
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::img_class::GetModelPointer(),
                    arm::app::img_class::GetModelLen())) {
        printf_err("Failed to initialise model\n");
        return;
    }
#endif

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"img_class"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);

    ImgClassClassifier classifier;  /* Classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    std::vector<std::string> labels;
    GetLabelsVector(labels);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);

    /* Loop. */
    do {
        alif::app::ClassifyImageHandler(caseContext);
    } while (1);
}
