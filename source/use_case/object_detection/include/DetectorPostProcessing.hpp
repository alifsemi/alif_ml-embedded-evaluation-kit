/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#ifndef DETECTOR_POST_PROCESSING_HPP
#define DETECTOR_POST_PROCESSING_HPP

#include "UseCaseCommonUtils.hpp"
#include "DetectionResult.hpp"

namespace arm {
namespace app {

#if DISPLAY_RGB_IMAGE
#define FORMAT_MULTIPLY_FACTOR 3
#else
#define FORMAT_MULTIPLY_FACTOR 1
#endif /* DISPLAY_RGB_IMAGE */

    /**
     * @brief       Post processing part of Yolo object detection CNN
     * @param[in]   img_in        Pointer to the input image,detection bounding boxes drown on it.
     * @param[in]   model_output  Output tesnsors after CNN invoked
     * @param[out]  results_out   Vector of detected results.
     * @return      void
     **/
void RunPostProcessing(uint8_t *img_in,TfLiteTensor* model_output[2],std::vector<arm::app::DetectionResult> & results_out);


    /**
     * @brief       Converts RGB image to grayscale
     * @param[in]   rgb    Pointer to RGB input image
     * @param[out]  gray   Pointer to RGB out image
     * @param[in]   im_w   Input image width
     * @param[in]   im_h   Input image height
     * @return      void
     **/
void RgbToGrayscale(const uint8_t *rgb,uint8_t *gray, int im_w,int im_h);

} /* namespace app */
} /* namespace arm */

#endif /* DETECTOR_POST_PROCESSING_HPP */
