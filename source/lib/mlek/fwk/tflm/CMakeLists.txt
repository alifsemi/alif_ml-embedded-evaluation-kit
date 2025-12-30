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
#               TensorFlow Lite Micro wrapper           #
#########################################################
cmake_minimum_required(VERSION 3.21.0)

set(ML_FWK_TFLM_TARGET   ml_framework_tflm)
project(${ML_FWK_TFLM_TARGET}
    DESCRIPTION     "TensorFlow Lite Micro framework module"
    LANGUAGES       C CXX)

if (NOT "TensorFlowLiteMicro" STREQUAL ${ML_FRAMEWORK})
    message(FATAL_ERROR "${CMAKE_CURRENT_SOURCE_DIR} should not be included for ${ML_FRAMEWORK}")
endif()

# For CMSIS pack generation flow, we don't need to include TensorFlow Lite
# Micro lib or link against it.
if (NOT CMSIS_PACK_GEN_FLOW)
    # Include the TensorFlow Lite Micro build target
    # if the target is not available. An external project
    # including this project can provide its own version
    # of the target.
    if (NOT TARGET google::tensorflow-lite-micro)
        set(TFLM_EXTERNAL_TARGET OFF)
        include(tensorflow_lite_micro)
    else()
        set(TFLM_EXTERNAL_TARGET ON)
    endif()
endif ()

# Create static library
add_library(${ML_FWK_TFLM_TARGET} STATIC
    YoloFastestModel.cc
    AdModel.cc
    MicroNetKwsModel.cc
    Wav2LetterModel.cc
    VisualWakeWordModel.cc
    TflmTensor.cc
    TensorFlowLiteMicro.cc
    RNNoiseModel.cc
    MobileNetModel.cc
    TestModel.cc
    TflmModel.cc
    MicroMutableAllOpsResolver.cc
)

# Link time library targets:
target_link_libraries(${ML_FWK_TFLM_TARGET} PUBLIC
    mlek::log                                                               # Logging functions
    mlek::ml_framework_iface                                                # ML framework interface
    $<$<NOT:$<BOOL:${CMSIS_PACK_GEN_FLOW}>>:google::tensorflow-lite-micro>) # TensorFlow Lite Micro library

target_compile_definitions(${ML_FWK_TFLM_TARGET} INTERFACE MLEK_FWK_TFLM=1)

# Set the source file property to indicate that it should not use the logging
# facility from the Cortex-M CPU generic target unless it is an internally
# configured target. Reason: for external target, we can't assume that it
# compiles the sources required.
if (TFLM_EXTERNAL_TARGET)
    set_source_files_properties(TensorFlowLiteMicro.cc
        PROPERTIES COMPILE_DEFINITIONS TFLM_EXTERNAL_TARGET)
endif()

add_library(mlek::${ML_FWK_TFLM_TARGET} ALIAS ${ML_FWK_TFLM_TARGET})

# Display status:
message(STATUS "*******************************************************")
message(STATUS "Library: "                ${ML_FWK_TFLM_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
