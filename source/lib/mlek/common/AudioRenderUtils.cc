/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
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
#include "mlek/common/AudioRenderUtils.hpp"
#include "mlek/log/log_macros.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace arm::app::audio {

union Colour {
    uint32_t value;
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } components;
};

/**
 * @brief       Jet colormap interpolation
 * @param[in]   v   Normalised floating point value; range [0,1].
 * @return      Colour object with RGB values.
 */
static Colour JetColourMap(float v)
{
    Colour colour{};
    const float fourValue = 4.0f * v;
    colour.components.r   = static_cast<uint8_t>(
        255 * std::clamp(std::min(fourValue - 1.5f, -fourValue + 4.5f), 0.0f, 1.0f));
    colour.components.g = static_cast<uint8_t>(
        255 * std::clamp(std::min(fourValue - 0.5f, -fourValue + 3.5f), 0.0f, 1.0f));
    colour.components.b = static_cast<uint8_t>(
        255 * std::clamp(std::min(fourValue + 0.5f, -fourValue + 2.5f), 0.0f, 1.0f));
    return colour;
}

bool RenderMelSpec(Tensor melTensor,
                   uint32_t melWidth,
                   uint32_t melHeight,
                   uint8_t* imageBuffer,
                   uint32_t imageWidth,
                   uint32_t imageHeight,
                   uint32_t imageChannels,
                   bool transpose)
{
    /** Basic validation of input parameters */
    if (!melTensor || !imageBuffer) {
        return false;
    }

    if (imageWidth == 0 || imageHeight == 0) {
        return false;
    }

    if (imageChannels != 3) {
        printf_err("Currently only 3 channel images are supported\n");
        return false;
    }

    auto tensorShape    = melTensor->Shape();
    auto* melData       = melTensor->GetData<float>();
    auto numMelElements = melTensor->GetNumElements();

    const auto melRows = melHeight;
    const auto melCols = melWidth;
    info("MelRows: %" PRIu32 "; MelCols: %" PRIu32 "\n", melRows, melCols);

    if ((melRows * melCols) > numMelElements) {
        printf_err("Invalid size attr for MEL spec.\n");
        return false;
    }

    /* We could only be plotting a initial part of MEL.
     * Limit traversing to the required size. */
    numMelElements = melRows * melCols;
    debug("Number of MEL elements: %zu\n", numMelElements);

    /** Tensor shape should be more than 3 */
    if (tensorShape.size() < 3) {
        printf_err("Invalid tensor shape.\n");
        return false;
    }

    /* Find the min and max from mel data */
    auto minMax = std::minmax_element(melData, melData + numMelElements);
    if (*minMax.second - *minMax.first == 0.f) {
        printf_err("Mel spec is empty?.\n");
        return false;
    }

    uint32_t scaleX = transpose ? melRows / imageWidth : melCols / imageWidth;
    uint32_t scaleY = transpose ? melCols / imageHeight : melRows / imageHeight;

    scaleX = scaleX == 0 ? 1 : scaleX;
    scaleY = scaleY == 0 ? 1 : scaleY;
    debug("ScaleX: %" PRIu32 "; ScaleY: %" PRIu32 "\n", scaleX, scaleY);

    auto minVal = *minMax.first;
    auto maxVal = *minMax.second;
    info("Mel min: %f; max: %f\n", minVal, maxVal);

    auto imageStride = imageWidth * imageChannels;

    /* Don't need to transpose, mapping is more straight forward. */
    if (!transpose) {
        uint8_t* dst = imageBuffer;
        for (uint32_t melR = 0, imgR = 0; melR < melRows && imgR < imageHeight;
             melR += scaleY, ++imgR) {
            float* src = melData + (melR * melCols);
            for (uint32_t melC = 0, imgC = 0; melC < melCols && imgC < imageWidth;
                 melC += scaleX, ++imgC) {
                auto normalisedVal = (*src - minVal) / (maxVal - minVal);
                auto clr           = JetColourMap(normalisedVal);
                *dst++             = clr.components.r;
                *dst++             = clr.components.g;
                *dst++             = clr.components.b;
                src += scaleX;
            }
        }
    } else {
        for (uint32_t melR = 0, imgC = 0; melR < melRows && imgC < imageWidth;
             melR += scaleX, ++imgC) {
            uint8_t* dst = imageBuffer + (imgC * imageChannels);
            float* src   = melData + (melR * melCols);
            for (uint32_t melC = 0, imgR = 0; melC < melCols && imgR < imageWidth;
                 melC += scaleY, ++imgR) {
                auto normalisedVal = (*src - minVal) / (maxVal - minVal);
                auto clr           = JetColourMap(normalisedVal);
                *dst               = clr.components.r;
                *(dst + 1)         = clr.components.g;
                *(dst + 2)         = clr.components.b;
                dst += imageStride;
                src += scaleY;
            }
        }
    }

    return true;
}

bool RenderAudioImg(const int16_t* audioData,
                    uint32_t audioNumElements,
                    uint32_t audioNumElementsToPlot,
                    uint8_t* imageBuffer,
                    uint32_t imageWidth,
                    uint32_t imageHeight,
                    uint32_t imageChannels)
{
    constexpr int32_t audioSampling = 1; /* plot one in X elements. */

    /* Find the min and max from audio data */
    auto minMax = std::minmax_element(audioData, audioData + audioNumElements - 1);
    memset(imageBuffer, 0, imageHeight * imageWidth * imageChannels);
    if (*minMax.second - *minMax.first == 0) {
        printf_err("Audio data is empty?.\n");
        return false;
    }
    auto audioRange = static_cast<int>(*minMax.second) - static_cast<int>(*minMax.first);

    debug("Audio min %" PRId16 "; max: %" PRId16 "\n", *minMax.first, *minMax.second);
    debug("Audio signal range: %d\n", audioRange);
    auto scaleMultiplier = static_cast<float>(imageHeight) / static_cast<float>(audioRange);

    const uint32_t audioStride = std::ceil<uint32_t>(static_cast<float>(audioNumElementsToPlot) /
                                                     static_cast<float>(imageWidth));
    const uint32_t imageStride = imageWidth * imageChannels;

    if (audioSampling > audioStride) {
        printf_err("Audio scaling stride > sampling stride\n");
        return false;
    }

    /* Draw the zero point */
    uint8_t* midColPtr = imageBuffer + (imageHeight / 2 * imageStride);
    for (uint32_t imgC = 0; imgC < imageWidth; ++imgC, midColPtr += imageChannels) {
        *midColPtr = 255;
    }

    auto GetIdx = [&](int16_t val) {
        int32_t idx =
            imageHeight -
            static_cast<int32_t>(std::ceil<int32_t>(static_cast<float>(val) * scaleMultiplier) +
                                 static_cast<int32_t>(imageHeight / 2));
        idx = std::clamp(idx, static_cast<int32_t>(0), static_cast<int32_t>(imageHeight - 1));
        return idx;
    };

    /* Plot the signal */
    for (uint32_t i = 0, imgC = 0; i < audioNumElements && imgC < imageWidth; i += audioSampling) {
        auto rowIdx                                                  = GetIdx(audioData[i]);
        imageBuffer[(rowIdx * imageStride) + (imgC * imageChannels)] = 255;
        if (i % audioStride == 0) {
            ++imgC;
        }
    }

    return true;
}

} /* namespace arm::app::audio */
