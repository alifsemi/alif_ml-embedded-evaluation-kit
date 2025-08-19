/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     MX66UM2G.h
 * @author   Kevin Bracey
 * @email    kevin.bracey@alifsemi.com
 * @version  V1.0.0
 * @date     01-Oct-2024
 * @brief    Device Specific Header file for MX66UM2G FLASH Driver.
 ******************************************************************************/

#ifndef __MX66UM2G_H__
#define __MX66UM2G_H__

#ifdef  __cplusplus
extern "C"
{
#endif

#define FLASH_MX66UM2G_DO_DDR                           0
#define FLASH_MX66UM2G_DO_BLOCK_ERASE                   1

#if FLASH_MX66UM2G_DO_BLOCK_ERASE
#define FLASH_MX66UM2G_SECTOR_COUNT                     ((uint32_t)4096)    /* Number of sectors */
#define FLASH_MX66UM2G_SECTOR_SIZE                      ((uint32_t)65536)   /* Sector size: 64kB */
#else
#define FLASH_MX66UM2G_SECTOR_COUNT                     ((uint32_t)65536)   /* Number of sectors */
#define FLASH_MX66UM2G_SECTOR_SIZE                      ((uint32_t)4096)    /* Sector size: 4kB */
#endif
#define FLASH_MX66UM2G_PAGE_SIZE                        ((uint32_t)256)     /* Programming page size in bytes */
#define FLASH_MX66UM2G_PROGRAM_UNIT                     ((uint32_t)2)       /* Smallest programmable unit in bytes */
#define FLASH_MX66UM2G_ERASED_VALUE                     ((uint8_t)0xFF)     /* Contents of erased memory */

#ifdef __cplusplus
}
#endif

#endif
