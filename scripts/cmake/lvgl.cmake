# This file was ported to work on Alif Semiconductor Ensemble family of devices.

#  Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

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

# LVGL library CMake helper script.

# Check if LVGL sources have been defined
if (NOT DEFINED LVGL_SRC_PATH)
    message(FATAL_ERROR "LVGL path should be defined for LVGL library to be built")
endif()

# 4. Create static library
set(LVGL_TARGET       lvgl)

# It's left to the person including this (a platform?) to set LV_CONF_PATH
# appropriately, or add it to our CMake include paths eg:
# set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/lv_conf.h CACHE STRING "" FORCE)
# OR
# target_include_directories(${LVGL_TARGET} PUBLIC include)

# Include the subdirectory - we can use LVGL's own CMake set-up
add_subdirectory(${LVGL_SRC_PATH} ${CMAKE_BINARY_DIR}/lvgl)

# If LVGL is configured to use Arm-2D, it's up to the caller to
# include Arm-2D.make and make it a link dependency for LVGL, eg:
# target_link_libraries(${LVGL_TARGET} PRIVATE ${ARM_2D_TARGET})

# In case LVGL is using Arm-2D
set_property(TARGET ${LVGL_TARGET} PROPERTY C_STANDARD 11)
set_property(TARGET ${LVGL_TARGET} PROPERTY C_EXTENSIONS ON)

# 6. Provide the library path for the top level CMake to use:
set(LVGL_LIB   "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${LVGL_TARGET}.a")
message(STATUS "LVGL_LIB set to be generated here: ${LVGL_LIB}")

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${LVGL_LIB})
message(STATUS "Build type                             : " ${CMAKE_BUILD_TYPE})
message(STATUS "TARGET_PLATFORM                        : " ${TARGET_PLATFORM})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
