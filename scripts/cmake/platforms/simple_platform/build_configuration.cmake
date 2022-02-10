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
    message(STATUS "Platform: Simple platform with minimal peripherals")
    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()
    set(LINKER_SCRIPT_NAME  "simple_platform" PARENT_SCOPE)
    set(PLATFORM_DRIVERS_DIR "${HAL_PLATFORM_DIR}/simple" PARENT_SCOPE)
endfunction()

function(platform_custom_post_build)
    set(oneValueArgs TARGET_NAME)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "" ${ARGN} )

    set_target_properties(${PARSED_TARGET_NAME} PROPERTIES SUFFIX ".axf")
    # Add link options for the linker script to be used:

    add_linker_script(
        ${PARSED_TARGET_NAME}          # Target
        ${CMAKE_SCRIPTS_DIR}/platforms/simple_platform    # Directory path
        ${LINKER_SCRIPT_NAME})  # Name of the file without suffix

    add_target_map_file(
        ${PARSED_TARGET_NAME}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PARSED_TARGET_NAME}.map)

    set(SECTORS_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sectors)
    set(SECTORS_BIN_DIR ${SECTORS_DIR}/${use_case})

    file(REMOVE_RECURSE ${SECTORS_BIN_DIR})
    file(MAKE_DIRECTORY ${SECTORS_BIN_DIR})

    set(LINKER_SECTION_TAGS     "*.at_itcm" "*.at_ddr")
    set(LINKER_OUTPUT_BIN_TAGS  "itcm.bin"  "ddr.bin")

    add_bin_generation_command(
        TARGET_NAME ${PARSED_TARGET_NAME}
        OUTPUT_DIR  ${SECTORS_BIN_DIR}
        AXF_PATH    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PARSED_TARGET_NAME}.axf
        SECTION_PATTERNS    "${LINKER_SECTION_TAGS}"
        OUTPUT_BIN_NAMES    "${LINKER_OUTPUT_BIN_TAGS}")
endfunction()