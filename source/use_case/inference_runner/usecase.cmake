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

USER_OPTION(${use_case}_ACTIVATION_BUF_SZ "Activation buffer size for the chosen model"
    0x00200000
    STRING)

generate_default_input_code(${INC_GEN_DIR})

# If there is no tflite file pointed to
if (NOT DEFINED ${use_case}_MODEL_TFLITE_PATH)

    set(MODEL_RESOURCES_DIR     ${DOWNLOAD_DEP_DIR}/${use_case})
    file(MAKE_DIRECTORY         ${MODEL_RESOURCES_DIR})
    set(MODEL_FILENAME          dnn_s_quantized.tflite)
    set(DEFAULT_MODEL_PATH      ${MODEL_RESOURCES_DIR}/${MODEL_FILENAME})

    # Download the default model
    set(ZOO_COMMON_SUBPATH      "models/keyword_spotting/dnn_small/tflite_int8/")
    set(ZOO_MODEL_SUBPATH       "${ZOO_COMMON_SUBPATH}/${MODEL_FILENAME}")
    set(ZOO_MODEL_VERSION       "68b5fbc77ed28e67b2efc915997ea4477c1d9d5b")

    download_file_from_modelzoo(${ZOO_MODEL_VERSION} ${ZOO_MODEL_SUBPATH} ${DEFAULT_MODEL_PATH})

    if (ETHOS_U55_ENABLED)
        message(STATUS
            "Ethos-U55 is enabled, but the model downloaded is not optimized by vela. "
            "To use Ethos-U55 acceleration, optimise the downloaded model and pass it "
            "as ${use_case}_MODEL_TFLITE_PATH to the CMake configuration.")
    endif()

else()
    set(DEFAULT_MODEL_PATH  "N/A")
endif()

USER_OPTION(${use_case}_MODEL_TFLITE_PATH "NN models file to be used in the evaluation application. Model files must be in tflite format."
    ${DEFAULT_MODEL_PATH}
    FILEPATH)

# Generate model file
generate_tflite_code(
    MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
    DESTINATION ${SRC_GEN_DIR}
)
