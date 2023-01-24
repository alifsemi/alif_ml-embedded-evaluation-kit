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
# This CMake project just consolidates all APIs to help #
# generate CMSIS-pack from this repository. Projects    #
# that need to be included in the pack, should be added #
# here along with their dependencies.                   #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(ml-embedded-eval-kit-api
        DESCRIPTION     "Platform agnostic API for all examples.")

set(SOURCE_ROOT             ${CMAKE_CURRENT_LIST_DIR}/../../../source)

set(LOGGING_PROJECT_DIR     ${SOURCE_ROOT}/log)
set(ARM_MATH_PROJECT_DIR    ${SOURCE_ROOT}/math)
set(API_COMMON_PROJECT_DIR  ${SOURCE_ROOT}/application/api/common)
set(API_UC_PROJECT_DIR      ${SOURCE_ROOT}/application/api/use_case)

# Add dependencies first:
if (NOT TARGET arm_math)
    add_subdirectory(${ARM_MATH_PROJECT_DIR} ${CMAKE_BINARY_DIR}/math)
endif()

if (NOT TARGET log)
    add_subdirectory(${LOGGING_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

# Add common part and the APIs:
add_subdirectory(${API_COMMON_PROJECT_DIR} ${CMAKE_BINARY_DIR}/common)

file(GLOB UC_API_LIST "${API_UC_PROJECT_DIR}/*")
list(FILTER UC_API_LIST EXCLUDE REGEX ".md") # Remove .md files from UC_API_LIST

foreach(API ${UC_API_LIST})
    add_subdirectory(${API} ${CMAKE_BINARY_DIR}/${API})
endforeach()

# Any custom steps that are required for the CMSIS pack generation flow
# must be declared here:
if (CMSIS_PACK_GEN_FLOW)
    target_include_directories(common_api PUBLIC ${TENSORFLOW_SRC_PATH})
    # For CMSIS packs, we need CMSIS DSP definition.
    # @TODO: Currently, this line is added multiple times to the include file. Uncomment when
    # packgen is fixed.
    # add_compile_definitions(arm_math PRIVATE ARM_MATH_DSP)
endif()
