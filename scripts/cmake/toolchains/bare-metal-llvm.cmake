#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
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
# specify the cross compiler
set(CMAKE_C_COMPILER                clang)
set(CMAKE_CXX_COMPILER              clang)
set(CMAKE_C_LINKER_PREFERENCE       clang)
set(CMAKE_ASM_LINKER_PREFERENCE     clang)
set(CMAKE_ASM_COMPILER              clang)
set(CMAKE_ASM_COMPILER_AR           llvm-ar)

set(CMAKE_CROSSCOMPILING            true)
set(CMAKE_SYSTEM_NAME               Generic)

# Minimum version of the toolchain required
set(MIN_CLANG_VERSION               19.1.5)

if (NOT DEFINED CMAKE_SYSTEM_PROCESSOR AND NOT DEFINED CMAKE_SYSTEM_ARCH)
    set(CMAKE_SYSTEM_PROCESSOR      cortex-m55 CACHE STRING "Cortex-M CPU to use")
endif()

if (CMAKE_SYSTEM_PROCESSOR STREQUAL cortex-m55)
    # Flags for cortex-m55
    set(CPU_ID                      M55)
    set(CPU_COMPILE_DEF             CPU_CORTEX_${CPU_ID})
    set(ARM_CPU                     "ARMC${CPU_ID}")
    set(CPU_HEADER_FILE             "${ARM_CPU}.h")
    set(CPU_COMPILE_OPTION          "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(FLOAT_ABI_COMPILE_OPTION    "-mfloat-abi=hard")
    set(TARGET_ABI                  "armv8.1m-none-eabi")
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL cortex-m85)
    # Flags for cortex-m85
    set(CPU_ID                      M85)
    set(CPU_COMPILE_DEF             CPU_CORTEX_${CPU_ID})
    set(ARM_CPU                     "ARMC${CPU_ID}")
    set(CPU_HEADER_FILE             "${ARM_CPU}.h")
    set(CPU_COMPILE_OPTION          "-mcpu=${CMAKE_SYSTEM_PROCESSOR}")
    set(FLOAT_ABI_COMPILE_OPTION    "-mfloat-abi=hard")
    set(TARGET_ABI                  "armv8.1m-none-eabi")
elseif (CMAKE_SYSTEM_ARCH STREQUAL armv8.1-m.main)
    # Flags for generic armv8.1-m profile
    set(CPU_ID                      ARMv81MML_DSP_DP_MVE_FP)
    set(ARM_CPU                     "ARMv81MML")
    set(CPU_COMPILE_DEF             ${CPU_ID})
    set(TARGET_ABI                  "armv8.1m-none-eabi")

    # @TODO: Revise once we have the CPU file in CMSIS and CPU flags
    # are supported by toolchains.
    set(CPU_HEADER_FILE             "${CPU_ID}.h")
    set(CPU_COMPILE_OPTION          "-march=armv8.1-m.main+mve.fp+fp.dp")
    set(FLOAT_ABI_COMPILE_OPTION    "-mfloat-abi=hard")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL cortex-m33)
    # Flags for cortex-m33 to go here
endif()

set(${CPU_COMPILE_DEF}              1)

# Warning options
add_compile_options(
    -Wall
    -Wextra
    -Wvla)

# General purpose compile options:
add_compile_options(
    -funsigned-char
    -ffunction-sections
    -fdata-sections
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-unwind-tables;-fno-rtti;-fno-exceptions>")

# Arch compile options:
add_compile_options(
    -mthumb
    --target=${TARGET_ABI}
    -mlittle-endian
    -MD
    ${CPU_COMPILE_OPTION}
    ${FLOAT_ABI_COMPILE_OPTION})

# Compile definitions:
add_compile_definitions(
    $<$<BOOL:${CPU_COMPILE_DEF}>:${CPU_COMPILE_DEF}>)

# Link options:
add_link_options(
    -v
    -mthumb
    --target=${TARGET_ABI}
    -fno-unwind-tables
    -fno-rtti
    -fno-exceptions
    -stdlib=libc++
    -lstdc++
    -nostartfiles
    ${CPU_COMPILE_OPTION}
    ${FLOAT_ABI_COMPILE_OPTION}
    -mlittle-endian
    "SHELL:-Xlinker --gc-sections"
    "$<$<CONFIG:RELEASE>:SHELL:-Xlinker --no-debug-names>"
    "$<$<CONFIG:RELEASE>:SHELL:-Xlinker --strip-all>")

# Function to configure semihosting options
function(configure_semihosting TARGET_NAME SEMIHOSTING)
    if (${SEMIHOSTING})
        target_link_options(${TARGET_NAME} PUBLIC -lcrt0-semihost -lsemihost)
        target_compile_definitions(${TARGET_NAME} PUBLIC USE_SEMIHOSTING)
    else()
        # Default `crt0` doesn't call the exit function once user main exits but sits
        # in a while loop instead. `crt0-hosted` calls exit and this is useful for
        # FVP runs for the application to exit gracefully. For details, see:
        # https://github.com/picolibc/picolibc/blob/e1110be5a948a9dfefea6cb30d9e1a5db630e76f/doc/using.md#crt0-variants
        target_link_options(${TARGET_NAME} PUBLIC -lcrt0-hosted)
    endif()
endfunction()

# Function to add a map file output for the linker to dump diagnostic information to.
function(add_target_map_file TARGET_NAME MAP_FILE_PATH)
    target_link_options(${TARGET_NAME} PUBLIC
            "SHELL:-Xlinker -Map=${MAP_FILE_PATH}")
endfunction()

# Function to add linker option to use the chosen linker script.
function(add_linker_script TARGET_NAME SCRIPT_DIR SCRIPT_NAME)
    set(LINKER_SCRIPT_PATH ${SCRIPT_DIR}/${SCRIPT_NAME}.llvm.ld
            CACHE STRING "Linker script path")
    if (NOT EXISTS ${LINKER_SCRIPT_PATH})
        message(FATAL_ERROR "Linker script not found: ${LINKER_SCRIPT_PATH}")
    endif()
    message(STATUS "Using linker script: ${LINKER_SCRIPT_PATH}")
    target_link_options(${TARGET_NAME} PUBLIC
        "SHELL:-T ${LINKER_SCRIPT_PATH}")
    set_target_properties(${TARGET_NAME}
        PROPERTIES
        LINK_DEPENDS ${LINKER_SCRIPT_PATH})
endfunction()

# Function to set the command to copy/extract contents from an elf
# into a binary file.
function(add_bin_generation_command)

    set(multiValueArgs SECTION_PATTERNS OUTPUT_BIN_NAMES)
    set(oneValueArgs TARGET_NAME OUTPUT_DIR AXF_PATH)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    list(LENGTH PARSED_SECTION_PATTERNS N_SECTION_PATTERNS)
    list(LENGTH PARSED_OUTPUT_BIN_NAMES N_OUTPUT_BIN_NAMES)

    if (NOT ${N_SECTION_PATTERNS} STREQUAL ${N_OUTPUT_BIN_NAMES})
        message(FATAL_ERROR "Section patterns and the output binary names "
                "should be of the same length")
    endif()

    message(STATUS "${TRIPLET}-objcopy requested to generate "
                   "${N_OUTPUT_BIN_NAMES} bin files.")

    math(EXPR MAX_IDX "${N_SECTION_PATTERNS} - 1")

    foreach(IDX RANGE ${MAX_IDX})

        list(GET PARSED_OUTPUT_BIN_NAMES ${IDX} OUTPUT_BIN_NAME)
        list(GET PARSED_SECTION_PATTERNS ${IDX} SECTION_PATTERN)

        add_custom_command(TARGET ${PARSED_TARGET_NAME}
            POST_BUILD
            COMMAND llvm-objcopy -O binary
            --only-section ${SECTION_PATTERN} ${PARSED_AXF_PATH}
            ${PARSED_OUTPUT_DIR}/${OUTPUT_BIN_NAME})
    endforeach()

endfunction()

# Function to assert the compiler version
function(enforce_compiler_version)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MIN_CLANG_VERSION})
        message( FATAL_ERROR "Arm compiler version must be ${MIN_CLANG_VERSION} or greater." )
    endif()
endfunction()
