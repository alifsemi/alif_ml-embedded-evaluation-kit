# /* This file was ported to work on Alif Semiconductor Ensemble family of devices. */
#
#/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

#########################################################
# Ensemble Camera library                               #
#########################################################

cmake_minimum_required(VERSION 3.16.3)

project(hal_camera_ensemble
    DESCRIPTION     "Ensemble camera interface library"
    LANGUAGES       C CXX ASM)

# Add top level interface library
set(CAMERA_IFACE_TARGET camera_iface_ensemble)
add_library(${CAMERA_IFACE_TARGET} INTERFACE)
target_include_directories(${CAMERA_IFACE_TARGET}
    INTERFACE
    include
    source)

# Create static library for Ensemble data
set(CAMERA_ENSEMBLE_COMPONENT_TARGET camera_ensemble)
add_library(${CAMERA_ENSEMBLE_COMPONENT_TARGET} STATIC)

## Component sources
target_sources(${CAMERA_ENSEMBLE_COMPONENT_TARGET}
    PRIVATE
    source/bayer2rgb.c
    source/camera.c
    source/color_correction.c
    source/hal_camera_alif.c
    source/image_processing.c
    source/tiff.c
    )

target_compile_definitions(${CAMERA_ENSEMBLE_COMPONENT_TARGET}
    PUBLIC
    ENSEMBLE_CAMERA_MODULE_${ENSEMBLE_CAMERA_MODULE}=1
    $<$<BOOL:${USE_FAKE_CAMERA}>:USE_FAKE_CAMERA>)

if (CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang")
    # Additional option to enable "full" floating point standard conformance
    # (needed for NaNs and infinity).
    target_compile_options(${CAMERA_ENSEMBLE_COMPONENT_TARGET} PRIVATE -ffp-mode=full)
endif()

## Logging utilities:
if (NOT TARGET log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

## Add dependencies
target_link_libraries(${CAMERA_ENSEMBLE_COMPONENT_TARGET} PUBLIC
    ${CAMERA_IFACE_TARGET}
    hal_camera_interface
    log
    cmsis_device
    platform_drivers_core
    rte_components)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${CAMERA_ENSEMBLE_COMPONENT_TARGET})
message(STATUS "*******************************************************")
