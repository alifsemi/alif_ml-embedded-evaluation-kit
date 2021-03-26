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
#include "Wav2LetterPostprocess.hpp"
#include "Wav2LetterModel.hpp"

#include <algorithm>
#include <catch.hpp>
#include <limits>

template <typename T>
static TfLiteTensor GetTestTensor(std::vector <int>& shape,
                                  T                  initVal,
                                  std::vector<T>&    vectorBuf)
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
        const uint32_t ctxLen = 5;
        const uint32_t innerLen = 3;
        arm::app::audio::asr::Postprocess post{ctxLen, innerLen, 0};

        std::vector <int> tensorShape = {1, 1, 1, 13};
        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                                tensorShape, 100, tensorVec);
        REQUIRE(false == post.Invoke(&tensor, arm::app::Wav2LetterModel::ms_outputRowsIdx, false));
    }

    SECTION("Post processing succeeds")
    {
        const uint32_t ctxLen = 5;
        const uint32_t innerLen = 3;
        arm::app::audio::asr::Postprocess post{ctxLen, innerLen, 0};

        std::vector <int> tensorShape = {1, 1, 13, 1};
        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                                tensorShape, 100, tensorVec);

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* This step should not erase anything. */
        REQUIRE(true == post.Invoke(&tensor, arm::app::Wav2LetterModel::ms_outputRowsIdx, false));
    }
}

TEST_CASE("Postprocessing - erasing required elements")
{
    constexpr uint32_t ctxLen = 5;
    constexpr uint32_t innerLen = 3;
    constexpr uint32_t nRows = 2*ctxLen + innerLen;
    constexpr uint32_t nCols = 10;
    constexpr uint32_t blankTokenIdx = nCols - 1;
    std::vector <int> tensorShape = {1, 1, nRows, nCols};

    SECTION("First and last iteration")
    {
        arm::app::audio::asr::Postprocess post{ctxLen, innerLen, blankTokenIdx};
        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                                tensorShape, 100, tensorVec);

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* This step should not erase anything. */
        REQUIRE(true == post.Invoke(&tensor, arm::app::Wav2LetterModel::ms_outputRowsIdx, true));
        REQUIRE(originalVec == tensorVec);
    }

    SECTION("Right context erase")
    {
        arm::app::audio::asr::Postprocess post{ctxLen, innerLen, blankTokenIdx};

        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                                tensorShape, 100, tensorVec);

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* This step should erase the right context only. */
        REQUIRE(true == post.Invoke(&tensor, arm::app::Wav2LetterModel::ms_outputRowsIdx, false));
        REQUIRE(originalVec != tensorVec);

        /* The last ctxLen * 10 elements should be gone. */
        for (size_t i = 0; i < ctxLen; ++i) {
            for (size_t j = 0; j < nCols; ++j) {
                /* Check right context elements are zeroed. */
                if (j == blankTokenIdx) {
                    CHECK(tensorVec[(ctxLen + innerLen) * nCols + i*nCols + j] == 1);
                } else {
                    CHECK(tensorVec[(ctxLen + innerLen) * nCols + i*nCols + j] == 0);
                }

                /* Check left context is preserved. */
                CHECK(tensorVec[i*nCols + j] == originalVec[i*nCols + j]);
            }
        }

        /* Check inner elements are preserved. */
        for (size_t i = ctxLen * nCols; i < (ctxLen + innerLen) * nCols; ++i) {
            CHECK(tensorVec[i] == originalVec[i]);
        }
    }

    SECTION("Left and right context erase")
    {
        arm::app::audio::asr::Postprocess post{ctxLen, innerLen, blankTokenIdx};

        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(tensorShape, 100, tensorVec);

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* This step should erase right context. */
        REQUIRE(true == post.Invoke(&tensor, arm::app::Wav2LetterModel::ms_outputRowsIdx, false));

        /* Calling it the second time should erase the left context. */
        REQUIRE(true == post.Invoke(&tensor, arm::app::Wav2LetterModel::ms_outputRowsIdx, false));

        REQUIRE(originalVec != tensorVec);

        /* The first and last ctxLen * 10 elements should be gone. */
        for (size_t i = 0; i < ctxLen; ++i) {
            for (size_t j = 0; j < nCols; ++j) {
                /* Check left and right context elements are zeroed. */
                if (j == blankTokenIdx) {
                    CHECK(tensorVec[(ctxLen + innerLen) * nCols + i * nCols + j] == 1);
                    CHECK(tensorVec[i * nCols + j] == 1);
                } else {
                    CHECK(tensorVec[(ctxLen + innerLen) * nCols + i * nCols + j] == 0);
                    CHECK(tensorVec[i * nCols + j] == 0);
                }
            }
        }

        /* Check inner elements are preserved. */
        for (size_t i = ctxLen * nCols; i < (ctxLen + innerLen) * nCols; ++i) {
            /* Check left context is preserved. */
            CHECK(tensorVec[i] == originalVec[i]);
        }
    }

    SECTION("Try left context erase")
    {
        /* Should not be able to erase the left context if it is the first iteration. */
        arm::app::audio::asr::Postprocess post{ctxLen, innerLen, blankTokenIdx};

        std::vector <int8_t> tensorVec;
        TfLiteTensor tensor = GetTestTensor<int8_t>(
                                tensorShape, 100, tensorVec);

        /* Copy elements to compare later. */
        std::vector <int8_t> originalVec = tensorVec;

        /* Calling it the second time should erase the left context. */
        REQUIRE(true == post.Invoke(&tensor, arm::app::Wav2LetterModel::ms_outputRowsIdx, true));
        REQUIRE(originalVec == tensorVec);
    }
}