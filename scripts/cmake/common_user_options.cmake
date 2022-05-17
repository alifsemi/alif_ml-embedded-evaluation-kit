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

#----------------------------------------------------------------------------
# This file should contain all the common user options for the application
# For use case specific options, see individual usecase.cmake files under
# each example use case.
#----------------------------------------------------------------------------

if (NOT DEFINED USER_OPTIONS_INCLUDED)
    set(USER_OPTIONS_INCLUDED ON)
else()
    return()
endif()

message(STATUS "Assessing common user options...")

include(${CMAKE_CURRENT_LIST_DIR}/util_functions.cmake)

USER_OPTION(LOG_LEVEL "Log level for the application"
    LOG_LEVEL_INFO
    STRING)

USER_OPTION(TENSORFLOW_SRC_PATH "Path to the root of the tensor flow directory"
    "${DEPENDENCY_ROOT_DIR}/tensorflow"
    PATH)

USER_OPTION(TARGET_PLATFORM "Target platform to execute evaluation application: mps3, simple_platform, native"
    mps3
    STRING)

USER_OPTION(TARGET_SUBSYSTEM "Specify platform target subsystem: sse-300, sse-310 or none"
    sse-300
    STRING)

if (TARGET_PLATFORM STREQUAL native)
    set(NPU_AVAILABLE OFF)
else()
    set(NPU_AVAILABLE ON)
endif()

USER_OPTION(ETHOS_U_NPU_ENABLED "If Arm Ethos-U NPU is enabled in the target system."
    ${NPU_AVAILABLE}
    BOOL)

USER_OPTION(USE_CASE_BUILD "Optional. Defines the use-case to build from the available sources. By default, all use-cases are built."
    all
    STRING)

USER_OPTION(CPU_PROFILE_ENABLED "Output CPU performance profiling information. Should be used only for MPS3 board."
    OFF
    BOOL)

USER_OPTION(TENSORFLOW_LITE_MICRO_BUILD_TYPE "TensorFlow Lite Mirco build type (release/debug etc.)"
    $<IF:$<CONFIG:RELEASE>,release_with_logs,debug>
    STRING)

USER_OPTION(TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS "Select if TPIP downloads should be cleaned before each build."
    OFF
    BOOL)

USER_OPTION(TENSORFLOW_LITE_MICRO_CLEAN_BUILD "Select if clean target should be added to a list of targets"
    ON
    BOOL)

if (NOT TARGET_PLATFORM STREQUAL native)

    USER_OPTION(CMSIS_SRC_PATH
        "Path to CMSIS-5 sources"
        "${DEPENDENCY_ROOT_DIR}/cmsis"
        PATH)

    # If we need NPU libraries:
    if (ETHOS_U_NPU_ENABLED)
        USER_OPTION(ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH
            "Path to Ethos-U NPU timing adapter sources"
            "${DEPENDENCY_ROOT_DIR}/core-platform/drivers/timing_adapter"
            PATH
            )

        USER_OPTION(ETHOS_U_NPU_DRIVER_SRC_PATH
            "Path to Ethos-U NPU core driver sources"
            "${DEPENDENCY_ROOT_DIR}/core-driver"
            PATH
            )

        USER_OPTION(ETHOS_U_NPU_ID "Arm Ethos-U NPU IP (U55 or U65)"
            "U55"
            STRING)

        if ((ETHOS_U_NPU_ID STREQUAL U55) OR (ETHOS_U_NPU_ID STREQUAL U65))
            if (ETHOS_U_NPU_ID STREQUAL U55)
                set(DEFAULT_NPU_MEM_MODE    "Shared_Sram")
                set(DEFAULT_NPU_CONFIG_ID   "H128")
            elseif(ETHOS_U_NPU_ID STREQUAL U65)
                set(DEFAULT_NPU_MEM_MODE    "Dedicated_Sram")
                set(DEFAULT_NPU_CONFIG_ID   "Y256")
                set(DEFAULT_NPU_CACHE_SIZE  "393216")

                USER_OPTION(ETHOS_U_NPU_CACHE_SIZE "Arm Ethos-U65 NPU Cache Size"
                    "${DEFAULT_NPU_CACHE_SIZE}"
                    STRING)
            endif()
        else ()
            message(FATAL_ERROR "Non compatible Ethos-U NPU processor ${ETHOS_U_NPU_ID}")
        endif ()

        USER_OPTION(ETHOS_U_NPU_MEMORY_MODE "Specifies the memory mode used in the Vela command."
            "${DEFAULT_NPU_MEM_MODE}"
            STRING)

        USER_OPTION(ETHOS_U_NPU_CONFIG_ID "Specifies the configuration ID for the NPU."
            "${DEFAULT_NPU_CONFIG_ID}"
            STRING)

        if (ETHOS_U_NPU_ID STREQUAL U55)
            set(DEFAULT_TA_CONFIG_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}/timing_adapter/ta_config_u55_high_end.cmake")
        else ()
            set(DEFAULT_TA_CONFIG_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}/timing_adapter/ta_config_u65_high_end.cmake")
        endif ()

        USER_OPTION(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED "Specifies if the Ethos-U timing adapter is enabled"
            ON
            BOOL)

        if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
            USER_OPTION(TA_CONFIG_FILE "Path to the timing adapter configuration file"
                    ${DEFAULT_TA_CONFIG_FILE_PATH}
                    FILEPATH)
        endif()
    endif()
endif()
