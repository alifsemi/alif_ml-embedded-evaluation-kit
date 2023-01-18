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
 * @file     Driver_RTC.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     18-Feb-2021
 * @brief    CMSIS-Driver for RTC
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* Includes --------------------------------------------------------------------------- */
/* Project Includes */
#include "Driver_RTC.h"
#include "RTC_dev.h"

#define ARM_RTC_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /* driver version */

#if !(RTE_RTC0)
#error "RTC is not enabled in the RTE_Device.h"
#endif

#if (defined(RTE_Drivers_RTC0) && !RTE_RTC0)
#error "RTC0 not configured in RTE_Device.h!"
#endif

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_RTC_API_VERSION,
    ARM_RTC_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_RTC_CAPABILITIES DriverCapabilities = {
    1,  /* supports RTC Alarm Callback */
    0   /* Reserved (must be zero) */
};


//
//   Functions
//

/**
  \fn			ARM_DRIVER_VERSION RTC_GetVersion(void)
  \brief		get rtc version
  \return		driver version
*/
static ARM_DRIVER_VERSION RTC_GetVersion(void)
{
  return DriverVersion;
}

/**
  \fn			ARM_RTC_CAPABILITIES RTC_GetCapabilities(void)
  \brief		get rtc capabilites
  \return		driver capabilites
*/
static ARM_RTC_CAPABILITIES RTC_GetCapabilities(void)
{
  return DriverCapabilities;
}

/**
  \fn			void rtc_counter_control (bool ctrl, RTC_DRV_INFO *rtc)
  \brief		Enable/Disable rtc counter
  \param[in]	ctrl   : rtc counter enable/disable
  \param[in]	rtc    : Pointer to rtc device resources
  \return		none
*/
__STATIC_INLINE void rtc_counter_control (bool ctrl, RTC_DRV_INFO *rtc)
{
	if(ctrl) /* Enbale */
	{
		rtc->paddr->CCR |= (CCR_RTC_EN);
	}
	else /* Disable */
	{
		rtc->paddr->CCR &= ~(CCR_RTC_EN);
	}
}

/**
  \fn			void rtc_prescaler_control (bool ctrl, RTC_DRV_INFO *rtc)
  \brief		Enable/Disable rtc prescaler counter
  \param[in]	ctrl   : rtc prescaler counter enable/disable
  \param[in]	rtc    : Pointer to rtc device resources
  \return		none
*/
__STATIC_INLINE void rtc_prescaler_control (bool ctrl, RTC_DRV_INFO *rtc)
{
	if(ctrl) /* Enbale */
	{
		rtc->paddr->CCR |= (CCR_RTC_PSCLR_EN);
	}
	else /* Disable */
	{
		rtc->paddr->CCR &= ~(CCR_RTC_PSCLR_EN);
	}
}

/**
  \fn			void rtc_counter_wrap_control (bool ctrl, RTC_DRV_INFO *rtc)
  \brief		Enable/Disable rtc counter wrap
  \param[in]	ctrl   : rtc counter wrap enable/disable
  \param[in]	rtc    : Pointer to rtc device resources
  \return		none
*/
__STATIC_INLINE void rtc_counter_wrap_control (bool ctrl, RTC_DRV_INFO *rtc)
{
	if(ctrl) /* Enbale */
	{
		rtc->paddr->CCR |= (CCR_RTC_WEN);
	}
	else /* Disable */
	{
		rtc->paddr->CCR &= ~(CCR_RTC_WEN);
	}
}

/**
  \fn			void rtc_interrupt_control (bool ctrl, RTC_DRV_INFO *rtc)
  \brief		Enable/Disable rtc interrupt generation
  \param[in]	ctrl   : rtc interrupt enable/disable
  \param[in]	rtc    : Pointer to rtc device resources
  \return		none
*/
__STATIC_INLINE void rtc_interrupt_control (bool ctrl, RTC_DRV_INFO *rtc)
{
	if(ctrl) /* Enbale */
	{
		rtc->paddr->CCR |= (CCR_RTC_IEN);
	}
	else /* Disable */
	{
		rtc->paddr->CCR &= ~(CCR_RTC_IEN);
	}
}

/**
  \fn			void rtc_interrupt_mask (bool mask, RTC_DRV_INFO *rtc)
  \brief		Mask/Unmask rtc interrupt generation
  \param[in]	mask : mask rtc interrupt
  \param[in]	rtc  : Pointer to rtc device resources
  \return		none
*/
__STATIC_INLINE void rtc_interrupt_mask (bool mask, RTC_DRV_INFO *rtc)
{
	if(mask) /* mask */
	{
		rtc->paddr->CCR |= (CCR_RTC_MASK);
	}
	else /* Unmask */
	{
		rtc->paddr->CCR &= ~(CCR_RTC_MASK);
	}
}

/**
  \fn			void rtc_interrupt_ack (RTC_DRV_INFO *rtc)
  \brief		Acknowledge rtc interrupt
  \param[in]	rtc   : Pointer to rtc device resources
  \return		none
*/
__STATIC_INLINE void rtc_interrupt_ack (RTC_DRV_INFO *rtc)
{
	/* read to clear match interrupt. */
	(void) (rtc->paddr->EOI);
}

/**
  \fn			void rtc_load_prescaler (uint32_t prescaler_value, RTC_DRV_INFO *rtc)
  \brief		Load rtc prescaler value
  \param[in]	prescaler_value : rtc prescaler value
  \param[in]	rtc             : Pointer to rtc device resources
  \return		none
*/
__STATIC_INLINE void rtc_load_prescaler (uint32_t prescaler_value, RTC_DRV_INFO *rtc)
{
	rtc->paddr->CPSR = prescaler_value;
}

/**
  \fn			void rtc_load_counter_match_register (uint32_t value, RTC_DRV_INFO *rtc)
  \brief		Interrupt match register,When the internal counter matches this register,
				an interrupt is generated, provided interrupt generation is enabled.
  \param[in]	value : rtc counter match register value
  \param[in]	rtc   : Pointer to rtc device resources
  \return		none
*/
__STATIC_INLINE void rtc_load_counter_match_register (uint32_t value, RTC_DRV_INFO *rtc)
{
	rtc->paddr->CMR = value;
}

/**
  \fn			uint32_t rtc_read_current_counter (RTC_DRV_INFO *rtc)
  \brief		Read rtc current counter value
  \param[in]	rtc   : Pointer to rtc device resources
  \return		rtc current counter value
*/
__STATIC_INLINE uint32_t rtc_read_current_counter (RTC_DRV_INFO *rtc)
{
	return rtc->paddr->CCVR;
}

/**
  \fn			int32_t rtc_read_counter (uint32_t *val, RTC_DRV_INFO *rtc)
  \brief		Read rtc current counter value
  \param[out]	val    : Pointer to the address where current rtc counter value
                         needs to be copied to
  \param[in]	rtc	   : Pointer to rtc device resources
  \return		ARM_DRIVER_ERROR_PARAMETER  : if device resource is invalid
				ARM_DRIVER_OK               : if counter value was successfully copied to val
*/
static int32_t rtc_read_counter (uint32_t *val, RTC_DRV_INFO *rtc)
{
	if (!rtc)
		return ARM_DRIVER_ERROR_PARAMETER;

	if ( !(rtc->flags & RTC_FLAG_DRV_INIT_DONE) )
		return ARM_DRIVER_ERROR;

	/* read rtc current counter value. */
	*val = rtc_read_current_counter (rtc);
	return ARM_DRIVER_OK;
}

/**
  \fn			int32_t rtc_load_counter (uint32_t loadvalue, RTC_DRV_INFO *rtc)
  \brief		load rtc counter value
  \param[out]		val    : Variable which contains desired counter value
  \param[in]		rtc    : Pointer to rtc device resources
  \return		ARM_DRIVER_ERROR_PARAMETER  : if device resource is invalid
			ARM_DRIVER_OK               : if new counter value was loaded successfully
*/
static int32_t rtc_load_counter (uint32_t loadvalue, RTC_DRV_INFO *rtc)
{
	if (!rtc)
		return ARM_DRIVER_ERROR_PARAMETER;

	if ( !(rtc->flags & RTC_FLAG_DRV_INIT_DONE) )
		return ARM_DRIVER_ERROR;

	/* disable RTC counter. */
	rtc_counter_control (DISABLE, rtc);

	/* Load counter value */
	rtc->paddr->CLR = loadvalue;

	/* enable RTC counter. */
	rtc_counter_control (ENABLE, rtc);

	return ARM_DRIVER_OK;
}

/**
  \fn			int32_t rtc_set_alarm (uint32_t  alarm_val,
                                       RTC_DRV_INFO *rtc)
  \brief		Set an alarm, Event call-back (\ref RTC_SignalEvent_t)
                will be called when the counter reaches 'alarm_val'.
  \param[in]	alarm_val  : counter value for which alarm should be generated
  \param[in]	rtc        : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER  : if rtc device is invalid
                ARM_DRIVER_ERROR_BUSY       : if an alarm has already been set in this rtc which has not yet rang.
                ARM_DRIVER_OK               : if alarm has been successfully setup.
*/
static int32_t rtc_set_alarm (uint32_t     alarm_val,
                              RTC_DRV_INFO *rtc)
{
	if (!rtc)
		return ARM_DRIVER_ERROR_PARAMETER;

	if ( !(rtc->flags & RTC_FLAG_DRV_INIT_DONE) )
		return ARM_DRIVER_ERROR;

	if ( rtc->flags & RTC_FLAG_DRV_ALARM_SET)
		return ARM_DRIVER_ERROR_BUSY;

	/* load rtc counter match register. */
	rtc_load_counter_match_register (alarm_val, rtc);

	/* Enable Interrupt generation. */
	rtc_interrupt_control (ENABLE, rtc);

	/* Unmask Interrupt. */
	rtc_interrupt_mask (INT_UNMASK, rtc);

	/* set rtc alarm flag. */
	rtc->flags |= RTC_FLAG_DRV_ALARM_SET;
	return ARM_DRIVER_OK;
}

/**
  \fn			int32_t rtc_set_prescaler (uint32_t     prescaler_val,
		                                   RTC_DRV_INFO *rtc)
  \brief		Configure the prescaler value, this does:
                  - Disable the counter and prescalar.
                  - Program the updated prescaler value.
                  - Enable the counter and prescaler.
  \param[in]    prescaler_val : new Prescaler value
  \param[in]    rtc           : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER  : if rtc device is invalid
                ARM_DRIVER_OK               : if rtc successfully updates Prescaler value.
*/
static int32_t rtc_set_prescaler (uint32_t     prescaler_val,
                                  RTC_DRV_INFO *rtc)
{
	if (!rtc)
		return ARM_DRIVER_ERROR_PARAMETER;

	if ( !(rtc->flags & RTC_FLAG_DRV_INIT_DONE) )
		return ARM_DRIVER_ERROR;

	/* disable RTC Prescaler. */
	rtc_prescaler_control (DISABLE, rtc);

	/* disable RTC counter. */
	rtc_counter_control (DISABLE, rtc);

	/*
	 * Configure the prescaler value.
	 * With the current input clock of 32.768kHz, program
	 * the prescaler with 32768 to get an effective counter
	 * frequency of 1 Hz.
	 */
	rtc_load_prescaler (prescaler_val, rtc);

	/* enable RTC Prescaler. */
	rtc_prescaler_control (ENABLE, rtc);

	/* enable RTC counter. */
	rtc_counter_control (ENABLE, rtc);

	/* Store the updated prescaler value. */
	rtc->prescaler_value = prescaler_val;

	return ARM_DRIVER_OK;
}

/**
  \fn			int32_t rtc_init (RTC_DRV_INFO *rtc)
  \brief		Initialize the rtc device, this does:
                  - Disable the counter, prescalar, and interrupts.
                  - Program the default prescaler value.
                  - Enable the counter and prescaler,
                  - Keep interrupt generation disabled until an alarm is set.
  \param[in]    rtc   : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER  : if rtc device is invalid
                ARM_DRIVER_OK               : if rtc successfully initialized or already initialized.
*/
static int32_t rtc_init (RTC_DRV_INFO *rtc)
{
	if (!rtc)
		return ARM_DRIVER_ERROR_PARAMETER;

	/*
	 * device has already been initialized ?
	 * This is not an error. So return SUCCESS, nothing
	 * else to do.
	 */
	if (rtc->flags & RTC_FLAG_DRV_INIT_DONE)
		return ARM_DRIVER_OK;

	/* disable RTC Prescaler. */
	rtc_prescaler_control (DISABLE, rtc);

	/* disable RTC counter. */
	rtc_counter_control (DISABLE, rtc);

	/* disable RTC interrupt. */
	rtc_interrupt_control (DISABLE, rtc);

	/* disable RTC counter wrap. */
	rtc_counter_wrap_control (DISABLE, rtc);

	/*
	 * Load the default prescaler value provided by RTE_device.h .
	 * With the current input clock of 32.768kHz, program
	 * the prescaler with 32768 to get an effective counter
	 * frequency of 1 Hz.
	 */
	rtc_load_prescaler (rtc->prescaler_value, rtc);

	/* enable RTC Prescaler. */
	rtc_prescaler_control (ENABLE, rtc);

	/* enable RTC counter. */
	rtc_counter_control (ENABLE, rtc);

	/* Set rtc flag to initialized. */
	rtc->flags = RTC_FLAG_DRV_INIT_DONE;

	return ARM_DRIVER_OK;
}

/**
  \fn			int32_t rtc_uninit (RTC_DRV_INFO *rtc)
  \brief		Uninitialize the rtc device, this does:
                - Disable the counter, prescalar, and interrupts.
  \param[in]	rtc     : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER : if rtc device is invalid
                ARM_DRIVER_OK              : if rtc successfully uninitialized or already not initialized.
*/
static int32_t rtc_uninit (RTC_DRV_INFO *rtc)
{
	if (!rtc)
		return ARM_DRIVER_ERROR_PARAMETER;

	/*
	 * device has not yet initialized ?
	 * return SUCCESS, nothing else to do.
	 */
	if ( !(rtc->flags & RTC_FLAG_DRV_INIT_DONE) )
		return ARM_DRIVER_OK;

	/* disable RTC Prescaler. */
	rtc_prescaler_control (DISABLE, rtc);

	/* disable RTC counter. */
	rtc_counter_control (DISABLE, rtc);

	/* disable RTC interrupt. */
	rtc_interrupt_control (DISABLE, rtc);

	/* disable RTC counter wrap. */
	rtc_counter_wrap_control (DISABLE, rtc);

	/* reset rtc flags. */
	rtc->flags = 0U;

	return ARM_DRIVER_OK;
}

/**
  \fn			void rtc_irq_handler (RTC_DRV_INFO *rtc)
  \brief		rtc interrupt handler
  \param[in]	rtc  : Pointer to rtc device resources
  \return       none
*/
static void rtc_irq_handler (RTC_DRV_INFO *rtc)
{
	uint32_t event = 0U;    /* callback event */

	/* Acknowledge the interrupt */
	rtc_interrupt_ack (rtc);

	/* mark event as Alarm Triggered. */
	event |= ARM_RTC_EVENT_ALARM_TRIGGER;

	/* call the user callback if any event occurs */
	if ((event != 0U) && (rtc->cb_event != NULL) )
	{
		/* call the user callback */
		rtc->cb_event(event);
	}

	/* Reset rtc Alarm flag. */
	rtc->flags &= (~RTC_FLAG_DRV_ALARM_SET);
}

/**
  \fn			int32_t RTC_PowerControl (ARM_POWER_STATE   state,
		                                  RTC_DRV_INFO      *rtc)
  \brief		rtc power control
  \param[in]	state : Power state
  \param[in]	rtc   : Pointer to rtc device resources
  \return       none
*/
static int32_t RTC_PowerControl (ARM_POWER_STATE   state,
                                 RTC_DRV_INFO      *rtc)
{
	switch (state)
	{
		case ARM_POWER_OFF:

			/* Disable RTC IRQ */
			NVIC_DisableIRQ (rtc->irq_num);

			/* Clear Any Pending IRQ*/
			NVIC_ClearPendingIRQ (rtc->irq_num);

			/* Reset rtc power flag. */
			rtc->flags &= (~RTC_FLAG_DRV_POWER_DONE);
			break;

		case ARM_POWER_FULL:

			if ( !(rtc->flags & RTC_FLAG_DRV_INIT_DONE) )
				return ARM_DRIVER_ERROR;

			/*
			 * device has already been powered up ?
			 * This is not an error. So return SUCCESS, nothing
			 * else to do.
			 */
			if ( rtc->flags & RTC_FLAG_DRV_POWER_DONE )
				return ARM_DRIVER_OK;

			/* Enable RTC IRQ*/
			NVIC_ClearPendingIRQ (rtc->irq_num);
			NVIC_SetPriority(rtc->irq_num, rtc->irq_priority);
			NVIC_EnableIRQ (rtc->irq_num);

			/* Set the power flag enabled */
			rtc->flags |= RTC_FLAG_DRV_POWER_DONE;
			break;

		case ARM_POWER_LOW:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ARM_DRIVER_OK;
}

/**
  \fn			int32_t RTC_Initialize (RTC_SignalEvent_t cb_event,
		                                RTC_DRV_INFO      *rtc)
  \brief		Initialize the rtc device, this does:
                  - Disable the counter, prescalar, and interrupts.
                  - Program the default prescaler value.
                  - Enable the counter and prescaler,
                  - Keep interrupt generation disabled until an alarm is set.
  \param[in]    cb_event : Pointer to RTC Event \ref RTC_SignalEvent
  \param[in]    rtc      : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER : if rtc device is invalid
                ARM_DRIVER_OK              : if rtc successfully initialized or already initialized.
*/
static int32_t RTC_Initialize (ARM_RTC_SignalEvent_t  cb_event,
                               RTC_DRV_INFO          *rtc)
{
	int32_t ret = ARM_DRIVER_OK;

	if (!cb_event)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* set the user callback event. */
	rtc->cb_event = cb_event;

	ret = rtc_init (rtc);
	return ret;
}

/**
  \fn			int32_t RTC_Uninitialize (RTC_DRV_INFO *rtc)
  \brief		Uninitialize the rtc device, this does:
                - Power-Off the RTC device.
                - Disable the counter, prescalar, and interrupts.
  \param[in]    rtc  : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER  : if rtc device is invalid
                ARM_DRIVER_OK               : if rtc successfully uninitialized or already not initialized.
*/
static int32_t RTC_Uninitialize (RTC_DRV_INFO *rtc)
{
	int32_t ret = ARM_DRIVER_OK;

	/* set the user callback event to NULL. */
	rtc->cb_event = NULL;

	/* Disable the power. */
	RTC_PowerControl (ARM_POWER_OFF, rtc);

	/* Un-initialize the RTC. */
	ret = rtc_uninit (rtc);
	return ret;
}

/**
  \fn			int32_t RTC_Control (uint32_t      control,
                                     uint32_t      arg,
                                     RTC_DRV_INFO  *rtc)
  \brief        CMSIS-Driver rtc control.
                Control RTC Interface.
  \param[in]    control : Operation
  \param[in]    arg     : Argument of operation (optional)
  \param[in]    rtc     : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER  : if rtc device is invalid
                ARM_DRIVER_OK               : if rtc successfully uninitialized or already not initialized.
*/
static int32_t RTC_Control (uint32_t      control,
                            uint32_t      arg,
                            RTC_DRV_INFO  *rtc)
{
	int ret = ARM_DRIVER_OK;

        if( !( (rtc->flags & RTC_FLAG_DRV_INIT_DONE) && ( rtc->flags & RTC_FLAG_DRV_POWER_DONE) ) )
                return ARM_DRIVER_ERROR ;

	switch (control)
	{
		case ARM_RTC_SET_PRESCALER:

			/* Set Prescaler Value. */
			ret = rtc_set_prescaler (arg, rtc);
			break;

		case ARM_RTC_SET_ALARM:

			/* Set Alarm as per arg. */
			ret = rtc_set_alarm (arg, rtc);
			break;

		default:
			/* Unsupported command */
			ret =  ARM_DRIVER_ERROR_UNSUPPORTED;
			break;
	}
	return ret;
}

/**
  \fn			int32_t RTC_ReadCounter (uint32_t *val, RTC_DRV_INFO *rtc)
  \brief		Read the current counter value
  \param[out]	val    : Pointer to the address where current rtc counter value
                          needs to be copied to
  \param[in]	rtc    : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER  : if device resource is invalid
                ARM_DRIVER_OK               : if counter value was successfully copied to val
*/
static int32_t RTC_ReadCounter (uint32_t *val, RTC_DRV_INFO *rtc)
{
	int32_t ret = ARM_DRIVER_OK;

	ret = rtc_read_counter (val, rtc);
	return ret;
}

/**
  \fn			int32_t RTC_LoadCounter (uint32_t loadvalue, RTC_DRV_INFO *rtc)
  \brief		Load new counter value
  \param[out]	val    : Variable which contains new counter value
  \param[in]	rtc    : Pointer to rtc device resources
  \return       ARM_DRIVER_ERROR_PARAMETER  : if device resource is invalid
                ARM_DRIVER_OK               : if counter value was loaded successfully
*/
static int32_t RTC_LoadCounter (uint32_t loadvalue, RTC_DRV_INFO *rtc)
{
	int32_t ret = ARM_DRIVER_OK;

	ret = rtc_load_counter (loadvalue, rtc);
	return ret;
}

/**
  \fn			void RTC_IRQHandler (RTC_DRV_INFO *rtc)
  \brief		rtc interrupt handler
  \param[in]    rtc  : Pointer to rtc device resources
  \return       none
*/
static void RTC_IRQHandler (RTC_DRV_INFO *rtc)
{
	rtc_irq_handler (rtc);
}

// End RTC Interface


/* RTC0 Driver Instance */
#if (RTE_RTC0)

/* RTC0 device configuration */
static RTC_DRV_INFO RTC0 = {
	.paddr                     = (RTC_TypeDef *) RTC0_BASE,
	.cb_event                  = NULL,
	.flags                     = 0,
	.prescaler_value           = RTE_RTC0_DEFAULT_PRESCALER_VALUE,
	.irq_num                   = (IRQn_Type) RTC0_IRQ,
	.irq_priority              = RTE_RTC0_IRQ_PRI,
};


/* Function Name: RTC0_Initialize */
static int32_t RTC0_Initialize (ARM_RTC_SignalEvent_t cb_event)
{
    return (RTC_Initialize (cb_event, &RTC0));
}

/* Function Name: RTC0_Uninitialize */
static int32_t RTC0_Uninitialize (void)
{
    return (RTC_Uninitialize (&RTC0));
}

/* Function Name: RTC0_PowerControl */
static int32_t RTC0_PowerControl(ARM_POWER_STATE state)
{
    return (RTC_PowerControl(state, &RTC0));
}

/* Function Name: RTC0_Control */
static int32_t RTC0_Control(uint32_t control, uint32_t arg)
{
    return (RTC_Control(control, arg, &RTC0));
}

/* Function Name: RTC0_ReadCounter */
static int32_t RTC0_ReadCounter (uint32_t *val)
{
    return (RTC_ReadCounter (val, &RTC0));
}

/* Function Name: RTC0_LoadCounter */
static int32_t RTC0_LoadCounter (uint32_t loadval)
{
    return (RTC_LoadCounter (loadval, &RTC0));
}

/* Function Name: RTC0_IRQHandler */
void RTC0_IRQHandler (void)
{
    RTC_IRQHandler (&RTC0);
}

extern ARM_DRIVER_RTC Driver_RTC0;
ARM_DRIVER_RTC Driver_RTC0 =
{
    RTC_GetVersion,
    RTC_GetCapabilities,
    RTC0_Initialize,
    RTC0_Uninitialize,
    RTC0_PowerControl,
    RTC0_Control,
    RTC0_ReadCounter,
    RTC0_LoadCounter
};
#endif /* End of RTE_RTC0 */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
