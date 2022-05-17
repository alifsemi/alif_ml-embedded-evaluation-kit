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

#ifndef PERIPHERAL_IRQS_H
#define PERIPHERAL_IRQS_H

/******************************************************************************/
/*                    Peripheral interrupt numbers                            */
/******************************************************************************/

/* -------------------  Cortex-M Processor Exceptions Numbers  -------------- */
/*                 -14 to -1 should be defined by the system header           */
/* ----------------------  Core Specific Interrupt Numbers  ------------------*/
#define NONSEC_WATCHDOG_RESET_IRQn ( 0)  /* Non-Secure Watchdog Reset Interrupt   */
#define NONSEC_WATCHDOG_IRQn       ( 1)  /* Non-Secure Watchdog Interrupt         */
#define S32K_TIMER_IRQn            ( 2)  /* S32K Timer Interrupt                  */
#define TIMER0_IRQn                ( 3)  /* TIMER 0 Interrupt                     */
#define TIMER1_IRQn                ( 4)  /* TIMER 1 Interrupt                     */
#define TIMER2_IRQn                ( 5)  /* TIMER 2 Interrupt                    */
#define MPC_IRQn                   ( 9)  /* MPC Combined () Interrupt       */
#define PPC_IRQn                   (10)  /* PPC Combined () Interrupt       */
#define MSC_IRQn                   (11)  /* MSC Combined () Interrput       */
#define BRIDGE_ERROR_IRQn          (12)  /* Bridge Error Combined () Interrupt */
#define MGMT_PPU_IRQn              (14)  /* MGMT_PPU */
#define EthosU_IRQn                (16)   /* Ethos-Uxx Interrupt */
#define CPU0_PPU_IRQn              (16)  /* CPU0_PPU */
#define DEBUG_PPU_IRQn             (26)  /* DEBUG_PPU */
#define TIMER3_AON_IRQn            (27)  /* TIMER3_AON */
#define CPU0CTIIQ0_IRQn            (28)  /* CPU0CTIIQ0 */
#define CPU0CTIIQ01_IRQn           (29)  /* CPU0CTIIQ01 */

#define SYS_TSTAMP_COUNTER_IRQn    (32)  /* System timestamp counter interrupt */

/* ----------------------  CMSDK Specific Interrupt Numbers  ----------------- */
#define UARTRX0_IRQn               (33)  /* UART 0 RX Interrupt                   */
#define UARTTX0_IRQn               (34)  /* UART 0 TX Interrupt                   */
#define UARTRX1_IRQn               (35)  /* UART 1 RX Interrupt                   */
#define UARTTX1_IRQn               (36)  /* UART 1 TX Interrupt                   */
#define UARTRX2_IRQn               (37)  /* UART 2 RX Interrupt                   */
#define UARTTX2_IRQn               (38)  /* UART 2 TX Interrupt                   */
#define UARTRX3_IRQn               (39)  /* UART 3 RX Interrupt                   */
#define UARTTX3_IRQn               (40)  /* UART 3 TX Interrupt                   */
#define UARTRX4_IRQn               (41)  /* UART 4 RX Interrupt                   */
#define UARTTX4_IRQn               (42)  /* UART 4 TX Interrupt                   */
#define UART0_IRQn                 (43)  /* UART 0 combined Interrupt             */
#define UART1_IRQn                 (44)  /* UART 1 combined Interrupt             */
#define UART2_IRQn                 (45)  /* UART 2 combined Interrupt             */
#define UART3_IRQn                 (46)  /* UART 3 combined Interrupt             */
#define UART4_IRQn                 (47)  /* UART 4 combined Interrupt             */
#define UARTOVF_IRQn               (48)  /* UART 0,1,2,3 and 4 Overflow Interrupt */
#define ETHERNET_IRQn              (49)  /* Ethernet Interrupt                    */
#define I2S_IRQn                   (50)  /* I2S Interrupt                         */
#define TSC_IRQn                   (51)  /* Touch Screen Interrupt                */
#define SPI2_IRQn                  (53)  /* SPI 2 Interrupt                       */
#define SPI3_IRQn                  (54)  /* SPI 3 Interrupt                       */
#define SPI4_IRQn                  (55)  /* SPI 4 Interrupt                       */

#define GPIO0_IRQn                 (69)  /* GPIO 0 Combined Interrupt             */
#define GPIO1_IRQn                 (70)  /* GPIO 1 Combined Interrupt             */
#define GPIO2_IRQn                 (71)  /* GPIO 2 Combined Interrupt             */
#define GPIO3_IRQn                 (72)  /* GPIO 3 Combined Interrupt             */

#define GPIO0_0_IRQn               (73)  /* All P0 I/O pins used as irq source    */
#define GPIO0_1_IRQn               (74)  /* There are 16 pins in total            */
#define GPIO0_2_IRQn               (75)
#define GPIO0_3_IRQn               (76)
#define GPIO0_4_IRQn               (77)
#define GPIO0_5_IRQn               (78)
#define GPIO0_6_IRQn               (79)
#define GPIO0_7_IRQn               (80)
#define GPIO0_8_IRQn               (81)
#define GPIO0_9_IRQn               (82)
#define GPIO0_10_IRQn              (83)
#define GPIO0_11_IRQn              (84)
#define GPIO0_12_IRQn              (85)
#define GPIO0_13_IRQn              (86)
#define GPIO0_14_IRQn              (87)
#define GPIO0_15_IRQn              (88)
#define GPIO1_0_IRQn               (89)  /* All P1 I/O pins used as irq source    */
#define GPIO1_1_IRQn               (90)  /* There are 16 pins in total            */
#define GPIO1_2_IRQn               (91)
#define GPIO1_3_IRQn               (92)
#define GPIO1_4_IRQn               (93)
#define GPIO1_5_IRQn               (94)
#define GPIO1_6_IRQn               (95)
#define GPIO1_7_IRQn               (96)
#define GPIO1_8_IRQn               (97)
#define GPIO1_9_IRQn               (98)
#define GPIO1_10_IRQn              (99)
#define GPIO1_11_IRQn              (100)
#define GPIO1_12_IRQn              (101)
#define GPIO1_13_IRQn              (102)
#define GPIO1_14_IRQn              (103)
#define GPIO1_15_IRQn              (104)
#define GPIO2_0_IRQn               (105)  /* All P2 I/O pins used as irq source    */
#define GPIO2_1_IRQn               (106)  /* There are 15 pins in total            */
#define GPIO2_2_IRQn               (107)
#define GPIO2_3_IRQn               (108)
#define GPIO2_4_IRQn               (109)
#define GPIO2_5_IRQn               (110)
#define GPIO2_6_IRQn               (111)
#define GPIO2_7_IRQn               (112)
#define GPIO2_8_IRQn               (113)
#define GPIO2_9_IRQn               (114)
#define GPIO2_10_IRQn              (115)
#define GPIO2_11_IRQn              (116)
#define GPIO2_12_IRQn              (117)
#define GPIO2_13_IRQn              (118)
#define GPIO2_14_IRQn              (119)
#define GPIO2_15_IRQn              (120)
#define GPIO3_0_IRQn               (121)  /* All P3 I/O pins used as irq source    */
#define GPIO3_1_IRQn               (122)  /* There are 4 pins in total             */
#define GPIO3_2_IRQn               (123)
#define GPIO3_3_IRQn               (124)
#define UARTRX5_IRQn               (125)  /* UART 5 RX Interrupt                   */
#define UARTTX5_IRQn               (126)  /* UART 5 TX Interrupt                   */
#define UART5_IRQn                 (127)  /* UART 5 combined Interrupt             */
/* #undef HDCLCD_IRQn */

#endif /* PERIPHERAL_IRQS_H */
