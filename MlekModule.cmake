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

# For any CMake project wrapping ml-embedded-eval-kit, it should include this before
# to get the default directory settings and set up the environment.
include_guard(GLOBAL)

# Variables used by all other scripts
set(MLEK_SCRIPTS_DIR         ${CMAKE_CURRENT_LIST_DIR}/scripts)
set(MLEK_DEPENDENCY_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/dependencies)
set(MLEK_RESOURCES_DIR       ${CMAKE_CURRENT_LIST_DIR}/resources_downloaded)
set(MLEK_HAL_PLATFORM_DIR    ${CMAKE_CURRENT_LIST_DIR}/source/hal/source/platform)
set(MLEK_DOWNLOAD_DEP_DIR    ${CMAKE_BINARY_DIR}/dependencies)
set(MLEK_CMAKE_TOOLCHAIN_DIR ${MLEK_SCRIPTS_DIR}/cmake/toolchains)
set(MLEK_CORE_PLATFORM_DIR   ${MLEK_DEPENDENCY_ROOT_DIR}/core-platform)

# The list of directories where platform build configurations are searched for.
# Any project including this file, should append to this list before inclusion
# to allow searching additional directories for target configurations.
list(APPEND MLEK_PLATFORM_BUILD_CONFIG_DIRS ${MLEK_SCRIPTS_DIR}/cmake/platforms)

# Append to CMAKE_MODULE_PATH to allow top level scripts to find and include
# utility functions.
list(APPEND CMAKE_MODULE_PATH
    "${MLEK_SCRIPTS_DIR}/cmake"
    "${MLEK_SCRIPTS_DIR}/cmake/configuration_options"
    "${MLEK_SCRIPTS_DIR}/cmake/timing_adapter")

# Include source generator.
include(source_gen_utils)

# Enable testing - allows top level to add tests.
enable_testing()

# Check that the source and build directories are different.
if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(FATAL_ERROR "Source and build are in the same directory")
else()
    message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
    message(STATUS "Binary directory: ${CMAKE_BINARY_DIR}")
endif()

# Include common configuration options.
include(common_opts)

# Check if the resources_downloaded needs update.
check_update_public_resources(${MLEK_RESOURCES_DIR})

# Find the platform's build configuration and then add it.
add_platform_build_configuration(TARGET_PLATFORM ${TARGET_PLATFORM})
set_platform_global_defaults()

message(STATUS "Using CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

# Make sure the following options are defined before proceeding.
assert_defined(LOG_LEVEL)
assert_defined(TENSORFLOW_SRC_PATH)
assert_defined(TARGET_PLATFORM)
assert_defined(USE_CASE_BUILD)
assert_defined(CPU_PROFILE_ENABLED)
assert_defined(CMAKE_TOOLCHAIN_FILE)

# Explicit policy definitions.
if(POLICY CMP0123)
    cmake_policy(SET CMP0123 NEW)
endif()
