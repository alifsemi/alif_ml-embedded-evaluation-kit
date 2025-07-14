#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
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

# Include the ExecuTorch build target
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(executorch)

# Create static library
add_library(${ML_FWK_ET_TARGET} STATIC
    source/EtModelAllocator.cc
    source/EtTensor.cc
    source/EtModel.cc
)

# Include directories - public
target_include_directories(${ML_FWK_ET_TARGET} PUBLIC
    include)

# Link time library targets:
target_link_libraries(${ML_FWK_ET_TARGET} PUBLIC
    mlek_log                        # Logging functions
    ml-framework-iface              # ML framework interface lib
    ${MLEK_EXECUTORCH_LINK_STR})    # ExecuTorch libraries

target_compile_definitions(${ML_FWK_ET_TARGET} INTERFACE MLEK_FWK_EXECUTORCH=1)

# Display status:
message(STATUS "*******************************************************")
message(STATUS "Library: "                ${ML_FWK_ET_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
