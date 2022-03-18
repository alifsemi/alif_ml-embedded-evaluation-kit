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
set(PLATFORM_DRIVERS_TARGET platform_drivers)
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

# Define target specific values here (before adding the components)
set(UART0_BASE          "0x49303000"    CACHE STRING "UART base address")
set(UART0_BAUDRATE      "115200"        CACHE STRING "UART baudrate")
set(SYSTEM_CORE_CLOCK   "25000000"      CACHE STRING "System peripheral clock (Hz)")
set(ACTIVATION_BUF_SRAM_SZ  "0x200000"  CACHE STRING "Maximum SRAM size for activation buffers")

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

## Include directories - public
target_include_directories(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    include
    ${SOURCE_GEN_DIR})

## Platform sources
target_sources(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source/timer_simple_platform.c
    source/platform_drivers.c)

## Directory for additional components required by generic platform:
if (NOT DEFINED COMPONENTS_DIR)
    set(COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../components)
endif()


## Platform component: cmsis_device (provides generic Cortex-M start up library)
add_subdirectory(${COMPONENTS_DIR}/cmsis_device ${CMAKE_BINARY_DIR}/cmsis_device)

## Platform component: stdout
set(STDOUT_RETARGET ON CACHE BOOL "Retarget stdout/err to UART")
add_subdirectory(${COMPONENTS_DIR}/stdout ${CMAKE_BINARY_DIR}/stdout)

## Platform component: lcd
add_subdirectory(${COMPONENTS_DIR}/lcd ${CMAKE_BINARY_DIR}/lcd)

## Compile defs
target_compile_definitions(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    ACTIVATION_BUF_SRAM_SZ=${ACTIVATION_BUF_SRAM_SZ})

# Add dependencies:
target_link_libraries(${PLATFORM_DRIVERS_TARGET}  PUBLIC
        cmsis_device
        log
        lcd_stubs
        $<IF:$<BOOL:STDOUT_RETARGET>,stdout_retarget_pl011,stdout>)

# If Ethos-U is enabled, we need the driver library too
if (ETHOS_U_NPU_ENABLED)

    ## Platform component: Ethos-U initialization
    add_subdirectory(${COMPONENTS_DIR}/npu ${CMAKE_BINARY_DIR}/npu)

    target_link_libraries(${PLATFORM_DRIVERS_TARGET}
        PUBLIC
        ethos_u_npu)

    if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
        ## Platform component: Ethos-U timing apadpter initialization
        add_subdirectory(${COMPONENTS_DIR}/npu_ta ${CMAKE_BINARY_DIR}/npu_ta)

        target_link_libraries(${PLATFORM_DRIVERS_TARGET}
            PUBLIC
            ethos_u_ta)
    endif()

endif()

# 5. Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${PLATFORM_DRIVERS_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")