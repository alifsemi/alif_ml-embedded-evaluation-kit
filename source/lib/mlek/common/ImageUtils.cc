/*
 * SPDX-FileCopyrightText: Copyright 2022, 2025-2026 Arm Limited and/or its
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
#include "mlek/common/ImageUtils.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace arm {
namespace app {
    namespace image {
        namespace {

            /*
             * Centered INT8 quantization cannot represent both -1 and 1 exactly at the
             * endpoints for common scale/offset values such as 2/255 and -1. Allow one
             * INT8 endpoint step when detecting whether a tensor covers [-1, 1].
             */
            constexpr float kQuantizationRangeTolerance = 1.0F / 128.0F;

            struct Uint8ToInt8Conversion {
                float multiplier;
                float offset;
            };

            int8_t ClampInt8(const int32_t value)
            {
                return static_cast<int8_t>(std::max<int32_t>(
                    std::numeric_limits<int8_t>::min(),
                    std::min<int32_t>(std::numeric_limits<int8_t>::max(), value)));
            }

            int8_t ConvertUint8PixelToCenteredInt8(const uint8_t value)
            { return static_cast<int8_t>(static_cast<int32_t>(value) - 128); }

            int8_t QuantizeUint8PixelLinear(const uint8_t value,
                                            const Uint8ToInt8Conversion& conversion)
            {
                const auto quantized = static_cast<int32_t>(std::lround(
                    static_cast<float>(value) * conversion.multiplier + conversion.offset));
                return ClampInt8(quantized);
            }

            bool IsApproximately(const float lhs, const float rhs)
            { return std::fabs(lhs - rhs) <= kQuantizationRangeTolerance; }

            bool IsCenteredUint8ToInt8Quantization(const fwk::iface::QuantParams& quantParams)
            {
                const float minReal =
                    quantParams.scale * (static_cast<float>(std::numeric_limits<int8_t>::min()) -
                                         static_cast<float>(quantParams.offset));
                const float maxReal =
                    quantParams.scale * (static_cast<float>(std::numeric_limits<int8_t>::max()) -
                                         static_cast<float>(quantParams.offset));

                return IsApproximately(minReal, -1.0F) && IsApproximately(maxReal, 1.0F);
            }

            Uint8ToInt8Conversion
            GetUint8ToInt8Conversion(const fwk::iface::QuantParams& quantParams)
            {
                return Uint8ToInt8Conversion{1.0F / (255.0F * quantParams.scale),
                                             static_cast<float>(quantParams.offset)};
            }

            void ConvertUint8ToInt8Linear(void* data,
                                          const size_t kMaxImageSize,
                                          const Uint8ToInt8Conversion& conversion)
            {
                auto* tmp_req_data        = static_cast<uint8_t*>(data);
                auto* tmp_signed_req_data = static_cast<int8_t*>(data);

                for (size_t i = 0; i < kMaxImageSize; i++) {
                    tmp_signed_req_data[i] = QuantizeUint8PixelLinear(tmp_req_data[i], conversion);
                }
            }

            void ConvertUint8ToInt8LinearWithLayout(int8_t* dst,
                                                    const uint8_t* src,
                                                    const size_t nElem,
                                                    fwk::iface::TensorLayout layout,
                                                    const Uint8ToInt8Conversion& conversion)
            {
                constexpr size_t nChannels = 3;
                const size_t imgArraySz    = nElem / nChannels;

                if (layout == fwk::iface::TensorLayout::NCHW) {
                    for (size_t i = 0; i < imgArraySz; i++) {
                        for (size_t j = 0; j < nChannels; ++j) {
                            dst[(j * imgArraySz) + i] =
                                QuantizeUint8PixelLinear(src[i * nChannels + j], conversion);
                        }
                    }
                } else {
                    for (size_t i = 0; i < nElem; ++i) {
                        dst[i] = QuantizeUint8PixelLinear(src[i], conversion);
                    }
                }
            }

        } /* namespace */

        float Calculate1DOverlap(float x1Center, float width1, float x2Center, float width2)
        {
            float left_1  = x1Center - width1 / 2;
            float left_2  = x2Center - width2 / 2;
            float leftest = left_1 > left_2 ? left_1 : left_2;

            float right_1  = x1Center + width1 / 2;
            float right_2  = x2Center + width2 / 2;
            float rightest = right_1 < right_2 ? right_1 : right_2;

            return rightest - leftest;
        }

        float CalculateBoxIntersect(Box& box1, Box& box2)
        {
            float width = Calculate1DOverlap(box1.x, box1.w, box2.x, box2.w);
            if (width < 0) {
                return 0;
            }
            float height = Calculate1DOverlap(box1.y, box1.h, box2.y, box2.h);
            if (height < 0) {
                return 0;
            }

            float total_area = width * height;
            return total_area;
        }

        float CalculateBoxUnion(Box& box1, Box& box2)
        {
            float boxes_intersection = CalculateBoxIntersect(box1, box2);
            float boxes_union        = box1.w * box1.h + box2.w * box2.h - boxes_intersection;
            return boxes_union;
        }

        float CalculateBoxIOU(Box& box1, Box& box2)
        {
            float boxes_intersection = CalculateBoxIntersect(box1, box2);
            if (boxes_intersection == 0) {
                return 0;
            }

            float boxes_union = CalculateBoxUnion(box1, box2);
            if (boxes_union == 0) {
                return 0;
            }

            return boxes_intersection / boxes_union;
        }

        void CalculateNMS(std::forward_list<Detection>& detections, int classes, float iouThreshold)
        {
            int idxClass{0};
            auto CompareProbs = [idxClass](Detection& prob1, Detection& prob2) {
                return prob1.prob[idxClass] > prob2.prob[idxClass];
            };

            for (idxClass = 0; idxClass < classes; ++idxClass) {
                detections.sort(CompareProbs);

                for (auto it = detections.begin(); it != detections.end(); ++it) {
                    if (it->prob[idxClass] == 0)
                        continue;
                    for (auto itc = std::next(it, 1); itc != detections.end(); ++itc) {
                        if (itc->prob[idxClass] == 0) {
                            continue;
                        }
                        if (CalculateBoxIOU(it->bbox, itc->bbox) > iouThreshold) {
                            itc->prob[idxClass] = 0;
                        }
                    }
                }
            }
        }

        void ConvertUint8ToInt8(void* data, const size_t kMaxImageSize)
        {
            auto* tmp_req_data        = static_cast<uint8_t*>(data);
            auto* tmp_signed_req_data = static_cast<int8_t*>(data);

            for (size_t i = 0; i < kMaxImageSize; i++) {
                tmp_signed_req_data[i] = ConvertUint8PixelToCenteredInt8(tmp_req_data[i]);
            }
        }

        void ConvertUint8ToInt8(void* data,
                                const size_t kMaxImageSize,
                                const fwk::iface::QuantParams& quantParams)
        {
            if (quantParams.scale <= 0.0F) {
                ConvertUint8ToInt8(data, kMaxImageSize);
                return;
            }

            if (IsCenteredUint8ToInt8Quantization(quantParams)) {
                ConvertUint8ToInt8(data, kMaxImageSize);
                return;
            }

            ConvertUint8ToInt8Linear(data, kMaxImageSize, GetUint8ToInt8Conversion(quantParams));
        }

        void ConvertUint8ToInt8(int8_t* dst,
                                const uint8_t* src,
                                const size_t nElem,
                                fwk::iface::TensorLayout layout)
        {
            constexpr size_t nChannels = 3;
            const size_t imgArraySz    = nElem / nChannels;

            if (layout == fwk::iface::TensorLayout::NCHW) {
                for (size_t i = 0; i < imgArraySz; i++) {
                    for (size_t j = 0; j < nChannels; ++j) {
                        dst[(j * imgArraySz) + i] =
                            ConvertUint8PixelToCenteredInt8(src[i * nChannels + j]);
                    }
                }
            } else {
                for (size_t i = 0; i < nElem; ++i) {
                    dst[i] = ConvertUint8PixelToCenteredInt8(src[i]);
                }
            }
        }

        void ConvertUint8ToInt8(int8_t* dst,
                                const uint8_t* src,
                                const size_t nElem,
                                fwk::iface::TensorLayout layout,
                                const fwk::iface::QuantParams& quantParams)
        {
            if (quantParams.scale <= 0.0F) {
                ConvertUint8ToInt8(dst, src, nElem, layout);
                return;
            }

            if (IsCenteredUint8ToInt8Quantization(quantParams)) {
                ConvertUint8ToInt8(dst, src, nElem, layout);
                return;
            }

            ConvertUint8ToInt8LinearWithLayout(
                dst, src, nElem, layout, GetUint8ToInt8Conversion(quantParams));
        }

        void RgbToGrayscale(const uint8_t* srcPtr, uint8_t* dstPtr, const size_t dstImgSz)
        {
            const float R              = 0.299;
            const float G              = 0.587;
            const float B              = 0.114;
            constexpr size_t nChannels = 3;
            for (size_t i = 0; i < dstImgSz; ++i, srcPtr += nChannels) {
                uint32_t int_gray = R * (*srcPtr) + G * (*(srcPtr + 1)) + B * (*(srcPtr + 2));
                *dstPtr++         = int_gray <= std::numeric_limits<uint8_t>::max()
                                        ? int_gray
                                        : std::numeric_limits<uint8_t>::max();
            }
        }

    } /* namespace image */
} /* namespace app */
} /* namespace arm */
