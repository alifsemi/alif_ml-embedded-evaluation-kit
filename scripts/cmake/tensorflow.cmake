#----------------------------------------------------------------------------
#  Copyright (c) 2021 Arm Limited. All rights reserved.
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
    set(TENSORFLOW_LITE_MICRO_DEFAULT_BUILD_TYPE "debug")
    set(TENSORFLOW_LITE_MICRO_OPTIMIZATION_LEVEL "-O0")
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
    set(TENSORFLOW_LITE_MICRO_DEFAULT_BUILD_TYPE "release")
    set(TENSORFLOW_LITE_MICRO_OPTIMIZATION_LEVEL "-O3")
elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    set(TENSORFLOW_LITE_MICRO_DEFAULT_BUILD_TYPE "release_with_logs")
    # No override for optimsiation level; we rely on the default
    # optimisation applied by TensorFlow Lite Micro build here.
elseif (NOT DEFINED TENSORFLOW_LITE_MICRO_BUILD_TYPE)
    message(WARNING     "TENSORFLOW_LITE_MICRO_BUILD_TYPE is not set.")
    message(FATAL_ERROR "Build type ${CMAKE_BUILD_TYPE} does not have a corresponding "
                        "default to set TensorFlow build type")
endif()

USER_OPTION(TENSORFLOW_LITE_MICRO_BUILD_TYPE "TensorFlow Lite Mirco build type (release/debug etc.)"
    ${TENSORFLOW_LITE_MICRO_DEFAULT_BUILD_TYPE}
    STRING)

USER_OPTION(TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS "Select if TPIP downloads should be cleaned before each build."
    OFF
    BOOL)

USER_OPTION(TENSORFLOW_LITE_MICRO_CLEAN_BUILD "Select if clean target should be added to a list of targets"
    ON
    BOOL)

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

set(CMSIS_DSP_MAKEFILE_INC ${CMAKE_CURRENT_SOURCE_DIR}/scripts/make/cmsis_dsp.inc)
set(ETHOS_EVAL_TARGET_MAKEFILE_INC ${CMAKE_CURRENT_SOURCE_DIR}/scripts/make/cortex_m_ethos_eval_makefile.inc)

if (TARGET_PLATFORM STREQUAL native)
    set(TENSORFLOW_LITE_MICRO_TARGET "linux")
    set(TENSORFLOW_LITE_MICRO_TARGET_ARCH x86_64)
else()
    set(TENSORFLOW_LITE_MICRO_TARGET "cortex_m_ethos_eval")
    set(TENSORFLOW_LITE_MICRO_TARGET_ARCH ${CMAKE_SYSTEM_PROCESSOR}${CPU_FEATURES})
    if(ETHOS_U55_ENABLED)
        # Arm Ethos-U55 NPU is the co-processor for ML workload:
        set(TENSORFLOW_LITE_MICRO_CO_PROCESSOR  "ethos_u")
    endif()

    set(TENSORFLOW_LITE_MICRO_OPTIMIZED_KERNEL  "cmsis_nn")

    # Copy over the target helper (cortex_m_ethos_eval)
    file(COPY ${ETHOS_EVAL_TARGET_MAKEFILE_INC}
        DESTINATION ${TENSORFLOW_LITE_MICRO_PATH}/tools/make/targets/)
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

# Commands and targets
add_custom_target(tensorflow_build ALL

    # Command to build the TensorFlow Lite Micro library
    COMMAND make -j${J} -f ${TENSORFLOW_LITE_MICRO_PATH}/tools/make/Makefile ${MAKE_TARGETS_LIST}
        TARGET_TOOLCHAIN_ROOT=${TENSORFLOW_LITE_MICRO_TARGET_TOOLCHAIN_ROOT}
        TOOLCHAIN=${TENSORFLOW_LITE_MICRO_TOOLCHAIN}
        GENDIR=${TENSORFLOW_LITE_MICRO_GENDIR}
        TARGET=${TENSORFLOW_LITE_MICRO_TARGET}
        TARGET_ARCH=${TENSORFLOW_LITE_MICRO_TARGET_ARCH}
        BUILD_TYPE=${TENSORFLOW_LITE_MICRO_BUILD_TYPE}
        ETHOSU_DRIVER_PATH=${ETHOS_U55_DRIVER_SRC_PATH}
        CMSIS_PATH=${CMSIS_SRC_PATH}

        # Conditional arguments
        $<$<BOOL:${ARMCLANG_DEBUG_DWARF_LEVEL}>:ARMCLANG_DEBUG_DWARF_LEVEL=${ARMCLANG_DEBUG_DWARF_LEVEL}>
        $<$<BOOL:${TENSORFLOW_LITE_MICRO_OPTIMIZATION_LEVEL}>:OPTIMIZATION_LEVEL=${TENSORFLOW_LITE_MICRO_OPTIMIZATION_LEVEL}>
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
add_library(tensorflow-lite-micro STATIC IMPORTED)
add_dependencies(tensorflow-lite-micro tensorflow_build)
