/*
 * SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License)Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing)software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND)either express or implied.
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
#define NONSEC_WATCHDOG_IRQn               (1)   /* Non-Secure Watchdog Interrupt */
#define SLOWCLK_TIMER_IRQn                 (2)   /* SLOWCLK Timer Interrupt */
#define TIMER0_IRQn                        (3)   /* TIMER 0 Interrupt */
#define TIMER1_IRQn                        (4)   /* TIMER 1 Interrupt */
#define TIMER2_IRQn                        (5)   /* TIMER 2 Interrupt */

/** 6,7 and 8 are reserved */

#define MPC_IRQn                           (9)   /* MPC Combined (Secure) Interrupt */
#define PPC_IRQn                           (10)  /* PPC Combined (Secure) Interrupt */
#define MSC_IRQn                           (11)  /* MSC Combined (Secure) Interrupt */
#define BRIDGE_ERROR_IRQn                  (12)  /* Bridge Error Combined (Secure) Interrupt*/

/** 13 reserved */

#define Combined_PPU_IRQn                  (14)  /* Combined PPU */
#define SDC_IRQn                           (15)  /* Secure Debug channel Interrupt */
#define NPU0_IRQn                          (16)  /* NPU0 i.e., EthosU_IRQn */

/** 17, 18 and 19 are reserved */

#define KMU_IRQn                           (20)  /* KMU Interrupt */

/** 21, 22 and 23 are reserved */

#define DMA_SEC_Combined_IRQn              (24)  /* DMA Secure Combined Interrupt */
#define DMA_NONSEC_Combined_IRQn           (25)  /* DMA Non-Secure Combined Interrupt */
#define DMA_SECURITY_VIOLATION_IRQn        (26)  /* DMA Security Violation Interrupt */
#define TIMER3_AON_IRQn                    (27)  /* TIMER 3 AON Interrupt */
#define CPU0_CTI_0_IRQn                    (28)  /* CPU0 CTI IRQ 0 */
#define CPU0_CTI_1_IRQn                    (29)  /* CPU0 CTI IRQ 1 */
#define SAM_CRITICAL_SEVERITY_FAULT_IRQn   (30)  /* SAM Critical Severity Fault Interrupt */
#define SAM_SEVERITY_FAULT_HANDLER_IRQn    (31)  /* SAM Severity Fault Handler Interrupt */

/** 32 reserved */

#define UARTRX0_IRQn                       (33)  /* UART 0 RX Interrupt */
#define UARTTX0_IRQn                       (34)  /* UART 0 TX Interrupt */
#define UARTRX1_IRQn                       (35)  /* UART 1 RX Interrupt */
#define UARTTX1_IRQn                       (36)  /* UART 1 TX Interrupt */
#define UARTRX2_IRQn                       (37)  /* UART 2 RX Interrupt */
#define UARTTX2_IRQn                       (38)  /* UART 2 TX Interrupt */
#define UARTRX3_IRQn                       (39)  /* UART 3 RX Interrupt */
#define UARTTX3_IRQn                       (40)  /* UART 3 TX Interrupt */
#define UARTRX4_IRQn                       (41)  /* UART 4 RX Interrupt */
#define UARTTX4_IRQn                       (42)  /* UART 4 TX Interrupt */
#define UART0_Combined_IRQn                (43)  /* UART 0 Combined Interrupt */
#define UART1_Combined_IRQn                (44)  /* UART 1 Combined Interrupt */
#define UART2_Combined_IRQn                (45)  /* UART 2 Combined Interrupt */
#define UART3_Combined_IRQn                (46)  /* UART 3 Combined Interrupt */
#define UART4_Combined_IRQn                (47)  /* UART 4 Combined Interrupt */
#define UARTOVF_IRQn                       (48)  /* UART 0, 1, 2, 3, 4 & 5 Overflow Interrupt */
#define ETHERNET_IRQn                      (49)  /* Ethernet Interrupt */
#define I2S_IRQn                           (50)  /* Audio I2S Interrupt */

/** 51-16 are reserved */

#define DMA_CHANNEL_0_IRQn                 (57)  /* DMA Channel 0 Interrupt */
#define DMA_CHANNEL_1_IRQn                 (58)  /* DMA Channel 1 Interrupt */

/** 59-68 are reseved */

#define GPIO0_Combined_IRQn                (69)  /* GPIO 0 Combined Interrupt */
#define GPIO1_Combined_IRQn                (70)  /* GPIO 1 Combined Interrupt */
#define GPIO2_Combined_IRQn                (71)  /* GPIO 2 Combined Interrupt */
#define GPIO3_Combined_IRQn                (72)  /* GPIO 3 Combined Interrupt */

/** 73-124 are reserved */

#define UARTRX5_IRQn                       (125) /* UART 5 RX Interrupt */
#define UARTTX5_IRQn                       (126) /* UART 5 TX Interrupt */

/** 127 reserved */

#define RTC_IRQn                           (128) /* RTC Interrupt */

/** 129-131 are reserved */

#define ISP_IRQn                           (132) /* ISP C55 Interrupt */
#define HDLCD_IRQn                         (133) /* HDLCD Interrupt */

/** 134-223 are reserved */

#define ARM_VSI0_IRQn                      (224) /* VSI 0 Interrupt */
#define ARM_VSI1_IRQn                      (225) /* VSI 1 Interrupt */
#define ARM_VSI2_IRQn                      (226) /* VSI 2 Interrupt */
#define ARM_VSI3_IRQn                      (227) /* VSI 3 Interrupt */
#define ARM_VSI4_IRQn                      (228) /* VSI 4 Interrupt */
#define ARM_VSI5_IRQn                      (229) /* VSI 5 Interrupt */
#define ARM_VSI6_IRQn                      (230) /* VSI 6 Interrupt */
#define ARM_VSI7_IRQn                      (231) /* VSI 7 Interrupt */

#endif /* PERIPHERAL_IRQS_H */
