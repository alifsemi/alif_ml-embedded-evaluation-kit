/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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

} /* namespace app */
} /* namespace alif */

#endif /* SCREEN_LAYOUT_HPP */
