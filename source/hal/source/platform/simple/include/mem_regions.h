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

#ifndef MEM_REGION_DEFS_H
#define MEM_REGION_DEFS_H

#define ITCM_SIZE             (0x00080000)     /* ITCM size */
#define DTCM_BLK_SIZE         (0x00020000)     /* DTCM size, 4 banks of this size available */
#define BRAM_SIZE             (0x00100000)     /* BRAM size */
#define ISRAM0_SIZE           (0x00100000)     /* ISRAM0 size */
#define ISRAM1_SIZE           (0x00100000)     /* ISRAM1 size */
#define DDR4_BLK_SIZE         (0x10000000)     /* DDR4 block size */

#define ITCM_BASE_NS          (0x00000000)     /* Instruction TCM Non-Secure base address */
#define BRAM_BASE_NS          (0x01000000)     /* CODE SRAM Non-Secure base address */
#define DTCM0_BASE_NS         (0x20000000)     /* Data TCM block 0 Non-Secure base address */
#define DTCM1_BASE_NS         (0x20020000)     /* Data TCM block 1 Non-Secure base address */
#define DTCM2_BASE_NS         (0x20040000)     /* Data TCM block 2 Non-Secure base address */
#define DTCM3_BASE_NS         (0x20060000)     /* Data TCM block 3 Non-Secure base address */
#define ISRAM0_BASE_NS        (0x21000000)     /* Internal SRAM Area Non-Secure base address */
#define ISRAM1_BASE_NS        (0x21100000)     /* Internal SRAM Area Non-Secure base address */
#define QSPI_SRAM_BASE_NS     (0x28000000)     /* QSPI SRAM Non-Secure base address */
#define DDR4_BLK0_BASE_NS     (0x60000000)     /* DDR4 block 0 Non-Secure base address */
#define DDR4_BLK1_BASE_NS     (0x80000000)     /* DDR4 block 1 Non-Secure base address */
#define DDR4_BLK2_BASE_NS     (0xA0000000)     /* DDR4 block 2 Non-Secure base address */
#define DDR4_BLK3_BASE_NS     (0xC0000000)     /* DDR4 block 3 Non-Secure base address */

#define ITCM_BASE_S           (0x10000000)     /* Instruction TCM Secure base address */
#define BRAM_BASE_S           (0x11000000)     /* CODE SRAM Secure base address */
#define DTCM0_BASE_S          (0x30000000)     /* Data TCM block 0 Secure base address */
#define DTCM1_BASE_S          (0x30020000)     /* Data TCM block 1 Secure base address */
#define DTCM2_BASE_S          (0x30040000)     /* Data TCM block 2 Secure base address */
#define DTCM3_BASE_S          (0x30060000)     /* Data TCM block 3 Secure base address */
#define ISRAM0_BASE_S         (0x31000000)     /* Internal SRAM Area Secure base address */
#define ISRAM1_BASE_S         (0x31100000)     /* Internal SRAM Area Secure base address */
#define DDR4_BLK0_BASE_S      (0x70000000)     /* DDR4 block 0 Secure base address */
#define DDR4_BLK1_BASE_S      (0x90000000)     /* DDR4 block 1 Secure base address */
#define DDR4_BLK2_BASE_S      (0xB0000000)     /* DDR4 block 2 Secure base address */
#define DDR4_BLK3_BASE_S      (0xD0000000)     /* DDR4 block 3 Secure base address */

#endif /*  MEM_REGION_DEFS_H  */
