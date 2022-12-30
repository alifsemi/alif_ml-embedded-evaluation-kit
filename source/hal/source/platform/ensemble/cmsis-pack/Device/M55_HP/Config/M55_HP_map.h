/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef M55_HP_MAP_H
#define M55_HP_MAP_H

#include "global_map.h"
/******************************************************************************/
/*                         Local memory map                              */
/******************************************************************************/
/* SRAM2 is mapped as ITCM */
#define ITCM_BASE                 (0x00000000UL)
#define ITCM_SIZE                 (SRAM2_SIZE) /* 256K */
#define ITCM_GLOBAL_BASE          (SRAM2_BASE)

/* SRAM3 is mapped as DTCM */
#define DTCM_BASE                 (0x20000000UL)
#define DTCM_SIZE                 (SRAM3_SIZE) /* 1MB */
#define DTCM_GLOBAL_BASE          (SRAM3_BASE)

/* NS REGIONs, should be 16KB aligned */
#define NS_REGION_0_BASE          (0x200E4000)
#define NS_REGION_0_END           (0x200FBFFF)
#define NS_REGION_0_SIZE          (NS_REGION_0_END - NS_REGION_0_BASE + 1) /* 96K */

#define LOCAL_PERIPHERAL_BASE     (0x40000000UL)

#define LOCAL_WDT_CTRL_BASE       (LOCAL_PERIPHERAL_BASE + 0x00100000)
#define LOCAL_WDT_REFRESH_BASE    (LOCAL_PERIPHERAL_BASE + 0x00101000)

#define DMA1_SE_BASE              (LOCAL_PERIPHERAL_BASE + 0xC0000)
#define DMA1_NS_BASE              (LOCAL_PERIPHERAL_BASE + 0xE0000)

#endif /* M55_HP_MAP_H */
