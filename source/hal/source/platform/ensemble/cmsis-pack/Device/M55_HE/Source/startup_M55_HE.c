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
 * @file     startup_M55_HE.c
 * @author   Rupesh Kumar
 * @email    rupesh@alifsemi.com
 * @brief    CMSIS Core Device Startup File for
 *           Alif Semiconductor M55_HE Device
 * @version  V1.0.0
 * @date     23. Feb 2021
 * @bug      None
 * @Note	 None
 ******************************************************************************/

#if defined (M55_HE)
  #include "M55_HE.h"
#else
  #error device not specified!
#endif

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern __NO_RETURN void __PROGRAM_START(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler  (void);
            void Default_Handler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak));
void MemManage_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void SecureFault_Handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

void Interrupt0_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt1_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt2_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt3_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt4_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt5_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt6_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt7_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt8_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));
void Interrupt9_Handler     (void) __attribute__ ((weak, alias("Default_Handler")));

void UART0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART5_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART6_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART7_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));

void I3C0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void SPI0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void I2S0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void ETH_SBD_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));

void ADC0_INTR_DONE_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_INTR_DONE_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC2_INTR_DONE_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));

void GPIO4_PIN0_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO4_PIN1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO4_PIN2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO4_PIN3_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO4_PIN4_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO4_PIN5_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO4_PIN6_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO4_PIN7_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN0_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN3_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN4_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN5_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN6_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN7_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN8_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN9_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN10_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN11_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN12_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN13_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN14_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN15_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN16_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN17_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN18_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN19_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN20_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN21_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN22_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN23_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN24_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN25_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN26_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN27_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN28_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN29_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN30_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO1_PIN31_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN0_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN3_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN4_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN5_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN6_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN7_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN8_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN9_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN10_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN11_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN12_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN13_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN14_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN15_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN16_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN17_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN18_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN19_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN20_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN21_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN22_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN23_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN24_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN25_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN26_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN27_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN28_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN29_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN30_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO2_PIN31_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN0_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN3_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN4_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN5_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN6_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN7_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN8_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN9_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN10_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN11_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN12_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN13_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN14_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN15_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN16_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN17_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN18_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN19_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN20_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN21_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN22_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN23_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));

void MIPI_CSI2_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));

void CAMERA0_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));

void LPTIMER_CHANNEL0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIMER_CHANNEL1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIMER_CHANNEL2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIMER_CHANNEL3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void RTC0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void UTIMER_IRQHandler0     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler1     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler2     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler3     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler4     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler5     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler6     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler7     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler8     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler9     (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler10    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler11    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler12    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler13    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler14    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler15    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler16    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler17    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler18    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler19    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler20    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler21    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler22    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler23    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler24    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler25    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler26    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler27    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler28    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler29    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler30    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler31    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler32    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler33    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler34    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler35    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler36    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler37    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler38    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler39    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler40    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler41    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler42    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler43    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler44    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler45    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler46    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler47    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler48    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler49    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler50    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler51    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler52    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler53    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler54    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler55    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler56    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler57    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler58    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler59    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler60    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler61    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler62    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler63    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler64    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler65    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler66    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler67    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler68    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler69    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler70    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler71    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler72    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler73    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler74    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler75    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler76    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler77    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler78    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler79    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler80    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler81    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler82    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler83    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler84    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler85    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler86    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler87    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler88    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler89    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler90    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler91    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler92    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler93    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler94    (void) __attribute__ ((weak, alias("Default_Handler")));
void UTIMER_IRQHandler95    (void) __attribute__ ((weak, alias("Default_Handler")));

void arm_ethosu_npu_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
/*
void mhu0_se_sender_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void mhu0_se_receiver_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void mhu1_se_sender_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void mhu1_se_receiver_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void mhu0_m55_sender_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void mhu0_m55_receiver_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void mhu1_m55_sender_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
void mhu1_m55_receiver_irq_handler    (void) __attribute__ ((weak, alias("Default_Handler")));
*/

void mhu0_se_sender_irq_handler    (void) ;
void mhu0_se_receiver_irq_handler    (void);
void mhu1_se_sender_irq_handler    (void);
void mhu1_se_receiver_irq_handler    (void);
void mhu0_m55_sender_irq_handler    (void) ;
void mhu0_m55_receiver_irq_handler    (void);
void mhu1_m55_sender_irq_handler    (void);
void mhu1_m55_receiver_irq_handler    (void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[496];
       const VECTOR_TABLE_Type __VECTOR_TABLE[496] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),       /*     Initial Stack Pointer */
  Reset_Handler,                            /*     Reset Handler */
  NMI_Handler,                              /* -14 NMI Handler */
  HardFault_Handler,                        /* -13 Hard Fault Handler */
  MemManage_Handler,                        /* -12 MPU Fault Handler */
  BusFault_Handler,                         /* -11 Bus Fault Handler */
  UsageFault_Handler,                       /* -10 Usage Fault Handler */
  SecureFault_Handler,                      /*  -9 Secure Fault Handler */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  0,                                        /*     Reserved */
  SVC_Handler,                              /*  -5 SVCall Handler */
  DebugMon_Handler,                         /*  -4 Debug Monitor Handler */
  0,                                        /*     Reserved */
  PendSV_Handler,                           /*  -2 PendSV Handler */
  SysTick_Handler,                          /*  -1 SysTick Handler */

  /* Interrupts */
  Interrupt0_Handler,                       /*   0 Interrupt 0 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   10 Interrupt 10 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   20 Interrupt 20 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   30 Interrupt 30 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  mhu0_se_receiver_irq_handler,                       /*   7 Interrupt 7 */
  mhu0_se_sender_irq_handler,                       /*   8 Interrupt 8 */
  mhu1_se_receiver_irq_handler,                       /*   9 Interrupt 9 */
  mhu1_se_sender_irq_handler,                       /*   40 Interrupt 40 */
  mhu0_m55_receiver_irq_handler,                       /*   1 Interrupt 1 */
  mhu0_m55_sender_irq_handler,                       /*   2 Interrupt 2 */
  mhu1_m55_receiver_irq_handler,                       /*   3 Interrupt 3 */
  mhu1_m55_sender_irq_handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   50 Interrupt 50 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  arm_ethosu_npu_irq_handler,                      /*   Ethos-U55 Interrupt */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   60 Interrupt 60 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   70 Interrupt 70 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   80 Interrupt 80 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   90 Interrupt 90 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   100 Interrupt 100 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   110 Interrupt 110 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 9 */
  Interrupt0_Handler,                       /*   120 Interrupt 120 */
  Interrupt1_Handler,                       /*   1 Interrupt 1 */
  Interrupt2_Handler,                       /*   2 Interrupt 2 */
  Interrupt3_Handler,                       /*   3 Interrupt 3 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  UART0_IRQHandler,                         /*   126 Interrupt 126 */
  UART1_IRQHandler,                         /*   127 Interrupt 127 */
  UART2_IRQHandler,                         /*   128 Interrupt 128 */
  UART3_IRQHandler,                         /*   129 Interrupt 129 */
  UART4_IRQHandler,                         /*   130 Interrupt 130 */
  UART5_IRQHandler,                         /*   131 Interrupt 131 */
  UART6_IRQHandler,                         /*   132 Interrupt 132 */
  UART7_IRQHandler,                         /*   133 Interrupt 133 */
  Interrupt4_Handler,                       /*   4 Interrupt 4 */
  Interrupt5_Handler,                       /*   5 Interrupt 5 */
  Interrupt6_Handler,                       /*   6 Interrupt 6 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  I3C0_IRQHandler,                          /*   138 Interrupt 138 */
  SPI0_IRQHandler,                          /*   139 Interrupt 139 */
  SPI1_IRQHandler,                          /*   140 Interrupt 140 */
  SPI2_IRQHandler,                          /*   141 Interrupt 141 */
  SPI3_IRQHandler,                          /*   142 Interrupt 142 */
  I2S0_IRQHandler,                          /*   143 Interrupt 143 */
  I2S1_IRQHandler,                          /*   144 Interrupt 144 */
  I2S2_IRQHandler,                          /*   145 Interrupt 145 */
  I2S3_IRQHandler,                          /*   146 Interrupt 146 */
  Interrupt7_Handler,                       /*   7 Interrupt 7 */
  Interrupt8_Handler,                       /*   8 Interrupt 8 */
  Interrupt9_Handler,                       /*   9 Interrupt 8 */
  Interrupt0_Handler,                       /*   150 Interrupt 150 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  ETH_SBD_IRQHandler,                       /*   155  Interrupt 155 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  ADC0_INTR_DONE_IRQHandler,                /*   158  Interrupt 158 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   160  Interrupt 160 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  ADC1_INTR_DONE_IRQHandler,                /*   166  Interrupt 166 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   170  Interrupt 170 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  ADC2_INTR_DONE_IRQHandler,                /*   174  Interrupt 174 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   180  Interrupt 180 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  GPIO4_PIN0_IRQHandler,                    /*   186 Interrupt 186  */
  GPIO4_PIN1_IRQHandler,                    /*   187 Interrupt 187  */
  GPIO4_PIN2_IRQHandler,                    /*   188 Interrupt 188  */
  GPIO4_PIN3_IRQHandler,                    /*   189 Interrupt 189  */
  GPIO4_PIN4_IRQHandler,                    /*   190 Interrupt 190  */
  GPIO4_PIN5_IRQHandler,                    /*   191 Interrupt 191  */
  GPIO4_PIN6_IRQHandler,                    /*   192 Interrupt 192  */
  GPIO4_PIN7_IRQHandler,                    /*   193 Interrupt 193  */
  GPIO1_PIN0_IRQHandler,                    /*   194  Interrupt 194 */
  GPIO1_PIN1_IRQHandler,                    /*   195  Interrupt 195 */
  GPIO1_PIN2_IRQHandler,                    /*   196  Interrupt 196 */
  GPIO1_PIN3_IRQHandler,                    /*   197  Interrupt 197 */
  GPIO1_PIN4_IRQHandler,                    /*   198  Interrupt 198 */
  GPIO1_PIN5_IRQHandler,                    /*   199  Interrupt 199 */
  GPIO1_PIN6_IRQHandler,                    /*   200  Interrupt 200 */
  GPIO1_PIN7_IRQHandler,                    /*   201  Interrupt 201 */
  GPIO1_PIN8_IRQHandler,                    /*   202  Interrupt 202 */
  GPIO1_PIN9_IRQHandler,                    /*   203  Interrupt 203 */
  GPIO1_PIN10_IRQHandler,                   /*   204  Interrupt 204 */
  GPIO1_PIN11_IRQHandler,                   /*   205  Interrupt 205 */
  GPIO1_PIN12_IRQHandler,                   /*   206  Interrupt 206 */
  GPIO1_PIN13_IRQHandler,                   /*   207  Interrupt 207 */
  GPIO1_PIN14_IRQHandler,                   /*   208  Interrupt 208 */
  GPIO1_PIN15_IRQHandler,                   /*   209  Interrupt 209 */
  GPIO1_PIN16_IRQHandler,                   /*   210  Interrupt 210 */
  GPIO1_PIN17_IRQHandler,                   /*   211  Interrupt 211 */
  GPIO1_PIN18_IRQHandler,                   /*   212  Interrupt 212 */
  GPIO1_PIN19_IRQHandler,                   /*   213  Interrupt 213 */
  GPIO1_PIN20_IRQHandler,                   /*   214  Interrupt 214 */
  GPIO1_PIN21_IRQHandler,                   /*   215  Interrupt 215 */
  GPIO1_PIN22_IRQHandler,                   /*   216  Interrupt 216 */
  GPIO1_PIN23_IRQHandler,                   /*   217  Interrupt 217 */
  GPIO1_PIN24_IRQHandler,                   /*   218  Interrupt 218 */
  GPIO1_PIN25_IRQHandler,                   /*   219  Interrupt 219 */
  GPIO1_PIN26_IRQHandler,                   /*   220  Interrupt 220 */
  GPIO1_PIN27_IRQHandler,                   /*   221  Interrupt 221 */
  GPIO1_PIN28_IRQHandler,                   /*   222  Interrupt 222 */
  GPIO1_PIN29_IRQHandler,                   /*   223  Interrupt 223 */
  GPIO1_PIN30_IRQHandler,                   /*   224  Interrupt 224 */
  GPIO1_PIN31_IRQHandler,                   /*   225  Interrupt 225 */
  GPIO2_PIN0_IRQHandler,                    /*   226  Interrupt 226 */
  GPIO2_PIN1_IRQHandler,                    /*   227  Interrupt 227 */
  GPIO2_PIN2_IRQHandler,                    /*   228  Interrupt 228 */
  GPIO2_PIN3_IRQHandler,                    /*   229  Interrupt 229 */
  GPIO2_PIN4_IRQHandler,                    /*   230  Interrupt 230 */
  GPIO2_PIN5_IRQHandler,                    /*   231  Interrupt 231 */
  GPIO2_PIN6_IRQHandler,                    /*   232  Interrupt 232 */
  GPIO2_PIN7_IRQHandler,                    /*   233  Interrupt 233 */
  GPIO2_PIN8_IRQHandler,                    /*   234  Interrupt 234 */
  GPIO2_PIN9_IRQHandler,                    /*   235  Interrupt 235 */
  GPIO2_PIN10_IRQHandler,                   /*   236  Interrupt 236 */
  GPIO2_PIN11_IRQHandler,                   /*   237  Interrupt 237 */
  GPIO2_PIN12_IRQHandler,                   /*   238  Interrupt 238 */
  GPIO2_PIN13_IRQHandler,                   /*   239  Interrupt 239 */
  GPIO2_PIN14_IRQHandler,                   /*   240  Interrupt 240 */
  GPIO2_PIN15_IRQHandler,                   /*   241  Interrupt 241 */
  GPIO2_PIN16_IRQHandler,                   /*   242  Interrupt 242 */
  GPIO2_PIN17_IRQHandler,                   /*   243  Interrupt 243 */
  GPIO2_PIN18_IRQHandler,                   /*   244  Interrupt 244 */
  GPIO2_PIN19_IRQHandler,                   /*   245  Interrupt 245 */
  GPIO2_PIN20_IRQHandler,                   /*   246  Interrupt 246 */
  GPIO2_PIN21_IRQHandler,                   /*   247  Interrupt 247 */
  GPIO2_PIN22_IRQHandler,                   /*   248  Interrupt 248 */
  GPIO2_PIN23_IRQHandler,                   /*   249  Interrupt 249 */
  GPIO2_PIN24_IRQHandler,                   /*   250  Interrupt 250 */
  GPIO2_PIN25_IRQHandler,                   /*   251  Interrupt 251 */
  GPIO2_PIN26_IRQHandler,                   /*   252  Interrupt 252 */
  GPIO2_PIN27_IRQHandler,                   /*   253  Interrupt 253 */
  GPIO2_PIN28_IRQHandler,                   /*   254  Interrupt 254 */
  GPIO2_PIN29_IRQHandler,                   /*   255  Interrupt 255 */
  GPIO2_PIN30_IRQHandler,                   /*   256  Interrupt 256 */
  GPIO2_PIN31_IRQHandler,                   /*   257  Interrupt 257 */
  GPIO3_PIN0_IRQHandler,                    /*   258  Interrupt 258 */
  GPIO3_PIN1_IRQHandler,                    /*   259  Interrupt 259 */
  GPIO3_PIN2_IRQHandler,                    /*   260  Interrupt 260 */
  GPIO3_PIN3_IRQHandler,                    /*   261  Interrupt 261 */
  GPIO3_PIN4_IRQHandler,                    /*   262  Interrupt 262 */
  GPIO3_PIN5_IRQHandler,                    /*   263  Interrupt 263 */
  GPIO3_PIN6_IRQHandler,                    /*   264  Interrupt 264 */
  GPIO3_PIN7_IRQHandler,                    /*   265  Interrupt 265 */
  GPIO3_PIN8_IRQHandler,                    /*   266  Interrupt 266 */
  GPIO3_PIN9_IRQHandler,                    /*   267  Interrupt 267 */
  GPIO3_PIN10_IRQHandler,                   /*   268  Interrupt 268 */
  GPIO3_PIN11_IRQHandler,                   /*   269  Interrupt 269 */
  GPIO3_PIN12_IRQHandler,                   /*   270  Interrupt 270 */
  GPIO3_PIN13_IRQHandler,                   /*   271  Interrupt 271 */
  GPIO3_PIN14_IRQHandler,                   /*   272  Interrupt 272 */
  GPIO3_PIN15_IRQHandler,                   /*   273  Interrupt 273 */
  GPIO3_PIN16_IRQHandler,                   /*   274  Interrupt 274 */
  GPIO3_PIN17_IRQHandler,                   /*   275  Interrupt 275 */
  GPIO3_PIN18_IRQHandler,                   /*   276  Interrupt 276 */
  GPIO3_PIN19_IRQHandler,                   /*   277  Interrupt 277 */
  GPIO3_PIN20_IRQHandler,                   /*   278  Interrupt 278 */
  GPIO3_PIN21_IRQHandler,                   /*   279  Interrupt 279 */
  GPIO3_PIN22_IRQHandler,                   /*   280  Interrupt 280 */
  GPIO3_PIN23_IRQHandler,                   /*   281  Interrupt 281 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   290  Interrupt 290 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   300  Interrupt 300 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   310  Interrupt 310 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   320  Interrupt 320 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   330  Interrupt 330 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  MIPI_CSI2_IRQHandler,                     /*   335  Interrupt 335 */
  CAMERA0_IRQHandler,                       /*   336  Interrupt 336 */
  LPTIMER_CHANNEL0_IRQHandler,              /*   337  Interrupt 337 */
  LPTIMER_CHANNEL1_IRQHandler,              /*   338  Interrupt 338 */
  LPTIMER_CHANNEL2_IRQHandler,              /*   339  Interrupt 339 */
  LPTIMER_CHANNEL3_IRQHandler,              /*   340  Interrupt 340 */
  RTC0_IRQHandler,                          /*   341  Interrupt 341 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   350  Interrupt 350 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  Interrupt8_Handler,                       /*   8  Interrupt 8 */
  Interrupt9_Handler,                       /*   9  Interrupt 9 */
  Interrupt0_Handler,                       /*   360  Interrupt 360 */
  Interrupt1_Handler,                       /*   1  Interrupt 1 */
  Interrupt2_Handler,                       /*   2  Interrupt 2 */
  Interrupt3_Handler,                       /*   3  Interrupt 3 */
  Interrupt4_Handler,                       /*   4  Interrupt 4 */
  Interrupt5_Handler,                       /*   5  Interrupt 5 */
  Interrupt6_Handler,                       /*   6  Interrupt 6 */
  Interrupt7_Handler,                       /*   7  Interrupt 7 */
  UTIMER_IRQHandler0,                       /*   368 Interrupt 368 */
  UTIMER_IRQHandler1,                       /*   369 Interrupt 369 */
  UTIMER_IRQHandler2,                       /*   370 Interrupt 370 */
  UTIMER_IRQHandler3,                       /*   371 Interrupt 371 */
  UTIMER_IRQHandler4,                       /*   372 Interrupt 372 */
  UTIMER_IRQHandler5,                       /*   373 Interrupt 373 */
  UTIMER_IRQHandler6,                       /*   374 Interrupt 374 */
  UTIMER_IRQHandler7,                       /*   375 Interrupt 375 */
  UTIMER_IRQHandler8,                       /*   376 Interrupt 376 */
  UTIMER_IRQHandler9,                       /*   377 Interrupt 377 */
  UTIMER_IRQHandler10,                      /*   378 Interrupt 378 */
  UTIMER_IRQHandler11,                      /*   379 Interrupt 379 */
  UTIMER_IRQHandler12,                      /*   380 Interrupt 380 */
  UTIMER_IRQHandler13,                      /*   381 Interrupt 381 */
  UTIMER_IRQHandler14,                      /*   382 Interrupt 382 */
  UTIMER_IRQHandler15,                      /*   383 Interrupt 383 */
  UTIMER_IRQHandler16,                      /*   384 Interrupt 384 */
  UTIMER_IRQHandler17,                      /*   385 Interrupt 385 */
  UTIMER_IRQHandler18,                      /*   386 Interrupt 386 */
  UTIMER_IRQHandler19,                      /*   387 Interrupt 387 */
  UTIMER_IRQHandler20,                      /*   388 Interrupt 388 */
  UTIMER_IRQHandler21,                      /*   389 Interrupt 389 */
  UTIMER_IRQHandler22,                      /*   390 Interrupt 390 */
  UTIMER_IRQHandler23,                      /*   391 Interrupt 391 */
  UTIMER_IRQHandler24,                      /*   392 Interrupt 392 */
  UTIMER_IRQHandler25,                      /*   393 Interrupt 393 */
  UTIMER_IRQHandler26,                      /*   394 Interrupt 394 */
  UTIMER_IRQHandler27,                      /*   395 Interrupt 395 */
  UTIMER_IRQHandler28,                      /*   396 Interrupt 396 */
  UTIMER_IRQHandler29,                      /*   397 Interrupt 397 */
  UTIMER_IRQHandler30,                      /*   398 Interrupt 398 */
  UTIMER_IRQHandler31,                      /*   399 Interrupt 399 */
  UTIMER_IRQHandler32,                      /*   400 Interrupt 400 */
  UTIMER_IRQHandler33,                      /*   401 Interrupt 401 */
  UTIMER_IRQHandler34,                      /*   402 Interrupt 402 */
  UTIMER_IRQHandler35,                      /*   403 Interrupt 403 */
  UTIMER_IRQHandler36,                      /*   404 Interrupt 404 */
  UTIMER_IRQHandler37,                      /*   405 Interrupt 405 */
  UTIMER_IRQHandler38,                      /*   406 Interrupt 406 */
  UTIMER_IRQHandler39,                      /*   407 Interrupt 407 */
  UTIMER_IRQHandler40,                      /*   408 Interrupt 408 */
  UTIMER_IRQHandler41,                      /*   409 Interrupt 409 */
  UTIMER_IRQHandler42,                      /*   410 Interrupt 410 */
  UTIMER_IRQHandler43,                      /*   411 Interrupt 411 */
  UTIMER_IRQHandler44,                      /*   412 Interrupt 412 */
  UTIMER_IRQHandler45,                      /*   413 Interrupt 413 */
  UTIMER_IRQHandler46,                      /*   414 Interrupt 414 */
  UTIMER_IRQHandler47,                      /*   415 Interrupt 415 */
  UTIMER_IRQHandler48,                      /*   416 Interrupt 416 */
  UTIMER_IRQHandler49,                      /*   417 Interrupt 417 */
  UTIMER_IRQHandler50,                      /*   418 Interrupt 418 */
  UTIMER_IRQHandler51,                      /*   419 Interrupt 419 */
  UTIMER_IRQHandler52,                      /*   420 Interrupt 420 */
  UTIMER_IRQHandler53,                      /*   421 Interrupt 421 */
  UTIMER_IRQHandler54,                      /*   422 Interrupt 422 */
  UTIMER_IRQHandler55,                      /*   423 Interrupt 423 */
  UTIMER_IRQHandler56,                      /*   424 Interrupt 424 */
  UTIMER_IRQHandler57,                      /*   425 Interrupt 425 */
  UTIMER_IRQHandler58,                      /*   426 Interrupt 426 */
  UTIMER_IRQHandler59,                      /*   427 Interrupt 427 */
  UTIMER_IRQHandler60,                      /*   428 Interrupt 428 */
  UTIMER_IRQHandler61,                      /*   429 Interrupt 429 */
  UTIMER_IRQHandler62,                      /*   430 Interrupt 430 */
  UTIMER_IRQHandler63,                      /*   431 Interrupt 431 */
  UTIMER_IRQHandler64,                      /*   432 Interrupt 432 */
  UTIMER_IRQHandler65,                      /*   433 Interrupt 433 */
  UTIMER_IRQHandler66,                      /*   434 Interrupt 434 */
  UTIMER_IRQHandler67,                      /*   435 Interrupt 435 */
  UTIMER_IRQHandler68,                      /*   436 Interrupt 436 */
  UTIMER_IRQHandler69,                      /*   437 Interrupt 437 */
  UTIMER_IRQHandler70,                      /*   438 Interrupt 438 */
  UTIMER_IRQHandler71,                      /*   439 Interrupt 439 */
  UTIMER_IRQHandler72,                      /*   440 Interrupt 440 */
  UTIMER_IRQHandler73,                      /*   441 Interrupt 441 */
  UTIMER_IRQHandler74,                      /*   442 Interrupt 442 */
  UTIMER_IRQHandler75,                      /*   443 Interrupt 443 */
  UTIMER_IRQHandler76,                      /*   444 Interrupt 444 */
  UTIMER_IRQHandler77,                      /*   445 Interrupt 445 */
  UTIMER_IRQHandler78,                      /*   446 Interrupt 446 */
  UTIMER_IRQHandler79,                      /*   447 Interrupt 447 */
  UTIMER_IRQHandler80,                      /*   448 Interrupt 448 */
  UTIMER_IRQHandler81,                      /*   449 Interrupt 449 */
  UTIMER_IRQHandler82,                      /*   450 Interrupt 450 */
  UTIMER_IRQHandler83,                      /*   451 Interrupt 451 */
  UTIMER_IRQHandler84,                      /*   452 Interrupt 452 */
  UTIMER_IRQHandler85,                      /*   453 Interrupt 453 */
  UTIMER_IRQHandler86,                      /*   454 Interrupt 454 */
  UTIMER_IRQHandler87,                      /*   455 Interrupt 455 */
  UTIMER_IRQHandler88,                      /*   456 Interrupt 456 */
  UTIMER_IRQHandler89,                      /*   457 Interrupt 457 */
  UTIMER_IRQHandler90,                      /*   458 Interrupt 458 */
  UTIMER_IRQHandler91,                      /*   459 Interrupt 459 */
  UTIMER_IRQHandler92,                      /*   460 Interrupt 460 */
  UTIMER_IRQHandler93,                      /*   461 Interrupt 461 */
  UTIMER_IRQHandler94,                      /*   462 Interrupt 462 */
  UTIMER_IRQHandler95                       /*   463 Interrupt 463 */
                                            /* Interrupts 464 .. 480 are left out */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
__NO_RETURN void Reset_Handler(void)
{
  /* Setup the main stack */
  __asm volatile ("MSR MSPLIM, %0" : : "r" (&__STACK_LIMIT));
  __asm volatile ("MSR MSP, %0" : : "r" (&__INITIAL_SP));

  SystemInit();                             /* CMSIS System Initialization */
  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}


#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*----------------------------------------------------------------------------
  Hard Fault Handler
 *----------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
  while(1);
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
  while(1);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#endif
