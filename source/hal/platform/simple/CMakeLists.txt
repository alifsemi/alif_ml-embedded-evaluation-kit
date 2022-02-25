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
#       A generic (simple) platform support library     #
#########################################################

cmake_minimum_required(VERSION 3.15.6)
set(PLATFORM_DRIVERS_TARGET platform-drivers)
project(${PLATFORM_DRIVERS_TARGET}
    DESCRIPTION     "Platform drivers library for a generic target"
    LANGUAGES       C CXX ASM)

# 1. We should be cross-compiling (MPS3 taregt only runs Cortex-M targets)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${PLATFORM_DRIVERS_TARGET} support for this target.")
endif()

# 2. Set the platform cmake descriptor file
if (NOT DEFINED PLATFORM_CMAKE_DESCRIPTOR_FILE)
    set(PLATFORM_CMAKE_DESCRIPTOR_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/subsystem-profiles/simple_platform.cmake)
endif()

## Include the platform cmake descriptor file
include(${PLATFORM_CMAKE_DESCRIPTOR_FILE})

# 3. Generate sources:
if (NOT DEFINED SOURCE_GEN_DIR)
    set(SOURCE_GEN_DIR ${CMAKE_BINARY_DIR}/generated/bsp)
endif()

set(MEM_PROFILE_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/peripheral_memmap.h.template)
set(IRQ_PROFILE_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/peripheral_irqs.h.template)
set(MEM_REGIONS_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/mem_regions.h.template)

configure_file("${MEM_PROFILE_TEMPLATE}" "${SOURCE_GEN_DIR}/peripheral_memmap.h")
configure_file("${IRQ_PROFILE_TEMPLATE}" "${SOURCE_GEN_DIR}/peripheral_irqs.h")
configure_file("${MEM_REGIONS_TEMPLATE}" "${SOURCE_GEN_DIR}/mem_regions.h")

## If a TA config file is provided, we generate a settings file
if (DEFINED TA_CONFIG_FILE)
    include(${TA_CONFIG_FILE})
    set(TA_SETTINGS_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/templates/timing_adapter_settings.template)
    configure_file("${TA_SETTINGS_TEMPLATE}" "${SOURCE_GEN_DIR}/timing_adapter_settings.h")
endif()

# 4. Create static library
add_library(${PLATFORM_DRIVERS_TARGET} STATIC)

## Include directories - public
target_include_directories(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    include
    ${SOURCE_GEN_DIR})

## Platform sources
target_sources(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source/stubs_glcd.c
    source/timer_simple_platform.c
    source/platform_drivers.c)

## Directory for additional components required by generic platform:
if (NOT DEFINED COMPONENTS_DIR)
    set(COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../components)
endif()

## Platform component: uart
target_sources(${PLATFORM_DRIVERS_TARGET}
        PRIVATE
        ${COMPONENTS_DIR}/uart_pl011/uart_pl011.c)
target_include_directories(${PLATFORM_DRIVERS_TARGET}
        PUBLIC
        ${COMPONENTS_DIR}/uart_pl011/include)

## Compile defs
target_compile_definitions(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    ACTIVATION_BUF_SRAM_SZ=${ACTIVATION_BUF_SRAM_SZ}
    $<$<BOOL:TA_CONFIG_FILE>:TIMING_ADAPTER_AVAILABLE>)

# Add dependencies:
target_link_libraries(${PLATFORM_DRIVERS_TARGET}  PUBLIC
        cmsis_device
        log)

# If Ethos-U is enabled, we need the driver library too
if (ETHOS_U_NPU_ENABLED)

    ## Platform component: Ethos-U initialization
    target_sources(${PLATFORM_DRIVERS_TARGET}
            PRIVATE
            ${COMPONENTS_DIR}/ethosu_npu_init/ethosu_npu_init.c)
    target_include_directories(${PLATFORM_DRIVERS_TARGET}
            PUBLIC
            ${COMPONENTS_DIR}/ethosu_npu_init/include)

    ## Platform component: Ethos-U timing apadpter initialization
    target_sources(${PLATFORM_DRIVERS_TARGET}
            PRIVATE
            ${COMPONENTS_DIR}/ethosu_ta_init/ethosu_ta_init.c)
    target_include_directories(${PLATFORM_DRIVERS_TARGET}
            PUBLIC
            ${COMPONENTS_DIR}/ethosu_ta_init/include)

    if (NOT DEFINED ETHOS_U_NPU_DRIVER_SRC_PATH)
        message(FATAL_ERROR "ETHOS_U_NPU_DRIVER_SRC_PATH should"
                " be defined when ETHOS_U_NPU_ENABLED=${ETHOS_U_NPU_ENABLED}")
    endif()

    # Timing adapter
    if (NOT DEFINED ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH)
        message(FATAL_ERROR "ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH should"
                " be defined when ETHOS_U_NPU_ENABLED=${ETHOS_U_NPU_ENABLED}")
    endif()

    target_compile_definitions(${PLATFORM_DRIVERS_TARGET}
        PUBLIC
        ARM_NPU)

    # For the driver, we need to provide the CMSIS_PATH variable
    set(CMSIS_PATH ${CMSIS_SRC_PATH} CACHE PATH "Path to CMSIS directory")
    add_subdirectory(${ETHOS_U_NPU_DRIVER_SRC_PATH} ${CMAKE_BINARY_DIR}/ethos-u-driver)
    add_subdirectory(${ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH} ${CMAKE_BINARY_DIR}/timing-adapter)

    target_link_libraries(${PLATFORM_DRIVERS_TARGET}
        PUBLIC
        ethosu_core_driver
        timing_adapter)

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

    target_compile_definitions(${PLATFORM_DRIVERS_TARGET}
        PUBLIC
        ${ETHOS_U_NPU_MEMORY_MODE_FLAG})
endif()

# 5. Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${PLATFORM_DRIVERS_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
