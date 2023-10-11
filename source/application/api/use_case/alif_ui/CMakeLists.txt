# This file was ported to work on Alif Semiconductor Ensemble family of devices.

#  Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

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
#                   ALIF UI API library                 #
#########################################################
cmake_minimum_required(VERSION 3.21.0)

set(ALIF_UI_API_TARGET alif_ui_api)
project(${ALIF_UI_API_TARGET}
        DESCRIPTION     "Alif UI API library"
        LANGUAGES       C CXX)

# Create static library
add_library(${ALIF_UI_API_TARGET} STATIC
        src/lv_paint_utils.c
        src/ScreenLayout.cc
        src/Alif240.c
        src/Alif240_white.c)

target_include_directories(${ALIF_UI_API_TARGET} PUBLIC include)

target_link_libraries(${ALIF_UI_API_TARGET} PUBLIC common_api lvgl)

message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${ALIF_UI_API_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
