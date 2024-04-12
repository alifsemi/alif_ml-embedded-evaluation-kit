#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2021-2022, 2024 Arm Limited and/or its
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

#----------------------------------------------------------------------------
# This file should contain all the common user options for the application
# For use case specific options, see individual usecase.cmake files under
# each example use case.
#----------------------------------------------------------------------------
include_guard()

message(STATUS "Assessing common user options...")

include(util_functions)

USER_OPTION(LOG_LEVEL "Log level for the application"
    LOG_LEVEL_INFO
    STRING)

## TensorFlow options
USER_OPTION(TENSORFLOW_SRC_PATH "Path to the root of the TensorFlow Lite Micro sources"
    "${DEPENDENCY_ROOT_DIR}/tensorflow"
    PATH)

USER_OPTION(TENSORFLOW_LITE_MICRO_BUILD_TYPE "TensorFlow Lite Mirco build type (release/debug etc.)"
    $<IF:$<CONFIG:RELEASE>,release_with_logs,debug>
    STRING)

USER_OPTION(TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS "Select if TPIP downloads should be cleaned before each build."
    OFF
    BOOL)

USER_OPTION(TENSORFLOW_LITE_MICRO_CLEAN_BUILD "Select if clean target should be added to a list of targets"
    ON
    BOOL)

USER_OPTION(TARGET_PLATFORM "Target platform to build for: mps3, mps4, simple_platform and native"
    mps3
    STRING)

USER_OPTION(USE_CASE_BUILD "Optional. Defines the use-case to build from the available sources. By default, all use-cases are built."
    all
    STRING)

USER_OPTION(CPU_PROFILE_ENABLED "Output CPU performance profiling information. Should be used only for MPS3 board."
    OFF
    BOOL)

USER_OPTION(USE_SINGLE_INPUT "Select if a use case should execute using a default known input file"
    OFF
    BOOL)
