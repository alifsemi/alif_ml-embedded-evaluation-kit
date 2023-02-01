#----------------------------------------------------------------------------
#  Copyright (c) 2022 Arm Limited. All rights reserved.
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

#########################################################
#           Ensemble Services lib                       #
#########################################################

cmake_minimum_required(VERSION 3.16.3)
set(ENSEMBLE_SERVICES_TARGET ensemble_services)

project(${ENSEMBLE_SERVICES_TARGET}
    DESCRIPTION     "Ensemble Services library for Ensemble RTSS-HP/RTSS-HE targets"
    LANGUAGES       C CXX ASM)

# 1. We should be cross-compiling (Ensemble target only runs Cortex-M/A targets)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${ENSEMBLE_SERVICES_TARGET} support for this target.")
endif()

# 2. Create static library
add_library(${ENSEMBLE_SERVICES_TARGET} STATIC)

## Include directories - private
target_include_directories(${ENSEMBLE_SERVICES_TARGET}
    PRIVATE
    source
    drivers/src)

## Include directories - public
target_include_directories(${ENSEMBLE_SERVICES_TARGET}
    PUBLIC
    drivers/include
    include
    )

## Platform sources
target_sources(${ENSEMBLE_SERVICES_TARGET}
    PRIVATE
    drivers/src/mhu_driver.c
    drivers/src/mhu_receiver.c
    drivers/src/mhu_sender.c
    services_lib/services_host_application.c
    services_lib/services_host_boot.c
    services_lib/services_host_cryptocell.c
    services_lib/services_host_error.c
    services_lib/services_host_handler.c
    services_lib/services_host_maintenance.c
    services_lib/services_host_padcontrol.c
    services_lib/services_host_pinmux.c
    services_lib/services_host_power.c
    services_lib/services_host_system.c
    services_lib/services_lib_interface.c
    services_lib/services_main.c
    )

# Add dependencies:
target_link_libraries(${ENSEMBLE_SERVICES_TARGET} PUBLIC
    rte_components
    platform_drivers_if
)

# 3. Display status:
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${ENSEMBLE_SERVICES_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_SYSTEM_ARCH                      : " ${CMAKE_SYSTEM_ARCH})
message(STATUS "*******************************************************")
