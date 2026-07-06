#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its
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

include_guard(GLOBAL)

set(ML_FRAMEWORK_TFLM "tflm")
set(ML_FRAMEWORK_EXECUTORCH "executorch")

set(ML_FRAMEWORK_TFLM_ALIASES
    "${ML_FRAMEWORK_TFLM}"
    "tensorflowlitemicro")

set(ML_FRAMEWORK_EXECUTORCH_ALIASES
    "${ML_FRAMEWORK_EXECUTORCH}"
    "et")

set(ML_FRAMEWORK_CANONICAL_VALUES
    "${ML_FRAMEWORK_TFLM}"
    "${ML_FRAMEWORK_EXECUTORCH}")

set(ML_FRAMEWORK_ACCEPTED_VALUES
    ${ML_FRAMEWORK_TFLM_ALIASES}
    ${ML_FRAMEWORK_EXECUTORCH_ALIASES})

# Normalize an ML framework selector to its canonical option value.
#
# Arguments:
#   output_var
#     Variable name that receives the canonical ML framework value.
#   input_value
#     User-provided framework selector. Matching is case-insensitive.
#
# Outputs:
#   output_var
#     Set to the canonical ML framework value in the parent scope.
function(normalize_ml_framework output_var input_value)
    string(STRIP "${input_value}" _ML_FRAMEWORK_VALUE)
    string(TOLOWER "${_ML_FRAMEWORK_VALUE}" _ML_FRAMEWORK_VALUE)

    if ("${_ML_FRAMEWORK_VALUE}" IN_LIST ML_FRAMEWORK_TFLM_ALIASES)
        set(${output_var} "${ML_FRAMEWORK_TFLM}" PARENT_SCOPE)
    elseif ("${_ML_FRAMEWORK_VALUE}" IN_LIST ML_FRAMEWORK_EXECUTORCH_ALIASES)
        set(${output_var} "${ML_FRAMEWORK_EXECUTORCH}" PARENT_SCOPE)
    else ()
        list(JOIN ML_FRAMEWORK_ACCEPTED_VALUES ", " _ML_FRAMEWORK_ACCEPTED_VALUES_TEXT)
        message(FATAL_ERROR
            "Invalid ML_FRAMEWORK: ${input_value}. Accepted values are "
            "${_ML_FRAMEWORK_ACCEPTED_VALUES_TEXT} (case-insensitive).")
    endif ()
endfunction()

# Configure the ML framework selector as a normalized cache option.
#
# Inputs:
#   ML_FRAMEWORK
#     Optional framework selector. Defaults to TensorFlow Lite Micro when it
#     is not defined. Matching is case-insensitive and accepts aliases.
#
# Outputs:
#   ML_FRAMEWORK
#     Set to the canonical framework value in the caller's scope and in the
#     CMake cache. The cache entry also advertises the canonical values.
function(configure_ml_framework_option)
    if (NOT DEFINED ML_FRAMEWORK)
        set(ML_FRAMEWORK "${ML_FRAMEWORK_TFLM}")
    endif()

    normalize_ml_framework(_ML_FRAMEWORK_VALUE "${ML_FRAMEWORK}")
    set(ML_FRAMEWORK "${_ML_FRAMEWORK_VALUE}" CACHE STRING
        "Select the ML inference framework to be used."
        FORCE)
    set_property(CACHE ML_FRAMEWORK PROPERTY STRINGS
        ${ML_FRAMEWORK_CANONICAL_VALUES})
    set(ML_FRAMEWORK "${_ML_FRAMEWORK_VALUE}" PARENT_SCOPE)
endfunction()
