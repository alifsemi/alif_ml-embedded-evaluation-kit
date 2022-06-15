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
 * @file     M55_HE.h
 * @author   Rupesh Kumar
 * @email    rupesh@alifsemi.com
 * @brief    CMSIS Core Peripheral Access Layer Header File for
 *           M55_HE Device (configured for ARMCM55 with
 *           Integer and half and single precision,
 *           DSP extension, MVE, TrustZone)
 * @version  V1.0.0
 * @date     23. Feb 2021
 * @bug      None
 * @Note	 None
 ******************************************************************************/

#ifndef M55_HE_H
#define M55_HE_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn           = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn                = -13,     /*  3 HardFault Interrupt */
  MemoryManagement_IRQn         = -12,     /*  4 Memory Management Interrupt */
  BusFault_IRQn                 = -11,     /*  5 Bus Fault Interrupt */
  UsageFault_IRQn               = -10,     /*  6 Usage Fault Interrupt */
  SecureFault_IRQn              =  -9,     /*  7 Secure Fault Interrupt */
  SVCall_IRQn                   =  -5,     /* 11 SV Call Interrupt */
  DebugMonitor_IRQn             =  -4,     /* 12 Debug Monitor Interrupt */
  PendSV_IRQn                   =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn                  =  -1,     /* 15 System Tick Interrupt */

/* -------------------  Processor Interrupt Numbers  ------------------------------ */
  Interrupt0_IRQn               =   0,
  Interrupt1_IRQn               =   1,
  Interrupt2_IRQn               =   2,
  Interrupt3_IRQn               =   3,
  Interrupt4_IRQn               =   4,
  Interrupt5_IRQn               =   5,
  Interrupt6_IRQn               =   6,
  Interrupt7_IRQn               =   7,
  Interrupt8_IRQn               =   8,
  Interrupt9_IRQn               =   9,
  UART0_IRQ                     =   126,
  UART1_IRQ                     =   127,
  UART2_IRQ                     =   128,
  UART3_IRQ                     =   129,
  UART4_IRQ                     =   130,
  UART5_IRQ                     =   131,
  UART6_IRQ                     =   132,
  UART7_IRQ                     =   133,
  I3C0_IRQ                      =   138,
  SPI0_IRQ                      =   139,
  SPI1_IRQ                      =   140,
  SPI2_IRQ                      =   141,
  SPI3_IRQ                      =   142,
  I2S0_IRQ                      =   143,
  I2S1_IRQ                      =   144,
  I2S2_IRQ                      =   145,
  I2S3_IRQ                      =   146,
  ETH_SBD_IRQ                   =   155,
  ADC0_INTR_DONE_IRQ            =   158,
  ADC1_INTR_DONE_IRQ            =   166,
  ADC2_INTR_DONE_IRQ            =   174,
  GPIO4_PIN0_IRQ                =   186,
  GPIO4_PIN1_IRQ                =   187,
  GPIO4_PIN2_IRQ                =   188,
  GPIO4_PIN3_IRQ                =   189,
  GPIO4_PIN4_IRQ                =   190,
  GPIO4_PIN5_IRQ                =   191,
  GPIO4_PIN6_IRQ                =   192,
  GPIO4_PIN7_IRQ                =   193,
  GPIO1_PIN0_IRQ                =   194,
  GPIO1_PIN1_IRQ                =   195,
  GPIO1_PIN2_IRQ                =   196,
  GPIO1_PIN3_IRQ                =   197,
  GPIO1_PIN4_IRQ                =   198,
  GPIO1_PIN5_IRQ                =   199,
  GPIO1_PIN6_IRQ                =   200,
  GPIO1_PIN7_IRQ                =   201,
  GPIO1_PIN8_IRQ                =   202,
  GPIO1_PIN9_IRQ                =   203,
  GPIO1_PIN10_IRQ               =   204,
  GPIO1_PIN11_IRQ               =   205,
  GPIO1_PIN12_IRQ               =   206,
  GPIO1_PIN13_IRQ               =   207,
  GPIO1_PIN14_IRQ               =   208,
  GPIO1_PIN15_IRQ               =   209,
  GPIO1_PIN16_IRQ               =   210,
  GPIO1_PIN17_IRQ               =   211,
  GPIO1_PIN18_IRQ               =   212,
  GPIO1_PIN19_IRQ               =   213,
  GPIO1_PIN20_IRQ               =   214,
  GPIO1_PIN21_IRQ               =   215,
  GPIO1_PIN22_IRQ               =   216,
  GPIO1_PIN23_IRQ               =   217,
  GPIO1_PIN24_IRQ               =   218,
  GPIO1_PIN25_IRQ               =   219,
  GPIO1_PIN26_IRQ               =   220,
  GPIO1_PIN27_IRQ               =   221,
  GPIO1_PIN28_IRQ               =   222,
  GPIO1_PIN29_IRQ               =   223,
  GPIO1_PIN30_IRQ               =   224,
  GPIO1_PIN31_IRQ               =   225,
  GPIO2_PIN0_IRQ                =   226,
  GPIO2_PIN1_IRQ                =   227,
  GPIO2_PIN2_IRQ                =   228,
  GPIO2_PIN3_IRQ                =   229,
  GPIO2_PIN4_IRQ                =   230,
  GPIO2_PIN5_IRQ                =   231,
  GPIO2_PIN6_IRQ                =   232,
  GPIO2_PIN7_IRQ                =   233,
  GPIO2_PIN8_IRQ                =   234,
  GPIO2_PIN9_IRQ                =   235,
  GPIO2_PIN10_IRQ               =   236,
  GPIO2_PIN11_IRQ               =   237,
  GPIO2_PIN12_IRQ               =   238,
  GPIO2_PIN13_IRQ               =   239,
  GPIO2_PIN14_IRQ               =   240,
  GPIO2_PIN15_IRQ               =   241,
  GPIO2_PIN16_IRQ               =   242,
  GPIO2_PIN17_IRQ               =   243,
  GPIO2_PIN18_IRQ               =   244,
  GPIO2_PIN19_IRQ               =   245,
  GPIO2_PIN20_IRQ               =   246,
  GPIO2_PIN21_IRQ               =   247,
  GPIO2_PIN22_IRQ               =   248,
  GPIO2_PIN23_IRQ               =   249,
  GPIO2_PIN24_IRQ               =   250,
  GPIO2_PIN25_IRQ               =   251,
  GPIO2_PIN26_IRQ               =   252,
  GPIO2_PIN27_IRQ               =   253,
  GPIO2_PIN28_IRQ               =   254,
  GPIO2_PIN29_IRQ               =   255,
  GPIO2_PIN30_IRQ               =   256,
  GPIO2_PIN31_IRQ               =   257,
  GPIO3_PIN0_IRQ                =   258,
  GPIO3_PIN1_IRQ                =   259,
  GPIO3_PIN2_IRQ                =   260,
  GPIO3_PIN3_IRQ                =   261,
  GPIO3_PIN4_IRQ                =   262,
  GPIO3_PIN5_IRQ                =   263,
  GPIO3_PIN6_IRQ                =   264,
  GPIO3_PIN7_IRQ                =   265,
  GPIO3_PIN8_IRQ                =   266,
  GPIO3_PIN9_IRQ                =   267,
  GPIO3_PIN10_IRQ               =   268,
  GPIO3_PIN11_IRQ               =   269,
  GPIO3_PIN12_IRQ               =   270,
  GPIO3_PIN13_IRQ               =   271,
  GPIO3_PIN14_IRQ               =   272,
  GPIO3_PIN15_IRQ               =   273,
  GPIO3_PIN16_IRQ               =   274,
  GPIO3_PIN17_IRQ               =   275,
  GPIO3_PIN18_IRQ               =   276,
  GPIO3_PIN19_IRQ               =   277,
  GPIO3_PIN20_IRQ               =   278,
  GPIO3_PIN21_IRQ               =   279,
  GPIO3_PIN22_IRQ               =   280,
  GPIO3_PIN23_IRQ               =   281,
  MIPI_CSI2_IRQ                 =   335,
  CAMERA0_IRQ                   =   336,
  LPTIMER_CHANNEL0_IRQ          =   337,
  LPTIMER_CHANNEL1_IRQ          =   338,
  LPTIMER_CHANNEL2_IRQ          =   339,
  LPTIMER_CHANNEL3_IRQ          =   340,
  RTC0_IRQ                      =   341,
  UTIMER0_IRQ                   =   368,
  UTIMER1_IRQ                   =   369,
  UTIMER2_IRQ                   =   370,
  UTIMER3_IRQ                   =   371,
  UTIMER4_IRQ                   =   372,
  UTIMER5_IRQ                   =   373,
  UTIMER6_IRQ                   =   374,
  UTIMER7_IRQ                   =   375,
  UTIMER8_IRQ                   =   376,
  UTIMER9_IRQ                   =   377,
  UTIMER10_IRQ                  =   378,
  UTIMER11_IRQ                  =   379,
  UTIMER12_IRQ                  =   380,
  UTIMER13_IRQ                  =   381,
  UTIMER14_IRQ                  =   382,
  UTIMER15_IRQ                  =   383,
  UTIMER16_IRQ                  =   384,
  UTIMER17_IRQ                  =   385,
  UTIMER18_IRQ                  =   386,
  UTIMER19_IRQ                  =   387,
  UTIMER20_IRQ                  =   388,
  UTIMER21_IRQ                  =   389,
  UTIMER22_IRQ                  =   390,
  UTIMER23_IRQ                  =   391,
  UTIMER24_IRQ                  =   392,
  UTIMER25_IRQ                  =   393,
  UTIMER26_IRQ                  =   394,
  UTIMER27_IRQ                  =   395,
  UTIMER28_IRQ                  =   396,
  UTIMER29_IRQ                  =   397,
  UTIMER30_IRQ                  =   398,
  UTIMER31_IRQ                  =   399,
  UTIMER32_IRQ                  =   400,
  UTIMER33_IRQ                  =   401,
  UTIMER34_IRQ                  =   402,
  UTIMER35_IRQ                  =   403,
  UTIMER36_IRQ                  =   404,
  UTIMER37_IRQ                  =   405,
  UTIMER38_IRQ                  =   406,
  UTIMER39_IRQ                  =   407,
  UTIMER40_IRQ                  =   408,
  UTIMER41_IRQ                  =   409,
  UTIMER42_IRQ                  =   410,
  UTIMER43_IRQ                  =   411,
  UTIMER44_IRQ                  =   412,
  UTIMER45_IRQ                  =   413,
  UTIMER46_IRQ                  =   414,
  UTIMER47_IRQ                  =   415,
  UTIMER48_IRQ                  =   416,
  UTIMER49_IRQ                  =   417,
  UTIMER50_IRQ                  =   418,
  UTIMER51_IRQ                  =   419,
  UTIMER52_IRQ                  =   420,
  UTIMER53_IRQ                  =   421,
  UTIMER54_IRQ                  =   422,
  UTIMER55_IRQ                  =   423,
  UTIMER56_IRQ                  =   424,
  UTIMER57_IRQ                  =   425,
  UTIMER58_IRQ                  =   426,
  UTIMER59_IRQ                  =   427,
  UTIMER60_IRQ                  =   428,
  UTIMER61_IRQ                  =   429,
  UTIMER62_IRQ                  =   430,
  UTIMER63_IRQ                  =   431,
  UTIMER64_IRQ                  =   432,
  UTIMER65_IRQ                  =   433,
  UTIMER66_IRQ                  =   434,
  UTIMER67_IRQ                  =   435,
  UTIMER68_IRQ                  =   436,
  UTIMER69_IRQ                  =   437,
  UTIMER70_IRQ                  =   438,
  UTIMER71_IRQ                  =   439,
  UTIMER72_IRQ                  =   440,
  UTIMER73_IRQ                  =   441,
  UTIMER74_IRQ                  =   442,
  UTIMER75_IRQ                  =   443,
  UTIMER76_IRQ                  =   444,
  UTIMER77_IRQ                  =   445,
  UTIMER78_IRQ                  =   446,
  UTIMER79_IRQ                  =   447,
  UTIMER80_IRQ                  =   448,
  UTIMER81_IRQ                  =   449,
  UTIMER82_IRQ                  =   400,
  UTIMER83_IRQ                  =   451,
  UTIMER84_IRQ                  =   452,
  UTIMER85_IRQ                  =   453,
  UTIMER86_IRQ                  =   454,
  UTIMER87_IRQ                  =   455,
  UTIMER88_IRQ                  =   456,
  UTIMER89_IRQ                  =   457,
  UTIMER90_IRQ                  =   458,
  UTIMER91_IRQ                  =   459,
  UTIMER92_IRQ                  =   460,
  UTIMER93_IRQ                  =   461,
  UTIMER94_IRQ                  =   462,
  UTIMER95_IRQ                  =   463
  /* Interrupts 464 .. 480 are left out */
} IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* --------  Configuration of Core Peripherals  ----------------------------------- */
#define __CM55_REV                0x0001U   /* Core revision r0p1 */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __FPU_PRESENT             1U        /* FPU present */
#define __FPU_DP                  1U        /* double precision FPU present*/
#define __DSP_PRESENT             1U        /* DSP extension present */
#define __SAUREGION_PRESENT       1U        /* SAU regions present */
#define __PMU_PRESENT             1U        /* PMU present */
#define __PMU_NUM_EVENTCNT        8U        /* PMU Event Counters */
#define __ICACHE_PRESENT          1U        /* Instruction Cache present */
#define __DCACHE_PRESENT          1U        /* Data Cache present */

#include "core_cm55.h"                      /* Processor and core peripherals */
#include "system_M55_HE.h"                 /* System Header */
#include "M55_HE_map.h"                     /* Memory Map */
#include "system_utils.h"                  /* Utility functions */


/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


#ifdef __cplusplus
}
#endif

#endif  /* M55_HE_H */
