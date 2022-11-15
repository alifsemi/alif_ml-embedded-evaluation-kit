#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

set(BSP_LOGGING_TARGET log)

project(${BSP_LOGGING_TARGET}
    DESCRIPTION     "Generic logging formatting header-only interface lib."
    LANGUAGES       C)

add_library(${BSP_LOGGING_TARGET} INTERFACE)

if (DEFINED LOG_LEVEL)
    message(STATUS "Setting log level to ${LOG_LEVEL}")
    target_compile_definitions(${BSP_LOGGING_TARGET}
        INTERFACE
        LOG_LEVEL=${LOG_LEVEL})
endif()

target_include_directories(${BSP_LOGGING_TARGET} INTERFACE include)

message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${BSP_LOGGING_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
