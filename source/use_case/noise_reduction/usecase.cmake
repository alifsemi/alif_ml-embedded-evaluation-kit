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
# Append the API to use for this use case
list(APPEND ${use_case}_API_LIST "noise_reduction")

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
    0x00200000
    STRING)

if (ETHOS_U_NPU_ENABLED)
    set(DEFAULT_MODEL_PATH      ${DEFAULT_MODEL_DIR}/rnnoise_INT8_vela_${ETHOS_U_NPU_CONFIG_ID}.tflite)
else()
    set(DEFAULT_MODEL_PATH      ${DEFAULT_MODEL_DIR}/rnnoise_INT8.tflite)
endif()

USER_OPTION(${use_case}_MODEL_TFLITE_PATH "NN models file to be used in the evaluation application. Model files must be in tflite format."
    ${DEFAULT_MODEL_PATH}
    FILEPATH)

USER_OPTION(${use_case}_FILE_PATH "Directory with custom WAV input files, or path to a single WAV file, to use in the evaluation application."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/samples/
    PATH_OR_FILE)

USER_OPTION(${use_case}_AUDIO_RATE "Specify the target sampling rate. Default is 48000."
    48000
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

USER_OPTION(${use_case}_AUDIO_MIN_SAMPLES "Specify the minimum number of samples to use. Default is 512, if the audio is shorter it will be automatically padded."
    512
    STRING)

# Generate input files from audio wav files
generate_audio_code(${${use_case}_FILE_PATH} ${SRC_GEN_DIR} ${INC_GEN_DIR}
    ${${use_case}_AUDIO_RATE}
    ${${use_case}_AUDIO_MONO}
    ${${use_case}_AUDIO_OFFSET}
    ${${use_case}_AUDIO_DURATION}
    ${${use_case}_AUDIO_RES_TYPE}
    ${${use_case}_AUDIO_MIN_SAMPLES})


set(EXTRA_MODEL_CODE
    "/* Model parameters for ${use_case} */"
    "extern const int        g_FrameLength         = 512"
    "extern const int        g_FrameStride         = 512"
    "extern const uint32_t   g_NumInputFeatures    = 42*1"  # Single time-step input of 42 features.
    )

# Generate model file.
generate_tflite_code(
    MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
    DESTINATION ${SRC_GEN_DIR}
    EXPRESSIONS ${EXTRA_MODEL_CODE}
    NAMESPACE   "arm" "app" "rnn")


# For MPS3, allow dumping of output data to memory, based on these parameters:
if (TARGET_PLATFORM STREQUAL mps3)
    USER_OPTION(${use_case}_MEM_DUMP_BASE_ADDR
        "Inference output dump address for ${use_case}"
        0x80000000 # DDR bank 2
        STRING)

    USER_OPTION(${use_case}_MEM_DUMP_LEN
        "Inference output dump buffer size for ${use_case}"
        0x00100000 # 1 MiB
        STRING)

    # Add special compile definitions for this use case files:
    set(${use_case}_COMPILE_DEFS
        "MEM_DUMP_BASE_ADDR=${${use_case}_MEM_DUMP_BASE_ADDR}"
        "MEM_DUMP_LEN=${${use_case}_MEM_DUMP_LEN}")

    file(GLOB_RECURSE SRC_FILES
        "${SRC_USE_CASE}/${use_case}/src/*.cpp"
        "${SRC_USE_CASE}/${use_case}/src/*.cc")

    set_source_files_properties(
        ${SRC_FILES}
        PROPERTIES COMPILE_DEFINITIONS
        "${${use_case}_COMPILE_DEFS}")
endif()
