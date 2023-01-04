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
 * @file     Flash_MRAM_dev.h
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     23-June-2022
 * @brief    Device Specific Header file for Flash(On-Chip NVM MRAM) Driver.
 ******************************************************************************/

#ifndef FLASH_MRAM_DEV_H__
#define FLASH_MRAM_DEV_H__

#ifdef __cplusplus
extern "C" {
#endif

/* System Includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

/* Project Includes */
#include "Driver_Flash.h"

/**
\brief Flash(On-Chip NVM MRAM) Address:
*/
#define FLASH_MRAM_BASE            MRAM_BASE
#define FLASH_MRAM_SIZE            RTE_FLASH_MRAM_SIZE

#define FLASH_MRAM_ADDR_ALIGN_MASK  (uint32_t)(0xFFFFFFF0)

/**
\brief Flash(On-Chip NVM MRAM) information: \ref ARM_FLASH_INFO
*/
#define FLASH_MRAM_SECTOR_INFO     (NULL)
#define FLASH_MRAM_SECTOR_COUNT    (FLASH_MRAM_SIZE >> 4)
#define FLASH_MRAM_SECTOR_SIZE     (0x10)
#define FLASH_MRAM_PAGE_SIZE       (0x10)
#define FLASH_MRAM_PROGRAM_UNIT    (0x10)
#define FLASH_MRAM_ERASED_VALUE    (0xFF)

#define FLASH_MRAM_FLAG_INIT       (1 << 0)   /* Driver initialized */
#define FLASH_MRAM_FLAG_POWER      (1 << 1)   /* Driver power on    */

/**
\brief Flash(On-Chip NVM MRAM) Device Resources
*/
typedef struct _FLASH_MRAM_DEV
{
  ARM_FLASH_INFO    *info;     /* Flash information.  */
  ARM_FLASH_STATUS  status;    /* Flash status.       */
  uint8_t           flags;     /* Flash driver flags. */
} FLASH_MRAM_DEV;

#ifdef __cplusplus
}
#endif

#endif /* FLASH_MRAM_DEV_H__ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
