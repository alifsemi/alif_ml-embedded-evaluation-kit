#----------------------------------------------------------------------------
#  Copyright (c) 2021 - 2022 Arm Limited. All rights reserved.
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
cmake_minimum_required(VERSION 3.15.6)
include(ExternalProject)

# Build in release mode by default
if (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_BUILD_TYPE Release CACHE INTERNAL "")
endif()

message(STATUS "Build type is set to ${CMAKE_BUILD_TYPE}")

# Set language standards.
set(CMAKE_C_STANDARD   99)
set(CMAKE_CXX_STANDARD 14)

# Make the standard a requirement => prevent fallback to previous
# supported standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We want to pass standard C/C++ flags, without gnu extensions
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SCRIPTS_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
set(CMAKE_SCRIPTS_DIR   ${SCRIPTS_DIR}/cmake)
set(CMAKE_TOOLCHAIN_DIR ${CMAKE_SCRIPTS_DIR}/toolchains)
set(DOWNLOAD_DEP_DIR    ${CMAKE_BINARY_DIR}/dependencies)
set(DEPENDENCY_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

set(RESOURCES_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/resources_downloaded
                        CACHE PATH "Resources directory")
set(HAL_PLATFORM_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/source/hal/platform)

include(${CMAKE_SCRIPTS_DIR}/source_gen_utils.cmake)

if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(FATAL_ERROR "Source and build are in the same directory")
else()
    message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
    message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
endif()

include(${CMAKE_SCRIPTS_DIR}/common_user_options.cmake)

add_platform_build_configuration(TARGET_PLATFORM ${TARGET_PLATFORM})

set_platform_global_defaults()

message(STATUS "Using CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

# Make sure the following options are defined before proceeding:
assert_defined(LOG_LEVEL)
assert_defined(TENSORFLOW_SRC_PATH)
assert_defined(TARGET_PLATFORM)
assert_defined(TARGET_SUBSYSTEM)
assert_defined(ETHOS_U_NPU_ENABLED)
assert_defined(USE_CASE_BUILD)
assert_defined(CPU_PROFILE_ENABLED)
assert_defined(CMAKE_TOOLCHAIN_FILE)

project(arm_ml_embedded_evaluation_kit
        VERSION     21.11.1
        DESCRIPTION "ARM ML Embedded Evaluation Kit"
        LANGUAGES   C CXX ASM)

enforce_compiler_version()
setup_source_generator()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TEST_SRCS  ${CMAKE_CURRENT_SOURCE_DIR}/tests)
list(APPEND USE_CASES_TESTS_SEARCH_DIR_LIST ${TEST_SRCS}/use_case)

# We include log target
add_subdirectory(${SRC_PATH}/log ${CMAKE_BINARY_DIR}/log)

# We include arm_math target
add_subdirectory(${SRC_PATH}/math ${CMAKE_BINARY_DIR}/math)

# We include the hal target
add_subdirectory(${SRC_PATH}/hal ${CMAKE_BINARY_DIR}/hal)

# Add the profiler target
if (NOT DEFINED PROFILER_DIR)
    set(PROFILER_DIR ${SRC_PATH}/profiler)
endif ()
add_subdirectory(${PROFILER_DIR} ${CMAKE_BINARY_DIR}/profiler)

# Include the tensorflow build target
include(${CMAKE_SCRIPTS_DIR}/tensorflow.cmake)

# Include directories for application module:
set(APPLICATION_INCLUDE_DIRS
    ${SRC_PATH}/application/tensorflow-lite-micro/include
    ${SRC_PATH}/application/main/include)

# Source files for application module:
file(GLOB_RECURSE SRC_APPLICATION
    "${SRC_PATH}/application/main/*.cc"
    "${SRC_PATH}/application/main/*.cpp"
    "${SRC_PATH}/application/main/*.c"
    "${SRC_PATH}/application/main/**/*.cc"
    "${SRC_PATH}/application/main/**/*.cpp"
    "${SRC_PATH}/application/main/**/*.c"
    "${SRC_PATH}/application/tensorflow-lite-micro/**/*.cc"
    "${SRC_PATH}/application/tensorflow-lite-micro/*.cc"
    )
list(FILTER SRC_APPLICATION EXCLUDE REGEX ".*main\\.c.*$")
set(SRC_MAIN "${SRC_PATH}/application/main/Main.cc")
set_source_files_properties(${SRC_MAIN}
    PROPERTIES COMPILE_DEFINITIONS
    "PRJ_VER_STR=\"${PROJECT_VERSION}\";PRJ_DES_STR=\"${PROJECT_DESCRIPTION}\"")

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

list(REMOVE_ITEM USE_CASES "" ${EXCLUDED_USE_CASES})
message(STATUS "Use-cases excluded by platform configuration: ${EXCLUDED_USE_CASES}")
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

    set(DEFAULT_MODEL_DIR ${RESOURCES_DIR}/${use_case})
    set(DEFAULT_TEST_DATA_DIR ${DEFAULT_MODEL_DIR})
    set(SRC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/src)
    set(INC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/include)

    # Remove old files and recreate dirs
    file(REMOVE_RECURSE ${SRC_GEN_DIR} ${INC_GEN_DIR})
    file(MAKE_DIRECTORY ${SRC_GEN_DIR} ${INC_GEN_DIR})

    file(GLOB UC_CMAKE_FILE
        "${SRC_USE_CASE}/${use_case}/*.cmake")

    include(${UC_CMAKE_FILE})

    file(GLOB_RECURSE UC_SRC
        "${SRC_USE_CASE}/${use_case}/src/*.cpp"
        "${SRC_USE_CASE}/${use_case}/src/*.cc"
        "${SRC_USE_CASE}/${use_case}/src/*.c"
        "${SRC_USE_CASE}/${use_case}/src/**/*.cpp"
        "${SRC_USE_CASE}/${use_case}/src/**/*.cc"
        "${SRC_USE_CASE}/${use_case}/src/**/*.c")

    file(GLOB_RECURSE SRC_GEN
        "${SRC_GEN_DIR}/*.cc"
        "${SRC_GEN_DIR}/*.cpp"
        "${SRC_GEN_DIR}/*.c")

    set(UC_INCLUDE  ${SRC_USE_CASE}/${use_case}/include)

    if (DEFINED ${use_case}_COMPILE_DEFS)
        message(STATUS "Additional compilation flags for ${use_case}: ${${use_case}_COMPILE_DEFS}")
        set_source_files_properties(${UC_SRC}
            PROPERTIES COMPILE_DEFINITIONS
            "${${use_case}_COMPILE_DEFS}")
    endif()

    set(UC_LIB_NAME lib${TARGET_NAME})

    # Consolidated application static lib:
    add_library(${UC_LIB_NAME} STATIC
        ${SRC_APPLICATION}
        ${UC_SRC}
        ${SRC_GEN})

    target_include_directories(${UC_LIB_NAME} PUBLIC
        ${APPLICATION_INCLUDE_DIRS}
        ${UC_INCLUDE}
        ${INC_GEN_DIR}
        ${TENSORFLOW_SRC_PATH}/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include)

    # Set the activation buffer size
    target_compile_definitions(${UC_LIB_NAME} PUBLIC
            "ACTIVATION_BUF_SZ=${${use_case}_ACTIVATION_BUF_SZ}")

    # Set the CPU profiling defintiion
    target_compile_definitions(${UC_LIB_NAME} PRIVATE
            $<$<BOOL:${CPU_PROFILE_ENABLED}>:CPU_PROFILE_ENABLED>)

    target_link_libraries(${UC_LIB_NAME} PUBLIC
        log
        arm_math
        hal
        profiler
        tensorflow-lite-micro)

    add_executable(${TARGET_NAME} ${SRC_MAIN})

    target_link_libraries(${TARGET_NAME} PUBLIC ${UC_LIB_NAME})

    platform_custom_post_build(TARGET_NAME ${TARGET_NAME})

endforeach()

print_useroptions()