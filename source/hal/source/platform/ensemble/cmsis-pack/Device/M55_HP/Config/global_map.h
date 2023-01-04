/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

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

/* EXPMST0 Global Peripheral Address Map */

/* AHB - 2 */
#define EXPMST0_AHB_A_BASE        (EXPMST0_BASE)
#define EXPMST0_AHB_B_BASE        (EXPMST0_BASE + 0x00100000)

/* AHB-A Peripherals */
#define UTIMER_BASE               (EXPMST0_AHB_A_BASE)

/* AHB-B Peripherals */
#define ETH_BASE                  (EXPMST0_AHB_B_BASE)
#define SDMMC_BASE                (EXPMST0_AHB_B_BASE + 0x00002000)
#define SPI0_BASE                 (EXPMST0_AHB_B_BASE + 0x00003000)
#define SPI1_BASE                 (EXPMST0_AHB_B_BASE + 0x00004000)
#define SPI2_BASE                 (EXPMST0_AHB_B_BASE + 0x00005000)
#define SPI3_BASE                 (EXPMST0_AHB_B_BASE + 0x00006000)
#define CRC0_BASE                 (EXPMST0_AHB_B_BASE + 0x00007000)
#define CRC1_BASE                 (EXPMST0_AHB_B_BASE + 0x00008000)
#define USB0_BASE                 (EXPMST0_AHB_B_BASE + 0x00100000)

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
#define I2C0_BASE                 (EXPMST0_APB_B_BASE)
#define I2C1_BASE                 (EXPMST0_APB_B_BASE + 0x00001000)
#define I2C2_BASE                 (EXPMST0_APB_B_BASE + 0x00002000)
#define I2C3_BASE                 (EXPMST0_APB_B_BASE + 0x00003000)
#define I2S0_BASE                 (EXPMST0_APB_B_BASE + 0x00004000)
#define I2S1_BASE                 (EXPMST0_APB_B_BASE + 0x00005000)
#define I2S2_BASE                 (EXPMST0_APB_B_BASE + 0x00006000)
#define I2S3_BASE                 (EXPMST0_APB_B_BASE + 0x00007000)
#define I3C0_BASE                 (EXPMST0_APB_B_BASE + 0x00008000)
#define CANFD_BASE                (EXPMST0_APB_B_BASE + 0x00009000)
#define CAN_CNT_BASE              (EXPMST0_APB_B_BASE + 0x0000A000)
#define HWSEM0_BASE               (EXPMST0_APB_B_BASE + 0x0000B000)
#define HWSEM1_BASE               (EXPMST0_APB_B_BASE + 0x0000B010)
#define HWSEM2_BASE               (EXPMST0_APB_B_BASE + 0x0000B020)
#define HWSEM3_BASE               (EXPMST0_APB_B_BASE + 0x0000B030)
#define HWSEM4_BASE               (EXPMST0_APB_B_BASE + 0x0000B040)
#define HWSEM5_BASE               (EXPMST0_APB_B_BASE + 0x0000B050)
#define HWSEM6_BASE               (EXPMST0_APB_B_BASE + 0x0000B060)
#define HWSEM7_BASE               (EXPMST0_APB_B_BASE + 0x0000B070)
#define HWSEM8_BASE               (EXPMST0_APB_B_BASE + 0x0000B080)
#define HWSEM9_BASE               (EXPMST0_APB_B_BASE + 0x0000B090)
#define HWSEM10_BASE              (EXPMST0_APB_B_BASE + 0x0000B0A0)
#define HWSEM11_BASE              (EXPMST0_APB_B_BASE + 0x0000B0B0)
#define HWSEM12_BASE              (EXPMST0_APB_B_BASE + 0x0000B0C0)
#define HWSEM13_BASE              (EXPMST0_APB_B_BASE + 0x0000B0D0)
#define HWSEM14_BASE              (EXPMST0_APB_B_BASE + 0x0000B0E0)
#define HWSEM15_BASE              (EXPMST0_APB_B_BASE + 0x0000B0F0)
#define PDM_BASE                  (EXPMST0_APB_B_BASE + 0x0000C000)

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
#define CFGMST0_GPIO_CTRL         (CFGMST0_BASE + 0x00000004)
#define CFGMST0_SSI               (CFGMST0_BASE + 0x00000028)

/* APB-D Peripherals */
#define CAMERA0_BASE              (EXPMST0_APB_D_BASE)
#define CDC200_BASE               (EXPMST0_APB_D_BASE + 0x00001000)
#define MIPI_DSI_BASE             (EXPMST0_APB_D_BASE + 0x00002000)
#define MIPI_CSI2_BASE            (EXPMST0_APB_D_BASE + 0x00003000)
#define CFGSLV1_BASE              (EXPMST0_APB_D_BASE + 0x0000F000)
#define CDC200_PIXCLK_CTRL        (CFGSLV1_BASE + 0x00000004)
#define CSI_PIXCLK_CTRL           (CFGSLV1_BASE + 0x00000008)

/* APB-E Peripherals */
#define AES0_BASE                 (EXPMST0_APB_E_BASE)
#define AES1_BASE                 (EXPMST0_APB_E_BASE + 0x01000)
#define DAVE2D_BASE               (EXPMST0_APB_E_BASE + 0x02000)
#define DMA0_SE_BASE              (EXPMST0_APB_E_BASE + 0x40000)
#define DMA0_NS_BASE              (EXPMST0_APB_E_BASE + 0x60000)

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

/* OSPI Address Map */
#define OSPI0_BASE                0xD0000000
#define OSPI1_BASE                0xD8000000
#define OSPI0_SIZE                0x08000000UL
#define OSPI1_SIZE                0x08000000UL

#endif /* GLOBAL_MAP_H */
