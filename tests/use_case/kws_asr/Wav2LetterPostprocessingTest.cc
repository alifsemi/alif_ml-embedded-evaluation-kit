/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
#include "Wav2LetterPostprocess.hpp"
#include "Wav2LetterModel.hpp"
#include "ClassificationResult.hpp"
#include "BufAttributes.hpp"

#include <algorithm>
#include <catch.hpp>
#include <limits>

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

template <typename T>
static TfLiteTensor GetTestTensor(
        std::vector<int>&      shape,
        T                      initVal,
        std::vector<T>&        vectorBuf)
{
    REQUIRE(0 != shape.size());

    shape.insert(shape.begin(), shape.size());
    uint32_t sizeInBytes = sizeof(T);
    for (size_t i = 1; i < shape.size(); ++i) {
        sizeInBytes *= shape[i];
    }

    /* Allocate mem. */
    vectorBuf = std::vector<T>(sizeInBytes, initVal);
    TfLiteIntArray* dims = tflite::testing::IntArrayFromInts(shape.data());
    return tflite::testing::CreateQuantizedTensor(
            vectorBuf.data(), dims,
            1, 0, "test-tensor");
}

TEST_CASE("Checking return value")
{
    SECTION("Mismatched post processing parameters and tensor size")
    {
        const uint32_t outputCtxLen = 5;
        arm::app::AsrClassifier classifier;
        arm::app::Wav2LetterModel model;
        model.Init(arm::app::tensorArena,
                   sizeof(arm::app::tensorArena),
                   arm::app::asr::GetModelPointer(),
                   arm::app::asr::GetModelLen());
        std::vector<std::string> dummyLabels = {"a", "b", "$"};
        const uint32_t blankTokenIdx = 2;
        std::vector<arm::app::ClassificationResult> dummyResult;
        std::vector <int> tensorShape = {1, 1, 1, 13};
        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                tensorShape, 100, tensorVec);

        arm::app::AsrPostProcess post{&tensor, classifier, dummyLabels, dummyResult, outputCtxLen,
                                      blankTokenIdx, arm::app::Wav2LetterModel::ms_outputRowsIdx};

        REQUIRE(!post.DoPostProcess());
    }

    SECTION("Post processing succeeds")
    {
        const uint32_t outputCtxLen = 5;
        arm::app::AsrClassifier classifier;
        arm::app::Wav2LetterModel model;
        model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                   arm::app::asr::GetModelPointer(),
                   arm::app::asr::GetModelLen());
        std::vector<std::string> dummyLabels = {"a", "b", "$"};
        const uint32_t blankTokenIdx = 2;
        std::vector<arm::app::ClassificationResult> dummyResult;
        std::vector<int> tensorShape = {1, 1, 13, 1};
        std::vector<int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                tensorShape, 100, tensorVec);

        arm::app::AsrPostProcess post{&tensor, classifier, dummyLabels, dummyResult, outputCtxLen,
                                      blankTokenIdx, arm::app::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector<int8_t> originalVec = tensorVec;

        /* This step should not erase anything. */
        REQUIRE(post.DoPostProcess());
    }
}


TEST_CASE("Postprocessing - erasing required elements")
{
    constexpr uint32_t outputCtxLen = 5;
    constexpr uint32_t innerLen = 3;
    constexpr uint32_t nRows = 2*outputCtxLen + innerLen;
    constexpr uint32_t nCols = 10;
    constexpr uint32_t blankTokenIdx = nCols - 1;
    std::vector<int> tensorShape = {1, 1, nRows, nCols};
    arm::app::AsrClassifier classifier;
    arm::app::Wav2LetterModel model;
    model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::asr::GetModelPointer(),
                    arm::app::asr::GetModelLen());
    std::vector<std::string> dummyLabels = {"a", "b", "$"};
    std::vector<arm::app::ClassificationResult> dummyResult;

    SECTION("First and last iteration")
    {
        std::vector<int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(tensorShape, 100, tensorVec);
        arm::app::AsrPostProcess post{&tensor, classifier, dummyLabels, dummyResult, outputCtxLen,
                                      blankTokenIdx, arm::app::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector<int8_t>originalVec = tensorVec;

        /* This step should not erase anything. */
        post.m_lastIteration = true;
        REQUIRE(post.DoPostProcess());
        REQUIRE(originalVec == tensorVec);
    }

    SECTION("Right context erase")
    {
        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                tensorShape, 100, tensorVec);
        arm::app::AsrPostProcess post{&tensor, classifier, dummyLabels, dummyResult, outputCtxLen,
                                      blankTokenIdx, arm::app::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector<int8_t> originalVec = tensorVec;

        /* This step should erase the right context only. */
        post.m_lastIteration = false;
        REQUIRE(post.DoPostProcess());
        REQUIRE(originalVec != tensorVec);

        /* The last ctxLen * 10 elements should be gone. */
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
        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                tensorShape, 100, tensorVec);
        arm::app::AsrPostProcess post{&tensor, classifier, dummyLabels, dummyResult, outputCtxLen,
                                      blankTokenIdx, arm::app::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* This step should erase right context. */
        post.m_lastIteration = false;
        REQUIRE(post.DoPostProcess());

        /* Calling it the second time should erase the left context. */
        REQUIRE(post.DoPostProcess());

        REQUIRE(originalVec != tensorVec);

        /* The first and last ctxLen * 10 elements should be gone. */
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
        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                tensorShape, 100, tensorVec);

        /* Should not be able to erase the left context if it is the first iteration. */
        arm::app::AsrPostProcess post{&tensor, classifier, dummyLabels, dummyResult, outputCtxLen,
                                      blankTokenIdx, arm::app::Wav2LetterModel::ms_outputRowsIdx};

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* Calling it the second time should erase the left context. */
        post.m_lastIteration = true;
        REQUIRE(post.DoPostProcess());

        REQUIRE(originalVec == tensorVec);
    }
}
