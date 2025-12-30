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

#include "mlek/common/AudioSlidingWindow.hpp"
#include "mlek/common/BaseProcessing.hpp"
#include "mlek/use_case/asr/ConformerMelSpectrogram.hpp"
#include "mlek/fwk/iface/Tensor.hpp"

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
            uint32_t melSpecWindowSize,
            uint32_t melSpecHopSize,
            uint32_t chunkSize
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
        const uint32_t m_melSpecWindowSize;
        const uint32_t m_melSpecHopSize;
        const uint32_t m_chunkSize;

        audio::ConformerMelSpectrogram m_melSpectrogram;
        audio::SlidingWindow<const T> m_slidingWindow;

    };

    class ConformerPostProcess : BasePostProcess {
    public:
        /**
         * @brief                           Constructor
         * @param outputTensorLogits        First output tensor representing logits
         * @param outputTensorChunkSize      Second input tensor containing chunk size
         */
        explicit ConformerPostProcess(
            const std::shared_ptr<fwk::iface::TensorIface>& outputTensorLogits,
            const std::shared_ptr<fwk::iface::TensorIface>& outputTensorChunkSize,
            const std::vector<std::string>& labels,
            std::string& decodedResult
        );

        /**
         * @brief               Compute ArgMax for a 1D array of floats
         * @param logits        Input data (logits)
         * @param logitsSize    Size of input data
         * @return              ArgMax for input data
         */
        static uint32_t ArgMax1D(
            const float * logits,
            size_t logitsSize
        );

        /**
         * @brief               Compute ArgMax for a 2D array of floats
         * @param logits        Input data (logits)
         * @param logitsSize    Size of input data
         * @param logitsShape   Shape of 2D input data (must be a vector of length 2)
         * @return              1D vector of ArgMax values for 2D input data
         */
        static std::vector<uint32_t> ArgMax2D(
            const float * logits,
            size_t logitsSize,
            const std::array<size_t, 2>& logitsShape
        );

        /**
         * @brief               Decode logits to output string
         * @param logits        Input data (logits)
         * @param logitsSize    Size of input data
         * @param logitsShape   Shape of 2D input data (must be a vector of length 2)
         * @param labels        1D vector of labels
         * @return              Decoded string comprised of labels determined by input logits
         */
        static void Decode(
            const float * logits,
            size_t logitsSize,
            const std::array<size_t, 2>& logitsShape,
            const std::vector<std::string>& labels,
            std::string& decodedResult
        );

        /**
         * @brief   Carry out postprocessing and populate the decoded results
         * @return  True if postprocessing succeeded, false otherwise
         */
        bool DoPostProcess() override;
    private:
        const std::shared_ptr<fwk::iface::TensorIface> m_outputTensorLogits;
        const std::shared_ptr<fwk::iface::TensorIface> m_outputTensorChunkSize;
        const std::vector<std::string>& m_labels;
        std::string& m_decodedResult;
    };
} /* namespace arm::app */

#endif /* CONFORMER_PROCESSING_HPP */
