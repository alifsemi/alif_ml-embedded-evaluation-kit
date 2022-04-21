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
set(SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scripts)

##############################################################################
# This function generates C++ files for images located in the directory it is
# pointed at. NOTE: uses python
##############################################################################
function(generate_images_code input_dir src_out hdr_out img_size)

    # Absolute paths for passing into python script
    get_filename_component(input_dir_abs ${input_dir} ABSOLUTE)
    get_filename_component(src_out_abs ${src_out} ABSOLUTE)
    get_filename_component(hdr_out_abs ${hdr_out} ABSOLUTE)

    message(STATUS "Generating image files from ${input_dir_abs}")
    execute_process(
        COMMAND ${PYTHON} ${SCRIPTS_DIR}/py/gen_rgb_cpp.py
        --image_path ${input_dir_abs}
        --source_folder_path ${src_out_abs}
        --header_folder_path ${hdr_out_abs}
        --image_size ${img_size} ${img_size}
        RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to generate image files.")
    endif ()

endfunction()

##############################################################################
# This function generates C++ files for audio files located in the directory it is
# pointed at. NOTE: uses python
##############################################################################
function(generate_audio_code input_dir src_out hdr_out s_rate_opt mono_opt off_opt duration_opt res_type_opt min_sample_opt)

    # Absolute paths for passing into python script
    get_filename_component(input_dir_abs ${input_dir} ABSOLUTE)
    get_filename_component(src_out_abs ${src_out} ABSOLUTE)
    get_filename_component(hdr_out_abs ${hdr_out} ABSOLUTE)

    to_py_bool(mono_opt mono_opt_py)

    message(STATUS "Generating audio files from ${input_dir_abs}")
    execute_process(
        COMMAND ${PYTHON} ${SCRIPTS_DIR}/py/gen_audio_cpp.py
        --audio_path ${input_dir_abs}
        --source_folder_path ${src_out_abs}
        --header_folder_path ${hdr_out_abs}
        --sampling_rate ${s_rate_opt}
        --mono ${mono_opt_py}
        --offset ${off_opt}
        --duration ${duration_opt}
        --res_type ${res_type_opt}
        --min_samples ${min_sample_opt}
        RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to generate audio files.")
    endif ()

endfunction()

##############################################################################
# This function generates default empty input C++ files for applications with no
# external input. Main use is for the inference runner. NOTE: uses python
##############################################################################
function(generate_default_input_code hdr_out)

    # Absolute paths for passing into python script
    get_filename_component(hdr_out_abs ${hdr_out} ABSOLUTE)

    message(STATUS "Generating default input files")
    execute_process(
            COMMAND ${PYTHON} ${SCRIPTS_DIR}/py/gen_default_input_cpp.py
            --header_folder_path ${hdr_out_abs}
            RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to generate default input .")
    endif ()

endfunction()
##############################################################################
# This function generates C++ files for tflite NN model files.
# @param[in]    MODEL_PATH      path to a tflite file
# @param[in]    DESTINATION     directory in which the output cc must be
#                               placed
# @param[in]    EXPRESSIONS     C++ code expressions to add to the generated file
# @param[in]    NAMESPACE       model name space
# NOTE: Uses python
##############################################################################
function(generate_tflite_code)

    set(multiValueArgs EXPRESSIONS NAMESPACE)
    set(oneValueArgs MODEL_PATH DESTINATION)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Absolute paths for passing into python script
    get_filename_component(ABS_MODEL_PATH ${PARSED_MODEL_PATH} ABSOLUTE)
    get_filename_component(ABS_DESTINATION ${PARSED_DESTINATION} ABSOLUTE)

    if (EXISTS ${ABS_MODEL_PATH})
        message(STATUS "Using ${ABS_MODEL_PATH}")
    else ()
        message(FATAL_ERROR "${ABS_MODEL_PATH} not found!")
    endif ()


    foreach(expression ${PARSED_EXPRESSIONS})
        set(py_arg_exp ${py_arg_exp} --expression=${expression})
    endforeach()

    foreach(name ${PARSED_NAMESPACE})
        set(py_arg_exp ${py_arg_exp} --namespaces=${name})
    endforeach()

    execute_process(
        COMMAND ${PYTHON} ${SCRIPTS_DIR}/py/gen_model_cpp.py
        --tflite_path ${ABS_MODEL_PATH}
        --output_dir ${ABS_DESTINATION} ${py_arg_exp}
        RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to generate model files.")
    endif ()
endfunction()


##############################################################################
# This function generates C++ file for a given labels' text file.
# @param[in]    INPUT          Path to the label text file
# @param[in]    DESTINATION_SRC directory in which the output cc must be
#                               placed
# @param[in]    DESTINATION_HDR directory in which the output h file must be
#                               placed
# @param[in]    OUTPUT_FILENAME    Path to required output file
# @param[in]    NAMESPACE       data name space
# NOTE: Uses python
##############################################################################
function(generate_labels_code)

    set(multiValueArgs NAMESPACE)
    set(oneValueArgs INPUT DESTINATION_SRC DESTINATION_HDR OUTPUT_FILENAME)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Absolute paths for passing into python script
    get_filename_component(input_abs ${PARSED_INPUT} ABSOLUTE)
    get_filename_component(src_out_abs ${PARSED_DESTINATION_SRC} ABSOLUTE)
    get_filename_component(hdr_out_abs ${PARSED_DESTINATION_HDR} ABSOLUTE)

    message(STATUS "Generating labels file from ${PARSED_INPUT}")
    file(REMOVE "${hdr_out_abs}/${PARSED_OUTPUT_FILENAME}.hpp")
    file(REMOVE "${src_out_abs}/${PARSED_OUTPUT_FILENAME}.cc")

    foreach(name ${PARSED_NAMESPACE})
        set(py_arg_exp ${py_arg_exp} --namespaces=${name})
    endforeach()

    message(STATUS "writing to ${hdr_out_abs}/${PARSED_OUTPUT_FILENAME}.hpp and ${src_out_abs}/${PARSED_OUTPUT_FILENAME}.cc")
    execute_process(
        COMMAND ${PYTHON} ${SCRIPTS_DIR}/py/gen_labels_cpp.py
        --labels_file ${input_abs}
        --source_folder_path ${src_out_abs}
        --header_folder_path ${hdr_out_abs}
        --output_file_name ${PARSED_OUTPUT_FILENAME} ${py_arg_exp}
        RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to generate label files.")
    endif ()
endfunction()


##############################################################################
# This function generates C++ data files for test located in the directory it is
# pointed at.
# @param[in]    INPUT_DIR       directory in which are the npy files
# @param[in]    DESTINATION_SRC directory in which the output cc must be
#                               placed
# @param[in]    DESTINATION_HDR directory in which the output h file must be
#                               placed
# @param[in]    NAMESPACE       data name space
# NOTE: Uses python
##############################################################################
function(generate_test_data_code)

    set(multiValueArgs NAMESPACE INPUT_DIR)
    set(oneValueArgs DESTINATION_SRC DESTINATION_HDR)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    list(LENGTH PARSED_INPUT_DIR input_dir_length)

    if (${input_dir_length} GREATER 1)
        set(add_extra_namespace TRUE)
    else()
        set(add_extra_namespace FALSE)
    endif()

    foreach(input_dir ${PARSED_INPUT_DIR})
        unset(py_arg_exp)
        file(GLOB_RECURSE input_data_files
                "${input_dir}/*.npy"
                )
        # no input NPY data found => skip code generation.
        if(NOT input_data_files)
            message(WARNING "No files were found to generate input data: ${input_dir}")
            break()
        endif()

        # Absolute paths for passing into python script
        get_filename_component(input_dir_abs ${input_dir} ABSOLUTE)
        get_filename_component(input_dir_name ${input_dir} NAME)
        get_filename_component(src_out_abs ${PARSED_DESTINATION_SRC} ABSOLUTE)
        get_filename_component(hdr_out_abs ${PARSED_DESTINATION_HDR} ABSOLUTE)

        foreach(name ${PARSED_NAMESPACE})
            set(py_arg_exp ${py_arg_exp} --namespaces=${name})
        endforeach()

        if (${add_extra_namespace})
            set(py_arg_exp ${py_arg_exp} --namespaces=${input_dir_name})
        endif()

        message(STATUS "Generating test ifm and ofm files from ${input_dir_abs}")
        execute_process(
            COMMAND ${PYTHON} ${SCRIPTS_DIR}/py/gen_test_data_cpp.py
            --data_folder_path ${input_dir_abs}
            --source_folder_path ${src_out_abs}
            --header_folder_path ${hdr_out_abs}
            --usecase ${input_dir_name}
            ${py_arg_exp}
            RESULT_VARIABLE return_code
        )
        if (NOT return_code EQUAL "0")
            message(FATAL_ERROR "Failed to generate test data files.")
        endif ()
    endforeach()
endfunction()


##############################################################################
# Function to prepare a python virtual environment for running the functions
# outlined above.
##############################################################################
function(setup_source_generator)

    # If a virtual env has been created in the resources_downloaded directory,
    # use it for source generator. Else, fall back to creating a virtual env
    # in the current build directory.
    if (EXISTS ${RESOURCES_DIR}/env)
        set(DEFAULT_VENV_DIR ${RESOURCES_DIR}/env)
    else()
        set(DEFAULT_VENV_DIR ${CMAKE_BINARY_DIR}/venv)
    endif()

    if (${CMAKE_HOST_WIN32})
        # Windows Python3 is python.exe
        set(PY_EXEC python)
        set(PYTHON ${DEFAULT_VENV_DIR}/Scripts/${PY_EXEC})
    else()
        set(PY_EXEC python3)
        set(PYTHON ${DEFAULT_VENV_DIR}/bin/${PY_EXEC})
    endif()
    set(PYTHON ${PYTHON} PARENT_SCOPE)
    set(PYTHON_VENV ${DEFAULT_VENV_DIR} PARENT_SCOPE)

    if (EXISTS ${PYTHON})
        message(STATUS "Using existing python at ${PYTHON}")
        return()
    endif ()

    message(STATUS "Configuring python environment at ${PYTHON}")

    execute_process(
        COMMAND ${PY_EXEC} -m venv ${DEFAULT_VENV_DIR}
        RESULT_VARIABLE return_code
    )
    if (NOT return_code STREQUAL "0")
        message(FATAL_ERROR "Failed to setup python3 environment. Return code: ${return_code}")
    endif ()

    execute_process(
        COMMAND ${PYTHON} -m pip install --upgrade pip
        RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to upgrade pip")
    endif ()

    execute_process(
        COMMAND ${PYTHON} -m pip install wheel
        RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to install wheel")
    endif ()

    execute_process(
        COMMAND ${PYTHON} -m pip install -r ${SCRIPTS_DIR}/py/requirements.txt
        RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to install requirements")
    endif ()
endfunction()
