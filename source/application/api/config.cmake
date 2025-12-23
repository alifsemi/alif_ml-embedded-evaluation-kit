#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
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
include_guard(GLOBAL)

message(STATUS "Including default API config parameters...")

if (NOT COMMAND assert_defined)
    function(assert_defined var_name)
        if (NOT DEFINED ${var_name})
            message(FATAL_ERROR "ERROR: ${var_name} is undefined!")
        endif()
    endfunction()
endif()

if (NOT DEFINED MLEK_ROOT)
    set(MLEK_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../)
endif()

if (NOT DEFINED MLEK_DEPENDENCIES_DIR)
    set(MLEK_DEPENDENCIES_DIR ${MLEK_ROOT}/dependencies)
endif()

if (NOT DEFINED MLEK_SCRIPTS_DIR)
    set(MLEK_SCRIPTS_DIR ${MLEK_ROOT}/scripts)
endif()

# Add CMake scripts to module path
list(APPEND CMAKE_MODULE_PATH ${MLEK_ROOT}/scripts/cmake)

# CMSIS sources
set(CMSIS_DSP_SRC_PATH ${MLEK_DEPENDENCIES_DIR}/cmsis-dsp
    CACHE PATH "CMSIS-DSP path")
set(CMSIS_SRC_PATH ${MLEK_DEPENDENCIES_DIR}/cmsis-6
    CACHE PATH "CMSIS path")
set(CORTEX_DFP_SRC_PATH ${MLEK_DEPENDENCIES_DIR}/cortex-dfp
    CACHE PATH "Cortex DFP path")

# TensorFlow Lite Micro
set(TENSORFLOW_LITE_MICRO_BUILD_TYPE release_with_logs
    CACHE STRING "Build type")
set(TENSORFLOW_SRC_PATH ${MLEK_DEPENDENCIES_DIR}/tensorflow
    CACHE PATH "TensorFlow source path")
set(CMSIS_NN_SRC_PATH ${MLEK_DEPENDENCIES_DIR}/cmsis-nn
    CACHE PATH "CMSIS-NN path")

# ExecuTorch
set(EXECUTORCH_SRC_PATH ${MLEK_ROOT}/dependencies/executorch
    CACHE PATH "ExecuTorch source path")
set(RESOURCES_PATH ${MLEK_ROOT}/resources_downloaded
    CACHE PATH "Path for resources")

# ExecuTorch set up requires Python virtual env set up.
if (NOT COMMAND setup_source_geberator)
    include(source_gen_utils)
    setup_source_generator()
endif()
