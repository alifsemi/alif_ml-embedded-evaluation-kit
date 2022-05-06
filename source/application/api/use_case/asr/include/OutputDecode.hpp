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
#ifndef ASR_OUTPUT_DECODE_HPP
#define ASR_OUTPUT_DECODE_HPP

#include "AsrClassifier.hpp"

namespace arm {
namespace app {
namespace audio {
namespace asr {

    /**
     * @brief       Gets the top N classification results from the
     *              output vector.
     * @param[in]   vecResults   Label output from classifier.
     * @return      true if successful, false otherwise.
    **/
    std::string DecodeOutput(const std::vector<ClassificationResult>& vecResults);

} /* namespace asr */
} /* namespace audio */
} /* namespace app */
} /* namespace arm */

#endif /* ASR_OUTPUT_DECODE_HPP */