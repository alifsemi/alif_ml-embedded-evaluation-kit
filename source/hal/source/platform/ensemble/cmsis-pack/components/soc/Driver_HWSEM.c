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
 * @file     Driver_HWSEM.c
 * @author   Khushboo Singh
 * @email    khushboo.singh@alifsemi.com
 * @version  V1.0.0
 * @date     16-June-2022
 * @brief    ARM CMSIS-Driver for Hardware Semaphore.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* Driver specific include */
#include "HWSEM_dev.h"

#define ARM_HWSEM_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* Driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
	ARM_HWSEM_API_VERSION,
	ARM_HWSEM_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_HWSEM_CAPABILITIES DriverCapabilities =
{
	1,         /* Supports HWSEM callback */
	0
};


/**
  \fn           static ARM_DRIVER_VERSION Hwsem_GetVersion(void)
  \brief        Returns the Driver Version
  \param[in]    void
  \return       ARM_DRIVER_VERSION : Driver version
 */
static ARM_DRIVER_VERSION Hwsem_GetVersion(void)
{
	return DriverVersion;
}

/**
  \fn           static ARM_HWSEM_CAPABILITIES Hwsem_GetCapabilities(void)
  \brief        Returns the Driver Capabilities
  \param[in]    void
  \return       ARM_HWSEM_CAPABILITIES : Driver Capabilities
 */
static ARM_HWSEM_CAPABILITIES Hwsem_GetCapabilities(void)
{
	return DriverCapabilities;
}

/**
  \fn           static int32_t Initialize(ARM_HWSEM_SignalEvent_t cb_event,
                uint8_t HWSEM_DEV *dev)
  \brief        Initializes the driver instance with semaphore id and call back event
  \param[in]    cb_event : call back function provided by user
  \param[in]    sem_id   : Semaphore id provided by user
  \param[in]    dev      : Pointer to Resources of the particular driver instance
  \return       ARM_DRIVER_OK : If no error is there
  \             ARM_DRIVER_ERROR_BUSY : Already initialized
 */
static int32_t Initialize(ARM_HWSEM_SignalEvent_t cb_event, HWSEM_DEV * dev)
{
	/* Confirm that driver is not already in use */
	if(dev->flags == HWSEM_INITIALIZED)
		return ARM_DRIVER_ERROR_BUSY;

	if(cb_event != NULL)
	{
		dev->cb_event = cb_event;
	}

	dev->flags |= HWSEM_INITIALIZED;

	return ARM_DRIVER_OK;
}

/**
  \fn           static int32_t Uninitialize(uint8_t HWSEM_DEV *dev)
  \brief        Uninitializes the driver instance
  \param[in]    sem_id   : Semaphore id provided by user
  \param[in]    dev      : Pointer to Resources of the particular driver instance
  \return       ARM_DRIVER_OK : If no error, otherwise ARM_DRIVER_ERROR_PARAMETER
 */
static int32_t Uninitialize(HWSEM_DEV *dev)
{
	/* Confirm that driver is initialized */
	if(!(dev->flags & HWSEM_INITIALIZED))
		return ARM_DRIVER_ERROR;

	dev->cb_event = NULL;
	dev->flags &= ~HWSEM_INITIALIZED;

	return ARM_DRIVER_OK;
}

/**
  \fn           static int32_t Lock(uint8_t HWSEM_DEV *dev)
  \brief        Acquire lock of the semaphore
  \param[in]    sem_id   : Semaphore id provided by user
  \param[in]    dev      : Pointer to Resources of the particular driver instance
  \return       ARM_DRIVER_OK : If no error
  \             ARM_DRIVER_ERROR_BUSY : if semaphore is not available
  \             ARM_DRIVER_ERROR : If Driver is not initialized
 */
static int32_t Lock(HWSEM_DEV *dev)
{
	/* Confirm that driver is initialized */
	if(!(dev->flags & HWSEM_INITIALIZED))
		return ARM_DRIVER_ERROR;

	/* Try to acquire semaphore by writing Master ID to the register */
	dev->regs->ACQ_SEM = HWSEM_MASTERID ;

	/* Read the register to check if the semaphore is acquired */
	if(dev->regs->ACQ_SEM == HWSEM_MASTERID)
	{
		return ARM_DRIVER_OK;
	}

	/* If the semaphore is not available, enable the IRQ */
	if(dev->cb_event != NULL)
	{
		/* Enable Hw Sem IRQ*/
		NVIC_ClearPendingIRQ(dev->irq);
		NVIC_SetPriority(dev->irq, dev->irq_priority);
		NVIC_EnableIRQ(dev->irq);
	}
	return ARM_DRIVER_ERROR_BUSY;
}

/**
  \fn           static int32_t UnLock(uint8_t HWSEM_DEV *dev)
  \brief        Release the semaphore
  \param[in]    sem_id   : Semaphore id provided by user
  \param[in]    dev      : Pointer to Resources of the particular driver instance
  \return       ARM_DRIVER_OK : If no error
  \             ARM_DRIVER_ERROR : if semaphore id is not same or driver is not initialized
 */
static int32_t UnLock(HWSEM_DEV *dev)
{
	/* Confirm that driver is initialized */
	if(!(dev->flags & HWSEM_INITIALIZED))
		return ARM_DRIVER_ERROR;

	/* Check if semaphore is locked */
	if(dev->regs->REL_SEM > 0)
	{
		/* Release the semaphore */
		dev->regs->REL_SEM = HWSEM_MASTERID;
		return ARM_DRIVER_OK;
	}
	else
	{
		return ARM_DRIVER_ERROR;
	}
}
/**
  \fn           static uint32_t GetCount(uint8_t HWSEM_DEV * dev)
  \brief        Get the semaphore count
  \param[in]    sem_id   : Semaphore id provided by user
  \param[in]    dev      : Pointer to Resources of the particular driver instance
  \return       Semaphore count : If no error
  \             ARM_DRIVER_ERROR : if driver is not initialized
 */
static int32_t GetCount(HWSEM_DEV * dev)
{
	/* Confirm that driver is initialized */
	if(!(dev->flags & HWSEM_INITIALIZED))
		return ARM_DRIVER_ERROR;

	return dev->regs->REL_SEM;
}

/**
  \fn          void HWSEM_IRQHandler(HWSEM_DEV *dev)
  \brief       HWSEM instance specific part of Hw Semaphore Interrupt handler.
  \param[in]   dev    :   Pointer to the HWSEM device instance
 */
static void ARM_HWSEM_IRQHandler(HWSEM_DEV *dev)
{
	/* Disable Hw Sem IRQ*/
	NVIC_DisableIRQ(dev->irq);
	NVIC_ClearPendingIRQ(dev->irq);

	/* Call the user provided call back function */
	dev->cb_event(HWSEM_AVAILABLE_CB_EVENT, dev->sem_id);
}

/* HWSEM0 Driver Instance */
#if (RTE_HWSEM0)

/* HWSEM0 Resources */
static HWSEM_DEV HwSem0 = {
	.regs =(HWSEM_REGS *)HWSEM0_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM0_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM0_IRQPRIORITY,
	.sem_id = HWSEMID0,
};

/* HWSEM0 Interrupt Handler */
void HWSEM0_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem0);
}

static int32_t Hwsem0_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem0);
}

static int32_t Hwsem0_Uninitialize(void)
{
	return Uninitialize(&HwSem0);
}

static int32_t Hwsem0_Lock(void)
{
	return Lock(&HwSem0);
}

static int32_t Hwsem0_UnLock(void)
{
	return UnLock(&HwSem0);
}

static int32_t Hwsem0_GetCount(void)
{
	return GetCount(&HwSem0);
}

/* HWSEM0 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM0 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem0_Initialize ,
	Hwsem0_Uninitialize,
	Hwsem0_Lock,
	Hwsem0_UnLock,
	Hwsem0_GetCount
};

#endif

/* HSWEM1 Driver Instance */
#if (RTE_HWSEM1)

/* HWSEM1 Resources */
static HWSEM_DEV HwSem1 = {
	.regs =(HWSEM_REGS *)HWSEM1_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM1_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM1_IRQPRIORITY,
	.sem_id = HWSEMID1,
};

/* HWSEM1 Interrupt Handler */
void HWSEM1_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem1);
}

static int32_t Hwsem1_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem1);
}

static int32_t Hwsem1_Uninitialize(void)
{
	return Uninitialize(&HwSem1);
}

static int32_t Hwsem1_Lock(void)
{
	return Lock(&HwSem1);
}

static int32_t Hwsem1_UnLock(void)
{
	return UnLock(&HwSem1);
}

static int32_t Hwsem1_GetCount(void)
{
	return GetCount(&HwSem1);
}

/* HWSEM1 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM1 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem1_Initialize ,
	Hwsem1_Uninitialize,
	Hwsem1_Lock,
	Hwsem1_UnLock,
	Hwsem1_GetCount
};

#endif

/* HWSEM2 Driver Instance */
#if (RTE_HWSEM2)

/* HWSEM2 Resources */
static HWSEM_DEV HwSem2 = {
	.regs =(HWSEM_REGS *)HWSEM2_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM2_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM2_IRQPRIORITY,
	.sem_id = HWSEMID2,
};

/* HWSEM2 Interrupt Handler */
void HWSEM2_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem2);
}

static int32_t Hwsem2_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem2);
}

static int32_t Hwsem2_Uninitialize(void)
{
	return Uninitialize(&HwSem2);
}

static int32_t Hwsem2_Lock(void)
{
	return Lock(&HwSem2);
}

static int32_t Hwsem2_UnLock(void)
{
	return UnLock(&HwSem2);
}

static int32_t Hwsem2_GetCount(void)
{
	return GetCount(&HwSem2);
}

/* HWSEM2 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM2 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem2_Initialize ,
	Hwsem2_Uninitialize,
	Hwsem2_Lock,
	Hwsem2_UnLock,
	Hwsem2_GetCount
};

#endif

/* HWSEM3 Driver Instance */
#if (RTE_HWSEM3)

/* HWSEM3 Resources */
static HWSEM_DEV HwSem3 = {
	.regs =(HWSEM_REGS *)HWSEM3_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM3_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM3_IRQPRIORITY,
	.sem_id = HWSEMID3,
};

/* HWSEM3 Interrupt Handler */
void HWSEM3_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem3);
}

static int32_t Hwsem3_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem3);
}

static int32_t Hwsem3_Uninitialize(void)
{
	return Uninitialize(&HwSem3);
}

static int32_t Hwsem3_Lock(void)
{
	return Lock(&HwSem3);
}

static int32_t Hwsem3_UnLock(void)
{
	return UnLock(&HwSem3);
}

static int32_t Hwsem3_GetCount(void)
{
	return GetCount(&HwSem3);
}

/* HWSEM3 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM3 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem3_Initialize ,
	Hwsem3_Uninitialize,
	Hwsem3_Lock,
	Hwsem3_UnLock,
	Hwsem3_GetCount
};

#endif

/* HWSEM4 Driver Instance */
#if (RTE_HWSEM4)

/* HWSEM4 Resources */
static HWSEM_DEV HwSem4 = {
	.regs =(HWSEM_REGS *)HWSEM4_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM4_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM4_IRQPRIORITY,
	.sem_id = HWSEMID4,
};

/* HWSEM4 Interrupt Handler */
void HWSEM4_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem4);
}

static int32_t Hwsem4_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem4);
}

static int32_t Hwsem4_Uninitialize(void)
{
	return Uninitialize(&HwSem4);
}

static int32_t Hwsem4_Lock(void)
{
	return Lock(&HwSem4);
}

static int32_t Hwsem4_UnLock(void)
{
	return UnLock(&HwSem4);
}

static int32_t Hwsem4_GetCount(void)
{
	return GetCount(&HwSem4);
}

/* HWSEM4 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM4 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem4_Initialize ,
	Hwsem4_Uninitialize,
	Hwsem4_Lock,
	Hwsem4_UnLock,
	Hwsem4_GetCount
};

#endif

/* HWSEM5 Driver Instance */
#if (RTE_HWSEM5)

/* HWSEM5 Resources */
static HWSEM_DEV HwSem5 = {
	.regs =(HWSEM_REGS *)HWSEM5_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM5_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM5_IRQPRIORITY,
	.sem_id = HWSEMID5,
};

/* HWSEM5 Interrupt Handler */
void HWSEM5_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem5);
}

static int32_t Hwsem5_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem5);
}

static int32_t Hwsem5_Uninitialize(void)
{
	return Uninitialize(&HwSem5);
}

static int32_t Hwsem5_Lock(void)
{
	return Lock(&HwSem5);
}

static int32_t Hwsem5_UnLock(void)
{
	return UnLock(&HwSem5);
}

static int32_t Hwsem5_GetCount(void)
{
	return GetCount(&HwSem5);
}

/* HWSEM5 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM5 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem5_Initialize ,
	Hwsem5_Uninitialize,
	Hwsem5_Lock,
	Hwsem5_UnLock,
	Hwsem5_GetCount
};

#endif

/* HWSEM6 Driver Instance */
#if (RTE_HWSEM6)

/* HWSEM6 Resources */
static HWSEM_DEV HwSem6 = {
	.regs =(HWSEM_REGS *)HWSEM6_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM6_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM6_IRQPRIORITY,
	.sem_id = HWSEMID6,
};

/* HWSEM6 Interrupt Handler */
void HWSEM6_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem6);
}

static int32_t Hwsem6_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem6);
}

static int32_t Hwsem6_Uninitialize(void)
{
	return Uninitialize(&HwSem6);
}

static int32_t Hwsem6_Lock(void)
{
	return Lock(&HwSem6);
}

static int32_t Hwsem6_UnLock(void)
{
	return UnLock(&HwSem6);
}

static int32_t Hwsem6_GetCount(void)
{
	return GetCount(&HwSem6);
}

/* HWSEM6 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM6 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem6_Initialize ,
	Hwsem6_Uninitialize,
	Hwsem6_Lock,
	Hwsem6_UnLock,
	Hwsem6_GetCount
};

#endif

/* HWSEM0 Driver Instance */
#if (RTE_HWSEM7)

/* HWSEM7 Resources */
static HWSEM_DEV HwSem7 = {
	.regs =(HWSEM_REGS *)HWSEM7_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM7_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM7_IRQPRIORITY,
	.sem_id = HWSEMID7,
};

/* HWSEM7 Interrupt Handler */
void HWSEM7_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem7);
}

static int32_t Hwsem7_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem7);
}

static int32_t Hwsem7_Uninitialize(void)
{
	return Uninitialize(&HwSem7);
}

static int32_t Hwsem7_Lock(void)
{
	return Lock(&HwSem7);
}

static int32_t Hwsem7_UnLock(void)
{
	return UnLock(&HwSem7);
}

static int32_t Hwsem7_GetCount(void)
{
	return GetCount(&HwSem7);
}

/* HWSEM7 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM7 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem7_Initialize ,
	Hwsem7_Uninitialize,
	Hwsem7_Lock,
	Hwsem7_UnLock,
	Hwsem7_GetCount
};

#endif

/* HWSEM8 Driver Instance */
#if (RTE_HWSEM8)

/* HWSEM8 Resources */
static HWSEM_DEV HwSem8 = {
	.regs =(HWSEM_REGS *)HWSEM8_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM8_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM8_IRQPRIORITY,
	.sem_id = HWSEMID8,
};

/* HWSEM8 Interrupt Handler */
void HWSEM8_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem8);
}

static int32_t Hwsem8_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem8);
}

static int32_t Hwsem8_Uninitialize(void)
{
	return Uninitialize(&HwSem8);
}

static int32_t Hwsem8_Lock(void)
{
	return Lock(&HwSem8);
}

static int32_t Hwsem8_UnLock(void)
{
	return UnLock(&HwSem8);
}

static int32_t Hwsem8_GetCount(void)
{
	return GetCount(&HwSem8);
}

/* HWSEM8 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM8 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem8_Initialize ,
	Hwsem8_Uninitialize,
	Hwsem8_Lock,
	Hwsem8_UnLock,
	Hwsem8_GetCount
};

#endif

/* HWSEM9 Driver Instance */
#if (RTE_HWSEM9)

/* HWSEM9 Resources */
static HWSEM_DEV HwSem9 = {
	.regs =(HWSEM_REGS *)HWSEM9_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM9_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM9_IRQPRIORITY,
	.sem_id = HWSEMID9,
};

/* HWSEM9 Interrupt Handler */
void HWSEM9_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem9);
}

static int32_t Hwsem9_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem9);
}

static int32_t Hwsem9_Uninitialize(void)
{
	return Uninitialize(&HwSem9);
}

static int32_t Hwsem9_Lock(void)
{
	return Lock(&HwSem9);
}

static int32_t Hwsem9_UnLock(void)
{
	return UnLock(&HwSem9);
}

static int32_t Hwsem9_GetCount(void)
{
	return GetCount(&HwSem9);
}

/* HWSEM9 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM9 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem9_Initialize ,
	Hwsem9_Uninitialize,
	Hwsem9_Lock,
	Hwsem9_UnLock,
	Hwsem9_GetCount
};

#endif

/* HWSEM10 Driver Instance */
#if (RTE_HWSEM10)

/* HWSEM10 Resources */
static HWSEM_DEV HwSem10 = {
	.regs =(HWSEM_REGS *)HWSEM10_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM10_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM10_IRQPRIORITY,
	.sem_id = HWSEMID10,
};

/* HWSEM10 Interrupt Handler */
void HWSEM10_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem10);
}

static int32_t Hwsem10_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem10);
}

static int32_t Hwsem10_Uninitialize(void)
{
	return Uninitialize(&HwSem10);
}

static int32_t Hwsem10_Lock(void)
{
	return Lock(&HwSem10);
}

static int32_t Hwsem10_UnLock(void)
{
	return UnLock(&HwSem10);
}

static int32_t Hwsem10_GetCount(void)
{
	return GetCount(&HwSem10);
}

/* HWSEM10 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM10 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem10_Initialize ,
	Hwsem10_Uninitialize,
	Hwsem10_Lock,
	Hwsem10_UnLock,
	Hwsem10_GetCount
};

#endif

/* HWSEM11 Driver Instance */
#if (RTE_HWSEM11)

/* HWSEM11 Resources */
static HWSEM_DEV HwSem11 = {
	.regs =(HWSEM_REGS *)HWSEM11_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM11_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM11_IRQPRIORITY,
	.sem_id = HWSEMID11,
};

/* HWSEM11 Interrupt Handler */
void HWSEM11_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem11);
}

static int32_t Hwsem11_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem11);
}

static int32_t Hwsem11_Uninitialize(void)
{
	return Uninitialize(&HwSem11);
}

static int32_t Hwsem11_Lock(void)
{
	return Lock(&HwSem11);
}

static int32_t Hwsem11_UnLock(void)
{
	return UnLock(&HwSem11);
}

static int32_t Hwsem11_GetCount(void)
{
	return GetCount(&HwSem11);
}

/* HWSEM11 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM11 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem11_Initialize ,
	Hwsem11_Uninitialize,
	Hwsem11_Lock,
	Hwsem11_UnLock,
	Hwsem11_GetCount
};

#endif

/* HWSEM12 Driver Instance */
#if (RTE_HWSEM12)

/* HWSEM12 Resources */
static HWSEM_DEV HwSem12 = {
	.regs =(HWSEM_REGS *)HWSEM12_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM12_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM12_IRQPRIORITY,
	.sem_id = HWSEMID12,
};

/* HWSEM12 Interrupt Handler */
void HWSEM12_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem12);
}

static int32_t Hwsem12_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem12);
}

static int32_t Hwsem12_Uninitialize(void)
{
	return Uninitialize(&HwSem12);
}

static int32_t Hwsem12_Lock(void)
{
	return Lock(&HwSem12);
}

static int32_t Hwsem12_UnLock(void)
{
	return UnLock(&HwSem12);
}

static int32_t Hwsem12_GetCount(void)
{
	return GetCount(&HwSem12);
}

/* HWSEM12 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM12 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem12_Initialize ,
	Hwsem12_Uninitialize,
	Hwsem12_Lock,
	Hwsem12_UnLock,
	Hwsem12_GetCount
};

#endif

/* HWSEM13 Driver Instance */
#if (RTE_HWSEM13)

/* HWSEM13 Resources */
static HWSEM_DEV HwSem13 = {
	.regs =(HWSEM_REGS *)HWSEM13_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM13_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM13_IRQPRIORITY,
	.sem_id = HWSEMID13,
};

/* HWSEM13 Interrupt Handler */
void HWSEM13_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem13);
}

static int32_t Hwsem13_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem13);
}

static int32_t Hwsem13_Uninitialize(void)
{
	return Uninitialize(&HwSem13);
}

static int32_t Hwsem13_Lock(void)
{
	return Lock(&HwSem13);
}

static int32_t Hwsem13_UnLock(void)
{
	return UnLock(&HwSem13);
}

static int32_t Hwsem13_GetCount(void)
{
	return GetCount(&HwSem13);
}

/* HWSEM13 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM13 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem13_Initialize ,
	Hwsem13_Uninitialize,
	Hwsem13_Lock,
	Hwsem13_UnLock,
	Hwsem13_GetCount
};

#endif

/* HWSEM14 Driver Instance */
#if (RTE_HWSEM14)

/* HWSEM14 Resources */
static HWSEM_DEV HwSem14 = {
	.regs =(HWSEM_REGS *)HWSEM14_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM14_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM14_IRQPRIORITY,
	.sem_id = HWSEMID14,
};

/* HWSEM14 Interrupt Handler */
void HWSEM14_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem14);
}

static int32_t Hwsem14_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem14);
}

static int32_t Hwsem14_Uninitialize(void)
{
	return Uninitialize(&HwSem14);
}

static int32_t Hwsem14_Lock(void)
{
	return Lock(&HwSem14);
}

static int32_t Hwsem14_UnLock(void)
{
	return UnLock(&HwSem14);
}

static int32_t Hwsem14_GetCount(void)
{
	return GetCount(&HwSem14);
}

/* HWSEM14 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM14 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem14_Initialize ,
	Hwsem14_Uninitialize,
	Hwsem14_Lock,
	Hwsem14_UnLock,
	Hwsem14_GetCount
};

#endif

/* HWSEM15 Driver Instance */
#if (RTE_HWSEM15)

/* HWSEM15 Resources */
static HWSEM_DEV HwSem15 = {
	.regs =(HWSEM_REGS *)HWSEM15_BASE,
	.cb_event = NULL,
	.irq =(IRQn_Type) HWSEM15_IRQ,
	.irq_priority =(uint8_t)RTE_HWSEM15_IRQPRIORITY,
	.sem_id = HWSEMID15,
};

/* HWSEM15 Interrupt Handler */
void HWSEM15_IRQHandler(void)
{
	ARM_HWSEM_IRQHandler(&HwSem15);
}

static int32_t Hwsem15_Initialize(ARM_HWSEM_SignalEvent_t cb_event)
{
	return Initialize(cb_event, &HwSem15);
}

static int32_t Hwsem15_Uninitialize(void)
{
	return Uninitialize(&HwSem15);
}

static int32_t Hwsem15_Lock(void)
{
	return Lock(&HwSem15);
}

static int32_t Hwsem15_UnLock(void)
{
	return UnLock(&HwSem15);
}

static int32_t Hwsem15_GetCount(void)
{
	return GetCount(&HwSem15);
}

/* HWSEM15 Access Struct */
ARM_DRIVER_HWSEM DRIVER_HWSEM15 =
{
	Hwsem_GetVersion,
	Hwsem_GetCapabilities,
	Hwsem15_Initialize ,
	Hwsem15_Uninitialize,
	Hwsem15_Lock,
	Hwsem15_UnLock,
	Hwsem15_GetCount
};

#endif
