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
cmake_minimum_required(VERSION 3.21.0)

project(MLEK_API
        DESCRIPTION "Generic platform agnostic API"
        LANGUAGES   CXX)

# Default configuration - paths and other defaults
include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)

# If this project is added independently by a third-party project, pull in required
# targets by including out-of-hierarchy directories where these dependencies live.
# Begs an argument for pulling `log` and `arm_math` into this directory.
if (NOT TARGET mlek::log)
    add_subdirectory(${MLEK_ROOT}/source/log ${CMAKE_CURRENT_BINARY_DIR}/log)
endif()

if (NOT TARGET mlek::arm_math)
    add_subdirectory(${MLEK_ROOT}/source/math ${CMAKE_CURRENT_BINARY_DIR}/math)
endif()

add_subdirectory(fwk)
add_subdirectory(common)
add_subdirectory(use_case)

###############################################################################
# Function to create an ML framework specific implementation interface library
# to allow easier use of targets defined in this CMake project.
# The function will create an alias library as well namespaced with 'mlek::'
# and the library will be called 'API_NAME_et_impl' or 'API_NAME_tflm_impl'.
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
        message(FATAL_ERROR "mlek::ml_framework_impl target must exist before creating ${TARGET_LIB_NAME}")
    endif()
    add_library(${TARGET_LIB_NAME} INTERFACE)
    target_link_libraries(${TARGET_LIB_NAME} INTERFACE
        ${PARSED_API_NAME}_api      # API target (relies on abstract fwk interface)
        mlek::ml_framework_impl)    # The ML framework impl to pull in

    add_library(mlek::${TARGET_LIB_NAME} ALIAS ${TARGET_LIB_NAME})
    message(STATUS "Created interface target: mlek::${TARGET_LIB_NAME}")
endfunction()

create_mlek_impl_lib_target(API_NAME ad)
create_mlek_impl_lib_target(API_NAME asr)
create_mlek_impl_lib_target(API_NAME img_class)
create_mlek_impl_lib_target(API_NAME inference_runner)
create_mlek_impl_lib_target(API_NAME kws)
create_mlek_impl_lib_target(API_NAME noise_reduction)
create_mlek_impl_lib_target(API_NAME object_detection)
create_mlek_impl_lib_target(API_NAME vww)
