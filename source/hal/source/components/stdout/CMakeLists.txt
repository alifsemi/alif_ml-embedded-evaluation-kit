#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022, 2025 Arm Limited and/or
#  its affiliates <open-source-office@arm.com>
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
#  Wrapper for enabling stdout and stderr facility      #
#########################################################
# This is a wrapper around the UART module for CMSDK    #
# and PL011 UART drivers with retarget functions for    #
# bare-metal targets if semihosting is disabled.        #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(stdout
        DESCRIPTION     "Standard output and err redirection over UART if available"
        LANGUAGES       C CXX)

# By default semihosting is disabled. Read more here:
# https://developer.arm.com/documentation/dui0203/j/semihosting/about-semihosting/what-is-semihosting-
if (CMAKE_CROSSCOMPILING)
    set(SEMIHOSTING_ENABLED     OFF CACHE BOOL "Enable semihosting instead of UART retarget.")
else()
    # For native targets, we don't need to do anything.
    return()
endif()

if (SEMIHOSTING_ENABLED)
    # Enable semihosting support
    if (COMMAND configure_semihosting)
        if (NOT TARGET cmsis_device)
            message(FATAL_ERROR "cmsis_device target should be available to enable semihosting")
        endif()
        message(STATUS "Enabling semihosting")
        configure_semihosting(cmsis_device ON)
    endif()
else()
    # Retarget stdout to UART
    set(STDOUT_COMPONENT_CMSDK stdout_retarget_cmsdk)
    set(STDOUT_COMPONENT_PL011 stdout_retarget_pl011)

    add_library(${STDOUT_COMPONENT_CMSDK} STATIC)
    add_library(${STDOUT_COMPONENT_PL011} STATIC)

    # Check prerequisites
    ## Core platform directory is required to add the UART library project.
    if (NOT DEFINED MLEK_CORE_PLATFORM_DIR)
        message(FATAL_ERROR "MLEK_CORE_PLATFORM_DIR undefined")
    endif()

    ## UART0_BASE is the base address for UART configuration. The platform
    ## should define it prior to including this library.
    if (NOT DEFINED UART0_BASE)
        message(WARNING "UART0_BASE undefined, default will be used.")
    endif()

    ## Platform component: UART
    add_subdirectory(${MLEK_CORE_PLATFORM_DIR}/drivers/uart ${CMAKE_BINARY_DIR}/uart)

    ## Component sources - public
    target_sources(${STDOUT_COMPONENT_CMSDK}
        PUBLIC
        source/retarget.c)

    ## Component sources - public
    target_sources(${STDOUT_COMPONENT_PL011}
        PUBLIC
        source/retarget.c)

    # Link
    target_link_libraries(${STDOUT_COMPONENT_CMSDK}
        PUBLIC
        ethosu_uart_cmsdk_apb)

    target_link_libraries(${STDOUT_COMPONENT_PL011}
        PUBLIC
        ethosu_uart_pl011)

    # Check if semihosting configuration is available
    if (COMMAND configure_semihosting)
        configure_semihosting(${STDOUT_COMPONENT_CMSDK} OFF)
        configure_semihosting(${STDOUT_COMPONENT_PL011} OFF)
    endif()

    # Display status
    message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
    message(STATUS "*******************************************************")
    message(STATUS "Library:                  " ${STDOUT_COMPONENT_CMSDK})
    message(STATUS "Library:                  " ${STDOUT_COMPONENT_PL011})
    message(STATUS "*******************************************************")
endif()
