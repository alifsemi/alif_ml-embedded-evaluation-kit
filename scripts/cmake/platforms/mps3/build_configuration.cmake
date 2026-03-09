#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022-2026 Arm Limited and/or its
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
    message(STATUS "Platform: Arm MPS3 FPGA Prototyping Board or FVP")

    # Set default subsystem for MPS3:
    USER_OPTION(TARGET_SUBSYSTEM "Specify platform target subsystem: sse-300 or sse-310"
        sse-300
        STRING)

    # Default NPU for SSE-300 target:
    USER_OPTION(ETHOS_U_NPU_ID "Arm Ethos-U NPU IP (U55 or U65)"
        "U55"
        STRING)

    # Include NPU, FVP tests and CMSIS configuration options
    include(npu_opts)
    include(fvp_opts)
    include(cmsis_opts)

    if (NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
        if (TARGET_SUBSYSTEM STREQUAL sse-300)
            set(CMAKE_SYSTEM_PROCESSOR cortex-m55 CACHE STRING "Cortex-M CPU to use")
        elseif(TARGET_SUBSYSTEM STREQUAL sse-310)
            set(CMAKE_SYSTEM_PROCESSOR cortex-m85 CACHE STRING "Cortex-M CPU to use")
        endif()
    endif()

    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${MLEK_CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()

    # Arm Corstone-310's timing adapter behaviour is very different to Arm Corstone-300 and cannot
    # be used for bandwidth/latency related performance sweeps for the Arm Ethos-U NPU. Read
    # docs/sections/timing_adapters.md for more details.
    if ((TARGET_SUBSYSTEM STREQUAL "sse-310") AND (DEFINED ETHOS_U_NPU_TIMING_ADAPTER_ENABLED))
        message(STATUS "Timing adapter will NOT be used for target subsystem ${TARGET_SUBSYSTEM}")
        set(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED OFF CACHE BOOL "Use of TA" FORCE)
    endif()

    set(LINKER_SCRIPT_NAME "mps3-${TARGET_SUBSYSTEM}" PARENT_SCOPE)
    set(PLATFORM_DRIVERS_DIR "${MLEK_HAL_PLATFORM_DIR}/mps3" PARENT_SCOPE)

endfunction()

function(platform_custom_post_build)
    set(oneValueArgs TARGET_NAME)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "" ${ARGN} )

    set_target_properties(${PARSED_TARGET_NAME} PROPERTIES SUFFIX ".axf")
    get_target_property(target_linker_script_override
        ${PARSED_TARGET_NAME}
        MLEK_LINKER_SCRIPT_OVERRIDE_PATH)
    set(default_linker_script_path
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${TARGET_SUBSYSTEM}/${LINKER_SCRIPT_NAME}${LINKER_SCRIPT_SUFFIX}")
    resolve_linker_script_path(
        linker_script_path
        "${default_linker_script_path}"
        "${target_linker_script_override}")

    # Add link options for the linker script to be used:
    add_linker_script(
        ${PARSED_TARGET_NAME}  # Target
        ${linker_script_path}) # Full linker control file path

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

    if (TARGET_SUBSYSTEM STREQUAL sse-310)
        set(LINKER_SECTION_TAGS     "*.at_bram" "*.at_ddr")
        set(LINKER_OUTPUT_BIN_TAGS  "bram.bin"  "ddr.bin")
    else()
        set(LINKER_SECTION_TAGS     "*.at_itcm" "*.at_ddr")
        set(LINKER_OUTPUT_BIN_TAGS  "itcm.bin"  "ddr.bin")
    endif()

    add_bin_generation_command(
            TARGET_NAME ${PARSED_TARGET_NAME}
            OUTPUT_DIR  ${SECTORS_BIN_DIR}
            AXF_PATH    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PARSED_TARGET_NAME}.axf
            SECTION_PATTERNS    "${LINKER_SECTION_TAGS}"
            OUTPUT_BIN_NAMES    "${LINKER_OUTPUT_BIN_TAGS}")

    set(MPS3_FPGA_CONFIG "${MLEK_SCRIPTS_DIR}/mps3/${TARGET_SUBSYSTEM}/images.txt")

    add_custom_command(TARGET ${PARSED_TARGET_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${MPS3_FPGA_CONFIG} ${SECTORS_DIR})

    get_target_property(EXCLUDED_TARGET ${PARSED_TARGET_NAME} EXCLUDE_FROM_ALL)

    # Add tests for application on FVP if FVP path specified
    if (BUILD_FVP_TESTS AND NOT EXCLUDED_TARGET)
        set(TEST_TARGET_NAME "${PARSED_TARGET_NAME}_fvp_test")

        message(STATUS "Adding FVP test for ${PARSED_TARGET_NAME}")
        add_test(
            NAME "${TEST_TARGET_NAME}"
            COMMAND ${FVP_PATH} -a $<TARGET_FILE:${PARSED_TARGET_NAME}>
                -C mps3_board.telnetterminal0.start_telnet=0
                -C mps3_board.uart0.out_file='-'
                -C mps3_board.uart0.shutdown_on_eot=1
                -C mps3_board.visualisation.disable-visualisation=1
                --stat)

        set_tests_properties(${TEST_TARGET_NAME} PROPERTIES
            REQUIRED_FILES "$<TARGET_FILE:${PARSED_TARGET_NAME}>;${FVP_PATH}")
    endif ()

endfunction()
