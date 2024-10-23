# /* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

# /* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
#  * Use, distribution and modification of this code is permitted under the
#  * terms stated in the Alif Semiconductor Software License Agreement
#  *
#  * You should have received a copy of the Alif Semiconductor Software
#  * License Agreement with this file. If not, please write to:
#  * contact@alifsemi.com, or visit: https://alifsemi.com/license
#  *
#  */

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

#----------------------------------------------------------------------------
# LVGL configuration options
#----------------------------------------------------------------------------
include_guard()

include(util_functions)

message(STATUS "Assessing LVGL configuration options...")

USER_OPTION(LVGL_SRC_PATH
    "Path to LVGL sources"
    "${MLEK_DEPENDENCY_ROOT_DIR}/lvgl"
    PATH)

USER_OPTION(ARM_2D_SRC_PATH
    "Path to Arm-2D sources"
    "${MLEK_DEPENDENCY_ROOT_DIR}/Arm-2D"
    PATH)
