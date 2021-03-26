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

USER_OPTION(${use_case}_FILE_PATH "Directory with WAV files, or path to a single WAV file, to use in the evaluation application."
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

USER_OPTION(${use_case}_AUDIO_MIN_SAMPLES "Specify the minimum number of samples to use. By default is 16000, if the audio is shorter will be automatically padded."
    16000
    STRING)

# Generate audio .cc files:
generate_audio_code(${${use_case}_FILE_PATH} ${SRC_GEN_DIR} ${INC_GEN_DIR}
        ${${use_case}_AUDIO_RATE}
        ${${use_case}_AUDIO_MONO}
        ${${use_case}_AUDIO_OFFSET}
        ${${use_case}_AUDIO_DURATION}
        ${${use_case}_AUDIO_RES_TYPE}
        ${${use_case}_AUDIO_MIN_SAMPLES})

# Generate kws labels file:
USER_OPTION(${use_case}_LABELS_TXT_FILE_KWS "Labels' txt file for the chosen model."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/labels/ds_cnn_labels.txt
    FILEPATH)

set(${use_case}_LABELS_CPP_FILE_KWS Labels_dscnn)
generate_labels_code(
    INPUT           "${${use_case}_LABELS_TXT_FILE_KWS}"
    DESTINATION_SRC ${SRC_GEN_DIR}
    DESTINATION_HDR ${INC_GEN_DIR}
    OUTPUT_FILENAME "${${use_case}_LABELS_CPP_FILE_KWS}"
    NAMESPACE       "arm" "app" "kws"
)

# Generate asr labels file:
USER_OPTION(${use_case}_LABELS_TXT_FILE_ASR "Labels' txt file for the chosen model."
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/${use_case}/labels/labels_wav2letter.txt
    FILEPATH)

set(${use_case}_LABELS_CPP_FILE_ASR Labels_wav2letter)
generate_labels_code(
    INPUT           "${${use_case}_LABELS_TXT_FILE_ASR}"
    DESTINATION_SRC ${SRC_GEN_DIR}
    DESTINATION_HDR ${INC_GEN_DIR}
    OUTPUT_FILENAME "${${use_case}_LABELS_CPP_FILE_ASR}"
    NAMESPACE       "arm" "app" "asr"
)

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
    0x00200000
    STRING)

USER_OPTION(${use_case}_MODEL_SCORE_THRESHOLD_KWS "Specify the score threshold [0.0, 1.0) that must be applied to the KWS results for a label to be deemed valid."
    0.9
    STRING)

USER_OPTION(${use_case}_MODEL_SCORE_THRESHOLD_ASR "Specify the score threshold [0.0, 1.0) that must be applied to the ASR results for a label to be deemed valid."
    0.5
    STRING)

# If there is no tflite file pointed to
if (NOT DEFINED ${use_case}_MODEL_TFLITE_PATH_KWS)

    set(SUB_USECASE_KWS                 "kws")
    set(MODEL_FILENAME_KWS          ds_cnn_clustered_int8.tflite)
    set(MODEL_RESOURCES_DIR_KWS     ${DOWNLOAD_DEP_DIR}/${use_case})
    file(MAKE_DIRECTORY             ${MODEL_RESOURCES_DIR_KWS})
    set(DEFAULT_MODEL_PATH_KWS      ${MODEL_RESOURCES_DIR_KWS}/${MODEL_FILENAME_KWS})

    # Download the default model
    set(ZOO_COMMON_SUBPATH_KWS      "models/keyword_spotting/ds_cnn_large/tflite_clustered_int8")
    set(ZOO_MODEL_SUBPATH_KWS       "${ZOO_COMMON_SUBPATH_KWS}/${MODEL_FILENAME_KWS}")

    download_file_from_modelzoo(${ZOO_MODEL_SUBPATH_KWS}    ${DEFAULT_MODEL_PATH_KWS})

    if (ETHOS_U55_ENABLED)
        message(STATUS
            "Ethos-U55 is enabled, but the model downloaded is not optimized by vela. "
            "To use Ethos-U55 acceleration, optimise the downloaded model and pass it "
            "as ${use_case}_MODEL_TFLITE_PATH_KWS to the CMake configuration.")
    endif()

    if (${TARGET_PLATFORM} STREQUAL native)

        # Download test vectors
        set(ZOO_TEST_IFM_SUBPATH_KWS    "${ZOO_COMMON_SUBPATH_KWS}/testing_input/input_2/0.npy")
        set(ZOO_TEST_OFM_SUBPATH_KWS    "${ZOO_COMMON_SUBPATH_KWS}/testing_output/Identity/0.npy")

        file(MAKE_DIRECTORY         ${MODEL_RESOURCES_DIR_KWS}/${SUB_USECASE_KWS})
        set(${use_case}_TEST_IFM    ${MODEL_RESOURCES_DIR_KWS}/${SUB_USECASE_KWS}/ifm0.npy CACHE FILEPATH
                                    "Input test vector for ${use_case}-${SUB_USECASE_KWS}")
        set(${use_case}_TEST_OFM    ${MODEL_RESOURCES_DIR_KWS}/${SUB_USECASE_KWS}/ofm0.npy CACHE FILEPATH
                                    "Input test vector for ${use_case}-${SUB_USECASE_KWS}.")

        download_file_from_modelzoo(${ZOO_TEST_IFM_SUBPATH_KWS} ${${use_case}_TEST_IFM})
        download_file_from_modelzoo(${ZOO_TEST_OFM_SUBPATH_KWS} ${${use_case}_TEST_OFM})
        set(TEST_SRC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/tests/src)
        set(TEST_INC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/tests/include)
        file(MAKE_DIRECTORY ${TEST_SRC_GEN_DIR} ${TEST_INC_GEN_DIR})

        generate_test_data_code(
                            INPUT_DIR "${DOWNLOAD_DEP_DIR}/${use_case}/${SUB_USECASE_KWS}"
                            DESTINATION_SRC ${TEST_SRC_GEN_DIR}
                            DESTINATION_HDR ${TEST_INC_GEN_DIR}
                            USECASE ${SUB_USECASE_KWS}
                            NAMESPACE   "arm" "app" ${SUB_USECASE_KWS})
    endif()

else()
    set(DEFAULT_MODEL_PATH_KWS  "N/A")
endif()

set(EXTRA_MODEL_CODE_KWS
    "/* Model parameters for ${use_case} */"
    "extern const uint32_t   g_NumMfcc = 10"
    "extern const uint32_t   g_NumAudioWins = 49"
    "extern const int        g_FrameLength = 640"
    "extern const int        g_FrameStride = 320"
    "extern const float      g_ScoreThreshold = ${${use_case}_MODEL_SCORE_THRESHOLD_KWS}"
    )

# If there is no tflite file pointed to
if (NOT DEFINED ${use_case}_MODEL_TFLITE_PATH_ASR)

    set(SUB_USECASE_ASR             "asr")
    set(MODEL_FILENAME_ASR          wav2letter_int8.tflite)
    set(MODEL_RESOURCES_DIR_ASR     ${DOWNLOAD_DEP_DIR}/${use_case})
    file(MAKE_DIRECTORY             ${MODEL_RESOURCES_DIR_ASR})
    set(DEFAULT_MODEL_PATH_ASR      ${MODEL_RESOURCES_DIR_ASR}/${MODEL_FILENAME_ASR})

    # Download the default model
    set(ZOO_COMMON_SUBPATH_ASR      "models/speech_recognition/wav2letter/tflite_int8")
    set(ZOO_MODEL_SUBPATH_ASR       "${ZOO_COMMON_SUBPATH_ASR}/${MODEL_FILENAME_ASR}")

    download_file_from_modelzoo(${ZOO_MODEL_SUBPATH_ASR}    ${DEFAULT_MODEL_PATH_ASR})

    if (ETHOS_U55_ENABLED)
        message(STATUS
            "Ethos-U55 is enabled, but the model downloaded is not optimized by vela. "
            "To use Ethos-U55 acceleration, optimise the downloaded model and pass it "
            "as ${use_case}_MODEL_TFLITE_PATH to the CMake configuration.")
    endif()

    # If the target platform is native
    if (${TARGET_PLATFORM} STREQUAL native)

        # Download test vectors
        set(ZOO_TEST_IFM_SUBPATH_ASR    "${ZOO_COMMON_SUBPATH_ASR}/testing_input/input_2_int8/0.npy")
        set(ZOO_TEST_OFM_SUBPATH_ASR    "${ZOO_COMMON_SUBPATH_ASR}/testing_output/Identity_int8/0.npy")

        file(MAKE_DIRECTORY             ${MODEL_RESOURCES_DIR_ASR}/${SUB_USECASE_ASR})
        set(${use_case}_TEST_IFM_ASR    ${MODEL_RESOURCES_DIR_ASR}/${SUB_USECASE_ASR}/ifm0.npy CACHE FILEPATH
                                    "Input test vector for ${use_case}-${SUB_USECASE_ASR}")
        set(${use_case}_TEST_OFM_ASR    ${MODEL_RESOURCES_DIR_ASR}/${SUB_USECASE_ASR}/ofm0.npy CACHE FILEPATH
                                    "Input test vector for ${use_case}-${SUB_USECASE_ASR}")

        download_file_from_modelzoo(${ZOO_TEST_IFM_SUBPATH_KWS} ${${use_case}_TEST_IFM_ASR})
        download_file_from_modelzoo(${ZOO_TEST_OFM_SUBPATH_KWS} ${${use_case}_TEST_OFM_ASR})

        set(TEST_SRC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/tests/src)
        set(TEST_INC_GEN_DIR ${CMAKE_BINARY_DIR}/generated/${use_case}/tests/include)
        file(MAKE_DIRECTORY ${TEST_SRC_GEN_DIR} ${TEST_INC_GEN_DIR})

        # Generate test data files to be included in x86 tests
        generate_test_data_code(
                            INPUT_DIR "${DOWNLOAD_DEP_DIR}/${use_case}/${SUB_USECASE_ASR}"
                            DESTINATION_SRC ${TEST_SRC_GEN_DIR}
                            DESTINATION_HDR ${TEST_INC_GEN_DIR}
                            USECASE ${SUB_USECASE_ASR}
                            NAMESPACE   "arm" "app" ${SUB_USECASE_ASR})
    endif()

else()
    set(DEFAULT_MODEL_PATH_ASR  "N/A")
endif()

set(EXTRA_MODEL_CODE_ASR
    "/* Model parameters for ${use_case} */"
    "extern const int   g_FrameLength    = 512"
    "extern const int   g_FrameStride    = 160"
    "extern const int   g_ctxLen         =  98"
    "extern const float g_ScoreThreshold = ${${use_case}_MODEL_SCORE_THRESHOLD_ASR}"
    )

USER_OPTION(${use_case}_MODEL_TFLITE_PATH_KWS "NN models file to be used for KWS in the evaluation application. Model files must be in tflite format."
    ${DEFAULT_MODEL_PATH_KWS}
    FILEPATH
    )

USER_OPTION(${use_case}_MODEL_TFLITE_PATH_ASR "NN models file to be used for ASR in the evaluation application. Model files must be in tflite format."
    ${DEFAULT_MODEL_PATH_ASR}
    FILEPATH
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
