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

# Build in release mode by default
if (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_BUILD_TYPE Release CACHE INTERNAL "")
endif()

# Set language standards.
set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)

# Make the standard a requirement => prevent fallback to previous
# supported standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# We want to pass standard C/C++ flags, without gnu extensions
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

project(MLEK_API
        DESCRIPTION "Generic platform agnostic API"
        LANGUAGES   CXX)

# Default configuration - paths and other defaults
include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)

add_subdirectory(mlek/log)
add_subdirectory(mlek/math)
add_subdirectory(mlek/fwk)
add_subdirectory(mlek/common)
add_subdirectory(mlek/use_case)

###############################################################################
# Function to add current directory to the targets includes. This ensures
# that all the headers included refer to this base directory as the root.
# It will be the same for installation target.
# @param[in]    target_name     Name of the CMake target to add include
#                               directories for.
###############################################################################
function(mlek_api_set_include_dirs target_name)
    if (NOT TARGET ${target_name})
        message(WARNING "${target_name} is not defined")
        return()
    endif()

    get_target_property(_target_type ${target_name} TYPE)
    if (_target_type STREQUAL "INTERFACE_LIBRARY")
        set(_scope INTERFACE)
    else()
        set(_scope PUBLIC)
    endif()

    target_include_directories(${target_name} ${_scope}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
endfunction()


###############################################################################
# Function to create an ML framework specific implementation interface library
# to allow easier use of targets defined in this CMake project.
# The function will create an alias library as well namespaced with 'mlek::'
# and the library will be called 'API_NAME_impl'.
# @param[in]    API_NAME        Name of the API to target. The name is suffixed
#                               by '_api' to get the expected target defined
#                               by subdirectory 'use_case'.
###############################################################################
function(create_mlek_impl_lib_target)
    set(oneValueArgs API_NAME)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})
    set(TARGET_LIB_NAME ${PARSED_API_NAME}_impl)
    if (NOT TARGET mlek::ml_framework_impl)
        message(FATAL_ERROR "mlek::ml_framework_impl target must "
                            "exist before creating ${TARGET_LIB_NAME}")
    endif()
    add_library(${TARGET_LIB_NAME} INTERFACE)
    target_link_libraries(${TARGET_LIB_NAME} INTERFACE
        ${PARSED_API_NAME}_api      # API target (relies on abstract fwk interface)
        mlek::ml_framework_impl)    # The ML framework impl to pull in

    add_library(mlek::${TARGET_LIB_NAME} ALIAS ${TARGET_LIB_NAME})
    message(STATUS "Created interface target: mlek::${TARGET_LIB_NAME}")
endfunction()

# Set include directories for base targets. All other targets depend on at
# least one of these targets.
mlek_api_set_include_dirs(mlek_log)
mlek_api_set_include_dirs(arm_math)
mlek_api_set_include_dirs(common_api)
mlek_api_set_include_dirs(ml_framework_iface)

# Create alises and add namepaced targets for easier linking.
if (NOT DEFINED MLEK_API_LIST)
    message(FATAL_ERROR "MLEK_API_LIST variable should be exposed by "
                        "${CMAKE_CURRENT_SOURCE_DIR}/mlek/use_case")
endif()

if (TARGET mlek::ml_framework_impl)
    foreach(_api_name ${MLEK_API_LIST})
        create_mlek_impl_lib_target(API_NAME ${_api_name})
    endforeach()
else()
    message(STATUS "No MLEK ML framework implementation target; "
                   "skipping use-case implementation convenience targets.")
endif()

# Only include export/install logic when building as part of the top-level MLEK
# project. When this library is embedded in another project, defer export to
# the parent to avoid clashing install/export settings.
if (CMAKE_SOURCE_DIR STREQUAL MLEK_ROOT)
    include(${CMAKE_CURRENT_SOURCE_DIR}/export.cmake)
endif()
