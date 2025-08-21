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

    set(TARGET_BOARD "AppKit-e7" CACHE STRING "Board type")
    set_property(CACHE TARGET_BOARD PROPERTY STRINGS "AppKit-e7" "DevKit-e1c" "DevKit-e4" "DevKit-e7" "DevKit-e8")

    set(USE_STRIPED_SRAM OFF CACHE BOOL "Use SRAM0 and SRAM1 in Striped view. Support at the moment only for AE822FA0E5597")

    # Sanity check for USE_STRIPED_SRAM
    if ((USE_STRIPED_SRAM) AND NOT ((TARGET_BOARD STREQUAL "DevKit-e8") OR (TARGET_BOARD STREQUAL "DevKit-e4")))
        message(FATAL_ERROR "USE_STRIPED_SRAM possible only with TARGET_BOARD DevKit-e4 or DevKit-e8")
    endif()

    # Sanity check DevKit or AppKit
    if (NOT ((TARGET_BOARD STREQUAL "AppKit-e7") OR (TARGET_BOARD STREQUAL "DevKit-e1c") OR (TARGET_BOARD STREQUAL "DevKit-e4") OR (TARGET_BOARD STREQUAL "DevKit-e7")
        OR (TARGET_BOARD STREQUAL "DevKit-e8")))
        message(FATAL_ERROR "Possible TARGET_BOARD values are: AppKit-e7, DevKit-e1c, DevKit-e4, DevKit-e7 and DevKit-e8 but given value was ${TARGET_BOARD}")
    endif()


    if ((TARGET_BOARD STREQUAL "DevKit-e8") OR (TARGET_BOARD STREQUAL "DevKit-e4")) # Add other Devkits and AppKits which are Ensemble devices with Striping support
        set(ALIF_DEVICE_SKU "AE822FA0E5597" CACHE STRING "" FORCE)
        add_compile_definitions("EAGLE_DEVICE") # Flag used by ServicesLIB and our ml-devkit files.
        if (USE_STRIPED_SRAM)
            add_compile_definitions(USE_STRIPED_SRAM)
            message(STATUS "Using Striped SRAM!")
        else()
            message(STATUS "Using Linear SRAM!")
        endif()
    endif()

    if ((TARGET_BOARD STREQUAL "AppKit-e7") OR (TARGET_BOARD STREQUAL "DevKit-e7")) # Add other Devkit and AppKits which are Ensemble devices
        set(ALIF_DEVICE_SKU "AE722F80F55D5" CACHE STRING "" FORCE)
    endif()

    if (TARGET_BOARD STREQUAL "DevKit-e1c") # Add other Devkit and AppKits which are Balletto devices
        set(IS_BALLETTO_DEVICE ON)
        set(ALIF_DEVICE_SKU "AE1C1F4051920" CACHE STRING "" FORCE)
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

    # Sanity check for ETHOS_U_NPU_ID
    if ((ETHOS_U_NPU_ID STREQUAL U85) AND NOT ((TARGET_BOARD STREQUAL "DevKit-e8") OR (TARGET_BOARD STREQUAL "DevKit-e4")))
        message(FATAL_ERROR "ETHOS_U_NPU_ID U85 possible only with TARGET_BOARD DevKit-e4 or DevKit-e8")
    endif()

    USER_OPTION(ETHOS_U_NPU_CONFIG_ID "Specifies the configuration ID for the NPU."
        "${RTSS_NPU_CONFIG_ID}"
        STRING)

    USER_OPTION(ETHOS_U_NPU_MEMORY_MODE "Specifies the memory mode used in the Vela command."
        "Shared_Sram"
        STRING)

    # Include NPU, CMSIS and LVGL configuration options
    include(npu_opts)
    include(cmsis_opts)
    include(lvgl_opts)

    message(STATUS "Board            : Alif Semiconductor ${TARGET_BOARD}")
    message(STATUS "Platform         : Alif ${TARGET_SUBSYSTEM}")
    message(STATUS "Alif Device SKU  : ${ALIF_DEVICE_SKU}")

    set(ALIF_CMSIS_PATH ${MLEK_DEPENDENCY_ROOT_DIR}/cmsis-alif PARENT_SCOPE)

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

    #####################################################
    ## Support for linker file to include header start ##
    #####################################################
    add_library(${PARSED_TARGET_NAME}_linkerfile OBJECT)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(${PARSED_TARGET_NAME}
            PRIVATE
            -T $<TARGET_OBJECTS:${PARSED_TARGET_NAME}_linkerfile>
            "SHELL:-Xlinker -Map=bin/${target}.map"
        )

        target_sources(${PARSED_TARGET_NAME}_linkerfile
            PRIVATE
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${ALIF_DEVICE_SKU}/${ALIF_CORE_STRING}/${LINKER_SCRIPT_NAME}.ld
        )
        set_source_files_properties(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${ALIF_DEVICE_SKU}/${ALIF_CORE_STRING}/${LINKER_SCRIPT_NAME}.ld
            PROPERTIES
            LANGUAGE C
        )
        target_compile_options(${PARSED_TARGET_NAME}_linkerfile
            PRIVATE
                -E
                -P
                -xc
        )
    else()
        target_link_options(${PARSED_TARGET_NAME}
            PRIVATE
            --scatter=$<TARGET_OBJECTS:${PARSED_TARGET_NAME}_linkerfile>
        )
        target_sources(${PARSED_TARGET_NAME}_linkerfile
        PRIVATE
            ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${ALIF_DEVICE_SKU}/${ALIF_CORE_STRING}/${LINKER_SCRIPT_NAME}.sct
        )

        set_source_files_properties(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${ALIF_DEVICE_SKU}/${ALIF_CORE_STRING}/${LINKER_SCRIPT_NAME}.sct
            PROPERTIES
            LANGUAGE C
        )

        target_compile_options(${PARSED_TARGET_NAME}_linkerfile
            PRIVATE
                -E
                -xc
        )
    endif()

    add_dependencies(${PARSED_TARGET_NAME}
        ${PARSED_TARGET_NAME}_linkerfile
    )

    set_target_properties(${PARSED_TARGET_NAME} PROPERTIES LINK_DEPENDS $<TARGET_OBJECTS:${PARSED_TARGET_NAME}_linkerfile>)

    # Link rte_components so linker files can use the alif_mem_config.h
    target_link_libraries(${PARSED_TARGET_NAME}_linkerfile
        rte_components
    )

    # Add link options for the linker script to be used:
    # add_linker_script(
    #     ${PARSED_TARGET_NAME}                                                       # Target
    #     ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${ALIF_DEVICE_SKU}/${ALIF_CORE_STRING}   # Directory path
    #     ${LINKER_SCRIPT_NAME})                                                      # Name of the file without suffix

    #####################################################
    ## Support for linker file to include header end ####
    #####################################################

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
