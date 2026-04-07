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
#ifndef MLEK_AUDIO_RENDER_UTILS_HPP
#define MLEK_AUDIO_RENDER_UTILS_HPP

#include "mlek/fwk/iface/Tensor.hpp"
#include <cstdint>
#include <memory>

namespace arm::app::audio {

using Tensor = std::shared_ptr<arm::app::fwk::iface::TensorIface>;

/**
 * @brief   Render the mel spectrogram into an image buffer.
 * @param[in] melTensor      The mel spectrogram tensor.
 * @param[in] melHeight      Number of rows in MEL data.
 * @param[in] melWidth       Number of columns in MEL data.
 * @parma[in] imageBuffer    The image buffer to render the mel spectrogram into.
 * @param[in] imageWidth     The width of the image buffer.
 * @param[in] imageHeight    The height of the image buffer.
 * @param[in] imageChannels  Number of channels in the image (defaults to 3
 *                           expecting RGB24).
 * @param[in] transpose      Indicates whether the image buffer should be
 *                           transposed relative to the MEL data. Defaults
 *                           to false.
 * @return  true if the mel spectrogram was rendered successfully, false otherwise.
 */
bool RenderMelSpec(Tensor melTensor,
                   uint32_t melWidth,
                   uint32_t melHeight,
                   uint8_t* imageBuffer,
                   uint32_t imageWidth,
                   uint32_t imageHeight,
                   uint32_t imageChannels = 3,
                   bool transpose         = false);
/**
 * @brief   Render the mel spectrogram into an image buffer.
 * @param[in] audioData      Pointer to 16-bit PCM audio data.
 * @param[in] audioNumElements   Number of elements in the audio array.
 * @param[in] audioNumElementsToPlot Total number of audio elements to plot.
 *                                   This can be more than the available elements
 *                                   and is used to decide horizontal scaling
 *                                   of the plot.
 * @parma[in] imageBuffer    The image buffer to render the mel spectrogram into.
 * @param[in] imageWidth     The width of the image buffer.
 * @param[in] imageHeight    The height of the image buffer.
 * @param[in] imageChannels  Number of channels in the image (defaults to 3
 *                           expecting RGB24).
 * @return  true if the audio signal was rendered successfully, false otherwise.
 */
bool RenderAudioImg(const int16_t* audioData,
                    uint32_t audioNumElements,
                    uint32_t audioNumElementsToPlot,
                    uint8_t* imageBuffer,
                    uint32_t imageWidth,
                    uint32_t imageHeight,
                    uint32_t imageChannels);
} /* namespace arm::app::audio */

#endif /* MLEK_AUDIO_RENDER_UTILS_HPP */
