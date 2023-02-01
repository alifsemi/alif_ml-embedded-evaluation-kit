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

# Arm-2D library CMake helper script.

# Check if Arm-2D sources have been defined
if (NOT DEFINED ARM_2D_SRC_PATH)
    message(FATAL_ERROR "Arm-2D path should be defined for Arm-2D library to be built")
endif()

# 3. Form a list of all the sources we need in Arm-2D library
set(ARM_2D_PATH_SUFFIX   "Library")
set(ARM_2D_SRC_DIR       "${ARM_2D_SRC_PATH}/${ARM_2D_PATH_SUFFIX}/Source")
set(ARM_2D_INC_DIR       "${ARM_2D_SRC_PATH}/${ARM_2D_PATH_SUFFIX}/Include")

file(GLOB_RECURSE
    ARM_2D_SRC

    "${ARM_2D_SRC_DIR}/*.c")

# 4. Create static library
set(ARM_2D_TARGET        Arm-2D)

add_library(${ARM_2D_TARGET} STATIC ${ARM_2D_SRC})
target_include_directories(${ARM_2D_TARGET} PUBLIC
                           ${ARM_2D_INC_DIR})

target_compile_options(${ARM_2D_TARGET} PRIVATE -ffunction-sections)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    # GCC produces a lot of warnings - silence them
    # (This is public, so will silence all warnings in any Arm-2D user, but
    # only LVGL uses it privately in our tree, so this won't spread further than LVGL)
    target_compile_options(${ARM_2D_TARGET} PUBLIC -w)
endif()

set_property(TARGET ${ARM_2D_TARGET} PROPERTY C_STANDARD 11)
set_property(TARGET ${ARM_2D_TARGET} PROPERTY C_EXTENSIONS ON)

# 5. General compile definitions
target_compile_definitions(${ARM_2D_TARGET} PUBLIC
)

## Add dependencies
target_link_libraries(${ARM_2D_TARGET} PUBLIC
    cmsis-dsp
    rte_components
)
    
# 6. Provide the library path for the top level CMake to use:
set(ARM_2D_LIB   "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${ARM_2D_TARGET}.a")
message(STATUS "ARM_2D_LIB set to be generated here: ${ARM_2D_LIB}")

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${ARM_2D_TARGET})
message(STATUS "Build type                             : " ${CMAKE_BUILD_TYPE})
message(STATUS "TARGET_PLATFORM                        : " ${TARGET_PLATFORM})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "*******************************************************")
