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
# Append the API to use for this use case
list(APPEND ${use_case}_API_LIST "object_detection" "alif_ui")

USER_OPTION(${use_case}_MODEL_IN_EXT_FLASH "Run model from external flash"
    OFF
    BOOL)

USER_OPTION(${use_case}_IMAGE_SIZE "Square image size in pixels. Images will be resized to this size."
    192
    STRING)

USER_OPTION(${use_case}_ANCHOR_1 "First anchor array estimated during training."
    "{38, 77, 47, 97, 61, 126}"
    STRING)

USER_OPTION(${use_case}_ANCHOR_2 "Second anchor array estimated during training."
    "{14, 26, 19, 37, 28, 55 }"
    STRING)

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
    0x00082000
    STRING)

USER_OPTION(${use_case}_SHOW_INF_TIME "Show inference time"
    OFF
    BOOL)

set(${use_case}_COMPILE_DEFS
    SHOW_INF_TIME=$<BOOL:${${use_case}_SHOW_INF_TIME}>
)

if (ETHOS_U_NPU_ENABLED)
    set(DEFAULT_MODEL_PATH      ${RESOURCES_DIR}/object_detection/yolo-fastest_192_face_v4_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
else()
    set(DEFAULT_MODEL_PATH      ${RESOURCES_DIR}/object_detection/yolo-fastest_192_face_v4.tflite)
endif()

set(${use_case}_ORIGINAL_IMAGE_SIZE
    ${${use_case}_IMAGE_SIZE}
    CACHE STRING
    "Original image size - for the post processing step to upscale the box co-ordinates.")

set(EXTRA_MODEL_CODE
    "extern const int originalImageSize = ${${use_case}_ORIGINAL_IMAGE_SIZE};"
    "/* NOTE: anchors are different for any given input model size, estimated during training phase */"
    "extern const float anchor1[] = ${${use_case}_ANCHOR_1};"
    "extern const float anchor2[] = ${${use_case}_ANCHOR_2};"
    )

USER_OPTION(${use_case}_MODEL_TFLITE_PATH "NN models file to be used in the evaluation application. Model files must be in tflite format."
    ${DEFAULT_MODEL_PATH}
    FILEPATH
    )

# Generate model file
generate_tflite_code(
    MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
    DESTINATION ${SRC_GEN_DIR}
    EXPRESSIONS ${EXTRA_MODEL_CODE}
    NAMESPACE   "arm" "app" "object_detection")
