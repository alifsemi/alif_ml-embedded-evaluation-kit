/*
 * SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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

#include "mem_regions.h"

#define DESIGN_NAME                     "Arm Corstone-320"

/** Non-Secure subsystem peripheral region */
#define DMA_350_BASE_NS                  0x40002000 /* DMA350 register block Non-Secure base address */
#define NPU0_APB_BASE_NS                 0x40004000 /* NPU0 APB Non-Secure base address */
#define CPU0_PWRCTRL_BASE_NS             0x40012000 /* CPU 0 Power Control Block Non-Secure base address */
#define CPU0_IDENTITY_BASE_NS            0x4001F000 /* CPU 0 Identity Block Non-Secure base address */
#define CORSTONE320_NSACFG_BASE_NS       0x40080000 /* Corstone-320 Non-Secure Access Configuration Register Block Non-Secure base address */

/** Non-Secure MSTEXPPILL peripheral region */
#define GPIO0_CMSDK_BASE_NS              0x40100000 /* GPIO 0 Non-Secure base address */
#define GPIO1_CMSDK_BASE_NS              0x40101000 /* GPIO 1 Non-Secure base address */
#define GPIO2_CMSDK_BASE_NS              0x40102000 /* GPIO 2 Non-Secure base address */
#define GPIO3_CMSDK_BASE_NS              0x40103000 /* GPIO 3 Non-Secure base address */
#define AHB_USER_0_BASE_NS               0x40104000 /* AHB USER 0 Non-Secure base address */
#define AHB_USER_1_BASE_NS               0x40105000 /* AHB USER 1 Non-Secure base address */
#define AHB_USER_2_BASE_NS               0x40106000 /* AHB USER 2 Non-Secure base address */
#define AHB_USER_3_BASE_NS               0x40107000 /* AHB USER 3 Non-Secure base address */
#define HDLCD_BASE_NS                    0x40310000 /* HDLCD Non-Secure base address */
#define ETHERNET_BASE_NS                 0x40400000 /* Ethernet Non-Secure base address */
#define USB_BASE_NS                      0x40500000 /* USB Non-Secure base address */
#define USER_APB0_BASE_NS                0x40700000 /* User APB 0 Non-Secure base address */
#define USER_APB1_BASE_NS                0x40701000 /* User APB 1 Non-Secure base address */
#define USER_APB2_BASE_NS                0x40702000 /* User APB 2 Non-Secure base address */
#define USER_APB3_BASE_NS                0x40703000 /* User APB 3 Non-Secure base address */
#define QSPI_CONFIG_BASE_NS              0x40800000 /* QSPI Config Non-Secure base address */
#define QSPI_WRITE_BASE_NS               0x40801000 /* QSPI Write Non-Secure base address */

/** Non-Secure subsystem peripheral region */
#define SYSTIMER0_ARMV8_M_BASE_NS        0x48000000 /* System Timer 0 Non-Secure base address */
#define SYSTIMER1_ARMV8_M_BASE_NS        0x48001000 /* System Timer 1 Non-Secure base address */
#define SYSTIMER2_ARMV8_M_BASE_NS        0x48002000 /* System Timer 2 Non-Secure base address */
#define SYSTIMER3_ARMV8_M_BASE_NS        0x48003000 /* System Timer 3 Non-Secure base address */
#define CORSTONE320_SYSINFO_BASE_NS      0x48020000 /* Corstone-320 System info Block Non-Secure base address */
#define SLOWCLK_TIMER_CMSDK_BASE_NS      0x4802F000 /* CMSDK based SLOWCLK Timer Non-Secure base address */
#define SYSWDOG_ARMV8_M_CNTRL_BASE_NS    0x48040000 /* Non-Secure Watchdog Timer control frame Non-Secure base address */
#define SYSWDOG_ARMV8_M_REFRESH_BASE_NS  0x48041000 /* Non-Secure Watchdog Timer refresh frame Non-Secure base address */
#define SYSCNTR_READ_BASE_NS             0x48101000 /* System Counter Read Secure base address */

/** Non-Secure MSTEXPPIHL peripheral region */
#define FPGA_SBCon_I2C_TOUCH_BASE_NS     0x48100000 /* FPGA - SBCon I2C (Touch) Non-Secure base address */
#define FPGA_SBCon_I2C_AUDIO_BASE_NS     0x48101000 /* FPGA - SBCon I2C (Audio Conf) Non-Secure base address */
#define FPGA_SPI_ADC_BASE_NS             0x48102000 /* FPGA - PL022 (SPI ADC) Non-Secure base address */
#define FPGA_SPI_SHIELD0_BASE_NS         0x48103000 /* FPGA - PL022 (SPI Shield0) Non-Secure base address */
#define FPGA_SPI_SHIELD1_BASE_NS         0x48104000 /* FPGA - PL022 (SPI Shield1) Non-Secure base address */
#define SBCon_I2C_SHIELD0_BASE_NS        0x48105000 /* SBCon (I2C - Shield0) Non-Secure base address */
#define SBCon_I2C_SHIELD1_BASE_NS        0x48106000 /* SBCon (I2C – Shield1) Non-Secure base address */
#define USER_APB_BASE_NS                 0x48107000 /* USER APB Non-Secure base address */
#define FPGA_DDR4_EEPROM_BASE_NS         0x48108000 /* FPGA - SBCon I2C (DDR4 EEPROM) Non-Secure base address */
#define FPGA_SCC_BASE_NS                 0x48200000 /* FPGA - SCC registers Non-Secure base address */
#define FPGA_I2S_BASE_NS                 0x48201000 /* FPGA - I2S (Audio) Non-Secure base address */
#define FPGA_IO_BASE_NS                  0x48202000 /* FPGA - IO (System Ctrl + I/O) Non-Secure base address */
#define UART0_BASE_NS                    0x48203000 /* UART 0 Non-Secure base address */
#define UART1_BASE_NS                    0x48204000 /* UART 1 Non-Secure base address */
#define UART2_BASE_NS                    0x48205000 /* UART 2 Non-Secure base address */
#define UART3_BASE_NS                    0x48206000 /* UART 3 Non-Secure base address */
#define UART4_BASE_NS                    0x48207000 /* UART 4 Non-Secure base address */
#define UART5_BASE_NS                    0x48208000 /* UART 5 Non-Secure base address */
#define RTC_BASE_NS                      0x4820B000 /* RTC Non-Secure base address */
#define ISP_BASE_NS                      0x48300000 /* ISP SOC base address */
#define ISP_VIRTUAL_CAMERA_BASE_NS       0x48400000 /* ISP Virtual Camera base address */

#define VSOCKET_BASE_NS                  0x4FEE0000 /*!< VSOCKET Non-Secure base address */
#define VIO_BASE_NS                      0x4FEF0000 /*!< VIO Non-Secure base address */
#define VSI0_BASE_NS                     0x4FF00000 /*!< VSI 0 Non-Secure base address */
#define VSI1_BASE_NS                     0x4FF10000 /*!< VSI 1 Non-Secure base address */
#define VSI2_BASE_NS                     0x4FF20000 /*!< VSI 2 Non-Secure base address */
#define VSI3_BASE_NS                     0x4FF30000 /*!< VSI 3 Non-Secure base address */
#define VSI4_BASE_NS                     0x4FF40000 /*!< VSI 4 Non-Secure base address */
#define VSI5_BASE_NS                     0x4FF50000 /*!< VSI 5 Non-Secure base address */
#define VSI6_BASE_NS                     0x4FF60000 /*!< VSI 6 Non-Secure base address */
#define VSI7_BASE_NS                     0x4FF70000 /*!< VSI 7 Non-Secure base address */

/** Secure subsystem peripheral region */
#define DMA_350_BASE_S                   0x50002000 /* DMA350 register block Secure base address */
#define NPU0_APB_BASE_S                  0x50004000 /* NPU0 APB Secure base address */
#define CPU0_SECCTRL_BASE_S              0x50011000 /* CPU 0 Local Security Control Block Secure base address */
#define CPU0_PWRCTRL_BASE_S              0x50012000 /* CPU 0 Power Control Block Secure base address */
#define CPU0_IDENTITY_BASE_S             0x5001F000 /* CPU 0 Identity Block Secure base address */
#define CORSTONE320_SACFG_BASE_S         0x50080000 /* Corstone-320 Secure Access Configuration Register Secure base address */
#define MPC_ISRAM0_BASE_S                0x50083000 /* Internal SRAM0 Memory Protection Controller Secure base address */
#define MPC_ISRAM1_BASE_S                0x50084000 /* Internal SRAM1 Memory Protection Controller Secure base address */
#define CC3XX_BASE_S                     0x50094000 /* CryptoCell CC3XX Secure base address */
#define KMU_BASE_S                       0x5009E000 /* KMU Secure base address */
#define LCM_BASE_S                       0x500A0000 /* LCM Secure base address */

/** Secure MSTEXPPILL peripheral region */
#define GPIO0_CMSDK_BASE_S               0x50100000 /* GPIO 0 Secure base address */
#define GPIO1_CMSDK_BASE_S               0x50101000 /* GPIO 1 Secure base address */
#define GPIO2_CMSDK_BASE_S               0x50102000 /* GPIO 2 Secure base address */
#define GPIO3_CMSDK_BASE_S               0x50103000 /* GPIO 3 Secure base address */
#define AHB_USER_0_BASE_S                0x50104000 /* AHB USER 0 Secure base address */
#define AHB_USER_1_BASE_S                0x50105000 /* AHB USER 1 Secure base address */
#define AHB_USER_2_BASE_S                0x50106000 /* AHB USER 2 Secure base address */
#define AHB_USER_3_BASE_S                0x50107000 /* AHB USER 3 Secure base address */
#define HDLCD_BASE_S                     0x50310000 /* HDLCD Secure base address */
#define ETHERNET_BASE_S                  0x50400000 /* Ethernet Secure base address */
#define USB_BASE_S                       0x50500000 /* USB Secure base address */
#define USER_APB0_BASE_S                 0x50700000 /* User APB 0 Secure base address */
#define USER_APB1_BASE_S                 0x50701000 /* User APB 1 Secure base address */
#define USER_APB2_BASE_S                 0x50702000 /* User APB 2 Secure base address */
#define USER_APB3_BASE_S                 0x50703000 /* User APB 3 Secure base address */
#define QSPI_CONFIG_BASE_S               0x50800000 /* QSPI Config Secure base address */
#define QSPI_WRITE_BASE_S                0x50801000 /* QSPI Write Secure base address */
#define MPC_SRAM_BASE_S                  0x57000000 /* SRAM Memory Protection Controller Secure base address */
#define MPC_QSPI_BASE_S                  0x57001000 /* QSPI Memory Protection Controller Secure base address */
#define MPC_DDR4_BASE_S                  0x57002000 /* DDR4 Memory Protection Controller Secure base address */

/** Secure subsystem peripheral region */
#define SYSTIMER0_ARMV8_M_BASE_S         0x58000000 /* System Timer 0 Secure base address */
#define SYSTIMER1_ARMV8_M_BASE_S         0x58001000 /* System Timer 1 Secure base address */
#define SYSTIMER2_ARMV8_M_BASE_S         0x58002000 /* System Timer 0 Secure base address */
#define SYSTIMER3_ARMV8_M_BASE_S         0x58003000 /* System Timer 1 Secure base address */
#define CORSTONE320_SYSINFO_BASE_S       0x58020000 /* Corstone-320 System info Block Secure base address */
#define CORSTONE320_SYSCTRL_BASE_S       0x58021000 /* Corstone-320 System control Block Secure base address */
#define CORSTONE320_SYSPPU_BASE_S        0x58022000 /* Corstone-320 System Power Policy Unit Secure base address */
#define CORSTONE320_CPU0PPU_BASE_S       0x58023000 /* Corstone-320 CPU 0 Power Policy Unit Secure base address */
#define CORSTONE320_MGMTPPU_BASE_S       0x58028000 /* Corstone-320 Management Power Policy Unit Secure base address */
#define CORSTONE320_DBGPPU_BASE_S        0x58029000 /* Corstone-320 Debug Power Policy Unit Secure base address */
#define CORSTONE320_NPU0PPU_BASE_S       0x5802A000 /* Corstone-320 NPU 0 Power Policy Unit Secure base address */
#define SLOWCLK_WDOG_CMSDK_BASE_S        0x5802E000 /* CMSDK based SLOWCLK Watchdog Secure base address */
#define SLOWCLK_TIMER_CMSDK_BASE_S       0x5802F000 /* CMSDK based SLOWCLK Timer Secure base address */
#define SYSWDOG_ARMV8_M_CNTRL_BASE_S     0x58040000 /* Secure Watchdog Timer control frame Secure base address */
#define SYSWDOG_ARMV8_M_REFRESH_BASE_S   0x58041000 /* Secure Watchdog Timer refresh frame Secure base address */
#define SAM_BASE_S                       0x58042000 /* SAM Secure base address */
#define SYSCNTR_CNTRL_BASE_S             0x58100000 /* System Counter Control Secure base address */
#define SYSCNTR_READ_BASE_S              0x58101000 /* System Counter Read Secure base address */

/** Secure MSTEXPPIHL peripheral region */
#define FPGA_SBCon_I2C_TOUCH_BASE_S      0x58100000 /* FPGA - SBCon I2C (Touch) Secure base address */
#define FPGA_SBCon_I2C_AUDIO_BASE_S      0x58101000 /* FPGA - SBCon I2C (Audio Conf) Secure base address */
#define FPGA_SPI_ADC_BASE_S              0x58102000 /* FPGA - PL022 (SPI ADC) Secure base address */
#define FPGA_SPI_SHIELD0_BASE_S          0x58103000 /* FPGA - PL022 (SPI Shield0) Secure base address */
#define FPGA_SPI_SHIELD1_BASE_S          0x58104000 /* FPGA - PL022 (SPI Shield1) Secure base address */
#define SBCon_I2C_SHIELD0_BASE_S         0x58105000 /* SBCon (I2C - Shield0) Secure base address */
#define SBCon_I2C_SHIELD1_BASE_S         0x58106000 /* SBCon (I2C – Shield1) Secure base address */
#define USER_APB_BASE_S                  0x58107000 /* USER APB Secure base address */
#define FPGA_DDR4_EEPROM_BASE_S          0x58108000 /* FPGA - SBCon I2C (DDR4 EEPROM) Secure base address */
#define FPGA_SCC_BASE_S                  0x58200000 /* FPGA - SCC registers Secure base address */
#define FPGA_I2S_BASE_S                  0x58201000 /* FPGA - I2S (Audio) Secure base address */
#define FPGA_IO_BASE_S                   0x58202000 /* FPGA - IO (System Ctrl + I/O) Secure base address */
#define UART0_BASE_S                     0x58203000 /* UART 0 Secure base address */
#define UART1_BASE_S                     0x58204000 /* UART 1 Secure base address */
#define UART2_BASE_S                     0x58205000 /* UART 2 Secure base address */
#define UART3_BASE_S                     0x58206000 /* UART 3 Secure base address */
#define UART4_BASE_S                     0x58207000 /* UART 4 Secure base address */
#define UART5_BASE_S                     0x58208000 /* UART 5 Secure base address */
#define RTC_BASE_S                       0x5820B000 /* RTC Secure base address */

#define VSOCKET_BASE_S                   0x5FEE0000 /*!< VSOCKET Secure base address */
#define VIO_BASE_S                       0x5FEF0000 /*!< VIO Secure base address */
#define VSI0_BASE_S                      0x5FF00000 /*!< VSI 0 Secure base address */
#define VSI1_BASE_S                      0x5FF10000 /*!< VSI 1 Secure base address */
#define VSI2_BASE_S                      0x5FF20000 /*!< VSI 2 Secure base address */
#define VSI3_BASE_S                      0x5FF30000 /*!< VSI 3 Secure base address */
#define VSI4_BASE_S                      0x5FF40000 /*!< VSI 4 Secure base address */
#define VSI5_BASE_S                      0x5FF50000 /*!< VSI 5 Secure base address */
#define VSI6_BASE_S                      0x5FF60000 /*!< VSI 6 Secure base address */
#define VSI7_BASE_S                      0x5FF70000 /*!< VSI 7 Secure base address */

/* TCM Security Gate register addresses */
#define ITGU_CTRL_BASE                   0xE001E500 /* TGU control register for ITCM */
#define ITGU_CFG_BASE                    0xE001E504 /* TGU configuration register for ITCM */
#define ITGU_LUTn_BASE                   0xE001E510 /* TGU Look Up Table  register for ITCM */
#define DTGU_CTRL_BASE                   0xE001E600 /* TGU control register for DTCM */
#define DTGU_CFG_BASE                    0xE001E604 /* TGU configuration register for DTCM */
#define DTGU_LUTn_BASE                   0xE001E610 /* TGU Look Up Table  register for DTCM */

/* Memory map addresses exempt from memory attribution by both the SAU and IDAU */
#define CORSTONE320_EWIC_BASE            0xE0047000 /* External Wakeup Interrupt Controller
                                                     * Access from Non-secure software is only allowed
                                                     * if AIRCR.BFHFNMINS is set to 1 */

#endif /* PERIPHERAL_MEMMAP_H */
