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
#ifndef IRQS_H
#define IRQS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "peripheral_irqs.h"

#include <stdint.h>

/* Interrupt handler function type. */
typedef void (*const irq_vec_type)(void);

/**
 *  @brief  Reset interrupt handler and also, the starting
 *          point of the application.
 **/
extern void Reset_Handler(void);

/**
 * @brief   Gets the system tick triggered cycle counter for the CPU.
 * @return  64-bit counter value.
 **/
extern uint64_t Get_SysTick_Cycle_Count(void);

/**
 * @brief   Initialises the system tick registers.
 * @return  Error code return from sys tick configuration function
 *          (0 = no error).
 **/
extern int Init_SysTick(void);

#ifdef __cplusplus
}
#endif

#endif /* IRQS_H */
