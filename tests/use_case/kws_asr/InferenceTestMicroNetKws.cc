/*
 * SPDX-FileCopyrightText: Copyright 2021, 2024-2025 Arm Limited and/or its affiliates
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
#include "BufAttributes.hpp"
#include "mlek/fwk/tflm/MicroNetKwsModel.hpp"
#include "mlek/fwk/tflm/TensorFlowLiteMicro.hpp"
#include "TestData_kws.hpp"

#include <catch.hpp>
#include <random>

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace kws {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace kws */
} /* namespace app */
} /* namespace arm */

namespace test {
namespace kws {

    bool RunInference(arm::app::fwk::iface::Model& model, const int8_t vec[])
    {
        auto inputTensor = model.GetInputTensor(0);
        REQUIRE(inputTensor);

        const size_t copySz =
            inputTensor->Bytes() < IFM_0_DATA_SIZE ? inputTensor->Bytes() : IFM_0_DATA_SIZE;
        memcpy(inputTensor->GetData(), vec, copySz);

        return model.RunInference();
    }

    bool RunInferenceRandom(arm::app::fwk::iface::Model& model)
    {
        auto inputTensor = model.GetInputTensor(0);
        REQUIRE(inputTensor);

        std::random_device rndDevice;
        std::mt19937 mersenneGen{rndDevice()};
        std::uniform_int_distribution<short> dist{-128, 127};

        auto gen = [&dist, &mersenneGen]() { return dist(mersenneGen); };

        std::vector<int8_t> randomAudio(inputTensor->Bytes());
        std::generate(std::begin(randomAudio), std::end(randomAudio), gen);

        REQUIRE(RunInference(model, randomAudio.data()));
        return true;
    }

    template <typename T>
    void TestInference(const T* input_goldenFV,
                       const T* output_goldenFV,
                       arm::app::fwk::iface::Model& model)
    {
        REQUIRE(RunInference(model, input_goldenFV));

        auto outputTensor = model.GetOutputTensor(0);

        REQUIRE(outputTensor);
        REQUIRE(outputTensor->Bytes() == OFM_0_DATA_SIZE);
        auto tensorData = outputTensor->GetData<T>();
        REQUIRE(tensorData);

        for (size_t i = 0; i < outputTensor->Bytes(); i++) {
            REQUIRE(static_cast<int>(tensorData[i]) == static_cast<int>((T)output_goldenFV[i]));
        }
    }

    TEST_CASE("Running random inference with Tflu and MicroNetKwsModel Int8", "[MicroNetKws]")
    {
        arm::app::fwk::tflm::MicroNetKwsModel model{};

        REQUIRE_FALSE(model.IsInited());
        arm::app::fwk::iface::MemoryRegion modelMem{arm::app::kws::GetModelPointer(),
                                                    arm::app::kws::GetModelLen()};
        arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                      sizeof(arm::app::tensorArena)};
        REQUIRE(model.Init(computeMem, modelMem));
        REQUIRE(model.IsInited());

        REQUIRE(RunInferenceRandom(model));
    }

    TEST_CASE("Running inference with Tflu and MicroNetKwsModel Int8", "[MicroNetKws]")
    {
        REQUIRE(NUMBER_OF_IFM_FILES == NUMBER_OF_OFM_FILES);
        for (uint32_t i = 0; i < NUMBER_OF_IFM_FILES; ++i) {
            auto input_goldenFV  = reinterpret_cast<const int8_t *>(GetIfmDataArray(i));
            auto output_goldenFV = reinterpret_cast<const int8_t *>(GetOfmDataArray(i));

            DYNAMIC_SECTION("Executing inference with re-init")
            {
                arm::app::fwk::tflm::MicroNetKwsModel model{};

                REQUIRE_FALSE(model.IsInited());
                arm::app::fwk::iface::MemoryRegion modelMem{arm::app::kws::GetModelPointer(),
                                                            arm::app::kws::GetModelLen()};
                arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                              sizeof(arm::app::tensorArena)};
                REQUIRE(model.Init(computeMem, modelMem));
                REQUIRE(model.IsInited());

                TestInference<int8_t>(input_goldenFV, output_goldenFV, model);
            }
        }
    }

} // namespace kws
} // namespace test
