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
    message(STATUS "Platform: Ensemble HP")

    set(CMAKE_SYSTEM_PROCESSOR  cortex-m55)
    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()

    set(LINKER_SCRIPT_NAME "ensemble-${TARGET_SUBSYSTEM}" PARENT_SCOPE)
    set(PLATFORM_DRIVERS_DIR "${HAL_PLATFORM_DIR}/ensemble" PARENT_SCOPE)

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
