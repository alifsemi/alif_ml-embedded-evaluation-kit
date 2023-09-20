/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdatomic.h>

#include "RTE_Components.h"
#include CMSIS_device_header
#include "RTE_Device.h"

#include "LCD_panel.h"
#include "Driver_CDC200.h"
#include "lvgl.h"
#include "lv_port.h"

#define MY_DISP_HOR_RES RTE_PANEL_HACTIVE_TIME
#define MY_DISP_VER_RES RTE_PANEL_VACTIVE_LINE
#define MY_DISP_BUFFER  (MY_DISP_VER_RES * 32)

#if LV_COLOR_DEPTH == 32
#if RTE_CDC200_PIXEL_FORMAT != 1
#error "LCD framebuffer must be set to RGB888 for 32-bit LVGL color depth"
#endif
static uint8_t lcd_image[MY_DISP_VER_RES][MY_DISP_HOR_RES][3] __attribute__((section(".bss.lcd_image_buf")));             // 480x800x3 = 1,152,000
#elif LV_COLOR_DEPTH == 16
#if RTE_CDC200_PIXEL_FORMAT != 2
#error "LCD framebuffer must be set to RGB565 for 16-bit LVGL color depth"
#endif
static uint8_t lcd_image[MY_DISP_VER_RES][MY_DISP_HOR_RES][2] __attribute__((section(".bss.lcd_image_buf")));             // 480x800x3 = 1,152,000
#else
#error "Unsupported LVGL color depth"
#endif

static atomic_bool lv_inited;
static uint32_t lv_last_timer_handler_trigger;
static atomic_uint_fast32_t lv_ticks;

static atomic_char pending_flush; // 0 = no pending flush, 1 = flush pending, 2 = flush in progress

static void lv_disp_flush(lv_disp_drv_t * restrict disp_drv, const lv_area_t * restrict area, lv_color_t * restrict color_p)
{
#if LV_COLOR_DEPTH == 32
// lv_rounder ensures x coordinates are multiples of 4
    for(int32_t y = area->y1; y <= area->y2; y++) {
        uint32_t *restrict dstp32 = (uint32_t *) lcd_image[y][area->x1];
        for (int32_t count = (area->x2 + 1 - area->x1) / 4; count; count--) {
            uint32_t argb0 = (*color_p++).full;
            uint32_t argb1 = (*color_p++).full;
            uint32_t b1r0g0b0 = (argb1 << 24) | (argb0 & 0x00ffffff);
            *dstp32++ = b1r0g0b0;
            uint32_t argb2 = (*color_p++).full;
            uint32_t g2b2r1g1 = (argb2 << 16) | ((argb1 >> 8) & 0x0000ffff);
            *dstp32++ = g2b2r1g1;
            uint32_t argb3 = (*color_p++).full;
            uint32_t r3g3b3r2 = (argb3 << 8) | ((argb2 >> 16) & 0x000000ff);
            *dstp32++ = r3g3b3r2;
        }
    }
#else
    lv_coord_t w = lv_area_get_width(area);
    lv_coord_t x = area->x1;
    for(int32_t y = area->y1; y <= area->y2; y++) {
        memcpy(lcd_image[y][x], color_p, w * sizeof *color_p);
        color_p += w;
    }
#endif

    pending_flush = 0;
    lv_disp_flush_ready(disp_drv);
}

static lv_disp_drv_t *pending_flush_disp_drv;
static lv_area_t pending_flush_area;
static lv_color_t *pending_flush_color_p;

void do_pending_flush(void)
{
    char expected = 1;
    if (atomic_compare_exchange_strong(&pending_flush, &expected, 2)) {
        lv_disp_flush(pending_flush_disp_drv, &pending_flush_area, pending_flush_color_p);
    }
}

static void lv_consider_immediate_flush(lv_disp_drv_t * restrict disp_drv)
{
    (void)(disp_drv);

    if (!pending_flush) {
        return;
    }

    int scan_y = LCD_current_v_pos();
    if (scan_y < pending_flush_area.y1 - 32 || scan_y > pending_flush_area.y2) {
        do_pending_flush();
    }
}

static void lv_disp_flush_async(lv_disp_drv_t * restrict disp_drv, const lv_area_t * restrict area, lv_color_t * restrict color_p)
{
    /* LVGL should not call us to flush while we have one already pending - trap just in case */
    if (pending_flush) {
        while (1) {
            __WFE();
        }
    }
    /* Prepare the flush info */
    pending_flush_disp_drv = disp_drv;
    pending_flush_area = *area;
    pending_flush_color_p = color_p;
    pending_flush = 1;

    /* And then do it immediately if we can, being race-free with the display interrupt which
     * could also do it.
     */
    lv_consider_immediate_flush(disp_drv);
}

#if LV_COLOR_DEPTH == 32
static void lv_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    (void)(disp_drv);
    area->x1 = area->x1 & ~(lv_coord_t) 3;
    area->x2 = area->x2 | 3;
}
#endif

/* lv_port_disp_init() may be called twice, as it's called both by our port of
 * ARM's GLCD UI (unless it's stubbed out), and also by native LVGL apps.
 *
 * On a repeat call, all existing objects are hidden, which has the effect of hiding
 * the unwanted GLCD canvas for a LVGL app. (To save memory, the app could be built
 * with GLCD stubs, but we want to handle the case where the build folder is configured
 * to support both GLCD and LVGL apps).
 */
void lv_port_disp_init(void)
{
    if (lv_inited)
    {
        uint32_t lv_lock_state = lv_port_lock();
        lv_obj_t *screen = lv_scr_act();
        uint32_t children = lv_obj_get_child_cnt(screen);
        for (uint32_t i = 0; i < children; i++) {
            lv_obj_add_flag(lv_obj_get_child(screen, i), LV_OBJ_FLAG_HIDDEN);
        }
        lv_theme_apply(screen);
        lv_port_unlock(lv_lock_state);
        return;
    }

    LCD_Panel_init(&lcd_image[0][0][0]);

    static lv_disp_drv_t disp_drv;
    static lv_disp_draw_buf_t disp_buf;
    /* This drawing buffer should be in DCTM for speed. */
    static lv_color_t buf_1[MY_DISP_BUFFER];

    lv_init();

    lv_disp_drv_init(&disp_drv);
    lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, MY_DISP_BUFFER);

    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = lv_disp_flush_async;
    disp_drv.wait_cb = lv_consider_immediate_flush;
#if LV_COLOR_DEPTH == 32
    disp_drv.rounder_cb = lv_rounder;
#endif
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;
    disp_drv.sw_rotate = true;
#if ROTATE_DISPLAY == 90
    disp_drv.rotated = LV_DISP_ROT_90;
#elif ROTATE_DISPLAY == 180
    disp_drv.rotated = LV_DISP_ROT_180;
#elif ROTATE_DISPLAY == 270
    disp_drv.rotated = LV_DISP_ROT_270;
#endif
    lv_disp_drv_register(&disp_drv);

    /* Set up priorities so that SysTick can interrupt LCD interrupts which
     * can interrupt the task handling. Most other hardware interrupts should
     * be more urgent than the LCD interrupt, as that will do significant
     * painting work.
     */
    NVIC_SetPriority (SysTick_IRQn, 0x80 >> (8-__NVIC_PRIO_BITS));

    LCD_enable_tear_interrupt(do_pending_flush, 0xC0 >> (8-__NVIC_PRIO_BITS));

    lv_last_timer_handler_trigger = -256;
    NVIC_SetPriority(PendSV_IRQn, 0xFF >> (8-__NVIC_PRIO_BITS));
    NVIC_SetPriorityGrouping(0);

    lv_inited = true;
}

static atomic_bool locked;

uint32_t lv_port_lock(void)
{
    /* Mask out the low-priority PendSV */
    uint32_t old_state = __get_BASEPRI() & 0xFF;
    const uint8_t desired_state = (uint8_t)(0xFF << (8-__NVIC_PRIO_BITS));
    if (old_state == 0 || old_state > desired_state) {
        __set_BASEPRI(desired_state);
    }
    locked = true;
    return old_state;
}

void lv_port_unlock(uint32_t state)
{
    locked = false;
    __set_BASEPRI(state);
}

uint32_t lv_port_get_ticks(void)
{
    return lv_ticks;
}

/* Override the weak function in timer_ensemble.c */
void lv_tick_handler(int ticks)
{
    if (lv_inited) {
#if defined __clang__ && __clang_major__ < 16
        // Clang had a problem with atomic += that hits Arm Compiler 6.18 and earlier
        // Bug described at https://github.com/llvm/llvm-project/issues/48742
        uint32_t new_ticks = atomic_fetch_add(&lv_ticks, ticks) + ticks;
#else
        uint32_t new_ticks = (lv_ticks += ticks);
#endif
        if (new_ticks - lv_last_timer_handler_trigger >= 5) {
            lv_last_timer_handler_trigger = new_ticks;
            SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
        }
    }
}

/* Run lv_timer_handler from a minimum-priority interrupt, triggered from SysTick */
void PendSV_Handler(void)
{
    if (locked)
    {
        while (1) {
            __WFE();
        }
    }
    lv_timer_handler();
}
