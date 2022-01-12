/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#ifndef EXPECTED_OBJECT_DETECTION_RESULTS_H
#define EXPECTED_OBJECT_DETECTION_RESULTS_H

#include "DetectionResult.hpp"
#include <vector>

void get_expected_ut_results(std::vector<std::vector<arm::app::DetectionResult>> &expected_results);


#endif /* EXPECTED_OBJECT_DETECTION_RESULTS_H */
