/*
 * SPDX-FileCopyrightText: Copyright 2021, 2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef INF_RUNNER_TESTMODEL_HPP
#define INF_RUNNER_TESTMODEL_HPP

#include "Model.hpp"
#include "MicroMutableAllOpsResolver.hpp"

namespace arm {
namespace app {

    class TestModel : public Model {

    protected:
        /** @brief   Gets the reference to op resolver interface class. */
        const tflite::MicroMutableOpResolver<kNumberOperators>& GetOpResolver() override;

        /** @brief   Adds operations to the op resolver instance, not needed as using AllOpsResolver. */
        bool EnlistOperations() override {return false;}

    private:

        /* No need to define individual ops at the cost of extra memory. */
        tflite::MicroMutableOpResolver<kNumberOperators> m_opResolver = CreateAllOpsResolver();
    };

} /* namespace app */
} /* namespace arm */

#endif /* INF_RUNNER_TESTMODEL_HPP */
