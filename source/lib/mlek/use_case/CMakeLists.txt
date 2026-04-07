#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
#  SPDX-License-Identifier: Apache-2.0
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#----------------------------------------------------------------------------

# Add individual use case APIs but don't include them in default build
set(_MLEK_API_LIST
    asr kws noise_reduction         # Voice/Audio pipelines
    img_class object_detection vww  # Vision pipelines
    ad                              # Vibration pipeline
    inference_runner)               # Generic runner - minimal inference

foreach(_api_name ${_MLEK_API_LIST})
    add_subdirectory(${_api_name} EXCLUDE_FROM_ALL)
endforeach()

# Expose the list to parent CMake project
set(MLEK_API_LIST ${_MLEK_API_LIST} PARENT_SCOPE)
