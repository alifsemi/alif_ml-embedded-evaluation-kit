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

#include "AdPostProcessing.hpp"
#include <catch.hpp>

TEST_CASE("Softmax_vector") {

    std::vector<float> testVec = {1, 2, 3, 4, 1, 2, 3};
    arm::app::Softmax(testVec);
    CHECK((testVec[0] - 0.024) == Approx(0.0).margin(0.001));
    CHECK((testVec[1] - 0.064) == Approx(0.0).margin(0.001));
    CHECK((testVec[2] - 0.175) == Approx(0.0).margin(0.001));
    CHECK((testVec[3] - 0.475) == Approx(0.0).margin(0.001));
    CHECK((testVec[4] - 0.024) == Approx(0.0).margin(0.001));
    CHECK((testVec[5] - 0.064) == Approx(0.0).margin(0.001));
    CHECK((testVec[6] - 0.175) == Approx(0.0).margin(0.001));
}

TEST_CASE("Output machine index") {

    auto index = arm::app::OutputIndexFromFileName("test_id_00.wav");
    CHECK(index == 0);

    auto index1 = arm::app::OutputIndexFromFileName("test_id_02.wav");
    CHECK(index1 == 1);

    auto index2 = arm::app::OutputIndexFromFileName("test_id_4.wav");
    CHECK(index2 == 2);

    auto index3 = arm::app::OutputIndexFromFileName("test_id_6.wav");
    CHECK(index3 == 3);

    auto index4 = arm::app::OutputIndexFromFileName("test_id_id_00.wav");
    CHECK(index4 == -1);

    auto index5 = arm::app::OutputIndexFromFileName("test_id_7.wav");
    CHECK(index5 == -1);
}