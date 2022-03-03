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
#  Ethos-U NPU timing adapter initialization library    #
#########################################################

cmake_minimum_required(VERSION 3.15.6)
set(ETHOS_U_NPU_TA_COMPONENT ethosu_ta_init_component)
project(${ETHOS_U_NPU_TA_COMPONENT}
    DESCRIPTION     "Ethos-U NPU timing adapter initialization library"
    LANGUAGES       C CXX ASM)

# Checks
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

add_subdirectory(${ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH} ${CMAKE_BINARY_DIR}/timing_adapter)

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

## Add dependencies
target_link_libraries(${ETHOS_U_NPU_TA_COMPONENT} PUBLIC
    timing_adapter
    log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${ETHOS_U_NPU_TA_COMPONENT})
message(STATUS "*******************************************************")
