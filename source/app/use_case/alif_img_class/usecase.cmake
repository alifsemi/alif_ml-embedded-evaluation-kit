# This file was ported to work on Alif Semiconductor devices.

#  Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

#----------------------------------------------------------------------------
#  Copyright (c) 2021 Arm Limited. All rights reserved.
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

# Specify the ML frameworks the use case supports
set(${use_case}_ML_FRAMEWORK "TensorFlowLiteMicro;ExecuTorch")
if (NOT ${ML_FRAMEWORK} IN_LIST ${use_case}_ML_FRAMEWORK)
    message(STATUS "${use_case} does not support ${ML_FRAMEWORK} framework")
    set(${use_case}_supports_${ML_FRAMEWORK} OFF)
    return()
endif ()

set(${use_case}_supports_${ML_FRAMEWORK} ON)

# Append the API to use for this use case
list(APPEND ${use_case}_API_LIST "img_class" "alif_ui")

if (${ML_FRAMEWORK} STREQUAL "TensorFlowLiteMicro")
    set(DEFAULT_ACTIVATION_BUF_SZ  0x00200000)
    set(LABELS_TXT_FILE ${MLEK_ROOT}/resources/img_class/labels/labels_mobilenet_v2_1.0_224.txt)

    if (ETHOS_U_NPU_ENABLED)
        set(DEFAULT_MODEL_PATH      ${RESOURCES_PATH}/img_class/mobilenet_v2_1.0_224_INT8_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
    else()
        set(DEFAULT_MODEL_PATH      ${RESOURCES_PATH}/img_class/mobilenet_v2_1.0_224_INT8.tflite)
    endif()

elseif(${ML_FRAMEWORK} STREQUAL "ExecuTorch")
    set(LABELS_TXT_FILE ${MLEK_ROOT}/resources/img_class/labels/labels_mobilenet_v2_1.IMAGENET1K_V2.txt)

    if (ETHOS_U_NPU_ENABLED)
        string(TOLOWER ${ETHOSU_TARGET_NPU_CONFIG} _NPU_CFG_ID)
        set(DEFAULT_MODEL_PATH          ${RESOURCES_PATH}/img_class/mv2_arm_delegate_${_NPU_CFG_ID}.pte)
        set(DEFAULT_ACTIVATION_BUF_SZ   0x00200000)
    else()
        set(DEFAULT_MODEL_PATH          ${RESOURCES_PATH}/img_class/mv2_arm_TOSA-1.0+INT.pte)
        set(DEFAULT_ACTIVATION_BUF_SZ   0x00C00000)
    endif()

    USER_OPTION(${use_case}_NORM_MEAN "Model specific per-channel array of normalisation mean values."
        "0.485, 0.456, 0.406"
        STRING)

    USER_OPTION(${use_case}_NORM_STD  "Model specific per-channel array of normalisation std values."
        "0.229, 0.224, 0.225"
        STRING)

    set(EXTRA_MODEL_CODE
        "/* Model parameters for ${use_case} */"
        "extern const unsigned int g_numChannels = 3"
        "extern const float g_normMean[]   = { ${${use_case}_NORM_MEAN} };"
        "extern const float g_normStddev[] = { ${${use_case}_NORM_STD} };"
    )
endif()

USER_OPTION(${use_case}_MODEL_PATH "Neural network model file to be used in the evaluation application."
        ${DEFAULT_MODEL_PATH}
        FILEPATH)

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
        ${DEFAULT_ACTIVATION_BUF_SZ}
        STRING)


USER_OPTION(${use_case}_LABELS_TXT_FILE "Labels' txt file for the chosen model"
        ${LABELS_TXT_FILE}
        FILEPATH)

USER_OPTION(${use_case}_SKIP_MODEL "Skip ML model - UI only"
    OFF
    BOOL)

USER_OPTION(${use_case}_SHOW_INF_TIME "Show inference time"
    OFF
    BOOL)

USER_OPTION(${use_case}_SHOW_PROFILING "Show CPU profiling"
    OFF
    BOOL)

USER_OPTION(${use_case}_SHOW_EXPOSURE "Show camera exposure stats"
    OFF
    BOOL)

set(${use_case}_COMPILE_DEFS
    SHOW_PROFILING=$<BOOL:${${use_case}_SHOW_PROFILING}>
    SHOW_EXPOSURE=$<BOOL:${${use_case}_SHOW_EXPOSURE}>
    SHOW_INF_TIME=$<BOOL:${${use_case}_SHOW_INF_TIME}>
    SKIP_MODEL=$<BOOL:${${use_case}_SKIP_MODEL}>
)

# Generate labels file
set(${use_case}_LABELS_CPP_FILE Labels)
generate_labels_code(
    INPUT           "${${use_case}_LABELS_TXT_FILE}"
    DESTINATION_SRC ${SRC_GEN_DIR}
    DESTINATION_HDR ${INC_GEN_DIR}
    OUTPUT_FILENAME "${${use_case}_LABELS_CPP_FILE}"
)

if (COMMAND generate_pte_ops_lib)
    generate_pte_ops_lib(
        MODEL_PATH      "${${use_case}_MODEL_PATH}"     # Path to the model PTE
        LIB_NAME        "${use_case}_portable_ops_lib"  # Library target name
        SELECT_OPS_LIST "")                             # Always included ops list

    # If the target is generated, request it to be linked for this use case.
    if (TARGET ${use_case}_portable_ops_lib)
        set(${use_case}_LINK_LIBS ${use_case}_portable_ops_lib)
    endif()
endif()

# Generate model file
generate_model_code(
    MODEL_PATH ${${use_case}_MODEL_PATH}
    DESTINATION ${SRC_GEN_DIR}
    EXPRESSIONS ${EXTRA_MODEL_CODE}
    NAMESPACE   "arm" "app" "img_class")

# Rest are for static images use case
set_input_file_path_user_option(".bmp" "img_class")

USER_OPTION(${use_case}_IMAGE_SIZE "Square image size in pixels. Images will be resized to this size."
    224
    STRING)

# Generate input files
generate_images_code("${img_class_FILE_PATH}"
                    ${SAMPLES_GEN_DIR}
                    "${${use_case}_IMAGE_SIZE}")
