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
# specify the cross compiler
set(CMAKE_C_COMPILER                armclang)
set(CMAKE_CXX_COMPILER              armclang)
set(CMAKE_C_LINKER_PREFERENCE       armlink)
set(CMAKE_ASM_LINKER_PREFERENCE     armlink)
set(CMAKE_ASM_COMPILER              armasm)
set(CMAKE_ASM_COMPILER_AR           armar)

set(CMAKE_CROSSCOMPILING            true)
set(CMAKE_SYSTEM_NAME               Generic)

set(MIN_ARM_CLANG_VERSION           6.14)

if (NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR      cortex-m55)
endif()

# Skip compiler test execution
set(CMAKE_C_COMPILER_WORKS          1)
set(CMAKE_CXX_COMPILER_WORKS        1)

set(PLATFORM_HAL                    1)

set(WARNING_OPTS                    "-Wall -Wextra -Wvla")
set(SPECIAL_OPTS                    "-fno-rtti -funsigned-char -fno-function-sections -fno-exceptions")
set(PLATFORM_FLAGS                  "-mthumb --target=arm-arm-non-eabi -mlittle-endian -DPLATFORM_HAL=${PLATFORM_HAL}")

set(CMAKE_C_FLAGS_DEBUG             "-DDEBUG -O0")
set(CMAKE_C_FLAGS_RELEASE           "-DNDEBUG -O3")

set(CMAKE_CXX_FLAGS_DEBUG           "-DDEBUG -O0")
set(CMAKE_CXX_FLAGS_RELEASE         "-DNDEBUG -O3")

if (CMAKE_SYSTEM_PROCESSOR STREQUAL cortex-m55)
    # Flags for cortex-m55
    set(CPU_CORTEX_M55              1)
    set(CPU_CC                      "-mcpu=cortex-m55 -mfloat-abi=hard -MD -DCPU_CORTEX_M55=1 -DARM_MATH_DSP -DARM_MATH_LOOPUNROLL -D__FPU_USED=1")
    set(CPU_LD                      "--cpu=8.1-M.Main.dsp")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL cortex-m33)
    # Flags for cortex-m33 to go here
endif()

set(ALL_COMMON_FLAGS                "${CPU_CC} ${WARNING_OPTS} ${SPECIAL_OPTS} ${PLATFORM_FLAGS}")

function(enforce_compiler_version)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MIN_ARM_CLANG_VERSION})
        message( FATAL_ERROR "Arm compiler version must be ${MIN_ARM_CLANG_VERSION} or greater to support ${CMAKE_SYSTEM_PROCESSOR} architecture." )
    endif()
endfunction()
