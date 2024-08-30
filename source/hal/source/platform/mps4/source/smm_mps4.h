/*
 * SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#ifndef SMM_MPS4_H
#define SMM_MPS4_H

#include "peripheral_memmap.h"      /* Peripheral memory map definitions. */

#include "RTE_Components.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/** FPGA System Register declaration */
typedef struct
{
  __IO uint32_t LED;             /* Offset: 0x000 (R/W)  LED connections
                                  *                         [31:2] : Reserved
                                  *                          [1:0] : LEDs
                                  */
       uint32_t RESERVED1[1];
  __IO uint32_t BUTTON;          /* Offset: 0x008 (R/W)  Buttons
                                  *                         [31:2] : Reserved
                                  *                          [1:0] : Buttons
                                  */
       uint32_t RESERVED2[1];
  __IO uint32_t CLK1HZ;          /* Offset: 0x010 (R/W)  1Hz up counter    */
  __IO uint32_t CLK100HZ;        /* Offset: 0x014 (R/W)  100Hz up counter  */
  __IO uint32_t COUNTER;         /* Offset: 0x018 (R/W)  Cycle Up Counter
                                  *                         Increments when 32-bit prescale counter reach zero
                                  */
  __IO uint32_t PRESCALE;        /* Offset: 0x01C (R/W)  Prescaler
                                  *                         Bit[31:0] : reload value for prescale counter
                                  */
  __IO uint32_t PSCNTR;          /* Offset: 0x020 (R/W)  32-bit Prescale counter
                                  *                         current value of the pre-scaler counter
                                  *                         The Cycle Up Counter increment when the prescale down counter reach 0
                                  *                         The pre-scaler counter is reloaded with PRESCALE after reaching 0.
                                  */
       uint32_t RESERVED3[1];
  __IO uint32_t SWITCHES;        /* Offset: 0x028 (R/W)  Switches
                                  *                         [31:8] : Reserved
                                  *                          [7:0] : Switches
                                  */
       uint32_t RESERVED4[8];
  __IO uint32_t MISC;            /* Offset: 0x04C (R/W)  Misc control
                                  *                         [31:10] : Reserved
                                  *                            [9] :
                                  *                            [8] :
                                  *                            [7] : ADC_SPI_nCS
                                  *                            [6] : CLCD_BL_CTRL
                                  *                            [5] : CLCD_RD
                                  *                            [4] : CLCD_RS
                                  *                            [3] : CLCD_RESET
                                  *                            [2] : SHIELD_1_SPI_nCS
                                  *                            [1] : SHIELD_0_SPI_nCS
                                  *                            [0] : CLCD_CS
                                  */
} MPS4_FPGAIO_TypeDef;

/** SCC Register declaration */
typedef struct
{
  __IO uint32_t CFG_REG0;        /* Offset: 0x000 (R/W)  Remaps block RAM to ZBT
                                  *                         [31:1] : Reserved
                                  *                            [0] 1 : REMAP BlockRam to ZBT
                                  */
  __IO uint32_t LEDS;            /* Offset: 0x004 (R/W)  Controls the MCC user LEDs
                                  *                         [31:8] : Reserved
                                  *                          [7:0] : MCC LEDs
                                  */
       uint32_t RESERVED0[1];
  __I  uint32_t SWITCHES;        /* Offset: 0x00C (R/ )  Denotes the state of the MCC user switches
                                  *                         [31:8] : Reserved
                                  *                          [7:0] : These bits indicate state of the MCC switches
                                  */
  __I  uint32_t CFG_REG4;        /* Offset: 0x010 (R/ )  Denotes the board revision
                                  *                         [31:4] : Reserved
                                  *                          [3:0] : Used by the MCC to pass PCB revision. 0 = A 1 = B
                                  */
  __I  uint32_t CFG_ACLK;        /* Offset: 0x014 (R/ )  System Clock
                                  */
       uint32_t RESERVED1[34];
  __IO uint32_t SYS_CFGDATA_RTN; /* Offset: 0x0A0 (R/W)  User data register
                                  *                         [31:0] : Data
                                  */
  __IO uint32_t SYS_CFGDATA_OUT; /* Offset: 0x0A4 (R/W)  User data register
                                  *                         [31:0] : Data
                                  */
  __IO uint32_t SYS_CFGCTRL;     /* Offset: 0x0A8 (R/W)  Control register
                                  *                           [31] : Start (generates interrupt on write to this bit)
                                  *                           [30] : R/W access
                                  *                        [29:26] : Reserved
                                  *                        [25:20] : Function value
                                  *                        [19:12] : Reserved
                                  *                         [11:0] : Device (value of 0/1/2 for supported clocks)
                                  */
  __IO uint32_t SYS_CFGSTAT;     /* Offset: 0x0AC (R/W)  Contains status information
                                  *                         [31:2] : Reserved
                                  *                            [1] : Error
                                  *                            [0] : Complete
                                  */
  __IO uint32_t RESERVED2[20];
  __IO uint32_t SCC_DLL;         /* Offset: 0x100 (R/W)  DLL Lock Register
                                  *                        [31:24] : DLL LOCK MASK[7:0] - Indicate if the DLL locked is masked
                                  *                        [23:16] : DLL LOCK MASK[7:0] - Indicate if the DLLs are locked or unlocked
                                  *                         [15:1] : Reserved
                                  *                            [0] : This bit indicates if all enabled DLLs are locked
                                  */
       uint32_t RESERVED3[957];
  __I  uint32_t SCC_AID;         /* Offset: 0xFF8 (R/ )  SCC AID Register
                                  *                        [31:24] : FPGA build number
                                  *                        [23:20] : V2M-MPS4 target board revision (A = 0, B = 1)
                                  *                        [19:11] : Reserved
                                  *                           [10] : if “1” SCC_SW register has been implemented
                                  *                            [9] : if “1” SCC_LED register has been implemented
                                  *                            [8] : if “1” DLL lock register has been implemented
                                  *                          [7:0] : number of SCC configuration register
                                  */
  __I  uint32_t SCC_ID;          /* Offset: 0xFFC (R/ )  Contains information about the FPGA image
                                  *                        [31:24] : Implementer ID: 0x41 = ARM
                                  *                        [23:20] : Application note IP variant number
                                  *                        [19:16] : IP Architecture: 0x4 =AHB
                                  *                         [15:4] : Primary part number: 386 = AN386
                                  *                          [3:0] : Application note IP revision number
                                  */
} MPS4_SCC_TypeDef;

/** Non-secure peripheral declarations */
#define MPS4_FPGAIO             ((MPS4_FPGAIO_TypeDef   *) FPGA_IO_BASE_NS)
#define MPS4_SCC                ((MPS4_SCC_TypeDef      *) FPGA_SCC_BASE_NS)

/** Secure peripheral declarations */
#define MPS4_FPGAIO_S           ((MPS4_FPGAIO_TypeDef   *) FPGA_IO_BASE_S)
#define MPS4_SCC_S              ((MPS4_SCC_TypeDef      *) FPGA_SCC_BASE_S)

#endif /* SMM_MPS4_H */
