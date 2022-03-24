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

# Define target specific base addresses here (before adding the components)
if (TARGET_SUBSYSTEM STREQUAL sse-300)
    set(UART0_BASE          "0x49303000"    CACHE STRING "UART base address")
    set(UART0_BAUDRATE      "115200"        CACHE STRING "UART baudrate")
    set(SYSTEM_CORE_CLOCK   "25000000"      CACHE STRING "System peripheral clock (Hz)")
    set(CLCD_CONFIG_BASE    "0x4930A000"    CACHE STRING "LCD configuration base address")
    set(ETHOS_U_BASE_ADDR   "0x58102000"    CACHE STRING "Ethos-U NPU base address")
    set(ETHOS_U_IRQN        "56"            CACHE STRING "Ethos-U55 Interrupt")
    set(ETHOS_U_SEC_ENABLED  "1"            CACHE STRING "Ethos-U NPU Security enable")
    set(ETHOS_U_PRIV_ENABLED "1"            CACHE STRING "Ethos-U NPU Privilege enable")
endif()

# 2. Create static library
add_library(${PLATFORM_DRIVERS_TARGET} STATIC)

## Include directories - private
target_include_directories(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source)

## Include directories - public
target_include_directories(${PLATFORM_DRIVERS_TARGET}
    PUBLIC
    include
    include/${TARGET_SUBSYSTEM})

## Platform sources
target_sources(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source/timer_mps3.c
    source/platform_drivers.c)

## Directory for additional components required by MPS3:
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

# Add dependencies:
target_link_libraries(${PLATFORM_DRIVERS_TARGET} PUBLIC
    log
    cmsis_device
    lcd_mps3
    $<IF:$<BOOL:STDOUT_RETARGET>,stdout_retarget_cmsdk,stdout>)

# If Ethos-U is enabled, we need the driver library too
if (ETHOS_U_NPU_ENABLED)

    ## Platform component: Ethos-U initialization
    add_subdirectory(${COMPONENTS_DIR}/npu ${CMAKE_BINARY_DIR}/npu)

    target_link_libraries(${PLATFORM_DRIVERS_TARGET}
        PUBLIC
        ethos_u_npu)

    if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
        ## Platform component: Ethos-U timing adapter initialization
        add_subdirectory(${COMPONENTS_DIR}/npu_ta ${CMAKE_BINARY_DIR}/npu_ta)

        target_link_libraries(${PLATFORM_DRIVERS_TARGET}
            PUBLIC
            ethos_u_ta)
    endif()

endif()

# 3. Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${PLATFORM_DRIVERS_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
