/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024-2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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

#include "mlek/fwk/iface/Model.hpp"
#include "Profiler.hpp"
#include "mlek/common/Classifier.hpp"

namespace arm {
namespace app {
    /**
     * @brief           Presents inference results for image classification use cases
     *                  using the data presentation object.
     * @param[in]       results     Vector of classification results to be displayed.
     * @return          true if successful, false otherwise.
     **/
    bool PresentInferenceResult(const std::vector<ClassificationResult>& results);

    /**
     * @brief           Run inference using given model
     *                  object. If profiling is enabled, it will log the
     *                  statistics too.
     * @param[in]       model      Reference to the initialised model.
     * @param[in]       profiler   Reference to the initialised profiler.
     * @param[in]       gpioMarker If false, the GPIO power-measurement line is not toggled
     *                             around the inference (only relevant with GPIO_PROFILING).
     * @return          true if inference succeeds, false otherwise.
     **/
    bool RunInference(fwk::iface::Model& model, Profiler& profiler, bool gpioMarker = true);

#if defined(GPIO_PROFILING)
#if defined(GPIO_PROFILING_CI)
/* Automated measurement: keep the sync line idle for the first rounds, giving the automation
 * time to start waiting for the LED signal and skipping the warm-up iterations. */
#ifndef GPIO_PROFILING_CI_SKIP_ROUNDS
#define GPIO_PROFILING_CI_SKIP_ROUNDS 10
#endif
#else
/* Manual measurement: the sync line is active from the first round. */
#define GPIO_PROFILING_CI_SKIP_ROUNDS 0
#endif

    /**
     * @brief           Marks the start of a pre-processing, inference and post-processing
     *                  round for the GPIO power-measurement line. Must be called exactly once
     *                  per round.
     * @return          true if the sync line is active for this round, false while the first
     *                  GPIO_PROFILING_CI_SKIP_ROUNDS rounds are being skipped.
     **/
    bool GpioMarkerStartRound();

    /**
     * @brief           Toggles the GPIO power-measurement line if enabled. Each active round
     *                  must toggle it 4 times (HIGH: pre-processing, LOW: inference,
     *                  HIGH: post-processing, LOW: idle). The two middle edges come from
     *                  RunInference().
     * @param[in]       enabled   Value returned by GpioMarkerStartRound() for this round.
     **/
    void GpioMarkerToggle(bool enabled);
#endif /* defined(GPIO_PROFILING) */

#ifdef INTERACTIVE_MODE
    /**
     * @brief           Wait for the user to provide any input.
     **/
    void AwaitUserInput();
#endif /* INTERACTIVE_MODE */

#if VERIFY_TEST_OUTPUT
    /**
     * @brief       Helper function to dump a tensor to stdout
     * @param[in]   tensor  tensor to be dumped
     * @param[in]   lineBreakForNumElements     number of elements
     *              after which line break will be added.
     **/
    void DumpTensor(const TfLiteTensor* tensor,
                    size_t lineBreakForNumElements = 16);


    void DumpTensorData(const uint8_t* tensorData,
                        size_t size,
                        size_t lineBreakForNumElements = 16);
#endif /* VERIFY_TEST_OUTPUT */
} /* namespace app */
} /* namespace arm */


#endif /* USECASE_COMMON_UTILS_HPP */
