#----------------------------------------------------------------------------
#  Copyright (c) 2021 Arm Limited. All rights reserved.
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
# minimum version of cmake = 3.15 for legit reason:
# armclang support doesn't work work in previous releases
cmake_minimum_required(VERSION 3.15.0)

# Build in release mode by default
if (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_BUILD_TYPE Release CACHE INTERNAL "")
endif()

message(STATUS "Build type is set to ${CMAKE_BUILD_TYPE}")

# Set language standards. TensorFlow Lite requires
# std=c++11.
set(CMAKE_C_STANDARD   99)
set(CMAKE_CXX_STANDARD 11)

# Make the standard a requirement => prevent fallback to previous
# supported standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We want to pass standard C/C++ flags, without gnu extensions
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_SCRIPTS_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)
set(DOWNLOAD_DEP_DIR    ${CMAKE_BINARY_DIR}/dependencies)
set(CMAKE_TOOLCHAIN_DIR ${CMAKE_SCRIPTS_DIR}/toolchains)

set(RESOURCES_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/resources_downloaded
                        CACHE PATH "Resources directory")

include(${CMAKE_SCRIPTS_DIR}/source_gen_utils.cmake)
include(${CMAKE_SCRIPTS_DIR}/util_functions.cmake)

if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(FATAL_ERROR "Source and build are in the same directory")
else()
    message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
    message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
endif()

USER_OPTION(LOG_LEVEL "Log level for the application"
    LOG_LEVEL_INFO
    STRING)

USER_OPTION(TENSORFLOW_SRC_PATH "Path to the root of the tensor flow directory"
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/tensorflow"
    PATH)

USER_OPTION(TARGET_PLATFORM "Target platform to execute evaluation application: mps3, simple_platform, native"
    mps3
    STRING)

USER_OPTION(TARGET_SUBSYSTEM "Specify platform target subsystem: sse-300 or none"
    sse-300
    STRING)

USER_OPTION(ETHOS_U_NPU_ENABLED "Select if Ethos-U55 is available for the platform and subsystem"
    ON
    BOOL)

USER_OPTION(USE_CASE_BUILD "Optional. Defines the use-case to build from the available sources. By default, all use-cases are built."
    all
    STRING)

USER_OPTION(CPU_PROFILE_ENABLED "Output CPU performance profiling information. Should be used only for MPS3 board."
    OFF
    BOOL)

if (TARGET_PLATFORM STREQUAL mps3)
    message(STATUS "Platform: MPS3 FPGA Prototyping Board or FVP")
    set(DEFAULT_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake)
elseif (TARGET_PLATFORM STREQUAL simple_platform)
    message(STATUS "Platform: Simple platform with minimal peripherals")
    set(DEFAULT_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_DIR}/bare-metal-gcc.cmake)
elseif (TARGET_PLATFORM STREQUAL native)
    message(STATUS "Platform: Native (Linux based x86_64/aarch64 system)")
    set(DEFAULT_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_DIR}/native-gcc.cmake)
else ()
    message(FATAL_ERROR "Invalid platform specified: ${TARGET_PLATFORM}")
endif ()

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${DEFAULT_TOOLCHAIN_FILE}
        CACHE FILEPATH "Toolchain file")
endif()
message(STATUS "Using CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

project(arm_ml_embedded_evaluation_kit
        VERSION     21.05
        DESCRIPTION "ARM ML Embedded Evaluation Kit for MPS3 FPGA and FastModel")

enforce_compiler_version()
setup_source_generator()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TEST_SRCS  ${CMAKE_CURRENT_SOURCE_DIR}/tests)
list(APPEND USE_CASES_TESTS_SEARCH_DIR_LIST ${TEST_SRCS}/use_case)

if (CPU_PROFILE_ENABLED)
    set(PROFILING_OPT "${PROFILING_OPT} -DCPU_PROFILE_ENABLED")
endif()

# Include platform specific sources
if (TARGET_PLATFORM STREQUAL native)
    set(PLATFORM_SOURCES_CMAKE_FILE ${CMAKE_SCRIPTS_DIR}/${TARGET_PLATFORM}-sources.cmake)
else ()
    set(PLATFORM_SOURCES_CMAKE_FILE ${CMAKE_SCRIPTS_DIR}/bare-metal-sources.cmake)

    USER_OPTION(CMSIS_SRC_PATH
        "Path to CMSIS-5 sources"
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/cmsis"
        PATH
        )

    if (CMAKE_BUILD_TYPE STREQUAL Debug AND CMAKE_CXX_COMPILER_ID STREQUAL ARMClang)
        USER_OPTION(ARMCLANG_DEBUG_DWARF_LEVEL
            "Dwarf conformance level for armclang toolchain"
            "4" # Default = 4 (Arm-DS etc). For model debugger specify "3"
            STRING
            )
    elseif (DEFINED ARMCLANG_DEBUG_DWARF_LEVEL)
        message(WARNING "ARMCLANG_DEBUG_DWARF_LEVEL definition is unsupported"
                        "within current configuration. Removing definition...")
        unset(ARMCLANG_DEBUG_DWARF_LEVEL CACHE)
    endif()

endif ()
message(STATUS "Including ${PLATFORM_SOURCES_CMAKE_FILE}")
include(${PLATFORM_SOURCES_CMAKE_FILE})

if (${CMAKE_CROSSCOMPILING})
    enable_language(ASM)

    # For non-native builds, we build with CMSIS-DSP support.
    include(${CMAKE_SCRIPTS_DIR}/cmsis-dsp.cmake)

    # All CMSIS headers to be used:
    set(CMSIS_HEADERS
        ${CMSIS_DSP_INC_DIR}
        ${CMSIS_CORE_INC_DIR}
        ${CMSIS_SRC_PATH}/Device/ARM/ARMCM55/Include
        ${CMSIS_SRC_PATH}/Device/ARM/ARMCM55/Include/Template)
endif ()

# If we need NPU libraries:
if (ETHOS_U_NPU_ENABLED)

    message(STATUS "Using ARM Ethos-U55 - adding core-driver and timing-adapter-driver includes and libraries")
    USER_OPTION(ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH
        "Path to Ethos-U55 timing adapter sources"
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/core-software/drivers/timing_adapter"
        PATH
        )

    USER_OPTION(ETHOS_U_NPU_DRIVER_SRC_PATH
        "Path to Ethos-U55 core driver sources"
        "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/core-driver"
        PATH
        )

    include_directories("${ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH}/include/")

    add_subdirectory("${ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH}" ${CMAKE_BINARY_DIR}/timing-adapter)

    set(ETHOSU_INCLUDES ${ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH}/include
                        ${ETHOS_U_NPU_DRIVER_SRC_PATH}/include)

    list(APPEND ETHOS_U_NPU_LIBS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libtiming_adapter.a)
endif ()

include(${CMAKE_SCRIPTS_DIR}/tensorflow.cmake)

set(DEP_TENSORFLOW_LITE_MICRO_SUB_DIR ${TENSORFLOW_SRC_PATH}/tensorflow/lite/micro)
set(DEP_TENSORFLOW_LITE_MICRO_MAKE_DIR ${DEP_TENSORFLOW_LITE_MICRO_SUB_DIR}/tools/make/targets)
set(DEP_FLATBUF_INCLUDE ${DEP_TENSORFLOW_LITE_MICRO_SUB_DIR}/tools/make/downloads/flatbuffers/include)

set(TENSORFLOW_LIBRARY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME})

set(DEP_TF_INCLUDE_DIRS
    ${TENSORFLOW_SRC_PATH}
    ${DEP_TENSORFLOW_LITE_MICRO_SUB_DIR}
    ${ETHOSU_INCLUDES}
    ${CMSIS_HEADERS}
    )

## All TPIP includes
set(DEP_RUNTIME_INCLUDE_DIRS
    ${DEP_TF_INCLUDE_DIRS}
    ${DEP_FLATBUF_INCLUDE}
    )

# Our entry point into tensorflow world:
file(GLOB_RECURSE SRC_TENSORFLOW_LITE_MICRO
    ${SRC_PATH}/application/tensorflow-lite-micro/**/*.cc
    ${SRC_PATH}/application/tensorflow-lite-micro/*.cc
    )

set(HAL_DIR ${SRC_PATH}/application/hal)

# HAL API sources
file(GLOB_RECURSE SRC_HAL
    "${HAL_DIR}/hal.c"
    )

# Set platform specific HAL sources; these should be provided
# by each platform's cmake include file
list(APPEND SRC_HAL ${SRC_PLAT_HAL})

# Include directories:
set(APPLICATION_INCLUDE_DIRS
    ${HAL_DIR}/include
    ${SRC_PATH}/application/tensorflow-lite-micro/include
    ${SRC_PATH}/application/main/include
    ${PLAT_INCLUDE_DIRS}
    )

file(GLOB_RECURSE SRC_APPLICATION
    "${SRC_PATH}/application/main/*.cc"
    "${SRC_PATH}/application/main/*.cpp"
    "${SRC_PATH}/application/main/*.c"
    "${SRC_PATH}/application/main/**/*.cc"
    "${SRC_PATH}/application/main/**/*.cpp"
    "${SRC_PATH}/application/main/**/*.c"
    )
list(FILTER SRC_APPLICATION EXCLUDE REGEX ".*main\\.c.*$")

list(JOIN USE_CASE_BUILD "" USE_CASE_BUILD_STR)
list(APPEND USE_CASES_SEARCH_DIR_LIST ${SRC_PATH}/use_case)
message(STATUS "Use-cases source paths: ${USE_CASES_SEARCH_DIR_LIST}.")
if (${USE_CASE_BUILD_STR} STREQUAL all)
    foreach(USE_CASES_SEARCH_DIR ${USE_CASES_SEARCH_DIR_LIST})
        SUBDIRLIST(USE_CASES_SUBDIRS ${USE_CASES_SEARCH_DIR})
        list(APPEND USE_CASES ${USE_CASES_SUBDIRS})
    endforeach()
else()
    set(USE_CASES ${USE_CASE_BUILD})
endif()

if (NOT ${CMAKE_CROSSCOMPILING})

    #Test TPIP
    set(TEST_TPIP ${DOWNLOAD_DEP_DIR}/test)
    file(MAKE_DIRECTORY ${TEST_TPIP})
    set(TEST_TPIP_INCLUDE ${TEST_TPIP}/include)
    file(MAKE_DIRECTORY ${TEST_TPIP_INCLUDE})

    include(ExternalProject)

    ExternalProject_Add(catch2-headers
        URL https://github.com/catchorg/Catch2/releases/download/v2.11.1/catch.hpp
        DOWNLOAD_NO_EXTRACT 1
        CONFIGURE_COMMAND ""
        BUILD_COMMAND bash -c "cp -R <DOWNLOAD_DIR>/catch.hpp ${TEST_TPIP_INCLUDE}"
        INSTALL_COMMAND ""
        )
endif ()

message(STATUS "Building use-cases: ${USE_CASES}.")
foreach(use_case ${USE_CASES})

    set(SRC_USE_CASE "")
    foreach(USE_CASES_SEARCH_DIR ${USE_CASES_SEARCH_DIR_LIST})
        if (EXISTS ${USE_CASES_SEARCH_DIR}/${use_case})
            message(STATUS "Found sources for use-case ${use_case}")
            set(SRC_USE_CASE ${USE_CASES_SEARCH_DIR})
            break()
        endif ()
    endforeach()

    if (${SRC_USE_CASE} STREQUAL "")
        message(FATAL_ERROR "Failed to find sources for ${use_case}!")
    endif ()
    # Executable application:
    set(TARGET_NAME "ethos-u-${use_case}")

    set(DEFAULT_MODEL_DIR       ${RESOURCES_DIR}/${use_case})
    set(DEFAULT_TEST_DATA_DIR   ${DEFAULT_MODEL_DIR})
    set(SRC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/src)
    set(INC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/include)

    # Remove old files and recreate dirs
    file(REMOVE_RECURSE ${SRC_GEN_DIR} ${INC_GEN_DIR})
    file(MAKE_DIRECTORY ${SRC_GEN_DIR} ${INC_GEN_DIR})

    file(GLOB_RECURSE UC_SRC
        "${SRC_USE_CASE}/${use_case}/src/*.cpp"
        "${SRC_USE_CASE}/${use_case}/src/*.cc"
        "${SRC_USE_CASE}/${use_case}/src/*.c"
        "${SRC_USE_CASE}/${use_case}/src/**/*.cpp"
        "${SRC_USE_CASE}/${use_case}/src/**/*.cc"
        "${SRC_USE_CASE}/${use_case}/src/**/*.c"
        )

    set(UC_INCLUDE
        ${SRC_USE_CASE}/${use_case}/include
        )

    file(GLOB UC_CMAKE_FILE
        "${SRC_USE_CASE}/${use_case}/*.cmake"
        )

    include(${UC_CMAKE_FILE})

    file(GLOB_RECURSE SRC_GEN
        "${SRC_GEN_DIR}/*.cc"
        "${SRC_GEN_DIR}/*.cpp"
        "${SRC_GEN_DIR}/*.c"
        )

    set(SRC_MAIN "${SRC_PATH}/application/main/Main.cc")

    set_source_files_properties(${SRC_MAIN}
        PROPERTIES COMPILE_DEFINITIONS
        "PRJ_VER_STR=\"${PROJECT_VERSION}\";PRJ_DES_STR=\"${PROJECT_DESCRIPTION}\"")

    set(UC_LIB_NAME lib${TARGET_NAME})

    # Consolidated application static lib:
    add_library(${UC_LIB_NAME} STATIC
        ${SRC_APPLICATION}
        ${SRC_TENSORFLOW_LITE_MICRO}
        ${SRC_HAL}
        ${UC_SRC}
        ${SRC_GEN}
        )
    target_include_directories(${UC_LIB_NAME} PUBLIC
        ${APPLICATION_INCLUDE_DIRS}
        ${DEP_RUNTIME_INCLUDE_DIRS}
        ${UC_INCLUDE}
        ${INC_GEN_DIR}
        )

    # Set the activation buffer size
    target_compile_definitions(${UC_LIB_NAME} PUBLIC
            "ACTIVATION_BUF_SZ=${${use_case}_ACTIVATION_BUF_SZ}")

    add_dependencies(${UC_LIB_NAME} tensorflow-lite-micro)

    if (${CMAKE_CROSSCOMPILING})
        # If we are building timing adapter, set the dependency:
        if (ETHOS_U_NPU_ENABLED)
            message(STATUS "Adding timing_adapter as a dependency to ${UC_LIB_NAME}")
            add_dependencies(${UC_LIB_NAME} timing_adapter)
        endif()

        # If building with CMSIS-DSP support:
        if (DEFINED CMSIS_DSP_TARGET)
            message(STATUS "Adding ${CMSIS_DSP_TARGET} as a dependency to ${UC_LIB_NAME}")
            add_dependencies(${UC_LIB_NAME} ${CMSIS_DSP_TARGET})
        endif()
    endif()

    target_link_libraries(${UC_LIB_NAME} PUBLIC
        ${TENSORFLOW_LIBRARY}
        $<$<BOOL:${ETHOS_U_NPU_ENABLED}>:${ETHOS_U_NPU_LIBS}>
        $<$<BOOL:${CMSIS_DSP_LIB}>:${CMSIS_DSP_LIB}>)

    add_executable(${TARGET_NAME} ${SRC_MAIN} ${PLAT_RETARGET_SOURCE})

    target_link_libraries(${TARGET_NAME} ${UC_LIB_NAME})

    if (${CMAKE_CROSSCOMPILING})
        set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ".axf")
        add_target_map_file(${TARGET_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.map)
    endif()

    if (${TARGET_PLATFORM} STREQUAL mps3 OR ${TARGET_PLATFORM} STREQUAL simple_platform)

        set(SECTORS_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sectors)
        set(SECTORS_BIN_DIR ${SECTORS_DIR}/${use_case})

        file(REMOVE_RECURSE ${SECTORS_BIN_DIR})
        file(MAKE_DIRECTORY ${SECTORS_BIN_DIR})

        add_bin_generation_command(
            TARGET_NAME ${TARGET_NAME}
            OUTPUT_DIR  ${SECTORS_BIN_DIR}
            AXF_PATH    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET_NAME}.axf
            SECTION_PATTERNS    "${LINKER_SECTION_TAGS}"
            OUTPUT_BIN_NAMES    "${LINKER_OUTPUT_BIN_TAGS}")

        if (${TARGET_PLATFORM} STREQUAL mps3)
            file(COPY ${MPS3_FPGA_CONFIG} DESTINATION ${SECTORS_DIR})
        endif()

    elseif (${TARGET_PLATFORM} STREQUAL native)

        # If native build tests
        set(TEST_SRC_USE_CASE "")
        foreach(USE_CASES_TESTS_SEARCH_DIR ${USE_CASES_TESTS_SEARCH_DIR_LIST})

            if (EXISTS ${USE_CASES_TESTS_SEARCH_DIR}/${use_case})
                message(STATUS "Found tests for use-case ${use_case} at ${USE_CASES_TESTS_SEARCH_DIR}/${use_case}.")
                set(TEST_SRC_USE_CASE ${USE_CASES_TESTS_SEARCH_DIR})
                break()
            endif ()
        endforeach()

        # Add tests only if they exists for the usecase
        if (NOT ${TEST_SRC_USE_CASE} STREQUAL "")

            set(TEST_RESOURCES_INCLUDE
                "${TEST_SRCS}/utils/"
                "${TEST_SRC_USE_CASE}/${use_case}/include/"
                )

            # Define Test sources and new target to run unit tests
            file(GLOB_RECURSE TEST_SOURCES
                "${TEST_SRCS}/common/*.cpp"
                "${TEST_SRCS}/common/*.cc"
                "${TEST_SRCS}/utils/*.cc"
                "${TEST_SRCS}/utils/*.cpp"
                "${TEST_SRC_USE_CASE}/${use_case}/*.cpp"
                "${TEST_SRC_USE_CASE}/${use_case}/*.cc"
                "${TEST_SRC_USE_CASE}/${use_case}/*.c"
                "${TEST_SRC_USE_CASE}/${use_case}/**/*.cpp"
                "${TEST_SRC_USE_CASE}/${use_case}/**/*.cc"
                "${TEST_SRC_USE_CASE}/${use_case}/**/*.c"
                )

            set(TEST_SRC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/tests/src)
            set(TEST_INC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/tests/include)
            file(MAKE_DIRECTORY ${TEST_SRC_GEN_DIR} ${TEST_INC_GEN_DIR})

            set(${use_case}_DEFAULT_TEST_DATA_DIR ${DEFAULT_TEST_DATA_DIR} CACHE PATH "")
            # Generate test data files to be included in x86 tests
            generate_test_data_code(
                    INPUT_DIR "${${use_case}_DEFAULT_TEST_DATA_DIR}"
                    DESTINATION_SRC ${TEST_SRC_GEN_DIR}
                    DESTINATION_HDR ${TEST_INC_GEN_DIR}
                    NAMESPACE "test"
            )

            file(GLOB_RECURSE TEST_SOURCES_GEN
                "${TEST_SRC_GEN_DIR}/*.cc"
                "${TEST_SRC_GEN_DIR}/**/*.cc"
                )
            message(STATUS "Adding ${TEST_SOURCES_GEN} to test sources")
            list(APPEND TEST_SOURCES ${TEST_SOURCES_GEN})
            list(APPEND TEST_RESOURCES_INCLUDE ${TEST_INC_GEN_DIR})

            set(TEST_TARGET_NAME "${CMAKE_PROJECT_NAME}-${use_case}-tests")
            add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES})
            target_include_directories(${TEST_TARGET_NAME} PUBLIC
                ${TEST_TPIP_INCLUDE} ${TEST_RESOURCES_INCLUDE})
            target_link_libraries(${TEST_TARGET_NAME} ${UC_LIB_NAME})
            target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
                "ACTIVATION_BUF_SZ=${${use_case}_ACTIVATION_BUF_SZ}"
                TESTS)

            add_dependencies(
                "${TEST_TARGET_NAME}"
                "catch2-headers"
            )

        endif ()
    endif ()
endforeach()

print_useroptions()
