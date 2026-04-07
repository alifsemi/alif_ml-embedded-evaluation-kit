#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2021, 2024-2026 Arm Limited and/or its
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

# Specify the ML frameworks the use case supports
set(${use_case}_ML_FRAMEWORK "TensorFlowLiteMicro;ExecuTorch")
if (NOT ${ML_FRAMEWORK} IN_LIST ${use_case}_ML_FRAMEWORK)
    message(STATUS "${use_case} does not support ${ML_FRAMEWORK} framework")
    set(${use_case}_supports_${ML_FRAMEWORK} OFF)
    return()
endif ()

set(${use_case}_supports_${ML_FRAMEWORK} ON)

# Append the API to use for this use case
list(APPEND ${use_case}_API_LIST "img_class")

set_input_file_path_user_option(".bmp" ${use_case})

USER_OPTION(${use_case}_IMAGE_SIZE "Square image size in pixels. Images will be resized to this size."
    224
    STRING)

if (${ML_FRAMEWORK} STREQUAL "TensorFlowLiteMicro")
    set(DEFAULT_ACTIVATION_BUF_SZ  0x00200000)
    set(LABELS_TXT_FILE ${MLEK_ROOT}/resources/${use_case}/labels/labels_mobilenet_v2_1.0_224.txt)

    if (ETHOS_U_NPU_ENABLED)
        set(DEFAULT_MODEL_PATH      ${DEFAULT_MODEL_DIR}/mobilenet_v2_1.0_224_INT8_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
    else()
        set(DEFAULT_MODEL_PATH      ${DEFAULT_MODEL_DIR}/mobilenet_v2_1.0_224_INT8.tflite)
    endif()
elseif(${ML_FRAMEWORK} STREQUAL "ExecuTorch")
    set(LABELS_TXT_FILE
        ${MLEK_ROOT}/resources/${use_case}/labels/labels_mobilenet_v2_1.IMAGENET1K_V2.txt)

    if (ETHOS_U_NPU_ENABLED)
        string(TOLOWER ${ETHOSU_TARGET_NPU_CONFIG} _NPU_CFG_ID)
        set(DEFAULT_MODEL_PATH          ${DEFAULT_MODEL_DIR}/mv2_arm_delegate_${_NPU_CFG_ID}.pte)
        set(DEFAULT_ACTIVATION_BUF_SZ   0x00200000)
    else()
        set(DEFAULT_MODEL_PATH          ${DEFAULT_MODEL_DIR}/mv2_arm_TOSA-1.0+FP.pte)
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

# Generate input files
generate_images_code("${${use_case}_FILE_PATH}"
        ${SAMPLES_GEN_DIR}
        "${${use_case}_IMAGE_SIZE}")

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
