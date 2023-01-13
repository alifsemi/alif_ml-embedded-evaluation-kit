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

#include "arm_math_f16.h"
#include "arm_mve.h"
#include <atomic>
#include <vector>

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

#define AUDIO_SAMPLES 16000 // 16k samples/sec, 1sec sample
#define AUDIO_STRIDE 8000 // 0.5 seconds
#define AUDIO_REC_SAMPLES 1600
#define NEW_SAMPLE_START (AUDIO_SAMPLES - AUDIO_STRIDE) // 0.5 second
#define MAX_GAIN 10000.0f // 80dB
#define MAX_GAIN_INC_PER_STRIDE 1.05925373f // 0.5dB, so 1dB per second
#define RESULTS_MEMORY 8

#define AUDIO_L_ONLY 1
#define AUDIO_R_ONLY 2
#define AUDIO_LR_MIX 3

#define AUDIO_MICS  AUDIO_LR_MIX

static void copy_audio_rec_to_in(float16_t * __RESTRICT in, const int32_t * __RESTRICT rec, int samples);

// 24-bit stereo record buffer
static int32_t audio_rec[2][AUDIO_REC_SAMPLES * 2] __attribute__((section(".bss.audio_rec"))); // stereo record buffer

// 16-bit working buffer. New data is initially loaded as float16, then converted to int16 during gain adjustment
static int16_t audio_inf[AUDIO_SAMPLES + AUDIO_STRIDE];
static float current_gain = MAX_GAIN;
static int32_t current_dc = 0;

static int audio_current_rec_buf;
static std::atomic_int data_received;
static std::atomic_int audio_async_error;

static void audio_start_next_rx()
{
    int err = hal_get_audio_data(audio_rec[audio_current_rec_buf], AUDIO_REC_SAMPLES * 2);
    if (err) {
        audio_async_error = err;
    }
}

static void audio_callback(uint32_t /*event*/)
{
    audio_current_rec_buf = !audio_current_rec_buf;
    int new_samples = data_received + AUDIO_REC_SAMPLES;
    if (new_samples < AUDIO_STRIDE) {
        audio_start_next_rx();
    }
    copy_audio_rec_to_in((float16_t *) audio_inf + AUDIO_SAMPLES + data_received, audio_rec[!audio_current_rec_buf], AUDIO_REC_SAMPLES);
    data_received = new_samples;
}

static void audio_start_stride()
{
    data_received = 0;
    audio_start_next_rx();
}

// Perform stereo->mono conversion and DC adjustment as we copy
// Gain will be handled later. Note that we use 32-bit input -
// the microphone provides 18 bits of precision. Using 24-bit
// mode gives us non-sign-extended data, so 32-bit is nicer.
static void copy_audio_rec_to_in(float16_t * __RESTRICT in, const int32_t * __RESTRICT rec, int len)
{
    const int32_t *input = rec;
    float16_t *output = in;
    int32_t offset = current_dc;
    int64_t sum = 0;
    int samples_to_go = len;
    while (samples_to_go > 0) {
        // Create tail predicate - if samples_to_go < 4, then limit following ops
        // This doesn't create a literal VCTP instruction - compiler forms a DLTP/LETP loop.
        mve_pred16_t p = vctp32q(samples_to_go);
        // No predication possible on interleaved load, so will overrun end of array
        int32x4x2_t stereo = vld2q(input);
#if AUDIO_MICS == AUDIO_LR_MIX
        // Average left and right
        int32x4_t mono = vrhaddq_x(stereo.val[0], stereo.val[1], p);
#elif AUDIO_MICS == AUDIO_L_ONLY
        int32x4_t mono = stereo.val[0];
#elif AUDIO_MICS == AUDIO_R_ONLY
        int32x4_t mono = stereo.val[1];
#else
#error "which microphone?"
#endif
        // Add it up to track the pre-adjustment mean
        sum = vaddlvaq_p(sum, mono, p);
        // Subtract the current DC offset (necessary to not lose accuracy
        // when converting to float16)
        mono = vqsubq_m(vuninitializedq_s32(), mono, offset, p);
        // Convert to float32, in range -1,+1, so as not to overflow float16 calculations later
        float32x4_t mono_f32 = vcvtq_x_n(mono, 31, p);
        // Convert to float16
        float16x8_t mono_f16 = vcvtbq_m_f16_f32(vuninitializedq_f16(), mono_f32, p);
        // Store 4 output values (or less). Just need to store bottom 16 bits
        // of each 32-bit word - can't easily express this with intrinisic types
        vstrhq_p_u32((uint16_t *)output, vreinterpretq_u32(mono_f16), p);
        input += 8;
        output += 4;
        samples_to_go -= 4;
    }
    // Update the DC offset based on the mean of this buffer
    int32_t mean = (int32_t) (sum / len);
    current_dc = (current_dc / 8) * 7 + mean / 8;
}

namespace alif {
namespace app {

namespace audio {
using namespace arm::app::audio;
}

namespace kws {
using namespace arm::app::kws;
}

std::vector<kws::KwsResult> infResults;
std::vector<ClassificationResult> singleInfResult;
/* We expect to be sampling 1 second worth of data at a time.
*  NOTE: This is only used for time stamp calculation. */
const float secondsPerSample = 1.0 / audio::MicroNetKwsMFCC::ms_defaultSamplingFreq;

 /**
 * @brief           Presents KWS inference results.
 * @param[in]       results     Vector of KWS classification results to be displayed.
 * @return          true if successful, false otherwise.
 **/
static bool PresentInferenceResult(const std::vector<arm::app::kws::KwsResult>& results);

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

static void apply_gain(int16_t *ptr, int length, float16_t gain)
{
    while (length > 0) {
        // Check whether we're doing 8 or fewer
        mve_pred16_t p = vctp16q(length);
        // Load up to 8 samples
        float16x8_t fp = vld1q_z((float16_t *)ptr, p);
        // Multiply by gain
        fp = vmulq_x(fp, gain, p);
        // Convert back to int16_t, rescaling for q15 (can't specify rounding with rescale)
        int16x8_t data = vcvtq_x_n_s16_f16(fp, 15, p);
        // Store back up to 8 samples
        vst1q_p(ptr, data, p);
        ptr += 8;
        length -= 8;
    }
}

bool process_audio(int16_t *audio, int samples)
{
    float16_t *audio_fp = (float16_t *)audio;
	float16_t audio_mean, audio_absmax;

    arm_mean_f16(audio_fp, samples, &audio_mean);
    arm_absmax_no_idx_f16(audio_fp, samples, &audio_absmax);
    //if (audio_absmax == INT16_MIN) audio_absmax = INT16_MAX; // CMSIS-DSP issue #66
    printf("Original sample stats: absmax = %ld, mean = %ld\n", lround(32768*audio_absmax), lround(32768*audio_mean));


    // Rescale to full range  while converting to integer
    float new_gain = std::fmin(1.0f / audio_absmax, MAX_GAIN);
    // Reduce gain immediately if necessary to avoid clipping, or increase slowly
    current_gain = std::fmin(new_gain, current_gain * MAX_GAIN_INC_PER_STRIDE);

    apply_gain(audio, samples, current_gain);

    q15_t audio_mean_q15, audio_absmax_q15;
    arm_mean_q15(audio, samples, &audio_mean_q15);
    arm_absmax_no_idx_q15(audio, samples, &audio_absmax_q15);
    if (audio_absmax_q15 == INT16_MIN) audio_absmax_q15 = INT16_MAX; // CMSIS-DSP issue #66
    printf("Normalized sample stats: absmax = %d, mean = %d (gain = %.0f dB)\n", audio_absmax_q15, audio_mean_q15, 20 * log10f(current_gain) );

    return true;
}

//static void get_one_audio_stride


    /* KWS inference handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx)
    {
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model = ctx.Get<Model&>("model");
        const auto mfccFrameLength = ctx.Get<int>("frameLength");
        const auto mfccFrameStride = ctx.Get<int>("frameStride");
        const auto scoreThreshold = ctx.Get<float>("scoreThreshold");
        int index = ctx.Get<int>("index");

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


        int err;

        if (index == 0) {
            err = hal_audio_init(audio::MicroNetKwsMFCC::ms_defaultSamplingFreq, 32);
            if (err) {
                printf_err("hal_audio_init failed with error: %d\n", err);
                return false;
            }

            hal_set_audio_callback(audio_callback);

            // Start first fill of final stride section of buffer
            audio_start_stride();
        }

        // Wait until stride buffer is full - intiated above or by previous call to ClassifyAudioHandler
        while (data_received < AUDIO_STRIDE && audio_async_error == 0) {
            __WFE();
        }
        err = audio_async_error;
        if (err) {
            printf_err("hal_get_audio_data failed with error: %d\n", err);
            return false;
        }

        // move buffer down by one stride, clearing space at the end for the next stride
        memmove(audio_inf, audio_inf + AUDIO_STRIDE, AUDIO_SAMPLES * sizeof audio_inf[0]);

        // start receiving the next stride immediately before we start heavy processing, so as not to lose anything
        audio_start_stride();

        process_audio(audio_inf + AUDIO_SAMPLES - AUDIO_STRIDE, AUDIO_STRIDE);

        const int16_t* inferenceWindow = audio_inf;

        preProcess.m_audioWindowIndex = index;

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
            infResults.erase(infResults.begin());
        }
        infResults.emplace_back(kws::KwsResult(singleInfResult,
                index * secondsPerSample * preProcess.m_audioDataStride,
                index, scoreThreshold));

        send_msg_if_needed(infResults.back());

    #if VERIFY_TEST_OUTPUT
        DumpTensor(outputTensor);
    #endif /* VERIFY_TEST_OUTPUT */

        ctx.Set<int>("index", index + 1);

        hal_lcd_clear(COLOR_BLACK);

        if (!PresentInferenceResult(infResults)) {
            return false;
        }

        profiler.PrintProfilingResult();

        return true;
    }

    static bool PresentInferenceResult(const std::vector<kws::KwsResult>& results)
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
