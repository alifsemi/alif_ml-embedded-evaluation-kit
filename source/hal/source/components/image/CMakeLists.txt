# /* This file was ported to work on Alif Semiconductor Ensemble family of devices. */
#
#/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

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
# Image library                                         #
#########################################################

cmake_minimum_required(VERSION 3.16.3)

project(image_component
    DESCRIPTION     "Image acquire support library"
    LANGUAGES       C CXX ASM)

# Add top level interface library
set(IMAGE_IFACE_TARGET image_iface)
add_library(${IMAGE_IFACE_TARGET} INTERFACE)
target_include_directories(${IMAGE_IFACE_TARGET}
    INTERFACE
    include
    source
    source/ensemble
    source/ensemble/include)

# Create static library for Ensemble data
set(IMAGE_ENSEMBLE_COMPONENT_TARGET image_ensemble)
add_library(${IMAGE_ENSEMBLE_COMPONENT_TARGET} STATIC)

## Component sources
target_sources(${IMAGE_ENSEMBLE_COMPONENT_TARGET}
    PRIVATE
    source/ensemble/image_ensemble.c
    source/ensemble/src/bayer2rgb.c
    source/ensemble/src/tiff.c
    source/ensemble/src/image_processing.c
    source/ensemble/src/color_correction.c
    source/ensemble/src/camera.c
    )

## Logging utilities:
if (NOT TARGET log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

## Add dependencies
target_link_libraries(${IMAGE_ENSEMBLE_COMPONENT_TARGET} PUBLIC
    ${IMAGE_IFACE_TARGET}
    log
    cmsis_ensemble
    platform_drivers_core
    rte_components)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${IMAGE_ENSEMBLE_COMPONENT_TARGET})
message(STATUS "*******************************************************")

# Create static library for Data Stubs
set(IMAGE_STUBS_COMPONENT_TARGET image_stubs)
add_library(${IMAGE_STUBS_COMPONENT_TARGET} STATIC)

## Include directories - private
target_include_directories(${IMAGE_STUBS_COMPONENT_TARGET}
    PRIVATE
    source)

## Component sources
target_sources(${IMAGE_STUBS_COMPONENT_TARGET}
    PRIVATE
    source/image_stubs/image_stubs.c)

## Add dependencies
target_link_libraries(${IMAGE_STUBS_COMPONENT_TARGET} PUBLIC
    ${IMAGE_IFACE_TARGET}
    log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${IMAGE_STUBS_COMPONENT_TARGET})
message(STATUS "*******************************************************")
