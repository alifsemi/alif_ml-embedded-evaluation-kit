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
 * @file     Driver_HWSEM.h
 * @author   Khushboo Singh
 * @email    khushboo.singh@alifsemi.com
 * @version  V1.0.0
 * @date     16-June-2022
 * @brief    Header file for hardware semaphore driver.
 ******************************************************************************/

#ifndef DRIVER_HWSEM_H_
#define DRIVER_HWSEM_H_

/* Driver Includes */
#include "Driver_Common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define ARM_HWSEM_API_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /* API version */

#define _ARM_Driver_HWSEM_(n)      DRIVER_HWSEM##n
#define  ARM_Driver_HWSEM_(n) _ARM_Driver_HWSEM_(n)

/* HWSEM Event */
#define HWSEM_AVAILABLE_CB_EVENT                         (1U << 0)

/* Call back event */
typedef void (*ARM_HWSEM_SignalEvent_t) (int32_t event, uint8_t sem_id);

/**
  \brief RTC Device Driver Capabilities.
 */
typedef struct _ARM_HWSEM_CAPABILITIES {
	uint32_t event_device_available      : 1;      ///< supports HWSEM Event Callback
	uint32_t reserved                    : 31;     ///< Reserved (must be zero)
} ARM_HWSEM_CAPABILITIES;

/**
  \brief Access structure of the Hardware Semaphore Driver
 */
typedef struct _ARM_DRIVER_HWSEM {
	ARM_DRIVER_VERSION       (*GetVersion)          (void);                               ///< Pointer to \ref ARM_HWSEM_GetVersion : Get driver version.
	ARM_HWSEM_CAPABILITIES   (*GetCapabilities)     (void);                               ///< Pointer to \ref ARM_HWSEM_GetCapabilities : Get driver capabilities.
	int32_t                  (*Initialize)          (ARM_HWSEM_SignalEvent_t cb_event);   ///< Pointer to \ref ARM_HWSEM_Initialize : Initialize HW semaphore Device.
	int32_t                  (*Uninitialize)        (void);                               ///< Pointer to \ref ARM_HWSEM_Uninitialize : De-initialize HW Semaphore Device.
	int32_t                  (*Lock)                (void);                               ///< Pointer to \ref ARM_HWSEM_Lock : Get semaphore lock
	int32_t                  (*UnLock)              (void);                               ///< Pointer to \ref ARM_HWSEM_UnLock : Release Hw semaphore
	int32_t                  (*GetCount)            (void);                               ///< Pointer to \ref ARM_HWSEM_GetCount : Get semaphore count
}const ARM_DRIVER_HWSEM;

#ifdef  __cplusplus
}
#endif
#endif /* DRIVER_HWSEM_H_ */
