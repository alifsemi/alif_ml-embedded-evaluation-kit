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

# Arm Virtual Streaming Interface (VSI) library CMake helper script.

# Check if VSI driver source path has been defined
if (NOT DEFINED FVP_VSI_SRC_PATH)
    message(FATAL_ERROR "FVP_VSI_SRC_PATH path should be defined for library to be built")
endif()

set(FVP_VSI_TARGET "virtual_streaming_interface")

add_library(${FVP_VSI_TARGET} STATIC
    ${FVP_VSI_SRC_PATH}/interface/video/source/video_drv.c)

target_include_directories(${FVP_VSI_TARGET} PUBLIC
    ${FVP_VSI_SRC_PATH}/interface/include
    ${FVP_VSI_SRC_PATH}/interface/video/include)

target_compile_definitions(${FVP_VSI_TARGET} PRIVATE
    CMSIS_device_header=\"${CPU_HEADER_FILE}\")

# We need IRQ definitions for VSI for each target. This is expected to be in the
# device headers we are configuring the build for. NOTE: this is a cyclic dependency
# as VSI module is expected to be part of platform drivers but we rely on some headers
# from platform driver source tree. But we only need definitions, no functionality.
set(MLEK_TARGET_HDR_ROOT ${MLEK_HAL_PLATFORM_DIR}/${TARGET_PLATFORM}/include/${TARGET_SUBSYSTEM})

if (NOT EXISTS ${MLEK_TARGET_HDR_ROOT})
    message(FATAL_ERROR "Path ${MLEK_TARGET_HDR_ROOT} required for VSI IRQ definitions")
endif()

target_compile_options(${FVP_VSI_TARGET}
    PRIVATE
    "SHELL: -include ${FVP_VSI_SRC_PATH}/interface/include/arm_vsi.h"

    PUBLIC
    "SHELL: -include ${MLEK_TARGET_HDR_ROOT}/peripheral_irqs.h")

target_link_libraries(${FVP_VSI_TARGET} PUBLIC
    rte_components)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                 : " ${FVP_VSI_TARGET})
message(STATUS "TARGET_PLATFORM         : " ${TARGET_PLATFORM})
message(STATUS "CMAKE_SYSTEM_PROCESSOR  : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
