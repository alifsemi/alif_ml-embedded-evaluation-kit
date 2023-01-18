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
 * @file     CRC_dev.h
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     18-april-2022
 * @brief    Device Specific Header file for CRC Driver.
 ******************************************************************************/

#ifndef CRC_DEV_H_
#define CRC_DEV_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/* System includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

#define REFLECT_CRC             (1 << 11)   /* To Reflect the CRC value */
#define INVERT_CRC              (1 << 10)   /* To Invert the CRC value */
#define CUSTOM_POLY             (1 << 9)    /* To enable the poly custom */
#define BIT_SWAP                (1 << 8)    /* To enable the Bit swap  */
#define BYTE_SWAP               (1 << 7)    /* To enable the Byte swap */
#define CRC_8_CCITT             (0 << 3)    /* To select the CRC_8_CCITT */
#define CRC_16                  (2 << 3)    /* To select the CRC_16 */
#define CRC_16_CCITT            (3 << 3)    /* To select the CRC_16_CCITT */
#define CRC_32                  (4 << 3)    /* To select the CRC_32 */
#define CRC_32C                 (5 << 3)    /* To select the CRC_32C */
#define ALGO_8_BIT_SIZE         (0 << 1)    /* To select the 8 bit algorithm size */
#define ALGO_16_BIT_SIZE        (1 << 1)    /* To select the 16 bit algorithm size */
#define ALGO_32_BIT_SIZE        (2 << 1)    /* To select the 32 bit algorithm size */
#define CRC_INIT_BIT            (1 << 0)    /* To select the init value */
#define ALGORITHM_CHECK         (3 << 1)    /* To check for the CRC algorithm */
#define STANDARD_POLY           0x04C11DB7  /* Standard polynomial for 32 bit CRC */

#define ALGO_SEL                (0xF << 3)  /* To clear algorithm select */
#define ALGO_SIZE               (0X3 << 1)  /* To clear the algorithm size */
#define CRC_8_BIT_SIZE           0          /* To select the 8 bit algorithm size */
#define CRC_16_BIT_SIZE          1          /* To select the 16 bit algorithm size */
#define CRC_32_BIT_SIZE          2          /* To select the 32 bit algorithm size */


/**
 @brief   : CRC flags to check the CRC initialization and CRC power done.
 */
typedef enum {
    CRC_FLAG_DRV_INIT_DONE    = (1U << 0),  /*CRC Driver is Initialized */
    CRC_FLAG_DRV_POWER_DONE   = (1U << 1),  /*CRC Driver is Powered */
} CRC_FLAG_Type;

/**
 @brief struct CRC_TypeDef:- Register map for CRC
 */
typedef struct _CRC_typedef
{
    __IOM uint32_t CONTROL;          /* control at (0x00)*/
          uint32_t reserved1[3];     /*reserved at (0x04) to (0x0c)*/
    __IOM uint32_t SEED;             /* seed value by defalut for 32 bit 0xFFFF_FFFF*/
    __IOM uint32_t POLY_CUSTOM;      /* customize polynomial used for 8 bit CRC's*/
    __IM  uint32_t CRC_OUT;          /* Accumulated CRC used for 8 bit CRC */
          uint32_t reserved2;        /* reserved at (0x1c) */
    __OM  uint8_t  DATA_IN_8[64];    /* Data written in this range is for 8 bit and 16 bit calculations , but only 8 bits can be written */
    __OM  uint32_t DATA_IN_32[39];   /* Data written here is for 32 bit CRC calculations */

} CRC_typedef;

/**
 * struct CRC_resources: structure representing a CRC device
 * @reg_base           : Register address of the CRC
 * @flags              : CRC driver flags
 */
typedef struct CRC_resources
{
    CRC_typedef    *reg_base; /*CRC register address */
    uint8_t        flags;     /*CRC Driver Flags */
}CRC_resources_t;

#endif /* CRC_DEV_H_ */
