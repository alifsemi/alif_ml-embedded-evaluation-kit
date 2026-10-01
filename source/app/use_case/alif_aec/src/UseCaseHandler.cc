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
#include "delay.h"
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
#include "FatFS/sd_fatfs.h"

#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE & 1)
#include <arm_mve.h>
#endif

#include <cstring>
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
/* Second capture buffer used to run the PDM microphone in parallel with I2S. */
static int16_t audio_inf_pdm[AUDIO_SAMPLES + AUDIO_STRIDE];
/* Triple-buffered DMA-source buffers for the I2S3 TX DMA. We prime two Sends
 * before the loop (one playing, one queued), so main's audio_out_transmit
 * ALWAYS finds the queue full on first attempt and spins. That pins main's
 * iteration rate to the DAC rate; otherwise the mic wait paces main at
 * 500+P ms per iter and the ~5 ms/stride drift exhausts the DAC's 500 ms
 * window after ~87 strides (~44 s), producing a 100% Send-to-Send miss rate
 * and a continuous 0.5 s crackle. Must live in shared SRAM (fabric DMA cannot
 * reach M55 TCM). */
static int16_t audio_out_a[AUDIO_STRIDE*2] __attribute__((section(".bss.NoInit.audio_out"))) __attribute__((aligned(32)));
static int16_t audio_out_b[AUDIO_STRIDE*2] __attribute__((section(".bss.NoInit.audio_out"))) __attribute__((aligned(32)));
static int16_t audio_out_c[AUDIO_STRIDE*2] __attribute__((section(".bss.NoInit.audio_out"))) __attribute__((aligned(32)));

/* AEC output accumulated in on-chip SRAM (NoInit region), one AUDIO_STRIDE per inference. */
#define AEC_OUTPUT_MAX_SAMPLES (128 * AUDIO_STRIDE)
static int16_t aec_output_sram[AEC_OUTPUT_MAX_SAMPLES] __attribute__((section(".bss.NoInit.temp_buf_sram")));
static uint32_t aec_output_len = 0;

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

        aec_output_len = 0;

        int index = 0;
        std::vector<kws::KwsResult> infResults;
        static bool audio_inited;
        static const int16_t* audioData = nullptr;
        static int strides_in_example_audio = 0;
        if (!audio_inited) {
            err = hal_audio_alif_init_ex(HAL_AUDIO_MIC_I2S, audioRate);
            if (err) {
                printf_err("hal_audio_alif_init_ex(I2S) failed with error: %d\n", err);
                return false;
            }
            err = hal_audio_alif_init_ex(HAL_AUDIO_MIC_PDM, audioRate);
            if (err) {
                printf_err("hal_audio_alif_init_ex(PDM) failed with error: %d\n", err);
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
        // set_audio_gain(30);

        std::memset(audio_out_a, 0, sizeof(audio_out_a));
        std::memset(audio_out_b, 0, sizeof(audio_out_b));
        std::memset(audio_out_c, 0, sizeof(audio_out_c));

        // Start first fill of final stride section of both mic buffers.
        hal_get_audio_data_ex(HAL_AUDIO_MIC_I2S, audio_inf     + AUDIO_SAMPLES, AUDIO_STRIDE);
        hal_get_audio_data_ex(HAL_AUDIO_MIC_PDM, audio_inf_pdm + AUDIO_SAMPLES, AUDIO_STRIDE);

        // Prime the DAC pipeline with TWO silent buffers (one playing + one
        // queued). The loop will then always find the queue full on its first
        // transmit attempt and spin on -1, pinning main to the DAC rate.
        err = audio_out_transmit(audio_out_a, AUDIO_STRIDE * 2);
        if (err) {
            printf_err("prime 1 audio_out_transmit failed with error: %d\n", err);
            return false;
        }
        err = audio_out_transmit(audio_out_b, AUDIO_STRIDE * 2);
        if (err) {
            printf_err("prime 2 audio_out_transmit failed with error: %d\n", err);
            return false;
        }

        // After prime: A is being Sent, B is queued (data_ready=true). The
        // loop first fills C, then rotates C -> A -> B -> C -> ...
        int16_t *audio_out_fill = audio_out_c;

        do {
            // Fill audio_out_fill from the PREVIOUS iteration's preprocessed
            // mic data (already in audio_inf / audio_inf_pdm from last iter's
            // tail). Doing this BEFORE the mic wait means the next DAC buffer
            // is queued near the top of the stride, giving the DAC_Callback a
            // ~500 ms margin before it needs data_ready=true.
            const int16_t* src_l = audio_inf     + AUDIO_SAMPLES - AUDIO_STRIDE;
            const int16_t* src_r = audio_inf_pdm + AUDIO_SAMPLES - AUDIO_STRIDE;
#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE & 1)
            static_assert(AUDIO_STRIDE % 8 == 0,
                          "AUDIO_STRIDE must be a multiple of 8 for MVE VST2");
            for (int i = 0; i < AUDIO_STRIDE; i += 8) {
                int16x8x2_t v;
                v.val[0] = vld1q_s16(&src_l[i]);
                v.val[1] = vld1q_s16(&src_r[i]);
                vst2q_s16(&audio_out_fill[2 * i], v);
            }
#else
            for (int i = 0; i < AUDIO_STRIDE; ++i) {
                audio_out_fill[2 * i]     = src_l[i];
                audio_out_fill[2 * i + 1] = src_r[i];
            }
#endif

            // Spin until the DAC queue has room. With triple-buffered priming
            // this spin is where main sleeps for most of each stride, pinning
            // iteration rate to the DAC rate and eliminating drift.
            while (audio_out_transmit(audio_out_fill, AUDIO_STRIDE * 2) == -1) {
                __WFE();
            }

            // Swap: cycle through the three buffers (C -> A -> B -> C ...).
            // After this transmit, the fill buffer is now "queued" and the
            // one that was previously queued is now "being Sent". The buffer
            // that was being Sent two iters ago is now free.
            if (audio_out_fill == audio_out_a) {
                audio_out_fill = audio_out_b;
            } else if (audio_out_fill == audio_out_b) {
                audio_out_fill = audio_out_c;
            } else {
                audio_out_fill = audio_out_a;
            }

            // Now spend the ~500 ms stride waiting for the next mic data and
            // preprocessing it. All of this happens in parallel with the DAC
            // DMA'ing the buffer we just queued.
            err = hal_wait_for_audio_ex(HAL_AUDIO_MIC_I2S);
            if (err) {
                printf_err("hal_wait_for_audio_ex(I2S) failed with error: %d\n", err);
                return false;
            }
            err = hal_wait_for_audio_ex(HAL_AUDIO_MIC_PDM);
            if (err) {
                printf_err("hal_wait_for_audio_ex(PDM) failed with error: %d\n", err);
                return false;
            }

            std::memmove(audio_inf, audio_inf + AUDIO_STRIDE, AUDIO_SAMPLES * sizeof(int16_t));
            std::memmove(audio_inf_pdm, audio_inf_pdm + AUDIO_STRIDE, AUDIO_SAMPLES * sizeof(int16_t));
            __disable_irq();
            hal_get_audio_data_ex(HAL_AUDIO_MIC_I2S, audio_inf + AUDIO_SAMPLES, AUDIO_STRIDE);
            hal_get_audio_data_ex(HAL_AUDIO_MIC_PDM, audio_inf_pdm + AUDIO_SAMPLES, AUDIO_STRIDE);
            __enable_irq();

            hal_audio_alif_preprocessing_ex(HAL_AUDIO_MIC_I2S,
                                            audio_inf + AUDIO_SAMPLES - AUDIO_STRIDE,
                                            AUDIO_STRIDE);
            hal_audio_alif_preprocessing_ex(HAL_AUDIO_MIC_PDM,
                                            audio_inf_pdm + AUDIO_SAMPLES - AUDIO_STRIDE,
                                            AUDIO_STRIDE);

            const int16_t* inferenceWindow = audio_inf;
            (void) inferenceWindow;  // Unused until inference is re-enabled.

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

            index++;
        } while (1);//while (index < strides_in_example_audio);

        info("AEC output stored in SRAM: %lu samples at %p\n",
             aec_output_len, (void*)aec_output_sram);

        // Write the accumulated SRAM buffer to a file
        err = init_fs();
        if (err) {
            printf_err("init_fs failed with error: %d\n", err);
            return false;
        }
        err = write_file(aec_output_sram, aec_output_len);
        if (err) {
            printf_err("write_file failed with error: %d\n", err);
            return false;
        }
        err = close_file();
        if (err) {
            printf_err("close_file failed with error: %d\n", err);
            return false;
        }
        err = deinit_fs();
        if (err) {
            printf_err("deinit_fs failed with error: %d\n", err);
            return false;
        }
        return true;
    }

} /* namespace app */
} /* namespace alif */
