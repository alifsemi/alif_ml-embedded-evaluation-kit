/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates
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
#ifndef CONFORMERMODEL_HPP
#define CONFORMERMODEL_HPP

#include "mlek/fwk/executorch/EtModel.hpp"

namespace arm::app::asr {
extern const int g_melSpecWindowSize;
extern const int g_melSpecHopSize;
extern const int g_chunkSize;
} // namespace arm::app::asr

namespace arm::app::fwk::et {

class ConformerModel : public EtModel {

public:
    /* Indices for the expected model - based on input tensor shape */
    static constexpr uint32_t ms_inputMelRowsIdx = 1;
    static constexpr uint32_t ms_inputMelColsIdx = 2;
};

} /* namespace arm::app::fwk::et */

#endif /* CONFORMERMODEL_HPP */
