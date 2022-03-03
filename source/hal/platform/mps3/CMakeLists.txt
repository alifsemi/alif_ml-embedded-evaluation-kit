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
#           MPS3 platform support library               #
#########################################################

cmake_minimum_required(VERSION 3.15.6)
set(PLATFORM_DRIVERS_TARGET platform_drivers)
project(${PLATFORM_DRIVERS_TARGET}
    DESCRIPTION     "Platform drivers library for MPS3 FPGA/FVP targets"
    LANGUAGES       C CXX ASM)

# 1. We should be cross-compiling (MPS3 taregt only runs Cortex-M targets)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${PLATFORM_DRIVERS_TARGET} support for this target.")
endif()

# 2. Set the platform cmake descriptor file
if (NOT DEFINED PLATFORM_CMAKE_DESCRIPTOR_FILE)
    set(PLATFORM_CMAKE_DESCRIPTOR_FILE
        cmake/subsystem-profiles/${TARGET_SUBSYSTEM}.cmake
        CACHE PATH
        "Platform's CMake descriptor file path")
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

# 4. Create static library
add_library(${PLATFORM_DRIVERS_TARGET} STATIC)

## Include directories - private
target_include_directories(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source)

## Include directories - public
target_include_directories(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    include
    ${SOURCE_GEN_DIR})

## Platform sources
target_sources(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source/device_mps3.c
    source/timer_mps3.c
    source/platform_drivers.c
    source/glcd_mps3.c)

## Directory for additional components required by MPS3:
if (NOT DEFINED COMPONENTS_DIR)
    set(COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../components)
endif()

## This target provides the following definitions for MPS3 specific behaviour
## TODO: We should aim to remove this now with platform refactoring..
target_compile_definitions(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    MPS3_PLATFORM
    ACTIVATION_BUF_SRAM_SZ=${ACTIVATION_BUF_SRAM_SZ})

## Platform component: uart
add_subdirectory(${DEPENDENCY_ROOT_DIR}/core-platform/drivers/uart ${CMAKE_BINARY_DIR}/uart)

# Add dependencies:
target_link_libraries(${PLATFORM_DRIVERS_TARGET} PUBLIC
    log
    cmsis_device
    ethosu_uart_cmsdk_apb)

# If Ethos-U is enabled, we need the driver library too
if (ETHOS_U_NPU_ENABLED)

    target_compile_definitions(${PLATFORM_DRIVERS_TARGET}
        PUBLIC
        ARM_NPU)

    ## Platform component: Ethos-U initialization
    add_subdirectory(${COMPONENTS_DIR}/ethosu_npu_init ${CMAKE_BINARY_DIR}/ethosu_npu_init)

    target_link_libraries(${PLATFORM_DRIVERS_TARGET}
        PUBLIC
        ethosu_npu_init_component)

    if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
        ## Platform component: Ethos-U timing adapter initialization
        add_subdirectory(${COMPONENTS_DIR}/ethosu_ta_init ${CMAKE_BINARY_DIR}/ethosu_ta_init)

        target_link_libraries(${PLATFORM_DRIVERS_TARGET}
            PUBLIC
            ethosu_ta_init_component)
        target_compile_definitions(${PLATFORM_DRIVERS_TARGET}
            PUBLIC
            ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
    endif()

endif()

# 5. Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${PLATFORM_DRIVERS_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
