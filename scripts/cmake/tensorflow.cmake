#----------------------------------------------------------------------------
#  Copyright (c) 2021 - 2022 Arm Limited. All rights reserved.
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

include(ProcessorCount)
ProcessorCount(J)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL "-O0")
    set(TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL "-O0")
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set(TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL "-O3")
    set(TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL "-O3")
endif()

assert_defined(TENSORFLOW_LITE_MICRO_BUILD_TYPE)
assert_defined(TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS)
assert_defined(TENSORFLOW_LITE_MICRO_CLEAN_BUILD)

if (CMAKE_CXX_COMPILER_ID STREQUAL "ARMClang")
    set(TENSORFLOW_LITE_MICRO_TOOLCHAIN "armclang")
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
        set(TENSORFLOW_LITE_MICRO_CO_PROCESSOR  "ethos_u")
        set(ETHOS_U_NPU_ID "u55")  # Currently only u55 is supported by TFLite Micro.
    endif()

    set(TENSORFLOW_LITE_MICRO_OPTIMIZED_KERNEL  "cmsis_nn")
endif()

if (TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS)
    list(APPEND MAKE_TARGETS_LIST "clean_downloads")
endif()

if (TENSORFLOW_LITE_MICRO_CLEAN_BUILD)
    list(APPEND MAKE_TARGETS_LIST "clean")
endif()

# Primary target
list(APPEND MAKE_TARGETS_LIST "microlite")
message(STATUS "TensorFlow Lite Micro build to be called for these targets: ${MAKE_TARGETS_LIST}")

# Add virtual environment's Python directory path to the system path.
# NOTE: This path is passed to the TensorFlow Lite Micro's make env
# as it depends on some basic Python packages (like Pillow) installed
# and the system-wide Python installation might not have them.
set(ENV_PATH "${PYTHON_VENV}/bin:$ENV{PATH}")

# Commands and targets
add_custom_target(tensorflow_build ALL

    # Command to build the TensorFlow Lite Micro library
    COMMAND ${CMAKE_COMMAND} -E env PATH=${ENV_PATH}
        make -j${J} -f ${TENSORFLOW_LITE_MICRO_PATH}/tools/make/Makefile ${MAKE_TARGETS_LIST}
        TARGET_TOOLCHAIN_ROOT=${TENSORFLOW_LITE_MICRO_TARGET_TOOLCHAIN_ROOT}
        TOOLCHAIN=${TENSORFLOW_LITE_MICRO_TOOLCHAIN}
        GENDIR=${TENSORFLOW_LITE_MICRO_GENDIR}
        TARGET=${TENSORFLOW_LITE_MICRO_TARGET}
        TARGET_ARCH=${TENSORFLOW_LITE_MICRO_TARGET_ARCH}
        BUILD_TYPE=${TENSORFLOW_LITE_MICRO_BUILD_TYPE}
        CMSIS_PATH=${CMSIS_SRC_PATH}
        # Conditional arguments
        $<$<BOOL:${ETHOS_U_NPU_ENABLED}>:ETHOSU_ARCH=${ETHOS_U_NPU_ID}>
        $<$<BOOL:${ETHOS_U_NPU_ENABLED}>:ETHOSU_DRIVER_PATH=${ETHOS_U_NPU_DRIVER_SRC_PATH}>
        $<$<BOOL:${ETHOS_U_NPU_ENABLED}>:ETHOSU_DRIVER_LIBS=$<TARGET_FILE:ethosu_core_driver>>

        $<$<BOOL:${TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL}>:CORE_OPTIMIZATION_LEVEL=${TENSORFLOW_LITE_MICRO_CORE_OPTIMIZATION_LEVEL}>
        $<$<BOOL:${TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL}>:KERNEL_OPTIMIZATION_LEVEL=${TENSORFLOW_LITE_MICRO_KERNEL_OPTIMIZATION_LEVEL}>
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
    add_dependencies(tensorflow_build ethosu_core_driver)
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
