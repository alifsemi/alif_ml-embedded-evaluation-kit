/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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

#ifndef PERIPHERAL_MEMMAP_H
#define PERIPHERAL_MEMMAP_H

#define DESIGN_NAME              "Arm Corstone-300 - AN552"

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

#define CMSDK_GPIO0_BASE         (0x41100000)       /* User GPIO 0 Base Address   */
#define CMSDK_GPIO1_BASE         (0x41101000)       /* User GPIO 1 Base Address   */
#define CMSDK_GPIO2_BASE         (0x41102000)       /* User GPIO 2 Base Address   */
#define CMSDK_GPIO3_BASE         (0x41103000)       /* User GPIO 3 Base Address   */

#define FMC_CMDSK_GPIO_BASE0        (0x41104000)       /* FMC_CMDSK_GPIO_BASE 0 Base Address (4KB) */
#define FMC_CMDSK_GPIO_BASE1        (0x41105000)       /* FMC_CMDSK_GPIO_BASE 1 Base Address (4KB)*/
#define FMC_CMDSK_GPIO_BASE2        (0x41106000)       /* FMC_CMDSK_GPIO_BASE 2 Base Address (4KB)*/
#define FMC_USER_AHB_BASE           (0x41107000)          /* FMC_USER_AHB_BASE     Base Address (4KB)*/

#define DMA0_BASE                (0x41200000)       /* DMA0 (4KB) */
#define DMA1_BASE                (0x41201000)       /* DMA1 (4KB) */
#define DMA2_BASE                (0x41202000)       /* DMA2 (4KB) */
#define DMA3_BASE                (0x41203000)       /* DMA3 (4KB) */

#define USER_APB0_BASE           (0x41700000)       /* User APB0 */
#define USER_APB1_BASE           (0x41701000)       /* User APB1 */
#define USER_APB2_BASE           (0x41702000)       /* User APB2 */
#define USER_APB3_BASE           (0x41703000)       /* User APB3 */

#define MPS3_I2C0_BASE           (0x49200000)       /* Touch Screen I2C Base Address */
#define MPS3_I2C1_BASE           (0x49201000)       /* Audio Interface I2C Base Address */
#define MPS3_SSP2_BASE           (0x49202000)       /* ADC SPI PL022 Base Address   */
#define MPS3_SSP3_BASE           (0x49203000)       /* Shield 0 SPI PL022 Base Address   */

#define MPS3_SSP4_BASE           (0x49204000)       /* Shield 1 SPI PL022 Base Address   */
#define MPS3_I2C2_BASE           (0x49205000)       /* Shield 0 SBCon Base Address */
#define MPS3_I2C3_BASE           (0x49206000)       /* Shield 1 SBCon Base Address */

#define USER_APB_BASE            (0x49207000)       /* User APB Base Address */
/* #undef MPS3_I2C4_BASE */
#define MPS3_I2C5_BASE           (0x49208000)       /* DDR EPROM I2C SBCon Base Address */
#define MPS3_SCC_BASE            (0x49300000)       /* SCC Base Address    */
#define MPS3_AAIC_I2S_BASE       (0x49301000)       /* Audio Interface I2S Base Address */
#define MPS3_FPGAIO_BASE         (0x49302000)       /* FPGA IO Base Address */
/* #undef PL011_UART0_BASE */
#define CMSDK_UART0_BASE         (0x49303000)       /* UART 0 Base Address */
#define CMSDK_UART1_BASE         (0x49304000)       /* UART 1 Base Address */
#define CMSDK_UART2_BASE         (0x49305000)       /* UART 2 Base Address */
#define CMSDK_UART3_BASE         (0x49306000)       /* UART 3 Base Address Shield 0*/

#define ETHOS_U_NPU_BASE           (0x48102000)    /* Ethos-U NPU base address*/
#define ETHOS_U_NPU_TA0_BASE       (0x48103000)    /* Ethos-U NPU's timing adapter 0 base address */
#define ETHOS_U_NPU_TA1_BASE       (0x48103200)    /* Ethos-U NPU's timing adapter 1 base address */

#define CMSDK_UART4_BASE         (0x49307000)       /* UART 4 Base Address Shield 1*/
#define CMSDK_UART5_BASE         (0x49308000)       /* UART 5 Base Address */
/* #undef HDMI_AUDIO_BASE */
#define CLCD_CONFIG_BASE         (0x4930A000)       /* CLCD CONFIG Base Address */
#define RTC_BASE                 (0x4930B000)       /* RTC Base address */
#define SMSC9220_BASE            (0x41400000)       /* Ethernet SMSC9220 Base Address */
#define USB_BASE                 (0x41500000)       /* USB Base Address */
/* #undef CMSDK_SDIO_BASE */
/* #undef MPS3_CLCD_BASE */
/* #undef MPS3_eMMC_BASE */
/* #undef USER_BASE */

#define QSPI_XIP_BASE            (0x41800000)       /* QSPI XIP config Base Address */
#define QSPI_WRITE_BASE          (0x41801000)       /* QSPI write config Base Address */

/******************************************************************************/
/*                      Secure Peripheral memory map                          */
/******************************************************************************/

#define MPC_ISRAM0_BASE_S        (0x50083000)       /* ISRAM0 Memory Protection Controller Secure base address */
#define MPC_ISRAM1_BASE_S        (0x50084000)       /* ISRAM1 Memory Protection Controller Secure base address */

#define SEC_CMSDK_GPIO0_BASE     (0x51100000)       /* User GPIO 0 Base Address   */
#define SEC_CMSDK_GPIO1_BASE     (0x51101000)       /* User GPIO 0 Base Address   */
#define SEC_CMSDK_GPIO2_BASE     (0x51102000)       /* User GPIO 0 Base Address   */
#define SEC_CMSDK_GPIO3_BASE     (0x51103000)       /* User GPIO 0 Base Address   */

#define SEC_AHB_USER0_BASE       (0x51104000)       /* AHB USER 0 Base Address (4KB) */
#define SEC_AHB_USER1_BASE       (0x51105000)       /* AHB USER 1 Base Address (4KB)*/
#define SEC_AHB_USER2_BASE       (0x51106000)       /* AHB USER 2 Base Address (4KB)*/
#define SEC_AHB_USER3_BASE       (0x51107000)       /* AHB USER 3 Base Address (4KB)*/

#define SEC_DMA0_BASE            (0x51200000)       /* DMA0 (4KB) */
#define SEC_DMA1_BASE            (0x51201000)       /* DMA1 (4KB) */
#define SEC_DMA2_BASE            (0x51202000)       /* DMA2 (4KB) */
#define SEC_DMA3_BASE            (0x51203000)       /* DMA3 (4KB) */

#define SEC_USER_APB0_BASE       (0x51700000)       /* User APB0 */
#define SEC_USER_APB1_BASE       (0x51701000)       /* User APB1 */
#define SEC_USER_APB2_BASE       (0x51702000)       /* User APB2 */
#define SEC_USER_APB3_BASE       (0x51703000)       /* User APB3 */

#define SEC_MPS3_I2C0_BASE       (0x59200000)       /* Touch Screen I2C Base Address */
#define SEC_MPS3_I2C1_BASE       (0x59201000)       /* Audio Interface I2C Base Address */
#define SEC_MPS3_SSP2_BASE       (0x59202000)       /* ADC SPI PL022 Base Address   */
#define SEC_MPS3_SSP3_BASE       (0x59203000)       /* Shield 0 SPI PL022 Base Address   */

#define SEC_MPS3_SSP4_BASE       (0x59204000)       /* Shield 1 SPI PL022 Base Address   */
#define SEC_MPS3_I2C2_BASE       (0x59205000)       /* Shield 0 SBCon Base Address */
#define SEC_MPS3_I2C3_BASE       (0x59206000)       /* Shield 1 SBCon Base Address */

/* #undef SEC_MPS3_I2C4_BASE */
#define SEC_MPS3_I2C5_BASE       (0x59208000)       /* DDR EPROM I2C SBCon Base Address */
#define SEC_MPS3_SCC_BASE        (0x59300000)       /* SCC Base Address    */
#define SEC_MPS3_AAIC_I2S_BASE   (0x59301000)       /* Audio Interface I2S Base Address */
#define SEC_MPS3_FPGAIO_BASE     (0x59302000)       /* FPGA IO Base Address */
#define SEC_CMSDK_UART0_BASE     (0x59303000)       /* UART 0 Base Address */
#define SEC_CMSDK_UART1_BASE     (0x59304000)       /* UART 1 Base Address */
#define SEC_CMSDK_UART2_BASE     (0x59305000)       /* UART 2 Base Address */
#define SEC_CMSDK_UART3_BASE     (0x59306000)       /* UART 3 Base Address Shield 0*/

#define SEC_CMSDK_UART4_BASE     (0x59307000)       /* UART 4 Base Address Shield 1*/
#define SEC_CMSDK_UART5_BASE     (0x59308000)       /* UART 5 Base Address */
/* #undef SEC_HDMI_AUDIO_BASE */
#define SEC_CLCD_CONFIG_BASE     (0x5930A000)       /* CLCD CONFIG Base Address */
#define SEC_RTC_BASE             (0x5930B000)       /* RTC Base address */
#define SEC_SMSC9220_BASE        (0x51400000)       /* Ethernet SMSC9220 Base Address */
#define SEC_USB_BASE             (0x51500000)       /* USB Base Address */

#define SEC_ETHOS_U_NPU_BASE       (0x58102000)   /* Ethos-U NPU base address*/
#define SEC_ETHOS_U_NPU_TA0_BASE   (0x58103000)   /* Ethos-U NPU's timing adapter 0 base address */
#define SEC_ETHOS_U_NPU_TA1_BASE   (0x58103200)   /* Ethos-U NPU's timing adapter 1 base address */

/* #undef SEC_USER_BASE */

#define SEC_QSPI_XIP_BASE        (0x51800000)       /* QSPI XIP config Base Address */
#define SEC_QSPI_WRITE_BASE      (0x51801000)       /* QSPI write config Base Address */

/******************************************************************************/
/*                                  MPCs                                      */
/******************************************************************************/

#define MPC_ISRAM0_BASE_S        (0x50083000)       /* Internal SRAM 0 MPC */
#define MPC_ISRAM1_BASE_S        (0x50084000)       /* Internal SRAM 1 MPC */
#define MPC_BRAM_BASE_S          (0x57000000)       /* SRAM Memory Protection Controller Secure base address */
#define MPC_QSPI_BASE_S          (0x57001000)       /* QSPI Memory Protection Controller Secure base address */
#define MPC_DDR4_BASE_S          (0x57002000)       /* DDR4 Memory Protection Controller Secure base address */

#endif /* PERIPHERAL_MEMMAP_H */
