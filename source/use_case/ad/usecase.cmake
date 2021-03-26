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

# If the path to a directory or source file has been defined,
# get the type here (FILEPATH or PATH):
if (DEFINED ${use_case}_FILE_PATH)
    get_path_type(${${use_case}_FILE_PATH} PATH_TYPE)

    # Set the default type if path is not a dir or file path (or undefined)
    if (NOT ${PATH_TYPE} STREQUAL PATH AND NOT ${PATH_TYPE} STREQUAL FILEPATH)
        message(FATAL_ERROR "Invalid ${use_case}_FILE_PATH. It should be a dir or file path.")
    endif()
else()
    # Default is a directory path
    set(PATH_TYPE PATH)
endif()

message(STATUS "${use_case}_FILE_PATH is of type: ${PATH_TYPE}")

USER_OPTION(${use_case}_FILE_PATH "Directory with custom WAV input files, or path to a single input WAV file, to use in the evaluation application."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/samples/
    ${PATH_TYPE})

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

USER_OPTION(${use_case}_AUDIO_MIN_SAMPLES "Specify the minimum number of samples to use. By default is amount needed to do one inference,
                                           if the audio is shorter then it will be automatically padded."
    33280
    STRING)

USER_OPTION(${use_case}_MODEL_SCORE_THRESHOLD "Specify the score threshold for a result to be deemed anomalous."
    -0.8
    STRING)

generate_audio_code(${${use_case}_FILE_PATH} ${SRC_GEN_DIR} ${INC_GEN_DIR}
        ${${use_case}_AUDIO_RATE}
        ${${use_case}_AUDIO_MONO}
        ${${use_case}_AUDIO_OFFSET}
        ${${use_case}_AUDIO_DURATION}
        ${${use_case}_AUDIO_RES_TYPE}
        ${${use_case}_AUDIO_MIN_SAMPLES})

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
        0x00200000
        STRING)

# If there is no tflite file pointed to
if (NOT DEFINED ${use_case}_MODEL_TFLITE_PATH)

    set(MODEL_RESOURCES_DIR     ${DOWNLOAD_DEP_DIR}/${use_case})
    file(MAKE_DIRECTORY         ${MODEL_RESOURCES_DIR})
    set(MODEL_FILENAME          ad_med_nov11_int8.tflite)
    set(DEFAULT_MODEL_PATH      ${MODEL_RESOURCES_DIR}/${MODEL_FILENAME})

    # TODO: Download the model here for this use case when available on Model Zoo.
    # For now we write a place holder file.
    file(WRITE ${DEFAULT_MODEL_PATH} "Placeholder")
else()
    set(DEFAULT_MODEL_PATH  "N/A")
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
)
