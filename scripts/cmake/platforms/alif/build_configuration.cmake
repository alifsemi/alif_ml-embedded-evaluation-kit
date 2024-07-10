# /* This file was ported to work on Alif Semiconductor devices. */

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

    USER_OPTION(TARGET_SUBSYSTEM "Specify platform target subsystem: RTSS-HP, RTSS-HE or none"
        RTSS-HE
        STRING)

    set(ALIF_DEVICE_SKU "AE722F80F55D5" CACHE STRING "Specify Alif SKU part number")
    set_property(CACHE ALIF_DEVICE_SKU PROPERTY STRINGS "AE1C1F4051920" "AE722F80F55D5" "AE822FA0E5597")

    set(ALIF_BOARDLIB_PATH_END "appkit_gen2" CACHE STRING "Specify Alif boardlib path")
    set_property(CACHE ALIF_BOARDLIB_PATH_END PROPERTY STRINGS "appkit_gen2" "devit_gen2" "devkit_e1c" "devkit_e8")

    set(TARGET_BOARD "AppKit" CACHE STRING "Board type")
    set_property(CACHE TARGET_BOARD PROPERTY STRINGS "DevKit" "AppKit")

    # Sanity check DevKit or AppKit
    if (NOT ((TARGET_BOARD STREQUAL "DevKit") OR (TARGET_BOARD STREQUAL "AppKit")))
        message(FATAL_ERROR "Possible TARGET_BOARD values are: DevKit and AppKit but given value was ${TARGET_BOARD}")
    endif()

    # Sanity check E1C and E8 need to be Devkit
    if ((ALIF_DEVICE_SKU STREQUAL "AE1C1F4051920") OR (ALIF_DEVICE_SKU STREQUAL "AE822FA0E5597"))
        if (NOT (TARGET_BOARD STREQUAL "DevKit"))
            message(FATAL_ERROR "Possible TARGET_BOARD value for AE1C1F4051920 and AE822FA0E5597 is DevKit. Given value was ${TARGET_BOARD}")
        endif()
    endif()


    set(IS_EAGLE_DEVICE OFF CACHE BOOL "Is device an Eagle device.")
    if (ALIF_DEVICE_SKU STREQUAL "AE822FA0E5597") # Add other SKUs which are Eagle devices
        set(ALIF_BOARDLIB_PATH_END "devkit_e8" CACHE STRING "" FORCE)
        set(IS_EAGLE_DEVICE ON CACHE BOOL "" FORCE)
        add_compile_definitions("EAGLE_DEVICE") # Flag used by ServicesLIB and our ml-devkit files.
    endif()

    if (ALIF_DEVICE_SKU STREQUAL "AE1C1F4051920") # Add other SKUs which are Balletto devices
        set(ALIF_BOARDLIB_PATH_END "devkit_e1c" CACHE STRING "" FORCE)
        set(IS_BALLETTO_DEVICE ON)
        add_compile_definitions("BALLETTO_DEVICE") # Flag used by ServicesLIB and our ml-devkit files.
        USER_OPTION(ALIF_CAMERA_ENABLED "If enabled, does use the real camera, otherwise uses static images instead. Disabled by default on E1C."
            OFF
            BOOL)
    else()
        set(IS_BALLETTO_DEVICE OFF)
        USER_OPTION(ALIF_CAMERA_ENABLED "If enabled, does use the real camera, otherwise uses static images instead. Enabled by default."
            ON
            BOOL)
    endif()

    USER_OPTION(ETHOS_U_NPU_ID "Arm Ethos-U NPU IP (U55 or U85)"
        "U55"
        STRING)
    set_property(CACHE ETHOS_U_NPU_ID PROPERTY STRINGS "U55" "U85")

    if (ETHOS_U_NPU_ID STREQUAL U85)
        set(RTSS_NPU_CONFIG_ID "Z256")
    elseif (TARGET_SUBSYSTEM STREQUAL RTSS-HP)
        set(RTSS_NPU_CONFIG_ID "H256")
    else()
        set(RTSS_NPU_CONFIG_ID "H128")
    endif()

    USER_OPTION(ETHOS_U_NPU_CONFIG_ID "Specifies the configuration ID for the NPU."
        "${RTSS_NPU_CONFIG_ID}"
        STRING)

    if ((ALIF_DEVICE_SKU STREQUAL "AE722F80F55D5") AND (TARGET_BOARD STREQUAL "DevKit"))
        set(ALIF_BOARDLIB_PATH_END "devkit_gen2" CACHE STRING "" FORCE)
    endif()

    # Include NPU, CMSIS and LVGL configuration options
    include(npu_opts)
    include(cmsis_opts)
    include(lvgl_opts)

    message(STATUS "Board            : Alif Semiconductor ${TARGET_BOARD}")
    message(STATUS "Platform         : Alif ${TARGET_SUBSYSTEM}")
    message(STATUS "Alif Device SKU  : ${ALIF_DEVICE_SKU}")

    set(ALIF_CMSIS_PATH ${MLEK_DEPENDENCY_ROOT_DIR}/cmsis-alif PARENT_SCOPE)

    set(BOARDLIB_PATH ${MLEK_DEPENDENCY_ROOT_DIR}/boardlib PARENT_SCOPE)

    set(CMAKE_SYSTEM_PROCESSOR cortex-m55 CACHE STRING "Cortex-M CPU to use")

    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${MLEK_CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()

    set(LINKER_SCRIPT_NAME "${TARGET_SUBSYSTEM}" CACHE STRING "Linker script name")
    set(PLATFORM_DRIVERS_DIR "${MLEK_HAL_PLATFORM_DIR}/alif" PARENT_SCOPE)

    if (TARGET_SUBSYSTEM STREQUAL RTSS-HP)
        set(ALIF_CORE "RTSS_HP")
    else()
        set(ALIF_CORE "RTSS_HE")
    endif()
    set(ALIF_CORE "${ALIF_CORE}" PARENT_SCOPE)
    set(ALIF_DEVICE_SKU "${ALIF_DEVICE_SKU}" PARENT_SCOPE)
    set(ALIF_BOARDLIB_PATH_END "${ALIF_BOARDLIB_PATH_END}" PARENT_SCOPE)
    set(IS_BALLETTO_DEVICE ${IS_BALLETTO_DEVICE} PARENT_SCOPE)

    add_compile_definitions(
        ${ALIF_CORE}
        )

    if (ETHOS_U_NPU_ID STREQUAL "U55")
        set(ETHOS_U_BASE_ADDR    "0x400E1000"   CACHE STRING "Ethos-U NPU base address")
        set(ETHOS_U_IRQN         "55"           CACHE STRING "Ethos-U55 Interrupt")
    else()
        set(ETHOS_U_BASE_ADDR    "0x49042000"   CACHE STRING "Ethos-U NPU base address")
        set(ETHOS_U_IRQN         "366"          CACHE STRING "Ethos-U85 Interrupt")
    endif()
    set(ETHOS_U_SEC_ENABLED  "1"            CACHE STRING "Ethos-U NPU Security enable")
    set(ETHOS_U_PRIV_ENABLED "1"            CACHE STRING "Ethos-U NPU Privilege enable")

endfunction()

function(platform_custom_post_build)
    set(oneValueArgs TARGET_NAME MODEL_IN_EXT_FLASH)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "" ${ARGN} )

    set_target_properties(${PARSED_TARGET_NAME} PROPERTIES SUFFIX ".axf")

    string(TOLOWER "${ALIF_CORE}" ALIF_CORE_STRING)

    # Add link options for the linker script to be used:
    add_linker_script(
        ${PARSED_TARGET_NAME}                                                       # Target
        ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${ALIF_DEVICE_SKU}/${ALIF_CORE_STRING}   # Directory path
        ${LINKER_SCRIPT_NAME})                                                      # Name of the file without suffix

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
