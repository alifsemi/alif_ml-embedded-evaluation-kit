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
#ifndef USECASE_COMMON_UTILS_HPP
#define USECASE_COMMON_UTILS_HPP

#include "hal.h"
#include "Model.hpp"
#include "AppContext.hpp"
#include "Profiler.hpp"

/* Helper macro to convert RGB888 to RGB565 format. */
#define RGB888_TO_RGB565(R8,G8,B8)  ((((R8>>3) & 0x1F) << 11) |     \
                                     (((G8>>2) & 0x3F) << 5)  |     \
                                     ((B8>>3) & 0x1F))

constexpr uint16_t COLOR_BLACK  = 0;
constexpr uint16_t COLOR_GREEN  = RGB888_TO_RGB565(  0, 255,  0); // 2016;
constexpr uint16_t COLOR_YELLOW = RGB888_TO_RGB565(255, 255,  0); // 65504;

namespace arm {
namespace app {

    /**
     * @brief           Run inference using given model
     *                  object. If profiling is enabled, it will log the
     *                  statistics too.
     * @param[in]       model      Reference to the initialised model.
     * @param[in]       profiler   Reference to the initialised profiler.
     * @return          true if inference succeeds, false otherwise.
     **/
    bool RunInference(arm::app::Model& model, Profiler& profiler);

    /**
     * @brief           Read input and return as an integer.
     * @param[in]       platform   Reference to the hal platform object.
     * @return          Integer value corresponding to the user input.
     **/
    int ReadUserInputAsInt(hal_platform& platform);

#if VERIFY_TEST_OUTPUT
    /**
     * @brief       Helper function to dump a tensor to stdout
     * @param[in]   tensor  tensor to be dumped
     * @param[in]   lineBreakForNumElements     number of elements
     *              after which line break will be added.
     **/
    void DumpTensor(TfLiteTensor* tensor,
                    const size_t lineBreakForNumElements = 16);
#endif /* VERIFY_TEST_OUTPUT */

    /**
     * @brief       List the files baked in the application.
     * @param[in]   ctx   Reference to the application context.
     * @return      true or false based on event being handled.
     **/
    bool ListFilesHandler(ApplicationContext& ctx);

} /* namespace app */
} /* namespace arm */

#endif /* USECASE_COMMON_UTILS_HPP */