#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022, 2025-2026 Arm Limited and/or
#  its affiliates <open-source-office@arm.com>
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

#########################################################
# This CMake project just consolidates all APIs to help #
# generate CMSIS-pack from this repository. Projects    #
# that need to be included in the pack, should be added #
# here along with their dependencies.                   #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(ml-embedded-eval-kit-lib
        DESCRIPTION     "Platform agnostic API for all examples.")

option(CMSIS_PACK_GEN_FLOW "Enable CMSIS-pack generation flow" ON)

set(MLEK_LIB_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../source/lib)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/..")
include(ml_framework)

# Add the lib but no framework implementation yet (by setting the proider
# as external).
set(MLEK_RUNTIME_PROVIDER "External")
add_subdirectory(${MLEK_LIB_PROJECT_DIR} ${CMAKE_BINARY_DIR}/mlek-lib)

# Include support for TensorFlow Lite Micro implementation.
set(ML_FRAMEWORK "${ML_FRAMEWORK_TFLM}")
add_subdirectory(${MLEK_LIB_PROJECT_DIR}/mlek/fwk/tflm
    ${CMAKE_BINARY_DIR}/mlek-lib/tflm)

# Include ExecuTorch framework implementation.
set(ML_FRAMEWORK "${ML_FRAMEWORK_EXECUTORCH}")
add_subdirectory(${MLEK_LIB_PROJECT_DIR}/mlek/fwk/executorch
    ${CMAKE_BINARY_DIR}/mlek-lib/executorch)
