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

#ifndef CONFORMER_PROCESSING_HPP
#define CONFORMER_PROCESSING_HPP

#include <memory>

#include "AudioUtils.hpp"
#include "BaseProcessing.hpp"
#include "ConformerMelSpectrogram.hpp"
#include "Tensor.hpp"

namespace arm::app
{
    /**
     *  @brief  Pre-processing implementation for the Conformer model.
     *          The constructor accepts references to both input tensors
     *          as well as parameters for the underlying mel spectrogram.
     *
     *          A full mel spectrogram will be computed for the provided
     *          audio input sample do the DoPreProcess method and the
     *          input tensors will be populated accordingly.
     *  @tparam     T   Type of audio input expected (typically int16_t
     *                  or float).
     */
    template<class T>
    class ConformerPreProcess : public BasePreProcess
    {
    public:
        /**
         * @brief                           Constructor
         * @param inputTensorMelSpec        First input tensor representing mel spectrogram
         * @param inputTensorChunkSize      Second input tensor containing chunk size
         * @param melSpecWindowSize         Mel spectrogram window size
         * @param melSpecHopSize            Mel spectrogram hop size
         * @param chunkSize                 Audio chunk size in milliseconds
         */
        explicit ConformerPreProcess(
            const std::shared_ptr<fwk::iface::TensorIface>& inputTensorMelSpec,
            const std::shared_ptr<fwk::iface::TensorIface>& inputTensorChunkSize,
            int32_t melSpecWindowSize,
            int32_t melSpecHopSize,
            int32_t chunkSize
        );

        /**
         * @brief               Carry out pre-processing and populate both input tensors
         *                      with the expected values.
         * @param input         Input audio data
         * @param inputSize     Size of input audio data
         * @return              True if pre-processing was successful, false otherwise
         */
        bool DoPreProcess(const void* input, size_t inputSize) override;

        /**
         *                      Generate a mel spectrogram for the given input audio sample
         * @param input         Input audio data
         * @param inputSize     Size of input audio data
         * @param output        Output data
         * @param outputSize    Size of output data
         * @return              Number of elements written to the output data
         */
        size_t GenerateMelSpectrogram(const void* input, size_t inputSize, float * output, size_t outputSize);

    private:
        const std::shared_ptr<fwk::iface::TensorIface> m_inputTensorMelSpec;
        const std::shared_ptr<fwk::iface::TensorIface> m_inputTensorChunkSize;
        const int32_t m_melSpecWindowSize;
        const int32_t m_melSpecHopSize;
        const int32_t m_chunkSize;

        audio::ConformerMelSpectrogram m_melSpectrogram;
        audio::SlidingWindow<const T> m_slidingWindow;

    };
} /* namespace arm::app */

#endif /* CONFORMER_PROCESSING_HPP */
