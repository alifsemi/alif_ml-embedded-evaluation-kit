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
#include "OutputDecode.hpp"

namespace arm {
namespace app {
namespace audio {
namespace asr {

    std::string DecodeOutput(const std::vector<ClassificationResult>& vecResults)
    {
        std::string CleanOutputBuffer;

        for (size_t i = 0; i < vecResults.size(); ++i)  /* For all elements in vector. */
        {
            while (i+1 < vecResults.size() &&
                   vecResults[i].m_label == vecResults[i+1].m_label)  /* While the current element is equal to the next, ignore it and move on. */
            {
                ++i;
            }
            if (vecResults[i].m_label != "$")  /* $ is a character used to represent unknown and double characters so should not be in output. */
            {
                CleanOutputBuffer += vecResults[i].m_label;  /* If the element is different to the next, it will be appended to CleanOutputBuffer. */
            }
        }

        return CleanOutputBuffer;  /* Return string type containing clean output. */
    }

} /* namespace asr */
} /* namespace audio */
} /* namespace app */
} /* namespace arm */
