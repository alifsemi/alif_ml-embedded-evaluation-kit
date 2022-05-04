/*
 * Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
#include "hal.h"
#include "UseCaseHandler.hpp"
#include "UseCaseCommonUtils.hpp"
#include "AudioUtils.hpp"
#include "ImageUtils.hpp"
#include "InputFiles.hpp"
#include "RNNoiseModel.hpp"
#include "RNNoiseFeatureProcessor.hpp"
#include "RNNoiseProcessing.hpp"
#include "log_macros.h"

namespace arm {
namespace app {

    /**
    * @brief            Helper function to increment current audio clip features index.
    * @param[in,out]    ctx   Pointer to the application context object.
    **/
    static void IncrementAppCtxClipIdx(ApplicationContext& ctx);

    /* Noise reduction inference handler. */
    bool NoiseReductionHandler(ApplicationContext& ctx, bool runAll)
    {
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        /* Variables used for memory dumping. */
        size_t memDumpMaxLen = 0;
        uint8_t* memDumpBaseAddr = nullptr;
        size_t undefMemDumpBytesWritten = 0;
        size_t* pMemDumpBytesWritten = &undefMemDumpBytesWritten;
        if (ctx.Has("MEM_DUMP_LEN") && ctx.Has("MEM_DUMP_BASE_ADDR") && ctx.Has("MEM_DUMP_BYTE_WRITTEN")) {
            memDumpMaxLen = ctx.Get<size_t>("MEM_DUMP_LEN");
            memDumpBaseAddr = ctx.Get<uint8_t*>("MEM_DUMP_BASE_ADDR");
            pMemDumpBytesWritten = ctx.Get<size_t*>("MEM_DUMP_BYTE_WRITTEN");
        }
        std::reference_wrapper<size_t> memDumpBytesWritten = std::ref(*pMemDumpBytesWritten);
        auto& profiler = ctx.Get<Profiler&>("profiler");

        /* Get model reference. */
        auto& model = ctx.Get<RNNoiseModel&>("model");
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        /* Populate Pre-Processing related parameters. */
        auto audioFrameLen = ctx.Get<uint32_t>("frameLength");
        auto audioFrameStride = ctx.Get<uint32_t>("frameStride");
        auto nrNumInputFeatures = ctx.Get<uint32_t>("numInputFeatures");

        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        if (nrNumInputFeatures != inputTensor->bytes) {
            printf_err("Input features size must be equal to input tensor size."
                       " Feature size = %" PRIu32 ", Tensor size = %zu.\n",
                       nrNumInputFeatures, inputTensor->bytes);
            return false;
        }

        TfLiteTensor* outputTensor = model.GetOutputTensor(model.m_indexForModelOutput);

        /* Initial choice of index for WAV file. */
        auto startClipIdx = ctx.Get<uint32_t>("clipIndex");

        std::function<const int16_t* (const uint32_t)> audioAccessorFunc = get_audio_array;
        if (ctx.Has("features")) {
            audioAccessorFunc = ctx.Get<std::function<const int16_t* (const uint32_t)>>("features");
        }
        std::function<uint32_t (const uint32_t)> audioSizeAccessorFunc = get_audio_array_size;
        if (ctx.Has("featureSizes")) {
            audioSizeAccessorFunc = ctx.Get<std::function<uint32_t (const uint32_t)>>("featureSizes");
        }
        std::function<const char*(const uint32_t)> audioFileAccessorFunc = get_filename;
        if (ctx.Has("featureFileNames")) {
            audioFileAccessorFunc = ctx.Get<std::function<const char*(const uint32_t)>>("featureFileNames");
        }
        do {
            hal_lcd_clear(COLOR_BLACK);

            auto startDumpAddress = memDumpBaseAddr + memDumpBytesWritten;
            auto currentIndex = ctx.Get<uint32_t>("clipIndex");

            /* Creating a sliding window through the audio. */
            auto audioDataSlider = audio::SlidingWindow<const int16_t>(
                    audioAccessorFunc(currentIndex),
                    audioSizeAccessorFunc(currentIndex), audioFrameLen,
                    audioFrameStride);

            info("Running inference on input feature map %" PRIu32 " => %s\n", currentIndex,
                 audioFileAccessorFunc(currentIndex));

            memDumpBytesWritten += DumpDenoisedAudioHeader(audioFileAccessorFunc(currentIndex),
                                                           (audioDataSlider.TotalStrides() + 1) * audioFrameLen,
                 memDumpBaseAddr + memDumpBytesWritten,
                 memDumpMaxLen - memDumpBytesWritten);

            /* Set up pre and post-processing. */
            std::shared_ptr<rnn::RNNoiseFeatureProcessor> featureProcessor =
                    std::make_shared<rnn::RNNoiseFeatureProcessor>();
            std::shared_ptr<rnn::FrameFeatures> frameFeatures =
                    std::make_shared<rnn::FrameFeatures>();

            RNNoisePreProcess preProcess = RNNoisePreProcess(inputTensor, featureProcessor, frameFeatures);

            std::vector<int16_t> denoisedAudioFrame(audioFrameLen);
            RNNoisePostProcess postProcess = RNNoisePostProcess(outputTensor, denoisedAudioFrame,
                    featureProcessor, frameFeatures);

            bool resetGRU = true;

            while (audioDataSlider.HasNext()) {
                const int16_t* inferenceWindow = audioDataSlider.Next();

                if (!preProcess.DoPreProcess(inferenceWindow, audioFrameLen)) {
                    printf_err("Pre-processing failed.");
                    return false;
                }

                /* Reset or copy over GRU states first to avoid TFLu memory overlap issues. */
                if (resetGRU){
                    model.ResetGruState();
                } else {
                    /* Copying gru state outputs to gru state inputs.
                     * Call ResetGruState in between the sequence of inferences on unrelated input data. */
                    model.CopyGruStates();
                }

                /* Strings for presentation/logging. */
                std::string str_inf{"Running inference... "};

                /* Display message on the LCD - inference running. */
                hal_lcd_display_text(str_inf.c_str(), str_inf.size(),
                        dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

                info("Inference %zu/%zu\n", audioDataSlider.Index() + 1, audioDataSlider.TotalStrides() + 1);

                /* Run inference over this feature sliding window. */
                if (!RunInference(model, profiler)) {
                    printf_err("Inference failed.");
                    return false;
                }
                resetGRU = false;

                /* Carry out post-processing. */
                if (!postProcess.DoPostProcess()) {
                    printf_err("Post-processing failed.");
                    return false;
                }

                /* Erase. */
                str_inf = std::string(str_inf.size(), ' ');
                hal_lcd_display_text(str_inf.c_str(), str_inf.size(),
                        dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

                if (memDumpMaxLen > 0) {
                    /* Dump final post processed output to memory. */
                    memDumpBytesWritten += DumpOutputDenoisedAudioFrame(
                            denoisedAudioFrame,
                            memDumpBaseAddr + memDumpBytesWritten,
                            memDumpMaxLen - memDumpBytesWritten);
                }
            }

            if (memDumpMaxLen > 0) {
                /* Needed to not let the compiler complain about type mismatch. */
                size_t valMemDumpBytesWritten = memDumpBytesWritten;
                info("Output memory dump of %zu bytes written at address 0x%p\n",
                     valMemDumpBytesWritten, startDumpAddress);
            }

            /* Finish by dumping the footer. */
            DumpDenoisedAudioFooter(memDumpBaseAddr + memDumpBytesWritten, memDumpMaxLen - memDumpBytesWritten);

            info("All inferences for audio clip complete.\n");
            profiler.PrintProfilingResult();
            IncrementAppCtxClipIdx(ctx);

            std::string clearString{' '};
            hal_lcd_display_text(clearString.c_str(), clearString.size(),
                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

            std::string completeMsg{"Inference complete!"};

            /* Display message on the LCD - inference complete. */
            hal_lcd_display_text(completeMsg.c_str(), completeMsg.size(),
                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

        } while (runAll && ctx.Get<uint32_t>("clipIndex") != startClipIdx);

        return true;
    }

    size_t DumpDenoisedAudioHeader(const char* filename, size_t dumpSize,
                                   uint8_t* memAddress, size_t memSize){

        if (memAddress == nullptr){
            return 0;
        }

        int32_t filenameLength = strlen(filename);
        size_t numBytesWritten = 0;
        size_t numBytesToWrite = 0;
        int32_t dumpSizeByte = dumpSize * sizeof(int16_t);
        bool overflow = false;

        /* Write the filename length */
        numBytesToWrite = sizeof(filenameLength);
        if (memSize - numBytesToWrite > 0) {
            std::memcpy(memAddress, &filenameLength, numBytesToWrite);
            numBytesWritten += numBytesToWrite;
            memSize -= numBytesWritten;
        } else {
            overflow = true;
        }

        /* Write file name */
        numBytesToWrite = filenameLength;
        if(memSize - numBytesToWrite > 0) {
            std::memcpy(memAddress + numBytesWritten, filename, numBytesToWrite);
            numBytesWritten += numBytesToWrite;
            memSize -= numBytesWritten;
        } else {
            overflow = true;
        }

        /* Write dumpSize in byte */
        numBytesToWrite = sizeof(dumpSizeByte);
        if(memSize  - numBytesToWrite > 0) {
            std::memcpy(memAddress + numBytesWritten, &(dumpSizeByte), numBytesToWrite);
            numBytesWritten += numBytesToWrite;
            memSize -= numBytesWritten;
        } else {
            overflow = true;
        }

        if(false == overflow) {
            info("Audio Clip dump header info (%zu bytes) written to %p\n", numBytesWritten,  memAddress);
        } else {
            printf_err("Not enough memory to dump Audio Clip header.\n");
        }

        return numBytesWritten;
    }

    size_t DumpDenoisedAudioFooter(uint8_t* memAddress, size_t memSize){
        if ((memAddress == nullptr) || (memSize < 4)) {
            return 0;
        }
        const int32_t eofMarker = -1;
        std::memcpy(memAddress, &eofMarker, sizeof(int32_t));

        return sizeof(int32_t);
     }

    size_t DumpOutputDenoisedAudioFrame(const std::vector<int16_t>& audioFrame,
                                        uint8_t* memAddress, size_t memSize)
    {
        if (memAddress == nullptr) {
            return 0;
        }

        size_t numByteToBeWritten = audioFrame.size() * sizeof(int16_t);
        if( numByteToBeWritten > memSize) {
            printf_err("Overflow error: Writing %zu of %zu bytes to memory @ 0x%p.\n", memSize, numByteToBeWritten, memAddress);
            numByteToBeWritten = memSize;
        }

        std::memcpy(memAddress, audioFrame.data(), numByteToBeWritten);
        info("Copied %zu bytes to %p\n", numByteToBeWritten,  memAddress);

        return numByteToBeWritten;
    }

    size_t DumpOutputTensorsToMemory(Model& model, uint8_t* memAddress, const size_t memSize)
    {
        const size_t numOutputs = model.GetNumOutputs();
        size_t numBytesWritten = 0;
        uint8_t* ptr = memAddress;

        /* Iterate over all output tensors. */
        for (size_t i = 0; i < numOutputs; ++i) {
            const TfLiteTensor* tensor = model.GetOutputTensor(i);
            const auto* tData = tflite::GetTensorData<uint8_t>(tensor);
#if VERIFY_TEST_OUTPUT
            DumpTensor(tensor);
#endif /* VERIFY_TEST_OUTPUT */
            /* Ensure that we don't overflow the allowed limit. */
            if (numBytesWritten + tensor->bytes <= memSize) {
                if (tensor->bytes > 0) {
                    std::memcpy(ptr, tData, tensor->bytes);

                    info("Copied %zu bytes for tensor %zu to 0x%p\n",
                        tensor->bytes, i, ptr);

                    numBytesWritten += tensor->bytes;
                    ptr += tensor->bytes;
                }
            } else {
                printf_err("Error writing tensor %zu to memory @ 0x%p\n",
                    i, memAddress);
                break;
            }
        }

        info("%zu bytes written to memory @ 0x%p\n", numBytesWritten, memAddress);

        return numBytesWritten;
    }

    static void IncrementAppCtxClipIdx(ApplicationContext& ctx)
    {
        auto curClipIdx = ctx.Get<uint32_t>("clipIndex");
        if (curClipIdx + 1 >= NUMBER_OF_FILES) {
            ctx.Set<uint32_t>("clipIndex", 0);
            return;
        }
        ++curClipIdx;
        ctx.Set<uint32_t>("clipIndex", curClipIdx);
    }

} /* namespace app */
} /* namespace arm */
