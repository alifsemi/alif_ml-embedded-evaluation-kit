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

#ifndef MEM_REGION_DEFS_H
#define MEM_REGION_DEFS_H

/** Sizes */
#define ITCM_SIZE             (0x00080000)     /* ITCM size */
#define DTCM_BLK_SIZE         (0x00020000)     /* DTCM size, 4 banks of this size available */
#define BRAM_SIZE             (0x00200000)     /* BRAM size */
#define ISRAM0_SIZE           (0x00200000)     /* ISRAM0 size */
#define ISRAM1_SIZE           (0x00200000)     /* ISRAM1 size */
#define QSPI_SRAM_SIZE        (0x00800000)     /* QSPI Flash size */
#define DDR4_BLK_SIZE         (0x10000000)     /* DDR4 block size */

/** Non-secure addresses */
#define ITCM_BASE_NS          (0x00000000)     /* Instruction TCM Non-Secure base address */
#define BRAM_BASE_NS          (0x01000000)     /* FPGA SRAM Non-Secure base address */
#define DTCM0_BASE_NS         (0x20000000)     /* Data TCM block 0 Non-Secure base address */
#define DTCM1_BASE_NS         (0x20020000)     /* Data TCM block 1 Non-Secure base address */
#define DTCM2_BASE_NS         (0x20040000)     /* Data TCM block 2 Non-Secure base address */
#define DTCM3_BASE_NS         (0x20060000)     /* Data TCM block 3 Non-Secure base address */
#define ISRAM0_BASE_NS        (0x21000000)     /* Internal SRAM Area Non-Secure base address */
#define ISRAM1_BASE_NS        (0x21200000)     /* Internal SRAM Area Non-Secure base address */
#define QSPI_SRAM_BASE_NS     (0x28000000)     /* QSPI SRAM Non-Secure base address */
#define DDR4_BLK0_BASE_NS     (0x60000000)     /* DDR4 block 0 Non-Secure base address */
#define DDR4_BLK1_BASE_NS     (0x80000000)     /* DDR4 block 1 Non-Secure base address */
#define DDR4_BLK2_BASE_NS     (0xA0000000)     /* DDR4 block 2 Non-Secure base address */
#define DDR4_BLK3_BASE_NS     (0xC0000000)     /* DDR4 block 3 Non-Secure base address */

/** Secure addresses */
#define ITCM_BASE_S           (0x10000000)     /* Instruction TCM Secure base address */
#define BRAM_BASE_S           (0x12000000)     /* FPGA SRAM Secure base address */
#define DTCM0_BASE_S          (0x30000000)     /* Data TCM block 0 Secure base address */
#define DTCM1_BASE_S          (0x30020000)     /* Data TCM block 1 Secure base address */
#define DTCM2_BASE_S          (0x30040000)     /* Data TCM block 2 Secure base address */
#define DTCM3_BASE_S          (0x30060000)     /* Data TCM block 3 Secure base address */
#define ISRAM0_BASE_S         (0x31000000)     /* Internal SRAM Area Secure base address */
#define ISRAM1_BASE_S         (0x31200000)     /* Internal SRAM Area Secure base address */
#define DDR4_BLK0_BASE_S      (0x70000000)     /* DDR4 block 0 Secure base address */
#define DDR4_BLK1_BASE_S      (0x90000000)     /* DDR4 block 1 Secure base address */
#define DDR4_BLK2_BASE_S      (0xB0000000)     /* DDR4 block 2 Secure base address */
#define DDR4_BLK3_BASE_S      (0xD0000000)     /* DDR4 block 3 Secure base address */

/** All VMs use the same MPC block size as defined by VMMPCBLKSIZE. */
#define SRAM_MPC_BLK_SIZE    (0x4000)     /* 16 kB */
#define QSPI_MPC_BLK_SIZE    (0x40000)    /* 256 kB */
#define DDR4_MPC_BLK_SIZE    (0x100000)   /* 1 MB */

/** Defines for Driver MPC's */
/** SRAM -- 2 MB */
#define MPC_SRAM_RANGE_BASE_NS   (SRAM_BASE_NS)
#define MPC_SRAM_RANGE_LIMIT_NS  (SRAM_BASE_NS + SRAM_SIZE-1)
#define MPC_SRAM_RANGE_OFFSET_NS (0x0)
#define MPC_SRAM_RANGE_BASE_S    (SRAM_BASE_S)
#define MPC_SRAM_RANGE_LIMIT_S   (SRAM_BASE_S + SRAM_SIZE-1)
#define MPC_SRAM_RANGE_OFFSET_S  (0x0)

/** QSPI -- 8 MB */
#define MPC_QSPI_RANGE_BASE_NS   (QSPI_SRAM_BASE_NS)
#define MPC_QSPI_RANGE_LIMIT_NS  (QSPI_SRAM_BASE_NS + QSPI_SRAM_SIZE-1)
#define MPC_QSPI_RANGE_OFFSET_NS (0x0)
#define MPC_QSPI_RANGE_BASE_S    (QSPI_SRAM_BASE_S)
#define MPC_QSPI_RANGE_LIMIT_S   (QSPI_SRAM_BASE_S + QSPI_SRAM_SIZE-1)
#define MPC_QSPI_RANGE_OFFSET_S  (0x0)

/** ISRAM0 -- 2 MB*/
#define MPC_ISRAM0_RANGE_BASE_NS   (ISRAM0_BASE_NS)
#define MPC_ISRAM0_RANGE_LIMIT_NS  (ISRAM0_BASE_NS + ISRAM0_SIZE-1)
#define MPC_ISRAM0_RANGE_OFFSET_NS (0x0)
#define MPC_ISRAM0_RANGE_BASE_S    (ISRAM0_BASE_S)
#define MPC_ISRAM0_RANGE_LIMIT_S   (ISRAM0_BASE_S + ISRAM0_SIZE-1)
#define MPC_ISRAM0_RANGE_OFFSET_S  (0x0)

/** ISRAM1 -- 2 MB */
#define MPC_ISRAM1_RANGE_BASE_NS   (ISRAM1_BASE_NS)
#define MPC_ISRAM1_RANGE_LIMIT_NS  (ISRAM1_BASE_NS + ISRAM1_SIZE-1)
#define MPC_ISRAM1_RANGE_OFFSET_NS (0x0)
#define MPC_ISRAM1_RANGE_BASE_S    (ISRAM1_BASE_S)
#define MPC_ISRAM1_RANGE_LIMIT_S   (ISRAM1_BASE_S + ISRAM1_SIZE-1)
#define MPC_ISRAM1_RANGE_OFFSET_S  (0x0)

/** DDR4 -- 2GB (8 * 256 MB) */
#define MPC_DDR4_BLK0_RANGE_BASE_NS   (DDR4_BLK0_BASE_NS)
#define MPC_DDR4_BLK0_RANGE_LIMIT_NS  (DDR4_BLK0_BASE_NS + ((DDR4_BLK_SIZE)-1))
#define MPC_DDR4_BLK0_RANGE_OFFSET_NS (0x0)
#define MPC_DDR4_BLK1_RANGE_BASE_S    (DDR4_BLK1_BASE_S)
#define MPC_DDR4_BLK1_RANGE_LIMIT_S   (DDR4_BLK1_BASE_S + ((DDR4_BLK_SIZE)-1))
#define MPC_DDR4_BLK1_RANGE_OFFSET_S  (DDR4_BLK1_BASE_S - DDR4_BLK0_BASE_NS)
#define MPC_DDR4_BLK2_RANGE_BASE_NS   (DDR4_BLK2_BASE_NS)
#define MPC_DDR4_BLK2_RANGE_LIMIT_NS  (DDR4_BLK2_BASE_NS + ((DDR4_BLK_SIZE)-1))
#define MPC_DDR4_BLK2_RANGE_OFFSET_NS (DDR4_BLK2_BASE_NS - DDR4_BLK0_BASE_NS)
#define MPC_DDR4_BLK3_RANGE_BASE_S    (DDR4_BLK3_BASE_S)
#define MPC_DDR4_BLK3_RANGE_LIMIT_S   (DDR4_BLK3_BASE_S + ((DDR4_BLK_SIZE)-1))
#define MPC_DDR4_BLK3_RANGE_OFFSET_S  (DDR4_BLK3_BASE_S - DDR4_BLK0_BASE_NS)
#define MPC_DDR4_BLK4_RANGE_BASE_NS   (DDR4_BLK4_BASE_NS)
#define MPC_DDR4_BLK4_RANGE_LIMIT_NS  (DDR4_BLK4_BASE_NS + ((DDR4_BLK_SIZE)-1))
#define MPC_DDR4_BLK4_RANGE_OFFSET_NS (DDR4_BLK4_BASE_NS - DDR4_BLK0_BASE_NS)
#define MPC_DDR4_BLK5_RANGE_BASE_S    (DDR4_BLK5_BASE_S)
#define MPC_DDR4_BLK5_RANGE_LIMIT_S   (DDR4_BLK5_BASE_S + ((DDR4_BLK_SIZE)-1))
#define MPC_DDR4_BLK5_RANGE_OFFSET_S  (DDR4_BLK5_BASE_S - DDR4_BLK0_BASE_NS)
#define MPC_DDR4_BLK6_RANGE_BASE_NS   (DDR4_BLK6_BASE_NS)
#define MPC_DDR4_BLK6_RANGE_LIMIT_NS  (DDR4_BLK6_BASE_NS + ((DDR4_BLK_SIZE)-1))
#define MPC_DDR4_BLK6_RANGE_OFFSET_NS (DDR4_BLK6_BASE_NS - DDR4_BLK0_BASE_NS)
#define MPC_DDR4_BLK7_RANGE_BASE_S    (DDR4_BLK7_BASE_S)
#define MPC_DDR4_BLK7_RANGE_LIMIT_S   (DDR4_BLK7_BASE_S + ((DDR4_BLK_SIZE)-1))
#define MPC_DDR4_BLK7_RANGE_OFFSET_S  (DDR4_BLK7_BASE_S - DDR4_BLK0_BASE_NS)

#endif /*  MEM_REGION_DEFS_H  */
