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
#       AUTOMATIC SPEECH RECOGNITION API library        #
#########################################################
cmake_minimum_required(VERSION 3.21.0)

set(ASR_API_TARGET asr_api)
project(${ASR_API_TARGET}
        DESCRIPTION     "Automatic speech recognition use case API library"
        LANGUAGES       C CXX)

# Create static library
add_library(${ASR_API_TARGET} STATIC
        src/Wav2LetterPreprocess.cc
        src/Wav2LetterPostprocess.cc
        src/Wav2LetterMfcc.cc
        src/AsrClassifier.cc
        src/OutputDecode.cc
        src/Wav2LetterModel.cc)

target_include_directories(${ASR_API_TARGET} PUBLIC include)

target_link_libraries(${ASR_API_TARGET} PUBLIC common_api)

message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${ASR_API_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
