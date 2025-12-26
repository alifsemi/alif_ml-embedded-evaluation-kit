/*
 * SPDX-FileCopyrightText: Copyright 2021-2023, 2025 Arm Limited and/or its affiliates
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
#include "mlek/common/ClassificationResult.hpp"
#include "mlek/fwk/tflm/TflmTensor.hpp"
#include "mlek/fwk/tflm/Wav2LetterModel.hpp"
#include "mlek/use_case/asr/Wav2LetterPostprocess.hpp"

#include <algorithm>
#include <catch.hpp>
#include <limits>

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;

    namespace asr {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } // namespace asr
    namespace kws {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } // namespace kws
} /* namespace app */
} /* namespace arm */

template <typename T>
static std::vector<T>
VectorFromTensor(const std::shared_ptr<arm::app::fwk::iface::TensorIface>& tensor)
{
    auto* tData = tensor->GetData<T>();
    return std::vector<int8_t>(tData, tData + tensor->GetNumElements());
}

TEST_CASE("Checking return value")
{
    SECTION("Post processing succeeds")
    {
        const uint32_t inputContexLen = 98;
        arm::app::AsrClassifier classifier{
            arm::app::fwk::tflm::Wav2LetterModel::ms_inputRowsIdx,
            arm::app::fwk::tflm::Wav2LetterModel::ms_inputColsIdx,
            arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx,
            arm::app::fwk::tflm::Wav2LetterModel::ms_outputColsIdx}; /* classifier */

        arm::app::fwk::tflm::Wav2LetterModel model;
        arm::app::fwk::iface::MemoryRegion modelMem{arm::app::asr::GetModelPointer(),
                                                    arm::app::asr::GetModelLen()};
        arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                      sizeof(arm::app::tensorArena)};
        REQUIRE(model.Init(computeMem, modelMem));
        std::vector<std::string> placeholderLabels = {"a", "b", "$"};
        const uint32_t blankTokenIdx = 2;
        std::vector<arm::app::ClassificationResult> placeholderResult;

        arm::app::AsrPostProcess post{model,
                                      classifier,
                                      placeholderLabels,
                                      placeholderResult,
                                      inputContexLen,
                                      blankTokenIdx,
                                      arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx};

        /* This step should not erase anything. */
        REQUIRE(post.DoPostProcess());
    }
}

TEST_CASE("Postprocessing - erasing required elements")
{
    constexpr uint32_t inputCtxLen   = 98;
    constexpr uint32_t outputCtxLen  = 49;
    constexpr uint32_t innerLen      = 50;
    constexpr uint32_t nCols         = 29;
    constexpr uint32_t blankTokenIdx = arm::app::fwk::tflm::Wav2LetterModel::ms_blankTokenIdx;
    arm::app::AsrClassifier classifier{
        arm::app::fwk::tflm::Wav2LetterModel::ms_inputRowsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_inputColsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx,
        arm::app::fwk::tflm::Wav2LetterModel::ms_outputColsIdx}; /* classifier */
    arm::app::fwk::tflm::Wav2LetterModel model;
    arm::app::fwk::iface::MemoryRegion modelMem{arm::app::asr::GetModelPointer(),
                                                arm::app::asr::GetModelLen()};
    arm::app::fwk::iface::MemoryRegion computeMem{arm::app::tensorArena,
                                                  sizeof(arm::app::tensorArena)};
    REQUIRE(model.Init(computeMem, modelMem));
    std::vector<std::string> placeholderLabels = {"a", "b", "$"};
    std::vector<arm::app::ClassificationResult> placeholderResult;

    SECTION("First and last iteration")
    {
        std::vector<int8_t> tensorVec = VectorFromTensor<int8_t>(model.GetOutputTensor(0));
        arm::app::AsrPostProcess post{model,
                                      classifier,
                                      placeholderLabels,
                                      placeholderResult,
                                      inputCtxLen,
                                      blankTokenIdx,
                                      arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector<int8_t>originalVec = tensorVec;

        /* This step should not erase anything. */
        post.m_lastIteration = true;
        REQUIRE(post.DoPostProcess());
        tensorVec = VectorFromTensor<int8_t>(model.GetOutputTensor(0));
        REQUIRE(originalVec == tensorVec);
    }

    SECTION("Right context erase")
    {
        std::vector<int8_t> tensorVec = VectorFromTensor<int8_t>(model.GetOutputTensor(0));
        arm::app::AsrPostProcess post{model,
                                      classifier,
                                      placeholderLabels,
                                      placeholderResult,
                                      inputCtxLen,
                                      blankTokenIdx,
                                      arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector<int8_t> originalVec = tensorVec;

        /* This step should erase the right context only. */
        post.m_lastIteration = false;
        REQUIRE(post.DoPostProcess());
        tensorVec = VectorFromTensor<int8_t>(model.GetOutputTensor(0));
        REQUIRE(originalVec != tensorVec);

        /* The last ctxLen * nCols elements should be gone. */
        for (size_t i = 0; i < outputCtxLen; ++i) {
            for (size_t j = 0; j < nCols; ++j) {
                /* Check right context elements are zeroed. Blank token idx should be set to 1 when erasing. */
                if (j == blankTokenIdx) {
                    CHECK(tensorVec[(outputCtxLen + innerLen) * nCols + i*nCols + j] == 1);
                } else {
                    CHECK(tensorVec[(outputCtxLen + innerLen) * nCols + i*nCols + j] == 0);
                }

                /* Check left context is preserved. */
                CHECK(tensorVec[i*nCols + j] == originalVec[i*nCols + j]);
            }
        }

        /* Check inner elements are preserved. */
        for (size_t i = outputCtxLen * nCols; i < (outputCtxLen + innerLen) * nCols; ++i) {
            CHECK(tensorVec[i] == originalVec[i]);
        }
    }

    SECTION("Left and right context erase")
    {
        std::vector<int8_t> tensorVec = VectorFromTensor<int8_t>(model.GetOutputTensor(0));
        arm::app::AsrPostProcess post{model,
                                      classifier,
                                      placeholderLabels,
                                      placeholderResult,
                                      inputCtxLen,
                                      blankTokenIdx,
                                      arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* This step should erase right context. */
        post.m_lastIteration = false;
        REQUIRE(post.DoPostProcess());

        /* Calling it the second time should erase the left context. */
        REQUIRE(post.DoPostProcess());

        tensorVec = VectorFromTensor<int8_t>(model.GetOutputTensor(0));
        REQUIRE(originalVec != tensorVec);

        /* The first and last ctxLen * nCols elements should be gone. */
        for (size_t i = 0; i < outputCtxLen; ++i) {
            for (size_t j = 0; j < nCols; ++j) {
                /* Check left and right context elements are zeroed. */
                if (j == blankTokenIdx) {
                    CHECK(tensorVec[(outputCtxLen + innerLen) * nCols + i*nCols + j] == 1);
                    CHECK(tensorVec[i*nCols + j] == 1);
                } else {
                    CHECK(tensorVec[(outputCtxLen + innerLen) * nCols + i*nCols + j] == 0);
                    CHECK(tensorVec[i*nCols + j] == 0);
                }
            }
        }

        /* Check inner elements are preserved. */
        for (size_t i = outputCtxLen * nCols; i < (outputCtxLen + innerLen) * nCols; ++i) {
            /* Check left context is preserved. */
            CHECK(tensorVec[i] == originalVec[i]);
        }
    }

    SECTION("Try left context erase")
    {
        std::vector<int8_t> tensorVec = VectorFromTensor<int8_t>(model.GetOutputTensor(0));

        /* Should not be able to erase the left context if it is the first iteration. */
        arm::app::AsrPostProcess post{model,
                                      classifier,
                                      placeholderLabels,
                                      placeholderResult,
                                      inputCtxLen,
                                      blankTokenIdx,
                                      arm::app::fwk::tflm::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* Calling it the second time should erase the left context. */
        post.m_lastIteration = true;
        REQUIRE(post.DoPostProcess());

        tensorVec = VectorFromTensor<int8_t>(model.GetOutputTensor(0));
        REQUIRE(originalVec == tensorVec);
    }
}
