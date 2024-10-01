#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
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
# NPU configuration user options
#----------------------------------------------------------------------------
include_guard()

include(util_functions)

USER_OPTION(ETHOS_U_NPU_ENABLED "If Arm Ethos-U NPU is enabled in the target system."
    ON
    BOOL)

if (NOT ETHOS_U_NPU_ENABLED)
    return()
endif()

message(STATUS "Assessing NPU configuration options...")

USER_OPTION(ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH
    "Path to Ethos-U NPU timing adapter sources"
    "${MLEK_DEPENDENCY_ROOT_DIR}/core-platform/drivers/timing_adapter"
    PATH
)

USER_OPTION(ETHOS_U_NPU_DRIVER_SRC_PATH
    "Path to Ethos-U NPU core driver sources"
    "${MLEK_DEPENDENCY_ROOT_DIR}/core-driver"
    PATH
)

USER_OPTION(ETHOS_U_NPU_ID "Arm Ethos-U NPU IP (U55 or U65)"
    "U55"
    STRING)

if (ETHOS_U_NPU_ID STREQUAL U55)
    set(DEFAULT_NPU_MEM_MODE    "Shared_Sram")
    set(DEFAULT_NPU_CONFIG_ID   "H128")
elseif (ETHOS_U_NPU_ID STREQUAL U65)
    set(DEFAULT_NPU_MEM_MODE    "Dedicated_Sram")
    set(DEFAULT_NPU_CONFIG_ID   "Y256")
elseif (ETHOS_U_NPU_ID STREQUAL U85)
    set(DEFAULT_NPU_MEM_MODE    "Dedicated_Sram")
    set(DEFAULT_NPU_CONFIG_ID   "Z256")
else()
    message(FATAL_ERROR "Non compatible Ethos-U NPU processor ${ETHOS_U_NPU_ID}")
endif()

if(DEFAULT_NPU_MEM_MODE STREQUAL "Dedicated_Sram")
    set(DEFAULT_NPU_CACHE_SIZE  "393216")
    USER_OPTION(ETHOS_U_NPU_CACHE_SIZE "Arm Ethos-U NPU Cache Size"
            "${DEFAULT_NPU_CACHE_SIZE}"
            STRING)
endif ()

USER_OPTION(ETHOS_U_NPU_MEMORY_MODE "Specifies the memory mode used in the Vela command."
    "${DEFAULT_NPU_MEM_MODE}"
    STRING)

USER_OPTION(ETHOS_U_NPU_CONFIG_ID "Specifies the configuration ID for the NPU."
    "${DEFAULT_NPU_CONFIG_ID}"
    STRING)

USER_OPTION(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED "Specifies if the Ethos-U timing adapter is enabled"
    ON
    BOOL)

if (ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
    if (${ETHOS_U_NPU_ID} STREQUAL "U55")
        set(DEFAULT_TA_CONFIG_FILE  "ta_config_u55_high_end")
    elseif (${ETHOS_U_NPU_ID} STREQUAL "U65")
        set(DEFAULT_TA_CONFIG_FILE  "ta_config_u65_high_end")
    elseif (${ETHOS_U_NPU_ID} STREQUAL "U85")
        set(U85_TA_LOW  "Z128" "Z256")
        set(U85_TA_MID  "Z512" "Z1024")
        set(U85_TA_HIGH "Z2048")

        if(${ETHOS_U_NPU_CONFIG_ID} IN_LIST U85_TA_LOW)
            set(DEFAULT_TA_CONFIG_FILE  "ta_config_u85_sys_dram_low")
        elseif (${ETHOS_U_NPU_CONFIG_ID} IN_LIST U85_TA_MID)
            set(DEFAULT_TA_CONFIG_FILE  "ta_config_u85_sys_dram_mid")
        elseif (${ETHOS_U_NPU_CONFIG_ID} IN_LIST U85_TA_HIGH)
            set(DEFAULT_TA_CONFIG_FILE  "ta_config_u85_sys_dram_high")
        endif ()
    endif ()

    USER_OPTION(TA_CONFIG_FILE "Path to the timing adapter configuration file"
        ${DEFAULT_TA_CONFIG_FILE}
        STRING)
endif()
