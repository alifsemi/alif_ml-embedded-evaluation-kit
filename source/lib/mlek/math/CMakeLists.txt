#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2022, 2025 Arm Limited and/or its
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

#######################################################
# Math functions for ML pipelines.                    #
#######################################################

project(arm_math
        DESCRIPTION "Collection of the optimised math functions for ML pipelines"
        LANGUAGES C CXX)

add_library(arm_math STATIC)

target_sources(arm_math
        PRIVATE
        PlatformMath.cc)

target_link_libraries(arm_math PRIVATE mlek_log)

if (${CMAKE_CROSSCOMPILING})
    # If CMSIS-DSP target doesn't exist, include the MLEK default
    # configuration. An external project that wants to re-use this
    # module is expected to declare this target before including
    # this project
    if (NOT TARGET arm::cmsis-dsp)
        include(cmsis-dsp)
    endif()
    target_link_libraries(arm_math PUBLIC arm::cmsis-dsp)
endif ()

add_library(mlek::arm_math ALIAS arm_math)

message(STATUS "*******************************************************")
message(STATUS "Library                    : " arm_math)
message(STATUS "CMAKE_SYSTEM_PROCESSOR     : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
