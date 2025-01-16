# This file was ported to work on Alif Semiconductor Ensemble family of devices.

#  Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2021-2024 Arm Limited and/or its
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
cmake_minimum_required(VERSION 3.21.0)

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

# Initialise global settings
include("${CMAKE_CURRENT_SOURCE_DIR}/MlekModule.cmake")

project(arm_ml_embedded_evaluation_kit
        VERSION     24.08.0
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

# Add user case definition so it can be used to #ifdef in code
if (NOT "${USE_CASE_BUILD}" STREQUAL "all")
string(TOUPPER "${USE_CASE_BUILD}" use_case_upper)
add_compile_definitions(${use_case_upper})
endif()

# We include log target
add_subdirectory(${SRC_PATH}/log ${CMAKE_BINARY_DIR}/log EXCLUDE_FROM_ALL)

# We include arm_math target
add_subdirectory(${SRC_PATH}/math ${CMAKE_BINARY_DIR}/math EXCLUDE_FROM_ALL)

# We include the hal target
add_subdirectory(${SRC_PATH}/hal ${CMAKE_BINARY_DIR}/hal EXCLUDE_FROM_ALL)

# Add the profiler target
if (NOT DEFINED PROFILER_DIR)
    set(PROFILER_DIR ${SRC_PATH}/profiler)
endif ()
add_subdirectory(${PROFILER_DIR} ${CMAKE_BINARY_DIR}/profiler EXCLUDE_FROM_ALL)

# Include the tensorflow build target
include(tensorflow_lite_micro)

# Add the common API library target (tensorflow-lite-micro target is needed)
add_subdirectory(${SRC_PATH}/application/api/common ${CMAKE_BINARY_DIR}/api/common EXCLUDE_FROM_ALL)

# Include directories for application module:
set(APPLICATION_INCLUDE_DIRS
    ${SRC_PATH}/application/main/include)

# Source files for application module:
file(GLOB_RECURSE SRC_APPLICATION
    "${SRC_PATH}/application/main/*.cc"
    "${SRC_PATH}/application/main/*.cpp"
    "${SRC_PATH}/application/main/*.c"
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

    set(DEFAULT_MODEL_DIR ${RESOURCES_PATH}/${use_case})
    set(DEFAULT_TEST_DATA_DIR ${DEFAULT_MODEL_DIR})
    set(SRC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/src)
    set(INC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/include)

    # Remove old files and recreate dirs
    file(REMOVE_RECURSE ${SRC_GEN_DIR} ${INC_GEN_DIR})
    file(MAKE_DIRECTORY ${SRC_GEN_DIR} ${INC_GEN_DIR})

    file(GLOB UC_CMAKE_FILE
        "${SRC_USE_CASE}/${use_case}/*.cmake")

    # Include the use case cmake file.
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

    set(UC_LIB_NAME ${use_case})

    # Consolidated application static lib:
    add_library(${UC_LIB_NAME} STATIC
        ${SRC_APPLICATION}
        ${UC_SRC}
        ${SRC_GEN})

    target_include_directories(${UC_LIB_NAME} PUBLIC
        ${APPLICATION_INCLUDE_DIRS}
        ${UC_INCLUDE}
        ${INC_GEN_DIR})

    # Set the activation buffer size
    target_compile_definitions(${UC_LIB_NAME} PUBLIC
        "ACTIVATION_BUF_SZ=${${use_case}_ACTIVATION_BUF_SZ}")

    # Set model in external flash
    if (${use_case}_MODEL_IN_EXT_FLASH)
        target_compile_definitions(${UC_LIB_NAME} PUBLIC
            MODEL_IN_EXT_FLASH=1)
    endif()

    target_link_libraries(${UC_LIB_NAME} PUBLIC
        log
        arm_math
        hal
        profiler
        tensorflow-lite-micro
        common_api)

    # If an API exists for this use case, include the projects here and add to
    # the library list.
    foreach(API_TO_USE ${${use_case}_API_LIST})

        # If the required target doesn't yet exist, include the project here:
        if (NOT TARGET ${API_TO_USE}_api)
            add_subdirectory(
                ${SRC_PATH}/application/api/use_case/${API_TO_USE}  # Source path
                ${CMAKE_BINARY_DIR}/api/use_case/${API_TO_USE})     # Binary path
            if (ETHOS_U_NPU_ASSUMED)
                target_compile_definitions(${API_TO_USE}_api PRIVATE
                    ETHOS_U_NPU_ASSUMED=1)
            endif()
        endif()

        # Check if the target now exists
        if (TARGET ${API_TO_USE}_api)
            message(STATUS "Using ${API_TO_USE}_api for ${use_case}")
            target_link_libraries(${UC_LIB_NAME} PUBLIC ${API_TO_USE}_api)
        else()
            message(FATAL_ERROR "${API_TO_USE}_api target not found!")
        endif()
    endforeach()

    add_executable(${TARGET_NAME} ${SRC_MAIN})

    target_link_libraries(${TARGET_NAME} PUBLIC ${UC_LIB_NAME})

    platform_custom_post_build(TARGET_NAME ${TARGET_NAME}
                               MODEL_IN_EXT_FLASH ${${use_case}_MODEL_IN_EXT_FLASH})

endforeach()

print_useroptions()
