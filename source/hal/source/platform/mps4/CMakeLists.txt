#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2024-2026 Arm Limited and/or its
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
#           MPS4 platform support library               #
#########################################################

cmake_minimum_required(VERSION 3.21.0)
set(PLATFORM_DRIVERS_TARGET platform_drivers)
project(${PLATFORM_DRIVERS_TARGET}
    DESCRIPTION     "Platform drivers library for MPS4 FPGA/FVP targets"
    LANGUAGES       C CXX ASM)

# 1. We should be cross-compiling (MPS4 target only runs Cortex-M targets)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${PLATFORM_DRIVERS_TARGET} support for this target.")
endif()

# Define target specific base addresses here (before adding the components)
if (FPGA_PLATFORM_SSE_320)
    set(UART0_BASE              "0x59303000"    CACHE STRING "UART base address")
    set(SYSTEM_CORE_CLOCK       "50000000"      CACHE STRING "System peripheral clock (Hz)")
else()
    set(UART0_BASE              "0x58203000"    CACHE STRING "UART base address")
    set(SYSTEM_CORE_CLOCK       "50000000"      CACHE STRING "System peripheral clock (Hz)")
endif()
set(UART0_BAUDRATE              "115200"        CACHE STRING "UART baudrate")

set(ETHOS_U_IRQN                "16"            CACHE STRING "Ethos-U Interrupt")
set(ETHOS_U_SEC_ENABLED         "1"             CACHE STRING "Ethos-U NPU Security enable")
set(ETHOS_U_PRIV_ENABLED        "1"             CACHE STRING "Ethos-U NPU Privilege enable")

if (ETHOS_U_SEC_ENABLED)
    set(ETHOS_U_BASE_ADDR       "0x50004000"    CACHE STRING "Ethos-U NPU base address")
    if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED AND TARGET_SUBSYSTEM STREQUAL sse-320)
        if (FPGA_PLATFORM_SSE_320)
            set(TA_SRAM_BASE    "0x51102000"    CACHE STRING "Ethos-U NPU SRAM timing adapter")
            set(TA_EXT_BASE     "0x51102400"    CACHE STRING "Ethos-U NPU EXT timing adapter")
        else()
            set(TA_SRAM_BASE    "0x50006000"    CACHE STRING "Ethos-U NPU SRAM timing adapter")
            set(TA_EXT_BASE     "0x50006800"    CACHE STRING "Ethos-U NPU EXT timing adapter")
        endif()
    endif()
else ()
    set(ETHOS_U_BASE_ADDR       "0x40004000"    CACHE STRING "Ethos-U NPU base address")
    if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED AND TARGET_SUBSYSTEM STREQUAL sse-320)
        if (FPGA_PLATFORM_SSE_320)
            set(TA_SRAM_BASE    "0x41102000"    CACHE STRING "Ethos-U NPU SRAM timing adapter")
            set(TA_EXT_BASE     "0x41102400"    CACHE STRING "Ethos-U NPU EXT timing adapter")
        else()
            set(TA_SRAM_BASE    "0x40006000"    CACHE STRING "Ethos-U NPU SRAM timing adapter")
            set(TA_EXT_BASE     "0x40006800"    CACHE STRING "Ethos-U NPU EXT timing adapter")
        endif()
    endif()
endif ()

set(DYNAMIC_MODEL_BASE   "0x90000000"   CACHE STRING "Region to be used for dynamic load of model into memory")
set(DYNAMIC_MODEL_SIZE   "0x02000000"   CACHE STRING "Size of the space reserved for the model")

math(EXPR IFM_BASE      "${DYNAMIC_MODEL_BASE} + ${DYNAMIC_MODEL_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_IFM_BASE    "${IFM_BASE}" CACHE STRING "Base address for IFMs to be loaded")
set(DYNAMIC_IFM_SIZE    "0x01000000" CACHE STRING "Size of the space reserved for the IFM")
math(EXPR OFM_BASE      "${DYNAMIC_IFM_BASE} + ${DYNAMIC_IFM_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_OFM_BASE    "${OFM_BASE}" CACHE STRING "Base address for OFMs to be dumped to")
set(DYNAMIC_OFM_SIZE    "0x01000000" CACHE STRING "Size of the space reserved for the OFM")

if (FPGA_PLATFORM_SSE_320)
    set(ML_FWK_TMP_MEM_BASE "0x72000000" CACHE STRING "Region used by ML framework for any temporary allocations")
    set(ML_FWK_TMP_MEM_SIZE "0x02000000" CACHE STRING "Buffer size for ML framework's temporary allocations")
else()
    math(EXPR TMP_BASE      "${DYNAMIC_OFM_BASE} + ${DYNAMIC_OFM_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
    set(ML_FWK_TMP_MEM_BASE "${TMP_BASE}" CACHE STRING "Region used by ML framework for any temporary allocations")
    set(ML_FWK_TMP_MEM_SIZE "0x02000000" CACHE STRING "Buffer size for ML framework's temporary allocations")
endif()

# HDLCD parameters
math(EXPR FB_BASE       "${ML_FWK_TMP_MEM_BASE} + ${ML_FWK_TMP_MEM_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(HDLCD_FRAME_BUFFER_BASE_ADDRESS "${FB_BASE}" CACHE STRING "HDLCD buffer region.")

# Although the device is capable of better resolutions,
# we currently stick to the resolution that is supported
# for MPS3 based targets to keep parity.
set(HDLCD_RES_WIDTH             320)
set(HDLCD_RES_HEIGHT            240)
set(HDLCD_BYTES_PER_PIXEL       4)

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

## Compile definitions
target_compile_definitions(${PLATFORM_DRIVERS_TARGET} PRIVATE
    HDLCD_FRAME_BUFFER_BASE_ADDRESS=${HDLCD_FRAME_BUFFER_BASE_ADDRESS}
    HDLCD_BYTES_PER_PIXEL=${HDLCD_BYTES_PER_PIXEL}
    HDLCD_RES_WIDTH=${HDLCD_RES_WIDTH}
    HDLCD_RES_HEIGHT=${HDLCD_RES_HEIGHT})

## Platform sources
target_sources(${PLATFORM_DRIVERS_TARGET}
    PRIVATE
    source/timer_mps4.c
    source/hdlcd_drv.c
    source/platform_drivers.c)

## Directory for additional components required by MPS4:
if (NOT DEFINED COMPONENTS_DIR)
    set(COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../components)
endif()

## Platform component: cmsis_device (provides generic Cortex-M start up library)
add_subdirectory(${COMPONENTS_DIR}/cmsis_device ${CMAKE_BINARY_DIR}/cmsis_device)

## Platform component: stdout
add_subdirectory(${COMPONENTS_DIR}/stdout ${CMAKE_BINARY_DIR}/stdout)

## Platform component: display
add_subdirectory(${COMPONENTS_DIR}/display ${CMAKE_BINARY_DIR}/display)

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

if (FPGA_PLATFORM_SSE_320)
    target_compile_definitions(${PLATFORM_DRIVERS_TARGET} PRIVATE
        FPGA=1)
endif()

## Logging utilities:
if (NOT TARGET hal_log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

# Add dependencies:
target_link_libraries(${PLATFORM_DRIVERS_TARGET} PUBLIC
    hal_log
    cmsis_device
    platform_pmu
    hal_display_mps4
    hal_audio_static_streams
    $<IF:$<BOOL:${FVP_VSI_ENABLED}>,hal_camera_vsi,hal_camera_static_images>
    $<$<NOT:$<BOOL:${SEMIHOSTING_ENABLED}>>:stdout_retarget_cmsdk>)

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
message(STATUS "Library                 : " ${PLATFORM_DRIVERS_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR  : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
