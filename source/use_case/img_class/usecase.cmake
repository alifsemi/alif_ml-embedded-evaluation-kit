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
list(APPEND ${use_case}_API_LIST "img_class")

set_input_file_path_user_option(".bmp" ${use_case})

USER_OPTION(${use_case}_IMAGE_SIZE "Square image size in pixels. Images will be resized to this size."
    224
    STRING)

USER_OPTION(${use_case}_LABELS_TXT_FILE "Labels' txt file for the chosen model"
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/labels/labels_mobilenet_v2_1.0_224.txt
    FILEPATH)

# Generate input files
generate_images_code("${${use_case}_FILE_PATH}"
                     ${SRC_GEN_DIR}
                     ${INC_GEN_DIR}
                     "${${use_case}_IMAGE_SIZE}")

# Generate labels file
set(${use_case}_LABELS_CPP_FILE Labels)
generate_labels_code(
    INPUT           "${${use_case}_LABELS_TXT_FILE}"
    DESTINATION_SRC ${SRC_GEN_DIR}
    DESTINATION_HDR ${INC_GEN_DIR}
    OUTPUT_FILENAME "${${use_case}_LABELS_CPP_FILE}"
)

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
    0x00200000
    STRING)

if (ETHOS_U_NPU_ENABLED)
    set(DEFAULT_MODEL_PATH      ${DEFAULT_MODEL_DIR}/mobilenet_v2_1.0_224_INT8_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
else()
    set(DEFAULT_MODEL_PATH      ${DEFAULT_MODEL_DIR}/mobilenet_v2_1.0_224_INT8.tflite)
endif()

USER_OPTION(${use_case}_MODEL_TFLITE_PATH "NN models file to be used in the evaluation application. Model files must be in tflite format."
    ${DEFAULT_MODEL_PATH}
    FILEPATH
    )

# Generate model file
generate_tflite_code(
    MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
    DESTINATION ${SRC_GEN_DIR}
    NAMESPACE   "arm" "app" "img_class")
