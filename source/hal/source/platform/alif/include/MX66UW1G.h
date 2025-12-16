/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 * @file     MX66UW1G.h
 * @author   Manoj A Murudi
 * @email    manoj.murudi@alifsemi.com
 * @version  V1.0.0
 * @date     10-Dec-2025
 * @brief    Header file for MX66UW1G flash.
 ******************************************************************************/

#ifndef MX66UW1G_H_
#define MX66UW1G_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Number of sectors */
#define FLASH_MX66UW1G_SECTOR_COUNT                     ((uint32_t) 65536)
/* Sector size: 4kB */
#define FLASH_MX66UW1G_SECTOR_SIZE                      ((uint32_t) 4096)
/* Programming page size in bytes */
#define FLASH_MX66UW1G_PAGE_SIZE                        ((uint32_t) 256)
/* Smallest programmable unit in bytes */
#define FLASH_MX66UW1G_PROGRAM_UNIT                     ((uint32_t) 2)
/* Contents of erased memory */
#define FLASH_MX66UW1G_ERASED_VALUE                     ((uint8_t) 0xFF)

#ifdef __cplusplus
}
#endif

#endif /* MX66UW1G_H_ */
