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
#if defined(ETH_STREAM)
#include "net/eth_stream.h"
#endif

#if defined(__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE & 1)
#include <arm_mve.h>
#endif

#include <cstdint>
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


/* Samples per loop iteration, set by alif_aec_FRAME_STRIDE (256 = 16 ms, the
 * AEC/NS frame stride). Mic capture, DAC output and the Ethernet stream all
 * run in strides. */
#ifndef AEC_FRAME_STRIDE
#define AEC_FRAME_STRIDE 256
#endif
#define AUDIO_STRIDE AEC_FRAME_STRIDE
/* History kept in the capture buffers, ending with the newest stride. The KWS
 * model needs a 1 s window; otherwise two strides, until the AEC/NS model
 * defines its window. */
#if defined(AEC_RUN_KWS)
#define AUDIO_SAMPLES 16000
#else
#define AUDIO_SAMPLES (2 * AUDIO_STRIDE)
#endif

/* Microphone streams. Each stream has two mics.
 *   USE_STEREO not defined (default): each stream is mono (the HAL mixes the
 *     two mics of a stream down). The first stream is played on the left DAC
 *     channel and the second one on the right.
 *   USE_STEREO defined: the HAL keeps the two mics of a stream as interleaved
 *     L/R samples. The first stream is played as is on the stereo DAC
 *     (left mic -> left, right mic -> right), the second stream is captured
 *     in the same format into its own buffer. Sample counts below are frames,
 *     buffers hold AEC_CHANNELS samples per frame.
 *
 *   LPPDM_PDM defined: PDM block (2 mics) + LPPDM block (2 mics) = 4 PDM mics
 *   otherwise:         I2S mic + PDM mic */
#if defined(USE_STEREO)
#define AEC_CHANNELS    2
#else
#define AEC_CHANNELS    1
#endif
#if defined(LPPDM_PDM)
#define AEC_MIC_L       HAL_AUDIO_MIC_PDM
#define AEC_MIC_R       HAL_AUDIO_MIC_LPPDM
#define AEC_MIC_L_NAME  "PDM"
#define AEC_MIC_R_NAME  "LPPDM"
#else
#define AEC_MIC_L       HAL_AUDIO_MIC_I2S
#define AEC_MIC_R       HAL_AUDIO_MIC_PDM
#define AEC_MIC_L_NAME  "I2S"
#define AEC_MIC_R_NAME  "PDM"
#endif

/* The stereo capture buffers are too big for DTCM together with the rest of
 * the application, keep them in the shared SRAM. The mono buffers stay in
 * DTCM. */
#if defined(USE_STEREO)
#define AEC_CAPTURE_BUF_SECTION __attribute__((section(".bss.NoInit.temp_buf_sram")))
#else
#define AEC_CAPTURE_BUF_SECTION
#endif

static int16_t audio_inf[(AUDIO_SAMPLES + AUDIO_STRIDE) * AEC_CHANNELS] AEC_CAPTURE_BUF_SECTION;
/* Second capture buffer used to run the second microphone stream in parallel
 * with the first one. */
static int16_t audio_inf_pdm[(AUDIO_SAMPLES + AUDIO_STRIDE) * AEC_CHANNELS] AEC_CAPTURE_BUF_SECTION;
#if defined(USE_STEREO) && defined(AEC_RUN_KWS)
/* Mono (left mic) copy of the first stream, the model input is mono. */
static int16_t audio_inf_mono[AUDIO_SAMPLES] AEC_CAPTURE_BUF_SECTION;
#endif
/* Triple-buffered DMA-source buffers for the DAC I2S TX DMA
 * (I2S3 on DevKit-e8, I2S2 on AppKit-e8). We prime two Sends
 * before the loop (one playing, one queued), so main's audio_out_transmit
 * ALWAYS finds the queue full on first attempt and spins. That pins main's
 * iteration rate to the DAC rate, so mic and DAC can't drift apart. Must live
 * in shared SRAM (fabric DMA cannot reach M55 TCM). */
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

#if defined(AEC_TIMING_PROBE)
/* Loop timing probe. Per iteration it measures the time spent waiting for the
 * DAC queue and the mics; the rest is work. A summary of the last 10 s is
 * printed at the top of an iteration, and the iteration that contains the
 * print is left out of the next summary. */
#define AEC_PROBE_ITERATIONS (10 * 16000 / AUDIO_STRIDE)

static struct {
    uint64_t iterStart;
    uint64_t waitStart;
    uint64_t wait;
    uint64_t sumIter;
    uint64_t minIter;
    uint64_t maxIter;
    uint64_t sumWork;
    uint64_t maxWork;
    uint32_t count;
    uint32_t overBudget;
    uint32_t underrunsAtStart;
    bool skip;
} s_probe = {0, 0, 0, 0, UINT64_MAX, 0, 0, 0, 0, 0, 0, true};

static uint32_t CyclesToUs(uint64_t cycles)
{
    return static_cast<uint32_t>(cycles * 1000000u / GetSystemCoreClock());
}

static void ProbeIteration(uint32_t audioRate)
{
    const uint64_t strideCycles =
        static_cast<uint64_t>(GetSystemCoreClock()) * AUDIO_STRIDE / audioRate;
    const uint64_t now = Get_SysTick_Cycle_Count();

    if (s_probe.iterStart != 0 && !s_probe.skip) {
        const uint64_t iter = now - s_probe.iterStart;
        const uint64_t work = iter - s_probe.wait;
        s_probe.sumIter += iter;
        s_probe.sumWork += work;
        s_probe.minIter = (iter < s_probe.minIter) ? iter : s_probe.minIter;
        s_probe.maxIter = (iter > s_probe.maxIter) ? iter : s_probe.maxIter;
        s_probe.maxWork = (work > s_probe.maxWork) ? work : s_probe.maxWork;
        s_probe.overBudget += (work > strideCycles) ? 1 : 0;
        s_probe.count++;
    }
    s_probe.skip = false;

    if (s_probe.count >= AEC_PROBE_ITERATIONS) {
        const uint32_t underruns = audio_out_underruns();
        const uint32_t strideUs = CyclesToUs(strideCycles);
        const uint32_t maxWorkUs = CyclesToUs(s_probe.maxWork);
        printf("AEC loop: %" PRIu32 " it, period avg %" PRIu32 " min %" PRIu32 " max %" PRIu32
               " us, work avg %" PRIu32 " max %" PRIu32 " us (%" PRIu32 "%% of %" PRIu32
               " us), over budget %" PRIu32 ", DAC underruns %" PRIu32 "\n",
               s_probe.count, CyclesToUs(s_probe.sumIter / s_probe.count),
               CyclesToUs(s_probe.minIter), CyclesToUs(s_probe.maxIter),
               CyclesToUs(s_probe.sumWork / s_probe.count), maxWorkUs,
               maxWorkUs * 100u / strideUs, strideUs, s_probe.overBudget,
               underruns - s_probe.underrunsAtStart);
        s_probe.sumIter = s_probe.sumWork = s_probe.maxIter = s_probe.maxWork = 0;
        s_probe.minIter = UINT64_MAX;
        s_probe.count = s_probe.overBudget = 0;
        s_probe.underrunsAtStart = underruns;
        s_probe.skip = true;
    }

    s_probe.iterStart = Get_SysTick_Cycle_Count();
    s_probe.wait = 0;
}

#define PROBE_ITERATION(rate) ProbeIteration(rate)
#define PROBE_WAIT_START()    (s_probe.waitStart = Get_SysTick_Cycle_Count())
#define PROBE_WAIT_END()      (s_probe.wait += Get_SysTick_Cycle_Count() - s_probe.waitStart)
#else
#define PROBE_ITERATION(rate)
#define PROBE_WAIT_START()
#define PROBE_WAIT_END()
#endif

/* Longest time to wait for one stride of microphone data. Anything much
 * longer than a stride means that the stream is not delivering data. */
#define AEC_MIC_WAIT_TIMEOUT_MS 3000

/**
 * Wait for a full stride of data from one microphone stream, with a timeout
 * so that a dead stream is reported instead of silently stalling the loop.
 * @return 0 on success, non-zero on error or timeout.
 */
static int WaitForMic(audio_mic_t mic, const char* name)
{
    const uint32_t start = Get_SysTick_Count();
    int err = 0;
    while (hal_get_audio_samples_received_ex(mic) < AUDIO_STRIDE) {
        if (Get_SysTick_Count() - start > AEC_MIC_WAIT_TIMEOUT_MS) {
            printf_err("%s mic: no data, got %d/%d samples in %d ms\n",
                       name,
                       hal_get_audio_samples_received_ex(mic),
                       AUDIO_STRIDE,
                       AEC_MIC_WAIT_TIMEOUT_MS);
            return -3;
        }
        __WFE();
    }
    err = hal_wait_for_audio_ex(mic);
    return err;
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
        [[maybe_unused]] auto& profiler = ctx.Get<Profiler&>("profiler");  // KWS only
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
            err = hal_audio_alif_init_ex(AEC_MIC_L, audioRate);
            if (err) {
                printf_err("hal_audio_alif_init_ex(" AEC_MIC_L_NAME ") failed with error: %d\n", err);
                return false;
            }
            err = hal_audio_alif_init_ex(AEC_MIC_R, audioRate);
            if (err) {
                printf_err("hal_audio_alif_init_ex(" AEC_MIC_R_NAME ") failed with error: %d\n", err);
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
#if defined(ETH_STREAM)
            // Streaming is optional: on failure keep running with the speaker only.
            err = eth_stream_init(audioRate);
            if (err) {
                printf_err("eth_stream_init failed with error: %d\n", err);
            }
#endif
        }
        // set_audio_gain(30);

        std::memset(audio_out_a, 0, sizeof(audio_out_a));
        std::memset(audio_out_b, 0, sizeof(audio_out_b));
        std::memset(audio_out_c, 0, sizeof(audio_out_c));

        // Start first fill of final stride section of both mic buffers.
        hal_get_audio_data_ex(AEC_MIC_L, audio_inf     + AUDIO_SAMPLES * AEC_CHANNELS, AUDIO_STRIDE);
        hal_get_audio_data_ex(AEC_MIC_R, audio_inf_pdm + AUDIO_SAMPLES * AEC_CHANNELS, AUDIO_STRIDE);

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
            PROBE_ITERATION(audioRate);

            // Fill audio_out_fill from the PREVIOUS iteration's preprocessed
            // mic data (already in audio_inf / audio_inf_pdm from last iter's
            // tail). Doing this BEFORE the mic wait means the next DAC buffer
            // is queued near the top of the stride, giving the DAC_Callback
            // most of a stride of margin before it needs data_ready=true.
#if defined(USE_STEREO)
            // The first stream is already interleaved L/R, which is the DAC format.
            std::memcpy(audio_out_fill,
                        audio_inf + (AUDIO_SAMPLES - AUDIO_STRIDE) * AEC_CHANNELS,
                        AUDIO_STRIDE * AEC_CHANNELS * sizeof(int16_t));
#else
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
#endif // USE_STEREO

#if defined(ETH_STREAM)
            // Send the same stride to the network before waiting for the DAC,
            // so the network copy doesn't wait for the speaker. Does not
            // block; audio_out_fill is not rewritten for another two strides.
            eth_stream_send(audio_out_fill, AUDIO_STRIDE);
#endif

            // Spin until the DAC queue has room. With triple-buffered priming
            // this spin is where main sleeps for most of each stride, pinning
            // iteration rate to the DAC rate and eliminating drift.
            PROBE_WAIT_START();
            while (audio_out_transmit(audio_out_fill, AUDIO_STRIDE * 2) == -1) {
                __WFE();
            }
            PROBE_WAIT_END();

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

            // Now spend the rest of the stride waiting for the next mic data and
            // preprocessing it. All of this happens in parallel with the DAC
            // DMA'ing the buffer we just queued.
            PROBE_WAIT_START();
            err = WaitForMic(AEC_MIC_L, AEC_MIC_L_NAME);
            if (err) {
                printf_err("Waiting for " AEC_MIC_L_NAME " audio failed with error: %d\n", err);
                return false;
            }
            err = WaitForMic(AEC_MIC_R, AEC_MIC_R_NAME);
            if (err) {
                printf_err("Waiting for " AEC_MIC_R_NAME " audio failed with error: %d\n", err);
                return false;
            }
            PROBE_WAIT_END();

            std::memmove(audio_inf,
                         audio_inf + AUDIO_STRIDE * AEC_CHANNELS,
                         AUDIO_SAMPLES * AEC_CHANNELS * sizeof(int16_t));
            std::memmove(audio_inf_pdm,
                         audio_inf_pdm + AUDIO_STRIDE * AEC_CHANNELS,
                         AUDIO_SAMPLES * AEC_CHANNELS * sizeof(int16_t));
            __disable_irq();
            hal_get_audio_data_ex(AEC_MIC_L, audio_inf + AUDIO_SAMPLES * AEC_CHANNELS, AUDIO_STRIDE);
            hal_get_audio_data_ex(AEC_MIC_R, audio_inf_pdm + AUDIO_SAMPLES * AEC_CHANNELS, AUDIO_STRIDE);
            __enable_irq();

            hal_audio_alif_preprocessing_ex(AEC_MIC_L,
                                            audio_inf + (AUDIO_SAMPLES - AUDIO_STRIDE) * AEC_CHANNELS,
                                            AUDIO_STRIDE);
            hal_audio_alif_preprocessing_ex(AEC_MIC_R,
                                            audio_inf_pdm + (AUDIO_SAMPLES - AUDIO_STRIDE) * AEC_CHANNELS,
                                            AUDIO_STRIDE);

#if defined(AEC_RUN_KWS)
#if defined(USE_STEREO)
            for (int i = 0; i < AUDIO_SAMPLES; ++i) {
                audio_inf_mono[i] = audio_inf[AEC_CHANNELS * i];
            }
            const int16_t* inferenceWindow = audio_inf_mono;
#else
            const int16_t* inferenceWindow = audio_inf;
#endif

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
#endif // AEC_RUN_KWS

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
