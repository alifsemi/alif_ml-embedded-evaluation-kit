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
    set(TARGET_BOARD "AppKit" CACHE STRING "Board type")
    set_property(CACHE TARGET_BOARD PROPERTY STRINGS "DevKit" "AppKit" "DevKit_Baseboard" "AppKit_Alpha1" "AppKit_Alpha2")

    set(TARGET_REVISION "B" CACHE STRING "Chip revision")
    set_property(CACHE TARGET_REVISION PROPERTY STRINGS "A" "B")

    if (TARGET_REVISION STREQUAL "A")
        # Sanity check DevKit, AppKit_Alpha1 and AppKit_Alpha2
        if (NOT ((TARGET_BOARD STREQUAL "DevKit") OR (TARGET_BOARD STREQUAL "AppKit_Alpha1") OR (TARGET_BOARD STREQUAL "AppKit_Alpha2")))
            message(FATAL_ERROR "'A' revision possible TARGET_BOARD values are: DevKit, AppKit_Alpha1 and AppKit_Alpha2 but given value was ${TARGET_BOARD}")
        endif()
        set(ENSEMBLE_CMSIS_PATH ${DEPENDENCY_ROOT_DIR}/cmsis-ensemble-a PARENT_SCOPE)
    else()
        # Sanity check DevKit, AppKit and DevKit_Baseboard
        if (NOT ((TARGET_BOARD STREQUAL "DevKit") OR (TARGET_BOARD STREQUAL "AppKit") OR (TARGET_BOARD STREQUAL "DevKit_Baseboard")))
            message(FATAL_ERROR "'B' revision possible TARGET_BOARD values are: DevKit, AppKit and DevKit_Baseboard but given value was ${TARGET_BOARD}")
        endif()
        set(ENSEMBLE_CMSIS_PATH ${DEPENDENCY_ROOT_DIR}/cmsis-ensemble PARENT_SCOPE)
    endif()
    set(BOARDLIB_PATH ${DEPENDENCY_ROOT_DIR}/boardlib PARENT_SCOPE)

    message(STATUS "Board:    Alif Semiconductor ${TARGET_BOARD}")
    message(STATUS "Platform: Ensemble ${TARGET_SUBSYSTEM} (rev ${TARGET_REVISION})")

    set(CMAKE_SYSTEM_PROCESSOR  cortex-m55)

    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()

    set(LINKER_SCRIPT_NAME "ensemble-${TARGET_SUBSYSTEM}" CACHE STRING "Linker script name")
    set(PLATFORM_DRIVERS_DIR "${HAL_PLATFORM_DIR}/ensemble" PARENT_SCOPE)

    if (TARGET_SUBSYSTEM STREQUAL RTSS-HP)
        set(ETHOS_U_NPU_CONFIG_ID "H256"    CACHE STRING "Ethos-U NPU configuration" FORCE)
        set(ENSEMBLE_CORE         "M55_HP")
    else()
        set(ETHOS_U_NPU_CONFIG_ID "H128"    CACHE STRING "Ethos-U NPU configuration" FORCE)
        set(ENSEMBLE_CORE         "M55_HE")
    endif()
    set(ENSEMBLE_CORE "${ENSEMBLE_CORE}" PARENT_SCOPE)
    add_compile_definitions(${ENSEMBLE_CORE})

    message(STATUS "Forced ETHOS_U_NPU_CONFIG_ID to ${ETHOS_U_NPU_CONFIG_ID}")
    set(ETHOS_U_BASE_ADDR    "0x400E1000"   CACHE STRING "Ethos-U NPU base address")
    set(ETHOS_U_IRQN         "55"           CACHE STRING "Ethos-U55 Interrupt")
    set(ETHOS_U_SEC_ENABLED  "1"            CACHE STRING "Ethos-U NPU Security enable")
    set(ETHOS_U_PRIV_ENABLED "1"            CACHE STRING "Ethos-U NPU Privilege enable")

    # Second AXI port M1 can't reach MRAM or OSPI on Ensemble B; overall performance loss of using
    # only one port is minimal in most cases, so simplest just to always disable by default, rather than
    # attempt to have any sort of dynamic switching based on model location.
    #
    # If not using MRAM or OSPI for the model data, then the user can set this back to two.
    if (TARGET_REVISION STREQUAL "B")
        set(ETHOS_U_NPU_PORTS "1"          CACHE STRING "Ethos-U AXI ports")
    else()
        set(ETHOS_U_NPU_PORTS "2"          CACHE STRING "Ethos-U AXI ports")
    endif()

endfunction()

function(platform_custom_post_build)
    set(oneValueArgs TARGET_NAME MODEL_IN_EXT_FLASH)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "" ${ARGN} )

    set_target_properties(${PARSED_TARGET_NAME} PROPERTIES SUFFIX ".axf")

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

    set(LINKER_SECTION_TAGS     "*.at_mram")
    set(LINKER_OUTPUT_BIN_TAGS  "mram.bin")

    # Create the ospi flash binaries if enabled
    if(PARSED_MODEL_IN_EXT_FLASH)
        list(APPEND LINKER_SECTION_TAGS "*.at_ext_flash")
        list(APPEND LINKER_OUTPUT_BIN_TAGS "ext_flash.bin")
    endif()

    add_bin_generation_command(
            TARGET_NAME ${PARSED_TARGET_NAME}
            OUTPUT_DIR  ${SECTORS_BIN_DIR}
            AXF_PATH    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PARSED_TARGET_NAME}.axf
            SECTION_PATTERNS    "${LINKER_SECTION_TAGS}"
            OUTPUT_BIN_NAMES    "${LINKER_OUTPUT_BIN_TAGS}")
endfunction()
