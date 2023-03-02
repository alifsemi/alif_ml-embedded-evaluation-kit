/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef SCREEN_LAYOUT_HPP
#define SCREEN_LAYOUT_HPP

#include "lvgl.h"

namespace alif {
namespace app {

void ScreenLayoutInit(const void *imgData, size_t imgSize, int imgWidth, int imgHeight, unsigned short imgZoom);

lv_obj_t *ScreenLayoutImageObject();
lv_obj_t *ScreenLayoutImageHolderObject();
lv_obj_t *ScreenLayoutHeaderObject();
lv_obj_t *ScreenLayoutLabelObject(int);
lv_obj_t *ScreenLayoutTimeObject();
lv_obj_t *ScreenLayoutLEDObject();

} /* namespace app */
} /* namespace alif */

#endif /* SCREEN_LAYOUT_HPP */
