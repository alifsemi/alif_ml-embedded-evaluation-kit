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

# Warning compiler definitions:
add_compile_options(
    -Wsign-compare
    -Wshadow
    -Wextra
    -Wall
    -Wunused-function
    -Wmissing-field-initializers
    -Wswitch
    -Wvla
    -Wunused-parameter)

# General purpose compile definitions:
add_compile_options(
    -fPIC
    -pthread
    "$<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>")

# Linker options
add_link_options(
    -lm
    -lc
    -lstdc++
    --verbose)

function(enforce_compiler_version)
endfunction()
