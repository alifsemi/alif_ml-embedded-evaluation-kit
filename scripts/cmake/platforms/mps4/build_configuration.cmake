#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its
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
    message(STATUS "Platform: Arm MPS4 FPGA Prototyping Board or FVP")

    # Default sub-system for MPS4
    USER_OPTION(TARGET_SUBSYSTEM "Specify platform target subsystem: sse-315 or sse-320"
        sse-315
        STRING)

    # NPU respective to target subsystem
    if(TARGET_SUBSYSTEM STREQUAL sse-315)
        set(DEFAULT_ETHOS_U_NPU_ID  "U65")
    elseif(TARGET_SUBSYSTEM STREQUAL sse-320)
        set(DEFAULT_ETHOS_U_NPU_ID  "U85")
    else()
        message(FATAL_ERROR "${TARGET_SUBSYSTEM} unsupported by ${TARGET_PLATFORM}")
    endif()

    # Default NPU for SSE-315 target:
    USER_OPTION(ETHOS_U_NPU_ID "Arm Ethos-U NPU IP (U65 or U85)"
        "${DEFAULT_ETHOS_U_NPU_ID}"
        STRING)

    # Include NPU, FVP tests and CMSIS configuration options
    include(npu_opts)
    include(fvp_opts)
    include(cmsis_opts)

    if (NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
        set(CMAKE_SYSTEM_PROCESSOR cortex-m85 CACHE STRING "Cortex-M CPU to use")
    endif()

    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${MLEK_CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()

    # Arm Corstone-315's timing adapter behaviour is very different to Arm Corstone-320 and cannot
    # be used for bandwidth/latency related performance sweeps for the Arm Ethos-U NPU. Read
    # docs/sections/timing_adapters.md for more details.
    if ((TARGET_SUBSYSTEM STREQUAL "sse-315") AND (DEFINED ETHOS_U_NPU_TIMING_ADAPTER_ENABLED))
        message(STATUS "Timing adapter will NOT be used for target subsystem ${TARGET_SUBSYSTEM}")
        set(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED OFF CACHE BOOL "Use of TA" FORCE)
    endif()

    set(LINKER_SCRIPT_NAME "mps4-${TARGET_SUBSYSTEM}" PARENT_SCOPE)
    set(PLATFORM_DRIVERS_DIR "${MLEK_HAL_PLATFORM_DIR}/mps4" PARENT_SCOPE)

endfunction()

function(platform_custom_post_build)
    set(oneValueArgs TARGET_NAME)
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
    if (DEFINED use_case)
        set(SECTORS_BIN_DIR ${SECTORS_DIR}/${use_case})
    else()
        set(SECTORS_BIN_DIR ${SECTORS_DIR}/${PARSED_TARGET_NAME})
    endif()

    file(REMOVE_RECURSE ${SECTORS_BIN_DIR})
    file(MAKE_DIRECTORY ${SECTORS_BIN_DIR})

    if (TARGET_SUBSYSTEM STREQUAL sse-315 OR TARGET_SUBSYSTEM STREQUAL sse-320)
        set(LINKER_SECTION_TAGS     "*.at_boot" "*.at_bram" "*.at_ddr")
        set(LINKER_OUTPUT_BIN_TAGS  "boot.bin" "bram.bin"  "ddr.bin")
    endif()

    add_bin_generation_command(
            TARGET_NAME ${PARSED_TARGET_NAME}
            OUTPUT_DIR  ${SECTORS_BIN_DIR}
            AXF_PATH    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PARSED_TARGET_NAME}.axf
            SECTION_PATTERNS    "${LINKER_SECTION_TAGS}"
            OUTPUT_BIN_NAMES    "${LINKER_OUTPUT_BIN_TAGS}")

    set(MPS4_FPGA_CONFIG "${MLEK_SCRIPTS_DIR}/mps4/${TARGET_SUBSYSTEM}/images.txt")

    add_custom_command(TARGET ${PARSED_TARGET_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${MPS4_FPGA_CONFIG} ${SECTORS_DIR})

    get_target_property(EXCLUDED_TARGET ${PARSED_TARGET_NAME} EXCLUDE_FROM_ALL)

    # Add tests for application on FVP if FVP path specified
    if (BUILD_FVP_TESTS AND NOT EXCLUDED_TARGET)
        set(TEST_TARGET_NAME "${PARSED_TARGET_NAME}_fvp_test")

        message(STATUS "Adding FVP test for ${PARSED_TARGET_NAME}")
        add_test(
            NAME "${TEST_TARGET_NAME}"
            COMMAND ${FVP_PATH} -a $<TARGET_FILE:${PARSED_TARGET_NAME}>
                -C mps4_board.telnetterminal0.start_telnet=0
                -C mps4_board.uart0.out_file='-'
                -C mps4_board.uart0.shutdown_on_eot=1
                -C mps4_board.visualisation.disable-visualisation=1
                -C vis_hdlcd.disable_visualisation=1
                --stat)

        set_tests_properties(${TEST_TARGET_NAME} PROPERTIES
            REQUIRED_FILES "$<TARGET_FILE:${PARSED_TARGET_NAME}>;${FVP_PATH}")
    endif ()

endfunction()
