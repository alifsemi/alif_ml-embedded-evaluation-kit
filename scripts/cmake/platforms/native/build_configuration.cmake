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
    message(STATUS "Platform: Native (Linux based x86_64/aarch64 system)")
    if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_DIR}/native-gcc.cmake
                CACHE FILEPATH "Toolchain file")
    endif()

    set(PLATFORM_DRIVERS_DIR "${HAL_PLATFORM_DIR}/native" PARENT_SCOPE)
    set(TEST_TPIP ${DOWNLOAD_DEP_DIR}/test)

    file(MAKE_DIRECTORY ${TEST_TPIP})
    set(TEST_TPIP_INCLUDE ${TEST_TPIP}/include)
    file(MAKE_DIRECTORY ${TEST_TPIP_INCLUDE})

    ExternalProject_Add(catch2-headers
            URL https://github.com/catchorg/Catch2/releases/download/v2.11.1/catch.hpp
            DOWNLOAD_NO_EXTRACT 1
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ${CMAKE_COMMAND} -E copy <DOWNLOAD_DIR>/catch.hpp ${TEST_TPIP_INCLUDE}
            INSTALL_COMMAND "")

    add_library(catch2 INTERFACE)
    target_include_directories(catch2
            INTERFACE
            ${TEST_TPIP_INCLUDE})
    add_dependencies(catch2 catch2-headers)

endfunction()

function(platform_custom_post_build)
    set(oneValueArgs TARGET_NAME)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "" ${ARGN} )


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

        set(TEST_TARGET_NAME "${use_case}_tests")
        add_executable(${TEST_TARGET_NAME} ${TEST_SOURCES})
        target_include_directories(${TEST_TARGET_NAME} PRIVATE ${TEST_RESOURCES_INCLUDE})
        target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${UC_LIB_NAME} catch2)
        target_compile_definitions(${TEST_TARGET_NAME} PRIVATE
                "ACTIVATION_BUF_SZ=${${use_case}_ACTIVATION_BUF_SZ}"
                TESTS)
        add_test(NAME "${use_case}-tests" COMMAND ${TEST_TARGET_NAME} -r junit -o ${TEST_TARGET_NAME}.xml)
    endif ()
endfunction()