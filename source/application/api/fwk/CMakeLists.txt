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
#                   ML framework abstraction            #
#########################################################
cmake_minimum_required(VERSION 3.21.0)

set(ML_FWK_IFACE_TARGET   ml_framework_iface)
project(${ML_FWK_IFACE_TARGET}
    DESCRIPTION     "ML framework interface library"
    LANGUAGES       CXX)

if (NOT DEFINED ML_FRAMEWORK)
    message(FATAL_ERROR "ML_FRAMEWORK must be defined")
endif ()

# Add interface library
add_library(${ML_FWK_IFACE_TARGET} INTERFACE)
target_include_directories(${ML_FWK_IFACE_TARGET} INTERFACE iface)
add_library(mlek::${ML_FWK_IFACE_TARGET} ALIAS ${ML_FWK_IFACE_TARGET})

if ("ExecuTorch" STREQUAL ${ML_FRAMEWORK})
    message(STATUS "Adding ExecuTorch framework to the build...")
    add_subdirectory(executorch)
    add_library(mlek::ml_framework_impl ALIAS ml_framework_et)
elseif ("TensorFlowLiteMicro" STREQUAL ${ML_FRAMEWORK})
    message(STATUS "Adding TensorFlow Lite Micro framework to the build...")
    add_subdirectory(tflm)
    add_library(mlek::ml_framework_impl ALIAS ml_framework_tflm)
else ()
    message(FATAL_ERROR "Unsupported ML framework '${ML_FRAMEWORK}'.")
endif ()

# Display status:
message(STATUS "*******************************************************")
message(STATUS "Library: "                ${ML_FWK_IFACE_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
