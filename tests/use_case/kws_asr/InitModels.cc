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
#include "MicroNetKwsModel.hpp"
#include "Wav2LetterModel.hpp"
#include "BufAttributes.hpp"

#include <catch.hpp>

namespace arm {
    namespace app {
        static uint8_t  tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

        namespace asr {
            extern uint8_t* GetModelPointer();
            extern size_t GetModelLen();
        }
        namespace kws {
            extern uint8_t* GetModelPointer();
            extern size_t GetModelLen();
        }
    } /* namespace app */
} /* namespace arm */

/* Skip this test, Wav2LetterModel if not Vela optimized but only from ML-zoo will fail. */
TEST_CASE("Init two Models", "[.]")
{
    arm::app::MicroNetKwsModel model1;
    arm::app::MicroNetKwsModel model2;

    /* Ideally we should load the wav2letter model here, but there is
     * none available to run on native (ops not supported on unoptimised
     * version). However, we can certainly create two instances of the
     * same type of model to see if our tensor arena re-use works as
     * intended.
     *
     * @TODO: uncomment this when this model can run on native pipeline. */
    //arm::app::Wav2LetterModel model2;     /* model2. */

    /* Load/initialise the first model. */
    REQUIRE(model1.Init(arm::app::tensorArena,
                        sizeof(arm::app::tensorArena),
                        arm::app::kws::GetModelPointer(),
                        arm::app::kws::GetModelLen()));

    /* Allocator instance should have been created. */
    REQUIRE(nullptr != model1.GetAllocator());

    /* Load the second model using the same allocator as model 1. */
    REQUIRE(model2.Init(arm::app::tensorArena,
                        sizeof(arm::app::tensorArena),
                        arm::app::asr::GetModelPointer(),
                        arm::app::asr::GetModelLen(),
                        model1.GetAllocator()));

    /* Make sure they point to the same allocator object. */
    REQUIRE(model1.GetAllocator() == model2.GetAllocator());

    /* Both models should report being initialised. */
    REQUIRE(true == model1.IsInited());
    REQUIRE(true == model2.IsInited());
}
