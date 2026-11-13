/*
 * SPDX-FileCopyrightText: Copyright 2022, 2024-2026 Arm Limited and/or
 * its affiliates <open-source-office@arm.com>
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
#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILS_HPP

#include "mlek/fwk/iface/Tensor.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <forward_list>
#include <vector>

/* Helper macro to convert RGB888 to RGB565 format. */
#define RGB888_TO_RGB565(R8, G8, B8) \
    ((((R8 >> 3) & 0x1F) << 11) | (((G8 >> 2) & 0x3F) << 5) | ((B8 >> 3) & 0x1F))

constexpr uint16_t COLOR_BLACK  = 0;
constexpr uint16_t COLOR_GREEN  = RGB888_TO_RGB565(0, 255, 0);
constexpr uint16_t COLOR_YELLOW = RGB888_TO_RGB565(255, 255, 0);
constexpr uint16_t COLOR_WHITE  = RGB888_TO_RGB565(255, 255, 255);
constexpr uint16_t COLOR_PURPLE = RGB888_TO_RGB565(128, 0, 128);
constexpr uint16_t COLOR_BLUE   = RGB888_TO_RGB565(0, 0, 255);
constexpr uint16_t COLOR_MAROON = RGB888_TO_RGB565(128, 0, 0);
constexpr uint16_t COLOR_DGREEN = RGB888_TO_RGB565(0, 128, 0);
constexpr uint16_t COLOR_DBLUE  = RGB888_TO_RGB565(0, 0, 128);

namespace arm {
namespace app {
    namespace image {

        /**
         * Contains the x,y co-ordinates of a box centre along with the box width and height.
         */
        struct Box {
            float x;
            float y;
            float w;
            float h;
        };

        struct Detection {
            Box bbox;
            std::vector<float> prob;
            float objectness;
        };

        /**
         * @brief       Calculate the 1D overlap.
         * @param[in]   x1Center   First center point.
         * @param[in]   width1     First width.
         * @param[in]   x2Center   Second center point.
         * @param[in]   width2     Second width.
         * @return      The overlap between the two lines.
         **/
        float Calculate1DOverlap(float x1Center, float width1, float x2Center, float width2);

        /**
         * @brief       Calculate the intersection between the two given boxes.
         * @param[in]   box1   First box.
         * @param[in]   box2   Second box.
         * @return      The intersection value.
         **/
        float CalculateBoxIntersect(Box& box1, Box& box2);

        /**
         * @brief       Calculate the union between the two given boxes.
         * @param[in]   box1   First box.
         * @param[in]   box2   Second box.
         * @return      The two given boxes union value.
         **/
        float CalculateBoxUnion(Box& box1, Box& box2);

        /**
         * @brief       Calculate the intersection over union between the two given boxes.
         * @param[in]   box1   First box.
         * @param[in]   box2   Second box.
         * @return      The intersection over union value.
         **/
        float CalculateBoxIOU(Box& box1, Box& box2);

        /**
         * @brief       Calculate the Non-Maxima suppression on the given detection boxes.
         * @param[in]   detections    List of Detection boxes.
         * @param[in]   classes       Number of classes.
         * @param[in]   iouThreshold  Intersection over union threshold.
         **/
        void
        CalculateNMS(std::forward_list<Detection>& detections, int classes, float iouThreshold);

        /**
         * @brief           Helper function to convert a UINT8 image to INT8 format.
         * @param[in,out]   data    Pointer to the data start.
         * @param[in]       nElem   Total number of pixels in the image.
         * @note            Performance in-place transformation.
         *
         **/
        void ConvertUint8ToInt8(void* data, size_t nElem);

        /**
         * @brief           Helper function to convert a UINT8 image to INT8 format.
         * @param[in,out]   data           Pointer to the data start.
         * @param[in]       nElem          Total number of pixels in the image.
         * @param[in]       quantParams    Destination tensor quantization parameters.
         * @note            Performance in-place transformation.
         *
         **/
        void
        ConvertUint8ToInt8(void* data, size_t nElem, const fwk::iface::QuantParams& quantParams);

        /**
         * @brief           Helper function to convert a UINT8 image to INT8 format.
         * @param[out] dst      Destination buffer.
         * @param[in]  src      Source pointer.
         * @param[in]  nElem    Number of elements to be copied over.
         * @param[in]  layout   Data layout for destination.
         **/
        void ConvertUint8ToInt8(int8_t* dst,
                                const uint8_t* const src,
                                size_t nElem,
                                fwk::iface::TensorLayout layout);

        /**
         * @brief           Helper function to convert a UINT8 image to INT8 format.
         * @param[out] dst          Destination buffer.
         * @param[in]  src          Source pointer.
         * @param[in]  nElem        Number of elements to be copied over.
         * @param[in]  layout       Data layout for destination.
         * @param[in]  quantParams  Destination tensor quantization parameters.
         **/
        void ConvertUint8ToInt8(int8_t* dst,
                                const uint8_t* const src,
                                size_t nElem,
                                fwk::iface::TensorLayout layout,
                                const fwk::iface::QuantParams& quantParams);

        /**
         * @brief           Function to convert unsigned 8-bit src image to
         *                  floating point.
         * @tparam     numChannels  Number of channels in the image
         * @param[out] dst          Destination buffer.
         * @param[in]  src          Source pointer.
         * @param[in]  nElem        Number of elements to be copied over.
         * @param[in]  layout       Data layout for destination.
         * @param[in]  mean         Normalisation mean for each channel.
         * @param[in]  stdDev       Normalisation standard deviation for each channel.
         * @return     true if conversion succeeds, false otherwise.
         */
        template <std::size_t numChannels>
        bool ConvertUint8ToFp32(float* dst,
                                const uint8_t* src,
                                const size_t nElem,
                                fwk::iface::TensorLayout layout,
                                const std::array<float, numChannels>& mean,
                                const std::array<float, numChannels>& stdDev)
        {
            const size_t imgArraySz = nElem / numChannels;

            auto Normalise = [](const uint8_t val, float meanVal, float stdVal) {
                return ((static_cast<float>(val) / 255.f) - meanVal) / stdVal;
            };

            if (layout == fwk::iface::TensorLayout::NCHW) {
                for (size_t i = 0; i < imgArraySz; i++) {
                    for (size_t j = 0; j < numChannels; ++j) {
                        dst[(j * imgArraySz) + i] =
                            Normalise(src[i * numChannels + j], mean[j], stdDev[j]);
                    }
                }
            } else if (layout == fwk::iface::TensorLayout::NHWC) {
                for (size_t i = 0; i < nElem; i += numChannels) {
                    for (size_t n = 0; n < numChannels; ++n) {
                        dst[i + n] = Normalise(src[i + n], mean[n], stdDev[n]);
                    }
                }
            } else {
                return false;
            }

            return true;
        }

        /**
         * @brief       Converts RGB image to grayscale.
         * @param[in]   srcPtr   Pointer to RGB source image.
         * @param[out]  dstPtr   Pointer to grayscale destination image.
         * @param[in]   imgSz    Destination image size.
         **/
        void RgbToGrayscale(const uint8_t* srcPtr, uint8_t* dstPtr, size_t dstImgSz);

    } /* namespace image */
} /* namespace app */
} /* namespace arm */

#endif /* IMAGE_UTILS_HPP */
