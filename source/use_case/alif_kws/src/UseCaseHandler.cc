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

#include "Classifier.hpp"
#include "MicroNetKwsModel.hpp"
#include "hal.h"
#include "AudioUtils.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "KwsResult.hpp"
#include "log_macros.h"
#include "KwsProcessing.hpp"
#include "services_lib_api.h"
#include "services_main.h"

#include "arm_mve.h"
#include <atomic>
#include <vector>
#include <deque>

extern uint32_t m55_comms_handle;
m55_data_payload_t mhu_data;

using KwsClassifier = arm::app::Classifier;
using arm::app::Profiler;
using arm::app::ClassificationResult;
using arm::app::ApplicationContext;
using arm::app::Model;
using arm::app::KwsPreProcess;
using arm::app::KwsPostProcess;
using arm::app::MicroNetKwsModel;

std::atomic_bool data_received;

static void audio_callback(uint32_t /*event*/)
{
    data_received = true;
}

#define AUDIO_SAMPLES 16000 // 16k samples/sec, 1sec sample
#define AUDIO_STRIDE 8000 // 0.5 seconds
#define NEW_SAMPLE_START (AUDIO_SAMPLES - AUDIO_STRIDE) // 0.5 second
#define MAX_GAIN 1000.0f
#define RESULTS_MEMORY 8

static int16_t audio_rec[AUDIO_STRIDE * 2] __attribute__((section(".bss.audio_rec"))); // stereo record buffer
static int16_t audio_in[AUDIO_SAMPLES]; // mono raw data
static int16_t audio_inf[AUDIO_SAMPLES]; // gain and DC adjusted data

namespace alif {
namespace app {

namespace audio {
using namespace arm::app::audio;
}

namespace kws {
using namespace arm::app::kws;
}

std::deque<kws::KwsResult> infResults;
std::vector<ClassificationResult> singleInfResult;
/* We expect to be sampling 1 second worth of data at a time.
*  NOTE: This is only used for time stamp calculation. */
const float secondsPerSample = 1.0 / audio::MicroNetKwsMFCC::ms_defaultSamplingFreq;

 /**
 * @brief           Presents KWS inference results.
 * @param[in]       results     Vector of KWS classification results to be displayed.
 * @return          true if successful, false otherwise.
 **/
static bool PresentInferenceResult(const std::deque<arm::app::kws::KwsResult>& results);

static std::string last_label;

static void send_msg_if_needed(arm::app::kws::KwsResult &result)
{
    mhu_data.id = 2; // id for M55_HE
    if (result.m_resultVec.empty()) {
        last_label.clear();
        return;
    }

    arm::app::ClassificationResult classification = result.m_resultVec[0];

    if (classification.m_label != last_label) {
        if (classification.m_label == "go" || classification.m_label == "stop") {
            info("******************* send_msg_if_needed, FOUND \"%s\", copy data end send! ******************\n", classification.m_label.c_str());
            strcpy(mhu_data.msg, classification.m_label.c_str());
            __DMB();
            SERVICES_send_msg(m55_comms_handle, &mhu_data);
        }
        last_label = classification.m_label;
    }
}

// Perform stereo->mono conversion as we copy
static void copy_audio_rec_to_in()
{
#if 0
    // ARM Compiler can auto-vectorise this to:
    // VLDRW.U32 Q0,[R1],#16   ; load 4 16-bit stereo pairs
    // MOVLB.S16 Q1,Q0         ; separate stereo pairs to 32-bit in two registers
    // MOVLT.S16 Q0,Q0
    // VADD.I32  Q0,Q0,Q1      ; add
    // VSHR.U32  Q0,Q0,#1      ; shift
    // VSTRH.32  Q0,[R0],#8    ; store 4 16-bit mono samples
    // I've failed to figure out an expression to persuade it to use VHADD...
    for (unsigned int i = 0; i < AUDIO_STRIDE; i++) {
        audio_in[NEW_SAMPLE_START + i] = ((uint_fast32_t) audio_rec[i*2] + audio_rec[i*2+1]) >> 1;
    }
#else
    // Optimal(?) vectorisation using stride load
    // VLD20.16  {Q0,Q1},[R1]  ; deinterleaving load, stride 2
    // VLD21.16  {Q0,Q1},[R1]! ; load 8 left samples into Q0 and 8 right samples into Q1
    // VHADD.S16 Q0,Q0,Q1      ; halving add
    // VSTRH.16  Q0,[R0],#16   ; store 8 mono samples
    const int16_t *input = audio_rec;
    int16_t *output = audio_in + NEW_SAMPLE_START;
    int samples_to_go = AUDIO_STRIDE;
    while (samples_to_go > 0) {
        // Create tail predicate - if samples_to_go < 8, then limit following ops
        // This doesn't create a literal VCTP instruction - compiler forms a DLTP/LETP loop.
        mve_pred16_t p = vctp16q(samples_to_go);
        // No predication possible on interleaved load, so will overrun end of array
        int16x8x2_t stereo = vld2q(input);
        // Add and halve left and right (predicated)
        int16x8_t mono = vrhaddq_m(stereo.val[0], stereo.val[0], stereo.val[1], p);
        // Store 8 output values (or less)
        vst1q_p(output, mono, p);
        input += 16;
        output += 8;
        samples_to_go -= 8;
    }
#endif
}

static void apply_gain(int16_t *ptr, int length, float16_t gain)
{
    // Outside the loop to avoid warnings about not being set for the merging convertR operation
    float16x8_t fp;
    while (length > 0) {
        // Check whether we're doing 8 or fewer
        mve_pred16_t p = vctp16q(length);
        // Load up to 8 samples
        int16x8_t data = vld1q_z(ptr, p);
        // Convert to float16_t
        fp = vcvtq_m(fp, data, p);
        // Multiply by gain
        fp = vmulq_m(fp, fp, gain, p);
        // Convert back to int16_t, saturating
        data = vcvtq_m_s16_f16(data, fp, p);
        // Store back up to 8 samples
        vst1q_p(ptr, data, p);
        ptr += 8;
        length -= 8;
    }
}

bool process_audio(arm::app::ApplicationContext& ctx)
{
	int16_t audio_mean, audio_absmax;

    arm_mean_q15(audio_in, AUDIO_SAMPLES, &audio_mean);
    arm_absmax_no_idx_q15(audio_in, AUDIO_SAMPLES, &audio_absmax);
    if (audio_absmax == INT16_MIN) audio_absmax = INT16_MAX; // CMSIS-DSP issue #66
    printf("Original sample stats: absmax = %d, mean = %d\n", audio_absmax, audio_mean);

    // Correct DC offset for entire buffer (previous and new samples)
    // and copy it to audio_inf (so audio_in is always raw mono samples, and
    // gain/DC adjustments are always applied equally to the whole one second
    // the inference sees).
    arm_offset_q15(audio_in, -audio_mean, audio_inf, AUDIO_SAMPLES);

    // Find maximum of inference window
    arm_absmax_no_idx_q15(audio_inf, AUDIO_SAMPLES, &audio_absmax);
    if (audio_absmax == INT16_MIN) audio_absmax = INT16_MAX; // CMSIS-DSP issue #66

    // Rescale to full range
    // First attempt used arm_scale_q15, but that had accuracy problems (CMSIS-DSP issue #57)
    // Local FP16 version loses a bit of precision, compared to what arm_scale_q15 should do, but good enough...
    float16_t gain = std::min(32768.0f / audio_absmax, MAX_GAIN);
    apply_gain(audio_inf, AUDIO_SAMPLES, gain);

    arm_mean_q15(audio_inf, AUDIO_SAMPLES, &audio_mean);
    arm_absmax_no_idx_q15(audio_inf, AUDIO_SAMPLES, &audio_absmax);
    if (audio_absmax == INT16_MIN) audio_absmax = INT16_MAX; // CMSIS-DSP issue #66
    printf("Normalized sample stats: absmax = %d, mean = %d (gain = %.0f dB)\n", audio_absmax, audio_mean, 20 * log10f(gain) );

    auto& profiler = ctx.Get<Profiler&>("profiler");
    auto& model = ctx.Get<Model&>("model");
    auto index = ctx.Get<int>("index");
    auto& preProcess = ctx.Get<KwsPreProcess&>("preproc");
    auto& postProcess = ctx.Get<KwsPostProcess&>("postproc");
    const auto scoreThreshold = ctx.Get<float>("scoreThreshold");

    const int16_t* inferenceWindow = audio_inf;

    /* Because we are adjusting the gain, we can't reuse the MFCC cache for the last half second */
    preProcess.m_audioWindowIndex = 0;

    uint32_t start = ARM_PMU_Get_CCNTR();
    /* Run the pre-processing, inference and post-processing. */
    if (!preProcess.DoPreProcess(inferenceWindow, audio::MicroNetKwsMFCC::ms_defaultSamplingFreq)) {
        printf_err("Pre-processing failed.");
        return false;
    }
    printf("Preprocessing time = %.3f ms\n", (double) (ARM_PMU_Get_CCNTR() - start) / SystemCoreClock * 1000);

    start = ARM_PMU_Get_CCNTR();
    if (!RunInference(model, profiler)) {
        printf_err("Inference failed.");
        return false;
    }
    printf("Inference time = %.3f ms\n", (double) (ARM_PMU_Get_CCNTR() - start) / SystemCoreClock * 1000);

    start = ARM_PMU_Get_CCNTR();
    if (!postProcess.DoPostProcess()) {
        printf_err("Post-processing failed.");
        return false;
    }
    printf("Postprocessing time = %.3f ms\n", (double) (ARM_PMU_Get_CCNTR() - start) / SystemCoreClock * 1000);

    /* Add results from this window to our final results vector. */
    if (infResults.size() == RESULTS_MEMORY) {
        infResults.pop_front();
    }
    infResults.emplace_back(kws::KwsResult(singleInfResult,
            index * secondsPerSample * preProcess.m_audioDataStride,
            index, scoreThreshold));

    send_msg_if_needed(infResults.back());

#if VERIFY_TEST_OUTPUT
    DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */

    // move 0.5s new data to the left so now there is new data in 0 - 7999 waiting for next call to this function and this is then the old data
    memmove(audio_in, audio_in + AUDIO_STRIDE, NEW_SAMPLE_START*sizeof(audio_inf[0]));
    ctx.Set<int>("index", index + 1);

    hal_lcd_clear(COLOR_BLACK);

    if (!PresentInferenceResult(infResults)) {
        return false;
    }

    profiler.PrintProfilingResult();

    singleInfResult.clear();

    return true;
}


    /* KWS inference handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx)
    {
        auto& model = ctx.Get<Model&>("model");
        const auto mfccFrameLength = ctx.Get<int>("frameLength");
        const auto mfccFrameStride = ctx.Get<int>("frameStride");

        constexpr int minTensorDims = static_cast<int>(
            (MicroNetKwsModel::ms_inputRowsIdx > MicroNetKwsModel::ms_inputColsIdx)?
             MicroNetKwsModel::ms_inputRowsIdx : MicroNetKwsModel::ms_inputColsIdx);

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        /* Get Input and Output tensors for pre/post processing. */
        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < minTensorDims) {
            printf_err("Input tensor dimension should be >= %d\n", minTensorDims);
            return false;
        }

        /* Get input shape for feature extraction. */
        TfLiteIntArray* inputShape = model.GetInputShape(0);
        const uint32_t numMfccFeatures = inputShape->data[MicroNetKwsModel::ms_inputColsIdx];
        const uint32_t numMfccFrames = inputShape->data[arm::app::MicroNetKwsModel::ms_inputRowsIdx];

        /* Set up pre and post-processing. */
        KwsPreProcess preProcess = KwsPreProcess(inputTensor, numMfccFeatures, numMfccFrames,
                                                 mfccFrameLength, mfccFrameStride);

        KwsPostProcess postProcess = KwsPostProcess(outputTensor, ctx.Get<KwsClassifier &>("classifier"),
                                                    ctx.Get<std::vector<std::string>&>("labels"),
                                                    singleInfResult);

        ctx.Set<int>("index", 0);
        ctx.Set<KwsPreProcess&>("preproc", preProcess);
        ctx.Set<KwsPostProcess&>("postproc", postProcess);

        hal_set_audio_callback(audio_callback);

        // sample rate is 16Khz and we support only stereo so AUDIO_ARRAY_SIZE is 32kHz to get 1s audio samples
        auto err = hal_get_audio_data(audio_rec, AUDIO_STRIDE * 2);
        while (err == 0) {
            while (!data_received.exchange(false)) {
                __WFE();
            }
            // start receiving new audio and process just received audio stream

            // transfer into processing buffer while converting to mono
            copy_audio_rec_to_in();

            err = hal_get_audio_data(audio_rec, AUDIO_STRIDE * 2);
            if (process_audio(ctx) == false) {
                printf_err("process_audio failed\n");
                return false;
            }
        }
        return false;
    }

    static bool PresentInferenceResult(const std::deque<kws::KwsResult>& results)
    {
        constexpr uint32_t dataPsnTxtStartX1 = 20;
        constexpr uint32_t dataPsnTxtStartY1 = 30;
        constexpr uint32_t dataPsnTxtYIncr   = 16;  /* Row index increment. */

        hal_lcd_set_text_color(COLOR_GREEN);
        info("Final results:\n");
        info("Total number of inferences: %zu\n", results.size());

        /* Display each result */
        uint32_t rowIdx1 = dataPsnTxtStartY1 + 2 * dataPsnTxtYIncr;

        for (const auto& result : results) {

            std::string topKeyword{"<none>"};
            float score = 0.f;
            if (!result.m_resultVec.empty()) {
                topKeyword = result.m_resultVec[0].m_label;
                score = result.m_resultVec[0].m_normalisedVal;
            }

            std::string resultStr =
                    std::string{"@"} + std::to_string(result.m_timeStamp) +
                    std::string{"s: "} + topKeyword + std::string{" ("} +
                    std::to_string(static_cast<int>(score * 100)) + std::string{"%)"};

            hal_lcd_display_text(resultStr.c_str(), resultStr.size(),
                    dataPsnTxtStartX1, rowIdx1, false);
            rowIdx1 += dataPsnTxtYIncr;

            if (result.m_resultVec.empty()) {
                info("For timestamp: %f (inference #: %" PRIu32
                             "); label: %s; threshold: %f\n",
                     result.m_timeStamp, result.m_inferenceNumber,
                     topKeyword.c_str(),
                     result.m_threshold);
            } else {
                for (uint32_t j = 0; j < result.m_resultVec.size(); ++j) {
                    info("For timestamp: %f (inference #: %" PRIu32
                                 "); label: %s, score: %f; threshold: %f\n",
                         result.m_timeStamp,
                         result.m_inferenceNumber,
                         result.m_resultVec[j].m_label.c_str(),
                         result.m_resultVec[j].m_normalisedVal,
                         result.m_threshold);
                }
            }
        }

        return true;
    }

} /* namespace app */
} /* namespace alif */
