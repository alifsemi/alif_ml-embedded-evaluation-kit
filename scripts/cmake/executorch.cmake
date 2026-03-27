#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its
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
set(EXECUTORCH_BUILD_DEVTOOLS                   OFF)
set(EXECUTORCH_ENABLE_EVENT_TRACER              OFF)
set(GFLAGS_INTTYPES_FORMAT                      C99)
set(CMAKE_POSITION_INDEPENDENT_CODE             OFF)

if(TARGET_PLATFORM STREQUAL native)
    set(EXECUTORCH_BUILD_ARM_BAREMETAL          OFF)
    set(EXECUTORCH_BUILD_CORTEX_M               OFF)
    set(EXECUTORCH_BUILD_CPUINFO                ON)
else()
    set(EXECUTORCH_BUILD_ARM_BAREMETAL          ON)
    set(EXECUTORCH_BUILD_CORTEX_M               ON)
    set(EXECUTORCH_BUILD_HOST_TARGETS           OFF)
endif()

set(EXECUTORCH_PAL_DEFAULT                      minimal)

# Map ExecuTorch supported log levels
if (${MLEK_LOG_LEVEL} STREQUAL MLEK_LOG_LEVEL_TRACE OR
    ${MLEK_LOG_LEVEL} STREQUAL MLEK_LOG_LEVEL_DEBUG)
    set(EXECUTORCH_LOG_LEVEL                    "Debug")
elseif(${MLEK_LOG_LEVEL} STREQUAL MLEK_LOG_LEVEL_INFO)
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
    add_subdirectory(${EXECUTORCH_SRC_PATH}
        ${CMAKE_BINARY_DIR}/executorch EXCLUDE_FROM_ALL)


    if(EXECUTORCH_ENABLE_EVENT_TRACER)
        target_compile_options(executorch INTERFACE -DET_EVENT_TRACER_ENABLED)
        target_compile_options(portable_ops_lib INTERFACE -DET_EVENT_TRACER_ENABLED)
    endif()
endblock()

# Collate the targets for easily linking against.
add_library(mlek_executorch INTERFACE)

target_link_libraries(mlek_executorch INTERFACE
    program_schema
    extension_runner_util
    quantized_ops_lib)

# Based on target, link to the correct portable ops library
if (TARGET_PLATFORM STREQUAL native)
    target_link_libraries(mlek_executorch INTERFACE
        portable_ops_lib)
else()
    target_link_libraries(mlek_executorch INTERFACE
        cortex_m_ops_lib
        kernels_util_all_deps)

    # Workaround for Arm CMSIS-NN library mistakenly expected to be placed in
    # the BINARY_DIR for the target. This can be removed once fixed in ExecuTorch
    # source tree - backends/cortex_m/CMakeLists.txt.
    if (TARGET cmsis-nn)
        set_property(TARGET cmsis-nn PROPERTY INTERFACE_INCLUDE_DIRECTORIES
          "$<BUILD_INTERFACE:${CMSIS_NN_SRC_PATH}/Include>"
          "$<BUILD_INTERFACE:${CMSIS_SRC_PATH}/CMSIS/Core/Include>"
          "$<INSTALL_INTERFACE:include/cmsis-nn>"
          "$<INSTALL_INTERFACE:include/cmsis>")
    endif()

    # Issue with ExecuTorch 1.0.0 in cortex_m_kernels library
    # Has been fixed in latest ExecuTorch - can be removed once
    # we migrate.
    if (TARGET cortex_m_kernels)
        target_compile_definitions(cortex_m_kernels PRIVATE False=false)
    endif()

    if (TARGET executorch_delegate_ethos_u)
        # If Arm Ethos-U NPU driver is defined as a target, we edit its
        # include directory paths for it to be installed as a dependency
        # for the Arm Ethos-U NPU backend within ExecuTorch.
        if (TARGET ethosu_core_driver AND EXECUTORCH_BUILD_ARM_BAREMETAL)
            install(TARGETS ethosu_core_driver EXPORT ExecuTorchTargets)
            get_target_property(_NPU_INTERFACE_INC
                ethosu_core_driver INTERFACE_INCLUDE_DIRECTORIES)
            set_target_properties(ethosu_core_driver
                PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")

            target_include_directories(ethosu_core_driver PUBLIC
                $<BUILD_INTERFACE:${_NPU_INTERFACE_INC}>
                $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)
        endif()

        # Supress warnings from Arm Ethos-U delegate library
        target_compile_options(executorch_delegate_ethos_u PRIVATE
            -Wno-error=deprecated-declarations
            -Wno-error=unused-parameter)

        # Whole archive needs to be included for the delegate.
        target_link_libraries(mlek_executorch INTERFACE
            "-Wl,--whole-archive"
            $<TARGET_FILE:executorch_delegate_ethos_u>
            "-Wl,--no-whole-archive")

        # Explicitly add dependency as some generators might not
        # add it automatically when only TARGET_FILE is used
        # in linking.
        add_dependencies(mlek_executorch executorch_delegate_ethos_u)
    endif()
endif()

# Provide alias target for rest of projects to use
add_library(meta::executorch ALIAS mlek_executorch)

# Include code generation wrappers from ExecuTorch.
set(EXECUTORCH_ROOT ${EXECUTORCH_SRC_PATH})
include(${EXECUTORCH_SRC_PATH}/tools/cmake/Codegen.cmake)

##############################################################################
# This function generates a portable ops library for the PTE model file.
# @param[in]    MODEL_PATH      path to a PTE file
# @param[in]    LIB_NAME        output target library name.
# @param[in]    SELECT_OPS_LIST ops list that should always be included
##############################################################################
function(generate_pte_ops_lib)

    if (NOT EXECUTORCH_BUILD_ARM_BAREMETAL)
        message(STATUS "Skipping custom portable ops lib generation. "
                       "Generation of PTE specific portable ops lib "
                       "is required only for bare-metal Arm targets.")
        return()
    endif()

    set(oneValueArgs MODEL_PATH LIB_NAME SELECT_OPS_LIST)
    cmake_parse_arguments(PARSED "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Absolute paths for passing into python script
    get_filename_component(ABS_MODEL_PATH ${PARSED_MODEL_PATH} ABSOLUTE)

    # Ensure Python virtual environment bin location is available.
    set(ENV_PATH "${PYTHON_VENV}/bin:$ENV{PATH}")
    set(EXECUTORCH_ROOT ${EXECUTORCH_SRC_PATH})

    # Override the Python executable set by ExecuTorch's Utils.cmake
    # It expects a conda environment and sets this. We set it here
    # for our virtual environment python to be used instead.
    set(PYTHON_EXECUTABLE ${PYTHON})

    message(STATUS "Attempting PTE ops library generation: ${PARSED_LIB_NAME}")
    message(STATUS "    PTE file: ${PARSED_MODEL_PATH}")
    message(STATUS "    Select ops list: ${PARSED_SELECT_OPS_LIST}")

    execute_process(
        COMMAND ${PYTHON_EXECUTABLE}
            "${EXECUTORCH_SRC_PATH}/codegen/tools/gen_oplist.py"
            --model_file_path=${PARSED_MODEL_PATH}
            --output_path=${CMAKE_CURRENT_BINARY_DIR}/${PARSED_LIB_NAME}-ops.yaml
        COMMAND_ERROR_IS_FATAL  ANY
        COMMAND_ECHO            STDOUT
        OUTPUT_VARIABLE         OPS_YML_GEN_RESULT)

    message(DEBUG "Ops report for ${PARSED_LIB_NAME}:"
                  "${OPS_YML_GEN_RESULT}")

    # Does the op list contain any aten or dim order ops? If so, we can provide
    # the path to PTE file for ops lib gen. Otherwise, we pass in an empty argument.
    if (OPS_YML_GEN_RESULT MATCHES "aten::" OR
        OPS_YML_GEN_RESULT MATCHES "dim_order_ops::")
        set(PTE_FOR_OPS_LIB ${PARSED_MODEL_PATH})
    else()
        message(STATUS "No aten or dim_order_ops found in ${PARSED_MODEL_PATH}")
        set(PTE_FOR_OPS_LIB "")
    endif()

    # Generate C++ bindings to register kernels into both PyTorch (for AOT) and
    # Executorch (for runtime). Here select all ops in functions.yaml
    if ("${PARSED_SELECT_OPS_LIST}" STREQUAL "" AND
        "${PTE_FOR_OPS_LIB}" STREQUAL "")
        message(STATUS "No portable ops library needs to be generated.")
        return()
    endif()

    gen_selected_ops(
        LIB_NAME                "${PARSED_LIB_NAME}"
        OPS_SCHEMA_YAML         ""
        ROOT_OPS                "${PARSED_SELECT_OPS_LIST}"
        INCLUDE_ALL_OPS         ""
        OPS_FROM_MODEL          "${PTE_FOR_OPS_LIB}"
        DTYPE_SELECTIVE_BUILD   "${EXECUTORCH_ENABLE_DTYPE_SELECTIVE_BUILD}"
    )

    generate_bindings_for_kernels(
        LIB_NAME                "${PARSED_LIB_NAME}"
        FUNCTIONS_YAML          "${EXECUTORCH_SRC_PATH}/kernels/portable/functions.yaml"
        DTYPE_SELECTIVE_BUILD   "${EXECUTORCH_ENABLE_DTYPE_SELECTIVE_BUILD}"
    )

    gen_operators_lib(
        LIB_NAME                "${PARSED_LIB_NAME}"
        KERNEL_LIBS             portable_kernels
        DEPS                    executorch
        DTYPE_SELECTIVE_BUILD   "${EXECUTORCH_ENABLE_DTYPE_SELECTIVE_BUILD}"
    )
endfunction()
