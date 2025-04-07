#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or
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
# Audio interface library                               #
#########################################################
cmake_minimum_required(VERSION 3.21.0)

project(hal_audio
    DESCRIPTION "Audio interface library"
    LANGUAGES   C)

add_library(hal_audio_interface INTERFACE)
target_include_directories(hal_audio_interface INTERFACE include)

option(HAL_AUDIO_LOOP "Loop static data infinitely" OFF)
add_library(hal_audio_static_streams STATIC)
target_sources(hal_audio_static_streams PRIVATE
    source/hal_audio_static.c
    source/hal_audio_static_external.c)
target_include_directories(hal_audio_static_streams PRIVATE source)
target_link_libraries(hal_audio_static_streams PUBLIC hal_audio_interface log)
target_compile_definitions(hal_audio_static_streams PRIVATE
        $<$<BOOL:${HAL_AUDIO_LOOP}>:HAL_AUDIO_LOOP>)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "CMAKE_SYSTEM_PROCESSOR  : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "Library                 :  hal_audio_interface")
message(STATUS "*******************************************************")
