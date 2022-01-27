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
#include "YoloFastestModel.hpp"

#include <forward_list>

namespace arm {
namespace app {
namespace object_detection {

    struct Branch {
        int resolution;
        int numBox;
        const float* anchor;
        int8_t* modelOutput;
        float scale;
        int zeroPoint;
        size_t size;
    };

    struct Network {
        int inputWidth;
        int inputHeight;
        int numClasses;
        std::vector<Branch> branches;
        int topN;
    };


    struct Box {
        float x;
        float y;
        float w;
        float h;
    };

    struct Detection {
        Box bbox;
        std::vector<float> prob;
        float objectness;
    };

    /**
     * @brief   Helper class to manage tensor post-processing for "object_detection"
     *          output.
     */
    class DetectorPostprocessing {
    public:
        /**
         * @brief       Constructor.
         * @param[in]   threshold     Post-processing threshold.
         * @param[in]   nms           Non-maximum Suppression threshold.
         * @param[in]   numClasses    Number of classes.
         * @param[in]   topN          Top N for each class.
         **/
        DetectorPostprocessing(float threshold = 0.5f,
                                float nms = 0.45f,
                                int numClasses = 1,
                                int topN = 0);

        /**
         * @brief       Post processing part of Yolo object detection CNN.
         * @param[in]   imgIn        Pointer to the input image,detection bounding boxes drown on it.
         * @param[in]   imgRows      Number of rows in the input image.
         * @param[in]   imgCols      Number of columns in the input image.
         * @param[in]   modelOutput  Output tensors after CNN invoked.
         * @param[out]  resultsOut   Vector of detected results.
         **/
        void RunPostProcessing(uint8_t* imgIn,
                               uint32_t imgRows,
                               uint32_t imgCols,
                               TfLiteTensor* modelOutput0,
                               TfLiteTensor* modelOutput1,
                               std::vector<DetectionResult>& resultsOut);

    private:
        float m_threshold;  /* Post-processing threshold */
        float m_nms;        /* NMS threshold */
        int   m_numClasses; /* Number of classes */
        int   m_topN;       /* TopN */

        /**
         * @brief       Calculate the Sigmoid function of the give value.
         * @param[in]   x   Value.
         * @return      Sigmoid value of the input.
         **/
        float Sigmoid(float x);

        /**
         * @brief       Insert the given Detection in the list.
         * @param[in]   detections   List of detections.
         * @param[in]   det          Detection to be inserted.
         **/
        void InsertTopNDetections(std::forward_list<Detection>& detections, Detection& det);

        /**
         * @brief        Given a Network calculate the detection boxes.
         * @param[in]    net           Network.
         * @param[in]    imageWidth    Original image width.
         * @param[in]    imageHeight   Original image height.
         * @param[in]    threshold     Detections threshold.
         * @param[out]   detections    Detection boxes.
         **/
        void GetNetworkBoxes(Network& net,
                             int imageWidth,
                             int imageHeight,
                             float threshold,
                             std::forward_list<Detection>& detections);

        /**
         * @brief       Calculate the 1D overlap.
         * @param[in]   x1Center   First center point.
         * @param[in]   width1     First width.
         * @param[in]   x2Center   Second center point.
         * @param[in]   width2     Second width.
         * @return      The overlap between the two lines.
         **/
        float Calculate1DOverlap(float x1Center, float width1, float x2Center, float width2);

        /**
         * @brief       Calculate the intersection between the two given boxes.
         * @param[in]   box1   First box.
         * @param[in]   box2   Second box.
         * @return      The intersection value.
         **/
        float CalculateBoxIntersect(Box& box1, Box& box2);

        /**
         * @brief       Calculate the union between the two given boxes.
         * @param[in]   box1   First box.
         * @param[in]   box2   Second box.
         * @return      The two given boxes union value.
         **/
        float CalculateBoxUnion(Box& box1, Box& box2);
        /**
         * @brief       Calculate the intersection over union between the two given boxes.
         * @param[in]   box1   First box.
         * @param[in]   box2   Second box.
         * @return      The intersection over union value.
         **/
        float CalculateBoxIOU(Box& box1, Box& box2);

        /**
         * @brief       Calculate the Non-Maxima suppression on the given detection boxes.
         * @param[in]   detections    Detection boxes.
         * @param[in]   classes       Number of classes.
         * @param[in]   iouThreshold  Intersection over union threshold.
         * @return      true or false based on execution success.
         **/
        void  CalculateNMS(std::forward_list<Detection>& detections, int classes, float iouThreshold);

        /**
         * @brief       Draw on the given image a bounding box starting at (boxX, boxY).
         * @param[in/out]   imgIn    Image.
         * @param[in]       imWidth    Image width.
         * @param[in]       imHeight   Image height.
         * @param[in]       boxX       Axis X starting point.
         * @param[in]       boxY       Axis Y starting point.
         * @param[in]       boxWidth   Box width.
         * @param[in]       boxHeight  Box height.
         **/
        void DrawBoxOnImage(uint8_t* imgIn,
                            int imWidth,
                            int imHeight,
                            int boxX,
                            int boxY,
                            int boxWidth,
                            int boxHeight);
    };

} /* namespace object_detection */
} /* namespace app */
} /* namespace arm */

#endif /* DETECTOR_POST_PROCESSING_HPP */
