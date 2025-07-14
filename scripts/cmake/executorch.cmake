#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
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

# Limitations
# 1. Arm compiler is not supported.
if (CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang")
    message(
            FATAL_ERROR
            "ExecuTorch does not currently support Arm Compiler. "
            "Use the ML_FRAMEWORK argument to specify a different framework."
    )
endif ()

# 2. Arm Ethos-U65 and Dedicated_Sram are not supported in current revision, but
#    are known to be working in a more recent versions which we need to migrate to.
if (ETHOS_U_NPU_ENABLED)
    if (ETHOS_U_NPU_ID STREQUAL U65)
        message(FATAL_ERROR "Support for Arm Ethos-U65 is currently disabled in this "
                            "experimental branch. Use Arm Ethos-U55 or Arm Ethos-U85")
    endif()
    if (ETHOS_U_NPU_MEMORY_MODE STREQUAL Dedicated_Sram)
        message(FATAL_ERROR "`Dedicated_Sram` memory mode is not supported "
                            "by current rev of ExecuTorch."
                            "Use `Sram_Only` or `Shared_Sram` modes")
    endif()
endif()

# Validate pre-requisites.
assert_defined(EXECUTORCH_SRC_PATH)
assert_defined(PYTHON_VENV)
assert_defined(PYTHON)

# Prepare CMake configuration overrides.
set(EXECUTORCH_BUILD_EXECUTOR_RUNNER            OFF)
set(EXECUTORCH_BUILD_KERNELS_QUANTIZED          ON)
set(EXECUTORCH_BUILD_EXTENSION_RUNNER_UTIL      ON)
set(EXECUTORCH_ENABLE_LOGGING                   ON)

if(TARGET_PLATFORM STREQUAL native)
    set(EXECUTORCH_BUILD_ARM_BAREMETAL          OFF)
    set(EXECUTORCH_BUILD_CPUINFO                ON)
else()
    set(EXECUTORCH_BUILD_ARM_BAREMETAL          ON)
    set(EXECUTORCH_BUILD_HOST_TARGETS           OFF)
endif()

set(EXECUTORCH_PAL_DEFAULT                      minimal)

# Map ExecuTorch supported log levels
if (${LOG_LEVEL} STREQUAL MLEK_LOG_LEVEL_TRACE OR
    ${LOG_LEVEL} STREQUAL MLEK_LOG_LEVEL_DEBUG)
    set(EXECUTORCH_LOG_LEVEL                    "Debug")
elseif(${LOG_LEVEL} STREQUAL MLEK_LOG_LEVEL_INFO)
    set(EXECUTORCH_LOG_LEVEL                    "Info")
else()
    set(EXECUTORCH_LOG_LEVEL                    "Error")
endif()

# Prevent littering callee/parent scope: Create a block for variables
# needed only by ExecuTorch related configuration.
block(SCOPE_FOR VARIABLES)
    # Ensure Python virtual environment bin location is available.
    set(ENV_PATH "${PYTHON_VENV}/bin:$ENV{PATH}")

    # Override the Python executable set by ExecuTorch's Utils.cmake
    # It expects a conda environment and sets this. We set it here
    # for our virtual environment python to be used instead.
    set(PYTHON_EXECUTABLE ${PYTHON})

    # Set FLATC location - this should always be available (built for host)
    # or else a build will be attempted.
    if (EXISTS ${PYTHON_VENV}/bin/flatc)
        set(FLATC_EXECUTABLE "${PYTHON_VENV}/bin/flatc")
    else()
        message(FATAL_ERROR "flatc executable doesn't exist")
    endif()

    # Add ET main subdirectory
    add_subdirectory(${EXECUTORCH_SRC_PATH} ${CMAKE_BINARY_DIR}/executorch)

    # Generate C++ bindings to register kernels into both PyTorch (for AOT) and
    # Executorch (for runtime). Here select all ops in functions.yaml
    set(EXECUTORCH_ROOT ${EXECUTORCH_SRC_PATH})

    # Set up build for portable ops library.
    include(${EXECUTORCH_SRC_PATH}/tools/cmake/Utils.cmake)
    include(${EXECUTORCH_SRC_PATH}/tools/cmake/Codegen.cmake)

    # @TODO Placeholder for experimentation - this needs to come from user.
    set(EXECUTORCH_SELECT_OPS_LIST "aten::_softmax.out")
    gen_selected_ops(
        LIB_NAME "arm_portable_ops_lib"
        OPS_SCHEMA_YAML ""
        ROOT_OPS "${EXECUTORCH_SELECT_OPS_LIST}"
        INCLUDE_ALL_OPS ""
    )

    generate_bindings_for_kernels(
        LIB_NAME "arm_portable_ops_lib"
        FUNCTIONS_YAML ${EXECUTORCH_SRC_PATH}/kernels/portable/functions.yaml
    )

    gen_operators_lib(
        LIB_NAME "arm_portable_ops_lib"
        KERNEL_LIBS portable_kernels
        DEPS executorch
    )

    if(EXECUTORCH_ENABLE_EVENT_TRACER)
        target_compile_options(executorch INTERFACE -DET_EVENT_TRACER_ENABLED)
        target_compile_options(portable_ops_lib INTERFACE -DET_EVENT_TRACER_ENABLED)
    endif()
endblock()

set(MLEK_EXECUTORCH_LINK_STR)
list(APPEND MLEK_EXECUTORCH_LINK_STR
    "-Wl,--whole-archive"
    $<$<BOOL:${ETHOS_U_NPU_ENABLED}>:executorch_delegate_ethos_u>
    "-Wl,--no-whole-archive"
    quantized_ops_lib
    quantized_kernels
    portable_kernels
    executorch
    extension_runner_util
)

if (TARGET_PLATFORM STREQUAL native)
    list(APPEND MLEK_EXECUTORCH_LINK_STR portable_ops_lib)
else()
    list(APPEND MLEK_EXECUTORCH_LINK_STR arm_portable_ops_lib)
endif()
