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
#ifndef KWS_EVT_HANDLER_HPP
#define KWS_EVT_HANDLER_HPP

#include "AppContext.hpp"

// TODO, give as command line parameter
#define AUDIO_SOURCE_CONTINOUS 1

namespace arm {
namespace app {

#ifdef AUDIO_SOURCE_CONTINOUS
/**
     * @brief       Handles the inference event for data coming from microphone or some other continuos source.
     * @param[in]   ctx         Pointer to the application context.
     * @return      true or false based on execution success.
     **/
    bool ClassifyAudioHandler(ApplicationContext& ctx);

#else
    /**
     * @brief       Handles the inference event.
     * @param[in]   ctx         Pointer to the application context.
     * @param[in]   clipIndex   Index to the audio clip to classify.
     * @param[in]   runAll      Flag to request classification of all the available audio clips.
     * @return      true or false based on execution success.
     **/
    bool ClassifyAudioHandler(ApplicationContext& ctx, uint32_t clipIndex, bool runAll);
#endif // AUDIO_SOURCE_CONTINOUS


} /* namespace app */
} /* namespace arm */

#endif /* KWS_EVT_HANDLER_HPP */
