#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022-2023, 2025 Arm Limited and/or
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
#  Common utility library used by use case libraries.   #
#  NOTE:                                                #
#  1. This library should not depend on HAL.            #
#  2. This library should depend on ML framework        #
#     interface and not on the framework wrapper        #
#     itself.                                           #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

set(COMMON_UC_UTILS_TARGET common_api)
project(${COMMON_UC_UTILS_TARGET}
    DESCRIPTION     "Common Utilities library"
    LANGUAGES       CXX)

# Create static library
add_library(${COMMON_UC_UTILS_TARGET} STATIC)

## Sources
target_sources(${COMMON_UC_UTILS_TARGET} PRIVATE
    Classifier.cc
    ImageUtils.cc
    MelSpectrogram.cc
    Mfcc.cc
    AudioRenderUtils.cc)

# Link time library targets:
target_link_libraries(${COMMON_UC_UTILS_TARGET} PUBLIC
    mlek::log                 # Logging interface
    mlek::arm_math            # Math functions
    mlek::ml_framework_iface) # ML framework interface

add_library(mlek::common_api ALIAS common_api)

# Display status:
message(STATUS "*******************************************************")
message(STATUS "Library:                " ${COMMON_UC_UTILS_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
