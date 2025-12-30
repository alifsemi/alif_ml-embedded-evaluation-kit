#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022, 2025 Arm Limited and/or
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

set(MLEK_LOG_TARGET mlek_log)

project(${MLEK_LOG_TARGET}
    DESCRIPTION     "Generic logging formatting header-only interface lib."
    LANGUAGES       C)

set(MLEK_LOG_ENABLE ON
    CACHE BOOL "Enable MLEK logging functions. Set this to off to disable all logging.")

set(MLEK_LOG_LEVEL  MLEK_LOG_LEVEL_INFO
    CACHE STRING "Log level.")

set_property(CACHE MLEK_LOG_LEVEL PROPERTY STRINGS
    MLEK_LOG_LEVEL_TRACE
    MLEK_LOG_LEVEL_DEBUG
    MLEK_LOG_LEVEL_INFO
    MLEK_LOG_LEVEL_WARN
    MLEK_LOG_LEVEL_ERROR)

add_library(${MLEK_LOG_TARGET} INTERFACE)

if (MLEK_LOG_ENABLE)
    target_compile_definitions(${MLEK_LOG_TARGET} INTERFACE
        MLEK_LOG_ENABLE)
    if (DEFINED MLEK_LOG_LEVEL)
        message(STATUS "Setting log level to ${MLEK_LOG_LEVEL}")
        target_compile_definitions(${MLEK_LOG_TARGET} INTERFACE
            MLEK_LOG_LEVEL=${MLEK_LOG_LEVEL})
    endif()
else()
    message(STATUS "MLEK log is disabled")
endif()

add_library(mlek::log ALIAS mlek_log)

message(STATUS "**************************************************")
message(STATUS "Library:                " ${MLEK_LOG_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "**************************************************")
