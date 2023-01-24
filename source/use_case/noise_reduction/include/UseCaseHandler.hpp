/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef NOISE_REDUCTION_EVT_HANDLER_HPP
#define NOISE_REDUCTION_EVT_HANDLER_HPP

#include "AppContext.hpp"
#include "Model.hpp"

namespace arm {
namespace app {

    /**
     * @brief       Handles the inference event for noise reduction.
     * @param[in]   ctx         pointer to the application context
     * @param[in]   runAll      flag to request classification of all the available audio clips
     * @return      True or false based on execution success
     **/
    bool NoiseReductionHandler(ApplicationContext& ctx, bool runAll);

    /**
     * @brief           Dumps the output tensors to a memory address.
     * This functionality is required for RNNoise use case as we want to
     * save the inference output to a file. Dumping out tensors to a
     * memory location will allow the Arm FVP or MPS3 to extract the
     * contents of this memory location to a file. This file could then
     * be used by an offline post-processing script.
     *
     * @param[in]   model       reference to a model
     * @param[in]   memAddress  memory address at which the dump will start
     * @param[in]   memSize     maximum size (in bytes) of the dump.
     *
     * @return  number of bytes written to memory.
     */
    size_t DumpOutputTensorsToMemory(Model& model, uint8_t* memAddress,
                                    size_t memSize);

    /**
     * @brief Dumps the audio file header.
     * This functionality is required for RNNoise use case as we want to
     * save the inference output to a file. Dumping out the header to a
     * memory location will allow the Arm FVP or MPS3 to extract the
     * contents of this memory location to a file. 
     * The header contains the following information 
     * int32_t filenameLength: filename length
     * uint8_t[] filename: the string containing the file name (without trailing \0)
     * int32_t dumpSizeByte: audiofile buffer size in bytes
     *
     * @param[in]   filename    the file name
     * @param[in]   dumpSize    the size of the audio file (int elements)
     * @param[in]   memAddress  memory address at which the dump will start
     * @param[in]   memSize     maximum size (in bytes) of the dump.
     *
     * @return  number of bytes written to memory.
     */
    size_t DumpDenoisedAudioHeader(const char* filename, size_t dumpSize,
                                   uint8_t* memAddress, size_t memSize);

    /**
     * @brief Write a EOF marker at the end of the dump memory.
     *
     * @param[in]   memAddress  memory address at which the dump will start
     * @param[in]   memSize     maximum size (in bytes) of the dump.
     *
     * @return  number of bytes written to memory.
     */
    size_t DumpDenoisedAudioFooter(uint8_t *memAddress, size_t memSize);

    /**
     * @brief Dump the audio data to the memory
     *
     * @param[in]   audioFrame  The vector containg the audio data
     * @param[in]   memAddress  memory address at which the dump will start
     * @param[in]   memSize     maximum size (in bytes) of the dump.
     *
     * @return  number of bytes written to memory.
     */
    size_t DumpOutputDenoisedAudioFrame(const std::vector<int16_t> &audioFrame,
                                        uint8_t *memAddress, size_t memSize);

} /* namespace app */
} /* namespace arm */

#endif /* NOISE_REDUCTION_EVT_HANDLER_HPP */