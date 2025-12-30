/*
 * SPDX-FileCopyrightText: Copyright 2022, 2025 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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
#include "mlek/use_case/ad/AdProcessing.hpp"

#include <functional>

namespace arm {
namespace app {

    AdPreProcess::AdPreProcess(const std::shared_ptr<fwk::iface::TensorIface> inputTensor,
                               uint32_t numRowsIdx,
                               uint32_t numColsIdx,
                               uint32_t melSpectrogramFrameLen,
                               uint32_t melSpectrogramFrameStride,
                               float adModelTrainingMean) :
        m_validInstance{false}, m_melSpectrogramFrameLen{melSpectrogramFrameLen},
        m_melSpectrogramFrameStride{melSpectrogramFrameStride},
        /**< Model is trained on features downsampled 2x */
        m_inputResizeScale{2},
        /**< We are choosing to move by 20 frames across the audio for each inference. */
        m_numMelSpecVectorsInAudioStride{20},
        m_audioDataStride{m_numMelSpecVectorsInAudioStride * melSpectrogramFrameStride},
        m_melSpec{melSpectrogramFrameLen}
    {
        UNUSED(this->m_melSpectrogramFrameStride);

        if (!inputTensor) {
            printf_err("Invalid input tensor provided to pre-process\n");
            return;
        }

        auto inputShape = inputTensor->Shape();

        if (inputShape.size() < std::max(numRowsIdx, numColsIdx)) {
            printf_err("Invalid input tensor dims\n");
            return;
        }

        const uint32_t kNumRows = inputShape[numRowsIdx];
        const uint32_t kNumCols = inputShape[numColsIdx];

        /* Deduce the data length required for 1 inference from the network parameters. */
        this->m_audioDataWindowSize =
            (((this->m_inputResizeScale * kNumCols) - 1) * melSpectrogramFrameStride) +
            melSpectrogramFrameLen;
        this->m_numReusedFeatureVectors =
            kNumRows - (this->m_numMelSpecVectorsInAudioStride / this->m_inputResizeScale);
        this->m_melSpec.Init();

        /* Creating a Mel Spectrogram sliding window for the data required for 1 inference.
         * "resizing" done here by multiplying stride by resize scale. */
        this->m_melWindowSlider = audio::SlidingWindow<const int16_t>(
            this->m_audioDataWindowSize,
            melSpectrogramFrameLen,
            melSpectrogramFrameStride * this->m_inputResizeScale);

        /* Construct feature calculation function. */
        this->m_featureCalc = GetFeatureCalculator(
            this->m_melSpec, inputTensor, this->m_numReusedFeatureVectors, adModelTrainingMean);
        this->m_validInstance = true;
    }

bool AdPreProcess::DoPreProcess(const void* input, size_t inputSize)
{
    /* Check that we have a valid instance. */
    if (!this->m_validInstance) {
        printf_err("Invalid pre-processor instance\n");
        return false;
    }

    /* We expect that we can traverse the size with which the MEL spectrogram
     * sliding window was initialised with. */
    if (!input || inputSize < this->m_audioDataWindowSize) {
        printf_err("Invalid input provided for pre-processing\n");
        return false;
    }

    /* We moved to the next window - set the features sliding to the new address. */
    this->m_melWindowSlider.Reset(static_cast<const int16_t*>(input));

    /* The first window does not have cache ready. */
    const bool useCache = this->m_audioWindowIndex > 0 && this->m_numReusedFeatureVectors > 0;

    /* Start calculating features inside one audio sliding window. */
    while (this->m_melWindowSlider.HasNext()) {
        const int16_t* melSpecWindow = this->m_melWindowSlider.Next();
        std::vector<int16_t> melSpecAudioData = std::vector<int16_t>(
                melSpecWindow,
                melSpecWindow + this->m_melSpectrogramFrameLen);

        /* Compute features for this window and write them to input tensor. */
        this->m_featureCalc(melSpecAudioData,
                            this->m_melWindowSlider.Index(),
                            useCache,
                            this->m_numMelSpecVectorsInAudioStride,
                            this->m_inputResizeScale);
    }

    return true;
}

uint32_t AdPreProcess::GetAudioWindowSize()
{
    return this->m_audioDataWindowSize;
}

uint32_t AdPreProcess::GetAudioDataStride()
{
    return this->m_audioDataStride;
}

void AdPreProcess::SetAudioWindowIndex(uint32_t idx)
{
    this->m_audioWindowIndex = idx;
}

AdPostProcess::AdPostProcess(const std::shared_ptr<fwk::iface::TensorIface> outputTensor) :
    m_outputTensor{outputTensor}
{}

bool AdPostProcess::DoPostProcess()
{
    switch (this->m_outputTensor->Type()) {
    case fwk::iface::TensorType::INT8:
        this->Dequantize<int8_t>();
        break;
    default:
        printf_err("Unsupported tensor type");
        return false;
    }

    math::MathUtils::SoftmaxF32(this->m_dequantizedOutputVec);
    return true;
}

float AdPostProcess::GetOutputValue(uint32_t index)
{
    if (index < this->m_dequantizedOutputVec.size()) {
        return this->m_dequantizedOutputVec[index];
    }
    printf_err("Invalid index for output\n");
    return 0.0;
}

std::function<void(std::vector<int16_t>&, int, bool, size_t, size_t)>
GetFeatureCalculator(audio::AdMelSpectrogram& melSpec,
                     const std::shared_ptr<fwk::iface::TensorIface> inputTensor,
                     size_t cacheSize,
                     float trainingMean)
{
    std::function<void (std::vector<int16_t>&, size_t, bool, size_t, size_t)> melSpecFeatureCalc = nullptr;

    auto quant = inputTensor->GetQuantParams();

    if (quant.scale != 0) {
        switch (inputTensor->Type()) {
        case fwk::iface::TensorType::INT8: {
            melSpecFeatureCalc = FeatureCalc<int8_t>(
                inputTensor, cacheSize, [=, &melSpec](const std::vector<int16_t>& audioDataWindow) {
                    return melSpec.MelSpecComputeQuant<int8_t>(
                        audioDataWindow, quant.scale, quant.offset, trainingMean);
                });
            break;
        }
        default:
            printf_err("Tensor type %s not supported\n",
                       fwk::iface::GetTensorDataTypeName(inputTensor->Type()));
        }
    } else {
        melSpecFeatureCalc = FeatureCalc<float>(
            inputTensor, cacheSize, [=, &melSpec](const std::vector<int16_t>& audioDataWindow) {
                return melSpec.ComputeMelSpec(audioDataWindow, trainingMean);
            });
    }
    return melSpecFeatureCalc;
}

} /* namespace app */
} /* namespace arm */
