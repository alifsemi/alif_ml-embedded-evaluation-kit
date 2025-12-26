#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022, 2024-2025 Arm Limited and/or
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
# Display library                                       #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(display_component
    DESCRIPTION     "Display support library"
    LANGUAGES       C CXX ASM)

# Add top level interface library
set(DISPLAY_IFACE_TARGET hal_display_iface)
add_library(${DISPLAY_IFACE_TARGET} INTERFACE)
target_include_directories(${DISPLAY_IFACE_TARGET} INTERFACE include)

## Logging utilities:
if (NOT TARGET hal_log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

# Create static library for LCD Stubs
set(DISPLAY_STUBS_TARGET hal_display_stubs)
add_library(${DISPLAY_STUBS_TARGET} STATIC)

## Component sources
target_sources(${DISPLAY_STUBS_TARGET} PRIVATE
    source/hal_display_stubs.c)

## Add dependencies
target_link_libraries(${DISPLAY_STUBS_TARGET} PUBLIC
    ${DISPLAY_IFACE_TARGET}
    hal_log)

# Add sub projects
add_subdirectory(source/glcd_mps3 EXCLUDE_FROM_ALL)
add_subdirectory(source/hdlcd_mps4 EXCLUDE_FROM_ALL)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR:" ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library:                 " ${DISPLAY_STUBS_TARGET})
message(STATUS "*******************************************************")
