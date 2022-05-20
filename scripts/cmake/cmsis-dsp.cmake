#----------------------------------------------------------------------------
#  Copyright (c) 2021 Arm Limited. All rights reserved.
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

# CMSIS-DSP library CMake helper script.

# Check if CMSIS sources have been defined
if (NOT DEFINED CMSIS_SRC_PATH)
    message(FATAL_ERROR "CMSIS path should be defined for CMSIS-DSP library to be built")
endif()

# 3. Form a list of all the sources we need in CSMS-DSP library
set(CMSIS_DSP_PATH_SUFFIX   "CMSIS/DSP")
set(CMSIS_CORE_PATH_SUFFIX  "CMSIS/Core")
set(CMSIS_DSP_SRC_DIR       "${CMSIS_SRC_PATH}/${CMSIS_DSP_PATH_SUFFIX}/Source")
set(CMSIS_DSP_INC_DIR       "${CMSIS_SRC_PATH}/${CMSIS_DSP_PATH_SUFFIX}/Include")
set(CMSIS_DSP_PRI_INC_DIR   "${CMSIS_SRC_PATH}/${CMSIS_DSP_PATH_SUFFIX}/PrivateInclude")
set(CMSIS_CORE_INC_DIR      "${CMSIS_SRC_PATH}/${CMSIS_CORE_PATH_SUFFIX}/Include")

file(GLOB_RECURSE
    CMSIS_DSP_SRC

    "${CMSIS_DSP_SRC_DIR}/BasicMathFunctions/arm_*.c"
    "${CMSIS_DSP_SRC_DIR}/FastMathFunctions/arm_*.c"
    "${CMSIS_DSP_SRC_DIR}/CommonTables/arm_*.c"
    "${CMSIS_DSP_SRC_DIR}/TransformFunctions/arm_*.c"
    "${CMSIS_DSP_SRC_DIR}/StatisticsFunctions/arm_*.c"

    # Issue with q15 and q31 functions with Arm GNU toolchain, we only
    # need f32 functions.
    "${CMSIS_DSP_SRC_DIR}/ComplexMathFunctions/arm_*f32.c")

# 4. Create static library
set(CMSIS_DSP_TARGET        cmsis-dsp)

add_library(${CMSIS_DSP_TARGET} STATIC ${CMSIS_DSP_SRC})

target_include_directories(${CMSIS_DSP_TARGET} PUBLIC
                           ${CMSIS_DSP_INC_DIR}
                           ${CMSIS_CORE_INC_DIR})
target_include_directories(${CMSIS_DSP_TARGET} PRIVATE
                           ${CMSIS_DSP_PRI_INC_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${CMSIS_DSP_TARGET} PUBLIC -flax-vector-conversions)

    # There is a known issue with -O0 optimisation option that affects
    # FFT functions from CMSIS-DSP when compiling with Arm GNU embedded
    # toolchain version 10.2.1 or 10.3-2021.07
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        message(WARNING "There are known issues with CMSIS-DSP builds using "
                        "MVE extension without optimisation. Forcing -O3 "
                        "optimisation level")
        target_compile_options(${CMSIS_DSP_TARGET} PUBLIC -O3)
    endif()
endif ()

# 5. General compile definitions
target_compile_definitions(${CMSIS_DSP_TARGET} PUBLIC ARM_MATH_LOOPUNROLL)

# 6. Provide the library path for the top level CMake to use:
set(CMSIS_DSP_LIB   "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${CMSIS_DSP_TARGET}.a")
message(STATUS "CMSIS_DSP_LIB set to be generated here: ${CMSIS_DSP_LIB}")

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${CMSIS_DSP_TARGET})
message(STATUS "Build type                             : " ${CMAKE_BUILD_TYPE})
message(STATUS "TARGET_PLATFORM                        : " ${TARGET_PLATFORM})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
