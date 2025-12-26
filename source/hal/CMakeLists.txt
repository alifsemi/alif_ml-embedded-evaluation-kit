#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022, 2025 Arm Limited and/or its
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

#########################################################
#                       HAL library                     #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

set(HAL_TARGET hal)
project(${HAL_TARGET}
    DESCRIPTION     "HAL library"
    LANGUAGES       C CXX)

# Create static library
add_library(${HAL_TARGET} STATIC)

## Common include directories - public
target_include_directories(${HAL_TARGET}
    PUBLIC
    include)

## Common sources for all profiles
target_sources(${HAL_TARGET}
    PRIVATE
    source/hal.c
    source/hal_pmu.c)

if (DEFINED VERIFY_TEST_OUTPUT)
    message(STATUS "Test output verification flag is: ${VERIFY_TEST_OUTPUT}")
    target_compile_definitions(${HAL_TARGET} PUBLIC
        VERIFY_TEST_OUTPUT=${VERIFY_TEST_OUTPUT})
endif ()

if (NOT DEFINED PLATFORM_DRIVERS_DIR)
    message(FATAL_ERROR "PLATFORM_DRIVERS_DIR undefined")
endif()

# Include logging interface:
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/components/log)

# Add platform_drivers target
add_subdirectory(${PLATFORM_DRIVERS_DIR} ${CMAKE_BINARY_DIR}/platform_driver)

# Link time library targets:
target_link_libraries(${HAL_TARGET}
    PRIVATE
        hal_log             # Logging functions
    PUBLIC 
        hal_display_iface   # Display interface
        platform_drivers    # Platform drivers implementing the required interfaces
)

# Add a simple test target to verify HAL boots. Exclude it
# from `all` target to reduce noise.
if (COMMAND platform_custom_post_build)
    add_executable(hal-test EXCLUDE_FROM_ALL source/hal_test.c)
    target_link_libraries(hal-test PRIVATE ${HAL_TARGET})
    platform_custom_post_build(TARGET_NAME hal-test)
endif()

# Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${HAL_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
