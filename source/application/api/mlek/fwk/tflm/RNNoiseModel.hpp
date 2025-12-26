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
#ifndef RNNOISE_MODEL_HPP
#define RNNOISE_MODEL_HPP

#include "mlek/fwk/tflm/TflmModel.hpp"

namespace arm::app::rnn {
extern const uint32_t g_NumInputFeatures;
extern const uint32_t g_FrameLength;
extern const uint32_t g_FrameStride;
} /* namespace arm::app::rnn */

namespace arm::app::fwk::tflm {

class RNNoiseModel : public TflmModel {
public:
    /**
     * @brief Sets GRU input states to zeros.
     * Call this method before starting processing the new sequence of logically related data.
     */
    void ResetGruState();

    /**
     * @brief Copy current GRU output states to input states.
     * Call this method before starting processing the next sequence of logically related data.
     */
    bool CopyGruStates() const;

    /* Which index of model outputs does the main output (gains) come from. */
    const size_t m_indexForModelOutput = 1;

protected:
    /** @brief   Gets the reference to op resolver interface class. */
    const tflite::MicroOpResolver& GetOpResolver() override;

    /** @brief   Adds operations to the op resolver instance. */
    bool EnlistOperations() override;

    /*
    Each inference after the first needs to copy 3 GRU states from a output index to input index
    (model dependent): 0 -> 3, 2 -> 2, 3 -> 1
    */
    const std::vector<std::pair<size_t, size_t>> m_gruStateMap = {{0, 3}, {2, 2}, {3, 1}};

private:
    /* Maximum number of individual operations that can be enlisted. */
    static constexpr int ms_maxOpCnt = 15;

    /* A mutable op resolver instance. */
    tflite::MicroMutableOpResolver<ms_maxOpCnt> m_opResolver;
};

} /* namespace arm::app::fwk::tflm */

#endif /* RNNOISE_MODEL_HPP */
