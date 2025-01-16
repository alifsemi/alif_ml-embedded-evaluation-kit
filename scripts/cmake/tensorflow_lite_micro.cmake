#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2021-2024 Arm Limited and/or its
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

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL "-O0")
    set(TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL "-O0")
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    # -Ofast is not an option as we set the floating-point conformance mode
    # to 'full' in the TensorFlow Lite Micro Makefile. Although this is done
    # only for Arm Compiler, we stick with the '-O3' optimisation level for
    # all compilers.
    set(TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL "-O3")
    set(TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL "-O3")
endif()

assert_defined(TENSORFLOW_LITE_MICRO_BUILD_TYPE)

if (NOT TARGET_PLATFORM STREQUAL native)
    assert_defined(CMSIS_VER)
endif()

# When using CMSIS version 6, we can't use the TensorFlow Lite Micro build framework as the Makefile build
# system relies on CMSIS-5. Here, we use the CMake wrapper provided by ethos-u-core-software instead. We could,
# in theory, use the same flow even when we are using CMSIS-5 but it is worth keeping the older path alive
# because it exercises the canonical TensorFlow Lite Micro build flow and it doesn't download the third party
# dependencies every time the project is configured.
if (CMSIS_VER STREQUAL 6)
    include(FetchContent)
    set(CORE_SOFTWARE_REPO_URL "https://git.mlplatform.org/ml/ethos-u/ethos-u-core-software.git")
    set(CORE_SOFTWARE_GIT_TAG  "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}")
    set(TFLM_CMAKE_URL "${CORE_SOFTWARE_REPO_URL}/plain/tflite_micro.cmake?h=${CORE_SOFTWARE_GIT_TAG}")
    set(TFLM_CMAKE_MD5 "b830f62c2e6e5a0a87438fcf1f4b8eee")

    FetchContent_Declare(TensorFlow_Lite_Micro_CMake_Wrapper
        URL                 ${TFLM_CMAKE_URL}
        URL_HASH            MD5=${TFLM_CMAKE_MD5}
        DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}
        DOWNLOAD_NAME       "tflite_micro.cmake"
        DOWNLOAD_NO_EXTRACT ON)

    FetchContent_MakeAvailable(TensorFlow_Lite_Micro_CMake_Wrapper)

    set(CMSIS_NN_PATH           ${CMSIS_NN_SRC_PATH})
    set(TENSORFLOW_PATH         ${TENSORFLOW_SRC_PATH})
    set(TFLM_OPTIMIZATION_LEVEL ${TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL}
                                CACHE STRING "Optimization level")
    set(TFLM_BUILD_TYPE         ${TENSORFLOW_LITE_MICRO_BUILD_TYPE}
                                CACHE STRING "Build type")

    include(${CMAKE_BINARY_DIR}/tflite_micro.cmake)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang")
        # Additional option to enable "full" floating point standard conformance
        # (needed for NaNs and infinity).
        target_compile_options(tflu PRIVATE "-ffp-mode=full")
    endif()

    # If CPU_HEADER_FILE is defined, add this to compile definitions.
    if (DEFINED CPU_HEADER_FILE)
        target_compile_definitions(tflu PRIVATE
            "-DCMSIS_DEVICE_ARM_CORTEX_M_XX_HEADER_FILE=\"${CPU_HEADER_FILE}\"")
    endif()

    # Create an alias to use in other parts of the project.
    add_library(tensorflow-lite-micro ALIAS tflu)

    # Return from here, rest of the section is for CMSIS-5 and building with
    # default Makefile.
    return()
endif()

if (DEFINED ENV{CMAKE_BUILD_PARALLEL_LEVEL})
    set(PARALLEL_JOBS $ENV{CMAKE_BUILD_PARALLEL_LEVEL})
else()
    include(ProcessorCount)
    ProcessorCount(PARALLEL_JOBS)
endif()

assert_defined(TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS)
assert_defined(TENSORFLOW_LITE_MICRO_CLEAN_BUILD)

if (CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang")
    set(TENSORFLOW_LITE_MICRO_TOOLCHAIN "armclang")
    # Alif workaround as this version of Tensorflow Makefile does not set -ffp-mode=full
    # This fixed in ML development version, so should be removed when rebasing with new version
    set(TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL "${TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL} -ffp-mode=full")
    set(TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL "${TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL} -ffp-mode=full")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(TENSORFLOW_LITE_MICRO_TOOLCHAIN "gcc")
else ()
    message(FATAL_ERROR "No compiler ID is set")
endif()

get_filename_component(TENSORFLOW_LITE_MICRO_TARGET_TOOLCHAIN_ROOT ${CMAKE_C_COMPILER} DIRECTORY)
set(TENSORFLOW_LITE_MICRO_TARGET_TOOLCHAIN_ROOT "${TENSORFLOW_LITE_MICRO_TARGET_TOOLCHAIN_ROOT}/")

set(TENSORFLOW_LITE_MICRO_PATH "${TENSORFLOW_SRC_PATH}/tensorflow/lite/micro")
set(TENSORFLOW_LITE_MICRO_GENDIR ${CMAKE_CURRENT_BINARY_DIR}/tensorflow/)
set(TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME "libtensorflow-microlite.a")

# Add virtual environment's Python directory path to the system path.
# NOTE: This path is passed to the TensorFlow Lite Micro's make env
# as it depends on some basic Python packages (like Pillow) installed
# and the system-wide Python installation might not have them.
set(ENV_PATH "${PYTHON_VENV}/bin:$ENV{PATH}")

if (TARGET_PLATFORM STREQUAL native)
    set(TENSORFLOW_LITE_MICRO_TARGET "linux")
    set(TENSORFLOW_LITE_MICRO_TARGET_ARCH x86_64)
else()
    set(TENSORFLOW_LITE_MICRO_TARGET "cortex_m_generic")

    if ("${CMAKE_SYSTEM_ARCH}" STREQUAL "armv8.1-m.main")
        set(TENSORFLOW_LITE_MICRO_TARGET_ARCH "cortex-m55")
    else()
        set(TENSORFLOW_LITE_MICRO_TARGET_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
    endif()

    if(ETHOS_U_NPU_ENABLED)
        # Arm Ethos-U55 NPU is the co-processor for ML workload:
        set(TENSORFLOW_LITE_MICRO_CO_PROCESSOR      "ethos_u")
        if(${ETHOS_U_NPU_ID} STREQUAL "U65")
            set(TENSORFLOW_LITE_MICRO_CO_PROCESSOR_ARCH "u65")
        else()
            set(TENSORFLOW_LITE_MICRO_CO_PROCESSOR_ARCH "u55")
        endif ()
    endif()

    set(TENSORFLOW_LITE_MICRO_OPTIMIZED_KERNEL  "cmsis_nn")
endif()

if (TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS)
    message(STATUS "Refreshing TensorFlow Lite Micro's third party downloads...")
    set(ENV{PATH} "${ENV_PATH}")
    execute_process(
        COMMAND
        make -f ${TENSORFLOW_LITE_MICRO_PATH}/tools/make/Makefile clean_downloads third_party_downloads
        RESULT_VARIABLE return_code
        WORKING_DIRECTORY ${TENSORFLOW_SRC_PATH})
    if (NOT return_code EQUAL "0")
        message(FATAL_ERROR "Failed to clean TensorFlow Lite Micro's third party downloads.")
    else()
        message(STATUS "Refresh completed.")
    endif ()
endif()

if (TENSORFLOW_LITE_MICRO_CLEAN_BUILD)
    list(APPEND MAKE_TARGETS_LIST "clean")
endif()

# Primary target
list(APPEND MAKE_TARGETS_LIST "microlite")
message(STATUS "TensorFlow Lite Micro build to be called for these targets: ${MAKE_TARGETS_LIST}")

# Commands and targets
add_custom_target(tensorflow_build ALL

    # Command to build the TensorFlow Lite Micro library
    COMMAND ${CMAKE_COMMAND} -E env PATH="${ENV_PATH}"
        make -j${PARALLEL_JOBS} -f ${TENSORFLOW_LITE_MICRO_PATH}/tools/make/Makefile ${MAKE_TARGETS_LIST}
        TARGET_TOOLCHAIN_ROOT=${TENSORFLOW_LITE_MICRO_TARGET_TOOLCHAIN_ROOT}
        TOOLCHAIN=${TENSORFLOW_LITE_MICRO_TOOLCHAIN}
        GENDIR=${TENSORFLOW_LITE_MICRO_GENDIR}
        TARGET=${TENSORFLOW_LITE_MICRO_TARGET}
        TARGET_ARCH=${TENSORFLOW_LITE_MICRO_TARGET_ARCH}
        BUILD_TYPE=${TENSORFLOW_LITE_MICRO_BUILD_TYPE}
        CMSIS_PATH=${CMSIS_SRC_PATH}
        CMSIS_NN_PATH=${CMSIS_NN_SRC_PATH}
        # Conditional arguments
        $<$<BOOL:${ETHOS_U_NPU_ENABLED}>:ETHOSU_ARCH=${TENSORFLOW_LITE_MICRO_CO_PROCESSOR_ARCH}>
        $<$<BOOL:${ETHOS_U_NPU_ENABLED}>:ETHOSU_DRIVER_PATH=${ETHOS_U_NPU_DRIVER_SRC_PATH}>
        $<$<BOOL:${ETHOS_U_NPU_ENABLED}>:ETHOSU_DRIVER_LIBS=$<TARGET_FILE:ethosu_core_driver>>

        $<$<BOOL:${TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL}>:CORE_OPTIMIZATION_LEVEL=${TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL}>
        $<$<BOOL:${TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL}>:KERNEL_OPTIMIZATION_LEVEL=${TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL}>
        $<$<BOOL:${TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL}>:THIRD_PARTY_KERNEL_OPTIMIZATION_LEVEL=${TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL}>
        $<$<BOOL:${TENSORFLOW_LITE_MICRO_OPTIMIZED_KERNEL}>:OPTIMIZED_KERNEL_DIR=${TENSORFLOW_LITE_MICRO_OPTIMIZED_KERNEL}>
        $<$<BOOL:${TENSORFLOW_LITE_MICRO_CO_PROCESSOR}>:CO_PROCESSOR=${TENSORFLOW_LITE_MICRO_CO_PROCESSOR}>

    # Command to copy over the generated library to the local build tree.
    COMMAND ${CMAKE_COMMAND} -E copy  ${TENSORFLOW_LITE_MICRO_GENDIR}/lib/${TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME}
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME}

    COMMENT "Building TensorFlow Lite Micro library..."

    BYPRODUCTS ${TENSORFLOW_SRC_PATH}/tensorflow/tensorflow/lite/micro/tools/make/downloads
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME}
                ${TENSORFLOW_LITE_MICRO_GENDIR}/lib/${TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME}

    WORKING_DIRECTORY ${TENSORFLOW_SRC_PATH})

# Create library


set(TENSORFLOW_LITE_MICRO_TARGET tensorflow-lite-micro)
add_library(${TENSORFLOW_LITE_MICRO_TARGET} STATIC IMPORTED)

if(ETHOS_U_NPU_ENABLED)
    target_link_libraries(tensorflow-lite-micro INTERFACE ethos_u_npu)
endif()

add_dependencies(tensorflow-lite-micro tensorflow_build)

set_property(TARGET tensorflow-lite-micro PROPERTY IMPORTED_LOCATION
             "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME}")

target_include_directories(tensorflow-lite-micro
    INTERFACE
    ${TENSORFLOW_SRC_PATH})

target_compile_definitions(tensorflow-lite-micro
    INTERFACE
    TF_LITE_STATIC_MEMORY)
