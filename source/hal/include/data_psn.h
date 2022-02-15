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
#ifndef DATA_PSN_H
#define DATA_PSN_H

/**
 * This file is the top level abstraction for the data presentation module
 **/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Structure to encompass the data presentation module and it's methods */
typedef struct data_presentation_module {
    int inited;                 /**< initialised or not */
    char system_name[8];        /**< name of the system in use */
    int (* system_init)(void);  /**< pointer to init function */

    /** Pointer to the image presentation function */
    int (* present_data_image)(const uint8_t *data, const uint32_t width,
        const uint32_t height, const uint32_t channels,
        const uint32_t pos_x, const uint32_t pos_y,
        const uint32_t downsample_factor);

    /* Pointer to text presentation function */
    int (* present_data_text)(const char *str, const size_t str_sz,
        const uint32_t pos_x, const uint32_t pos_y,
        const bool allow_multiple_lines);

    /* Pointer to box presentation function */
    int (* present_box)(const uint32_t pos_x, const uint32_t pos_y,
        const uint32_t width, const uint32_t height, const uint16_t color);

    /* Pointer to clear presentation function */
    int (* clear)(const uint16_t color);

    /* Pointer to set text color presentation function */
    int (* set_text_color)(const uint16_t color);
} data_psn_module;


/**
 * @brief           Initialises the data presentation system.
 * @param[in,out]   module  Pointer to a pre-allocated data
 *                          presentation structure object.
 * @return          0 if successful, error code otherwise.
 **/
int data_psn_system_init(data_psn_module *module);

/**
 * @brief           Releases the data presentation system.
 * @param[in,out]   module  Pointer to a pre-allocated data
 *                          presentation structure object.
 * @return          0 if successful, error code otherwise.
 **/
int data_psn_system_release(data_psn_module *module);

#endif /* DATA_PSN_H */
