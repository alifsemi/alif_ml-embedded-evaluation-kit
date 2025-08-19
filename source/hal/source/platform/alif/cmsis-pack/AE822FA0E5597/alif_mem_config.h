/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef __LINKER_MEM_CONFIG_H
#define __LINKER_MEM_CONFIG_H

/*******************************
 *      SRAM memory maps      *
 * ****************************/
#ifdef USE_STRIPED_SRAM
#define ALIF_SRAM_BASE          0x03000000 /* Use striped SRAMs  */
#define UNSTRIPED_BASE          0x02380000
#else
#define ALIF_SRAM_BASE          0x02000000 /* Use Linear SRAMs */
#define UNSTRIPED_BASE          0x02700000
#endif // USE_STRIPED_SRAM

/* Whole SRAM is 8MB */
#define ALIF_SRAM_SIZE          0x00700000
#define UNSTRIPED_SIZE          0x00100000 /* All SRAM is unstriped with Linear, but just to keep same linker files with both configs */

#endif // __LINKER_MEM_CONFIG_H
