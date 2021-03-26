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
#ifndef BSP_H
#define BSP_H

/* Core modules - these are common */
#include "bsp_core_log.h"   /* Logging related helpers. */
#include "uart_stdout.h"    /* stdout over UART. */

#if defined(MPS3_PLATFORM) /* If running on MPS3 platform. */

#include "smm_mps3.h"       /* Mem map for MPS3 peripherals. */
#include "glcd_mps3.h"      /* LCD functions. */
#include "timer_mps3.h"     /* Timer functions. */
#include "device_mps3.h"    /* FPGA level definitions and functions. */

#else /* MPS3_PLATFORM */

#include "stubs_fvp.h"      /* Stubs for FVP. */
#include "timer_fvp.h"      /* Timer API for FVP. */

#endif /* MPS3_PLATFORM */

#endif /* BSP_H */
