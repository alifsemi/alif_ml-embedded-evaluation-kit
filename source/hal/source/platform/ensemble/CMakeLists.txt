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
set(PLATFORM_DRIVERS_CORE platform_drivers_core)

project(${PLATFORM_DRIVERS_TARGET}
    DESCRIPTION     "Platform drivers library for Ensemble RTSS-HP/RTSS-HE targets"
    LANGUAGES       C CXX ASM)

set(CONSOLE_UART 2 CACHE STRING "Console UART for the application, or 'None'")
set(CONSOLE_ITM OFF CACHE BOOL "Send console output to ITM (hardware trace system)")
set(CONSOLE_STM OFF) # M55 cores cannot access STM, and don't really need to, as they have ITM. so no user-visible option
set(CONSOLE_SEMIHOSTING OFF CACHE BOOL "Use debugger semihosting for console (cannot be used in conjunction with other console options)")
set(OSPI_FLASH_SUPPORT ON CACHE BOOL "Enables OSPI flash driver initialization (keep enabled when setting ML_MODEL_IN_EXT_FLASH)")

set_property(CACHE CONSOLE_UART PROPERTY STRINGS "None" "0" "1" "2" "3" "4" "5" "6" "7" "LP")

set(GLCD_UI ON CACHE BOOL "Provide GLCD UI as used by Arm's demos - can be turned off if only using LVGL to save RAM")

set(COPY_VECTORS ON CACHE BOOL "Copy vectors - can be turned off if not booting from MRAM")

# 1. We should be cross-compiling (Ensemble target only runs Cortex-M/A targets)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${PLATFORM_DRIVERS_TARGET} support for this target.")
endif()

set(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED OFF)

# Define target specific base addresses here (before adding the components)
if (TARGET_SUBSYSTEM STREQUAL RTSS-HP)
    set(SYSTEM_CORE_CLOCK    "400000000"    CACHE STRING "System peripheral clock (Hz)")
endif()

# Define target specific base addresses here (before adding the components)
if (TARGET_SUBSYSTEM STREQUAL RTSS-HE)
    set(SYSTEM_CORE_CLOCK    "160000000"     CACHE STRING "System peripheral clock (Hz)")
endif()

set(DYNAMIC_MODEL_BASE   "0x90000000"   CACHE STRING "Region to be used for dynamic load of model into memory")
set(DYNAMIC_MODEL_SIZE   "0x02000000"   CACHE STRING "Size of the space reserved for the model")

math(EXPR IFM_BASE      "${DYNAMIC_MODEL_BASE} + ${DYNAMIC_MODEL_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_IFM_BASE    "${IFM_BASE}" CACHE STRING "Base address for IFMs to be loaded")
set(DYNAMIC_IFM_SIZE    "0x01000000" CACHE STRING "Size of the space reserved for the IFM")
math(EXPR OFM_BASE      "${DYNAMIC_IFM_BASE} + ${DYNAMIC_IFM_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_OFM_BASE    "${OFM_BASE}" CACHE STRING "Base address for OFMs to be dumped to")
set(DYNAMIC_OFM_SIZE    "0x01000000" CACHE STRING "Size of the space reserved for the OFM")

#set(CMAKE_ASM_COMPILE_OBJECT    ${CMAKE_CXX_FLAGS})

# 2. Create static libraries
add_library(${PLATFORM_DRIVERS_TARGET} INTERFACE)
add_library(${PLATFORM_DRIVERS_CORE} STATIC)

## Include directories - private
target_include_directories(${PLATFORM_DRIVERS_CORE}
    PRIVATE
    source)

## Include directories - public
target_include_directories(${PLATFORM_DRIVERS_CORE}
    PUBLIC
    include
    include/${TARGET_SUBSYSTEM}
    )

# Interface includes
add_library(${PLATFORM_DRIVERS_INTERFACE} INTERFACE)

target_include_directories(${PLATFORM_DRIVERS_INTERFACE}
INTERFACE
include
)

target_compile_definitions(${PLATFORM_DRIVERS_CORE} PRIVATE
    CONSOLE_UART=${CONSOLE_UART}
    $<$<NOT:$<STREQUAL:${CONSOLE_UART},None>>:USE_UART>
    $<$<BOOL:${CONSOLE_STM}>:USE_STM>
    $<$<BOOL:${CONSOLE_ITM}>:USE_ITM>
    $<$<BOOL:${OSPI_FLASH_SUPPORT}>:OSPI_FLASH_SUPPORT>
    # USE_SEMIHOSTING is handled via configure_semihosting in the CMSIS pack
)

if (COPY_VECTORS)
    target_compile_definitions(${PLATFORM_DRIVERS_CORE} PRIVATE COPY_VECTORS)
endif()

## Platform sources
target_sources(${PLATFORM_DRIVERS_CORE}
    PRIVATE
    source/fault_handler.c
    source/timer_ensemble.c
    source/platform_drivers.c
    $<$<NOT:$<STREQUAL:${CONSOLE_UART},None>>:source/uart_tracelib.c>
    $<$<BOOL:${CONSOLE_STM}>:source/stm_tracelib.c>
    $<$<BOOL:${CONSOLE_ITM}>:source/itm_tracelib.c>
    source/tracelib.c
    source/retarget_ensemble.c
    source/delay.c
    $<$<NOT:$<STREQUAL:${TARGET_REVISION},A>>:source/ospi_flash.c>
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

## Platform component: lcd
add_subdirectory(${COMPONENTS_DIR}/lcd ${CMAKE_BINARY_DIR}/lcd)

## Platform component: lvgl port
add_subdirectory(${COMPONENTS_DIR}/lvgl_port ${CMAKE_BINARY_DIR}/lvgl_port)

## Platform component: audio
add_subdirectory(${COMPONENTS_DIR}/audio ${CMAKE_BINARY_DIR}/audio)

## Platform component: image
add_subdirectory(${COMPONENTS_DIR}/image ${CMAKE_BINARY_DIR}/image)

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
target_link_libraries(${PLATFORM_DRIVERS_CORE} PUBLIC
    log
    platform_pmu
    cmsis_ensemble
    rte_components
)

target_link_libraries(${PLATFORM_DRIVERS_TARGET} INTERFACE
    ${PLATFORM_DRIVERS_CORE}
    image_ensemble
    $<IF:$<BOOL:${GLCD_UI}>,lcd_lvgl,lcd_stubs>
    audio_ensemble
    ensemble_services
)

add_compile_options(-Werror-implicit-function-declaration)

# Set the CPU profiling definition
if (CPU_PROFILE_ENABLED)
    target_compile_definitions(${PLATFORM_DRIVERS_CORE} PUBLIC CPU_PROFILE_ENABLED)
endif()

# If Ethos-U is enabled, we need the driver library too
if (ETHOS_U_NPU_ENABLED)

    ## Platform component: Ethos-U initialization
    add_subdirectory(${COMPONENTS_DIR}/npu ${CMAKE_BINARY_DIR}/npu)

    target_link_libraries(${PLATFORM_DRIVERS_CORE}
        PUBLIC
        ethos_u_npu)

endif()

# 3. Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${PLATFORM_DRIVERS_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_SYSTEM_ARCH                      : " ${CMAKE_SYSTEM_ARCH})
message(STATUS "*******************************************************")
