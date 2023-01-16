/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
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
 * @file     startup_M55_HP.c
 * @author   Rupesh Kumar
 * @email    rupesh@alifsemi.com
 * @brief    CMSIS Core Device Startup File for
 *           Alif Semiconductor M55_HP Device
 * @version  V1.0.1
 * @date     02. Dec 2022
 * @bug      None
 * @Note	 None
 ******************************************************************************/

#if defined (M55_HP)
  #include "M55_HP.h"
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
void HardFault_Handler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void MemManage_Handler      (void) __attribute__ ((weak, alias("Fault_Handler")));
void BusFault_Handler       (void) __attribute__ ((weak, alias("Fault_Handler")));
void UsageFault_Handler     (void) __attribute__ ((weak, alias("Fault_Handler")));
void SecureFault_Handler    (void) __attribute__ ((weak, alias("Fault_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

/* Interrupts */
void DMA1_IRQ0Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ1Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ2Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ3Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ4Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ5Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ6Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ7Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ8Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ9Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ10Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ11Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ12Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ13Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ14Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ15Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ16Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ17Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ18Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ19Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ20Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ21Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ22Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ23Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ24Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ25Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ26Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ27Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ28Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ29Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ30Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_IRQ31Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_AbortIRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));

void MHU_APSS_S_RX_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_APSS_S_TX_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_APSS_NS_RX_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_APSS_NS_TX_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));

void MHU_SESS_S_RX_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_SESS_S_TX_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_SESS_NS_RX_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_SESS_NS_TX_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));

void MHU_RTSS_S_RX_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_RTSS_S_TX_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_RTSS_NS_RX_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void MHU_RTSS_NS_TX_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));

void HP_PPU_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));

void NPU_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));

void FW_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SDC600_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void PPU_COMB_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));

void REFCLK_CNTBASE0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void REFCLK_CNTBASE1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void REFCLK_CNTBASE2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void REFCLK_CNTBASE3_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));

void S32K_CNTBASE0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void S32K_CNTBASE1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void SOC_ETR_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SOC_CATU_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));

void XNVM_OSPI0_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void XNVM_OSPI1_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));

void XNVM_AESDEC0_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void XNVM_AESDEC1_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));

void AON_CLKC_A_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));
void AON_CLKC_B_IRQHandler           (void) __attribute__ ((weak, alias("Default_Handler")));

void VBAT_BOD_IRQHandler             (void) __attribute__ ((weak, alias("Default_Handler")));

void USB0_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));

void SDMMC_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void SDMMC_WAKEUP_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));

void CANFD_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));

void HWSEM0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM3_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM4_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM5_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM6_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM7_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM8_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM9_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM10_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM11_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM12_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM13_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM14_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void HWSEM15_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));

void PPU0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void PPU1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void PPU2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void UART0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART4_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART5_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART6_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART7_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));

void I2C0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void I3C0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void SPI0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void I2S0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void PDM_IRQHandler0        (void) __attribute__ ((weak, alias("Default_Handler")));
void PDM_IRQHandler1        (void) __attribute__ ((weak, alias("Default_Handler")));
void PDM_IRQHandler2        (void) __attribute__ ((weak, alias("Default_Handler")));
void PDM_IRQHandler3        (void) __attribute__ ((weak, alias("Default_Handler")));
void PDM_IRQHandler4        (void) __attribute__ ((weak, alias("Default_Handler")));
void PDM_IRQHandler5        (void) __attribute__ ((weak, alias("Default_Handler")));
void PDM_IRQHandler6        (void) __attribute__ ((weak, alias("Default_Handler")));
void PDM_IRQHandler7        (void) __attribute__ ((weak, alias("Default_Handler")));

void ETH_SBD_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void ETH_PMT_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));
void ETH_LPI_IRQHandler     (void) __attribute__ ((weak, alias("Default_Handler")));

void ADC0_INTR_DONE_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_INTR_DONE2_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_INTR_CMP0_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_INTR_CMP1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_INTR_CMP2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_INTR_CMP3_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_INTR_CMP4_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC0_INTR_CMP5_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));

void ADC1_INTR_DONE_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_INTR_DONE2_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_INTR_CMP0_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_INTR_CMP1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_INTR_CMP2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_INTR_CMP3_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_INTR_CMP4_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_INTR_CMP5_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));

void ADC2_INTR_DONE_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC2_INTR_DONE2_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC2_INTR_CMP0_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC2_INTR_CMP1_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC2_INTR_CMP2_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC2_INTR_CMP3_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC2_INTR_CMP4_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC2_INTR_CMP5_IRQHandler  (void) __attribute__ ((weak, alias("Default_Handler")));

void CMP0_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void CMP1_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void CMP2_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));
void CMP3_IRQHandler            (void) __attribute__ ((weak, alias("Default_Handler")));

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
void GPIO3_PIN24_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN25_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN26_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN27_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN28_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN29_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN30_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void GPIO3_PIN31_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));

void DMA0_IRQ0Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ1Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ2Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ3Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ4Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ5Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ6Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ7Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ8Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ9Handler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ10Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ11Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ12Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ13Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ14Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ15Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ16Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ17Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ18Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ19Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ20Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ21Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ22Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ23Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ24Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ25Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ26Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ27Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ28Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ29Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ30Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_IRQ31Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA0_AbortIRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));

void DAVE2_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));

void CDC200_SCANLINE0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_SCANLINE1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_FIFO_WARNING0_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_FIFO_WARNING1_IRQHandler    (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_FIFO_UNDERRUN0_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_FIFO_UNDERRUN1_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_BUS_ERROR0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_BUS_ERROR1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_REG_RELOAD0_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void CDC200_REG_RELOAD1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));

void MIPI_DSI_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));
void MIPI_CSI2_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));

void CAMERA0_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));

void LPTIMER_CHANNEL0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIMER_CHANNEL1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIMER_CHANNEL2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void LPTIMER_CHANNEL3_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));

void RTC0_IRQHandler                    (void) __attribute__ ((weak, alias("Default_Handler")));

void QEC0_INTR_CMP_A_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void QEC0_INTR_CMP_B_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void QEC1_INTR_CMP_A_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void QEC1_INTR_CMP_B_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void QEC2_INTR_CMP_A_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void QEC2_INTR_CMP_B_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void QEC3_INTR_CMP_A_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void QEC3_INTR_CMP_B_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));

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
  DMA1_IRQ0Handler,                         /*   0 Interrupt 0 */
  DMA1_IRQ1Handler,                         /*   1 Interrupt 1 */
  DMA1_IRQ2Handler,                         /*   2 Interrupt 2 */
  DMA1_IRQ3Handler,                         /*   3 Interrupt 3 */
  DMA1_IRQ4Handler,                         /*   4 Interrupt 4 */
  DMA1_IRQ5Handler,                         /*   5 Interrupt 5 */
  DMA1_IRQ6Handler,                         /*   6 Interrupt 6 */
  DMA1_IRQ7Handler,                         /*   7 Interrupt 7 */
  DMA1_IRQ8Handler,                         /*   8 Interrupt 8 */
  DMA1_IRQ9Handler,                         /*   9 Interrupt 9 */
  DMA1_IRQ10Handler,                        /*   10 Interrupt 10 */
  DMA1_IRQ11Handler,                        /*   11 Interrupt 11 */
  DMA1_IRQ12Handler,                        /*   12 Interrupt 12 */
  DMA1_IRQ13Handler,                        /*   13 Interrupt 13 */
  DMA1_IRQ14Handler,                        /*   14 Interrupt 14 */
  DMA1_IRQ15Handler,                        /*   15 Interrupt 15 */
  DMA1_IRQ16Handler,                        /*   16 Interrupt 16 */
  DMA1_IRQ17Handler,                        /*   17 Interrupt 17 */
  DMA1_IRQ18Handler,                        /*   18 Interrupt 18 */
  DMA1_IRQ19Handler,                        /*   19 Interrupt 19 */
  DMA1_IRQ20Handler,                        /*   20 Interrupt 20 */
  DMA1_IRQ21Handler,                        /*   21 Interrupt 21 */
  DMA1_IRQ22Handler,                        /*   22 Interrupt 22 */
  DMA1_IRQ23Handler,                        /*   23 Interrupt 23 */
  DMA1_IRQ24Handler,                        /*   24 Interrupt 24 */
  DMA1_IRQ25Handler,                        /*   25 Interrupt 25 */
  DMA1_IRQ26Handler,                        /*   26 Interrupt 26 */
  DMA1_IRQ27Handler,                        /*   27 Interrupt 27 */
  DMA1_IRQ28Handler,                        /*   28 Interrupt 28 */
  DMA1_IRQ29Handler,                        /*   29 Interrupt 29 */
  DMA1_IRQ30Handler,                        /*   30 Interrupt 30 */
  DMA1_IRQ31Handler,                        /*   31 Interrupt 31 */
  DMA1_AbortIRQHandler,                     /*   32 Interrupt 32 */
  MHU_APSS_S_RX_IRQHandler,                 /*   33 Interrupt 33 */
  MHU_APSS_S_TX_IRQHandler,                 /*   34 Interrupt 34 */
  MHU_APSS_NS_RX_IRQHandler,                /*   35 Interrupt 35 */
  MHU_APSS_NS_TX_IRQHandler,                /*   36 Interrupt 36 */
  MHU_SESS_S_RX_IRQHandler,                 /*   37 Interrupt 37 */
  MHU_SESS_S_TX_IRQHandler,                 /*   38 Interrupt 38 */
  MHU_SESS_NS_RX_IRQHandler,                /*   39 Interrupt 39 */
  MHU_SESS_NS_TX_IRQHandler,                /*   40 Interrupt 40 */
  MHU_RTSS_S_RX_IRQHandler,                 /*   41 Interrupt 41 */
  MHU_RTSS_S_TX_IRQHandler,                 /*   42 Interrupt 42 */
  MHU_RTSS_NS_RX_IRQHandler,                /*   43 Interrupt 43 */
  MHU_RTSS_NS_TX_IRQHandler,                /*   44 Interrupt 44 */
  0,                                        /*   45 Interrupt 45 */
  0,                                        /*   46 Interrupt 46 */
  0,                                        /*   47 Interrupt 47 */
  HP_PPU_IRQHandler,                        /*   48 Interrupt 48 */
  0,                                        /*   49 Interrupt 49 */
  0,                                        /*   50 Interrupt 50 */
  0,                                        /*   51 Interrupt 51 */
  0,                                        /*   52 Interrupt 52 */
  0,                                        /*   53 Interrupt 53 */
  0,                                        /*   54 Interrupt 54 */
  NPU_IRQHandler,                           /*   55 Interrupt 55 */
  0,                                        /*   56 Interrupt 56 */
  0,                                        /*   57 Interrupt 57 */
  0,                                        /*   58 Interrupt 58 */
  0,                                        /*   59 Interrupt 59 */
  0,                                        /*   60 Interrupt 60 */
  0,                                        /*   61 Interrupt 61 */
  0,                                        /*   62 Interrupt 62 */
  0,                                        /*   63 Interrupt 63 */
  FW_IRQHandler,                            /*   64 Interrupt 64 */
  SDC600_IRQHandler,                        /*   65 Interrupt 65 */
  PPU_COMB_IRQHandler,                      /*   66 Interrupt 66 */
  REFCLK_CNTBASE0_IRQHandler,               /*   67 Interrupt 67 */
  REFCLK_CNTBASE1_IRQHandler,               /*   68 Interrupt 68 */
  REFCLK_CNTBASE2_IRQHandler,               /*   69 Interrupt 69 */
  REFCLK_CNTBASE3_IRQHandler,               /*   70 Interrupt 70 */
  S32K_CNTBASE0_IRQHandler,                 /*   71 Interrupt 71 */
  S32K_CNTBASE1_IRQHandler,                 /*   72 Interrupt 72 */
  SOC_ETR_IRQHandler,                       /*   73 Interrupt 73 */
  SOC_CATU_IRQHandler,                      /*   74 Interrupt 74 */
  0,                                        /*   75 Reserved Interrupt 75 */
  0,                                        /*   76 Reserved Interrupt 76 */
  0,                                        /*   77 Reserved Interrupt 77 */
  0,                                        /*   78 Reserved Interrupt 78 */
  0,                                        /*   79 Reserved Interrupt 79 */
  0,                                        /*   80 Reserved Interrupt 80 */
  0,                                        /*   81 Reserved Interrupt 81 */
  0,                                        /*   82 Reserved Interrupt 82 */
  0,                                        /*   83 Reserved Interrupt 83 */
  0,                                        /*   84 Reserved Interrupt 84 */
  0,                                        /*   85 Reserved Interrupt 85 */
  0,                                        /*   86 Reserved Interrupt 86 */
  0,                                        /*   87 Reserved Interrupt 87 */
  0,                                        /*   88 Reserved Interrupt 88 */
  0,                                        /*   89 Reserved Interrupt 89 */
  0,                                        /*   90 Reserved Interrupt 90 */
  0,                                        /*   91 Reserved Interrupt 91 */
  0,                                        /*   92 Reserved Interrupt 92 */
  0,                                        /*   93 Reserved Interrupt 93 */
  0,                                        /*   94 Reserved Interrupt 94 */
  0,                                        /*   95 Reserved Interrupt 95 */
  XNVM_OSPI0_IRQHandler,                    /*   96 Interrupt 96 */
  XNVM_OSPI1_IRQHandler,                    /*   97 Interrupt 97 */
  XNVM_AESDEC0_IRQHandler,                  /*   98 Interrupt 98 */
  XNVM_AESDEC1_IRQHandler,                  /*   99 Interrupt 99 */
  AON_CLKC_A_IRQHandler,                    /*   100 Interrupt 100 */
  AON_CLKC_B_IRQHandler,                    /*   101 Interrupt 101 */
  VBAT_BOD_IRQHandler,                      /*   102 Interrupt 102 */
  USB0_IRQHandler,                          /*   103 Interrupt 103 */
  SDMMC_IRQHandler,                         /*   104 Interrupt 104 */
  SDMMC_WAKEUP_IRQHandler,                  /*   105 Interrupt 105 */
  CANFD_IRQHandler,                         /*   106 Interrupt 106 */
  HWSEM0_IRQHandler,                        /*   107 Interrupt 107 */
  HWSEM1_IRQHandler,                        /*   108 Interrupt 108 */
  HWSEM2_IRQHandler,                        /*   109 Interrupt 109 */
  HWSEM3_IRQHandler,                        /*   110 Interrupt 110 */
  HWSEM4_IRQHandler,                        /*   111 Interrupt 111 */
  HWSEM5_IRQHandler,                        /*   112 Interrupt 112 */
  HWSEM6_IRQHandler,                        /*   113 Interrupt 113 */
  HWSEM7_IRQHandler,                        /*   114 Interrupt 114 */
  HWSEM8_IRQHandler,                        /*   115 Interrupt 115 */
  HWSEM9_IRQHandler,                        /*   116 Interrupt 116 */
  HWSEM10_IRQHandler,                       /*   117 Interrupt 117 */
  HWSEM11_IRQHandler,                       /*   118 Interrupt 118 */
  HWSEM12_IRQHandler,                       /*   119 Interrupt 119 */
  HWSEM13_IRQHandler,                       /*   120 Interrupt 120 */
  HWSEM14_IRQHandler,                       /*   121 Interrupt 121 */
  HWSEM15_IRQHandler,                       /*   122 Interrupt 122 */
  PPU0_IRQHandler,                          /*   123 Interrupt 123 */
  PPU1_IRQHandler,                          /*   124 Interrupt 124 */
  PPU2_IRQHandler,                          /*   125 Interrupt 125 */
  UART0_IRQHandler,                         /*   126 Interrupt 126 */
  UART1_IRQHandler,                         /*   127 Interrupt 127 */
  UART2_IRQHandler,                         /*   128 Interrupt 128 */
  UART3_IRQHandler,                         /*   129 Interrupt 129 */
  UART4_IRQHandler,                         /*   130 Interrupt 130 */
  UART5_IRQHandler,                         /*   131 Interrupt 131 */
  UART6_IRQHandler,                         /*   132 Interrupt 132 */
  UART7_IRQHandler,                         /*   133 Interrupt 133 */
  I2C0_IRQHandler,                          /*   134 Interrupt 134 */
  I2C1_IRQHandler,                          /*   135 Interrupt 135 */
  I2C2_IRQHandler,                          /*   136 Interrupt 136 */
  I2C3_IRQHandler,                          /*   137 Interrupt 137 */
  I3C0_IRQHandler,                          /*   138 Interrupt 138 */
  SPI0_IRQHandler,                          /*   139 Interrupt 143 */
  SPI1_IRQHandler,                          /*   140 Interrupt 144 */
  SPI2_IRQHandler,                          /*   141 Interrupt 145 */
  SPI3_IRQHandler,                          /*   142 Interrupt 146 */
  I2S0_IRQHandler,                          /*   143 Interrupt 143 */
  I2S1_IRQHandler,                          /*   144 Interrupt 144 */
  I2S2_IRQHandler,                          /*   145 Interrupt 145 */
  I2S3_IRQHandler,                          /*   146 Interrupt 146 */
  PDM_IRQHandler0,                          /*   147  Interrupt 147 */
  PDM_IRQHandler1,                          /*   148  Interrupt 148 */
  PDM_IRQHandler2,                          /*   149  Interrupt 148 */
  PDM_IRQHandler3,                          /*   150  Interrupt 150 */
  PDM_IRQHandler4,                          /*   151  Interrupt 151 */
  PDM_IRQHandler5,                          /*   152  Interrupt 152 */
  PDM_IRQHandler6,                          /*   153  Interrupt 153 */
  PDM_IRQHandler7,                          /*   154  Interrupt 154 */
  ETH_SBD_IRQHandler,                       /*   155  Interrupt 155 */
  ETH_PMT_IRQHandler,                       /*   156  Interrupt 156 */
  ETH_LPI_IRQHandler,                       /*   157  Interrupt 157 */
  ADC0_INTR_DONE_IRQHandler,                /*   158  Interrupt 158 */
  ADC0_INTR_DONE2_IRQHandler,               /*   159  Interrupt 159 */
  ADC0_INTR_CMP0_IRQHandler,                /*   160  Interrupt 160 */
  ADC0_INTR_CMP1_IRQHandler,                /*   161  Interrupt 161 */
  ADC0_INTR_CMP2_IRQHandler,                /*   162  Interrupt 162 */
  ADC0_INTR_CMP3_IRQHandler,                /*   163  Interrupt 163 */
  ADC0_INTR_CMP4_IRQHandler,                /*   164  Interrupt 164 */
  ADC0_INTR_CMP5_IRQHandler,                /*   165  Interrupt 165 */
  ADC1_INTR_DONE_IRQHandler,                /*   166  Interrupt 166 */
  ADC1_INTR_DONE2_IRQHandler,               /*   167  Interrupt 167 */
  ADC1_INTR_CMP0_IRQHandler,                /*   168  Interrupt 168 */
  ADC1_INTR_CMP1_IRQHandler,                /*   169  Interrupt 169 */
  ADC1_INTR_CMP2_IRQHandler,                /*   170  Interrupt 170 */
  ADC1_INTR_CMP3_IRQHandler,                /*   171  Interrupt 171 */
  ADC1_INTR_CMP4_IRQHandler,                /*   172  Interrupt 172 */
  ADC1_INTR_CMP5_IRQHandler,                /*   173  Interrupt 173 */
  ADC2_INTR_DONE_IRQHandler,                /*   174  Interrupt 174 */
  ADC2_INTR_DONE2_IRQHandler,               /*   175  Interrupt 175 */
  ADC2_INTR_CMP0_IRQHandler,                /*   176  Interrupt 176 */
  ADC2_INTR_CMP1_IRQHandler,                /*   177  Interrupt 177 */
  ADC2_INTR_CMP2_IRQHandler,                /*   178  Interrupt 178 */
  ADC2_INTR_CMP3_IRQHandler,                /*   179  Interrupt 179 */
  ADC2_INTR_CMP4_IRQHandler,                /*   180  Interrupt 180 */
  ADC2_INTR_CMP5_IRQHandler,                /*   181  Interrupt 181 */
  CMP0_IRQHandler,                          /*   182  Interrupt 182 */
  CMP1_IRQHandler,                          /*   183  Interrupt 183 */
  CMP2_IRQHandler,                          /*   184  Interrupt 184 */
  CMP3_IRQHandler,                          /*   185  Interrupt 185 */
  GPIO4_PIN0_IRQHandler,                    /*   186  Interrupt 186 */
  GPIO4_PIN1_IRQHandler,                    /*   187  Interrupt 187 */
  GPIO4_PIN2_IRQHandler,                    /*   188  Interrupt 188 */
  GPIO4_PIN3_IRQHandler,                    /*   189  Interrupt 189 */
  GPIO4_PIN4_IRQHandler,                    /*   190  Interrupt 190 */
  GPIO4_PIN5_IRQHandler,                    /*   191  Interrupt 191 */
  GPIO4_PIN6_IRQHandler,                    /*   192  Interrupt 192 */
  GPIO4_PIN7_IRQHandler,                    /*   193  Interrupt 193 */
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
  GPIO3_PIN24_IRQHandler,                   /*   282  Interrupt 282 */
  GPIO3_PIN25_IRQHandler,                   /*   283  Interrupt 283 */
  GPIO3_PIN26_IRQHandler,                   /*   284  Interrupt 284 */
  GPIO3_PIN27_IRQHandler,                   /*   285  Interrupt 285 */
  GPIO3_PIN28_IRQHandler,                   /*   286  Interrupt 286 */
  GPIO3_PIN29_IRQHandler,                   /*   287  Interrupt 287 */
  GPIO3_PIN30_IRQHandler,                   /*   288  Interrupt 288 */
  GPIO3_PIN31_IRQHandler,                   /*   289  Interrupt 289 */
  DMA0_IRQ0Handler,                         /*   290 Interrupt 290 */
  DMA0_IRQ1Handler,                         /*   291 Interrupt 291 */
  DMA0_IRQ2Handler,                         /*   292 Interrupt 292 */
  DMA0_IRQ3Handler,                         /*   293 Interrupt 293 */
  DMA0_IRQ4Handler,                         /*   294 Interrupt 294 */
  DMA0_IRQ5Handler,                         /*   295 Interrupt 295 */
  DMA0_IRQ6Handler,                         /*   296 Interrupt 296 */
  DMA0_IRQ7Handler,                         /*   297 Interrupt 297 */
  DMA0_IRQ8Handler,                         /*   298 Interrupt 298 */
  DMA0_IRQ9Handler,                         /*   299 Interrupt 299 */
  DMA0_IRQ10Handler,                        /*   300 Interrupt 300 */
  DMA0_IRQ11Handler,                        /*   301 Interrupt 301 */
  DMA0_IRQ12Handler,                        /*   302 Interrupt 302 */
  DMA0_IRQ13Handler,                        /*   303 Interrupt 303 */
  DMA0_IRQ14Handler,                        /*   304 Interrupt 304 */
  DMA0_IRQ15Handler,                        /*   305 Interrupt 305 */
  DMA0_IRQ16Handler,                        /*   306 Interrupt 306 */
  DMA0_IRQ17Handler,                        /*   307 Interrupt 307 */
  DMA0_IRQ18Handler,                        /*   308 Interrupt 308 */
  DMA0_IRQ19Handler,                        /*   309 Interrupt 309 */
  DMA0_IRQ20Handler,                        /*   310 Interrupt 310 */
  DMA0_IRQ21Handler,                        /*   311 Interrupt 311 */
  DMA0_IRQ22Handler,                        /*   312 Interrupt 312 */
  DMA0_IRQ23Handler,                        /*   313 Interrupt 313 */
  DMA0_IRQ24Handler,                        /*   314 Interrupt 314 */
  DMA0_IRQ25Handler,                        /*   315 Interrupt 315 */
  DMA0_IRQ26Handler,                        /*   316 Interrupt 316 */
  DMA0_IRQ27Handler,                        /*   317 Interrupt 317 */
  DMA0_IRQ28Handler,                        /*   318 Interrupt 318 */
  DMA0_IRQ29Handler,                        /*   319 Interrupt 319 */
  DMA0_IRQ30Handler,                        /*   320 Interrupt 320 */
  DMA0_IRQ31Handler,                        /*   321 Interrupt 321 */
  DMA0_AbortIRQHandler,                     /*   322 Interrupt 322 */
  DAVE2_IRQHandler,                         /*   323  Interrupt 323 */
  CDC200_SCANLINE0_IRQHandler,              /*   324  Interrupt 324 */
  CDC200_SCANLINE1_IRQHandler,              /*   325  Interrupt 325 */
  CDC200_FIFO_WARNING0_IRQHandler,          /*   326  Interrupt 326 */
  CDC200_FIFO_WARNING1_IRQHandler,          /*   327  Interrupt 327 */
  CDC200_FIFO_UNDERRUN0_IRQHandler,         /*   328  Interrupt 328 */
  CDC200_FIFO_UNDERRUN1_IRQHandler,         /*   329  Interrupt 329 */
  CDC200_BUS_ERROR0_IRQHandler,             /*   330  Interrupt 330 */
  CDC200_BUS_ERROR1_IRQHandler,             /*   331  Interrupt 331 */
  CDC200_REG_RELOAD0_IRQHandler,            /*   332  Interrupt 332 */
  CDC200_REG_RELOAD1_IRQHandler,            /*   333  Interrupt 333 */
  MIPI_DSI_IRQHandler,                      /*   334  Interrupt 334 */
  MIPI_CSI2_IRQHandler,                     /*   335  Interrupt 335 */
  CAMERA0_IRQHandler,                       /*   336  Interrupt 336 */
  LPTIMER_CHANNEL0_IRQHandler,              /*   337  Interrupt 337 */
  LPTIMER_CHANNEL1_IRQHandler,              /*   338  Interrupt 338 */
  LPTIMER_CHANNEL2_IRQHandler,              /*   339  Interrupt 339 */
  LPTIMER_CHANNEL3_IRQHandler,              /*   340  Interrupt 340 */
  RTC0_IRQHandler,                          /*   341  Interrupt 341 */
  0,			                    /*   342  Interrupt 342 */
  0,                			    /*   343  Interrupt 343 */
  0,                      		    /*   344  Interrupt 344 */
  0,                 			    /*   345  Interrupt 345 */
  0,                  			    /*   346  Interrupt 346 */
  0,                      		    /*   347  Interrupt 347 */
  0,                      		    /*   348  Interrupt 348 */
  0,                      		    /*   349  Interrupt 349 */
  0,                      		    /*   350  Interrupt 350 */
  0,                      		    /*   351  Interrupt 351 */
  0,                     		    /*   352  Interrupt 352 */
  0,                     		    /*   353  Interrupt 353 */
  0,                                        /*   354  Interrupt 354 */
  0,                                        /*   355  Interrupt 355 */
  0,                                        /*   356  Interrupt 356 */
  0,                                        /*   357  Interrupt 357 */
  0,                                        /*   358  Interrupt 358 */
  0,                                        /*   359  Interrupt 359 */
  QEC0_INTR_CMP_A_IRQHandler,               /*   360  Interrupt 360 */
  QEC0_INTR_CMP_B_IRQHandler,               /*   361  Interrupt 361 */
  QEC1_INTR_CMP_A_IRQHandler,               /*   362  Interrupt 362 */
  QEC1_INTR_CMP_B_IRQHandler,               /*   363  Interrupt 363 */
  QEC2_INTR_CMP_A_IRQHandler,               /*   364  Interrupt 364 */
  QEC2_INTR_CMP_B_IRQHandler,               /*   365  Interrupt 365 */
  QEC3_INTR_CMP_A_IRQHandler,               /*   366  Interrupt 366 */
  QEC3_INTR_CMP_B_IRQHandler,               /*   367  Interrupt 367 */
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
__attribute__((naked))
__NO_RETURN void Reset_Handler(void)
{
  /* Set up the main stack */

  /*
   * Function must be naked to ensure the compiler doesn't use the
   * stack on entry.
   *
   * Only basic asm (no parameters) is permitted for naked functions,
   * so we have to get the values in by text substitution.
   */
#define xstr(s) str(s)
#define str(s) #s
  __asm (
    "LDR     R0, =" xstr(__STACK_LIMIT) "\n\t"
    "LDR     R1, =" xstr(__INITIAL_SP) "\n\t"
    "MSR     MSPLIM, R0\n\t"
    "MSR     MSP, R1\n\t"
    "BL      Reset_Handler_C"
  );
#undef xstr
#undef str
}

__attribute__((used))
__NO_RETURN void Reset_Handler_C(void)
{
  SystemInit();                             /* CMSIS System Initialization */
  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}

/* This hook is called automatically by the ARM C library after scatter loading */
/* We add it to the preinit table for GCC */
void _platform_pre_stackheap_init(void)
{
    /* Synchronise the caches for any copied code */
    SCB_CleanDCache();
    SCB_InvalidateICache();
}

#if !defined(__ARMCC_VERSION)
void (*_do_platform_pre_stackheap_init)() __attribute__((section(".preinit_array"))) = _platform_pre_stackheap_init;
#endif

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wmissing-noreturn"
#endif

/*----------------------------------------------------------------------------
  Default Handler for Faults
 *----------------------------------------------------------------------------*/
void Fault_Handler(void)
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
