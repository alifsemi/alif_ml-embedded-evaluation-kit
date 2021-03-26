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
#ifndef DATA_ACQ_H
#define DATA_ACQ_H

/**
 * This file is the top level abstraction for the data acquisition module.
 **/
#include <stdint.h>

/* Structure to encompass the data acquisition module and it's methods. */
typedef struct data_acquisition_module {
    int inited;                 /**< initialised or not. */
    char system_name[8];        /**< name(s) of the channel in use. */
    int (* system_init)(void);  /**< channel initialisation function. */

    /* Function to go and check if there are any events that require handling. */
    int (* get_input)(char *user_input, int size);
} data_acq_module;

/**
 * @brief           Initialise the data acquisition channel: goes and
 *                  sets the required channel up for usage.
 * @param[in,out]   module  Pointer to a pre-allocated data
 *                          acquisition structure object.
 * @return          0 if successful, error code otherwise.
 **/
int data_acq_channel_init(data_acq_module *module);

/**
 * @brief           Releases the data acquisition channel.
 * @param[in,out]   module  Pointer to a pre-allocated data
 *                          acquisition structure object.
 * @return          0 if successful, error code otherwise.
 **/
int data_acq_channel_release(data_acq_module *module);

#endif /* DATA_ACQ_H */
