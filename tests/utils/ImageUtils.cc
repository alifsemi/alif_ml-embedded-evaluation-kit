/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
#include "ImageUtils.hpp"

void convertImgIoInt8(void * data, const size_t sz)
{
    uint8_t * tmp_req_data          = (uint8_t *)data;
    int8_t * tmp_signed_req_data    = (int8_t *) data;

    for (size_t i = 0; i < sz; ++i) {
        tmp_signed_req_data[i] = (int8_t)(
                (int32_t)(tmp_req_data[i]) - 128);
    }
}

void convertImgIoGreyscale(const uint8_t * srcPtr, uint8_t * dstPtr, const size_t sz)
{
    for (size_t i = 0; i < sz; ++i, srcPtr += 3) {
        *dstPtr++ = 0.2989 * (*srcPtr) +
                    0.587 * (*(srcPtr+1)) +
                    0.114 * (*(srcPtr+2));
    }
}