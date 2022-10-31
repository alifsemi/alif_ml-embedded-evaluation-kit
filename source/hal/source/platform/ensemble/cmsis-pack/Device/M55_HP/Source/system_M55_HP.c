/* Copyright (c) 2021 - 2022 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
/*
 * Copyright (c) 2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     system_M55_HP.c
 * @author   Rupesh Kumar
 * @email    rupesh@alifsemi.com
 * @brief    CMSIS Device System Source File for
 *           Alif Semiconductor HPCM55 Device
 * @version  V1.0.0
 * @date     19. Feb 2021
 * @bug      None
 * @Note	 None
 ******************************************************************************/

#if defined (M55_HP)
  #include "M55_HP.h"
#else
  #error device not specified!
#endif

  #if defined (__ARM_FEATURE_CMSE) &&  (__ARM_FEATURE_CMSE == 3U)
    #include "partition_M55_HP.h"
    #include "tgu_M55_HP.h"
  #endif

#if defined (__MPU_PRESENT)
  #include <mpu_M55_HP.h>
#endif

#include "app_map.h"
/**
 * Application CPU identifiers
 */
#define TOC_IMAGE_CPU_ID_MASK       0x0000000Fu
#define TOC_IMAGE_CPU_A32_0         0u
#define TOC_IMAGE_CPU_A32_1         1u
#define TOC_IMAGE_CPU_M55_HP        2u
#define TOC_IMAGE_CPU_M55_HE        3u

#define MINI_TOC_SIGNATURE 			0xFFA0A710u

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

typedef struct
{
  uint32_t loadAddress;
  uint32_t configuration;
  uint32_t execAddress;
  uint32_t objSize;
} atoc_t;

static const atoc_t __mram_atoc = {

  .loadAddress = 0xFFFFFFFF, // Indicate XIP Mode
  .configuration = MINI_TOC_SIGNATURE + TOC_IMAGE_CPU_M55_HP,
  .execAddress = _APP_ADDRESS,
  .objSize = 0
};

atoc_t *__dummy;

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define  MHZ            ( 1000000UL)

#define  SYSTEM_CLOCK    (400U * MHZ)


/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
extern const VECTOR_TABLE_Type __VECTOR_TABLE[496];


/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;


/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)
{
  SystemCoreClock = SYSTEM_CLOCK;
}

/*----------------------------------------------------------------------------
  Get System Core Clock function
 *----------------------------------------------------------------------------*/
uint32_t GetSystemCoreClock (void)
{
  return SystemCoreClock;
}

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t)(&__VECTOR_TABLE[0]);
#endif

  /* Enable UsageFault, BusFault, MemFault and SecurityFault exceptions */
  /* Otherwise all you see is HardFault, even in the debugger */
  SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk |
                SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_SECUREFAULTENA_Msk;

#if (defined (__FPU_USED) && (__FPU_USED == 1U)) || \
    (defined (__ARM_FEATURE_MVE) && (__ARM_FEATURE_MVE > 0U))
  SCB->CPACR |= ((3U << 10U*2U) |           /* enable CP10 Full Access */
                 (3U << 11U*2U)  );         /* enable CP11 Full Access */
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

#ifdef __ICACHE_PRESENT
  /*Enable ICache*/
  SCB_InvalidateICache();
  SCB_EnableICache();
#endif
#ifdef __DCACHE_PRESENT
 /*Enable DCache*/
  SCB_InvalidateDCache();
  SCB_EnableDCache();
#endif

// Enable Loop and branch info cache
SCB->CCR |= SCB_CCR_LOB_Msk;
__ISB();

#if defined (__MPU_PRESENT)
  MPU_Setup();
#endif

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  TZ_SAU_Setup();
  TGU_Setup();
#endif
  // Initialize Reset VTOR registers
  *((uint32_t *)0x7100A080) = (uint32_t)(&__VECTOR_TABLE[0]);
  *((uint32_t *)0x7100A084) = (uint32_t)(&__VECTOR_TABLE[0]);

  SystemCoreClock = SYSTEM_CLOCK;
  __dummy = &__mram_atoc;
  //Enable the PMU 
  ARM_PMU_Enable();

  //Enable PMU Cycle Counter
  ARM_PMU_CNTR_Enable(PMU_CNTENSET_CCNTR_ENABLE_Msk);
}
