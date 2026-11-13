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
#ifndef MLEK_TFLM_RESOURCE_VARIABLES_HPP
#define MLEK_TFLM_RESOURCE_VARIABLES_HPP

#include "mlek/fwk/tflm/TensorFlowLiteMicro.hpp"

namespace arm::app::fwk::tflm {

/**
 * @brief   Count unique resource variables required by a TFLM model.
 * @param[in] model     Pointer to the model object.
 * @return  Number of unique resource variables referenced by the model.
 */
int CountResourceVariables(const tflite::Model* model);

/**
 * @brief   Create resource variable storage for a model if required.
 * @param[in] resourceVariableCount  Number of resource variable slots needed.
 * @param[in] allocator              TFLM allocator used for persistent storage.
 * @return  Resource variables object, or nullptr if no storage is required.
 */
tflite::MicroResourceVariables* CreateResourceVariables(int resourceVariableCount,
                                                        tflite::MicroAllocator* allocator);

} /* namespace arm::app::fwk::tflm */

#endif /* MLEK_TFLM_RESOURCE_VARIABLES_HPP */
