# This file was ported to work on Alif Semiconductor devices.

#  Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2021, 2024-2025 Arm Limited and/or its
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
    set(${use_case}_supports_${ML_FRAMEWORK} OFF)
    return()
endif ()

# Default Conformer model is not supported on all NPUs yet.
# TODO: Remove this check once this limitation is resolved.
if (DEFINED ETHOS_U_NPU_ID)
    if (${ML_FRAMEWORK} STREQUAL "ExecuTorch"
        AND ${ETHOS_U_NPU_ID} STREQUAL "U55"
        AND NOT DEFINED ${use_case}_MODEL_PATH)
        message(STATUS "Arm Ethos-U55 NPU is not supported with Conformer model yet.")
        message(STATUS "Build this example for Arm Ethos-U85 NPU instead.")
        return()
    endif()
endif()

set(${use_case}_supports_${ML_FRAMEWORK} ON)

# Add define to be used in code
add_compile_definitions(ALIF_ASR)

# Append the API to use for this use case
list(APPEND ${use_case}_API_LIST "asr" "alif_ui")

set_input_file_path_user_option(".wav" "asr")

USER_OPTION(${use_case}_MODEL_IN_EXT_FLASH "Run model from external flash"
    ON
    BOOL)

if (${ML_FRAMEWORK} STREQUAL "ExecuTorch")
    USER_OPTION(${use_case}_LABELS_TXT_FILE "Labels' txt file for the chosen model."
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/asr/labels/librispeech_sp.pieces
        FILEPATH)
else ()
    USER_OPTION(${use_case}_LABELS_TXT_FILE "Labels' txt file for the chosen model."
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/asr/labels/labels_wav2letter.txt
        FILEPATH)
endif ()

USER_OPTION(${use_case}_AUDIO_RATE "Specify the target sampling rate. Default is 16000."
    16000
    STRING)

USER_OPTION(${use_case}_AUDIO_MONO "Specify if the audio needs to be converted to mono. Default is ON."
    ON
    BOOL)

USER_OPTION(${use_case}_AUDIO_OFFSET "Specify the offset to start reading after this time (in seconds). Default is 0."
    0
    STRING)

USER_OPTION(${use_case}_AUDIO_DURATION "Specify the audio duration to load (in seconds). If set to 0 the entire audio will be processed."
    0
    STRING)

USER_OPTION(${use_case}_AUDIO_RES_TYPE "Specify re-sampling algorithm to use. By default is 'kaiser_best'."
    kaiser_best
    STRING)

USER_OPTION(${use_case}_AUDIO_MIN_SAMPLES "Specify the minimum number of samples to use. By default is 16000, if the audio is shorter will be automatically padded."
    16000
    STRING)

USER_OPTION(${use_case}_MODEL_SCORE_THRESHOLD "Specify the score threshold [0.0, 1.0) that must be applied to the inference results for a label to be deemed valid."
    0.5
    STRING)

# Generate input files
generate_audio_code(${asr_FILE_PATH} ${SAMPLES_GEN_DIR}
    ${${use_case}_AUDIO_RATE}
    ${${use_case}_AUDIO_MONO}
    ${${use_case}_AUDIO_OFFSET}
    ${${use_case}_AUDIO_DURATION}
    ${${use_case}_AUDIO_RES_TYPE}
    ${${use_case}_AUDIO_MIN_SAMPLES})

# Generate labels file
set(${use_case}_LABELS_CPP_FILE Labels)
generate_labels_code(
    INPUT           "${${use_case}_LABELS_TXT_FILE}"
    DESTINATION_SRC ${SRC_GEN_DIR}
    DESTINATION_HDR ${INC_GEN_DIR}
    OUTPUT_FILENAME "${${use_case}_LABELS_CPP_FILE}"
)

if (${ML_FRAMEWORK} STREQUAL "TensorFlowLiteMicro")
    USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
        0x00108000 STRING) # ~1 MiB

    if (ETHOS_U_NPU_ENABLED)
        set(DEFAULT_MODEL_PATH      ${RESOURCES_PATH}/asr/wav2letter_pruned_int8_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
    else()
        set(DEFAULT_MODEL_PATH      ${RESOURCES_PATH}/asr/wav2letter_pruned_int8.tflite)
    endif()

    set(EXTRA_MODEL_CODE
        "/* Model parameters for ${use_case} */"
        "extern const int   g_FrameLength    = 512"
        "extern const int   g_FrameStride    = 160"
        "extern const int   g_ctxLen         =  98"
        "extern const float g_ScoreThreshold = ${${use_case}_MODEL_SCORE_THRESHOLD}"
        )
elseif(${ML_FRAMEWORK} STREQUAL "ExecuTorch")

    if (ETHOS_U_NPU_ENABLED)
        string(TOLOWER ${ETHOSU_TARGET_NPU_CONFIG} _NPU_CFG_ID)
        set(DEFAULT_MODEL_PATH      ${RESOURCES_PATH}/asr/conformer_arm_delegate_${_NPU_CFG_ID}.pte)
        set(DEFAULT_ACT_BUF_SZ      0x00108000) # ~1 MiB
    else()
        set(DEFAULT_MODEL_PATH      ${RESOURCES_PATH}/asr/conformer_arm_TOSA-1.0+INT.pte)
        set(DEFAULT_ACT_BUF_SZ      0x03000000) # 48 MiB
    endif()

    USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
        ${DEFAULT_ACT_BUF_SZ}
        STRING)

    set(EXTRA_MODEL_CODE
        "/* Model parameters for ${use_case} */"
        "extern const int   g_melSpecWindowSize = 512"
        "extern const int   g_melSpecHopSize    = 160"
        "extern const int   g_chunkSize         = 1500"
        )
endif()

USER_OPTION(${use_case}_MODEL_PATH "NN models file to be used in the evaluation application. Model files must be in tflite or pte format."
    ${DEFAULT_MODEL_PATH}
    FILEPATH
    )

# Generate model file
generate_model_code(
    MODEL_PATH ${${use_case}_MODEL_PATH}
    DESTINATION ${SRC_GEN_DIR}
    EXPRESSIONS ${EXTRA_MODEL_CODE}
    NAMESPACE   "arm" "app" "asr")
