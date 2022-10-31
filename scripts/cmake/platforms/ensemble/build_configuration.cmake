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

function(set_platform_global_defaults)
    message(STATUS "Platform: Ensemble ${TARGET_SUBSYSTEM}")

    set(CMAKE_SYSTEM_PROCESSOR  cortex-m55)

    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()

    set(LINKER_SCRIPT_NAME "ensemble-${TARGET_SUBSYSTEM}" PARENT_SCOPE)
    set(PLATFORM_DRIVERS_DIR "${HAL_PLATFORM_DIR}/ensemble" PARENT_SCOPE)

    if (TARGET_SUBSYSTEM STREQUAL RTSS-HP)
        set(ETHOS_U_NPU_CONFIG_ID "H256"    CACHE STRING "Ethos-U NPU configuration" FORCE)
    else()
        set(ETHOS_U_NPU_CONFIG_ID "H128"    CACHE STRING "Ethos-U NPU configuration" FORCE)
    endif()
    message(STATUS "Forced ETHOS_U_NPU_CONFIG_ID to ${ETHOS_U_NPU_CONFIG_ID}")
    set(ETHOS_U_BASE_ADDR    "0x400E1000"   CACHE STRING "Ethos-U NPU base address")
    set(ETHOS_U_IRQN         "55"           CACHE STRING "Ethos-U55 Interrupt")
    set(ETHOS_U_SEC_ENABLED  "1"            CACHE STRING "Ethos-U NPU Security enable")
    set(ETHOS_U_PRIV_ENABLED "1"            CACHE STRING "Ethos-U NPU Privilege enable")

endfunction()

function(platform_custom_post_build)
    set(oneValueArgs TARGET_NAME)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "" ${ARGN} )

    set_target_properties(${PARSED_TARGET_NAME} PROPERTIES SUFFIX ".axf")

    message(STATUS "******************** PARSED_TARGET_NAME: ${PARSED_TARGET_NAME} ********************")
    message(STATUS "******************** LINKER_SCRIPT_NAME: ${LINKER_SCRIPT_NAME} ********************")
    message(STATUS "******************** CMAKE_SCRIPTS_DIR/platforms/ensemble/TARGET_SUBSYSTEM: ${CMAKE_SCRIPTS_DIR}/platforms/ensemble/${TARGET_SUBSYSTEM} ********************")

    # Add link options for the linker script to be used:
    add_linker_script(
            ${PARSED_TARGET_NAME}          # Target
            ${CMAKE_SCRIPTS_DIR}/platforms/ensemble/${TARGET_SUBSYSTEM}    # Directory path
            ${LINKER_SCRIPT_NAME})  # Name of the file without suffix

    add_target_map_file(
            ${PARSED_TARGET_NAME}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PARSED_TARGET_NAME}.map)

    set(SECTORS_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sectors)
    set(SECTORS_BIN_DIR ${SECTORS_DIR}/${use_case})

    file(REMOVE_RECURSE ${SECTORS_BIN_DIR})
    file(MAKE_DIRECTORY ${SECTORS_BIN_DIR})

    add_bin_generation_command(
            TARGET_NAME ${PARSED_TARGET_NAME}
            OUTPUT_DIR  ${SECTORS_BIN_DIR}
            AXF_PATH    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PARSED_TARGET_NAME}.axf
            SECTION_PATTERNS    "${LINKER_SECTION_TAGS}"
            OUTPUT_BIN_NAMES    "${LINKER_OUTPUT_BIN_TAGS}")

endfunction()
