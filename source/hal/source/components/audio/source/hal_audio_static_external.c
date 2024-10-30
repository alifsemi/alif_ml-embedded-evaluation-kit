/*
 * SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "log_macros.h"
#include "hal_audio_static_external.h"

/** These functions must be provided to this interface */
__attribute__((weak)) const char* get_sample_data_filename(uint32_t idx)
{
    UNUSED(idx);
    return NULL;
}

__attribute__((weak)) const int16_t* get_sample_data_ptr(uint32_t idx)
{
    UNUSED(idx);
    return NULL;
}

__attribute__((weak)) uint32_t get_sample_n_elements(void)
{
    return 0;
}

__attribute__((weak)) uint32_t get_sample_data_size(uint32_t idx)
{
    UNUSED(idx);
    return 0;
}

__attribute__((weak)) uint32_t get_sample_sampling_frequency(void)
{
    return 0;
}

__attribute__((weak)) uint32_t get_sample_audio_num_channels(void)
{
    return 0;
}
