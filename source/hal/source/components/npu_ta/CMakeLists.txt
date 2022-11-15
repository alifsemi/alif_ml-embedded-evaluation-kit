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
#  Ethos-U NPU timing adapter initialization library    #
#########################################################

# Timing adapter component is only available on certain implementations
# on FPGA and FVP where it is necessary to run bandwidth and latency
# sweeps on the Arm Ethos-U NPUs. The wrapper library here provides an
# easy way to add initialisation of the timing adapter block.

cmake_minimum_required(VERSION 3.21.0)
set(ETHOS_U_NPU_TA_COMPONENT ethos_u_ta)
project(${ETHOS_U_NPU_TA_COMPONENT}
    DESCRIPTION     "Ethos-U NPU timing adapter initialization library"
    LANGUAGES       C CXX ASM)

# Checks
## Source generated Source path check
if (NOT DEFINED SOURCE_GEN_DIR)
    set(SOURCE_GEN_DIR ${CMAKE_BINARY_DIR}/generated/ta)
endif()

# Base address definitions for the two timing adapters (platform should override these):
set(TA0_BASE            "0x58103000"    CACHE STRING "Ethos-U NPU timing adapter 0")
set(TA1_BASE            "0x58103200"    CACHE STRING "Ethos-U NPU timing adapter 1")

## If a TA config file is provided, we generate a settings file
if (DEFINED TA_CONFIG_FILE)
    include(${TA_CONFIG_FILE})
    set(TA_SETTINGS_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/timing_adapter_settings.template)
    configure_file("${TA_SETTINGS_TEMPLATE}" "${SOURCE_GEN_DIR}/timing_adapter_settings.h")
endif()

## Timing adapter Source path check
if (NOT DEFINED ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH)
    message(FATAL_ERROR "ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH should"
            " be defined when ETHOS_U_NPU_ENABLED=${ETHOS_U_NPU_ENABLED}")
endif()

## TA driver
add_subdirectory(${ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH} ${CMAKE_BINARY_DIR}/timing_adapter)

## Logging utilities:
if (NOT TARGET log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

# Create static library
add_library(${ETHOS_U_NPU_TA_COMPONENT} STATIC)

## Include directories - public
target_include_directories(${ETHOS_U_NPU_TA_COMPONENT}
    PUBLIC
    include
    ${SOURCE_GEN_DIR})

## Component sources
target_sources(${ETHOS_U_NPU_TA_COMPONENT}
    PRIVATE
    ethosu_ta_init.c)

## Compile definitions
target_compile_definitions(${ETHOS_U_NPU_TA_COMPONENT}
    PUBLIC
    ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)

## Add dependencies
target_link_libraries(${ETHOS_U_NPU_TA_COMPONENT} PUBLIC
    timing_adapter
    log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${ETHOS_U_NPU_TA_COMPONENT})
message(STATUS "*******************************************************")
