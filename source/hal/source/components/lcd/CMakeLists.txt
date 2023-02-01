# This file was ported to work on Alif Semiconductor Ensemble family of devices.

#  Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

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
# LCD library                                           #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(lcd_component
    DESCRIPTION     "LCD support library"
    LANGUAGES       C CXX ASM)

# Add top level interface library
set(LCD_IFACE_TARGET lcd_iface)
add_library(${LCD_IFACE_TARGET} INTERFACE)
target_include_directories(${LCD_IFACE_TARGET} INTERFACE include)

# Create static library for MPS3 LCD
set(LCD_MPS3_COMPONENT_TARGET lcd_mps3)
add_library(${LCD_MPS3_COMPONENT_TARGET} STATIC)

set(CLCD_CONFIG_BASE "0x4930A000" CACHE STRING "LCD configuration base address")

## Include directories - private
target_include_directories(${LCD_MPS3_COMPONENT_TARGET}
    PRIVATE
    source)

## Component sources
target_sources(${LCD_MPS3_COMPONENT_TARGET}
    PRIVATE
    source/glcd_mps3/glcd_mps3.c
    source/lcd_img.c)

# Compile definitions
target_compile_definitions(${LCD_MPS3_COMPONENT_TARGET}
    PRIVATE
    CLCD_CONFIG_BASE=${CLCD_CONFIG_BASE})

## Logging utilities:
if (NOT TARGET log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

## Add dependencies
target_link_libraries(${LCD_MPS3_COMPONENT_TARGET} PUBLIC
    ${LCD_IFACE_TARGET}
    log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${LCD_MPS3_COMPONENT_TARGET})
message(STATUS "*******************************************************")

# Create static library for LCD Stubs
set(LCD_STUBS_COMPONENT_TARGET lcd_stubs)
add_library(${LCD_STUBS_COMPONENT_TARGET} STATIC)

## Include directories - private
target_include_directories(${LCD_STUBS_COMPONENT_TARGET}
    PRIVATE
    source)

## Component sources
target_sources(${LCD_STUBS_COMPONENT_TARGET}
    PRIVATE
    source/glcd_stubs/glcd_stubs.c
    source/lcd_img.c)

## Add dependencies
target_link_libraries(${LCD_STUBS_COMPONENT_TARGET} PUBLIC
    ${LCD_IFACE_TARGET}
    log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${LCD_STUBS_COMPONENT_TARGET})
message(STATUS "*******************************************************")

# Create static library for LVGL LCD
set(LCD_LVGL_COMPONENT_TARGET lcd_lvgl)
add_library(${LCD_LVGL_COMPONENT_TARGET} STATIC)

## Include directories - private
target_include_directories(${LCD_LVGL_COMPONENT_TARGET}
    PRIVATE
    source)

## Component sources
target_sources(${LCD_LVGL_COMPONENT_TARGET}
    PRIVATE
    source/glcd_lvgl/glcd_lvgl.c
    source/glcd_lvgl/lv_font_9x15_h.c
    source/lcd_img.c)

## Add dependencies
target_link_libraries(${LCD_LVGL_COMPONENT_TARGET} PUBLIC
    ${LCD_IFACE_TARGET}
    lvgl
    log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${LCD_LVGL_COMPONENT_TARGET})
message(STATUS "*******************************************************")
