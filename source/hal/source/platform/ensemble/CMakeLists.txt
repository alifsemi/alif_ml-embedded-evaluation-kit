# /* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

# /* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
#  * Use, distribution and modification of this code is permitted under the
#  * terms stated in the Alif Semiconductor Software License Agreement
#  *
#  * You should have received a copy of the Alif Semiconductor Software
#  * License Agreement with this file. If not, please write to:
#  * contact@alifsemi.com, or visit: https://alifsemi.com/license
#  *
#  */

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
#           Ensemble platform support library           #
#########################################################

cmake_minimum_required(VERSION 3.16.3)
set(PLATFORM_DRIVERS_TARGET platform_drivers)
set(PLATFORM_DRIVERS_INTERFACE platform_drivers_if)

project(${PLATFORM_DRIVERS_TARGET}
    DESCRIPTION     "Platform drivers library for Ensemble RTSS-HP/RTSS-HE targets"
    LANGUAGES       C CXX ASM)

# This currently is also used to infer roles for the dual-CPU bits
set(CONSOLE_UART 2 CACHE STRING "Console UART for the application")

set(GLCD_UI ON CACHE BOOL "Provide GLCD UI as used by Arm's demos - can be turned off if only using LVGL to save RAM")

# 1. We should be cross-compiling (Ensemble target only runs Cortex-M/A targets)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${PLATFORM_DRIVERS_TARGET} support for this target.")
endif()

set(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED OFF)

# Define target specific base addresses here (before adding the components)
if (TARGET_SUBSYSTEM STREQUAL RTSS-HP)
    set(ENSEMBLE_CORE       "M55_HP")
    #set(UART0_BASE           "0x49303000"   CACHE STRING "UART base address")
    #set(UART0_BAUDRATE       "115200"       CACHE STRING "UART baudrate")
    set(SYSTEM_CORE_CLOCK    "400000000"    CACHE STRING "System peripheral clock (Hz)")
    #set(CLCD_CONFIG_BASE     "0x4930A000"   CACHE STRING "LCD configuration base address")

    # if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
    #     set(TA0_BASE         "0x58103000"   CACHE STRING "Ethos-U NPU timing adapter 0")
    #     set(TA1_BASE         "0x58103200"   CACHE STRING "Ethos-U NPU timing adapter 1")
    # endif()
endif()

# Define target specific base addresses here (before adding the components)
if (TARGET_SUBSYSTEM STREQUAL RTSS-HE)
    set(ENSEMBLE_CORE       "M55_HE")
    #set(UART0_BASE           "0x49303000"   CACHE STRING "UART base address")
    #set(UART0_BAUDRATE       "115200"       CACHE STRING "UART baudrate")
    set(SYSTEM_CORE_CLOCK    "160000000"     CACHE STRING "System peripheral clock (Hz)")
    #set(CLCD_CONFIG_BASE     "0x4930A000"   CACHE STRING "LCD configuration base address")

    # if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
    #     set(TA0_BASE         "0x58103000"   CACHE STRING "Ethos-U NPU timing adapter 0")
    #     set(TA1_BASE         "0x58103200"   CACHE STRING "Ethos-U NPU timing adapter 1")
    # endif()
endif()

set(DYNAMIC_MODEL_BASE   "0x90000000"   CACHE STRING "Region to be used for dynamic load of model into memory")
set(DYNAMIC_MODEL_SIZE   "0x02000000"   CACHE STRING "Size of the space reserved for the model")

math(EXPR IFM_BASE      "${DYNAMIC_MODEL_BASE} + ${DYNAMIC_MODEL_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_IFM_BASE    "${IFM_BASE}" CACHE STRING "Base address for IFMs to be loaded")
set(DYNAMIC_IFM_SIZE    "0x01000000" CACHE STRING "Size of the space reserved for the IFM")
math(EXPR OFM_BASE      "${DYNAMIC_IFM_BASE} + ${DYNAMIC_IFM_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_OFM_BASE    "${OFM_BASE}" CACHE STRING "Base address for OFMs to be dumped to")
set(DYNAMIC_OFM_SIZE    "0x01000000" CACHE STRING "Size of the space reserved for the OFM")

add_compile_definitions("${ENSEMBLE_CORE}")
#set(CMAKE_ASM_COMPILE_OBJECT    ${CMAKE_CXX_FLAGS})

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
    include/${TARGET_SUBSYSTEM}
    )

# Interface includes
add_library(${PLATFORM_DRIVERS_INTERFACE} INTERFACE)

target_include_directories(${PLATFORM_DRIVERS_INTERFACE}
INTERFACE
include
include/${TARGET_SUBSYSTEM}
)

target_compile_definitions(${PLATFORM_DRIVERS_TARGET} PUBLIC
    CONSOLE_UART=${CONSOLE_UART}
)

## Platform sources
target_sources(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source/timer_ensemble.c
    source/platform_drivers.c
    source/uart_tracelib.c
    source/retarget_ensemble.c
    source/delay.c
    )

## Directory for additional components required by MPS3:
if (NOT DEFINED COMPONENTS_DIR)
    set(COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../components)
endif()

# Ensemble services lib to have IPC communication
add_subdirectory(services_lib ${CMAKE_BINARY_DIR}/services_lib)

## Platform component: cmsis_device (provides generic Cortex-M start up library)
add_subdirectory(cmsis-pack ${CMAKE_BINARY_DIR}/cmsis_device)

## Platform component: stdout
set(STDOUT_RETARGET OFF CACHE BOOL "Retarget stdout/err to UART" FORCE)
add_subdirectory(${COMPONENTS_DIR}/stdout ${CMAKE_BINARY_DIR}/stdout)

## Platform component: audio
add_subdirectory(${COMPONENTS_DIR}/audio ${CMAKE_BINARY_DIR}/audio)

## Platform component image
add_subdirectory(${COMPONENTS_DIR}/image ${CMAKE_BINARY_DIR}/image)

## Platform component: lcd
add_subdirectory(${COMPONENTS_DIR}/lcd ${CMAKE_BINARY_DIR}/lcd)

## Platform component: PMU
add_subdirectory(${COMPONENTS_DIR}/platform_pmu ${CMAKE_BINARY_DIR}/platform_pmu)


## Logging utilities:
if (NOT TARGET log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

# Add dependencies:
target_link_libraries(${PLATFORM_DRIVERS_TARGET} PUBLIC
    log
    image_ensemble
    $<IF:$<BOOL:${GLCD_UI}>,lcd_lvgl,lcd_stubs>
    ensemble_display
    audio_ensemble
    ensemble_services
    cmsis_ensemble
    platform_pmu
    rte_components
)

add_compile_options(-Werror-implicit-function-declaration)

# Set the CPU profiling definition
if (CPU_PROFILE_ENABLED)
    target_compile_definitions(${PLATFORM_DRIVERS_TARGET} PUBLIC CPU_PROFILE_ENABLED)
endif()

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
message(STATUS "CMAKE_SYSTEM_ARCH                      : " ${CMAKE_SYSTEM_ARCH})
message(STATUS "*******************************************************")
