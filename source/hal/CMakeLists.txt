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

#########################################################
#                       HAL library                     #
#########################################################

cmake_minimum_required(VERSION 3.15.6)

set(HAL_TARGET hal)

project(${HAL_TARGET}
    DESCRIPTION     "HAL library"
    LANGUAGES       C CXX)

# Create static library
add_library(${HAL_TARGET} STATIC)

# Select which profile needs to be used:
if (${CMAKE_CROSSCOMPILING})
    set(PLATFORM_PROFILE bare-metal)
else()
    set(PLATFORM_PROFILE native)
endif()

set(PLATFORM_PROFILE_DIR    profiles/${PLATFORM_PROFILE})

## Common include directories - public
target_include_directories(${HAL_TARGET}
    PUBLIC
    include
    ${PLATFORM_PROFILE_DIR}/timer/include)

## Common sources for all profiles
target_sources(${HAL_TARGET}
    PRIVATE
    hal.c
    ${PLATFORM_PROFILE_DIR}/data_presentation/data_psn.c
    ${PLATFORM_PROFILE_DIR}/data_acquisition/data_acq.c
    ${PLATFORM_PROFILE_DIR}/timer/platform_timer.c)

if (DEFINED VERIFY_TEST_OUTPUT)
    message(STATUS "Test output verification flag is: ${VERIFY_TEST_OUTPUT}")
    target_compile_definitions(${HAL_TARGET} PUBLIC
        VERIFY_TEST_OUTPUT=${VERIFY_TEST_OUTPUT})
endif ()

if (NOT DEFINED PLATFORM_DRIVERS_DIR)
    message(FATAL_ERROR "PLATFORM_DRIVERS_DIR need to be defined for this target")
endif()


############################ bare-metal profile #############################
if (PLATFORM_PROFILE STREQUAL bare-metal)

    ## Additional include directories - private
    target_include_directories(${HAL_TARGET}
        PRIVATE
        ${PLATFORM_PROFILE_DIR}/data_presentation/lcd/include)

    ## Additional sources - public
    target_sources(${HAL_TARGET}
        PUBLIC
        ${PLATFORM_PROFILE_DIR}/bsp/retarget.c)

    ## Additional sources - private
    target_sources(${HAL_TARGET}
        PRIVATE
        ${PLATFORM_PROFILE_DIR}/data_presentation/lcd/lcd_img.c)

    ## Compile definition:
    target_compile_definitions(${HAL_TARGET}
        PUBLIC
        PLATFORM_HAL=PLATFORM_CORTEX_M_BAREMETAL)

    # Add dependencies for platform_driver first, in case they are needed by it.
    add_subdirectory(cmsis_device ${CMAKE_BINARY_DIR}/cmsis_device)

############################   native profile   #############################
elseif (PLATFORM_PROFILE STREQUAL native)

    ## Additional include directories - private
    target_include_directories(${HAL_TARGET}
        PRIVATE
        ${PLATFORM_PROFILE_DIR}/data_presentation/log/include)

    ## Additional sources - private
    target_sources(${HAL_TARGET}
        PRIVATE
        ${PLATFORM_PROFILE_DIR}/data_presentation/log/log.c)

    ## Compile definition:
    target_compile_definitions(${HAL_TARGET}
        PUBLIC
        PLATFORM_HAL=PLATFORM_UNKNOWN_LINUX_OS
        ACTIVATION_BUF_SRAM_SZ=0)
endif()

# Add platform-drivers target
add_subdirectory(${PLATFORM_DRIVERS_DIR} ${CMAKE_BINARY_DIR}/platform_driver)

# Link time library targets:
target_link_libraries(${HAL_TARGET}
    PUBLIC
    log
    platform-drivers)

# Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${HAL_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
