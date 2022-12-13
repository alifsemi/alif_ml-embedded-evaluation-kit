/* Copyright (c) 2022 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/**************************************************************************//**
 * @file     lv_port_disp.c
 * @author   Ahmad Rashed
 * @email    ahmad.rashed@alifsemi.com
 * @version  V1.0.0
 * @date     23-Feb-2022
 * @brief    for lvgl library display init
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "RTE_Components.h"
#include CMSIS_device_header

#include "lvgl.h"
#include "image_processing.h"


#define MY_DISP_HOR_RES (DIMAGE_X >> 1)
#define MY_DISP_VER_RES (DIMAGE_Y >> 1)
#define MY_DISP_BUFFER  (MY_DISP_VER_RES * 10)

extern uint8_t lcd_image[DIMAGE_Y][DIMAGE_X][RGB_BYTES];

__STATIC_INLINE void put_px(int32_t x, int32_t y, lv_color_t * color_p)
{
	x <<= 1;
	y <<= 1;

	memcpy(&lcd_image[y  ][x  ][0], color_p, RGB_BYTES);
	memcpy(&lcd_image[y  ][x+1][0], color_p, RGB_BYTES);
	memcpy(&lcd_image[y+1][x  ][0], color_p, RGB_BYTES);
	memcpy(&lcd_image[y+1][x+1][0], color_p, RGB_BYTES);
}

static void lv_disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            put_px(x, y, color_p);
            color_p++;
        }
    }

    lv_disp_flush_ready(disp_drv);
}

static void lv_clean_dcache_cb(lv_disp_drv_t * disp_drv) {
	/* Example for Cortex-M (CMSIS) */
	SCB_CleanInvalidateDCache();
	(void)(disp_drv);
}

lv_obj_t *labelResult1;
lv_obj_t *labelResult2;
lv_obj_t *labelResult3;
lv_obj_t *labelResult4;
lv_obj_t *labelResult5;
//lv_obj_t *labelTime;

void lv_port_disp_init(void) {
	static lv_disp_drv_t disp_drv;
	static lv_disp_draw_buf_t disp_buf;
	static lv_color_t buf_1[MY_DISP_BUFFER];

	lv_init();

	lv_disp_drv_init(&disp_drv);
	lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, MY_DISP_BUFFER);

	disp_drv.draw_buf = &disp_buf;
	disp_drv.clean_dcache_cb = lv_clean_dcache_cb;
	disp_drv.flush_cb = lv_disp_flush;
	disp_drv.hor_res = MY_DISP_HOR_RES;
	disp_drv.ver_res = MY_DISP_VER_RES;
	lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

	/*Create a Label on the currently active screen*/
	lv_obj_t *label1 =  lv_label_create(lv_scr_act());

	/*Modify the Label's text*/
	lv_label_set_text_static(label1, "Image Classifier Top 3 Results");

	/* Align the Label to the center
	 * NULL means align on parent (which is the screen now)
	 * 0, 0 at the end means an x, y offset after alignment*/
	lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label1, LV_ALIGN_CENTER, 0, 50);

	labelResult1 =  lv_label_create(lv_scr_act());
	labelResult2 =  lv_label_create(lv_scr_act());
	labelResult3 =  lv_label_create(lv_scr_act());
	labelResult4 =  lv_label_create(lv_scr_act());
	labelResult5 =  lv_label_create(lv_scr_act());
	//labelTime = lv_label_create(lv_scr_act());

	lv_obj_set_style_text_align(labelResult1, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_align(labelResult2, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_align(labelResult3, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_align(labelResult4, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_align(labelResult5, LV_TEXT_ALIGN_CENTER, 0);
	//lv_obj_set_style_text_align(labelTime, LV_TEXT_ALIGN_CENTER, 0);

	lv_obj_align(labelResult1, LV_ALIGN_CENTER, 0, 75);
	lv_obj_align(labelResult2, LV_ALIGN_CENTER, 0, 100);
	lv_obj_align(labelResult3, LV_ALIGN_CENTER, 0, 125);
	lv_obj_align(labelResult4, LV_ALIGN_CENTER, 0, 150);
	lv_obj_align(labelResult5, LV_ALIGN_CENTER, 0, 175);
	//lv_obj_align(labelTime, LV_ALIGN_CENTER, 0, 150);

	lv_label_set_text_static(labelResult1, "Result 1");
	lv_label_set_text_static(labelResult2, "Result 2");
	lv_label_set_text_static(labelResult3, "Result 3");
	lv_label_set_text_static(labelResult4, "");
	lv_label_set_text_static(labelResult5, "");
	//lv_label_set_text_static(labelTime, "Time: XX ms");

}
