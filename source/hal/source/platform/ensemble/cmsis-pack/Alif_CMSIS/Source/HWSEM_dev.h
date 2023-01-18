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
 * @file     HWSEM_dev.h
 * @author   Khushboo Singh
 * @email    khushboo.singh@alifsemi.com
 * @version  V1.0.0
 * @date     16-June-2022
 * @brief    Device Specific Header file for hardware semaphore driver.
 ******************************************************************************/

#ifndef HWSEM_DEV_H_
#define HWSEM_DEV_H_

/* System Includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include "Driver_Common.h"
#include "Driver_HWSEM.h"

#include CMSIS_device_header

#ifdef  __cplusplus
extern "C"
{
#endif

/* Hardware Sem IDs */
typedef enum _HWSEM_ID
{
	HWSEMID0,
	HWSEMID1,
	HWSEMID2,
	HWSEMID3,
	HWSEMID4,
	HWSEMID5,
	HWSEMID6,
	HWSEMID7,
	HWSEMID8,
	HWSEMID9,
	HWSEMID10,
	HWSEMID11,
	HWSEMID12,
	HWSEMID13,
	HWSEMID14,
	HWSEMID15
}HWSEM_ID;

/* Defines for HWSEM flags */
#define HWSEM_INITIALIZED                              1U

/** \brief Hw Sem register map. */
typedef struct {
	__IOM uint32_t ACQ_SEM;
	__IOM uint32_t REL_SEM;
	__OM uint32_t RESET_SEM;
}HWSEM_REGS;

/** \brief Representation of Hw Semaphore. */
typedef struct {
	HWSEM_REGS *regs;                   /**< IOMEM base address of the Hw Semaphore instance */
	ARM_HWSEM_SignalEvent_t cb_event;   /**< Registered callback function */
	IRQn_Type irq;                      /**< IRQ number of the HSEM instance */
	uint8_t flags;                      /**< HSEM driver flags */
	uint8_t irq_priority;               /**< IRQ priority can be modified by user */
	uint8_t sem_id;                     /**< HWSEM ID */
}HWSEM_DEV;

#ifdef  __cplusplus
}
#endif
#endif /* HWSEM_DEV_H_ */
