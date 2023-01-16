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
 * @file     Driver_Comparator.c
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     20-June-2022
 * @brief    Analog Comparator driver definitions.
 ******************************************************************************/

/* Project Includes */
#include "Driver_Comparator.h"
#include "Comparator_dev.h"
#include "analog_config.h"

#if !(RTE_CMP0 || RTE_CMP1 || RTE_CMP2 || RTE_CMP3)
#error "Comparator is not configured in RTE_device.h!"
#endif

#if (defined(RTE_Drivers_CMP0) && !RTE_CMP0)
#error "CMP0 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_CMP1) && !RTE_CMP1)
#error "CMP1 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_CMP2) && !RTE_CMP2)
#error "CMP2 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_CMP3) && !RTE_CMP3)
#error "CMP3 not configured in RTE_Device.h!"
#endif

#define ARM_CMP_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /*  Driver version */

/*Driver version*/
static const ARM_DRIVER_VERSION DriverVersion = {
        ARM_CMP_API_VERSION,
        ARM_CMP_DRV_VERSION
};

/*Driver Capabilities   */
static const ARM_COMPARATOR_CAPABILITIES DriverCapabilities = {
    1,/* Ability to invert the input signal */
    1,/* Used to define when to look at the comparator input */
    1,/* Supports Filter function */
    1,/* Supports Prescaler function */
    0 /* Reserved ( must be ZERO) */
};

/**
 @fn           ARM_DRIVER_VERSION CMP_GetVersion(void)
 @brief        get CMP version
 @param        none
 @return       driver version
 */
static ARM_DRIVER_VERSION CMP_GetVersion(void)
{
    return DriverVersion;
}

/**
 @fn           ARM_COMPARATOR_CAPABILITIES CMP_GetCapabilities(void)
 @brief        get Comparator Capabilities
 @param        none
 @return       driver Capabilities
 */
static ARM_COMPARATOR_CAPABILITIES CMP_GetCapabilities(void)
{
    return DriverCapabilities;
}

/*
 * @func        void CMP_output_sel( CMP_resources_t *cmp)
 * @brief       Select the Comparator instance for the output
 * @param[in]   cmp    : pointer to the CMP_resources_t
 */
static void CMP_output_sel ( CMP_resources_t *cmp)
{
    /* ADC Output register address */
    #define ADC_REG1_BASE   (ADC0_BASE + 0x00000038)

    __IOM uint32_t *output = (uint32_t *) ADC_REG1_BASE;

    /* Select the comparator output and add to the ADC reg1 address */
    *output = cmp->output_mux_sel;

}

/**
 * @fn          void CMP_controller(int ctrl, CMP_resources_t *cmp)
 * @brief       To enable or disable the comparator module
 * @param[in]   ctrl : Enable or Disable the comparator module
 * @param[in]   cmp  : Pointer to structure
 * @return      None
 */
static void CMP_controller(int ctrl, CMP_resources_t *cmp)
{
    /* comparator configuration register is provided on CMP0 base address */
    CMP_TypeDef *cmp_reg = (CMP_TypeDef *)COMP0_BASE;

    if (ctrl)
    {
        /* Enable the Analog Comparator module */
        cmp_reg->reg1 |= (cmp->input_enable);
    }
    else
    {
        /* Disable the Analog Comparator module */
        cmp_reg->reg1 &= ~(cmp->input_enable);
    }
}

/*
 * @func        void CMP_IRQ_handler(CMP_resources_t *cmp)
 * @brief       - First toggle the polarity bit
 *              - clear the interrupt source in comparator
 *              - Finally clear IRQ in NVIC
 * @param[in]   cmp : pointer to the structure
 * @return      NONE
 */
void CMP_IRQ_handler(CMP_resources_t *cmp)
{
    int32_t event = 0U;

    /* toggle the polarity control bit */
    cmp->reg_base->polarity_ctrl ^= 1;

    /* clear the interrupt source */
    while (cmp->reg_base->interrupt)
        cmp->reg_base->interrupt = CMP_INTERRUPT_CLEAR;

    /* clear the pending IRQ flag */
    NVIC_ClearPendingIRQ(cmp->irq_num);

    event |= ARM_CMP_FILTER_EVENT_OCCURRED;

    if((event != 0U) && (cmp->cb_event != NULL))
    {
        /* call user callback */
        cmp->cb_event(event);
    }

}

/**
 * @fn          void CMP_configure(uint32_t config_value, CMP_resources_t *cmp)
 * @brief       Add configuration value to the reg1 of Comp0 instance
 * @param[in]   config_value : To store the configuration values
 * @return      None
 */
static void CMP_configure(uint32_t config_value, CMP_resources_t *cmp)
{
    /* comparator configuration register is provided on CMP0 base address */
    CMP_TypeDef *cmp_reg = (CMP_TypeDef *)COMP0_BASE;

    /* Adding configuration values to the reg1 of Comp0 instance  */
    cmp_reg->reg1 = config_value;
}

/**
 @fn           void analog_config(void)
 @brief        Analog configuration register includes Vbat and comparator
 @param[in]    none
 @return       none
 */
static void analog_config(void)
{
    /* Analog configuration Vbat register2 */
    analog_config_vbat_reg2();

    /* Analog configuration comparator register2 */
    analog_config_comp_reg2();
}

/**
 * @fn         CMP_Initialize (ARM_Comparator_SignalEvent_t cb_event)
 * @brief      Initialize the following:
                 - Access vbat and comparator register configuration
                 - Select the positive and negative terminal of comparator
                 - Enable the polarity control for Silicon Rev A1
                 - Select the instances for the output comparator
                 - Mask the interrupt
                 - Store the call back event
 * @param[in]  cb_event : Pointer to /ref ARM_Comparator_SignalEvent_t cb_event
 * @param[in]  cmp      : Pointer to Comparator resources
 * @return     ARM_DRIVER_OK : if driver initialized successfully
 */
static int32_t CMP_Initialize (ARM_Comparator_SignalEvent_t cb_event ,CMP_resources_t *cmp )
{
    int32_t ret = ARM_DRIVER_OK;

    if(!cb_event)
        return ARM_DRIVER_ERROR_PARAMETER;

    /*function include vbat and comparator address and it value */
    analog_config();

    /* Initialize the Comparator configurations */
    CMP_configure(cmp->config, cmp);

    /* Note:
     * For Silicon Rev A1: Enable the polarity control
     * For Silicon Rev A0: Disable the polarity control
     */

#if RTE_SILICON_REV_A1
    /* invert the value of comparator input before processing  */
    cmp->reg_base->polarity_ctrl = ENABLE;
#endif

    /* To select the output of the comparator module */
    CMP_output_sel(cmp);

    /* To mask the interrupt */
    cmp->reg_base->interrupt_mask = CMP_INT_MASK;

    /* User call back Event */
    cmp->cb_event = cb_event;

    /* Setting flag to initialize */
    cmp->flags |= CMP_FLAG_DRV_INIT_DONE;

    return ret;
}

/**
 @fn           int32_t CMP_Uninitialize (CMP_resources_t *cmp)
 @brief        this function will
                 -initialize all the register to 0,
                 -set call back to NULL
                 -Disable the interrupt mask
                 -Reset the flag
 @param[in]    cmp  : Pointer to Comparator resources
 @return       ARM_DRIVER_OK : if Comparator successfully uninitialized or already not initialized
 */

static int32_t CMP_Uninitialize (CMP_resources_t *cmp)
{
    int32_t ret = ARM_DRIVER_OK;

    if(!cmp)
        return ARM_DRIVER_ERROR_PARAMETER;

    /* Checking initialized has done or not */
    if(!(cmp->flags & CMP_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_OK;

    /* Set the configuration values of reg1 to zero */
    cmp->reg_base->reg1 = 0U;

    /* To the mask the interrupt */
    cmp->reg_base->interrupt_mask = CMP_INT_MASK;

    /* set call back to NULL */
    cmp->cb_event = NULL;

    /* Reset the flags */
    cmp->flags = 0;

    return ret;
}

/**
 * @fn          CMP_PowerControl(ARM_POWER_STATE status,CMP_resources_t *cmp )
 * @brief       power the driver and enable NVIC
 * @param[in]   state : power state
 * @param[in]   adc   : pointer to /ref CMP_resources_t
 * @return      ARM_DRIVER_OK    : if power done successful
                ARM_DRIVER_ERROR : if initialize is not done
 */

static int32_t CMP_PowerControl(ARM_POWER_STATE status,CMP_resources_t *cmp )
{
    int32_t ret = ARM_DRIVER_OK;
    switch (status)
    {
    case ARM_POWER_FULL:
        if(!(cmp->flags & CMP_FLAG_DRV_INIT_DONE))
            return ARM_DRIVER_ERROR;

        if((cmp->flags & CMP_FLAG_DRV_POWER_DONE))
            return ARM_DRIVER_OK;

        /* Clear Any Pending IRQ */
        NVIC_ClearPendingIRQ(cmp->irq_num);

        /* Set the priority */
        NVIC_SetPriority (cmp->irq_num, cmp->irq_priority);

        /* Enable the NIVC */
        NVIC_EnableIRQ(cmp->irq_num);

        /* Set the power flag enabled */
        cmp->flags |= CMP_FLAG_DRV_POWER_DONE;
        break;

    case ARM_POWER_OFF:

        /* Disable ADC NVIC */
        NVIC_DisableIRQ(cmp->irq_num);

        /* Clear Any Pending IRQ */
        NVIC_ClearPendingIRQ(cmp->irq_num);

        /* Reset the power status of ADC */
        cmp->flags &= ~CMP_FLAG_DRV_POWER_DONE;

        break;

    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;

    }

    return ret;
}

/**
 * @brief        CMP_Control(uint32_t control, uint32_t arg, CMP_resources_t *cmp)
 * @brief        CMSIS-Driver comparator control.
                 Control comparator Interface.
 * @param[in]    control : Operation \ref Driver_Comparator.h : comparator control codes
 * @param[in]    arg     : Argument of operation (optional)
 * @param[in]    cmp     : Pointer to comparator resources
 * @return       ARM_DRIVER_ERROR_PARAMETER  : if comparator device is invalid
                 ARM_DRIVER_OK               : if comparator successfully uninitialized or already not initialized
 */

static int32_t CMP_Control(uint32_t control, uint32_t arg, CMP_resources_t *cmp)
{
    int32_t ret = ARM_DRIVER_OK;

    if (!(cmp->flags & CMP_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_ERROR;

    if (!(cmp->flags & CMP_FLAG_DRV_POWER_DONE))
        return ARM_DRIVER_ERROR;

    switch (control)
    {
    case ARM_CMP_POLARITY_CONTROL:

        if(arg > 2)
            return ARM_DRIVER_ERROR_PARAMETER;

        /* If active, invert the value of comparator input before processing  */
        cmp->reg_base->polarity_ctrl = arg;

        break;

    case ARM_CMP_WINDOW_CONTROL:

        if(arg >3)
            return ARM_DRIVER_ERROR_PARAMETER;

        /* To enable the windowing function for ASIC A0 board */
        #if RTE_SILICON_REV_A

        cmp->reg_base->window_ctrl |= (CMP_WINDOW_CONTROL_A0_ENABLE |arg <<8);

        /* To enable the windowing function for ASIC B0 board */
        #elif RTE_SILICON_REV_B0

        cmp->reg_base->window_ctrl |= (CMP_WINDOW_CONTROL_B0_ENABLE | arg <<8);

        #endif

        break;

    case ARM_CMP_FILTER_CONTROL:

        if(arg < 2 || arg > 8 )
            return ARM_DRIVER_ERROR_PARAMETER;

        #if RTE_SILICON_REV_A

        /* to enable the filter function in windowing control register */
        cmp->reg_base->window_ctrl = CMP_FILTER_CONTROL_ENABLE;

        #endif

        /* To enable the filter function and adding filter values to the filter control register */
        cmp->reg_base->filter_ctrl |= (CMP_FILTER_CONTROL_ENABLE |(arg<<8));

        break;

    case ARM_CMP_PRESCALER_CONTROL:

        if(arg > CMP_PRESCALER_MAX_VALUE)
            return ARM_DRIVER_ERROR_PARAMETER;

        /* Comparator input will be sampled at the given clocks */
        cmp->reg_base->prescaler_ctrl = arg;

        break;

    default:
        ret = ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ret;

}

/**
 * @fn         CMP_Start (CMP_resources_t *cmp)
 * @brief      CMSIS-Driver Comparator Start
 *             Clear the IRQ before re-starting
 *             Enable the Comparator.
 * @param[in]  cmp  : Pointer to Comparator resources
 * @return     ARM_DRIVER_OK  : if the function are return successful
 *             ARM_DRIVER_ERROR : if initialize is not done
 */
static int32_t CMP_Start (CMP_resources_t *cmp)
{
    int32_t ret = ARM_DRIVER_OK;

    if (!(cmp->flags & CMP_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_ERROR;

    if (!(cmp->flags & CMP_FLAG_DRV_POWER_DONE))
        return ARM_DRIVER_ERROR;

    /* enable the interrupt(unmask the interrupt 0x0)*/
    cmp->reg_base->interrupt_mask = 0x0;

    /* clear the interrupt before re-starting */
    while (cmp->reg_base->interrupt)
        cmp->reg_base->interrupt = CMP_INTERRUPT_CLEAR;

    /* Clear Any Pending IRQ */
    NVIC_ClearPendingIRQ(cmp->irq_num);

    /* enable the NVIC interrupt. */
    NVIC_EnableIRQ(cmp->irq_num);

    /* Enable the Comparator module */
    CMP_controller(ENABLE,cmp);

    return ret;
}

/**
 * @fn         CMP_Stop (CMP_resources_t *cmp)
 * @brief      CMSIS-Driver Comparator Stop
 *               Disable the Comparator.
 * @param[in]  cmp  : Pointer to Comparator resources
 * @return     ARM_DRIVER_OK  : if the function are return successful
 *             ARM_DRIVER_ERROR : if initialize is not done
 */
static int32_t CMP_Stop (CMP_resources_t *cmp)
{
	int32_t ret = ARM_DRIVER_OK;
    if (!(cmp->flags & CMP_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_ERROR;

    if (!(cmp->flags & CMP_FLAG_DRV_POWER_DONE))
        return ARM_DRIVER_ERROR;

    /* Disable the Comparator module */
    CMP_controller(DISABLE,cmp);

    /* Mask the interrupt */
    cmp->reg_base->interrupt_mask = CMP_INT_MASK;

    /* Disable the interrupt */
    NVIC_DisableIRQ(cmp->irq_num);

    return ret;
}

/* CMP0 driver instance */
#if (RTE_CMP0)

/* Comparator Configurations */
static CMP_resources_t CMP0 = {

    .cb_event           = NULL,
    .reg_base           = (CMP_TypeDef *)COMP0_BASE,
    .flags              = 0,
    .irq_num            = (IRQn_Type)CMP0_IRQ,
    .config             = (RTE_CMP0_SEL_POSITIVE << 0 )     |
                          (RTE_CMP0_SEL_NEGATIVE << 2)      |
                          (RTE_CMP0_SEL_HYSTERISIS << 4 ),
    .irq_priority       = RTE_CMP0_IRQ_PRIORITY,
    .output_mux_sel     = RTE_CMP0_OUTPUT_MUX_SEL,
    .input_enable       = CMP0_ENABLE
};

/* Function Name: CMP0_Initialize */
static int32_t CMP0_Initialize(ARM_Comparator_SignalEvent_t cb_event)
{
    return (CMP_Initialize(cb_event,&CMP0));
}

/* Function Name: CMP0_Uninitialize */
static int32_t CMP0_Uninitialize(void)
{
    return (CMP_Uninitialize(&CMP0));
}

/* Function Name: CMP0_PowerControl */
static int32_t CMP0_PowerControl(ARM_POWER_STATE status)
{
    return (CMP_PowerControl(status,&CMP0));
}

/* Function Name: CMP0_Control */
static int32_t CMP0_Control(uint32_t control, uint32_t arg)
{
    return (CMP_Control(control,arg,&CMP0));
}

/* Function Name: CMP0_Start */
static int32_t CMP0_Start (void)
{
    return (CMP_Start(&CMP0));
}

/* Function name :CMP0_Stop */
static int32_t CMP0_Stop (void)
{
    return (CMP_Stop(&CMP0));
}

/*Function Name : CMP0_IRQHandler */
void CMP0_IRQHandler (void)
{
    return CMP_IRQ_handler(&CMP0);
}

extern ARM_DRIVER_CMP Driver_CMP0;
ARM_DRIVER_CMP Driver_CMP0 =
{
    CMP_GetVersion,
    CMP_GetCapabilities,
    CMP0_Initialize,
    CMP0_Uninitialize,
    CMP0_PowerControl,
    CMP0_Control,
    CMP0_Start,
    CMP0_Stop
};

#endif

/* CMP1 driver instance */
#if (RTE_CMP1)
/* Comparator Configurations */
static CMP_resources_t CMP1 = {

    .cb_event           = NULL,
    .reg_base           = (CMP_TypeDef *)COMP1_BASE,
    .flags              = 0,
    .irq_num            = (IRQn_Type)CMP1_IRQ,
    .config             = (RTE_CMP1_SEL_POSITIVE << 7)      |
                          (RTE_CMP1_SEL_NEGATIVE << 9)      |
                          (RTE_CMP1_SEL_HYSTERISIS << 11),
    .irq_priority       = RTE_CMP1_IRQ_PRIORITY,
    .output_mux_sel     = RTE_CMP1_OUTPUT_MUX_SEL,
    .input_enable       = CMP1_ENABLE
};


/* Function Name: CMP1_Initialize */
static int32_t CMP1_Initialize(ARM_Comparator_SignalEvent_t cb_event)
{
    return (CMP_Initialize(cb_event,&CMP1));
}

/* Function Name: CMP1_Uninitialize */
static int32_t CMP1_Uninitialize(void)
{
    return (CMP_Uninitialize(&CMP1));
}

/* Function Name: CMP1_PowerControl */
static int32_t CMP1_PowerControl(ARM_POWER_STATE status)
{
    return (CMP_PowerControl(status,&CMP1));
}

/* Function Name: CMP1_Control */
static int32_t CMP1_Control(uint32_t control, uint32_t arg)
{
    return (CMP_Control(control,arg,&CMP1));
}

/* Function Name: CMP1_Start */
static int32_t CMP1_Start (void)
{
    return (CMP_Start(&CMP1));
}

/* Function name :CMP1_Stop */
static int32_t CMP1_Stop (void)
{
    return (CMP_Stop(&CMP1));
}

/*Function Name : CMP1_IRQHandler */
void CMP1_IRQHandler (void)
{
    return CMP_IRQ_handler(&CMP1);
}

extern ARM_DRIVER_CMP Driver_CMP1;
ARM_DRIVER_CMP Driver_CMP1 =
{
    CMP_GetVersion,
    CMP_GetCapabilities,
    CMP1_Initialize,
    CMP1_Uninitialize,
    CMP1_PowerControl,
    CMP1_Control,
    CMP1_Start,
    CMP1_Stop
};

#endif

/* CMP2 driver instance */
#if (RTE_CMP2)

/* Comparator Configurations */
static CMP_resources_t CMP2 = {

    .cb_event           = NULL,
    .reg_base           = (CMP_TypeDef *)COMP2_BASE,
    .flags              = 0,
    .irq_num            = (IRQn_Type)CMP2_IRQ,
    .config             = (RTE_CMP2_SEL_POSITIVE << 14)      |
                          (RTE_CMP2_SEL_NEGATIVE << 16)      |
                          (RTE_CMP2_SEL_HYSTERISIS << 18),
    .irq_priority       = RTE_CMP2_IRQ_PRIORITY,
    .output_mux_sel     = RTE_CMP2_OUTPUT_MUX_SEL,
    .input_enable       = CMP2_ENABLE
};


/* Function Name: CMP2_Initialize */
static int32_t CMP2_Initialize(ARM_Comparator_SignalEvent_t cb_event)
{
    return (CMP_Initialize(cb_event,&CMP2));
}

/* Function Name: CMP2_Uninitialize */
static int32_t CMP2_Uninitialize(void)
{
    return (CMP_Uninitialize(&CMP2));
}

/* Function Name: CMP2_PowerControl */
static int32_t CMP2_PowerControl(ARM_POWER_STATE status)
{
    return (CMP_PowerControl(status,&CMP2));
}

/* Function Name: CMP2_Control */
static int32_t CMP2_Control(uint32_t control, uint32_t arg)
{
    return (CMP_Control(control,arg,&CMP2));
}

/* Function Name: CMP2_Start */
static int32_t CMP2_Start (void)
{
    return (CMP_Start(&CMP2));
}

/* Function name :CMP2_Stop */
static int32_t CMP2_Stop (void)
{
    return (CMP_Stop(&CMP2));
}

/*Function Name : CMP2_IRQHandler */
void CMP2_IRQHandler (void)
{
    return CMP_IRQ_handler(&CMP2);
}

extern ARM_DRIVER_CMP Driver_CMP2;
ARM_DRIVER_CMP Driver_CMP2 =
{
    CMP_GetVersion,
    CMP_GetCapabilities,
    CMP2_Initialize,
    CMP2_Uninitialize,
    CMP2_PowerControl,
    CMP2_Control,
    CMP2_Start,
    CMP2_Stop
};

#endif

/* CMP3 driver instance */
#if (RTE_CMP3)

/* Comparator Configurations */
static CMP_resources_t CMP3 = {

    .cb_event           = NULL,
    .reg_base           = (CMP_TypeDef *)COMP3_BASE,
    .flags              = 0,
    .irq_num            = (IRQn_Type)CMP3_IRQ,
    .config             = (RTE_CMP3_SEL_POSITIVE << 21)      |
                          (RTE_CMP3_SEL_NEGATIVE << 23)      |
                          (RTE_CMP3_SEL_HYSTERISIS << 25),
    .irq_priority       = RTE_CMP3_IRQ_PRIORITY,
    .output_mux_sel     = RTE_CMP3_OUTPUT_MUX_SEL,
    .input_enable       = CMP3_ENABLE
};


/* Function Name: CMP3_Initialize */
static int32_t CMP3_Initialize(ARM_Comparator_SignalEvent_t cb_event)
{
    return (CMP_Initialize(cb_event,&CMP3));
}

/* Function Name: CMP3_Uninitialize */
static int32_t CMP3_Uninitialize(void)
{
    return (CMP_Uninitialize(&CMP3));
}

/* Function Name: CMP3_PowerControl */
static int32_t CMP3_PowerControl(ARM_POWER_STATE status)
{
    return (CMP_PowerControl(status,&CMP3));
}

/* Function Name: CMP3_Control */
static int32_t CMP3_Control(uint32_t control, uint32_t arg)
{
    return (CMP_Control(control,arg,&CMP3));
}

/* Function Name: CMP3_Start */
static int32_t CMP3_Start (void)
{
    return (CMP_Start(&CMP3));
}

/* Function name :CMP3_Stop */
static int32_t CMP3_Stop (void)
{
    return (CMP_Stop(&CMP3));
}

/*Function Name : CMP3_IRQHandler */
void CMP3_IRQHandler (void)
{
    return CMP_IRQ_handler(&CMP3);
}

extern ARM_DRIVER_CMP Driver_CMP3;
ARM_DRIVER_CMP Driver_CMP3 =
{
    CMP_GetVersion,
    CMP_GetCapabilities,
    CMP3_Initialize,
    CMP3_Uninitialize,
    CMP3_PowerControl,
    CMP3_Control,
    CMP3_Start,
    CMP3_Stop
};

#endif
