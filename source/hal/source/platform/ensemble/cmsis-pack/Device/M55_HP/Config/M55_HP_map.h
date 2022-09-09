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
 * Project:      Memory Map for M55_HP_map.h
 * -------------------------------------------------------------------------- */

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
