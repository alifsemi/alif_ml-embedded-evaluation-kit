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
USER_OPTION(${use_case}_FILE_PATH "Directory with custom WAV input files, or path to a single WAV file, to use in the evaluation application."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/samples/
    ${PATH_TYPE})

USER_OPTION(${use_case}_LABELS_TXT_FILE "Labels' txt file for the chosen model."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/labels/ds_cnn_labels.txt
    FILEPATH)

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
    0.9
    STRING)

# Generate input files
generate_audio_code(${${use_case}_FILE_PATH} ${SRC_GEN_DIR} ${INC_GEN_DIR}
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

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
    0x00100000
    STRING)

# If there is no tflite file pointed to
if (NOT DEFINED ${use_case}_MODEL_TFLITE_PATH)

    set(MODEL_FILENAME          ds_cnn_clustered_int8.tflite)
    set(MODEL_RESOURCES_DIR     ${DOWNLOAD_DEP_DIR}/${use_case})
    file(MAKE_DIRECTORY         ${MODEL_RESOURCES_DIR})
    set(DEFAULT_MODEL_PATH      ${MODEL_RESOURCES_DIR}/${MODEL_FILENAME})

    # Download the default model
    set(ZOO_COMMON_SUBPATH      "models/keyword_spotting/ds_cnn_large/tflite_clustered_int8")
    set(ZOO_MODEL_SUBPATH       "${ZOO_COMMON_SUBPATH}/${MODEL_FILENAME}")
    set(ZOO_MODEL_VERSION       "68b5fbc77ed28e67b2efc915997ea4477c1d9d5b")

    download_file_from_modelzoo(${ZOO_MODEL_VERSION} ${ZOO_MODEL_SUBPATH} ${DEFAULT_MODEL_PATH})

    if (ETHOS_U55_ENABLED)
        message(STATUS
            "Ethos-U55 is enabled, but the model downloaded is not optimized by vela. "
            "To use Ethos-U55 acceleration, optimise the downloaded model and pass it "
            "as ${use_case}_MODEL_TFLITE_PATH to the CMake configuration.")
    endif()

    # If the target platform is native
    if (${TARGET_PLATFORM} STREQUAL native)

        # Download test vectors
        set(ZOO_TEST_IFM_SUBPATH    "${ZOO_COMMON_SUBPATH}/testing_input/input_2/0.npy")
        set(ZOO_TEST_OFM_SUBPATH    "${ZOO_COMMON_SUBPATH}/testing_output/Identity/0.npy")

        set(${use_case}_TEST_IFM    ${MODEL_RESOURCES_DIR}/ifm0.npy CACHE FILEPATH
                                    "Input test vector for ${use_case}")
        set(${use_case}_TEST_OFM    ${MODEL_RESOURCES_DIR}/ofm0.npy CACHE FILEPATH
                                    "Input test vector for ${use_case}")

        download_file_from_modelzoo(${ZOO_MODEL_VERSION} ${ZOO_TEST_IFM_SUBPATH} ${${use_case}_TEST_IFM})
        download_file_from_modelzoo(${ZOO_MODEL_VERSION} ${ZOO_TEST_OFM_SUBPATH} ${${use_case}_TEST_OFM})

        set(TEST_SRC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/tests/src)
        set(TEST_INC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/tests/include)
        file(MAKE_DIRECTORY ${TEST_SRC_GEN_DIR} ${TEST_INC_GEN_DIR})

        # Generate test data files to be included in x86 tests
        generate_test_data_code(
                            INPUT_DIR "${DOWNLOAD_DEP_DIR}/${use_case}"
                            DESTINATION_SRC ${TEST_SRC_GEN_DIR}
                            DESTINATION_HDR ${TEST_INC_GEN_DIR}
                            USECASE  "${use_case}")
    endif()

else()
    set(DEFAULT_MODEL_PATH  "N/A")
endif()

set(EXTRA_MODEL_CODE
    "/* Model parameters for ${use_case} */"
    "extern const int   g_FrameLength    = 640"
    "extern const int   g_FrameStride    = 320"
    "extern const float g_ScoreThreshold = ${${use_case}_MODEL_SCORE_THRESHOLD}"
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
