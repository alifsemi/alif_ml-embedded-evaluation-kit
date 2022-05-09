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
#include <catch.hpp>
#include "VisualWakeWordModel.hpp"
#include "hal.h"

#include "ClassificationResult.hpp"
#include "Labels.hpp"
#include "UseCaseHandler.hpp"
#include "Classifier.hpp"
#include "UseCaseCommonUtils.hpp"

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace vww {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace vww */
} /* namespace app */
} /* namespace arm */

TEST_CASE("Model info")
{
    arm::app::VisualWakeWordModel model;    /* model wrapper object */

    /* Load the model */
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::vww::GetModelPointer(),
                       arm::app::vww::GetModelLen()));

    /* Instantiate application context */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::Model&>("model", model);

    REQUIRE(model.ShowModelInfoHandler());
}

TEST_CASE("Inference by index")
{
    hal_platform_init();

    arm::app::VisualWakeWordModel model;    /* model wrapper object */

    /* Load the model */
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::vww::GetModelPointer(),
                       arm::app::vww::GetModelLen()));

    /* Instantiate application context */
    arm::app::ApplicationContext caseContext;
    arm::app::Profiler profiler{"pd"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("imgIndex", 0);
    arm::app::Classifier classifier;    /* classifier wrapper object */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    std::vector <std::string> labels;
    GetLabelsVector(labels);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);

    REQUIRE(arm::app::ClassifyImageHandler(caseContext, 0, false));

    auto results = caseContext.Get<std::vector<arm::app::ClassificationResult>>("results");

    REQUIRE(results[0].m_labelIdx == 1);
}

TEST_CASE("Inference run all images")
{
    /* Initialise the HAL and platform */
    hal_platform_init();

    arm::app::VisualWakeWordModel model;    /* model wrapper object */

    /* Load the model */
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::vww::GetModelPointer(),
                       arm::app::vww::GetModelLen()));

    /* Instantiate application context */
    arm::app::ApplicationContext caseContext;
    arm::app::Profiler profiler{"pd"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("imgIndex", 0);
    arm::app::Classifier classifier;    /* classifier wrapper object */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    std::vector <std::string> labels;
    GetLabelsVector(labels);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);

    REQUIRE(arm::app::ClassifyImageHandler(caseContext, 0, true));
}

TEST_CASE("List all images")
{
    /* Initialise the HAL and platform */
    hal_platform_init();

    arm::app::VisualWakeWordModel model;    /* model wrapper object */

    /* Load the model */
    REQUIRE(model.Init(arm::app::tensorArena,
                       sizeof(arm::app::tensorArena),
                       arm::app::vww::GetModelPointer(),
                       arm::app::vww::GetModelLen()));

    /* Instantiate application context */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::Model&>("model", model);

    REQUIRE(arm::app::ListFilesHandler(caseContext));
}
