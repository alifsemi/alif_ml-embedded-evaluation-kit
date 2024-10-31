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
# FVP configuration options
#----------------------------------------------------------------------------
include_guard()

include(util_functions)

message(STATUS "Assessing FVP test configuration options...")

USER_OPTION(BUILD_FVP_TESTS "Build tests for CTest driven FVP runs for built applications"
    OFF
    BOOL)

if (BUILD_FVP_TESTS)
    USER_OPTION(FVP_PATH "Path to FVP for verifying execution"
            ""
            FILEPATH)
else()
    USER_OPTION(FVP_VSI_ENABLED "Indicates if Virtual Streaming Interface is enabled"
        OFF
        BOOL)

    if (${FVP_VSI_ENABLED})
        USER_OPTION(FVP_VSI_SRC_PATH "Root directory for VSI sources"
            "${MLEK_DEPENDENCY_ROOT_DIR}/avh"
            PATH)
    endif()
endif()

# Assuming USE_SINGLE_INPUT (from common options) is included before these options
# warn the developer that FVP tests may run using multiple sample files (depending on
# the use case)
if (BUILD_FVP_TESTS AND (NOT USE_SINGLE_INPUT))
    message(WARNING "FVP tests enabled but USE_SINGLE_INPUT is set to OFF")
endif()

# Warn if path to sources are set but VSI is not enabled.
if (FVP_VSI_SRC_PATH AND (NOT FVP_VSI_ENABLED))
    message(WARNING "FVP_VSI_SRC_PATH provided but FVP_VSI_ENABLED is not set.")
endif()

# Fail if FVP tests and VSI are enabled - the tests won't ever finish executing
# gracefully.
if (BUILD_FVP_TESTS AND FVP_VSI_ENABLED)
    message(FATAL_ERROR "FVP tests cannot be enabled with VSI enabled too.")
endif()
