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
#include "Profiler.hpp"

#include "AppContext.hpp"
#include "TensorFlowLiteMicro.hpp"

#include <catch.hpp>
#include <iostream>


TEST_CASE("Common: Test Profiler")
{
    hal_platform    platform;
    data_acq_module data_acq {};
    data_psn_module data_psn {};
    platform_timer  timer {};

    /* Initialise the HAL and platform. */
    hal_init(&platform, &data_acq, &data_psn, &timer);
    hal_platform_init(&platform);

    /* An invalid profiler shouldn't be of much use. */
    arm::app::Profiler profilerInvalid {nullptr, "test_invalid"};
    REQUIRE(false == profilerInvalid.StartProfiling());
    REQUIRE(false == profilerInvalid.StopProfiling());

    arm::app::Profiler profilerValid{&platform, "test_valid"};
    REQUIRE(true == profilerValid.StartProfiling());
    REQUIRE(true == profilerValid.StopProfiling());

    std::string strProfile = profilerValid.GetResultsAndReset();
    REQUIRE(std::string::npos != strProfile.find("test_valid"));

#if defined(CPU_PROFILE_ENABLED)
    /* We should have milliseconds elapsed. */
    REQUIRE(std::string::npos != strProfile.find("ms"));
#endif /* defined(CPU_PROFILE_ENABLED) */

    /* Abuse should fail: */
    REQUIRE(false == profilerValid.StopProfiling());  /* We need to start it first. */
    REQUIRE(true == profilerValid.StartProfiling());  /* Should be able to start it fine. */
    REQUIRE(false == profilerValid.StartProfiling()); /* Can't restart it without resetting. */
    profilerValid.Reset();                            /* Reset. */
    REQUIRE(true == profilerValid.StartProfiling());  /* Can start it again now. */
    REQUIRE(true == profilerValid.StopProfiling());   /* Can start it again now. */
}
