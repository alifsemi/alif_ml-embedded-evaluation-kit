/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
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

#include "Classifier.hpp"
#include "InputFiles.hpp"
#include "MobileNetModel.hpp"
#include "ImageUtils.hpp"
#include "ScreenLayout.hpp"
#include "UseCaseCommonUtils.hpp"
#include "hal.h"
#include "log_macros.h"
#include "ImgClassProcessing.hpp"


#include <cinttypes>

#include "lvgl.h"
#include "lv_port.h"
#include "lv_paint_utils.h"

// Do we get LVGL to zoom the camera image, or do we double it up?
#define USE_LVGL_ZOOM

using ImgClassClassifier = arm::app::Classifier;

#define MIMAGE_X 224
#define MIMAGE_Y 224

#ifdef USE_LVGL_ZOOM
#define LIMAGE_X        MIMAGE_X
#define LIMAGE_Y        MIMAGE_Y
#define LV_ZOOM         (2 * 256)
#else
#define LIMAGE_X        (MIMAGE_X * 2)
#define LIMAGE_Y        (MIMAGE_Y * 2)
#define LV_ZOOM         (1 * 256)
#endif


extern "C" {
extern uint32_t tprof1, tprof2, tprof3, tprof4, tprof5;
}

namespace {

lv_color_t  lvgl_image[LIMAGE_Y][LIMAGE_X] __attribute__((section(".bss.lcd_image_buf")));                      // 448x448x4 = 802,856
};

namespace alif {
namespace app {

    using namespace arm::app;

    static std::string first_bit(const std::string &s)
    {
        std::string::size_type comma = s.find_first_of(',');
        return s.substr(0, comma);
    }

    bool ClassifyImageInit()
    {
        ScreenLayoutInit(lvgl_image, sizeof lvgl_image, LIMAGE_X, LIMAGE_Y, LV_ZOOM);
        uint32_t lv_lock_state = lv_port_lock();
        lv_label_set_text_static(ScreenLayoutHeaderObject(), "Image Classifier");
        lv_port_unlock(lv_lock_state);

        /* Initialise the camera */
        int err = hal_image_init();
        if (0 != err) {
            printf_err("hal_image_init failed with error: %d\n", err);
        }

        return true;
    }

    /* Image classification inference handler. */
    bool ClassifyImageHandler(ApplicationContext& ctx)
    {
#if !SKIP_MODEL
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model = ctx.Get<Model&>("model");

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        TfLiteTensor* outputTensor = model.GetOutputTensor(0);
        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 4) {
            printf_err("Input tensor dimension should be = 4\n");
            return false;
        }

        /* Get input shape for displaying the image. */
        TfLiteIntArray* inputShape = model.GetInputShape(0);
        const uint32_t nCols       = inputShape->data[arm::app::MobileNetModel::ms_inputColsIdx];
        const uint32_t nRows       = inputShape->data[arm::app::MobileNetModel::ms_inputRowsIdx];

        /* Set up pre and post-processing. */
        ImgClassPreProcess preProcess = ImgClassPreProcess(inputTensor, model.IsDataSigned());

        std::vector<ClassificationResult> results;
        ImgClassPostProcess postProcess = ImgClassPostProcess(outputTensor,
                ctx.Get<ImgClassClassifier&>("classifier"), ctx.Get<std::vector<std::string>&>("labels"),
                results);
#else
        const uint32_t nCols       = MIMAGE_X;
        const uint32_t nRows       = MIMAGE_Y;
#endif

        const uint8_t *image_data = hal_get_image_data(nCols, nRows);
        if (!image_data) {
            printf_err("hal_get_image_data failed");
            return false;
        }

        uint32_t lv_lock_state = lv_port_lock();
        tprof5 = Get_SysTick_Cycle_Count32();
        /* Display this image on the LCD. */
#ifdef USE_LVGL_ZOOM
        write_to_lvgl_buf(
#else
        write_to_lvgl_buf_doubled(
#endif
                MIMAGE_X, MIMAGE_Y, image_data, &lvgl_image[0][0]);
        tprof5 = Get_SysTick_Cycle_Count32() - tprof5;

        lv_obj_invalidate(ScreenLayoutImageObject());

        if (SKIP_MODEL || !run_requested()) {
#if SHOW_PROFILING
            lv_label_set_text_fmt(ScreenLayoutLabelObject(0), "tprof1=%.3f ms", (double)tprof1 / SystemCoreClock * 1000);
            lv_label_set_text_fmt(ScreenLayoutLabelObject(1), "tprof2=%.3f ms", (double)tprof2 / SystemCoreClock * 1000);
            lv_label_set_text_fmt(ScreenLayoutLabelObject(2), "tprof3=%.3f ms", (double)tprof3 / SystemCoreClock * 1000);
            lv_label_set_text_fmt(ScreenLayoutLabelObject(3), "tprof4=%.3f ms", (double)tprof4 / SystemCoreClock * 1000);
            lv_label_set_text_fmt(ScreenLayoutLabelObject(4), "tprof5=%.3f ms", (double)tprof5 / SystemCoreClock * 1000);
#endif
#if SHOW_EXPOSURE
            lv_label_set_text_fmt(ScreenLayoutLabelObject(1), "low=%" PRIu32, exposure_low_count);
            lv_label_set_text_fmt(ScreenLayoutLabelObject(2), "high=%" PRIu32, exposure_high_count);
            lv_label_set_text_fmt(ScreenLayoutLabelObject(3), "gain=%.3f", get_image_gain());
#endif
            lv_led_off(ScreenLayoutLEDObject());
            lv_port_unlock(lv_lock_state);
            return true;
        }

        lv_led_on(ScreenLayoutLEDObject());
        lv_port_unlock(lv_lock_state);

#if !SKIP_MODEL
        const size_t imgSz = inputTensor->bytes;

#if SHOW_INF_TIME
        uint32_t inf_prof = Get_SysTick_Cycle_Count32();
#endif

        /* Run the pre-processing, inference and post-processing. */
        if (!preProcess.DoPreProcess(image_data, imgSz)) {
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

#if SHOW_INF_TIME
        inf_prof = Get_SysTick_Cycle_Count32() - inf_prof;
#endif

        /* Add results to context for access outside handler. */
        ctx.Set<std::vector<ClassificationResult>>("results", results);

        lv_lock_state = lv_port_lock();
        for (int r = 0; r < 3; r++) {
            lv_obj_t *label = ScreenLayoutLabelObject(r);
            lv_label_set_text_fmt(label, "%s (%d%%)", first_bit(results[r].m_label).c_str(), (int)(results[r].m_normalisedVal * 100));
            if (results[r].m_normalisedVal >= 0.7) {
                lv_obj_add_state(label, LV_STATE_USER_1);
            } else {
                lv_obj_clear_state(label, LV_STATE_USER_1);
            }
            if (results[r].m_normalisedVal < 0.2) {
                lv_obj_add_state(label, LV_STATE_USER_2);
            } else {
                lv_obj_clear_state(label, LV_STATE_USER_2);
            }
        }

#if SHOW_INF_TIME
        lv_label_set_text_fmt(ScreenLayoutHeaderObject(), "%s - %.2f FPS", "Image Classifier", (double) SystemCoreClock / inf_prof);
        lv_label_set_text_fmt(ScreenLayoutTimeObject(), "%.3f ms", (double)inf_prof / SystemCoreClock * 1000);
#endif
        lv_port_unlock(lv_lock_state);

        if (!PresentInferenceResult(results)) {
            return false;
        }

        profiler.PrintProfilingResult();
#endif

        return true;
    }

} /* namespace app */
} /* namespace alif */
