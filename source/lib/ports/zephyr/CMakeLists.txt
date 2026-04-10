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
if (NOT CONFIG_MLEK_EXAMPLES_LIB)
    return()
endif()

project(MLEK_API_ZEPHYR_PORT
    DESCRIPTION "MLEK platform agnostic API port for Zephyr"
    LANGUAGES   CXX
    VERSION     0.1.0)

macro(mlek_config_err dependency)
    message(FATAL_ERROR "To use MLEK examples, ${dependency} must be enabled"
                        "in Zephyr configuration")
endmacro()

if (NOT CONFIG_CMSIS_DSP)
    mlek_config_err(CONFIG_CMSIS_DSP)
endif()

if (TARGET modules__cmsis-dsp)
    add_library(arm::cmsis-dsp ALIAS modules__cmsis-dsp)
else()
    message(FATAL_ERROR "Target modules__cmsis-dsp not found.")
endif()

if (CONFIG_MLEK_EXAMPLES_FWK_EXECUTORCH)
    set(ML_FRAMEWORK "ExecuTorch")
    if (NOT CONFIG_EXECUTORCH)
        mlek_config_err(CONFIG_EXECUTORCH)
    endif()

    if (TARGET modules__executorch)
        add_library(meta::executorch ALIAS modules__executorch)
    else()
        message(FATAL_ERROR "Target modules__executorch not found.")
    endif()
elseif (CONFIG_MLEK_EXAMPLES_FWK_TFLM)
    set(ML_FRAMEWORK "TensorFlowLiteMicro")
    if (NOT CONFIG_TENSORFLOW_LITE_MICRO)
        mlek_config_err(CONFIG_TENSORFLOW_LITE_MICRO)
    endif()

    if (TARGET modules__tflite-micro)
        add_library(google::tensorflow-lite-micro ALIAS modules__tflite-micro)
    else()
        message(FATAL_ERROR "Target modules__tflite-micro not found.")
    endif()
else()
    message(FATAL_ERROR "No ML framework enabled for MLEK to use in the config.")
endif()

set(MLEK_LOG_ENABLE ${CONFIG_MLEK_EXAMPLES_LOG_ENABLE} CACHE BOOL "Enable MLEK log")

# If logging has been enabled.
if (CONFIG_MLEK_EXAMPLES_LOG_ENABLE)
    if (CONFIG_MLEK_EXAMPLES_LOG_LEVEL_TRACE)
        set(_MLEK_LOG_LEVEL MLEK_LOG_LEVEL_TRACE)
    elseif (CONFIG_MLEK_EXAMPLES_LOG_LEVEL_DEBUG)
        set(_MLEK_LOG_LEVEL MLEK_LOG_LEVEL_DEBUG)
    elseif (CONFIG_MLEK_EXAMPLES_LOG_LEVEL_INFO)
        set(_MLEK_LOG_LEVEL MLEK_LOG_LEVEL_INFO)
    elseif (CONFIG_MLEK_EXAMPLES_LOG_LEVEL_WARN)
        set(_MLEK_LOG_LEVEL MLEK_LOG_LEVEL_WARN)
    elseif (CONFIG_MLEK_EXAMPLES_LOG_LEVEL_ERROR)
        set(_MLEK_LOG_LEVEL MLEK_LOG_LEVEL_ERROR)
    else()
        message(FATAL_ERROR "Unknown MLEK log level configuration.")
    endif()
    set(MLEK_LOG_LEVEL ${_MLEK_LOG_LEVEL} CACHE STRING "MLEK log level")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/mlek)
