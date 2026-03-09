#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its
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

# Add the profiler target
if (NOT DEFINED PROFILER_DIR)
    set(PROFILER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/profiler)
endif()

add_subdirectory(${PROFILER_DIR} ${CMAKE_BINARY_DIR}/profiler)

# Include directories for application module:
set(APPLICATION_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include)

# Source files for application module:
file(GLOB_RECURSE SRC_APPLICATION
    "${CMAKE_CURRENT_SOURCE_DIR}/main/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/main/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main/*.c"
    )
list(FILTER SRC_APPLICATION EXCLUDE REGEX ".*main\\.c.*$")
set(SRC_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/main/Main.cc")

list(APPEND SRC_MAIN_DEFS
    "PRJ_VER_STR=\"${CMAKE_PROJECT_VERSION}\""
    "PRJ_DES_STR=\"${CMAKE_PROJECT_DESCRIPTION}\""
    "PRJ_COMPILER=\"${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}\"")
set_source_files_properties(${SRC_MAIN}
    PROPERTIES COMPILE_DEFINITIONS
    "${SRC_MAIN_DEFS}")

list(JOIN USE_CASE_BUILD "" USE_CASE_BUILD_STR)
list(APPEND USE_CASES_SEARCH_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR}/use_case)
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

# For some board configurations, these definitions are used by
# platform_custom_post_build to configure tests.
set(TEST_SRCS  ${CMAKE_CURRENT_SOURCE_DIR}/../../tests)
list(APPEND USE_CASES_TESTS_SEARCH_DIR_LIST ${TEST_SRCS}/use_case)

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
    set(TARGET_NAME "mlek_${use_case}")

    set(DEFAULT_MODEL_DIR ${RESOURCES_PATH}/${use_case})
    set(DEFAULT_TEST_DATA_DIR ${DEFAULT_MODEL_DIR})
    set(SRC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/src)
    set(INC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/include)
    set(SAMPLES_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/samples)

    # Remove old files and recreate dirs
    file(REMOVE_RECURSE ${SRC_GEN_DIR} ${INC_GEN_DIR} ${SAMPLES_GEN_DIR})
    file(MAKE_DIRECTORY ${SRC_GEN_DIR} ${INC_GEN_DIR} ${SAMPLES_GEN_DIR})

    file(GLOB UC_CMAKE_FILE
        "${SRC_USE_CASE}/${use_case}/*.cmake")

    # Include the use case cmake file.
    include(${UC_CMAKE_FILE})

    # Proceed only if use case supports the chosen ML framework
    if (NOT ${use_case}_supports_${ML_FRAMEWORK})
        message(STATUS "${use_case} skipped")
        continue()
    endif ()

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

    # Checking and setting the interactive mode
    if(INTERACTIVE_MODE)
        target_compile_definitions(${UC_LIB_NAME} PUBLIC "INTERACTIVE_MODE")
    endif()

    target_include_directories(${UC_LIB_NAME} PUBLIC
        ${APPLICATION_INCLUDE_DIRS}
        ${UC_INCLUDE}
        ${INC_GEN_DIR})

    # Set the activation buffer size
    target_compile_definitions(${UC_LIB_NAME} PUBLIC
        "ACTIVATION_BUF_SZ=${${use_case}_ACTIVATION_BUF_SZ}")

    target_link_libraries(${UC_LIB_NAME} PUBLIC
        hal
        profiler)

    # If an API exists for this use case, include the projects here and add to
    # the library list.
    foreach(API_TO_USE ${${use_case}_API_LIST})
        # Check if the target now exists
        if (TARGET ${API_TO_USE}_impl)
            target_link_libraries(${UC_LIB_NAME} PUBLIC ${API_TO_USE}_impl)
        else()
            message(FATAL_ERROR "${API_TO_USE}_impl target not found!")
        endif()
    endforeach()

    # If the use case has additional library target requirements,
    # link them here.
    if (DEFINED ${use_case}_LINK_LIBS)
        message(STATUS "Additional link libraries specified for ${use_case}: "
                       "${${use_case}_LINK_LIBS}")

        # For each library target, check if it exists before linking.
        foreach(_LIB ${${use_case}_LINK_LIBS})
            if (TARGET ${_LIB})
                target_link_libraries(${UC_LIB_NAME} PUBLIC ${_LIB})
            else()
                message(FATAL_ERROR "${_LIB} requested by ${use_case} not found. ")
            endif()
        endforeach()
    endif()

    file(GLOB_RECURSE SRC_SAMPLES
            "${SAMPLES_GEN_DIR}/*.cc"
            "${SAMPLES_GEN_DIR}/*.cpp"
            "${SAMPLES_GEN_DIR}/*.c")

    add_executable(${TARGET_NAME} ${SRC_MAIN} ${SRC_SAMPLES})
    target_link_libraries(${TARGET_NAME} PUBLIC ${UC_LIB_NAME})

    if (DEFINED ${use_case}_LINKER_SCRIPT_OVERRIDE_PATH
            AND NOT "${${use_case}_LINKER_SCRIPT_OVERRIDE_PATH}" STREQUAL "")
        set_property(TARGET ${TARGET_NAME} PROPERTY
            MLEK_LINKER_SCRIPT_OVERRIDE_PATH
            "${${use_case}_LINKER_SCRIPT_OVERRIDE_PATH}")
    endif()

    platform_custom_post_build(TARGET_NAME ${TARGET_NAME})
endforeach()
