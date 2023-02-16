#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

# Append the API to use for this use case
list(APPEND ${use_case}_API_LIST "ad")

USER_OPTION(${use_case}_MODEL_SCORE_THRESHOLD "Specify the score threshold for a result to be deemed anomalous."
    -0.8
    STRING)
USER_OPTION(${use_case}_MACHINE_GAIN "Specify the gain amount for a specific machine."
    10.0
    STRING)

# Add special compile definitions for this use case files:
set(${use_case}_COMPILE_DEFS
    "MACHINE_GAIN=${${use_case}_MACHINE_GAIN}")

file(GLOB_RECURSE SRC_FILES
    "${SRC_USE_CASE}/alif_ad/src/*.cpp"
    "${SRC_USE_CASE}/alif_ad/src/*.cc")

set_source_files_properties(
    ${SRC_FILES}
    PROPERTIES COMPILE_DEFINITIONS
    "${${use_case}_COMPILE_DEFS}")

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
        0x00200000
        STRING)

if (ETHOS_U_NPU_ENABLED)
    set(DEFAULT_MODEL_PATH      ${RESOURCES_DIR}/ad/ad_medium_int8_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
else()
    set(DEFAULT_MODEL_PATH      ${RESOURCES_DIR}/ad/ad_medium_int8.tflite)
endif()

set(EXTRA_MODEL_CODE
    "/* Model parameters for ${use_case} */"
    "extern const int       g_FrameLength = 1024"
    "extern const int       g_FrameStride = 512"
    "extern const float     g_ScoreThreshold = ${${use_case}_MODEL_SCORE_THRESHOLD}"
    "extern const float     g_TrainingMean = -30"
    )

USER_OPTION(${use_case}_MODEL_TFLITE_PATH "NN models file to be used in the evaluation application. Model files must be in tflite format."
        ${DEFAULT_MODEL_PATH}
        FILEPATH)

# Generate model file
generate_tflite_code(
        MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
        DESTINATION ${SRC_GEN_DIR}
        EXPRESSIONS ${EXTRA_MODEL_CODE}
        NAMESPACE   "arm" "app" "ad")
