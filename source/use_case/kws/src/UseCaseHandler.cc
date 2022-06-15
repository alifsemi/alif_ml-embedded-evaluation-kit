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

#include "InputFiles.hpp"
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

#include <vector>

extern uint32_t m55_comms_handle;
m55_data_payload_t mhu_data;


using KwsClassifier = arm::app::Classifier;

#define RECORD_AUDIO

#ifdef RECORD_AUDIO
volatile uint8_t data_received;

void audio_callback(uint32_t /*event*/)
{
    data_received = true;
}

#define AUDIO_SAMPLES (32000) // 16k samples/sec, 1sec sample, stereo

static int16_t audio_rec[AUDIO_SAMPLES] IFM_BUF_ATTRIBUTE;
static int16_t audio_max;
static int16_t audio_min;
static int16_t audio_average;

namespace arm {
namespace app {

static void send_msg_if_needed(std::vector<kws::KwsResult> &results)
{
    mhu_data.id = 2; // id for M55_HE
    for (const auto& result : results) {
        if (result.m_resultVec.empty()) {
            // no results, return fast
            return;
        }

        for (uint32_t j = 0; j < result.m_resultVec.size(); ++j) {
            if (result.m_resultVec[j].m_normalisedVal > 0.7) {
                if (result.m_resultVec[j].m_label == "go") {
                    memcpy(mhu_data.msg, result.m_resultVec[j].m_label.c_str(), strlen(result.m_resultVec[j].m_label.c_str()));
                    SERVICES_send_msg(m55_comms_handle, &mhu_data);

                } else if (result.m_resultVec[j].m_label == "stop") {
                    memcpy(mhu_data.msg, result.m_resultVec[j].m_label.c_str(), strlen(result.m_resultVec[j].m_label.c_str()));
                    SERVICES_send_msg(m55_comms_handle, &mhu_data);
                }
            }
        }
    }
}

void calc_audio_stats(const int16_t* audio, size_t samples)
{
    int64_t total = 0;
    audio_min = 0;
    audio_max = 0;
    audio_average = 0;
    for (unsigned int i = 0; i < samples; i++) {
        total += audio[i];
        if (audio[i] > audio_max)
            audio_max = audio[i];
        if (audio[i] < audio_min)
            audio_min = audio[i];
    }
    audio_average = total / (samples);
}

int record_audio(int16_t* audio_buffer, size_t samples)
{
    hal_set_audio_callback(audio_callback);

    info("Recording audio...\n");
    int err = hal_get_audio_data(audio_buffer, samples);
    while (!data_received);
    data_received = false;

    // Convert sample to mono, after this only samples/2 of data is valid in the buffer
    for (unsigned int i = 0; i < samples/2; i++) {
        audio_buffer[i] = (audio_buffer[i*2] >> 1) + (audio_buffer[i*2+1] >> 1);
    }
    calc_audio_stats(audio_buffer, samples/2);
    printf("Original sample stats: min = %d, max = %d, average = %d\n", audio_min, audio_max, audio_average);

#if 1
    // Normalize and add some gain
    for (unsigned int i = 0; i < samples/2; i++) {
        audio_buffer[i] -= audio_average;
        audio_buffer[i] <<= 2;
    }
#endif
    __DSB();
    calc_audio_stats(audio_buffer, samples/2);
    printf("Normalized sample stats: min = %d, max = %d, average = %d\n", audio_min, audio_max, audio_average);

#if 0
    // Print out audio sample
    const int8_t *p = (const int8_t*)audio_buffer;
    for (unsigned int i = 0; i < samples; i++) {
        printf("%02hhX ", p[i]);
        if (!((i+1) % 48)) {
            printf("\n");
        }
    }
    printf("\n");
#endif
    return err;
}
#endif // #ifdef RECORD_AUDIO



    /**
     * @brief           Presents KWS inference results.
     * @param[in]       results     Vector of KWS classification results to be displayed.
     * @return          true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(const std::vector<kws::KwsResult>& results);

    /* KWS inference handler. */
    bool ClassifyAudioHandler(ApplicationContext& ctx, uint32_t clipIndex, bool runAll)
    {
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model = ctx.Get<Model&>("model");
        const auto mfccFrameLength = ctx.Get<int>("frameLength");
        const auto mfccFrameStride = ctx.Get<int>("frameStride");
        const auto scoreThreshold = ctx.Get<float>("scoreThreshold");

        /* If the request has a valid size, set the audio index. */
        if (clipIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, clipIndex,"clipIndex")) {
                return false;
            }
        }
        auto initialClipIdx = ctx.Get<uint32_t>("clipIndex");

        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;
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

        /* We expect to be sampling 1 second worth of data at a time.
         * NOTE: This is only used for time stamp calculation. */
        const float secondsPerSample = 1.0 / audio::MicroNetKwsMFCC::ms_defaultSamplingFreq;

        /* Set up pre and post-processing. */
        KwsPreProcess preProcess = KwsPreProcess(inputTensor, numMfccFeatures, numMfccFrames,
                                                 mfccFrameLength, mfccFrameStride);

        std::vector<ClassificationResult> singleInfResult;
        KwsPostProcess postProcess = KwsPostProcess(outputTensor, ctx.Get<KwsClassifier &>("classifier"),
                                                    ctx.Get<std::vector<std::string>&>("labels"),
                                                    singleInfResult);

        /* Loop to process audio clips. */
        do {
            hal_lcd_clear(COLOR_BLACK);

#ifdef RECORD_AUDIO
            record_audio(audio_rec, AUDIO_SAMPLES);

            auto currentIndex = 0; // ctx.Get<uint32_t>("clipIndex");
#else
            auto currentIndex = ctx.Get<uint32_t>("clipIndex");
#endif

            /* Creating a sliding window through the whole audio clip. */
            auto audioDataSlider = audio::SlidingWindow<const int16_t>(
#ifdef RECORD_AUDIO
                    audio_rec,
                    AUDIO_SAMPLES/2,
#else
                    get_audio_array(currentIndex),
                    get_audio_array_size(currentIndex),
#endif
                    preProcess.m_audioDataWindowSize, preProcess.m_audioDataStride);

            /* Declare a container to hold results from across the whole audio clip. */
            std::vector<kws::KwsResult> finalResults;

            /* Display message on the LCD - inference running. */
            std::string str_inf{"Running inference... "};
            hal_lcd_display_text(str_inf.c_str(), str_inf.size(),
                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);
#ifndef RECORD_AUDIO
            info("Running inference on audio clip %" PRIu32 " => %s\n", currentIndex,
                 get_filename(currentIndex));
#endif

            /* Start sliding through audio clip. */
            while (audioDataSlider.HasNext()) {
                const int16_t* inferenceWindow = audioDataSlider.Next();

                /* The first window does not have cache ready. */
                preProcess.m_audioWindowIndex = audioDataSlider.Index();

                info("Inference %zu/%zu\n", audioDataSlider.Index() + 1,
                     audioDataSlider.TotalStrides() + 1);

                /* Run the pre-processing, inference and post-processing. */
                if (!preProcess.DoPreProcess(inferenceWindow, audio::MicroNetKwsMFCC::ms_defaultSamplingFreq)) {
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

                /* Add results from this window to our final results vector. */
                finalResults.emplace_back(kws::KwsResult(singleInfResult,
                        audioDataSlider.Index() * secondsPerSample * preProcess.m_audioDataStride,
                        audioDataSlider.Index(), scoreThreshold));

#if VERIFY_TEST_OUTPUT
                DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */
            } /* while (audioDataSlider.HasNext()) */

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            hal_lcd_display_text(str_inf.c_str(), str_inf.size(),
                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

            ctx.Set<std::vector<kws::KwsResult>>("results", finalResults);

            if (!PresentInferenceResult(finalResults)) {
                return false;
            }

            profiler.PrintProfilingResult();

            // send the message to HP is correct word was recognized
            send_msg_if_needed(finalResults);

            IncrementAppCtxIfmIdx(ctx,"clipIndex");

#ifdef RECORD_AUDIO
        } while (1);
#else
        } while (runAll && ctx.Get<uint32_t>("clipIndex") != initialClipIdx);
#endif
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
} /* namespace arm */
