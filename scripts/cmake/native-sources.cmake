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
# Set the install prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build_native)
set(PLAT_HAL ${CMAKE_CURRENT_SOURCE_DIR}/source/application/hal/platforms/native)

if (ETHOS_U55_ENABLED)
    message(WARNING "EthosU can't be enabled for native builds."
                    "Use -DETHOS_U55_ENABLED=0 flag for this target platform."
                    "Overriding, disabling use of EthosU...")
    set(ETHOS_U55_ENABLED OFF)
endif()

if (DEFINED LOG_LEVEL)
    message(STATUS "Setting log level to ${LOG_LEVEL}")
    set (LOG_FLAG "-DLOG_LEVEL=${LOG_LEVEL}")
endif()

set(TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME  "libtensorflow-microlite.a")
set(TENSORFLOW_LITE_MICRO_FLAGS "-DTF_LITE_STATIC_MEMORY -DACTIVATION_BUF_SRAM_SZ=0")

if (DEFINED VERIFY_TEST_OUTPUT)
    message(STATUS "Test output verification flag is: ${VERIFY_TEST_OUTPUT}")
    set(PROFILING_OPT "${PROFILING_OPT} -DVERIFY_TEST_OUTPUT=${VERIFY_TEST_OUTPUT}")
endif ()

set(CMAKE_C_FLAGS
        "${PROFILING_OPT} ${LOG_FLAG} ${TENSORFLOW_LITE_MICRO_FLAGS}"
        CACHE INTERNAL "")

set(CMAKE_CXX_FLAGS
        "${PROFILING_OPT} ${LOG_FLAG} ${TENSORFLOW_LITE_MICRO_FLAGS}"
        CACHE INTERNAL "")

# Include directories:
set(PLAT_INCLUDE_DIRS
    ${PLAT_HAL}/utils/include
    ${PLAT_HAL}/images/include
    ${PLAT_HAL}/data_presentation/log/include
    ${PLAT_HAL}/timer/include
    )

# Source files
file(GLOB_RECURSE SRC_PLAT_HAL
    "${PLAT_HAL}/**/*.c"
    "${PLAT_HAL}/**/*.cc"
    )
