/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#include "DetectionResult.hpp"
//include "Detector.hpp"
#include "hal.h"
#include "YoloFastestModel.hpp"
#include "UseCaseHandler.hpp"
#include "UseCaseCommonUtils.hpp"

#include <catch.hpp>

TEST_CASE("Model info")
{
printf("Entering Model info \n");
    /* Model wrapper object. */
    arm::app::YoloFastestModel model;

    /* Load the model. */
    REQUIRE(model.Init());

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::Model&>("model", model);

    REQUIRE(model.ShowModelInfoHandler());
}


TEST_CASE("Inference by index")
{
printf("Entering Inference by index \n");
    hal_platform    platform;
    data_acq_module data_acq;
    data_psn_module data_psn;
    platform_timer  timer;

    /* Initialise the HAL and platform. */
    hal_init(&platform, &data_acq, &data_psn, &timer);
    hal_platform_init(&platform);

    /* Model wrapper object. */
    arm::app::YoloFastestModel model;

    /* Load the model. */
    REQUIRE(model.Init());

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{&platform, "object_detection"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("imgIndex", 0);

    REQUIRE(arm::app::ObjectDetectionHandler(caseContext, 0, false));

    auto results = caseContext.Get<std::vector<arm::app::DetectionResult>>("results");

}


TEST_CASE("Inference run all images")
{
    printf("Entering Inference run all images \n");
    hal_platform    platform;
    data_acq_module data_acq;
    data_psn_module data_psn;
    platform_timer  timer;

    /* Initialise the HAL and platform. */
    hal_init(&platform, &data_acq, &data_psn, &timer);
    hal_platform_init(&platform);

    /* Model wrapper object. */
    arm::app::YoloFastestModel model;

    /* Load the model. */
    REQUIRE(model.Init());

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{&platform, "object_detection"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("imgIndex", 0);

    REQUIRE(arm::app::ObjectDetectionHandler(caseContext, 0, true));
    
}


TEST_CASE("List all images")
{
printf("Entering List all images \n");
    hal_platform    platform;
    data_acq_module data_acq;
    data_psn_module data_psn;
    platform_timer  timer;

    /* Initialise the HAL and platform. */
    hal_init(&platform, &data_acq, &data_psn, &timer);
    hal_platform_init(&platform);

    /* Model wrapper object. */
    arm::app::YoloFastestModel model;

    /* Load the model. */
    REQUIRE(model.Init());

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<arm::app::Model&>("model", model);

    REQUIRE(arm::app::ListFilesHandler(caseContext));
}
