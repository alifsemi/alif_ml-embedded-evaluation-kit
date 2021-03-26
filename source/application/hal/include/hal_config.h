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
#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

/* This header provides some basic configuration for HAL */

/* Platform definitions for the systems we expect to support */
#define PLATFORM_CORTEX_M_BAREMETAL 1U
#define PLATFORM_UNKNOWN_LINUX_OS   3U

/* This should come from compile time definition */
#ifndef PLATFORM_HAL
    #define PLATFORM_HAL    PLATFORM_UNKNOWN_LINUX_OS    /* Default platform */
#endif /* PLATFORM_HAL */

#if ((PLATFORM_HAL) == PLATFORM_CORTEX_M_BAREMETAL)
    #include "bsp.h"
#elif ((PLATFORM_HAL) == PLATFORM_UNKNOWN_LINUX_OS)
    #include "dummy_log.h"
#else
    #error "Invalid platform!"
#endif /* PLATFORM_HAL==PLATFORM_CORTEX_M_BAREMETAL */

#if !defined (DESIGN_NAME)
    #define DESIGN_NAME   ("N/A")
#endif /* !defined (DESIGN_NAME) */

#endif /* HAL_CONFIG_H */
