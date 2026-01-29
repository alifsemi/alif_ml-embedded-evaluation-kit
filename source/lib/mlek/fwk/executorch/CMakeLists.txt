#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
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
#                     ExecuTorch wrapper                #
#########################################################
cmake_minimum_required(VERSION 3.21.0)

set(ML_FWK_ET_TARGET   ml_framework_et)
project(${ML_FWK_ET_TARGET}
    DESCRIPTION     "ExecuTorch framework module"
    LANGUAGES       CXX)

if (NOT "ExecuTorch" STREQUAL ${ML_FRAMEWORK})
    message(FATAL_ERROR "${CMAKE_CURRENT_SOURCE_DIR} should not be included for ${ML_FRAMEWORK}")
endif()

# For CMSIS pack generation flow, we don't need to include the ExecuTorch lib
# or link against it.
if (NOT CMSIS_PACK_GEN_FLOW)
    # Include the ExecuTorch build target if the target is
    # not available. An external project including this
    # project can provide its own version of the target.
    if (NOT TARGET meta::executorch)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
        include(executorch)
    endif()
endif()

# Create static library
add_library(${ML_FWK_ET_TARGET} STATIC
    EtModelAllocator.cc
    EtTensor.cc
    EtModel.cc)

target_sources(${ML_FWK_ET_TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/EtPal.cc>)

# Link time library targets:
target_link_libraries(${ML_FWK_ET_TARGET} PUBLIC
    mlek::log                                                   # Logging functions
    mlek::ml_framework_iface                                    # ML framework interface lib
    $<$<NOT:$<BOOL:${CMSIS_PACK_GEN_FLOW}>>:meta::executorch>)  # ExecuTorch libraries

target_compile_definitions(${ML_FWK_ET_TARGET} INTERFACE
    MLEK_FWK_EXECUTORCH=1)

# By default, use 16 MiB of temporary allocation size. Note that
# this may be overridden by the platform definitions.
set(ML_FWK_TMP_MEM_SIZE "0x01000000" CACHE STRING "Temporary memory allocation size")

# If the platform mandates a temporary memory location, enforce it here.
if (DEFINED ML_FWK_TMP_MEM_BASE)
    target_compile_definitions(${ML_FWK_ET_TARGET} PRIVATE
        ML_FWK_TMP_MEM_BASE=${ML_FWK_TMP_MEM_BASE})
endif()

target_compile_definitions(${ML_FWK_ET_TARGET} PRIVATE
    ML_FWK_TMP_MEM_SIZE=${ML_FWK_TMP_MEM_SIZE})

add_library(mlek::${ML_FWK_ET_TARGET} ALIAS ${ML_FWK_ET_TARGET})

# Display status:
message(STATUS "*******************************************************")
message(STATUS "Library: "                ${ML_FWK_ET_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
