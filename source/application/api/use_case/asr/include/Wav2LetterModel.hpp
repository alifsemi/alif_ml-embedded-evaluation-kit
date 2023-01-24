/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef ASR_WAV2LETTER_MODEL_HPP
#define ASR_WAV2LETTER_MODEL_HPP

#include "Model.hpp"

namespace arm {
namespace app {
namespace asr {
    extern const int g_FrameLength;
    extern const int g_FrameStride;
    extern const float g_ScoreThreshold;
    extern const int g_ctxLen;
} /* namespace asr */
} /* namespace app */
} /* namespace arm */

namespace arm {
namespace app {

    class Wav2LetterModel : public Model {

    public:
        /* Indices for the expected model - based on input and output tensor shapes */
        static constexpr uint32_t ms_inputRowsIdx  = 1;
        static constexpr uint32_t ms_inputColsIdx  = 2;
        static constexpr uint32_t ms_outputRowsIdx = 2;
        static constexpr uint32_t ms_outputColsIdx = 3;

        /* Model specific constants. */
        static constexpr uint32_t ms_blankTokenIdx   = 28;
        static constexpr uint32_t ms_numMfccFeatures = 13;

    protected:
        /** @brief   Gets the reference to op resolver interface class. */
        const tflite::MicroOpResolver& GetOpResolver() override;

        /** @brief   Adds operations to the op resolver instance. */
        bool EnlistOperations() override;

    private:
        /* Maximum number of individual operations that can be enlisted. */
        static constexpr int ms_maxOpCnt = 5;

        /* A mutable op resolver instance. */
        tflite::MicroMutableOpResolver<ms_maxOpCnt> m_opResolver;
    };

} /* namespace app */
} /* namespace arm */

#endif /* ASR_WAV2LETTER_MODEL_HPP */
