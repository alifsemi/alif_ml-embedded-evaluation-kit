#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or
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
# Display library for MPS4                              #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(display_component
    DESCRIPTION     "Display support library for MPS4"
    LANGUAGES       C CXX ASM)

# Create static library for MPS4 HDLCD
set(DISPLAY_MPS4_TARGET hal_display_mps4)
add_library(${DISPLAY_MPS4_TARGET} STATIC)

set(HDLCD_FRAME_BUFFER_BASE_ADDRESS "0x90000000" CACHE STRING "Framebuffer base address")
set(HDLCD_RES_WIDTH 320 CACHE STRING "Frame width - target HDLCD resolution")
set(HDLCD_RES_HEIGHT 240 CACHE STRING "Frame height - target HDLCD resolution")
set(HDLCD_BYTES_PER_PIXEL 3 CACHE STRING "Bytes per pixel for HDLCD")

## Definitions - private
target_compile_definitions(${DISPLAY_MPS4_TARGET} PRIVATE
    HDLCD_FRAME_BUFFER_BASE_ADDRESS=${HDLCD_FRAME_BUFFER_BASE_ADDRESS}
    HDLCD_BYTES_PER_PIXEL=${HDLCD_BYTES_PER_PIXEL}
    HDLCD_RES_WIDTH=${HDLCD_RES_WIDTH}
    HDLCD_RES_HEIGHT=${HDLCD_RES_HEIGHT}
    $<$<BOOL:${DISPLAY_TITLE_EN}>:DISPLAY_TITLE_STRING=\"${DISPLAY_TITLE_STRING}\">)

## Component sources
target_sources(${DISPLAY_MPS4_TARGET} PRIVATE
    hal_display_mps4.c)

## Add dependencies
target_link_libraries(${DISPLAY_MPS4_TARGET} PUBLIC
    hal_display_iface
    hal_log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library:                  " ${DISPLAY_MPS4_TARGET})
message(STATUS "*******************************************************")
