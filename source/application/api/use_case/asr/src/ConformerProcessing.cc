/*
* SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates
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

#include "ConformerProcessing.hpp"

#include <cstring>

#include "log_macros.h"

namespace arm::app
{
    template class ConformerPreProcess<float>;
    template class ConformerPreProcess<int16_t>;

    template<class T>
    ConformerPreProcess<T>::ConformerPreProcess(
        const std::shared_ptr<fwk::iface::TensorIface>& inputTensorMelSpec,
        const std::shared_ptr<fwk::iface::TensorIface>& inputTensorChunkSize,
        int32_t melSpecWindowSize,
        int32_t melSpecHopSize,
        int32_t chunkSize
    ) : m_inputTensorMelSpec{inputTensorMelSpec},
        m_inputTensorChunkSize{inputTensorChunkSize},
        m_melSpecWindowSize{melSpecWindowSize},
        m_melSpecHopSize{melSpecHopSize},
        m_chunkSize{chunkSize},
        m_melSpectrogram{audio::ConformerMelSpectrogram(melSpecWindowSize)}
    {
        this->m_slidingWindow = audio::SlidingWindow<const T>(
            this->m_melSpecWindowSize,
            this->m_melSpecHopSize
        );
    }

    template<class T>
    bool ConformerPreProcess<T>::DoPreProcess(const void* input, size_t inputSize)
    {
        // Generate full mel spectrogram with padding
        auto elementsWritten = this->GenerateMelSpectrogram(
            input,
            inputSize,
            static_cast<float*>(this->m_inputTensorMelSpec->GetData()),
            this->m_inputTensorMelSpec->Bytes()
        );
        auto paddingNeeded = this->m_inputTensorMelSpec->GetNumElements() - elementsWritten;
        std::fill_n(
            static_cast<float*>(this->m_inputTensorMelSpec->GetData()) + elementsWritten,
            paddingNeeded,
            -20.f
        );

        // Populate second input tensor with audio chunk size value
        auto chunkSizeTensorData = static_cast<float *>(this->m_inputTensorChunkSize->GetData());
        *chunkSizeTensorData = static_cast<float>(this->m_chunkSize);

        return true;
    }

    template<class T>
    size_t ConformerPreProcess<T>::GenerateMelSpectrogram(
        const void* input, size_t inputSize, float* output, size_t outputSize)
    {
        this->m_melSpectrogram.Init();
        this->m_slidingWindow.Reset(static_cast<const T*>(input), inputSize);
        auto writeFrom = output;
        auto upperBound = output + outputSize;
        while (this->m_slidingWindow.HasNext()) {
            if (writeFrom >= upperBound) {
                printf_err("Would overflow\n");
                return false;
            }
            const T* windowStart = this->m_slidingWindow.Next();
            const T* windowEnd = windowStart + this->m_melSpecWindowSize;
            auto window = std::vector(windowStart, windowEnd);
            auto melSpecVector = this->m_melSpectrogram.ComputeMelSpec(window);
            memcpy(writeFrom, melSpecVector.data(), melSpecVector.size() * sizeof(float));
            writeFrom += melSpecVector.size();
        }
        return writeFrom - output;
    }
}
