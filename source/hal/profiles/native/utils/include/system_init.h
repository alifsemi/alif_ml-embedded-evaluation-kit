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
#ifndef NATIVE_SYSTEM_INIT_H
#define NATIVE_SYSTEM_INIT_H

#include <stddef.h>
/**
 * @brief  Platform initialisation for native platform.
 **/
int system_init(void);

/**
 * @brief  Platform release for native platform.
 **/
void system_release(void);

/**
 * @brief       Returns the name of the platform.
 * @param[out]  name Platform name string.
 * @param[in]   size Name string length.
 */
void system_name(char* name, size_t size);

#endif /* NATIVE_SYSTEM_INIT_H */
