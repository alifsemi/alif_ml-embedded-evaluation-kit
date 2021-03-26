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
#include "cmsis.h"

extern void *__Vectors;                   /* see irqs.c */

/*----------------------------------------------------------------------------*\
 *                        Define clocks (uses OSC1 ACLK)                      *
\*----------------------------------------------------------------------------*/
#define __XTAL            (25000000)      /* Oscillator frequency             */
#define __SYSTEM_CLOCK    (__XTAL)

#define STR(x) #x
#define RESET_REG(n) __ASM volatile("MOV " STR(r##n) ", #0" : : : STR(r##n))

#if defined(CPU_CORTEX_M55)
#define CCR_DL   (1 << 19)
#else
#error  "Invalid CPU; This file only services Cortex-M55 CPUs"
#endif /* (CPU_CORTEX_M55) */

/*----------------------------------------------------------------------------
  System Core Clock Variable (Core Clock)
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __SYSTEM_CLOCK;


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
/**
 * @brief  Updates the SystemCoreClock variable with current core Clock
 *         retrieved from cpu registers.
 */
void SystemCoreClockUpdate(void)
{
    /* Update the SystemCoreClock variable */
    SystemCoreClock = __SYSTEM_CLOCK;
}

uint32_t GetSystemCoreClock(void)
{
    return SystemCoreClock;
}

/**
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 **/
void SystemInit(void)
{
#if (defined (__FPU_USED) && (__FPU_USED == 1U)) || \
    (defined (__MVE_USED) && (__MVE_USED == 1U))
  SCB->CPACR |= ((3U << 10U*2U) |   /* enable CP10 Full Access */
                 (3U << 11U*2U) );
#endif

    /* Initialise registers r0-r12 and LR(=r14)
     * They must have a valid value before being potentially pushed to stack by
     * C calling convention or by context saving in exception handling
     */
    RESET_REG(0);
    RESET_REG(1);
    RESET_REG(2);
    RESET_REG(3);
    RESET_REG(4);
    RESET_REG(5);
    RESET_REG(6);
    RESET_REG(7);
    RESET_REG(8);
    RESET_REG(9);
    RESET_REG(10);
    RESET_REG(11);
    RESET_REG(12);
    RESET_REG(14);

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t) &__Vectors;
#endif

    /* Enable hard, bus, mem and usage fault detection in SHCSR, bits 16-18.
     * Enable stkof, bf, div_0_trp, unalign_trp and usersetm bits in CCR.
     */
    SCB->SHCSR = (
        _VAL2FLD(SCB_SHCSR_USGFAULTENA, 1) |
        _VAL2FLD(SCB_SHCSR_BUSFAULTENA, 1) |
        _VAL2FLD(SCB_SHCSR_MEMFAULTENA, 1));

    SCB->CCR = (_VAL2FLD(SCB_CCR_USERSETMPEND, 1) |
                _VAL2FLD(SCB_CCR_DIV_0_TRP, 1)    |
                _VAL2FLD(SCB_CCR_BFHFNMIGN, 1)    |
                _VAL2FLD(SCB_CCR_STKOFHFNMIGN, 1));
#ifdef UNALIGNED_SUPPORT_DISABLE
    SCB->CCR |= _VAL2FLD(SCB_CCR_UNALIGN_TRP, 1);
#endif

    SCB->CCR |= CCR_DL;

  /* Reset pipeline. */
  __DSB();
  __ISB();

#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

  SystemCoreClock = __SYSTEM_CLOCK;
}
