/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#define DESIGN_NAME              "Simple platform"

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
#define PL011_UART0_BASE           (0x49303000)    /* PL011 UART0 Base Address */

#define ETHOS_U_NPU_BASE           (0x48102000)    /* Ethos-U NPU base address*/
#define ETHOS_U_NPU_TA0_BASE       (0x48103000)    /* Ethos-U NPU's timing adapter 0 base address */
#define ETHOS_U_NPU_TA1_BASE       (0x48103200)    /* Ethos-U NPU's timing adapter 1 base address */

/******************************************************************************/
/*                      Secure Peripheral memory map                          */
/******************************************************************************/

#define SEC_ETHOS_U_NPU_BASE       (0x58102000)   /* Ethos-U NPU base address*/
#define SEC_ETHOS_U_NPU_TA0_BASE   (0x58103000)   /* Ethos-U NPU's timing adapter 0 base address */
#define SEC_ETHOS_U_NPU_TA1_BASE   (0x58103200)   /* Ethos-U NPU's timing adapter 1 base address */

#endif /* PERIPHERAL_MEMMAP_H */
