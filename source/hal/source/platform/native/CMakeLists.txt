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

#########################################################
#    Native target platform support library             #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

set(PLATFORM_DRIVERS_TARGET platform_drivers)

project(${PLATFORM_DRIVERS_TARGET}
    DESCRIPTION     "Platform drivers library for native target"
    LANGUAGES       C CXX)

# We should not be cross-compiling
if (${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "Native drivers not available when cross-compiling.")
endif()

# Create static library
add_library(${PLATFORM_DRIVERS_TARGET} STATIC)

## Include directories - public
target_include_directories(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    include)

## Platform sources
target_sources(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source/platform_drivers.c
    source/timer_native.c)

## Platform component directory
if (NOT DEFINED COMPONENTS_DIR)
    set(COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../components)
endif()

## Platform component: stdout
set(STDOUT_RETARGET OFF CACHE BOOL "Retarget stdout/err to UART")
add_subdirectory(${COMPONENTS_DIR}/stdout ${CMAKE_BINARY_DIR}/stdout)

## Platform component: lcd
add_subdirectory(${COMPONENTS_DIR}/lcd ${CMAKE_BINARY_DIR}/lcd)

## Platform component: audio
add_subdirectory(${COMPONENTS_DIR}/audio ${CMAKE_BINARY_DIR}/audio)

## Platform component: image
add_subdirectory(${COMPONENTS_DIR}/image ${CMAKE_BINARY_DIR}/image)

## Platform component: PMU
add_subdirectory(${COMPONENTS_DIR}/platform_pmu ${CMAKE_BINARY_DIR}/platform_pmu)

# Add dependencies:
target_link_libraries(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    log
    platform_pmu
    stdout
    lcd_stubs
    audio_stubs
    image_stubs)

# Display status:
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${PLATFORM_DRIVERS_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
