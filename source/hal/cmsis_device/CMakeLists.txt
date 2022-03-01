#----------------------------------------------------------------------------
#  Copyright (c) 2022 Arm Limited. All rights reserved.
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
# Generic CMSIS Start up library for Cortex-M targets   #
#########################################################
cmake_minimum_required(VERSION 3.15.6)

set(CMSIS_DEVICE_TARGET cmsis_device)

project(${CMSIS_DEVICE_TARGET}
    DESCRIPTION     "Generic CMSIS start up file for Cortex-M targets"
    LANGUAGES       C CXX ASM)

# 1. We should be cross-compiling (non-native target)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${CMSIS_DEVICE_TARGET} support for this target.")
endif()

# 2. Check if CMSIS sources have been defined
if (NOT DEFINED CMSIS_SRC_PATH)
    message(FATAL_ERROR "CMSIS_SRC_PATH path should be defined for ${CMSIS_DEVICE_TARGET}.")
endif()

# 3. Create static library
add_library(${CMSIS_DEVICE_TARGET} STATIC)

## Include directories - public
target_include_directories(${CMSIS_DEVICE_TARGET}
    PUBLIC
    include
    ${CMSIS_SRC_PATH}/CMSIS/Core/Include
    ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Include
    ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Include/Template)

## Sources
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Source/system_${ARM_CPU}.c
    ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Source/startup_${ARM_CPU}.c)

# Device definition needs to be set, is checked in source files to include correct header
target_compile_definitions(${CMSIS_DEVICE_TARGET} PUBLIC ${ARM_CPU})

# Tell linker that reset interrupt handler is our entry point
target_link_options(
    ${CMSIS_DEVICE_TARGET}
    INTERFACE
    --entry Reset_Handler)

# Check if semihosting configuration is available
if (COMMAND configure_semihosting)
    option(USE_SEMIHOSTING "Enable/disable semihosting option" OFF)
    configure_semihosting(${CMSIS_DEVICE_TARGET} ${USE_SEMIHOSTING})
endif()

# 4 Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${CMSIS_DEVICE_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
