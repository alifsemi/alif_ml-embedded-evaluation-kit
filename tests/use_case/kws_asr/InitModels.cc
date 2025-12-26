/*
 * SPDX-FileCopyrightText: Copyright 2021, 2025 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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
#include "mlek/fwk/tflm/MicroNetKwsModel.hpp"
#include "mlek/fwk/tflm/Wav2LetterModel.hpp"
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
    arm::app::fwk::tflm::MicroNetKwsModel model1;
    arm::app::fwk::tflm::Wav2LetterModel model2;
    {
        arm::app::fwk::iface::MemoryRegion modelMem{arm::app::kws::GetModelPointer(),
                                                    arm::app::kws::GetModelLen()};
        arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                      sizeof(arm::app::tensorArena)};
        REQUIRE(model1.Init(computeMem, modelMem));
    }

    {
        arm::app::fwk::iface::MemoryRegion modelMem{arm::app::asr::GetModelPointer(),
                                                    arm::app::asr::GetModelLen()};
        arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                      sizeof(arm::app::tensorArena)};
        REQUIRE(model2.Init(computeMem, modelMem, &model1.GetBackendData()));
    }

    /* Both models should report being initialised. */
    REQUIRE(true == model1.IsInited());
    REQUIRE(true == model2.IsInited());
}
