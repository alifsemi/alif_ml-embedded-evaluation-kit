/* This file was ported to work on Alif Semiconductor devices. */

/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#if defined(MLEK_FWK_EXECUTORCH)

#include "UseCaseHandler.hpp"

#include "ConformerProcessing.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "Wav2LetterPreprocess.hpp"
#include "hal.h"
#include "log_macros.h"

#include "ScreenLayout.hpp"
#include "board_utils.h"
#include "lv_paint_utils.h"
#include "lv_port.h"
#include "lvgl.h"

#include <cstring>

#define AUDIO_LIVE      (1)

#define LIMAGE_X        (340)
#define LIMAGE_Y        (80)
#define LV_ZOOM         (1.2 * 256)

namespace {
    lv_style_t boxStyle;
    lvgl_pixel_t lvgl_image[LIMAGE_Y][LIMAGE_X] __attribute__((section(".bss.lcd_image_buf")));
};

static const int result_label_idx = 4;

#if AUDIO_LIVE
#define AUDIO_RATE               (16000)
#define AUDIO_CHUNK_SIZE_SAMPLES (2048)
#define AUDIO_CHUNKS             (78) // ~10s max
#define AUDIO_SAMPLES            (AUDIO_CHUNK_SIZE_SAMPLES * AUDIO_CHUNKS)
static int16_t audio_inf[AUDIO_SAMPLES + AUDIO_CHUNK_SIZE_SAMPLES];
#endif

static volatile bool button_pressed = false;
static volatile int button_pressed_sample_count = 0;

void button_cb(uint32_t event)
{
    (void)event;
    button_pressed = true;
    button_pressed_sample_count = get_audio_samples_received();
}

namespace arm {
namespace app {
     bool ClassifyAudioInit()
    {
        alif::app::ScreenLayoutInit(lvgl_image, sizeof(lvgl_image), LIMAGE_X, LIMAGE_Y, LV_ZOOM, true);
        std::memset(lvgl_image, 0, sizeof(lvgl_image));
        uint32_t lv_lock_state = lv_port_lock();
        lv_label_set_text_static(alif::app::ScreenLayoutHeaderObject(), "Conformer ASR (ExecuTorch)");

        lv_style_init(&boxStyle);
        lv_style_set_bg_opa(&boxStyle, LV_OPA_TRANSP);
        lv_style_set_pad_all(&boxStyle, 0);
        lv_style_set_border_width(&boxStyle, 0);
        lv_style_set_outline_width(&boxStyle, 2);
        lv_style_set_outline_pad(&boxStyle, 0);
        lv_style_set_outline_color(
            &boxStyle, lv_theme_get_color_primary(alif::app::ScreenLayoutHeaderObject()));
        lv_style_set_radius(&boxStyle, 4);

        lv_obj_add_flag(alif::app::ScreenLayoutBarObject(), LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_static(alif::app::ScreenLayoutLabelObject(0), "");
        lv_label_set_text_static(alif::app::ScreenLayoutLabelObject(result_label_idx), "");
        lv_obj_set_width(alif::app::ScreenLayoutLabelObject(result_label_idx), 460);
        lv_label_set_long_mode(alif::app::ScreenLayoutLabelObject(result_label_idx),
                               LV_LABEL_LONG_WRAP);

        lv_port_unlock(lv_lock_state);

        BOARD_BUTTON2_Init(button_cb);
        BOARD_BUTTON2_Control(BOARD_BUTTON_ENABLE_INTERRUPT);
        BOARD_LED2_Control(BOARD_LED_STATE_LOW);
        return true;
    }

    // from https://www.andrewnoske.com
    void getHeatMapColor(float value, float* red, float* green, float* blue)
    {
        const int NUM_COLORS              = 4;
        static float color[NUM_COLORS][3] = {
            {0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}};

        int idx1; // |-- Our desired color will be between these two indexes in "color".
        int idx2; // |
        float fractBetween = 0; // Fraction between "idx1" and "idx2" where our value is.

        if (value <= 0) {
            idx1 = idx2 = 0;
        } // accounts for an input <=0
        else if (value >= 1) {
            idx1 = idx2 = NUM_COLORS - 1;
        } // accounts for an input >=0
        else {
            value        = value * (NUM_COLORS - 1); // Will multiply value by 3.
            idx1         = floor(value);             // Our desired color will be after this index.
            idx2         = idx1 + 1;                 // ... and before this index (inclusive).
            fractBetween = value - float(idx1);      // Distance between the two indexes (0-1).
        }

        *red   = (color[idx2][0] - color[idx1][0]) * fractBetween + color[idx1][0];
        *green = (color[idx2][1] - color[idx1][1]) * fractBetween + color[idx1][1];
        *blue  = (color[idx2][2] - color[idx1][2]) * fractBetween + color[idx1][2];
    }

    void drawMelSpec(fwk::iface::TensorIface &inputMelSpec)
    {
        const int input_channels       = inputMelSpec.Shape()[2];
        const int input_visualize_step = 3; // limited image space for spectroram, draw every third
        float* mel_input               = (float*)inputMelSpec.GetData();
        for (int xx = 0; xx < LIMAGE_X; xx++) {
            for (int yy = 0; yy < LIMAGE_Y; yy++) {
                float mel_value =
                    (mel_input[(xx * input_visualize_step * input_channels) + yy] + 1.0f) / 2;

                float fr,fg,fb;
                getHeatMapColor(mel_value, &fr, &fg, &fb);

                lv_color16_t rgb   = {static_cast<uint16_t>((uint16_t)(fb * 255) >> 3),
                                      static_cast<uint16_t>((uint16_t)(fg * 255) >> 2),
                                      static_cast<uint16_t>((uint16_t)(fr *255) >> 3)};
                lvgl_image[yy][xx] = rgb;
            }
        }
        {
            lv_obj_invalidate(alif::app::ScreenLayoutImageObject());
            ScopedLVGLLock lv_lock;
        }
    }

    bool ClassifyAudioHandler(ApplicationContext& ctx)
    {
        auto& model            = ctx.Get<fwk::iface::Model&>("model");
        auto& profiler         = ctx.Get<Profiler&>("profiler");
        auto& labels           = ctx.Get<std::vector<std::string>&>("labels");
        auto melSpecWindowSize = ctx.Get<uint32_t>("melSpecWindowSize");
        auto melSpecHopSize    = ctx.Get<uint32_t>("melSpecHopSize");
        auto chunkSize         = ctx.Get<uint32_t>("chunkSize");

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto inputTensorMelSpec  = model.GetInputTensor(0);
        auto inputTensorChunkSize = model.GetInputTensor(1);
        auto outputTensorLogits  = model.GetOutputTensor(0);
        auto outputTensorChunkSize = model.GetOutputTensor(1);

        auto preProcess = ConformerPreProcess<int16_t>(
            inputTensorMelSpec,
            inputTensorChunkSize,
            melSpecWindowSize,
            melSpecHopSize,
            chunkSize
        );

        std::string decodedResult;

        auto postProcess = ConformerPostProcess(
            outputTensorLogits,
            outputTensorChunkSize,
            labels,
            decodedResult
        );

        ClassifyAudioInit();
#if AUDIO_LIVE
        int err = hal_audio_alif_init(AUDIO_RATE);
        if (err) {
            printf_err("hal_audio_alif_init failed with error: %d\n", err);
            return false;
        }
#else
        hal_audio_init();
        if (!hal_audio_configure(HAL_AUDIO_MODE_SINGLE_BURST,
                                 HAL_AUDIO_FORMAT_16KHZ_MONO_16BIT)) {
            printf_err("Failed to configure audio\n");
            return false;
        }
#endif // AUDIO_LIVE

        /* Loop to process audio clips. */
        while (true) {
#ifdef INTERACTIVE_MODE
            AwaitUserInput(); // Wait for user input before moving forward.
#endif /* INTERACTIVE_MODE */

#if AUDIO_LIVE
            // Wait for button press callback ...
            button_pressed = false;
            uint32_t idle_count = 0;
            while(!button_pressed) {
                // run audio in order to update the automatic gain
                hal_get_audio_data(audio_inf, AUDIO_CHUNK_SIZE_SAMPLES);

                // Blink the UI 'LED'
                {
                    ScopedLVGLLock lv_lock;
                    if (idle_count & 0x08) {
                        lv_led_on(alif::app::ScreenLayoutLEDObject());
                    } else {
                        lv_led_off(alif::app::ScreenLayoutLEDObject());
                    }
                }
                hal_wait_for_audio();
                hal_audio_alif_preprocessing(audio_inf, AUDIO_CHUNK_SIZE_SAMPLES);
                idle_count++;
            }

            {
                ScopedLVGLLock lv_lock;
                lv_label_set_text_static(alif::app::ScreenLayoutLabelObject(0), "");
                lv_obj_remove_flag(alif::app::ScreenLayoutBarObject(), LV_OBJ_FLAG_HIDDEN);
                lv_obj_invalidate(alif::app::ScreenLayoutBarObject());
                for (int ii = 0; ii <= result_label_idx; ii++) {
                    lv_label_set_text_static(alif::app::ScreenLayoutLabelObject(ii), "");
                }
            }

            // Copy the starting chunk
            const int start_samples = AUDIO_CHUNK_SIZE_SAMPLES - button_pressed_sample_count;
            int16_t *audio_inf_ptr = &audio_inf[start_samples];
            if (start_samples > 0) {
                memmove(audio_inf,
                        &audio_inf[button_pressed_sample_count - 1],
                        start_samples * sizeof(audio_inf[0]));
            }

            // Capture audio as long as button is kept pressed
            BOARD_LED2_Control(BOARD_LED_STATE_HIGH);
            int audio_idx = 0;

            hal_get_audio_data(audio_inf_ptr, AUDIO_CHUNK_SIZE_SAMPLES);

            while (1) {
                BOARD_BUTTON_STATE bs;
                BOARD_BUTTON2_GetState(&bs);
                int err = hal_wait_for_audio();
                if (err) {
                    printf_err("hal audio error: %d\n", err);
                    return false;
                }

                // Start next chunk
                if (audio_idx < (AUDIO_CHUNKS-1)) {
                    hal_get_audio_data(audio_inf_ptr + ((audio_idx + 1) * AUDIO_CHUNK_SIZE_SAMPLES),
                                       AUDIO_CHUNK_SIZE_SAMPLES);
                }

                // Update bar
                {
                    ScopedLVGLLock lv_lock;
                    lv_bar_set_value(alif::app::ScreenLayoutBarObject(), 100 * (audio_idx + 1) / AUDIO_CHUNKS, LV_ANIM_OFF);
                    lv_obj_invalidate(alif::app::ScreenLayoutBarObject());
                }

                // Preprocess current chunk
                hal_audio_alif_preprocessing(audio_inf_ptr + (audio_idx * AUDIO_CHUNK_SIZE_SAMPLES),
                                             AUDIO_CHUNK_SIZE_SAMPLES);

                if (bs != BOARD_BUTTON_STATE_LOW) {
                    break;
                }

                if (audio_idx >= (AUDIO_CHUNKS - 1)) {
                    break;
                }
                audio_idx++;
            }
            BOARD_LED2_Control(BOARD_LED_STATE_LOW);

            int16_t *audioArr = audio_inf;
            uint32_t audioArrSize = start_samples + (audio_idx + 1) * AUDIO_CHUNK_SIZE_SAMPLES;
#else

            /* Get the current audio buffer and respective size. */
            uint32_t audioArrSize = 0;
            hal_audio_start();
            auto audioArr = hal_audio_get_captured_frame(&audioArrSize);
            if (!audioArrSize || !audioArr) {
                debug("End of stream\n");
                break;
            }
#endif
            {
                ScopedLVGLLock lv_lock;
                lv_label_set_text_static(alif::app::ScreenLayoutLabelObject(result_label_idx), "");
                lv_obj_add_flag(alif::app::ScreenLayoutBarObject(), LV_OBJ_FLAG_HIDDEN);
                lv_led_on(alif::app::ScreenLayoutLEDObject());
                lv_bar_set_value(alif::app::ScreenLayoutBarObject(), 0,
                                 LV_ANIM_OFF);
                lv_obj_invalidate(alif::app::ScreenLayoutLabelObject(result_label_idx));
            }

            /* Run the pre-processing, inference and post-processing. */
#if defined(GPIO_PROFILING)
            BOARD_LED1_BLUE_Control(BOARD_LED_STATE_TOGGLE);
#else
            const uint32_t ts_start_pre = Get_SysTick_Cycle_Count32();
#endif
            if (!preProcess.DoPreProcess(audioArr, audioArrSize)) {
                printf_err("Pre-processing failed.");
                return false;
            }
#ifndef GPIO_PROFILING
            const uint32_t ts_done_pre = Get_SysTick_Cycle_Count32();
            drawMelSpec(*inputTensorMelSpec);
            const uint32_t ts_start_inference = Get_SysTick_Cycle_Count32();
#endif

            if (!RunInference(model, profiler)) {
                printf_err("Inference failed.");
                return false;
            }

#ifndef GPIO_PROFILING
            const uint32_t ts_start_post = Get_SysTick_Cycle_Count32();
#endif
            if (!postProcess.DoPostProcess()) {
                printf_err("Post-processing failed.");
                return false;
            }
#if defined(GPIO_PROFILING)
            BOARD_LED1_BLUE_Control(BOARD_LED_STATE_TOGGLE);
#else
            const uint32_t ts_done = Get_SysTick_Cycle_Count32();

            {
                ScopedLVGLLock lv_lock;
                lv_led_off(alif::app::ScreenLayoutLEDObject());
                lv_label_set_text_fmt(alif::app::ScreenLayoutLabelObject(0),
                                      "Input duration: %.1fs",
                                      (double)(audioArrSize) / AUDIO_RATE);

                lv_label_set_text_fmt(alif::app::ScreenLayoutLabelObject(1),
                                      "Inference time: %.2f ms",
                                      (double)(ts_start_post - ts_start_inference) /
                                          SystemCoreClock * 1000);

                lv_label_set_text_fmt(
                    alif::app::ScreenLayoutLabelObject(2),
                    "Pre: %.2fms Post: %.2fms",
                    (double)(ts_done_pre - ts_start_pre) / SystemCoreClock * 1000,
                    (double)(ts_done - ts_start_post) / SystemCoreClock * 1000);
            }
#endif
            {
                ScopedLVGLLock lv_lock;
                lv_label_set_text_static(alif::app::ScreenLayoutLabelObject(3), "Output:");
                lv_label_set_text(alif::app::ScreenLayoutLabelObject(result_label_idx), decodedResult.c_str());
                lv_obj_invalidate(alif::app::ScreenLayoutLabelObject(result_label_idx));
            }

            profiler.PrintProfilingResult();
            info("Decoded output: %s\n", decodedResult.c_str());

        }
        return true;
    }

} /* namespace app */
} /* namespace arm */

#endif /* defined(MLEK_FWK_EXECUTORCH) */
