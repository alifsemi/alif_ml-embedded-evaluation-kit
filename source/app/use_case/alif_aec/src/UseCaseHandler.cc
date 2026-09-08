/* This file was ported to work on Alif Semiconductor devices. */

/* Copyright (C) 2023-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

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
#include "UseCaseHandler.hpp"


#include "hal.h"
#include "timer_alif.h"
#include "sys_utils.h"
#include "UseCaseCommonUtils.hpp"
#include "mlek/common/ImageUtils.hpp"
#include "mlek/log/log_macros.h"
#include "mlek/use_case/kws/KwsClassifier.hpp"
#include "mlek/use_case/kws/KwsProcessing.hpp"
#include "mlek/use_case/kws/KwsResult.hpp"
#include "mlek/fwk/tflm/MicroNetKwsModel.hpp"
#include "audio/audio_out.h"
#include "board_utils.h"

#include <vector>

using arm::app::KwsClassifier;
using arm::app::Profiler;
using arm::app::ClassificationResult;
using arm::app::ApplicationContext;
using arm::app::fwk::iface::Model;
using arm::app::KwsPreProcess;
using arm::app::KwsPostProcess;
using arm::app::fwk::tflm::MicroNetKwsModel;


// #define AUDIO_SAMPLES 256
// #define AUDIO_STRIDE 128
#define AUDIO_SAMPLES 16000 // 16k samples/sec, 1sec sample
#define AUDIO_STRIDE 8000 // 0.5 seconds

static int16_t audio_inf[AUDIO_SAMPLES + AUDIO_STRIDE];
static int16_t audio_out[AUDIO_STRIDE*2];

static volatile bool button_pressed = false;

void button_cb(uint32_t event)
{
    (void)event; // Unused parameter
    // Debounce: ignore presses that arrive within 300ms of the previous one.
    static uint32_t lastPressTime = 0;
    const uint32_t now = Get_SysTick_Count();
    if (now - lastPressTime < 300) {
        return;
    }
    lastPressTime = now;

    button_pressed = !button_pressed;
}

namespace alif {
namespace app {

namespace audio {
using namespace arm::app::audio;
}

namespace kws {
using namespace arm::app::kws;
}


    /* inference handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx)
    {
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model = ctx.Get<Model&>("model");
        const auto mfccFrameLength = ctx.Get<int>("frameLength");
        const auto mfccFrameStride = ctx.Get<int>("frameStride");
        const auto audioRate = ctx.Get<int>("audioRate");
        // const auto scoreThreshold = ctx.Get<float>("scoreThreshold");
        int err = 0;

        BOARD_BUTTON2_Init(button_cb);
        BOARD_BUTTON2_Control(BOARD_BUTTON_ENABLE_INTERRUPT);

        constexpr int minTensorDims = static_cast<int>(
            (MicroNetKwsModel::ms_inputRowsIdx > MicroNetKwsModel::ms_inputColsIdx)?
             MicroNetKwsModel::ms_inputRowsIdx : MicroNetKwsModel::ms_inputColsIdx);

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        /* Get Input and Output tensors for pre/post processing. */
        auto inputTensor = model.GetInputTensor(0);
        auto outputTensor = model.GetOutputTensor(0);
        const auto inputShape = inputTensor->Shape();
        if (inputShape.empty()) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputShape.size() < minTensorDims) {
            printf_err("Input tensor dimension should be >= %d\n", minTensorDims);
            return false;
        }

        /* Get input shape for feature extraction. */
        const uint32_t numMfccFeatures = inputShape[arm::app::fwk::tflm::MicroNetKwsModel::ms_inputColsIdx];
        const uint32_t numMfccFrames   = inputShape[arm::app::fwk::tflm::MicroNetKwsModel::ms_inputRowsIdx];

        /* We expect to be sampling 1 second worth of data at a time.
        *  NOTE: This is only used for time stamp calculation. */
        // const float secondsPerSample = 1.0f / audioRate;

        /* Set up pre and post-processing. */
        KwsPreProcess preProcess = KwsPreProcess(inputTensor, numMfccFeatures, numMfccFrames,
                                                 mfccFrameLength, mfccFrameStride);

        std::vector<ClassificationResult> singleInfResult;
        KwsPostProcess postProcess = KwsPostProcess(outputTensor, ctx.Get<KwsClassifier &>("classifier"),
                                                    ctx.Get<std::vector<std::string>&>("labels"),
                                                    singleInfResult);

        int index = 0;
        std::vector<kws::KwsResult> infResults;
        static bool audio_inited;
        static const int16_t* audioData = nullptr;
        static int strides_in_example_audio = 0;
        if (!audio_inited) {
            // LIVE AUDIO IN init
            err = hal_audio_alif_init(audioRate);
            if (err) {
                printf_err("hal_audio_alif_init failed with error: %d\n", err);
                return false;
            }
            audio_inited = true;

            // // STATIC AUDIO init
            hal_audio_init();
            if (!hal_audio_configure(HAL_AUDIO_MODE_SINGLE_BURST,
                                    HAL_AUDIO_FORMAT_16KHZ_MONO_16BIT)) {
                printf_err("Failed to configure audio\n");
                return false;
            }

            // READ example audio data from static array
            uint32_t nElements = 0;
            hal_audio_start();
            audioData = hal_audio_get_captured_frame(&nElements);
            if (!nElements || !audioData) {
                debug("End of stream\n");
                return false;
            }
            strides_in_example_audio = nElements / AUDIO_STRIDE;
            info("Example audio data: %lu samples, %d strides\n", nElements, strides_in_example_audio);

            // AUDIO OUT init
            err = audio_out_init(audioRate);
            if (err) {
                printf_err("audio_out_init failed with error: %d\n", err);
                return false;
            }
        }

        // Start first fill of final stride section of buffer
        hal_get_audio_data(audio_inf + AUDIO_SAMPLES, AUDIO_STRIDE);

        do {
            // // Wait until stride buffer is full - initiated above or by previous interation of loop
            err = hal_wait_for_audio();
            if (err) {
                printf_err("hal_wait_for_audio failed with error: %d\n", err);
                return false;
            }

            // move buffer down by one stride, clearing space at the end for the next stride
            std::copy(audio_inf + AUDIO_STRIDE, audio_inf + AUDIO_STRIDE + AUDIO_SAMPLES, audio_inf);

            // start receiving the next stride immediately before we start heavy processing, so as not to lose anything
            // Skip on the final stride: an unconsumed receive leaves the driver busy (rx_busy) and the next session fails with -2.
            if (index + 1 < strides_in_example_audio) {
                hal_get_audio_data(audio_inf + AUDIO_SAMPLES, AUDIO_STRIDE);
            }

            hal_audio_alif_preprocessing(audio_inf + AUDIO_SAMPLES - AUDIO_STRIDE, AUDIO_STRIDE);

            const int16_t* inferenceWindow = audio_inf;

            const int16_t* audio_out_ptr = audioData + index * AUDIO_STRIDE;

            std::copy(audio_out_ptr, audio_out_ptr + AUDIO_STRIDE, audio_out);
            err = audio_out_transmit(audio_out, AUDIO_STRIDE);
            if (err) {
                printf_err("audio_out_transmit failed with error: %d\n", err);
                return false;
            }

            /* Run the pre-processing, inference and post-processing. */
            if (!preProcess.DoPreProcess(inferenceWindow, index)) {
                printf_err("Pre-processing failed.");
                return false;
            }
            if (!RunInference(model, profiler)) {
                printf_err("Inference failed.");
                return false;
            }
            if (!postProcess.DoPostProcess()) {
                printf_err("Post-processing failed.");
                return false;
            }

            // AEC done, write to memory

            index++;
        } while (index < strides_in_example_audio);
        return true;
    }

} /* namespace app */
} /* namespace alif */
