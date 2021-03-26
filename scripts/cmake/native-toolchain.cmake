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
set(CMAKE_CXX_COMPILER          g++)
set(CMAKE_C_COMPILER            gcc)
set(CMAKE_C_LINKER_PREFERENCE   gcc)
set(CMAKE_CXX_LINKER_PREFERENCE gcc)

set(CMAKE_C_FLAGS_DEBUG         "-DDEBUG -O0 -g")
set(CMAKE_C_FLAGS_RELEASE       "-DNDEBUG -O3")

set(CMAKE_CXX_FLAGS_DEBUG       "-DDEBUG -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE     "-DNDEBUG -O3")

# Platform specific directory:
set(PLATFORM_HAL                3)
set(WARNING_FLAGS               "-Wsign-compare -Wshadow         \
                                 -Wextra -Wall -Wunused-function \
                                 -Wmissing-field-initializers    \
                                 -Wswitch -Wvla -Wunused-parameter")
set(SPECIAL_OPTS                "-fPIC -pthread")
set(PLATFORM_FLAGS              "-DPLATFORM_HAL=${PLATFORM_HAL}")
set(SPECIAL_OPTS_CXX            "-fno-threadsafe-statics")
set(CMAKE_EXE_LINKER_FLAGS      "-lm -lc -lstdc++ --verbose")

function(enforce_compiler_version)
endfunction()
