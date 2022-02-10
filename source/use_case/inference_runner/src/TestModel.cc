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
#include "TestModel.hpp"
#include "log_macros.h"

const tflite::AllOpsResolver& arm::app::TestModel::GetOpResolver()
{
    return this->m_opResolver;
}

#if defined(DYNAMIC_MODEL_BASE) && defined(DYNAMIC_MODEL_SIZE)

    const uint8_t* arm::app::TestModel::ModelPointer()
    {
        info("Model pointer: 0x%08x\n", DYNAMIC_MODEL_BASE);
        return reinterpret_cast<uint8_t *>(DYNAMIC_MODEL_BASE);
    }

    size_t arm::app::TestModel::ModelSize()
    {
        /* TODO: Can we get the actual model size here somehow?
         * Currently we return the reserved space. It is possible to do
         * so by reading the memory pattern but it will not be reliable. */
        return static_cast<size_t>(DYNAMIC_MODEL_SIZE);
    }

#else /* defined(DYNAMIC_MODEL_BASE) && defined(DYNAMIC_MODEL_SIZE) */

    extern uint8_t* GetModelPointer();
    const uint8_t* arm::app::TestModel::ModelPointer()
    {
        return GetModelPointer();
    }

    extern size_t GetModelLen();
    size_t arm::app::TestModel::ModelSize()
    {
        return GetModelLen();
    }

#endif /* defined(DYNAMIC_MODEL_BASE) && defined(DYNAMIC_MODEL_SIZE) */
