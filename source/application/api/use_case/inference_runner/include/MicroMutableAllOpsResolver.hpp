/*
 * SPDX-FileCopyrightText: Copyright 2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef INF_RUNNER_MICRO_MUTABLE_ALL_OPS_RESOLVER_HPP
#define INF_RUNNER_MICRO_MUTABLE_ALL_OPS_RESOLVER_HPP

#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>

namespace arm {
namespace app {

    /* Maximum number of individual operations that can be enlisted. */
    constexpr int kNumberOperators = 97;

    /**  An Op resolver containing all ops is no longer supplied with TFLite Micro
     *   so we create our own instead for the generic inference runner.
     *
     * @return MicroMutableOpResolver containing all TFLite Micro Ops registered.
     */
    tflite::MicroMutableOpResolver<kNumberOperators> CreateAllOpsResolver();

} /* namespace app */
} /* namespace arm */

#endif /* INF_RUNNER_MICRO_MUTABLE_ALL_OPS_RESOLVER_HPP */
