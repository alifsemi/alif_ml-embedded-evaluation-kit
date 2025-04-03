# /* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

# /* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
#  * Use, distribution and modification of this code is permitted under the
#  * terms stated in the Alif Semiconductor Software License Agreement
#  *
#  * You should have received a copy of the Alif Semiconductor Software
#  * License Agreement with this file. If not, please write to:
#  * contact@alifsemi.com, or visit: https://alifsemi.com/license
#  *
#  */

#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022-2024 Arm Limited and/or its
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

function(set_platform_global_defaults)

    USER_OPTION(ALIF_DEVICE_SKU "Specify Alif SKU part number"
        AE722F80F55D5
        STRING)

    USER_OPTION(TARGET_SUBSYSTEM "Specify platform target subsystem: RTSS-HP, RTSS-HE or none"
        RTSS-HE
        STRING)

    if (TARGET_SUBSYSTEM STREQUAL RTSS-HP)
        set(RTSS_NPU_CONFIG_ID  "H256")
        set(ENSEMBLE_CORE       "M55_HP")
        set(ENSEMBLE_CORE_NEW   "RTSS_HP" CACHE STRING "Alif core")
    else()
        set(RTSS_NPU_CONFIG_ID  "H128")
        set(ENSEMBLE_CORE       "M55_HE")
        set(ENSEMBLE_CORE_NEW   "RTSS_HE" CACHE STRING "Alif core")
    endif()

    USER_OPTION(ETHOS_U_NPU_ID "Arm Ethos-U NPU IP (U55 or U65)"
        "U55"
        STRING)

    USER_OPTION(ETHOS_U_NPU_CONFIG_ID "Specifies the configuration ID for the NPU."
        "${RTSS_NPU_CONFIG_ID}"
        STRING)


    USER_OPTION(ALIF_CAMERA_ENABLED "If enabled, does use the real camera, otherwise uses static images instead."
        ON
        BOOL)

    # Include NPU, CMSIS and LVGL configuration options
    include(npu_opts)
    include(cmsis_opts)
    include(lvgl_opts)

    set(TARGET_BOARD "AppKit" CACHE STRING "Board type")
    set_property(CACHE TARGET_BOARD PROPERTY STRINGS "DevKit" "AppKit")

    set(TARGET_REVISION "B" CACHE STRING "Chip revision")
    set_property(CACHE TARGET_REVISION PROPERTY STRINGS "B")

    message(STATUS "Board:    Alif Semiconductor ${TARGET_BOARD}")
    message(STATUS "Platform: Ensemble ${TARGET_SUBSYSTEM} (rev ${TARGET_REVISION})")
    message(STATUS "Alif SoC part number: ${ALIF_DEVICE_SKU}")

    if (TARGET_REVISION STREQUAL "B")
        # Sanity check DevKit and AppKit
        if (NOT ((TARGET_BOARD STREQUAL "DevKit") OR (TARGET_BOARD STREQUAL "AppKit") ))
            message(FATAL_ERROR "'B' revision possible TARGET_BOARD values are: DevKit and AppKit but given value was ${TARGET_BOARD}")
        endif()
        set(ENSEMBLE_CMSIS_PATH ${MLEK_DEPENDENCY_ROOT_DIR}/cmsis-ensemble PARENT_SCOPE)
    endif()
    set(BOARDLIB_PATH ${MLEK_DEPENDENCY_ROOT_DIR}/boardlib PARENT_SCOPE)

    set(CMAKE_SYSTEM_PROCESSOR cortex-m55 CACHE STRING "Cortex-M CPU to use")

    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${MLEK_CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()

    set(LINKER_SCRIPT_NAME "ensemble-${TARGET_SUBSYSTEM}" CACHE STRING "Linker script name")
    set(PLATFORM_DRIVERS_DIR "${MLEK_HAL_PLATFORM_DIR}/ensemble" PARENT_SCOPE)

    set(ENSEMBLE_CORE "${ENSEMBLE_CORE}" PARENT_SCOPE)
    add_compile_definitions(
        ${ENSEMBLE_CORE}
        ${ENSEMBLE_CORE_NEW}
        )

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
    endif()

endfunction()

function(platform_custom_post_build)
    set(oneValueArgs TARGET_NAME MODEL_IN_EXT_FLASH)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "" ${ARGN} )

    set_target_properties(${PARSED_TARGET_NAME} PROPERTIES SUFFIX ".axf")

    # Add link options for the linker script to be used:
    add_linker_script(
        ${PARSED_TARGET_NAME}                                   # Target
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${TARGET_SUBSYSTEM}  # Directory path
        ${LINKER_SCRIPT_NAME})                                  # Name of the file without suffix

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
