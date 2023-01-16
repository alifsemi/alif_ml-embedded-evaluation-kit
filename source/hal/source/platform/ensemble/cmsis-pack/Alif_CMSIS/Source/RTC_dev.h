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
 * @file     RTC_dev.h
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     18-Feb-2021
 * @brief    Device Specific Header file for RTC Driver.
 ******************************************************************************/

#ifndef RTC_DEV_H_
#define RTC_DEV_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/* Includes --------------------------------------------------------------------------- */
/* System Includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

/* Project Includes */
#include "Driver_RTC.h"

#define INT_MASK                true           /* Interrupt MASK    */
#define INT_UNMASK              false          /* Interrupt UNMASK  */

/* CCR register fields */
#define CCR_RTC_PSCLR_EN        (1 << 4)       /* Enable prescaler  */
#define CCR_RTC_WEN             (1 << 3)       /* Wrap enable       */
#define CCR_RTC_EN              (1 << 2)       /* Enable counter    */
#define CCR_RTC_MASK            (1 << 1)       /* Mask interrupts   */
#define CCR_RTC_IEN             (1 << 0)       /* Enable interrupts */

/* STAT register fields */
#define STAT_RTC_STAT           (1 << 0)       /* Interrupt status  */

/* RSTAT register fields */
#define RSTAT_RTC_RSTAT         (1 << 0)       /* Interrupt status  */


typedef enum {
	RTC_FLAG_DRV_INIT_DONE    = (1U << 0),     /* RTC Driver is Initialized */
	RTC_FLAG_DRV_POWER_DONE   = (1U << 1),     /* RTC Driver is Powered     */
	RTC_FLAG_DRV_ALARM_SET    = (1U << 2),     /* RTC Driver Alarm set      */
} RTC_FLAG_Type;


/*
 \brief struct RTC_TypeDef:- Register map for RTC
 */
typedef struct _RTC_TypeDef {
	__IM  uint32_t CCVR;                    /* Current Counter value register at 0x0        */
	__IOM uint32_t CMR;                     /* Counter Match register at 0x4                */
	__IOM uint32_t CLR;                     /* Counter Load Register at 0x8                 */
	__IOM uint32_t CCR;                     /* Counter Control Register at 0xC              */
	__IM  uint32_t STAT;                    /* Interrupt Status Register at 0x10            */
	__IM  uint32_t RSTAT;                   /* Interrupt Raw Status Register at 0x14        */
	__IM  uint32_t EOI;                     /* End of Interrupt Register at 0x18            */
	__IM  uint32_t COMP_VERSION;            /* Component Version Register at 0x1C           */
	__IOM uint32_t CPSR;                    /* Counter Prescaler Register at 0x20           */
	__IM  uint32_t CPCVR;                   /* Current Prescaler Counter Value Reg at 0x24  */
}RTC_TypeDef;

/*
 * struct RTC_DRV_INFO: structure representing a RTC device
 *
 * @cb_event        : Application Event callback \ref ARM_RTC_SignalEvent_t
 * @paddr           : Physical address of the RTC
 * @flags           : RTC driver flags
 * @prescaler_value : With the current input clock of 32.768kHz,
 *                    program the prescaler with 32768 to get an effective
 *                    counter frequency of 1 Hz.
 * @irq_num         : RTC interrupt vector number
 * @irq_priority    : RTC interrupt priority
 */
typedef struct _RTC_DRV_INFO {
	ARM_RTC_SignalEvent_t  cb_event;          /* RTC Application Event callback   */
	RTC_TypeDef           *paddr;             /* RTC physical address             */
	uint8_t                flags;             /* RTC Driver Flags                 */
	uint32_t               prescaler_value;   /* RTC default prescaler value      */
	IRQn_Type              irq_num;           /* RTC interrupt vector number      */
	uint32_t               irq_priority;      /* RTC interrupt priority           */
}RTC_DRV_INFO;

#ifdef  __cplusplus
}
#endif

#endif /* RTC_DEV_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
