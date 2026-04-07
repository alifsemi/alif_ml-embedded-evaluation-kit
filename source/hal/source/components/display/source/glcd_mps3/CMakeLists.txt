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
# Display library for MPS3                              #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(display_component
    DESCRIPTION     "Display support library for MPS3"
    LANGUAGES       C CXX ASM)

# Create static library for MPS3 LCD
set(DISPLAY_MPS3_TARGET hal_display_mps3)
add_library(${DISPLAY_MPS3_TARGET} STATIC)

set(CLCD_CONFIG_BASE "0x4930A000" CACHE STRING "LCD configuration base address")
option(DISPLAY_TITLE_EN "Enables display of title string on LCD" ON)

if (DISPLAY_TITLE_EN)
    set(DISPLAY_TITLE_STRING "Arm ML Embedded Evaluation Kit" CACHE STRING "Title string")
endif()

## Component sources
target_sources(${DISPLAY_MPS3_TARGET} PRIVATE
    glcd_mps3.c
    hal_display_mps3.c)

# Compile definitions
target_compile_definitions(${DISPLAY_MPS3_TARGET}
    PRIVATE
    CLCD_CONFIG_BASE=${CLCD_CONFIG_BASE}
    $<$<BOOL:${DISPLAY_TITLE_EN}>:DISPLAY_TITLE_STRING=\"${DISPLAY_TITLE_STRING}\">)

## Add dependencies
target_link_libraries(${DISPLAY_MPS3_TARGET} PUBLIC
    hal_display_iface
    hal_log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library:                  " ${DISPLAY_MPS3_TARGET})
message(STATUS "*******************************************************")
