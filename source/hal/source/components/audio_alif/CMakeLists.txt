# /* This file was ported to work on Alif Semiconductor devices. */
#
#/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
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
# Audio library                                         #
#########################################################

cmake_minimum_required(VERSION 3.16.3)

project(audio_component
    DESCRIPTION     "Audio acquire support library"
    LANGUAGES       C CXX ASM)

# Add top level interface library
set(AUDIO_IFACE_TARGET audio_iface)
add_library(${AUDIO_IFACE_TARGET} INTERFACE)
target_include_directories(${AUDIO_IFACE_TARGET}
    INTERFACE
    include
    source
    source/alif
    source/alif/include)

# Create static library for Alif data
set(AUDIO_ALIF_COMPONENT_TARGET audio_alif)
add_library(${AUDIO_ALIF_COMPONENT_TARGET} STATIC)

## Component sources
target_sources(${AUDIO_ALIF_COMPONENT_TARGET}
    PRIVATE
    source/alif/audio_alif.c
    source/alif/mic_listener.c)

# Alif TARGET_BOARD needs to be set
target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}
    PRIVATE
    TARGET_BOARD=BOARD_${TARGET_BOARD})

## Logging utilities:
if (NOT TARGET hal_log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

## Add dependencies
target_link_libraries(${AUDIO_ALIF_COMPONENT_TARGET} PUBLIC
    ${AUDIO_IFACE_TARGET}
    hal_log
    cmsis_device
    rte_components)

target_link_libraries(${AUDIO_ALIF_COMPONENT_TARGET} PRIVATE
    ${AUDIO_IFACE_TARGET}
    arm_math
    platform_drivers_core)

if ((TARGET_BOARD STREQUAL "AppKit-e8") OR (TARGET_BOARD STREQUAL "DevKit-e1c") OR (TARGET_BOARD STREQUAL "StartKit-e1c"))
    set(TARGET_MICS "PDM" CACHE STRING "Microphone selection")
    set_property(CACHE TARGET_MICS PROPERTY STRINGS "PDM")
    if (TARGET_BOARD STREQUAL "AppKit-e8")
        # AppKit-e8 has 2 PDM mics (PDM block) and 2 LPPDM mics (LPPDM block):
        #   PDM        - the 2 PDM mics
        #   LPPDM      - the 2 LPPDM mics
        #   LPPDM_PDM  - all 4 mics, captured in parallel as two streams
        set_property(CACHE TARGET_MICS PROPERTY STRINGS "PDM" "LPPDM" "LPPDM_PDM")
    endif()
else()
    set(TARGET_MICS "I2S" CACHE STRING "Microphone selection")
    set_property(CACHE TARGET_MICS PROPERTY STRINGS "I2S" "PDM" "I2S_PDM")
endif()

# When two mic types are selected the PDM channel cache keys are still driven by
# "PDM"; there is no I2S channel selection to make. For LPPDM_PDM the LPPDM
# channels are selected separately further below.
if ((TARGET_MICS STREQUAL "I2S_PDM") OR (TARGET_MICS STREQUAL "LPPDM_PDM"))
    set(TARGET_MICS_CHANNEL_KEY "PDM")
else()
    set(TARGET_MICS_CHANNEL_KEY "${TARGET_MICS}")
endif()

set(USE_MIC   "LR"    CACHE   STRING  "Mono or Stereo selection")
set_property(CACHE  USE_MIC   PROPERTY    STRINGS     "L" "R" "LR")

if(USE_MIC        STREQUAL      "L")
    set(USE_MIC       1)
elseif(USE_MIC    STREQUAL      "R")
    set(USE_MIC       2)
else()
    set(USE_MIC       3)
endif()

if ( (TARGET_BOARD STREQUAL "AppKit-e8") AND ((TARGET_MICS STREQUAL "LPPDM") OR (TARGET_MICS STREQUAL "LPPDM_PDM")) )
    # The LPPDM pin muxing (pins.h) and BOARD_LPPDM_ENABLED (board_defs.h) are
    # evaluated in cmsis_device sources as well, so the define must be visible
    # there, not only in this component.
    if (TARGET cmsis_device)
        target_compile_definitions(cmsis_device PUBLIC USE_APPKIT_LPPDM=1)
        if (TARGET_MICS STREQUAL "LPPDM_PDM")
            # Keep the PDM mic pins muxed to PDM next to the LPPDM mic pins
            target_compile_definitions(cmsis_device PUBLIC USE_APPKIT_LPPDM_PDM=1)
        endif()
    else()
        message(FATAL_ERROR "cmsis_device target is needed for TARGET_MICS=${TARGET_MICS}")
    endif()
endif()

if ( (TARGET_BOARD STREQUAL "AppKit-e8") AND (TARGET_MICS STREQUAL "LPPDM") )
    set(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH     0    CACHE   STRING  "Primary channel as ")
    set(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH   1    CACHE   STRING  "Secondary channel as ")
else()

    set(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH     4    CACHE   STRING  "Primary channel as ")
    set(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH   5    CACHE   STRING  "Secondary channel as ")
endif()

set_property(CACHE  USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH     PROPERTY    STRINGS     0 1 2 3 4 5 6 7)
set_property(CACHE  USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH   PROPERTY    STRINGS     0 1 2 3 4 5 6 7)

if(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH        EQUAL      0)
    set(USE_PRIMARY_CH       0)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH    EQUAL      1)
    set(USE_PRIMARY_CH       1)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH    EQUAL      2)
    set(USE_PRIMARY_CH       2)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH    EQUAL      3)
    set(USE_PRIMARY_CH       3)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH    EQUAL      4)
    set(USE_PRIMARY_CH       4)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH    EQUAL      5)
    set(USE_PRIMARY_CH       5)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH    EQUAL      6)
    set(USE_PRIMARY_CH       6)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH    EQUAL      7)
    set(USE_PRIMARY_CH       7)
else()
    message(WARNING     "USE_${TARGET_MICS_CHANNEL_KEY}_PRIMARY_CH is not proper")
endif()

if(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH        EQUAL      0)
    set(USE_SECONDARY_CH       0)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH    EQUAL      1)
    set(USE_SECONDARY_CH       1)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH    EQUAL      2)
    set(USE_SECONDARY_CH       2)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH    EQUAL      3)
    set(USE_SECONDARY_CH       3)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH    EQUAL      4)
    set(USE_SECONDARY_CH       4)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH    EQUAL      5)
    set(USE_SECONDARY_CH       5)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH    EQUAL      6)
    set(USE_SECONDARY_CH       6)
elseif(USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH    EQUAL      7)
    set(USE_SECONDARY_CH       7)
else()
    message(WARNING     "USE_${TARGET_MICS_CHANNEL_KEY}_SECONDARY_CH is not proper")
endif()

target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}   PRIVATE    USE_MIC=${USE_MIC})

# USE_STEREO: keep the left and right mic of each stream as interleaved stereo
# samples instead of mixing them down to mono (USE_MIC is then not used).
# PUBLIC so that applications know the layout of the captured data.
option(USE_STEREO "Keep microphone streams stereo instead of converting to mono" OFF)
if (USE_STEREO)
    target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET} PUBLIC USE_STEREO=1)
endif()
target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}   PRIVATE    PRIMARY_CHANNEL=${USE_PRIMARY_CH})
target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}   PRIVATE    SECONDARY_CHANNEL=${USE_SECONDARY_CH})

# LPPDM_PDM: the LPPDM block has its own channel selection (the two LPPDM mics
# of AppKit-e8 are on channels 0 and 1), while PRIMARY/SECONDARY_CHANNEL above
# select the channels of the PDM block (channels 4 and 5).
if (TARGET_MICS STREQUAL "LPPDM_PDM")
    set(USE_LPPDM_PRIMARY_CH     0    CACHE   STRING  "LPPDM primary channel as ")
    set(USE_LPPDM_SECONDARY_CH   1    CACHE   STRING  "LPPDM secondary channel as ")
    set_property(CACHE  USE_LPPDM_PRIMARY_CH     PROPERTY    STRINGS     0 1 2 3 4 5 6 7)
    set_property(CACHE  USE_LPPDM_SECONDARY_CH   PROPERTY    STRINGS     0 1 2 3 4 5 6 7)

    foreach(LPPDM_CH_VAR USE_LPPDM_PRIMARY_CH USE_LPPDM_SECONDARY_CH)
        if (NOT "${${LPPDM_CH_VAR}}" MATCHES "^[0-7]$")
            message(WARNING "${LPPDM_CH_VAR} is not proper")
        endif()
    endforeach()

    target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}
        PRIVATE
        LPPDM_PRIMARY_CHANNEL=${USE_LPPDM_PRIMARY_CH}
        LPPDM_SECONDARY_CHANNEL=${USE_LPPDM_SECONDARY_CH})
endif()

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${AUDIO_ALIF_COMPONENT_TARGET})
message(STATUS "Microphone type                        : " ${TARGET_MICS})
message(STATUS "Stereo microphone streams (USE_STEREO) : " ${USE_STEREO})
message(STATUS "*******************************************************")

if (TARGET_MICS STREQUAL "I2S_PDM")
    target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}
        PRIVATE
        USE_I2S_MICS
        USE_PDM_MICS)
elseif (TARGET_MICS STREQUAL "LPPDM_PDM")
    # Two PDM streams: PDM block (Driver_PDM) and LPPDM block (Driver_LPPDM)
    target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}
        PRIVATE
        USE_PDM_MICS
        USE_LPPDM_MICS)
    # Visible to applications (e.g. alif_aec) to select the 4 PDM mic setup
    target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET} PUBLIC LPPDM_PDM=1)
elseif (TARGET_MICS STREQUAL "LPPDM")
    # Single PDM stream; BOARD_LPPDM_ENABLED makes mic_listener use Driver_LPPDM
    target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}
        PRIVATE
        USE_PDM_MICS)
else()
    target_compile_definitions(${AUDIO_ALIF_COMPONENT_TARGET}
            PRIVATE
            USE_${TARGET_MICS}_MICS)
endif()

# Create static library for Data Stubs
set(AUDIO_STUBS_COMPONENT_TARGET audio_stubs)
add_library(${AUDIO_STUBS_COMPONENT_TARGET} STATIC)

## Include directories - private
target_include_directories(${AUDIO_STUBS_COMPONENT_TARGET}
    PRIVATE
    source)

## Component sources
target_sources(${AUDIO_STUBS_COMPONENT_TARGET}
    PRIVATE
    source/audio_stubs/audio_stubs.c)

## Add dependencies
target_link_libraries(${AUDIO_STUBS_COMPONENT_TARGET} PUBLIC
    ${AUDIO_IFACE_TARGET}
    hal_log)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${AUDIO_STUBS_COMPONENT_TARGET})
message(STATUS "*******************************************************")
