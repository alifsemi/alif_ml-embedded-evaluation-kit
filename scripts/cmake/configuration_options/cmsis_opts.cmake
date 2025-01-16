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
# CMSIS configuration options
#----------------------------------------------------------------------------
include_guard()

include(util_functions)

message(STATUS "Assessing CMSIS configuration options...")

USER_OPTION(CMSIS_SRC_PATH
    "Path to CMSIS sources"
    "${MLEK_DEPENDENCY_ROOT_DIR}/cmsis-6"
    PATH)

USER_OPTION(CMSIS_DSP_SRC_PATH
    "Path to CMSIS DSP sources"
    "${MLEK_DEPENDENCY_ROOT_DIR}/cmsis-dsp"
    PATH)

USER_OPTION(CMSIS_NN_SRC_PATH
    "Path to CMSIS NN sources"
    "${MLEK_DEPENDENCY_ROOT_DIR}/cmsis-nn"
    PATH)

USER_OPTION(CORTEX_DFP_SRC_PATH
    "Path to Cortex DFP sources"
    "${MLEK_DEPENDENCY_ROOT_DIR}/cortex-dfp"
    PATH)
