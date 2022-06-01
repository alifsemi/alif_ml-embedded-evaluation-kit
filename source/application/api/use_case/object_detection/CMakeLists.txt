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
#             OBJECT DETECTION API library              #
#########################################################
cmake_minimum_required(VERSION 3.21.0)

set(OBJECT_DETECTION_API_TARGET object_detection_api)
project(${OBJECT_DETECTION_API_TARGET}
        DESCRIPTION     "Object detection use case API library"
        LANGUAGES       C CXX)

# Create static library
add_library(${OBJECT_DETECTION_API_TARGET} STATIC
        src/DetectorPreProcessing.cc
        src/DetectorPostProcessing.cc
        src/YoloFastestModel.cc)

target_include_directories(${OBJECT_DETECTION_API_TARGET} PUBLIC include)

target_link_libraries(${OBJECT_DETECTION_API_TARGET} PUBLIC common_api)

message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${OBJECT_DETECTION_API_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")