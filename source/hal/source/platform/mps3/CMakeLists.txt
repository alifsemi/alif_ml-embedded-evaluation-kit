#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022, 2024 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
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

cmake_minimum_required(VERSION 3.21.0)
set(PLATFORM_DRIVERS_TARGET platform_drivers)
project(${PLATFORM_DRIVERS_TARGET}
    DESCRIPTION     "Platform drivers library for MPS3 FPGA/FVP targets"
    LANGUAGES       C CXX ASM)

# 1. We should be cross-compiling (MPS3 target only runs Cortex-M targets)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${PLATFORM_DRIVERS_TARGET} support for this target.")
endif()

# Define target specific base addresses here (before adding the components)
if (TARGET_SUBSYSTEM STREQUAL sse-300)
    set(UART0_BASE           "0x49303000"   CACHE STRING "UART base address")
    set(UART0_BAUDRATE       "115200"       CACHE STRING "UART baudrate")
    set(CLCD_CONFIG_BASE     "0x4930A000"   CACHE STRING "LCD configuration base address")
    set(ETHOS_U_IRQN         "56"           CACHE STRING "Ethos-U55 Interrupt")
    set(ETHOS_U_SEC_ENABLED  "1"            CACHE STRING "Ethos-U NPU Security enable")
    set(ETHOS_U_PRIV_ENABLED "1"            CACHE STRING "Ethos-U NPU Privilege enable")

    if (ETHOS_U_SEC_ENABLED)
        set(ETHOS_U_BASE_ADDR       "0x58102000"   CACHE STRING "Ethos-U NPU base address")
        if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
            set(TA_SRAM_BASE        "0x58103000"   CACHE STRING "Ethos-U NPU SRAM timing adapter")
            set(TA_EXT_BASE         "0x58103200"   CACHE STRING "Ethos-U NPU EXT timing adapter")
        endif()
    else ()
        set(ETHOS_U_BASE_ADDR       "0x48102000"   CACHE STRING "Ethos-U NPU base address")
        if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
            set(TA_SRAM_BASE        "0x48103000"   CACHE STRING "Ethos-U NPU SRAM timing adapter")
            set(TA_EXT_BASE         "0x48103200"   CACHE STRING "Ethos-U NPU EXT timing adapter")
        endif()
    endif()
endif()

# Define target specific base addresses here (before adding the components)
if (TARGET_SUBSYSTEM STREQUAL sse-310)
    set(UART0_BASE           "0x49303000"   CACHE STRING "UART base address")
    set(UART0_BAUDRATE       "115200"       CACHE STRING "UART baudrate")
    set(CLCD_CONFIG_BASE     "0x4930A000"   CACHE STRING "LCD configuration base address")

    set(ETHOS_U_IRQN         "16"           CACHE STRING "Ethos-U55 Interrupt")
    set(ETHOS_U_SEC_ENABLED  "1"            CACHE STRING "Ethos-U NPU Security enable")
    set(ETHOS_U_PRIV_ENABLED "1"            CACHE STRING "Ethos-U NPU Privilege enable")

    if (ETHOS_U_SEC_ENABLED)
        set(ETHOS_U_BASE_ADDR    "0x50004000"   CACHE STRING "Ethos-U NPU base address")
    else ()
        set(ETHOS_U_BASE_ADDR    "0x40004000"   CACHE STRING "Ethos-U NPU base address")
    endif ()

    if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
        message(FATAL_ERROR "Timing adapter support should be disabled for ${TARGET_SUBSYSTEM}")
    endif()
endif()

set(DYNAMIC_MODEL_BASE   "0x90000000"   CACHE STRING "Region to be used for dynamic load of model into memory")
set(DYNAMIC_MODEL_SIZE   "0x02000000"   CACHE STRING "Size of the space reserved for the model")

math(EXPR IFM_BASE      "${DYNAMIC_MODEL_BASE} + ${DYNAMIC_MODEL_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_IFM_BASE    "${IFM_BASE}" CACHE STRING "Base address for IFMs to be loaded")
set(DYNAMIC_IFM_SIZE    "0x01000000" CACHE STRING "Size of the space reserved for the IFM")
math(EXPR OFM_BASE      "${DYNAMIC_IFM_BASE} + ${DYNAMIC_IFM_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_OFM_BASE    "${OFM_BASE}" CACHE STRING "Base address for OFMs to be dumped to")
set(DYNAMIC_OFM_SIZE    "0x01000000" CACHE STRING "Size of the space reserved for the OFM")


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

## Platform component: audio
add_subdirectory(${COMPONENTS_DIR}/audio_alif ${CMAKE_BINARY_DIR}/audio_alif)

## Platform component: image
add_subdirectory(${COMPONENTS_DIR}/image ${CMAKE_BINARY_DIR}/image)

## Platform component: PMU
add_subdirectory(${COMPONENTS_DIR}/platform_pmu ${CMAKE_BINARY_DIR}/platform_pmu)

## Platform component: Camera interface
add_subdirectory(${COMPONENTS_DIR}/camera ${CMAKE_BINARY_DIR}/camera)

## Platform component: Audio interface
add_subdirectory(${COMPONENTS_DIR}/audio ${CMAKE_BINARY_DIR}/audio)

# Add the platform headers to the CMSIS device target (provide access to platform's
# memory map and interrupt numbers.
target_include_directories(cmsis_device PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${TARGET_SUBSYSTEM})

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
    cmsis_device
    platform_pmu
    lcd_mps3
    hal_audio_static_streams
    $<IF:$<BOOL:${FVP_VSI_ENABLED}>,hal_camera_vsi,hal_camera_static_images>
    audio_stubs
    $<IF:$<BOOL:STDOUT_RETARGET>,stdout_retarget_cmsdk,stdout>)

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
message(STATUS "*******************************************************")
