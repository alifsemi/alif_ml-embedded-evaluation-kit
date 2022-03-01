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

##############################################################################
# Helper function to provide user option and corresponding default value
##############################################################################
function(USER_OPTION name description default type)

    if (${type} STREQUAL PATH_OR_FILE)

        if (DEFINED ${name})
            get_filename_component(ABSPATH "${${name}}" ABSOLUTE BASE_DIR ${CMAKE_SOURCE_DIR})

            get_path_type(${${name}} PATH_TYPE)
        else()
            get_filename_component(ABSPATH "${default}" ABSOLUTE BASE_DIR ${CMAKE_SOURCE_DIR})

            get_path_type(${default} PATH_TYPE)
        endif()

        if (NOT EXISTS ${ABSPATH})
            message(FATAL_ERROR
                    "Invalid directory or file path. Description: ${description}; Path: ${ABSPATH}")
        endif()

        # Set the default type if path is not a dir or file path (or undefined)
        if (NOT ${PATH_TYPE} STREQUAL PATH AND NOT ${PATH_TYPE} STREQUAL FILEPATH)
            message(FATAL_ERROR "Invalid ${name}. It should be a dir or file path.")
        endif()
        set(type ${PATH_TYPE})
    endif()

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


# Function to check if a variable is defined, and throw
# an error if it is not.
function(assert_defined var_name)
    if (NOT DEFINED ${var_name})
        message(FATAL_ERROR "ERROR: ${var_name} is undefined!")
    endif()
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
#   file_sub_path: subpath within the model zoo repository
#   download_path: location where this file is to be downloaded (path including filename)
function(download_file_from_modelzoo model_zoo_version file_sub_path download_path)

    set(MODEL_ZOO_REPO "https://github.com/ARM-software/ML-zoo/raw")

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

function(add_platform_build_configuration)

    set(oneValueArgs TARGET_PLATFORM)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "" ${ARGN} )
    message(STATUS "Searching for ${PARSED_TARGET_PLATFORM} build configuration")
    list(APPEND PLATFORM_BUILD_CONFIG_DIRS ${CMAKE_SCRIPTS_DIR}/platforms)

    FIND_PATH(PLATFORM_BUILD_CONFIG
            NAMES build_configuration.cmake
            PATH_SUFFIXES ${PARSED_TARGET_PLATFORM}
            PATHS ${PLATFORM_BUILD_CONFIG_DIRS}
            )

    message(STATUS "Found build configuration: ${PLATFORM_BUILD_CONFIG}")
    include(${PLATFORM_BUILD_CONFIG}/build_configuration.cmake)
endfunction()

function(check_update_public_resources resource_downloaded_dir)

    string(JOIN "/" FILE_URL ${resource_downloaded_dir})
    execute_process(
            COMMAND python3 ${SCRIPTS_DIR}/py/check_update_resources_downloaded.py
            --resource_downloaded_dir ${resource_downloaded_dir}
            --setup_script_path ${SCRIPTS_DIR}/../set_up_default_resources.py
            RESULT_VARIABLE return_code
    )
    if (NOT return_code EQUAL "0")
        if (NOT return_code EQUAL "1")
            # Return code equal to 2 or else means an error in the resources_downloaded folder
            message(FATAL_ERROR "Resources downloaded error, please run: set_up_default_resources.py")
        else()
            # Return code equal to 1 means that resources_downloaded need to be updated
            message(FATAL_ERROR "Resources downloaded need to be updated, please run: set_up_default_resources.py --clean")
        endif()
    endif ()

endfunction()
