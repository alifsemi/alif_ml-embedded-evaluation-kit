#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or
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

#######################################################
# Header-only logging definitions as an interface lib.#
#######################################################
cmake_minimum_required(VERSION 3.21.0)

set(HAL_LOG_TARGET hal_log)

project(${HAL_LOG_TARGET}
    DESCRIPTION     "Generic logging formatting header-only interface lib."
    LANGUAGES       C)

set(HAL_LOG_ENABLE ON
    CACHE BOOL "Enable HAL logging functions. Set this to off to disable all logging.")

set(HAL_LOG_LEVEL  HAL_LOG_LEVEL_INFO
    CACHE STRING "Log level.")

set_property(CACHE HAL_LOG_LEVEL PROPERTY STRINGS
    HAL_LOG_LEVEL_TRACE
    HAL_LOG_LEVEL_DEBUG
    HAL_LOG_LEVEL_INFO
    HAL_LOG_LEVEL_WARN
    HAL_LOG_LEVEL_ERROR)

add_library(${HAL_LOG_TARGET} INTERFACE)
target_include_directories(${HAL_LOG_TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})

if (HAL_LOG_ENABLE)
    target_compile_definitions(${HAL_LOG_TARGET} INTERFACE
        HAL_LOG_ENABLE)
    if (DEFINED HAL_LOG_LEVEL)
        message(STATUS "Setting log level to ${HAL_LOG_LEVEL}")
        target_compile_definitions(${HAL_LOG_TARGET} INTERFACE
            HAL_LOG_LEVEL=${HAL_LOG_LEVEL})
    endif()
else()
    message(STATUS "HAL log is disabled")
endif()

message(STATUS "**************************************************")
message(STATUS "Library:                " ${HAL_LOG_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "**************************************************")
