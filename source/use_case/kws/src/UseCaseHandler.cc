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

#include <vector>

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace kws {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace kws */
} /* namespace app */
} /* namespace arm */

// sample rate is 16Khz and we support only stereo so AUDIO_ARRAY_SIZE is 32kHz to get 1s audio samples
#define AUDIO_ARRAY_SIZE 64000
int16_t audioxxx[AUDIO_ARRAY_SIZE] IFM_BUF_ATTRIBUTE;
int16_t audio_recv[AUDIO_ARRAY_SIZE] IFM_BUF_ATTRIBUTE;
int16_t audio_recv2[AUDIO_ARRAY_SIZE] IFM_BUF_ATTRIBUTE;

volatile uint8_t data_received;

void audio_callback(uint32_t /*event*/)
{
    data_received = true;
}

using KwsClassifier = arm::app::Classifier;
#define AUDIO_SOURCE_CONTINOUS 1
namespace arm {
namespace app {

#ifdef AUDIO_SOURCE_CONTINOUS

    /**
     * @brief   Inference is run to audioxxx[0 -15999]
     *          1. iteration data in audioxxx: old data in 0 - 7999 (0.5s), new data is placed in 8000 - 23999 ( 1s).
     *             After inference, data is shifted 8000 -> 0 so now only new data in 0 -15999
     *          2. iteration only new data in 0 -15999, after inference data is shifted 8000 -> 0 so now only new data is in 0 -7999 and ready for next round
     *
     * @param prof
     * @param mod
     * @param preProcess
     * @param postProcess
     * @param audioDataSlider
     * @return true
     * @return false
     */
    /*bool process_audio(Profiler &prof, Model &mod, KwsPreProcess &preProcess, KwsPostProcess &postProcess, audio::SlidingWindow<const int16_t> &audioDataSlider,
                      std::vector<ClassificationResult> &singleInfResult)
    {*/

    bool process_audio2(ApplicationContext& ctx) {
        int64_t mean_val = 0;
        int16_t max_value = 0;
        int16_t min_value = 0;
        int16_t offset = 0;

        // find the mean value so we can add that value to data as currently data negative also for the silence
        // also find the max value so we find out the multipier to increase volume for better inference.
        for (int i = 0; i < AUDIO_ARRAY_SIZE/2; i++) {
            mean_val += audio_recv[i*2];
            // convert first AUDIO_ARRAY_SIZE/2 to mono
            audio_recv[i] = audio_recv[i*2];

            if (audio_recv[i*2] > max_value) {
                max_value = audio_recv[i*2];
            }
            if (audio_recv[i*2] < min_value) {
                min_value = audio_recv[i*2];
            }
        }

        offset = mean_val/(AUDIO_ARRAY_SIZE/2);
        info("offset value is %d, max_value: %d, min_value: %d\n", offset, max_value, min_value);


        int16_t multiplier = 0;
        if ((max_value + min_value) > 0) {
            multiplier = max_value;
            info("using max_value as multiplier: %d\n", multiplier);
        } else {
            multiplier = -min_value;
            info("using min_value as multiplier: %d\n", multiplier);
        }

        int16_t multiplier_max = 32767/multiplier; // max int16_t = 32767
        if (multiplier > multiplier_max) {
            multiplier = multiplier_max;
        }

        info("Final value for multiplier: %d\n", multiplier);

        // Convert stereo to mono and more loud by multiplying data after adding 'good' value to get center level to zero

        int16_t temppi = 0;
        int32_t tmp32 = 0;
        int overf = 0;
        for (int i = 0; i < AUDIO_ARRAY_SIZE/2; i++) {
            //audioxxx[8000 + i] = (audio_recv[i] - offset) * multiplier;

            // normalize data to near zero
            temppi = audio_recv[i] + 1720;

            // increase volume by multiplying by 5 and check for overflows
            multiplier = 8;
            tmp32 = temppi * multiplier;
            if (tmp32 <= -32767) {
                temppi = -32767;
                overf++;
            } else if (tmp32 >= 32767) {
                temppi = 32767;
                overf++;
            } else {
                temppi = temppi * 5;
            }
            audioxxx[AUDIO_ARRAY_SIZE/2 + i] = temppi;
        }

        info("Multiplied with: %d and got %d overflows\n", multiplier, overf);

        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model = ctx.Get<Model&>("model");
        const auto mfccFrameLength = ctx.Get<int>("frameLength");
        const auto mfccFrameStride = ctx.Get<int>("frameStride");
        const auto scoreThreshold = ctx.Get<float>("scoreThreshold");
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;
        constexpr int minTensorDims = static_cast<int>(
            (MicroNetKwsModel::ms_inputRowsIdx > MicroNetKwsModel::ms_inputColsIdx)?
             MicroNetKwsModel::ms_inputRowsIdx : MicroNetKwsModel::ms_inputColsIdx);


        for (int x = 0; x < 2; x++)
        {
            /* Load the model. */
            if (!model.Init(arm::app::tensorArena,
                            sizeof(arm::app::tensorArena),
                            arm::app::kws::GetModelPointer(),
                            arm::app::kws::GetModelLen())) {
                printf_err("Failed to initialise model\n");
                return false;
            }

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

            std::vector<ClassificationResult> singleInfResult;
            KwsPostProcess postProcess = KwsPostProcess(outputTensor, ctx.Get<KwsClassifier &>("classifier"),
                                                        ctx.Get<std::vector<std::string>&>("labels"),
                                                        singleInfResult);
            audio::SlidingWindow<const int16_t> audioDataSlider = audio::SlidingWindow<const int16_t>(
                audioxxx,
                AUDIO_ARRAY_SIZE/2,
                preProcess.m_audioDataWindowSize, preProcess.m_audioDataStride);

            audioDataSlider.Reset(audioxxx);

            /* Start sliding through audio clip. */
            while (audioDataSlider.HasNext()) {
                const int16_t* inferenceWindow = audioDataSlider.Next();

                /* The first window does not have cache ready. */
                preProcess.m_audioWindowIndex = audioDataSlider.Index();

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

                ClassificationResult res = singleInfResult.back();
                printf("**********************************************************************************\n");
                printf("       Found keyword %s with propability value : %f and number of results: %d\n", res.m_label.c_str(), res.m_normalisedVal, singleInfResult.size());
                printf("**********************************************************************************\n");

            }

            if (x == 0) {
                // move 1.0 /2.0s new data to beginning
                memmove(audioxxx, audioxxx + AUDIO_ARRAY_SIZE/2, AUDIO_ARRAY_SIZE*sizeof(audioxxx[0]));
            } else {
                // move 0.5/1s new data to the left so now there is new data in 0 - 7999 waiting for next call to this function and this is then the old data
                memmove(audioxxx, audioxxx + AUDIO_ARRAY_SIZE/2, (AUDIO_ARRAY_SIZE/2)*sizeof(audioxxx[0]));
            }
        }
        return true;
    }


    bool ClassifyAudioHandler(ApplicationContext& ctx)
    {
#if 0
        auto& profiler = ctx.Get<Profiler&>("profiler");
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

        std::vector<ClassificationResult> singleInfResult;
        KwsPostProcess postProcess = KwsPostProcess(outputTensor, ctx.Get<KwsClassifier &>("classifier"),
                                                    ctx.Get<std::vector<std::string>&>("labels"),
                                                    singleInfResult);

        /* Creating a sliding window through the whole audio clip. */
        auto audioDataSlider = audio::SlidingWindow<const int16_t>(
                audio0,
                AUDIO_ARRAY_SIZE/2,
                preProcess.m_audioDataWindowSize, preProcess.m_audioDataStride);
#endif
        hal_set_audio_callback(audio_callback);

        // sample rate is 16Khz and we support only stereo so AUDIO_ARRAY_SIZE is 32kHz to get 1s audio samples
        auto err = hal_get_audio_data(audio_recv2, AUDIO_ARRAY_SIZE);
        while (err == 0) {
            if (data_received) {
                // start receiving new audio and process just received audio stream
                data_received = false;
                memcpy(audio_recv, audio_recv2, AUDIO_ARRAY_SIZE*sizeof(audio_recv2[0]));
                err = hal_get_audio_data(audio_recv2, AUDIO_ARRAY_SIZE);
                //if (process_audio(profiler, model, preProcess, postProcess, audioDataSlider, singleInfResult) == false) {
                if (process_audio2(ctx) == false) {
                    printf_err("process_audio failed\n");
                    return false;
                }
            }
        }
            /*
            for (int i = 0; i < AUDIO_ARRAY_SIZE/2; i++) {
                const int8_t *p = (const int8_t*)&audioxxx[i*2];
                printf("%02hhX ", *p);
                printf("%02hhX ", *(p + 1));
                if (!((i+1) % 24)) {
                    printf("\n");
                }
            }*/


#if 0
            // prints as little endian, like in memory stream
            const int8_t *p = (const int8_t*)&audioxxx[0];
            for (int i = 0; i < AUDIO_ARRAY_SIZE; i++) {
                printf("%02hhX", p[i]);
                if (!((i+1) % 48)) {
                    printf("\n");
                } else if (i%2 == 0) {
                    printf(" ");
                }
            }

            continue;
#endif

        return false;
    }
#else
    /**
     * @brief           Presents KWS inference results.
     * @param[in]       results     Vector of KWS classification results to be displayed.
     * @return          true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(const std::vector<kws::KwsResult>& results);


/* Model parameters for kws ;
Nämä tulloo modelilta, ei pitäisi olla mitenkään riippuvainen näytteen pituuteen!
extern const int   g_FrameLength    = 640;
extern const int   g_FrameStride    = 320;
extern const float g_ScoreThreshold = 0.7;
*/

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


        /* We expect to be sampling 1 second worth of data at a time.
         * NOTE: This is only used for time stamp calculation. */
        const float secondsPerSample = 1.0 / audio::MicroNetKwsMFCC::ms_defaultSamplingFreq;



        hal_set_audio_callback(audio_callback);



        int err = 0;
        int rounds = 0;
        /* Loop to process audio clips. */
        do {

            /* Load the model. */
            if (!model.Init(arm::app::tensorArena,
                            sizeof(arm::app::tensorArena),
                            arm::app::kws::GetModelPointer(),
                            arm::app::kws::GetModelLen())) {
                printf_err("Failed to initialise model\n");
                return false;
            }

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

            std::vector<ClassificationResult> singleInfResult;
            KwsPostProcess postProcess = KwsPostProcess(outputTensor, ctx.Get<KwsClassifier &>("classifier"),
                                                        ctx.Get<std::vector<std::string>&>("labels"),
                                                        singleInfResult);
            const int16_t *arr = get_audio_array(0);
            audio::SlidingWindow<const int16_t> audioDataSlider = audio::SlidingWindow<const int16_t>(
                get_audio_array(0),
                get_audio_array_size(0),
                preProcess.m_audioDataWindowSize, preProcess.m_audioDataStride);

            hal_lcd_clear(COLOR_BLACK);
            rounds++;

            auto currentIndex = ctx.Get<uint32_t>("clipIndex");

            if (rounds == 1) {
                info("*************** Using get_audio_array, this should SUCCEED!!! *************\n");
            } else if (rounds == 2) {
                memcpy(audioxxx, get_audio_array(0), 16000*sizeof(arr[0]));
                audioDataSlider.Reset(audioxxx);
                info("*************** Using audioxxx, memcopied from get_audio_array, this should SUCCEED!!! *************\n");
            } else if (rounds == 3) {
                memset(audioxxx, 0 , 16000*sizeof(arr[0]));
                audioDataSlider.Reset(audioxxx);
                info("*************** Using audioxxx with memset to zero, this should fail!!! *************\n");
            } else if (rounds == 4) {
                audioDataSlider.Reset(audio_recv);
                info("*************** Using audio_recv, this should fail!!! *************\n");
            } else if (rounds == 5) {
                audioDataSlider.Reset(audio_recv2);
                info("*************** Using audio_recv2, this should fail!!! *************\n");
            } else if (rounds == 6) {
                audioDataSlider.Reset(get_audio_array(0));
                info("*************** Using audio_recv2, this should SUCCEED!!! *************\n");
            }else if (rounds == 7) {
                audioDataSlider.Reset(audio_recv2);
                info("*************** Using audio_recv2, this should fail!!! *************\n");
            }else if (rounds == 8) {
                audioDataSlider.Reset(get_audio_array(0));
                info("*************** Using audio_recv2, this should SUCCEED!!! *************\n");
            }else if (rounds == 9) {
                audioDataSlider.Reset(audio_recv2);
                info("*************** Using audio_recv2, this should fail!!! *************\n");
            }
            else {
                audioDataSlider.Reset(get_audio_array(0));
                info("*************** Using get_audio_array, this should SUCCEED!!! *************\n");
            }


            /* Declare a container to hold results from across the whole audio clip. */
            std::vector<kws::KwsResult> finalResults;

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

                ClassificationResult res = singleInfResult.back();
                printf("**************************************** Found keyword %s with propability value : %f and number of results: %d *************************\n", res.m_label.c_str(), res.m_normalisedVal, singleInfResult.size());

                /* Add results from this window to our final results vector. */
                finalResults.emplace_back(kws::KwsResult(singleInfResult,
                        audioDataSlider.Index() * secondsPerSample * preProcess.m_audioDataStride,
                        audioDataSlider.Index(), scoreThreshold));

#if VERIFY_TEST_OUTPUT
                DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */
            } /* while (audioDataSlider.HasNext()) */

            /* Erase. */

            ctx.Set<std::vector<kws::KwsResult>>("results", finalResults);

            if (!PresentInferenceResult(finalResults)) {
                return false;
            }

            //profiler.PrintProfilingResult();

            IncrementAppCtxIfmIdx(ctx,"clipIndex");


            // sample rate is 16Khz and we support only stereo so AUDIO_ARRAY_SIZE is 32kHz to get 1s audio samples
            data_received = false;
            err = hal_get_audio_data(audio_recv2, AUDIO_ARRAY_SIZE);
            while (!data_received) {
            }
        } while ((err == 0) && (rounds < 10));

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
                info("****************************** For timestamp: %f (inference #: %" PRIu32
                             "); label: %s; threshold: %f **********************\n",
                     result.m_timeStamp, result.m_inferenceNumber,
                     topKeyword.c_str(),
                     result.m_threshold);
            } else {
                for (uint32_t j = 0; j < result.m_resultVec.size(); ++j) {
                    info("************************** For timestamp: %f (inference #: %" PRIu32
                                 "); label: %s, score: %f; threshold: %f **************************\n",
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
#endif // AUDIO_SOURCE_CONTINOUS
} /* namespace app */
} /* namespace arm */
