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
#         Ethos-U NPU initialization library            #
#########################################################

cmake_minimum_required(VERSION 3.15.6)
set(ETHOS_U_NPU_INIT_COMPONENT ethosu_npu_init_component)
project(${ETHOS_U_NPU_INIT_COMPONENT}
    DESCRIPTION     "Ethos-U NPU initialization library"
    LANGUAGES       C CXX ASM)

if (NOT DEFINED ETHOS_U_NPU_DRIVER_SRC_PATH)
    message(FATAL_ERROR "ETHOS_U_NPU_DRIVER_SRC_PATH should"
            " be defined when ETHOS_U_NPU_ENABLED=${ETHOS_U_NPU_ENABLED}")
endif()

# For the driver, we need to provide the CMSIS_PATH variable
set(CMSIS_PATH ${CMSIS_SRC_PATH} CACHE PATH "Path to CMSIS directory")

# Driver needs to know what MAC configuration to build for.
if(ETHOS_U_NPU_CONFIG_ID MATCHES "^[A-Z]([0-9]+$)")
    set(ETHOSU_MACS ${CMAKE_MATCH_1})
else()
    message(FATAL_ERROR "Couldn't work out Ethos-U number of MACS from ${ETHOS_U_NPU_CONFIG_ID}")
endif()
set(ETHOSU_TARGET_NPU_CONFIG
        "ethos-${ETHOS_U_NPU_ID}-${ETHOSU_MACS}" CACHE STRING "Target Ethos-U configuration for driver.")

## Memory mode target definition
if (NOT DEFINED ETHOS_U_NPU_ID)
    set(ETHOS_U_NPU_ID U55)
endif()

if (NOT DEFINED ETHOS_U_NPU_MEMORY_MODE)
    set(ETHOS_U_NPU_MEMORY_MODE Shared_Sram)
endif()

if (ETHOS_U_NPU_MEMORY_MODE STREQUAL Sram_Only)
    if (ETHOS_U_NPU_ID STREQUAL U55)
        set(ETHOS_U_NPU_MEMORY_MODE_FLAG "-DETHOS_U_NPU_MEMORY_MODE=ETHOS_U_NPU_MEM_MODE_SRAM_ONLY")
    else ()
        message(FATAL_ERROR "Non compatible Ethos-U NPU memory mode and processor ${ETHOS_U_NPU_MEMORY_MODE} - ${ETHOS_U_NPU_ID}. `sram_only` can be used only for Ethos-U55.")
    endif ()
elseif (ETHOS_U_NPU_MEMORY_MODE STREQUAL Shared_Sram)
    # Shared Sram can be used for Ethos-U55 and Ethos-U65
    set(ETHOS_U_NPU_MEMORY_MODE_FLAG "-DETHOS_U_NPU_MEMORY_MODE=ETHOS_U_NPU_MEMORY_MODE_SHARED_SRAM")
elseif (ETHOS_U_NPU_MEMORY_MODE STREQUAL Dedicated_Sram)
    # Dedicated Sram is used only for Ethos-U65
    if (ETHOS_U_NPU_ID STREQUAL U65)
        list(APPEND ETHOS_U_NPU_MEMORY_MODE_FLAG "-DETHOS_U_NPU_MEMORY_MODE=ETHOS_U_NPU_MEMORY_MODE_DEDICATED_SRAM" "-DETHOS_U_NPU_CACHE_SIZE=${ETHOS_U_NPU_CACHE_SIZE}")
    else ()
        message(FATAL_ERROR "Non compatible Ethos-U NPU memory mode and processor ${ETHOS_U_NPU_MEMORY_MODE} - ${ETHOS_U_NPU_ID}. `dedicated_sram` can be used only for Ethos-U65.")
    endif ()
else ()
    message(FATAL_ERROR "Non compatible Ethos-U NPU memory mode ${ETHOS_U_NPU_MEMORY_MODE}")
endif ()

add_subdirectory(${ETHOS_U_NPU_DRIVER_SRC_PATH} ${CMAKE_BINARY_DIR}/ethos-u-driver)

# Create static library
add_library(${ETHOS_U_NPU_INIT_COMPONENT} STATIC)

## Include directories - public
target_include_directories(${ETHOS_U_NPU_INIT_COMPONENT}
    PUBLIC
    include
    ${SOURCE_GEN_DIR})

## Component sources
target_sources(${ETHOS_U_NPU_INIT_COMPONENT}
    PRIVATE
    ethosu_npu_init.c)

## Add dependencies:
target_link_libraries(${ETHOS_U_NPU_INIT_COMPONENT} PUBLIC
    cmsis_device
    ethosu_core_driver
    log)

target_compile_definitions(${ETHOS_U_NPU_INIT_COMPONENT}
    PUBLIC
    ${ETHOS_U_NPU_MEMORY_MODE_FLAG})

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${ETHOS_U_NPU_INIT_COMPONENT})
message(STATUS "*******************************************************")
