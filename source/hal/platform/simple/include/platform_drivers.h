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

#ifndef PLATFORM_DRIVERS_H
#define PLATFORM_DRIVERS_H

#include "log_macros.h"   /* Logging related helpers. */

/* Platform components */
#include "stubs/glcd.h"             /* LCD stubs to support use cases that use LCD */
#include "timer_simple_platform.h"  /* timer implementation */
#include "cmsis.h"                  /* For CPU related defintiions */

/** Platform definitions. TODO: These should be removed. */
#include "peripheral_memmap.h"  /* Peripheral memory map definitions. */
#include "peripheral_irqs.h"    /* IRQ numbers for this platform. */

/**
 * @brief   Initialises the platform components.
 * @return  0 if successful, error code otherwise.
 */
int platform_init(void);

/**
 * @brief   Teardown for platform components.
 */
void platform_release(void);

/**
 * @brief   Sets the platform name.
 * @param[out] name     Name of the platform to be set
 * @param[in]  size     Size of the input buffer
 */
void platform_name(char* name, size_t size);

#endif /* PLATFORM_DRIVERS_H */
