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
#include "UseCaseCommonUtils.hpp"
#include "hal.h"
#include "log_macros.h"
#include "ImgClassProcessing.hpp"

#include <cinttypes>

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_paint_utils.h"

// Do we get LVGL to zoom the camera image, or do we double it up?
#define USE_LVGL_ZOOM

using ImgClassClassifier = arm::app::Classifier;

#define MIMAGE_X 224
#define MIMAGE_Y 224

#ifdef USE_LVGL_ZOOM
#define LIMAGE_X        MIMAGE_X
#define LIMAGE_Y        MIMAGE_Y
#else
#define LIMAGE_X        (MIMAGE_X * 2)
#define LIMAGE_Y        (MIMAGE_Y * 2)
#endif


extern bool run_requested(void);
extern "C" {
uint32_t tprof1, tprof2, tprof3, tprof4, tprof5;
LV_IMG_DECLARE(Alif240);
}

namespace {
lv_obj_t *labelResult[5];
//lv_obj_t *labelTime;
lv_obj_t *imageObj;
lv_obj_t *alifObj;
lv_img_dsc_t imageDesc;

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

static lv_style_t confident;
static lv_style_t weak;

#define DISP_SCALE 2
bool ClassifyImageInit()
{
    lv_port_disp_init();

    /* Initialise the camera */
    int err = hal_image_init();
    if (0 != err) {
        printf_err("hal_image_init failed with error: %d\n", err);
    }

    lv_port_lock();
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style, &lv_font_montserrat_28);
    lv_obj_add_style(lv_scr_act(), &style, 0);

    lv_style_init(&confident);
    lv_style_set_text_color(&confident, lv_color_hex(0x0080ff));
    lv_style_set_text_font(&confident, &lv_font_montserrat_32);

    lv_style_init(&weak);
    lv_style_set_text_color(&weak, lv_color_hex(0xcccccc));
    lv_style_set_text_font(&weak, &lv_font_montserrat_24);

    /*Create a Label on the currently active screen*/
    lv_obj_t *label1 =  lv_label_create(lv_scr_act());

    /*Modify the Label's text*/
    lv_label_set_text_static(label1, "Image Classifier Top 3 Results");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 50 * DISP_SCALE);

    int y = 75 * DISP_SCALE;
    for (auto &lbl : labelResult) {
        lbl = lv_label_create(lv_scr_act());
        lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(lbl, LV_ALIGN_CENTER, 0, y);
        lv_label_set_text(lbl, "");
        y += 25 * DISP_SCALE;
        // Text styles can be inherited, and that works for the
        // default style, but these state styles have to be put
        // directly onto the labels so that they check the
        // label's own state. If inherited, they check the parent's.
        lv_obj_add_style(lbl, &confident, LV_STATE_USER_1);
        lv_obj_add_style(lbl, &weak, LV_STATE_USER_2);
    }

    //labelTime = lv_label_create(lv_scr_act());
    //lv_obj_set_style_text_align(labelTime, LV_TEXT_ALIGN_CENTER, 0);
    //lv_obj_align(labelTime, LV_ALIGN_CENTER, 0, 150 * DISP_SCALE);
    //lv_label_set_text_static(labelTime, "Time: XX ms");

    imageObj = lv_img_create(lv_scr_act());
    imageDesc.data = (const uint8_t *)lvgl_image;
    imageDesc.data_size = sizeof lvgl_image;
    imageDesc.header.always_zero = 0;
    imageDesc.header.reserved = 0;
    imageDesc.header.cf = LV_IMG_CF_TRUE_COLOR;
    imageDesc.header.w = LIMAGE_X;
    imageDesc.header.h = LIMAGE_Y;
    lv_obj_align(imageObj, LV_ALIGN_TOP_MID, 0, 8 * DISP_SCALE);
#ifdef USE_LVGL_ZOOM
    lv_img_set_size_mode(imageObj, LV_IMG_SIZE_MODE_REAL);
    lv_img_set_zoom(imageObj, 2*256);
    lv_img_set_antialias(imageObj, false);
#endif
    lv_img_set_src(imageObj, &imageDesc);
    lv_obj_move_background(imageObj);

    alifObj = lv_img_create(lv_scr_act());
    lv_img_set_src(alifObj, &Alif240);
    lv_obj_align(alifObj, LV_ALIGN_BOTTOM_MID, 0, -2 * DISP_SCALE);
    lv_obj_move_background(alifObj);

#if 0
    lv_obj_t new_scr = lv_obj_create(NULL);

    l
    lv_scr_load_anim(new_scr,  LV_SCR_LOAD_ANIM_FADE_IN, 100, 0, false);
    lv_scr_load_anim(new_scr,  LV_SCR_LOAD_ANIM_FADE_IN, 100, 0, false);
#endif
    lv_port_unlock();
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

        /* Set up pre and post-processing. */
        ImgClassPreProcess preProcess = ImgClassPreProcess(inputTensor, model.IsDataSigned());

        std::vector<ClassificationResult> results;
        ImgClassPostProcess postProcess = ImgClassPostProcess(outputTensor,
                ctx.Get<ImgClassClassifier&>("classifier"), ctx.Get<std::vector<std::string>&>("labels"),
                results);
#endif

        const uint8_t *image_data = hal_get_image_data();
        if (!image_data) {
            printf_err("hal_get_image_data failed");
            return false;
        }

        lv_port_lock();
        tprof5 = ARM_PMU_Get_CCNTR();
        /* Display this image on the LCD. */
#ifdef USE_LVGL_ZOOM
        write_to_lvgl_buf(
#else
        write_to_lvgl_buf_doubled(
#endif
                (const uint8_t (*)[MIMAGE_X][3]) image_data, lvgl_image);
        tprof5 = ARM_PMU_Get_CCNTR() - tprof5;

        lv_obj_invalidate(imageObj);
        lv_port_unlock();

        if (SKIP_MODEL || !run_requested()) {
#if SHOW_PROFILING
            lv_port_lock();
            lv_label_set_text_fmt(labelResult[0], "tprof1=%.3f ms", (double)tprof1 / SystemCoreClock * 1000);
            lv_label_set_text_fmt(labelResult[1], "tprof2=%.3f ms", (double)tprof2 / SystemCoreClock * 1000);
            lv_label_set_text_fmt(labelResult[2], "tprof3=%.3f ms", (double)tprof3 / SystemCoreClock * 1000);
            lv_label_set_text_fmt(labelResult[3], "tprof4=%.3f ms", (double)tprof4 / SystemCoreClock * 1000);
            lv_label_set_text_fmt(labelResult[4], "tprof5=%.3f ms", (double)tprof5 / SystemCoreClock * 1000);
            lv_port_unlock();
#endif
            return true;
        }

#if !SKIP_MODEL
        const size_t imgSz = inputTensor->bytes < IMAGE_DATA_SIZE ?
                              inputTensor->bytes : IMAGE_DATA_SIZE;

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

        /* Add results to context for access outside handler. */
        ctx.Set<std::vector<ClassificationResult>>("results", results);

        lv_port_lock();
        for (int r = 0; r < 3; r++) {
            lv_label_set_text_fmt(labelResult[r], "%s (%d%%)", first_bit(results[r].m_label).c_str(), (int)(results[r].m_normalisedVal * 100));
            if (results[r].m_normalisedVal >= 0.75) {
                lv_obj_add_state(labelResult[r], LV_STATE_USER_1);
            } else {
                lv_obj_clear_state(labelResult[r], LV_STATE_USER_1);
            }
            if (results[r].m_normalisedVal < 0.2) {
                lv_obj_add_state(labelResult[r], LV_STATE_USER_2);
            } else {
                lv_obj_clear_state(labelResult[r], LV_STATE_USER_2);
            }
        }
        lv_port_unlock();

        if (!PresentInferenceResult(results)) {
            return false;
        }

        profiler.PrintProfilingResult();
#endif

        return true;
    }

} /* namespace app */
} /* namespace alif */
