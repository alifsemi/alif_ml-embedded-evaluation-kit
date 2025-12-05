# This file was ported to work on Alif Semiconductor devices.

#  Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2021, 2025 Arm Limited and/or its
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

# CMSIS-DSP library CMake helper script.

# Check if CMSIS sources have been defined
if (NOT DEFINED CMSIS_DSP_SRC_PATH)
    message(FATAL_ERROR "CMSIS-DSP path should be defined for CMSIS-DSP library to be built")
endif()
if (NOT DEFINED CMSIS_SRC_PATH)
    message(FATAL_ERROR "CMSIS path should be defined to include CMSIS-CORE")
endif()

# List of config options to configure CMSIS-DSP
set(CMSIS_DSP_SRC_DIR       "${CMSIS_DSP_SRC_PATH}/Source")
set(CMSISCORE               "${CMSIS_SRC_PATH}/CMSIS/Core")

if (CMAKE_SYSTEM_PROCESSOR STREQUAL cortex-m55 OR
    CMAKE_SYSTEM_PROCESSOR STREQUAL cortex-m85 OR
    CMAKE_SYSTEM_PROCESSOR STREQUAL armv8.1-m.main)
    set(HELIUM              ON)
    set(MVEF                ON)
    set(MVEI                ON)
endif()

# There is a known issue with Arm GNU Compiler version 14.
# See https://github.com/ARM-software/CMSIS-DSP/issues/242
# and https://gitlab.arm.com/tooling/gnu-devtools-for-arm/-/issues/4
set(DISABLEFLOAT16          ON)

add_subdirectory(${CMSIS_DSP_SRC_DIR} ${CMAKE_BINARY_DIR}/cmsis-dsp)

# Filter out sources we don't need in this project
if (CMSIS_DSP_MIN_REQ_SRC_LIST)
    get_target_property(_MLEK_CMSIS_DSP_SOURCES CMSISDSP SOURCES)

    # Exclude fixed signed/unsigned, f16 and f64 support. We only use f32
    # functions.
    list(FILTER _MLEK_CMSIS_DSP_SOURCES
         EXCLUDE
         REGEX ".*(f16|[\\u|\\q][0-9]|f64)")

    # Include only the modules we need:
    set(MLEK_CMSIS_DSP_MODULES_REQ
        BasicMathFunctions
        FastMathFunctions
        CommonTables
        TransformFunctions
        StatisticsFunctions
        ComplexMathFunctions)

    string(JOIN "|" _MLEK_CMSIS_DSP_INC_REGEX ${MLEK_CMSIS_DSP_MODULES_REQ})

    list(FILTER _MLEK_CMSIS_DSP_SOURCES
         INCLUDE
         REGEX ".*(${_MLEK_CMSIS_DSP_INC_REGEX})")

    set_target_properties(CMSISDSP PROPERTIES
        SOURCES "${_MLEK_CMSIS_DSP_SOURCES}")

    message(DEBUG "Required CMSIS DSP sources: ${_MLEK_CMSIS_DSP_SOURCES}")
endif()

# Enable fast-math option
target_compile_options(CMSISDSP PRIVATE -ffast-math)

# Create alias to use for linking with other libs.
add_library(arm::cmsis-dsp ALIAS CMSISDSP)
