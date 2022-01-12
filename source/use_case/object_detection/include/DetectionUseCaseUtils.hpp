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
#ifndef DETECTION_USE_CASE_UTILS_HPP
#define DETECTION_USE_CASE_UTILS_HPP

#include "hal.h"
#include "DetectionResult.hpp"
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include <inttypes.h>
#include <vector>


void DisplayDetectionMenu();

namespace image{


  /**
   * @brief           Presents inference results using the data presentation
   *                  object.
   * @param[in]       platform    Reference to the hal platform object.
   * @param[in]       results     Vector of detection results to be displayed.
   * @return          true if successful, false otherwise.
   **/
  bool PresentInferenceResult(hal_platform & platform,
                              const std::vector < arm::app::DetectionResult > & results);


  /**
   * @brief           Presents inference results along with the inference time using the data presentation
   *                  object.
   * @param[in]       platform    Reference to the hal platform object.
   * @param[in]       results     Vector of detection results to be displayed.
   * @param[in]       infTimeMs   Inference time in ms.
   * @return          true if successful, false otherwise.
   **/
  bool PresentInferenceResult(hal_platform & platform,
                              const std::vector < arm::app::DetectionResult > & results,
                              const time_t infTimeMs);

  /**
  * @brief           Presents inference results along with the inference time using the data presentation
  *                  object.
  * @param[in]       platform    Reference to the hal platform object.
  * @param[in]       results     Vector of detection results to be displayed.
  * @param[in]       infTimeMs   Inference time in ms.
  * @return          true if successful, false otherwise.
  **/
  bool PresentInferenceResult(hal_platform & platform,
                              const std::vector < arm::app::DetectionResult > & results,
                              bool profilingEnabled,
                              const time_t infTimeMs = 0);
  }




#endif /* DETECTION_USE_CASE_UTILS_HPP */
