#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
# and PL011 UART drivers with retarget functions.       #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(stdout
    DESCRIPTION     "Standard output and err redirection over UART"
    LANGUAGES       C CXX)


set(STDOUT_RETARGET     OFF CACHE BOOL "Retarget stdout/err to UART")

# Interface library for standard output:
set(STDOUT_IFACE_TARGET stdout_iface)
add_library(${STDOUT_IFACE_TARGET} INTERFACE)
target_include_directories(${STDOUT_IFACE_TARGET} INTERFACE include)

if (STDOUT_RETARGET)

    set(STDOUT_COMPONENT_CMSDK stdout_retarget_cmsdk)
    set(STDOUT_COMPONENT_PL011 stdout_retarget_pl011)

    add_library(${STDOUT_COMPONENT_CMSDK} STATIC)
    add_library(${STDOUT_COMPONENT_PL011} STATIC)

    # Check prerequisites
    ## Core platform directory is required to add the UART library project.
    if (NOT DEFINED CORE_PLATFORM_DIR)
        message(FATAL_ERROR "CORE_PLATFORM_DIR undefined")
    endif()

    ## UART0_BASE is the base address for UART configuration. The platform
    ## should define it prior to including this library.
    if (NOT DEFINED UART0_BASE)
        message(WARNING "UART0_BASE undefined, default will be used.")
    endif()

    ## Platform component: UART
    add_subdirectory(${CORE_PLATFORM_DIR}/drivers/uart ${CMAKE_BINARY_DIR}/uart)

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
        ${STDOUT_IFACE_TARGET}
        ethosu_uart_cmsdk_apb)

    target_link_libraries(${STDOUT_COMPONENT_PL011}
        PUBLIC
        ${STDOUT_IFACE_TARGET}
        ethosu_uart_pl011)

    # Display status
    message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
    message(STATUS "*******************************************************")
    message(STATUS "Library:                  " ${STDOUT_COMPONENT_CMSDK})
    message(STATUS "Library:                  " ${STDOUT_COMPONENT_PL011})
    message(STATUS "*******************************************************")

else()

    # Create static library for retarget (stdout/err over UART)
    set(STDOUT_COMPONENT stdout)
    add_library(${STDOUT_COMPONENT} STATIC)

    ## Component sources - public
    target_sources(${STDOUT_COMPONENT}
        PUBLIC
        source/user_input.c)

    target_link_libraries(${STDOUT_COMPONENT}
        PUBLIC
        ${STDOUT_IFACE_TARGET})

    # Display status
    message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
    message(STATUS "*******************************************************")
    message(STATUS "Library:                  " ${STDOUT_COMPONENT})
    message(STATUS "*******************************************************")
endif()
