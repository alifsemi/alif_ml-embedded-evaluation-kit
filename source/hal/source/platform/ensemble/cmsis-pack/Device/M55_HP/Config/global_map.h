/* -----------------------------------------------------------------------------
 * Copyright (c) 2021 Alif Semiconductor Inc.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * $Date:        25. Feburary 2021
 * $Revision:    V1.0.0
 * Author:       Sudhir Sreedharan
 * Project:      Global Memory Map of SOC
 * -------------------------------------------------------------------------- */

#ifndef GLOBAL_MAP_H
#define GLOBAL_MAP_H

/******************************************************************************/
/*                         Global memory map                              */
/******************************************************************************/

/*On chip RAM Regions */
#define SRAM0_BASE                0x02000000
#define SRAM0_SIZE                0x00400000		/* 4M */
#define SRAM1_BASE                0x08000000
#define SRAM1_SIZE                0x00280000		/* 2.5M */
#define SRAM2_BASE                0x50000000
#define SRAM2_SIZE                0x00040000		/* 256K */
#define SRAM3_BASE                0x50800000
#define SRAM3_SIZE                0x00100000		/* 1M */
#define SRAM4_BASE                0x60000000
#define SRAM4_SIZE                0x00040000		/* 256K */
#define SRAM5_BASE                0x60800000
#define SRAM5_SIZE                0x00040000		/* 256K */
#define SRAM6_BASE                0x62000000
#define SRAM6_SIZE                0x00200000		/* 2M */
#define SRAM7_BASE                0x63000000
#define SRAM7_SIZE                0x00080000		/* 512K */
#define SRAM8_BASE                0x63100000
#define SRAM8_SIZE                0x00200000		/* 2M */
#define SRAM9_BASE                0x64000000
#define SRAM9_SIZE                0x000C0000		/* 768K */
/* On Chip NVM */
#define MRAM_BASE                 0x80000000
#define MRAM_SIZE                 0x00580000		/* 5.5M */

/* Peripheral regions Base Address */
#define EXPMST0_BASE              (0x48000000UL)
#define LP_PERIPHERAL_BASE        (0x70000000UL)

/* EXPMSTO Global Peripheral Address Map */

/* AHB - 2 */
#define EXPMST0_AHB_A_BASE        (EXPMST0_BASE)
#define EXPMST0_AHB_B_BASE        (EXPMST0_BASE + 0x00100000)

/* AHB-A Peripherals */
#define UTIMER_BASE               (EXPMST0_AHB_A_BASE)

/* AHB-B Peripherals */
#define ETH_BASE                  (EXPMST0_AHB_B_BASE)
#define SPI0_BASE                 (EXPMST0_AHB_B_BASE + 0x00003000)
#define SPI1_BASE                 (EXPMST0_AHB_B_BASE + 0x00004000)
#define SPI2_BASE                 (EXPMST0_AHB_B_BASE + 0x00005000)
#define SPI3_BASE                 (EXPMST0_AHB_B_BASE + 0x00006000)

/* APB - 5 */
#define EXPMST0_APB_A_BASE        (EXPMST0_BASE + 0x01000000)
#define EXPMST0_APB_B_BASE        (EXPMST0_BASE + 0x01010000)
#define EXPMST0_APB_C_BASE        (EXPMST0_BASE + 0x01020000)
#define EXPMST0_APB_D_BASE        (EXPMST0_BASE + 0x01030000)
#define EXPMST0_APB_E_BASE        (EXPMST0_BASE + 0x01040000)


/* APB-A Peripherals */
#define GPIO1_BASE                  (EXPMST0_APB_A_BASE)
#define GPIO2_BASE                  (EXPMST0_APB_A_BASE + 0x00001000)
#define GPIO3_BASE                  (EXPMST0_APB_A_BASE + 0x00002000)
#define GPIO4_BASE                  (LP_PERIPHERAL_BASE)
#define UART0_BASE                  (EXPMST0_APB_A_BASE + 0x00008000)
#define UART1_BASE                  (EXPMST0_APB_A_BASE + 0x00009000)
#define UART2_BASE                  (EXPMST0_APB_A_BASE + 0x0000A000)
#define UART3_BASE                  (EXPMST0_APB_A_BASE + 0x0000B000)
#define UART4_BASE                  (EXPMST0_APB_A_BASE + 0x0000C000)
#define UART5_BASE                  (EXPMST0_APB_A_BASE + 0x0000D000)
#define UART6_BASE                  (EXPMST0_APB_A_BASE + 0x0000E000)
#define UART7_BASE                  (EXPMST0_APB_A_BASE + 0x0000F000)

/* APB-B Peripherals */
#define I2S0_BASE                 (EXPMST0_APB_B_BASE + 0x00004000)
#define I2S1_BASE                 (EXPMST0_APB_B_BASE + 0x00005000)
#define I2S2_BASE                 (EXPMST0_APB_B_BASE + 0x00006000)
#define I2S3_BASE                 (EXPMST0_APB_B_BASE + 0x00007000)
#define I3C0_BASE                 (EXPMST0_APB_B_BASE + 0x00008000)

/* APB-C Peripherals */
#define ADC0_BASE                 (EXPMST0_APB_C_BASE)
#define ADC1_BASE                 (EXPMST0_APB_C_BASE + 0x00001000)
#define ADC2_BASE                 (EXPMST0_APB_C_BASE + 0x00002000)
#define COMP0_BASE                (EXPMST0_APB_C_BASE + 0x00003000)
#define COMP1_BASE                (EXPMST0_APB_C_BASE + 0x00004000)
#define COMP2_BASE                (EXPMST0_APB_C_BASE + 0x00005000)
#define COMP3_BASE                (EXPMST0_APB_C_BASE + 0x00006000)
#define DAC0_BASE                 (EXPMST0_APB_C_BASE + 0x00008000)
#define DAC1_BASE                 (EXPMST0_APB_C_BASE + 0x00009000)
#define CFGMST0_BASE              (EXPMST0_APB_C_BASE + 0x0000F000)
#define CFGMST0_SSI               (CFGMST0_BASE + 0x00000028)

/* APB-D Peripherals */
#define CAMERA0_BASE              (EXPMST0_APB_D_BASE)
#define CDC200_BASE               (EXPMST0_APB_D_BASE + 0x00001000)
#define MIPI_DSI_BASE             (EXPMST0_APB_D_BASE + 0x00002000)
#define MIPI_CSI2_BASE            (EXPMST0_APB_D_BASE + 0x00003000)
#define CFGSLV1_BASE              (EXPMST0_APB_D_BASE + 0x0000F000)
#define CSI_PIXCLK_CTRL           (CFGSLV1_BASE + 0x00000008)

/* APB-E Peripherals */

/* VBAT Global Address Map */
#define VBAT_BASE                 (LP_PERIPHERAL_BASE)
#define RTC0_BASE                 (VBAT_BASE + 0x00010000)
#define LPTIMER_BASE              (VBAT_BASE + 0x00030000)
#if RTE_SILICON_REV_A
#define LPTIMER_CLK_SEL_REG       (LPTIMER_BASE + 0x00010028)
#endif
#if RTE_SILICON_REV_B0
#define LPTIMER_CLK_SEL_REG       (0x1A609004)
#endif
#define VBAT_REGS_BASE            (VBAT_BASE + 0x00040000)
#define VBAT_ANA_REG2_BASE        (VBAT_REGS_BASE + 0x0000001C)

/* LPAON Global Address Map */
#define PADCTRL_BASE                        (LPAON_VBAT_REGS_BASE)
#define VBAT_GPIOV_REGS                     (LP_PERIPHERAL_BASE + 0x70000)
#define LP_PADCTRL_BASE                     (VBAT_GPIOV_REGS + 0x160)
#define LPAON_BASE                          (LP_PERIPHERAL_BASE + 0x01000000)
#define PINMUX_BASE                         (LPAON_BASE + 0x00006000)
#define LPAON_VBAT_REGS_BASE                (LPAON_BASE + 0x00007000)
#define LPAON_CLK_RST_4_BASE                (LPAON_VBAT_REGS_BASE + 0x00000410)
#define LPAON_REGS_BASE                     (LPAON_BASE + 0x0000A000)
#define LPAON_ANA_XTAL_OSCILLATOR_REG_BASE  (LPAON_REGS_BASE + 0x00000100)

/* Ethernet 50MHz clock mux register */
#define ETH_50M_CLK_MUX_REG       (LPAON_VBAT_REGS_BASE + 0x00000408)

#endif /* GLOBAL_MAP_H */
