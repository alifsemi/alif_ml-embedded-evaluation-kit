#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
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

# Add individual use case APIs but don't include them in default build
add_subdirectory(ad EXCLUDE_FROM_ALL)
add_subdirectory(asr EXCLUDE_FROM_ALL)
add_subdirectory(img_class EXCLUDE_FROM_ALL)
add_subdirectory(inference_runner EXCLUDE_FROM_ALL)
add_subdirectory(kws EXCLUDE_FROM_ALL)
add_subdirectory(noise_reduction EXCLUDE_FROM_ALL)
add_subdirectory(object_detection EXCLUDE_FROM_ALL)
add_subdirectory(vww EXCLUDE_FROM_ALL)
