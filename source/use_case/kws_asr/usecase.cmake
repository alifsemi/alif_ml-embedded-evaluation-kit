#----------------------------------------------------------------------------
#  Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
# Append the APIs to use for this use case
list(APPEND ${use_case}_API_LIST "kws" "asr")

USER_OPTION(${use_case}_FILE_PATH "Directory with WAV files, or path to a single WAV file, to use in the evaluation application."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/samples/
    PATH_OR_FILE)

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

# Generate kws labels file:
USER_OPTION(${use_case}_LABELS_TXT_FILE_KWS "Labels' txt file for the chosen model."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/labels/micronet_kws_labels.txt
    FILEPATH)

# Generate asr labels file:
USER_OPTION(${use_case}_LABELS_TXT_FILE_ASR "Labels' txt file for the chosen model."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/labels/labels_wav2letter.txt
    FILEPATH)


USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
    0x00200000
    STRING)

USER_OPTION(${use_case}_MODEL_SCORE_THRESHOLD_KWS "Specify the score threshold [0.0, 1.0) that must be applied to the KWS results for a label to be deemed valid."
    0.7
    STRING)

USER_OPTION(${use_case}_MODEL_SCORE_THRESHOLD_ASR "Specify the score threshold [0.0, 1.0) that must be applied to the ASR results for a label to be deemed valid."
    0.5
    STRING)

if (ETHOS_U_NPU_ENABLED)
    set(DEFAULT_MODEL_PATH_KWS      ${DEFAULT_MODEL_DIR}/kws_micronet_m_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
    set(DEFAULT_MODEL_PATH_ASR      ${DEFAULT_MODEL_DIR}/wav2letter_pruned_int8_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
else()
    set(DEFAULT_MODEL_PATH_KWS      ${DEFAULT_MODEL_DIR}/kws_micronet_m.tflite)
    set(DEFAULT_MODEL_PATH_ASR      ${DEFAULT_MODEL_DIR}/wav2letter_pruned_int8.tflite)
endif()

USER_OPTION(${use_case}_MODEL_TFLITE_PATH_KWS "NN models file to be used for KWS in the evaluation application. Model files must be in tflite format."
    ${DEFAULT_MODEL_PATH_KWS}
    FILEPATH
    )

USER_OPTION(${use_case}_MODEL_TFLITE_PATH_ASR "NN models file to be used for ASR in the evaluation application. Model files must be in tflite format."
    ${DEFAULT_MODEL_PATH_ASR}
    FILEPATH
    )

# If the target platform is native
if (${TARGET_PLATFORM} STREQUAL native)
    set(DEFAULT_TEST_DATA_DIR ${DEFAULT_TEST_DATA_DIR}/asr ${DEFAULT_TEST_DATA_DIR}/kws)
endif()

set(EXTRA_MODEL_CODE_KWS
        "/* Model parameters for ${use_case} */"
        "extern const uint32_t   g_NumMfcc = 10"
        "extern const uint32_t   g_NumAudioWins = 49"
        "extern const int        g_FrameLength = 640"
        "extern const int        g_FrameStride = 320"
        "extern const float      g_ScoreThreshold = ${${use_case}_MODEL_SCORE_THRESHOLD_KWS}"
        )

set(EXTRA_MODEL_CODE_ASR
        "/* Model parameters for ${use_case} */"
        "extern const int   g_FrameLength    = 512"
        "extern const int   g_FrameStride    = 160"
        "extern const int   g_ctxLen         =  98"
        "extern const float g_ScoreThreshold = ${${use_case}_MODEL_SCORE_THRESHOLD_ASR}"
        )

# Generate model file for KWS
generate_tflite_code(
    MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH_KWS}
    DESTINATION ${SRC_GEN_DIR}
    EXPRESSIONS ${EXTRA_MODEL_CODE_KWS}
    NAMESPACE   "arm" "app" "kws"
)

# and for ASR
generate_tflite_code(
    MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH_ASR}
    DESTINATION ${SRC_GEN_DIR}
    EXPRESSIONS ${EXTRA_MODEL_CODE_ASR}
    NAMESPACE   "arm" "app" "asr"
)

generate_labels_code(
        INPUT           "${${use_case}_LABELS_TXT_FILE_ASR}"
        DESTINATION_SRC ${SRC_GEN_DIR}
        DESTINATION_HDR ${INC_GEN_DIR}
        OUTPUT_FILENAME "Labels_wav2letter"
        NAMESPACE       "arm" "app" "asr"
)

generate_labels_code(
        INPUT           "${${use_case}_LABELS_TXT_FILE_KWS}"
        DESTINATION_SRC ${SRC_GEN_DIR}
        DESTINATION_HDR ${INC_GEN_DIR}
        OUTPUT_FILENAME "Labels_micronetkws"
        NAMESPACE       "arm" "app" "kws"
)

# Generate audio .cc files:
generate_audio_code(${${use_case}_FILE_PATH} ${SRC_GEN_DIR} ${INC_GEN_DIR}
        ${${use_case}_AUDIO_RATE}
        ${${use_case}_AUDIO_MONO}
        ${${use_case}_AUDIO_OFFSET}
        ${${use_case}_AUDIO_DURATION}
        ${${use_case}_AUDIO_RES_TYPE}
        ${${use_case}_AUDIO_MIN_SAMPLES})
