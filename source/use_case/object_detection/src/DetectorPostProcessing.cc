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
#include "DetectorPostProcessing.hpp"
#include <algorithm>
#include <cmath>
#include <stdint.h>
#include <forward_list>


typedef struct boxabs {
    float left, right, top, bot;
} boxabs;


typedef struct branch {
    int resolution;
    int num_box;
    float *anchor;
    int8_t *tf_output;
    float scale;
    int zero_point;
    size_t size;
    float scale_x_y;
} branch;

typedef struct network {
    int input_w;
    int input_h;
    int num_classes;
    int num_branch;
    branch *branchs;
    int topN;
} network;


typedef struct box {
    float x, y, w, h;
} box;

typedef struct detection{
    box bbox;
    float *prob;
    float objectness;
} detection;



static int sort_class;

static void free_dets(std::forward_list<detection> &dets){
    std::forward_list<detection>::iterator it;
    for ( it = dets.begin(); it != dets.end(); ++it ){
        free(it->prob);
    }
}

float sigmoid(float x)
{
    return 1.f/(1.f + exp(-x));
} 

static bool det_objectness_comparator(detection &pa, detection &pb)
{
    return pa.objectness < pb.objectness;
}

static void insert_topN_det(std::forward_list<detection> &dets, detection det)
{
    std::forward_list<detection>::iterator it;
    std::forward_list<detection>::iterator last_it;
    for ( it = dets.begin(); it != dets.end(); ++it ){
        if(it->objectness > det.objectness)
            break;
        last_it = it;
    }
    if(it != dets.begin()){
        dets.emplace_after(last_it, det);
        free(dets.begin()->prob);
        dets.pop_front();
    }
    else{
        free(det.prob);
    }
}

static std::forward_list<detection> get_network_boxes(network *net, int image_w, int image_h, float thresh, int *num)
{
    std::forward_list<detection> dets;
    int i;
    int num_classes = net->num_classes;
    *num = 0;

    for (i = 0; i < net->num_branch; ++i) {
        int height  = net->branchs[i].resolution;
        int width = net->branchs[i].resolution;
        int channel  = net->branchs[i].num_box*(5+num_classes);

        for (int h = 0; h < net->branchs[i].resolution; h++) {
            for (int w = 0; w < net->branchs[i].resolution; w++) {
                for (int anc = 0; anc < net->branchs[i].num_box; anc++) {
                    
                    // objectness score
                    int bbox_obj_offset = h * width * channel + w * channel + anc * (num_classes + 5) + 4;
                    float objectness = sigmoid(((float)net->branchs[i].tf_output[bbox_obj_offset] - net->branchs[i].zero_point) * net->branchs[i].scale);

                    if(objectness > thresh){
                        detection det;
                        det.prob = (float*)calloc(num_classes, sizeof(float));
                        det.objectness = objectness;
                        //get bbox prediction data for each anchor, each feature point
                        int bbox_x_offset = bbox_obj_offset -4;
                        int bbox_y_offset = bbox_x_offset + 1;
                        int bbox_w_offset = bbox_x_offset + 2;
                        int bbox_h_offset = bbox_x_offset + 3;
                        int bbox_scores_offset = bbox_x_offset + 5;
                        //int bbox_scores_step = 1;
                        det.bbox.x = ((float)net->branchs[i].tf_output[bbox_x_offset] - net->branchs[i].zero_point) * net->branchs[i].scale;
                        det.bbox.y = ((float)net->branchs[i].tf_output[bbox_y_offset] - net->branchs[i].zero_point) * net->branchs[i].scale;
                        det.bbox.w = ((float)net->branchs[i].tf_output[bbox_w_offset] - net->branchs[i].zero_point) * net->branchs[i].scale;
                        det.bbox.h = ((float)net->branchs[i].tf_output[bbox_h_offset] - net->branchs[i].zero_point) * net->branchs[i].scale;
                        

                        float bbox_x, bbox_y;

                        // Eliminate grid sensitivity trick involved in YOLOv4
                        bbox_x = sigmoid(det.bbox.x); //* net->branchs[i].scale_x_y - (net->branchs[i].scale_x_y - 1) / 2;
                        bbox_y = sigmoid(det.bbox.y); //* net->branchs[i].scale_x_y - (net->branchs[i].scale_x_y - 1) / 2;
                        det.bbox.x = (bbox_x + w) / width;
                        det.bbox.y = (bbox_y + h) / height;

                        det.bbox.w = exp(det.bbox.w) * net->branchs[i].anchor[anc*2] / net->input_w;
                        det.bbox.h = exp(det.bbox.h) * net->branchs[i].anchor[anc*2+1] / net->input_h;
                        
                        for (int s = 0; s < num_classes; s++) {
                            det.prob[s] = sigmoid(((float)net->branchs[i].tf_output[bbox_scores_offset + s] - net->branchs[i].zero_point) * net->branchs[i].scale)*objectness;
                            det.prob[s] = (det.prob[s] > thresh) ? det.prob[s] : 0;
                        }

                        //correct_yolo_boxes 
                        det.bbox.x *= image_w;
                        det.bbox.w *= image_w;
                        det.bbox.y *= image_h;
                        det.bbox.h *= image_h;

                        if (*num < net->topN || net->topN <=0){
                            dets.emplace_front(det);
                            *num += 1;
                        }
                        else if(*num ==  net->topN){
                            dets.sort(det_objectness_comparator);
                            insert_topN_det(dets,det);
                            *num += 1;
                        }else{
                            insert_topN_det(dets,det);
                        }
                    }
                }
            }
        }
    }
    if(*num > net->topN)
        *num -=1;
    return dets;
}

// init part

static branch create_brach(int resolution, int num_box, float *anchor, int8_t *tf_output, size_t size, float scale, int zero_point)
{
    branch b;
    b.resolution = resolution;
    b.num_box = num_box;
    b.anchor = anchor;
    b.tf_output = tf_output;
    b.size = size;
    b.scale = scale;
    b.zero_point = zero_point;
    return b;
}

static network creat_network(int input_w, int input_h, int num_classes, int num_branch, branch* branchs, int topN)
{
    network net;
    net.input_w = input_w;
    net.input_h = input_h;
    net.num_classes = num_classes;
    net.num_branch = num_branch;
    net.branchs = branchs;
    net.topN = topN;
    return net;
}

// NMS part

static float Calc1DOverlap(float x1_center, float width1, float x2_center, float width2)
{
    float left_1 = x1_center - width1/2;
    float left_2 = x2_center - width2/2;
    float leftest;
    if (left_1 > left_2) {
        leftest = left_1;
    } else {
        leftest = left_2;    
    }
        
    float right_1 = x1_center + width1/2;
    float right_2 = x2_center + width2/2;
    float rightest;
    if (right_1 < right_2) {
        rightest = right_1;
    } else {
        rightest = right_2;    
    }
        
    return rightest - leftest;
}


static float CalcBoxIntersect(box box1, box box2)
{
    float width = Calc1DOverlap(box1.x, box1.w, box2.x, box2.w);
    if (width < 0) return 0;
    float height = Calc1DOverlap(box1.y, box1.h, box2.y, box2.h);
    if (height < 0) return 0;
    
    float total_area = width*height;
    return total_area;
}


static float CalcBoxUnion(box box1, box box2)
{
    float boxes_intersection = CalcBoxIntersect(box1, box2);
    float boxes_union = box1.w*box1.h + box2.w*box2.h - boxes_intersection;
    return boxes_union;
}


static float CalcBoxIOU(box box1, box box2)
{
    float boxes_intersection = CalcBoxIntersect(box1, box2); 
    
    if (boxes_intersection == 0) return 0;    
    
    float boxes_union = CalcBoxUnion(box1, box2);

    if (boxes_union == 0) return 0;    
    
    return boxes_intersection / boxes_union;
}


static bool CompareProbs(detection &prob1, detection &prob2)
{
    return prob1.prob[sort_class] > prob2.prob[sort_class];
}


static void CalcNMS(std::forward_list<detection> &detections, int classes, float iou_threshold)
{
    int k;
    
    for (k = 0; k < classes; ++k) {
        sort_class = k;
        detections.sort(CompareProbs);
        
        for (std::forward_list<detection>::iterator it=detections.begin(); it != detections.end(); ++it){
            if (it->prob[k] == 0) continue;
            for (std::forward_list<detection>::iterator itc=std::next(it, 1); itc != detections.end(); ++itc){
                if (itc->prob[k] == 0) continue;
                if (CalcBoxIOU(it->bbox, itc->bbox) > iou_threshold) {
                    itc->prob[k] = 0;
                }
            }
        }
    }
}


static void inline check_and_fix_offset(int im_w,int im_h,int *offset) 
{
    
    if (!offset) return;    
    
    if ( (*offset) >= im_w*im_h*FORMAT_MULTIPLY_FACTOR)
        (*offset) = im_w*im_h*FORMAT_MULTIPLY_FACTOR -1;
    else if ( (*offset) < 0)
            *offset =0;    
    
}


static void DrawBoxOnImage(uint8_t *img_in,int im_w,int im_h,int bx,int by,int bw,int bh) 
{
    
    if (!img_in) {
        return;
    }
    
    int offset=0;
    for (int i=0; i < bw; i++) {        
        /*draw two lines */
        for (int line=0; line < 2; line++) {
            /*top*/
            offset =(i + (by + line)*im_w + bx)*FORMAT_MULTIPLY_FACTOR;
            check_and_fix_offset(im_w,im_h,&offset);
            img_in[offset] = 0xFF;  /* FORMAT_MULTIPLY_FACTOR for rgb or grayscale*/
            /*bottom*/
            offset = (i + (by + bh - line)*im_w + bx)*FORMAT_MULTIPLY_FACTOR;
            check_and_fix_offset(im_w,im_h,&offset);
            img_in[offset] = 0xFF;    
        }                
    }
    
    for (int i=0; i < bh; i++) {
        /*draw two lines */
        for (int line=0; line < 2; line++) {
            /*left*/
            offset = ((i + by)*im_w + bx + line)*FORMAT_MULTIPLY_FACTOR;
            check_and_fix_offset(im_w,im_h,&offset);            
            img_in[offset] = 0xFF;
            /*right*/
            offset = ((i + by)*im_w + bx + bw - line)*FORMAT_MULTIPLY_FACTOR;
            check_and_fix_offset(im_w,im_h,&offset);            
            img_in[offset] = 0xFF;    
        }
    }

}


void arm::app::RunPostProcessing(uint8_t *img_in,TfLiteTensor* model_output[2],std::vector<arm::app::DetectionResult> & results_out)
{
       
    TfLiteTensor* output[2] = {nullptr,nullptr};
    int input_w = INPUT_IMAGE_WIDTH;
    int input_h = INPUT_IMAGE_HEIGHT;
  
    for(int anchor=0;anchor<2;anchor++)
    {
         output[anchor] = model_output[anchor];
    }

    /* init postprocessing 	 */
    int num_classes = 1;
    int num_branch = 2;
    int topN = 0;

    branch* branchs = (branch*)calloc(num_branch, sizeof(branch));

    /*NOTE: anchors are different for any given input model size, estimated during training phase */
    float anchor1[] = {38, 77, 47, 97, 61, 126};
    float anchor2[] = {14, 26, 19, 37, 28, 55 };


    branchs[0] = create_brach(INPUT_IMAGE_WIDTH/32, 3, anchor1, output[0]->data.int8, output[0]->bytes, ((TfLiteAffineQuantization*)(output[0]->quantization.params))->scale->data[0], ((TfLiteAffineQuantization*)(output[0]->quantization.params))->zero_point->data[0]);

    branchs[1] = create_brach(INPUT_IMAGE_WIDTH/16, 3, anchor2, output[1]->data.int8, output[1]->bytes, ((TfLiteAffineQuantization*)(output[1]->quantization.params))->scale->data[0],((TfLiteAffineQuantization*)(output[1]->quantization.params))->zero_point->data[0]);

    network net = creat_network(input_w, input_h, num_classes, num_branch, branchs,topN);
    /* end init */

    /* start postprocessing */
    int nboxes=0;
    float thresh = .5;//50%
    float nms = .45;
    int orig_image_width = ORIGINAL_IMAGE_WIDTH;
    int orig_image_height = ORIGINAL_IMAGE_HEIGHT;
    std::forward_list<detection> dets = get_network_boxes(&net, orig_image_width, orig_image_height, thresh, &nboxes);
    /* do nms */
    CalcNMS(dets, net.num_classes, nms);
    uint8_t temp_unsuppressed_counter = 0;
    int j;
    for (std::forward_list<detection>::iterator it=dets.begin(); it != dets.end(); ++it){
        float xmin = it->bbox.x - it->bbox.w / 2.0f;
        float xmax = it->bbox.x + it->bbox.w / 2.0f;
        float ymin = it->bbox.y - it->bbox.h / 2.0f;
        float ymax = it->bbox.y + it->bbox.h / 2.0f;

        if (xmin < 0) xmin = 0;
        if (ymin < 0) ymin = 0;
        if (xmax > orig_image_width) xmax = orig_image_width;
        if (ymax > orig_image_height) ymax = orig_image_height;

        float bx = xmin;
        float by = ymin;
        float bw = xmax - xmin;
        float bh = ymax - ymin;

        for (j = 0; j <  net.num_classes; ++j) {
            if (it->prob[j] > 0) {

                arm::app::DetectionResult tmp_result = {};
                
                tmp_result.m_normalisedVal = it->prob[j];
                tmp_result.m_x0=bx;
                tmp_result.m_y0=by;
                tmp_result.m_w=bw;
                tmp_result.m_h=bh;
                
                results_out.push_back(tmp_result);

                DrawBoxOnImage(img_in,orig_image_width,orig_image_height,bx,by,bw,bh);
                
                temp_unsuppressed_counter++;
            }
        }
    }

    free_dets(dets);
    free(branchs);

}

void arm::app::RgbToGrayscale(const uint8_t *rgb,uint8_t *gray, int im_w,int im_h) 
{
    float R=0.299;
    float G=0.587; 
    float B=0.114; 
    for (int i=0; i< im_w*im_h; i++ ) {

        uint32_t  int_gray = rgb[i*3 + 0]*R + rgb[i*3 + 1]*G+ rgb[i*3 + 2]*B;
        /*clip if need */
        if (int_gray <= UINT8_MAX) {
            gray[i] =  int_gray;
        } else {
            gray[i] = UINT8_MAX;
        }

    }

}

