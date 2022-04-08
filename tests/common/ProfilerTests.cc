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
    hal_platform_init();

    SECTION("Test default construction") {
        arm::app::Profiler profiler{};
        REQUIRE(true == profiler.StartProfiling());
        REQUIRE(true == profiler.StopProfiling());
    }

    SECTION("Test valid profiler") {
        arm::app::Profiler profilerValid{"test_valid"};
        REQUIRE(true == profilerValid.StartProfiling());
        REQUIRE(true == profilerValid.StopProfiling());
        std::vector<arm::app::ProfileResult> results;
        profilerValid.GetAllResultsAndReset(results);
        REQUIRE(results.size() == 1);
        REQUIRE(results[0].name == "test_valid");
        /* Abuse should still fail: */
        REQUIRE(false == profilerValid.StopProfiling()); /* We need to start it first */
        REQUIRE(true == profilerValid.StartProfiling()); /* Should be able to start it fine */
        REQUIRE(false == profilerValid.StartProfiling()); /* Can't restart it without resetting */
        profilerValid.Reset();
        REQUIRE(true == profilerValid.StartProfiling()); /* Can start it again now.. */
        REQUIRE(true == profilerValid.StopProfiling());
    }

    SECTION("Test multiple profilers") {
        arm::app::Profiler profilerValid{"one"};
        REQUIRE(true == profilerValid.StartProfiling());
        REQUIRE(true == profilerValid.StopProfiling());

        REQUIRE(true == profilerValid.StartProfiling("two"));
        REQUIRE(true == profilerValid.StopProfiling());
        REQUIRE(true == profilerValid.StartProfiling("two"));
        REQUIRE(true == profilerValid.StopProfiling());

        std::vector<arm::app::ProfileResult> results;
        profilerValid.GetAllResultsAndReset(results);
        REQUIRE(results.size() == 2);
        REQUIRE(results[0].name == "one");
        REQUIRE(results[0].samplesNum == 1);
        REQUIRE(results[1].name == "two");
        REQUIRE(results[1].samplesNum == 2);
    }

#if defined (CPU_PROFILE_ENABLED)
    SECTION("Test CPU profiler") {

        arm::app::Profiler profilerCPU{"test cpu"};
        std::vector<arm::app::ProfileResult> results;
        profilerCPU.StartProfiling();
        profilerCPU.StopProfiling();
        profilerCPU.GetAllResultsAndReset(results);
        REQUIRE(results.size() == 1);
        bool foundTime = false;
        bool foundCPU_ACTIVE = false;
        for(arm::app::Statistics& stat: results[0].data) {

            if (!foundTime) {
                foundTime = stat.name == "Time";
            }

            if (!foundCPU_ACTIVE) {
                foundCPU_ACTIVE = stat.name == "CPU ACTIVE";
            }

        }
        REQUIRE(foundTime);
        REQUIRE(foundCPU_ACTIVE);
    }
#endif /* defined (CPU_PROFILE_ENABLED) */
}