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
 * @file     Driver_DAC.h
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     22-Feb-2022
 * @brief    DAC(Digital to Analog Converter) driver definitions.DAC0 connected
 *           to PO_18 and DAC1 connected to PO_19.
 ******************************************************************************/

/* Project Includes */
#include "Driver_DAC.h"
#include "DAC_dev.h"
#include "analog_config.h"

#if !(RTE_DAC0 || RTE_DAC1)
#error "DAC is not enabled in the RTE_device.h"
#endif

#if (defined(RTE_Drivers_DAC0) && !RTE_DAC0)
#error "DAC0 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_DAC1) && !RTE_DAC1)
#error "DAC1 not configured in RTE_Device.h!"
#endif

#define ARM_DAC_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /*  Driver version */

/*Driver version*/
static const ARM_DRIVER_VERSION DriverVersion = {
        ARM_DAC_API_VERSION,
        ARM_DAC_DRV_VERSION
};

/*Driver Capabilities   */
static const ARM_DAC_CAPABILITIES DriverCapabilities = {
    1,/* 12 bit DAC resolution */
    0 /* Reserved ( must be ZERO) */
};

/**
 @fn           ARM_DRIVER_VERSION DAC_GetVersion(void)
 @brief        get DAC version
 @param        none
 @return       driver version
 */
static ARM_DRIVER_VERSION DAC_GetVersion(void)
{
    return DriverVersion;
}

/**
 @fn           ARM_RTC_CAPABILITIES DAC_GetCapabilities(void)
 @brief        get DAC Capabilites
 @param        none
 @return       driver Capabilites
 */
static ARM_DAC_CAPABILITIES DAC_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
 @fn           DAC_controller(int ctrl ,DAC_resources_t *dac)
 @brief        Driver to ENABLE or DISABLE or RESET the DAC.
 @param[in]    control : Operation
 @param[in]    ctrl    : DAC control parameter
 @param[in]    dac     : Pointer to dac device resources
 @return       none
 */
static void DAC_controller(int ctrl ,DAC_resources_t *dac)
{
    /* Enable the DAC */
    if(ctrl == ENABLE)
    {
         dac->reg_base->REG1 |= (DAC_EN);
    }

    /* Disable the DAC */
    if(ctrl == DISABLE)
    {
        dac->reg_base->REG1 &= ~(DAC_EN);
    }

    /* It will RESET both DAC0 and DAC1 */
    if(ctrl == RESET)
    {
        /* DAC configuration register is provided on DAC0 base address */
        DAC_TypeDef_t *config_addr= (DAC_TypeDef_t *)DAC0_BASE;
        config_addr->REG1 &= ~(DAC_RESET_OR_ENABLE);
    }
}
/**
 @fn           DAC_configure(uint32_t config_value,DAC_resources_t *dac )
 @brief        CMSIS-Driver to configure the DAC.
 @param[in]    configure: Operation
 @param[in]    config_value :to give the value to the register
 @param[in]    dac      : Pointer to dac device resources
 @return       none
 */
static void DAC_configure(uint32_t config_value, DAC_resources_t *dac )
{
    /* DAC configuration register is provided on DAC0 base address */
    DAC_TypeDef_t *config_addr= (DAC_TypeDef_t *)DAC0_BASE;

    /* It will enable the both the instance of DAC*/
    config_addr->REG1 |= (DAC_RESET_OR_ENABLE);

    /* Adding value to the register */
    dac->reg_base->REG1 |= config_value;
}

/**
 @fn           void analog_config()
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
 @fn           int32_t DAC_Initialize (DAC_DRV_INFO *dac)
 @brief        this function will
                                 -disable the dac,
                                 -initialize the dac configuration
 @param[in]    dac : Pointer to dac resources
 @return       ARM_DRIVER_ERROR_PARAMETER : if dac device is invalid
               ARM_DRIVER_OK              : if dac successfully initialized or already initialized
 */
static int32_t DAC_Initialize (DAC_resources_t *dac)
{
    int ret = ARM_DRIVER_OK;

    /* Initialization for Analog configuration register */
    analog_config();

    /* DAC Disable */
    DAC_controller(DISABLE, dac);

    /* Initialize DAC configuration */
    DAC_configure(dac->config,dac);

    /* Setting the flag */
    dac->flags |= DAC_FLAG_DRV_INIT_DONE;

    return ret;
}

/**
 @fn           int32_t DAC_Uninitialize (DAC_DRV_INFO *dac)
 @brief        this function will
                                 -initialize all the register to 0,
                                 -Reset the flag
 @param[in]    dac  : Pointer to dac resources
 @return       ARM_DRIVER_OK : if dac successfully uninitialized or already not initialized
 */
static int32_t DAC_Uninitialize (DAC_resources_t *dac)
{
      int32_t ret = ARM_DRIVER_OK;

      /* Initailise all the configurations to zero */
      dac->reg_base->REG1 = 0U;

      /* Reset the flag */
      dac->flags = 0U;

     return ret;
}

/**
 @fn           int32_t DAC_PowerControl (ARM_POWER_STATE state,
                                          DAC_DRV_INFO *DAC)
 @brief        CMSIS-DRIVER DAC power control
 @param[in]    state : Power state
 @param[in]    DAC   : Pointer to DAC resources
 @return       ARM_DRIVER_ERROR_PARAMETER  : if dac device is invalid
               ARM_DRIVER_OK               : if dac successfully uninitialized or already not initialized
 */
static int32_t DAC_PowerControl(ARM_POWER_STATE status,
                                DAC_resources_t *dac)
{
    switch(status)
    {
          case ARM_POWER_OFF:

               /* Reset the power flag */
               dac->flags &= ~(DAC_FLAG_DRV_POWER_DONE);
               break;

          case ARM_POWER_FULL:
               if(!(dac->flags & DAC_FLAG_DRV_INIT_DONE))
               {
                    /* error:Driver is not initialized */
                    return ARM_DRIVER_ERROR;
               }

               if((dac->flags & DAC_FLAG_DRV_POWER_DONE))
               {
                    return ARM_DRIVER_OK;
               }

               /* Set the power flag enabled */
               dac->flags |= DAC_FLAG_DRV_POWER_DONE;
              break;

          case ARM_POWER_LOW:
          default:
              return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ARM_DRIVER_OK;
}

/**
 @fn           int32_t DAC_Control (uint32_t control,
                                    uint32_t arg,
                                    DAC_DRV_INFO *dac)
 @brief        CMSIS-Driver dac control.
               Control DAC Interface.
 @param[in]    control : Operation \ref Driver_DAC.h : DAC control codes
 @param[in]    arg     : Argument of operation (optional)
 @param[in]    dac     : Pointer to dac resources
 @return       ARM_DRIVER_ERROR_PARAMETER  : if dac device is invalid
               ARM_DRIVER_OK               : if dac successfully uninitialized or already not initialized
 */
static int32_t DAC_Control (uint32_t control,
                            uint32_t arg,
                            DAC_resources_t *dac)
{
    int32_t ret = ARM_DRIVER_OK;

    switch (control)
    {
          case ARM_DAC_RESET:
               /* To reset the both DAC0 and DAC1 */
               DAC_controller(RESET, dac);
               break;

          default:
               ret = ARM_DRIVER_ERROR_UNSUPPORTED;
               break;
    }
    return ret;
}

/**
 * @fn         DAC_Start (DAC_resources_t *dac)
 * @brief      CMSIS-Driver DAC Start
 *             Enable the DAC.
 @param[in]   dac  : Pointer to dac resources
 @return      ARM_DRIVER_OK  : if dac successfully uninitialized or already not initialized
 */
static int32_t DAC_Start (DAC_resources_t *dac)
{
    int32_t ret = ARM_DRIVER_OK;

    if(!(dac->flags & DAC_FLAG_DRV_POWER_DONE))
    {
         /* error:Driver is not initialized */
         return ARM_DRIVER_ERROR;
    }

    if((dac->flags & DAC_STARTED))
    {
         return ARM_DRIVER_OK;
    }

    dac->flags |= DAC_STARTED;

    /* Enable the DAC */
    DAC_controller(ENABLE, dac);

    return ret;
}

/**
 *@fn          DAC_Stop (DAC_resources_t *dac)
 *@brief       CMSIS-Driver DAC Stop
               Disable the DAC
 *@param[in]   dac     : Pointer to dac resources
 *@return      ARM_DRIVER_OK : if dac successfully uninitialized or already not initialized
 */
static int32_t DAC_Stop (DAC_resources_t *dac)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Disable the DAC */
    DAC_controller(DISABLE, dac);

    dac->flags &= ~(DAC_STARTED);

    return ret;
}

/**
 @fn           DAC_SetInput(uint32_t num, DAC_resources_t *dac)
 @brief        CMSIS-Driver to set the DAC input.
 @param[in]    Input : Operation
 @param[in]    value  : DAC input
 @param[in]    dac  : Pointer to dac resources
 @return       ARM_DRIVER_ERROR_PARAMETER  : if dac device is invalid
                ARM_DRIVER_OK               : if dac successfully uninitialized or already not initialized
 */
static int32_t DAC_SetInput(uint32_t value, DAC_resources_t *dac)
{
    int32_t ret = ARM_DRIVER_OK;

    if(!(dac->flags & DAC_STARTED))
    {
         /* error:Driver is not started */
         return ARM_DRIVER_ERROR;
    }
    /* error if input is out of the range */
    if(value > DAC_MAX_INPUT)
    {
        ret = ARM_DRIVER_ERROR_PARAMETER;
    }

    dac->reg_base->INPUT = value;

    return ret;
}

/* DAC0 driver instance */
#if(RTE_DAC0)
/* DAC configuration */
static DAC_resources_t DAC0 = {
        .reg_base       = (DAC_TypeDef_t *)DAC0_BASE,
        .flags          = 0,
        .config         = (RTE_DAC0_INPUT_BYP_MUX_EN << 1) |
                          (RTE_DAC0_BYP_VAL << 2)          |
                          (RTE_DAC0_CAP_CONT <<14)         |
                          (RTE_DAC0_RES_CONT <<18)         |
                          (RTE_DAC0_TWOSCOMP_EN << 22 )    |
                          (RTE_DAC0_IBIAS << 23)
};

/* Function Name: DAC0_Initialize */
static int32_t DAC0_Initialize(void)
{
    return (DAC_Initialize(&DAC0));
}

/* Function Name: DAC0_uninitialize */
static int32_t DAC0_Uninitialize(void)
{
    return (DAC_Uninitialize(&DAC0));
}

/* Function Name: DAC0_PowerControl */
static int32_t DAC0_PowerControl(ARM_POWER_STATE status)
{
  return (DAC_PowerControl(status,&DAC0));
}

/* Function Name: DAC0_Control */
static int32_t DAC0_Control(uint32_t control, uint32_t arg)
{
    return (DAC_Control(control,arg,&DAC0));
}

/* Function Name: DAC0_Start */
static int32_t DAC0_Start(void)
{
    return (DAC_Start(&DAC0));
}

/* Function Name: DAC0_Stop */
static int32_t DAC0_Stop(void)
{
    return (DAC_Stop(&DAC0));
}

/* Function Name: DAC0_SetInput */
static int32_t DAC0_SetInput(uint32_t value)
{
    return (DAC_SetInput(value, &DAC0));
}

extern ARM_DRIVER_DAC Driver_DAC0;
ARM_DRIVER_DAC Driver_DAC0 =
{
        DAC_GetVersion,
        DAC_GetCapabilities,
        DAC0_Initialize,
        DAC0_Uninitialize,
        DAC0_PowerControl,
        DAC0_Control,
        DAC0_Start,
        DAC0_Stop,
        DAC0_SetInput
};

#endif /*RTE_DAC0 */

/* DAC1 driver instance */
#if(RTE_DAC1)

/* DAC1 configuration */
static DAC_resources_t DAC1 = {
        .reg_base       = (DAC_TypeDef_t *)DAC1_BASE,
        .flags          = 0,
        .config         = (RTE_DAC1_INPUT_BYP_MUX_EN << 1) |
                          (RTE_DAC1_BYP_VAL << 2)          |
                          (RTE_DAC1_CAP_CONT <<14)         |
                          (RTE_DAC1_RES_CONT <<18)         |
                          (RTE_DAC1_TWOSCOMP_EN << 22 )    |
                          (RTE_DAC1_IBIAS << 23)
};

/* Function Name: DAC1_Initialize */
static int32_t DAC1_Initialize(void)
{
    return (DAC_Initialize(&DAC1));
}

/* Function Name: DAC1_uninitialize */
static int32_t DAC1_Uninitialize(void)
{
    return (DAC_Uninitialize(&DAC1));
}

/* Function Name: DAC1_PowerControl */
static int32_t DAC1_PowerControl(ARM_POWER_STATE status)
{
  return (DAC_PowerControl(status,&DAC1));
}

/* Function Name: DAC1_Control */
static int32_t DAC1_Control(uint32_t control, uint32_t arg)
{
    return (DAC_Control(control,arg,&DAC1));
}

/* Function Name: DAC1_Start */
static int32_t DAC1_Start(void)
{
    return (DAC_Start(&DAC1));
}

/* Function Name: DAC1_Stop */
static int32_t DAC1_Stop(void)
{
    return (DAC_Stop(&DAC1));
}

/* Function Name: DAC1_SetInput */
static int32_t DAC1_SetInput(uint32_t value)
{
    return (DAC_SetInput(value, &DAC1));
}

extern ARM_DRIVER_DAC Driver_DAC1;
ARM_DRIVER_DAC Driver_DAC1 =
{
        DAC_GetVersion,
        DAC_GetCapabilities,
        DAC1_Initialize,
        DAC1_Uninitialize,
        DAC1_PowerControl,
        DAC1_Control,
        DAC1_Start,
        DAC1_Stop,
        DAC1_SetInput
};

#endif /* RTE_DAC1 */
