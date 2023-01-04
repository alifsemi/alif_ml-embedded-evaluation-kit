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
 * @file     WDT_dev.h
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     27-April-2021
 * @brief    Device Specific Header file for Wachdog Driver.
 ******************************************************************************/

#ifndef WDT_DEV_H_
#define WDT_DEV_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/* Includes --------------------------------------------------------------------------- */
/* System Includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header


/* Watchdog control definitions */
#define WATCHDOG_CTRL_INTEN        (0x1UL << 0)
#define WATCHDOG_CTRL_RESEN        (0x1UL << 1)
#define WATCHDOG_INTCLR            1UL
#define WATCHDOG_RAWINTSTAT        1UL
#define WATCHDOG_MASKINTSTAT       1UL
#define WATCHDOG_UNLOCK_VALUE      0x1ACCE551
#define WATCHDOG_LOCK_VALUE        0x00000001
#define WATCHDOG_INTEGTESTEN       1UL
#define WATCHDOG_INTEGTESTOUTSET   1UL
#define WATCHDOG_MAX_VALUE         0xFFFFFFFF

/* Watchdog state definitions */
#define WATCHDOG_INITIALIZED       (1ul << 0)
#define WATCHDOG_STARTED           (1ul << 1)
#define WATCHDOG_POWERED           (1ul << 2)


/*
 \brief struct WDT_TypeDef:-  Watchdog memory mapped register access structure
 */
typedef struct _WDT_TypeDef {
    __IOM uint32_t  load;           /* Offset: 0x000 (R/W) Load register       */
    __IM uint32_t   value;          /* Offset: 0x004 (R/ ) Value register      */
    __IOM uint32_t  ctrl;           /* Offset: 0x008 (R/W) Control register    */
    __OM uint32_t   intclr;         /* Offset: 0x00C ( /W) Clear interrupt
                                       *                     register          */
    __IM uint32_t   rawintstat;     /* Offset: 0x010 (R/ ) Raw interrupt
                                       *                     status register   */
    __IM uint32_t   maskintstat;    /* Offset: 0x014 (R/ ) Interrupt status
                                       *                     register          */
    uint32_t        reserved0[762];
    __IOM uint32_t  lock;           /* Offset: 0xC00 (R/W) Lock register       */
    uint32_t        reserved1[191];
    __IOM uint32_t  itcr;           /* Offset: 0xF00 (R/W) Integration test
                                       *                     control register  */
    __OM uint32_t   itop;           /* Offset: 0xF04 ( /W) Integration Test
                                       *                     output set
                                       *                     register          */
}WDT_TypeDef;


/*
 * struct WDT_DRV_INFO: structure representing a Watchdog device
 * @paddr           : Physical address of the Watchdog
 * @flags           : Watchdog driver flags
 * @timeout         : Timeout in msec, must be > 0.
 */
typedef struct _WDT_DRV_INFO {
    WDT_TypeDef           *paddr;             /* Watchdog physical address             */
    uint8_t                flags;             /* Watchdog Driver Flags                 */
    uint32_t               timeout;           /* Watchdog Timeout to reset in msec   */
}WDT_DRV_INFO;

#ifdef  __cplusplus
}
#endif

#endif /* WDT_DEV_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
