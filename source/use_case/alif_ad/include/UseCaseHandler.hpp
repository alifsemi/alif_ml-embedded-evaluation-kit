/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef ALIF_AD_EVT_HANDLER_HPP
#define ALIF_AD_EVT_HANDLER_HPP
#include "AppContext.hpp"

namespace alif {
namespace app {
    /**
     * @brief       Handles the inference event
     * @param[in]   ctx         pointer to the application context
     * @return      True or false based on execution success
     **/
    bool ClassifyVibrationHandler(arm::app::ApplicationContext& ctx);
} /* namespace app */
} /* namespace alif */
#endif /* AD_EVT_HANDLER_H */
