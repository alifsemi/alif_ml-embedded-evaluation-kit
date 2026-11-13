/*
 * SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates
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
#include "mlek/fwk/tflm/TflmResourceVariables.hpp"
#include "mlek/log/log_macros.h"

#include <set>
#include <string>
#include <utility>

int arm::app::fwk::tflm::CountResourceVariables(const tflite::Model* model)
{
    if (!model || !model->subgraphs() || !model->operator_codes()) {
        return 0;
    }

    std::set<std::pair<std::string, std::string>> uniqueResourceVariables;
    const auto* opcodes = model->operator_codes();

    for (uint32_t subgraphIndex = 0; subgraphIndex < model->subgraphs()->size(); ++subgraphIndex) {
        const tflite::SubGraph* subgraph = model->subgraphs()->Get(subgraphIndex);
        if (!subgraph || !subgraph->operators()) {
            continue;
        }

        for (uint32_t operatorIndex = 0; operatorIndex < subgraph->operators()->size();
             ++operatorIndex) {
            const tflite::Operator* op = subgraph->operators()->Get(operatorIndex);
            if (!op || op->opcode_index() >= opcodes->size()) {
                continue;
            }

            const tflite::OperatorCode* opcode = opcodes->Get(op->opcode_index());
            if (!opcode || tflite::GetBuiltinCode(opcode) != tflite::BuiltinOperator_VAR_HANDLE) {
                continue;
            }

            const tflite::VarHandleOptions* options = op->builtin_options_as_VarHandleOptions();
            if (!options || !options->shared_name()) {
                continue;
            }

            uniqueResourceVariables.emplace(options->container() ? options->container()->str()
                                                                 : std::string{},
                                            options->shared_name()->str());
        }
    }

    return static_cast<int>(uniqueResourceVariables.size());
}

tflite::MicroResourceVariables*
arm::app::fwk::tflm::CreateResourceVariables(int resourceVariableCount,
                                             tflite::MicroAllocator* allocator)
{
    if (resourceVariableCount <= 0) {
        return nullptr;
    }

    info("Creating %d resource variable slot(s)\n", resourceVariableCount);
    return tflite::MicroResourceVariables::Create(allocator, resourceVariableCount);
}
