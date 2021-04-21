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

##############################################################################
# Helper function to provide user option and corresponding default value
##############################################################################
function(USER_OPTION name description default type)

    if (NOT DEFINED ${name})
        set(${name} ${default} CACHE ${type} ${description})
    endif()

    # if it is a path
    if (${type} STREQUAL PATH)

        # Get the absolute path, relative to the cmake root
        get_filename_component(ABSPATH "${${name}}" ABSOLUTE BASE_DIR ${CMAKE_SOURCE_DIR})

        # check that this is a directory
        if (NOT IS_DIRECTORY ${ABSPATH})
            message(FATAL_ERROR
                "Invalid directory path. Description: ${description}; Path: ${ABSPATH}")
        endif()

        set(${name} ${ABSPATH} CACHE ${type} ${description} FORCE)

    # if this is a file path
    elseif(${type} STREQUAL FILEPATH)

        # Get the absolute path, relative to the cmake root
        get_filename_component(ABSPATH "${${name}}" ABSOLUTE BASE_DIR ${CMAKE_SOURCE_DIR})

        # check that the file exists:
        if (NOT EXISTS ${ABSPATH})
            message(FATAL_ERROR
                "Invalid file path. Description: ${description}; Path: ${ABSPATH}")
        endif()

        set(${name} ${ABSPATH} CACHE ${type} ${description} FORCE)

    endif()

    message(STATUS "User option ${name} is set to ${${name}}")
    LIST(APPEND USER_OPTIONS ${name})
    set(USER_OPTIONS ${USER_OPTIONS} CACHE INTERNAL "")

endfunction()

# Function to get the path type for a variable
# Args:
#   path_var[in]:           path variable for which the cmake path type is requested
#   cmake_path_type[out]:   CMake path type. Set to FILEPATH when it is a file
#                           or PATH when it points to a directory. If the path
#                           is invalid, this remains empty.
function(get_path_type path_var cmake_path_type)
    # Validate path - get absolute value
    get_filename_component(ABSPATH "${path_var}" ABSOLUTE
                           BASE_DIR ${CMAKE_SOURCE_DIR})

    if (DEFINED path_var)
        if (IS_DIRECTORY ${ABSPATH})
            set(${cmake_path_type} PATH PARENT_SCOPE)
            message(STATUS "Variable of PATH type")
        elseif(EXISTS ${ABSPATH})
            set(${cmake_path_type} FILEPATH PARENT_SCOPE)
        else()
            set(${cmake_path_type} "" PARENT_SCOPE)
        endif()
    else()
        set(${cmake_path_type} UNINITIALIZED PARENT_SCOPE)
    endif()

endfunction()

# Function to print all the user options added using the function `USER_OPTION`
function(print_useroptions)
    message(STATUS "--------------------------------------------------------------------------------------------------")
    message(STATUS "Defined build user options:")
    message(STATUS "")
    foreach(opt ${USER_OPTIONS})
        message(STATUS "    ${opt}=${${opt}}")
    endforeach()
    message(STATUS "--------------------------------------------------------------------------------------------------")
endfunction()

function (SUBDIRLIST result curdir)
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        endif()
    endforeach()
    set(${result} ${dirlist} PARENT_SCOPE)
endfunction()

function(to_py_bool cmake_bool py_bool)
    if(${${cmake_bool}})
        set(${py_bool} True PARENT_SCOPE)
    else()
        set(${py_bool} False PARENT_SCOPE)
    endif()
endfunction()

# Function to download a files from the Arm Model Zoo
# Arguments:
#   model_zoo_version: hash of the Arm Model Zoo commit to use
#   file_sub_path: subpath within the model zoo respository
#   download_path: location where this file is to be downloaded (path including filename)
function(download_file_from_modelzoo model_zoo_version file_sub_path download_path)

    set(MODEL_ZOO_REPO      "https://github.com/ARM-software/ML-zoo/raw")

    string(JOIN "/" FILE_URL
        ${MODEL_ZOO_REPO} ${model_zoo_version} ${file_sub_path})

    message(STATUS "Downloading ${FILE_URL} to ${download_path}...")

    file(DOWNLOAD ${FILE_URL} ${download_path}
        STATUS DOWNLOAD_STATE)
    list(GET DOWNLOAD_STATE 0 RET_VAL)

    if(${RET_VAL})
        list(GET DOWNLOAD_STATE 1 RET_MSG)
        message(FATAL_ERROR "Download failed with error code: ${RET_VAL}; "
                            "Error message: ${RET_MSG}")
    endif()

endfunction()
