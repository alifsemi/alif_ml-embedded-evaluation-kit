/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/****************************************************************************
 * @file     Driver_ADC.c
 * @author   Prabhakar Kumar
 * @email    prabhakar.kumar@alifsemi.com
 * @version  V1.0.0
 * @date     22-feb-2022
 * @brief    CMSIS-Driver for ADC
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#include "Driver_ADC.h"
#include "ADC_dev.h"
#include "analog_config.h"

#define ARM_ADC_DRV_VERISON ARM_DRIVER_VERSION_MAJOR_MINOR(1,0) /*DRIVER VERSION*/

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion ={
    ARM_ADC_API_VERSION,
    ARM_ADC_DRV_VERISON
};

/* Driver Capabilities */
static const ARM_ADC_CAPABILITIES DriverCapabilities = {
    1,    /* Resolution 12 or 20 bits*/
    0     /* Reserved                */
};
/*
 *    @func           : void analog_config()
 *    @brief          : vbat comparator value and register configuration
 *    @parameter      : NONE
 *    @return         : NONE
 */
static void analog_config()
{
    /* vbat register configuration       */
    analog_config_vbat_reg2();

    /* Comparator register configuration */
    analog_config_comp_reg2();
}

/*
 *    @func           : int32_t adc_conifg(uint32_t confg_value, ADC_resources_t *adc)
 *    @brief          : set the ADC configuration
 *    @parameter[1]   : confg_value : value to set the particular REG1 register bit for ADC
 *    @parameter[2]   : adc         : pointer to structure ADC_resources_t
 *    @return         : NONE
 */
static void adc_conifg(uint32_t confg_value, ADC_resources_t *adc)
{
    /* ADC REG1 register configuration for 1st instance only*/
    ADC_TypeDef *config = (ADC_TypeDef *) ADC0_BASE;

    /* assigning value to the register for enabling each ADC instances*/
    config->REG1 |= confg_value;
}

/*
 *    @func           : void adc_set_clock_divisor (ADC_resources_t *adc)
 *    @brief          : Setting the value for the clock divisor and
 *                      clock value should be between 2 to 64
 *    @parameter[1]   : clock_value : value to set clock divisor
 *    @parameter[2]   : adc         : pointer to the ADC_resources_t
 *    @return         : NONE
 */
static int32_t adc_set_clock_divisor(uint32_t clock_value, ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;

    if (clock_value >= ADC_CLOCK_DIV_MIN_VALUE && clock_value <= ADC_CLOCK_DIV_MAX_VALUE)
    {
        adc->reg_base->CLK_DIVISOR = clock_value;
    }
    else
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    return ret;
}

/*
 *    @func           : in32_t adc_set_avg_sample(ADC_resources_t *adc)
 *    @brief          : Setting the average value, the value must be
 *                      up to 256 and value should be power of 2
 *    @parameter[1]   : avg_num_value : value to set average number
 *    @parameter[2]   : adc           : pointer to the ADC_resources_t
 *    @return         : NONE
 */
static int32_t adc_set_avg_sample(uint32_t avg_num_value, ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;

    if((avg_num_value <= ADC_AVG_SAMPLES_FOR_AVG_MAX) && ((avg_num_value % ADC_MOD_BY_TWO) == 0))
    {
        adc->reg_base->AVG_NUM = avg_num_value;
    }
    else
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    return ret;
}

/*
 *    @func           : int32_t adc_set_sample_width(ADC_resources_t *adc)
 *    @brief          : Setting the sample width and value must be between
 *                      2 to 32
 *    @parameter[1]   : sample_width_value : value to set sample width
 *    @parameter[2]   : adc                : pointer to the ADC_resources_t
 *    @return         : NONE
 */
static int32_t adc_set_sample_width(uint32_t sample_width_value, ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;

    if((sample_width_value >= ADC_SAMPLE_WIDTH_MIN_VALUE) && (sample_width_value <= ADC_SAMPLE_WIDTH_MAX_VALUE))
    {
        adc->reg_base->SAMPLE_WIDTH = sample_width_value;
    }
    else
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    return ret;
}

/*
 *    @func           : void adc_set_n_shift_bit(ADC_resources_t *adc)
 *    @brief          : Setting the number of shift to bit (as per user input)
 *    @parameter[1]   : shift number             : number of bytes to shift
 *    @parameter[2]   : shift_left_right_control : enable shift control 0 for left
 *                                                 1 for right
 *    @parameter[3]   : adc                      : pointer to the ADC_resources_t
 *    @return         : NONE
*/
static int32_t adc_set_n_shift_bit(uint32_t shift_number,
                                   uint32_t shift_left_right_control,
                                   ADC_resources_t *adc)
{
    int ret =ARM_DRIVER_OK;

    if(!(shift_left_right_control == 0 || shift_left_right_control == 1))
        return ARM_DRIVER_ERROR_PARAMETER;

    adc->reg_base->SHIFT_CONTROL = (shift_number | shift_left_right_control << 16);

    return ret;
}

/*
 *    @func           : int32_t adc_control_module(bool ctrl, ADC_resources_t *adc)
 *    @brief          : set the start src register to start ADC
 *    @parameter[1]   : ctrl          : Enable/Disable
 *    @parameter[2]   : adc           : pointer to the ADC_resources_t
 *    @return         : ARM_DRIVER_OK : if function start the ADC Successfully
 */
static int32_t adc_control_module(bool ctrl, ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;

    if(ctrl)
    {
        adc->reg_base->START_SRC |= (ADC_START_SRC);
    }
    else
    {
        adc->reg_base->START_SRC &= ~(ADC_START_SRC);
    }
    return ret;
}

/*
 *    @func           : int32_t adc_enable_irq(ADC_resources_t *adc, uint32_t arg)
 *    @brief          : Enable the irq
 *    @parameter[1]   : adc           : pointer to the ADC_resources_t
 *    @return         : ARM_DRIVER_OK : if enabling the adc irq is successful
 */

static int32_t adc_enable_irq(ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;

    /* enable the interrupt(unmask the interrupt 0x0)*/
    adc->reg_base->INTERRUPT_MASK = 0x0;

    /* enable the NVIC interrupt. */
    NVIC_EnableIRQ(adc->intr_done_irq_num);

    return ret;
}

/*
 *    @func           : void adc_disable_irq(ADC_resources_t *adc, uint32_t arg)
 *    @brief          : disable the interrupt and disable IRQ
 *    @parameter[1]   : adc : pointer to /ref ADC_resources_t
 *    @return         : NONE
 */
static void adc_disable_irq(ADC_resources_t *adc)
{
    /* disable the interrupt(mask the interrupt 0xf) */
    adc->reg_base->INTERRUPT_MASK = 0xF;

    /* Disable the irq */
    NVIC_DisableIRQ(adc->intr_done_irq_num);

}

/*
 *    @func           : int32_t adc_shift_controller(ADC_resources_t *adc)
 *    @brief          : control for left or right shift of bit
 *    @parameter[1]   : ctrl : give control option for left or right shift
 *    @parameter[2]   : adc  : pointer to /ref ADC_resources_t
 *    @return         : NONE
 */
static void adc_shift_controller(bool ctrl, ADC_resources_t *adc)
{
    if(ctrl)
    {
        adc->reg_base->SHIFT_CONTROL &= ~(ADC_SHIFT_CONTROL_RIGHT_OR_LEFT);
    }
    else
    {
        adc->reg_base->SHIFT_CONTROL |= (ADC_SHIFT_CONTROL_RIGHT_OR_LEFT);
    }
}

/*
 *    @func           : int32_t adc_sequencer_msk_ch_control(uint32_t arg, ADC_resources_t *adc)
 *    @brief          : Masking the channel which are not required
 *    @parameter[1]   : arg  : value for masking the un-required channel
 *    @parameter[2]   : adc  : pointer to the /ref ADC_resources_t
 *    @return         : ARM_DRIVER_OK              : if driver initialized successfully
 *                    : ARM_DRIVER_ERROR_PARAMETER : if parameter is invalid or not
 */
static int32_t adc_sequencer_msk_ch_control(uint32_t arg, ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;

    if(!(arg < ADC_MSK_ALL_CHANNELS))
         return ARM_DRIVER_ERROR_PARAMETER;

    /* clearing the previous mask bits */
    adc->reg_base->SEQUENCER_CTRL &= ~(ADC_MSK_ALL_CHANNELS);

    /* masking the channels */
    adc->reg_base->SEQUENCER_CTRL |= arg;

    return ret;
}

/*
 *    @func           : int32_t adc_init_channel_select(uint32_t arg, ADC_resources_t *adc)
 *    @brief          : control to select initial channel for storing sample value
 *    @parameter[1]   : arg  : selecting initial channel
 *    @parameter[2]   : adc  : pointer to the /ref ADC_resources_t
 *    @return         : ARM_DRIVER_OK              : if driver initialized successfully
 *                    : ARM_DRIVER_ERROR_PARAMETER : if parameter is invalid or not
 */
static int32_t adc_init_channel_select(uint32_t arg, ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;

    if(!(arg < ADC_MAX_INIT_CHANNEL))
         return ARM_DRIVER_ERROR_PARAMETER;

    /* clearing the channels */
    adc->reg_base->SEQUENCER_CTRL &= ~(ADC_MSK_INIT_CHANNEL << ADC_INIT_SHIFT_BIT);

    /* masking the channels */
    adc->reg_base->SEQUENCER_CTRL |= (arg << ADC_INIT_SHIFT_BIT);

    return ret;
}

/*
 *    @func           : int32_t adc_sequencer_control(bool ctrl, ADC_resources_t *adc)
 *    @brief          : control to rotate through all channels or fixed at particular channel
 *    @parameter[1]   : ctrl : provide the enable or disable parameter to function
 *    @parameter[2]   : adc  : pointer to the /ref ADC_resources_t
 *    @return         : NONE
 */
static void adc_sequencer_control(bool ctrl, ADC_resources_t *adc)
{
    if(ctrl)
    {
        /* set to single input scan */
        adc->reg_base->SEQUENCER_CTRL |= (ADC_SEQUENCER_CTRL_FIXED_OR_ROTATE );
        adc->config->sequencer_ctrl_status = ARM_ADC_CH_SINGLE_SCAN;
    }
    else
    {
        /* Set to continuous input scan */
        adc->reg_base->SEQUENCER_CTRL &= ~(ADC_SEQUENCER_CTRL_FIXED_OR_ROTATE );
        adc->config->sequencer_ctrl_status = ARM_ADC_CH_CONTINOUS_SCAN;
    }
}

/*
 *    @func           : int32_t adc_process_control(ADC_resources_t *adc)
 *    @brief          : control adc process control for enabling comparator
 *    @parameter[1]   : ctrl : provide the enable or disable parameter to function
 *    @parameter[2]   : adc  : pointer to the /ref ADC_resources_t
 *    @return         : NONE
 */
static int32_t adc_process_control(bool ctrl, ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;
    if(ctrl)
    {
        adc->reg_base->CONTROL |= (ADC_PROCESS_CONTROL);
        /* storing value of comparator control */
        adc->config->comp_ctrl_status = ENABLE;
    }
    else
    {
        adc->reg_base->CONTROL &= ~(ADC_PROCESS_CONTROL);
        adc->config->comp_ctrl_status = DISABLE;
    }
    return ret;
}

/*
 *    @func           : int32_t ADC_Initialize (ARM_ADC_SignalEvent_t cb_event)
 *    @brief          : initialize the following:
 *                      - Access vbat and comparator register configuration
 *                      - Control access of reg1 register
 *                      - set the clock divisor
 *                      - set the average sample number
 *                      - set the sample width
 *                      - set the number of shift bit
 *                      - Default single input scan mode
 *                      - Disable the interrupt mask
 *                      - store call back event
 *                      - set flag for driver initialization
 *    @parameter[1]   : cb_event : Pointer to /ref ARM_ADC_Signal_Event_t cb_event
 *    @parameter[2]   : adc      : Pointer to /ref ADC_resources_t structure
 *    @return         : ARM_DRIVER_OK              : if driver initialized successfully
 *                    : ARM_DRIVER_ERROR_PARAMETER : if parameter is invalid or not
 */
static int32_t ADC_Initialize(ARM_ADC_SignalEvent_t cb_event, ADC_resources_t *adc)
{
    int ret =  ARM_DRIVER_OK ;

    if(!cb_event)
        return ARM_DRIVER_ERROR_PARAMETER;

    /*function include vbat and comparator address and it value */
    analog_config();

    /* function reg1 register */
    adc_conifg(adc->adc_reg1_value, adc);

    /* set user channel input */
    ret = adc_init_channel_select(adc->config->user_input,adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set the clock divisor */
    ret = adc_set_clock_divisor(adc->config->clock_div, adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set avg sample value */
    ret = adc_set_avg_sample(adc->config->avg_sample_num, adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set Sample width value */
    ret = adc_set_sample_width(adc->config->sample_width, adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set number of n shift bits */
    ret = adc_set_n_shift_bit(adc->config->shift_n_bit, adc->config->shift_left_or_right, adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set sequencer control */
    adc_sequencer_control(ENABLE, adc);

    /* Disable the interrupt (mask the interrupt(0xF))*/
    adc->reg_base->INTERRUPT_MASK = 0xF;

    /* User call back Event */
    adc-> cb_event = cb_event;

    /* Setting flag to initialize */
    adc->flags |= ADC_FLAG_DRV_INIT_DONE;

    return ret;
}

/*
 *    @func           : int32_t ADC_Uninitialize (ARM_ADC_SignalEvent_t cb_event)
 *    @brief          : Uninitialize the adc device
 *                      - Access vbat and comparator address
 *                      - Control access of reg1 register
 *                      - Reset the clock divisor
 *                      - Reset the average sample number
 *                      - Reset the sample width
 *                      - Reset the number of shift bit
 *                      - Reset last read channel
 *                      - Disable the interrupt mask
 *                      - set call back event to NULL
 *                      - set flag to 0
 *    @parameter[in]  : adc    : Pointer to the structure ADC_resources_t
 *    @return         : ARM_DRIVER_OK              : if adc is successfully initialized
 *                    : ARM_DRIVER_ERROR_PARAMETER : if adc device is invalid
 */
static int32_t ADC_Uninitialize( ADC_resources_t *adc)
{
    int ret = ARM_DRIVER_OK;

    /* parameter checking */
    if(!adc)
        return ARM_DRIVER_ERROR_PARAMETER;

    /* Checking initialized has done or not */
    if(!(adc->flags & ADC_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_OK;
    /* set the clock divisor */
    ret = adc_set_clock_divisor(ADC_CLOCK_DIV_MIN_VALUE, adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set avg sample value */
    ret = adc_set_avg_sample(ADC_AVG_SAMPLES_FOR_AVG_MIN, adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set Sample width value */
    ret = adc_set_sample_width(ADC_SAMPLE_WIDTH_MIN_VALUE, adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set number of n shift bits */
    adc_set_n_shift_bit(0,0, adc);
    if(ret != ARM_DRIVER_OK)
         return ARM_DRIVER_ERROR;

    /* set sequencer control */
    adc_sequencer_control(ENABLE, adc);

    /* Disable the interrupt (mask the interrupt(0xF)) */
    adc->reg_base->INTERRUPT_MASK = 0xF;

    /* set call back to NULL */
    adc->cb_event = NULL;

    /* Reset last read channel */
    adc->config->last_read_channel = 0;

    /* flags */
    adc->flags = 0;

    return ret;
}

/*
 *    @func         : int32_t ADC_PowerControl(ARM_POWER_status status, ADC_resources_t *adc)
 *    @brief        : power the driver and enable NVIC
 *    @parameter[1] : state            : power state
 *    @parameter[2] : adc              : pointer to /ref ADC_resources_t
 *    @return       : ARM_DRIVER_OK    : if power done successful
 *                    ARM_DRIVER_ERROR : if initialize is not done
*/
static int32_t ADC_PowerControl(ARM_POWER_STATE state, ADC_resources_t *adc)
{
    int32_t ret = ARM_DRIVER_OK;
    switch(state)
    {
        case ARM_POWER_FULL:

            if (!(adc->flags & ADC_FLAG_DRV_INIT_DONE))
                return ARM_DRIVER_ERROR;

            if ((adc->flags & ADC_FLAG_DRV_POWER_DONE))
                return ARM_DRIVER_OK;

            /* Clear Any Pending IRQ */
            NVIC_ClearPendingIRQ (adc->intr_done_irq_num);

            /* Set priority */
            NVIC_SetPriority (adc->intr_done_irq_num, adc->intr_done_irq_priority);

            /* Enable the NIVC */
            NVIC_EnableIRQ (adc->intr_done_irq_num);

            /* Set the power flag enabled */
            adc->flags |= ADC_FLAG_DRV_POWER_DONE;

            break;

        case ARM_POWER_OFF:

            /* Disable ADC NVIC */
            NVIC_DisableIRQ (adc->intr_done_irq_num);

            /* Clear Any Pending IRQ */
            NVIC_ClearPendingIRQ (adc->intr_done_irq_num);

            /* Reset the power status of ADC */
            adc->flags &= ~ADC_FLAG_DRV_POWER_DONE;

            break;

        case ARM_POWER_LOW:
        default:
             return ARM_DRIVER_ERROR_UNSUPPORTED;
            break;
    }
        return ret;
}

/*
 *    @func           : int32_t ADC_Start(void *data, uint32_t num, ADC_resources_t *adc)
 *    @brief          : this will start the adc and initialize interrupt
 *    @parameter[1]   : data : pointer to input data
 *    @parameter[2]   : num  : number of data
 *    @parameter[3]   : adc  : pointer to ADC_resources_t structure
 *    @return         : ARM_DRIVER_OK              : if the function are return successful
 *                      ARM_DRIVER_ERROR_PARAMETER : if parameter are invalid
 */
static int32_t ADC_Start(uint32_t *data, uint32_t num, ADC_resources_t *adc)
{
    int ret = ARM_DRIVER_OK;

    if (!(adc->flags & ADC_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_ERROR;

    if (!(adc->flags & ADC_FLAG_DRV_POWER_DONE))
        return ARM_DRIVER_ERROR;

    conv_info_t *ADC_info_ptr = adc->conv_info;

    /* checking data is present or not */
    if(data == NULL || num == 0U)
        return ARM_DRIVER_ERROR_PARAMETER;

    if(adc->conv_status.conv_busy == 1U)
        return ARM_DRIVER_ERROR_BUSY;

    /* setup conversion structure as per info */
    ADC_info_ptr->conv_buff   = (uint32_t *) data;
    ADC_info_ptr->total_cnt   = num;
    ADC_info_ptr->current_cnt = 0;

    /* active the conv busy flag */
    adc->conv_status.conv_busy = 1U;

    /* enable the irq */
    ret = adc_enable_irq(adc);

    /* Start the ADC */
    adc_control_module(ENABLE,adc);

    return ret;
}

/*
 *    @func           : int32_t ADC_Stop( ADC_resources_t *adc)
 *    @brief          : disable the adc
 *    @parameter      : NONE
 *    @return         : ARM_DRIVER_OK : if function return successfully
 */
static int32_t ADC_Stop( ADC_resources_t *adc)
{
    int ret = ARM_DRIVER_OK;

    if (!(adc->flags & ADC_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_ERROR;

    if (!(adc->flags & ADC_FLAG_DRV_POWER_DONE))
        return ARM_DRIVER_ERROR;

    /* Disable the adc */
    ret = adc_control_module(DISABLE,adc);

    return ret;
}

/*
 *    @func         : in32_t ADC_Control(uint32_t control , uint32_t arg, ADC_resources_t adc)
 *    @brief        : control the following
 *                    - ARM_SET_SHIFT_CONTROL             : to control shift control of bits
 *                    - ARM_SET_SEQUENCER_CTRL            : selecting sample individual or rotate through
 *                                                          each unmasked sample
 *                    - ARM_ADC_SEQUENCER_MSK_CTRL        : to control masking of the channel
 *                    - ARM_ADC_CHANNEL_INIT_VAL          : to select initial channel for storing
 *                    - ARM_SET_ADC_COMPARATOR_A          : to set comparator a value
 *                    - ARM_SET_ADC_COMPARATOR_B          : to set comparator b value
 *                    - ARM_SET_ADC_THRESHOLD_COMPARISON  : to set the threshold comparison
 *                    - ARM_SET_ADC_COMPARATOR_CONTROLLER : to control comparator
 *    @parameter[1] : Control : Selecting the operation
 *    @parameter[2] : arg     : values for the the operation
 *    @return[1]    : ARM_DRIVER_OK              : if function return successfully
 *    @return[2]    : ARM_DRIVER_ERROR_PARAMETER : if adc parameter are invalid
 */
static int32_t ADC_Control(uint32_t Control, uint32_t arg, ADC_resources_t *adc)
{
    int ret = ARM_DRIVER_OK;

    if (!(adc->flags & ADC_FLAG_DRV_INIT_DONE))
        return ARM_DRIVER_ERROR;

    if (!(adc->flags & ADC_FLAG_DRV_POWER_DONE))
        return ARM_DRIVER_ERROR;

    switch(Control)
    {
        case ARM_ADC_SHIFT_CONTROL:

            /*selecting the mode for the shifting bit left(0) or right(1) */
            adc_shift_controller(arg, adc);

        break;

        case ARM_ADC_SEQUENCER_CTRL:

            if(!(arg == 0 || arg == 1))
                return ARM_DRIVER_ERROR_PARAMETER;

            /*selecting the mode of control for taking fixed sample(1) or rotate through the all sample(0)*/
            adc_sequencer_control(arg, adc);

        break;

        case ARM_ADC_SEQUENCER_MSK_CH_CTRL:

            if(!(arg < ADC_MSK_ALL_CHANNELS))
                 return ARM_DRIVER_ERROR_PARAMETER;

            /* set channel to be masked */
            ret = adc_sequencer_msk_ch_control(arg, adc);

        break;

        case ARM_ADC_CHANNEL_INIT_VAL:

            if(!(arg < ADC_MAX_INIT_CHANNEL))
                 return ARM_DRIVER_ERROR_PARAMETER;

            /* selecting the initial value */
            ret = adc_init_channel_select(arg, adc);

            adc->config->user_input = arg;

        break;

        case ARM_ADC_COMPARATOR_A:

            /* set comparator A */
            adc->reg_base->COMP_THRESH_A = arg;

        break;

        case ARM_ADC_COMPARATOR_B:

            /* set comparator B */
            adc->reg_base->COMP_THRESH_B = arg;

        break;

        case ARM_ADC_THRESHOLD_COMPARISON:

            if(!(arg < 3))
                return ARM_DRIVER_ERROR_PARAMETER;
            /* set comparison control bit */
              adc->reg_base->CONTROL = (arg << 16);

        break;

        case ARM_ADC_COMPARATOR_CONTROLLER:

            if(!(arg == 0 || arg == 1))
                return ARM_DRIVER_ERROR_PARAMETER;

            /* set threshold comparison control*/
            adc_process_control(arg, adc);
        break;

        default:
            return ARM_DRIVER_ERROR_PARAMETER;
    }

    return ret;
}

/*
 *    @func           : void ADC_intr_done_IRQ_handler(ADC_resources_t *adc)
 *    @brief          : irq handle for the particular interrupt
 *    @parameter[1]   : adc : pointer to the structure
 *    @return         : NONE
 */
void ADC_intr_done_IRQ_handler(ADC_resources_t *adc)
{
    int32_t event           = 0U;                                          /* storing event call                     */
    volatile uint32_t *sample_reg_ptr = &adc->reg_base->SAMPLE_REG_0;

    /* Clearing the done IRQ*/
    adc->reg_base->INTERRUPT = ADC_INTR_DONE_CLEAR;

    /* Single scan mode */
    if ((adc->config->sequencer_ctrl_status) == ARM_ADC_CH_SINGLE_SCAN)
    {
        /* read sample and store to user memory */
        *(adc->conv_info->conv_buff + adc->conv_info->current_cnt) = *(sample_reg_ptr + adc->config->user_input);

    }

    /* Continuous scan mode */
    if ((adc->config->sequencer_ctrl_status) == ARM_ADC_CH_CONTINOUS_SCAN)
    {

        /* validation for last read channel */
        if(adc->config->last_read_channel > ADC_LAST_AVAILABLE_CHANNEL)
        {
            adc->config->last_read_channel = 0;
        }

        while((adc->reg_base->SEQUENCER_CTRL >> adc->config->last_read_channel) & ADC_SEQUENCER_MSK_BIT)
        {
            if(adc->config->last_read_channel >= ADC_LAST_AVAILABLE_CHANNEL)
            {
                adc->config->last_read_channel = 0;
            }
            else
            {
		adc->config->last_read_channel ++;
            }
         }

        *(adc->conv_info->conv_buff + adc->conv_info->current_cnt) = *(sample_reg_ptr + adc->config->last_read_channel);

        adc->config->last_read_channel++;
    }

    adc->conv_info->current_cnt += 1;

    if(adc->conv_info->current_cnt >= adc->conv_info->total_cnt)
    {
        /* disable irq */
        adc_disable_irq(adc);

        /* set conv_busy flag to 0U */
        adc->conv_status.conv_busy = 0U;

        event |= ARM_ADC_EVENT_CONVERSION_COMPLETE;
    }

    /* only one interrupt is working so comparator interrupt we are checking in this
     * handler based on the interrupt status */

    /* below is for comparator0 interrupt  */
    if((adc->config->comp_ctrl_status) == ENABLE && (adc->reg_base->INTERRUPT & ADC_INTR_COMP0_MSK))
    {
        switch ((adc->reg_base->CONTROL >> ADC_SHIFT_BIT) & ADC_THRSHLD_COMP_MASK_BIT)
        {
        case ADC_COMP_THRHLD_ABOVE_A:

               event |= ARM_ADC_COMPARATOR_THRESHOLD_ABOVE_A;
               break;
        case ADC_COMP_THRHLD_BELOW_A:

               event |= ARM_ADC_COMPARATOR_THRESHOLD_BELOW_A;
               break;

        case ADC_COMP_THRHLD_BETWEEN_A_B:

               event |= ARM_ADC_COMPARATOR_THRESHOLD_BETWEEN_A_B;
               break;
        }
    adc->reg_base->INTERRUPT = ADC_INTR_COMP0_CLEAR;
    }

    /* below is for comparator1 */
    if((adc->config->comp_ctrl_status) == ENABLE && (adc->reg_base->INTERRUPT & ADC_INTR_COMP1_MSK))
    {
        switch ((adc->reg_base->CONTROL >> ADC_SHIFT_BIT) & ADC_THRSHLD_COMP_MASK_BIT)
        {
        case ADC_COMP_THRHLD_ABOVE_B:

               event |= ARM_ADC_COMPARATOR_THRESHOLD_ABOVE_B;
               break;
        case ADC_COMP_THRHLD_BELOW_B:

               event |= ARM_ADC_COMPARATOR_THRESHOLD_BELOW_B;
               break;

        case ADC_COMP_THRHLD_OUTSIDE_A_B:

               event |= ARM_ADC_COMPARATOR_THRESHOLD_OUTSIDE_A_B;
               break;
        }
    adc->reg_base->INTERRUPT = ADC_INTR_COMP1_CLEAR;
    }
    if((event != 0U) && (adc->cb_event != NULL))
    {
        /* call user callback */
        adc->cb_event(event);
    }

}

/* RTE_ADC0 */
#if (RTE_ADC0)

static ADC_config_info_t ADC0_Config ={
        .user_input             = RTE_ADC0_INPUT_NUM,                    /* user input             */
        .clock_div              = RTE_ADC0_CLOCK_DIV,                    /* clock divisor          */
        .sample_width           = RTE_ADC0_SAMPLE_WIDTH,                 /* sample width           */
        .avg_sample_num         = RTE_ADC0_AVG_SAMPLE_NUM,               /* average sample number  */
        .shift_n_bit            = RTE_ADC0_SHIFT_N_BIT,                  /* number of shift bit    */
        .shift_left_or_right    = RTE_ADC0_SHIFT_LEFT_OR_RIGHT,          /* shifting left to right */
        .comp_ctrl_status       = 0,                                     /* comparator status      */
        .last_read_channel      = 0                                      /* last read channel       */
};

static conv_info_t ADC0_conv_info ={0};                                  /* conversion value initialized to 0 */

static ADC_resources_t ADC0 = {
        .cb_event                   = NULL,                                  /* call back                    */
        .config                     = &ADC0_Config,                          /* ADC configuration (run time) */
        .reg_base                   = (ADC_TypeDef *) ADC0_BASE,             /* ADC register base address    */
        .conv_info                  = &ADC0_conv_info,                       /* ADC conversion info          */
        .conv_status                = {0},                                   /* ADC conversion status        */
        .flags                      = 0,                                     /* flag                         */
        .intr_done_irq_num          = (IRQn_Type) ADC0_INTR_DONE_IRQ,        /* ADC IRQ number               */
        .intr_done_irq_priority     = (uint32_t) RTE_ADC0_IRQ_PRIORITY,      /* ADC irq priority             */
        .adc_reg1_value             = (RTE_ADC0_TEST_EN << 0)                    |
                                      (RTE_ADC0_DIFFERENTIAL_EN << 1)            |
                                      (RTE_ADC0_COMPARATOR_EN << 2)              |
                                      (RTE_ADC0_COMPARATOR_BIAS << 3)            |
                                      (RTE_ADC0_VCM_RDIV_EN << 5)                |
                                      (RTE_ADC_CONFG_RESERVED_bits_18_23 << 18)  |
                                      (RTE_ADC_CONFG_amux_cont << 24)
};

/**
 @fn       ARM_DRIVER_VERSION ADC0_GetVersion(void)
 @brief    Get ADC0 VERSION
 @return   DriverVersion
**/
static ARM_DRIVER_VERSION ADC0_GetVersion(void)
{
  return DriverVersion;
}
/**
 @fn       ARM_ADC0_CAPABILITIES ADC0_GetCapabilities(void)
 @brief    Get ADC0 CAPABILITIES
 @return   DriverCapabilities
**/
static ARM_ADC_CAPABILITIES ADC0_GetCapabilities(void)
{
  return DriverCapabilities;
}

/*Function Name : ADC0_Intialize*/
static int32_t ADC0_Initialize(ARM_ADC_SignalEvent_t cb_event)
{
  return (ADC_Initialize(cb_event, &ADC0));
}

/*Function Name : ADC0_Unintialize*/
static int32_t ADC0_Uninitialize(void)
{
  return (ADC_Uninitialize(&ADC0));
}

/*Function Name : ADC0_Stop*/
static int32_t ADC0_Start(uint32_t *data, uint32_t num)
{
  return (ADC_Start(data, num, &ADC0));
}

/*Function Name : ADC0_Stop*/
static int32_t ADC0_Stop(void)
{
  return (ADC_Stop(&ADC0));
}

/*Function Name : ADC0_PowerControl*/
static int32_t ADC0_PowerControl(ARM_POWER_STATE status)
{
  return(ADC_PowerControl(status, &ADC0));
}

/*Function Name : ADC0_Control*/
static int32_t ADC0_Control(uint32_t Control, uint32_t arg)
{
  return (ADC_Control(Control, arg, &ADC0));
}

/*Function Name : ADC0_INTR_DONE_IRQHandler*/
void ADC0_INTR_DONE_IRQHandler(void)
{
  return (ADC_intr_done_IRQ_handler(&ADC0));
}

extern ARM_DRIVER_ADC Driver_ADC0;
ARM_DRIVER_ADC Driver_ADC0 ={
    ADC0_GetVersion,
    ADC0_GetCapabilities,
    ADC0_Initialize,
    ADC0_Uninitialize,
    ADC0_Start,
    ADC0_Stop,
    ADC0_PowerControl,
    ADC0_Control
};
#endif /* RTE_ADC0 */

/* RTE_ADC1 */
#if (RTE_ADC1)

static ADC_config_info_t ADC1_Config ={
        .user_input             = RTE_ADC1_INPUT_NUM,                    /* user input             */
        .clock_div              = RTE_ADC1_CLOCK_DIV,                    /* clock divisor          */
        .sample_width           = RTE_ADC1_SAMPLE_WIDTH,                 /* sample width           */
        .avg_sample_num         = RTE_ADC1_AVG_SAMPLE_NUM,               /* average sample number  */
        .shift_n_bit            = RTE_ADC1_SHIFT_N_BIT,                  /* number of shift bit    */
        .shift_left_or_right    = RTE_ADC1_SHIFT_LEFT_OR_RIGHT,          /* shifting left to right */
        .comp_ctrl_status       = 0,                                     /* comparator status      */
        .last_read_channel      = 0                                      /* last read channel       */
};

static conv_info_t ADC1_conv_info = {0};

static ADC_resources_t ADC1 = {
        .cb_event                   = NULL,                                  /* ARM_ADC_SignalEvent_t        */
        .config                     = &ADC1_Config,                          /* ADC configuration (run time) */
        .reg_base                   = (ADC_TypeDef *) ADC1_BASE,             /* ADC register base address    */
        .conv_info                  = &ADC1_conv_info,                       /* ADC conversion info          */
        .conv_status                = {0},                                   /* ADC conversion status        */
        .flags                      = 0,                                     /* flag                         */
        .intr_done_irq_num          = (IRQn_Type) ADC1_INTR_DONE_IRQ,        /* ADC IRQ number               */
        .intr_done_irq_priority     = (uint32_t) RTE_ADC0_IRQ_PRIORITY,      /* ADC irq priority             */
        .adc_reg1_value             = (RTE_ADC1_TEST_EN << 6)                     |
                                      (RTE_ADC1_DIFFERENTIAL_EN << 7)             |
                                      (RTE_ADC1_COMPARATOR_EN << 8)               |
                                      (RTE_ADC1_COMPARATOR_BIAS << 9)             |
                                      (RTE_ADC1_VCM_RDIV_EN << 11)                |
                                      (RTE_ADC_CONFG_RESERVED_bits_18_23 << 18)   |
                                      (RTE_ADC_CONFG_amux_cont << 24)
};

/**
 @fn       ARM_DRIVER_VERSION ADC1_GetVersion(void)
 @brief    Get ADC1 VERSION
 @return   DriverVersion
**/
static ARM_DRIVER_VERSION ADC1_GetVersion(void)
{
  return DriverVersion;
}
/**
 @fn       ARM_ADC1_CAPABILITIES ADC1_GetCapabilities(void)
 @brief    Get ADC1 CAPABILITIES
 @return   DriverCapabilities
**/
static ARM_ADC_CAPABILITIES ADC1_GetCapabilities(void)
{
  return DriverCapabilities;
}

/*Function Name : ADC1_Intialize*/
static int32_t ADC1_Initialize(ARM_ADC_SignalEvent_t cb_event)
{
  return (ADC_Initialize(cb_event, &ADC1));
}

/*Function Name : ADC1_Unintialize*/
static int32_t ADC1_Uninitialize(void)
{
  return (ADC_Uninitialize(&ADC1));
}

/*Function Name : ADC1_Start*/
static int32_t ADC1_Start(uint32_t *data, uint32_t num)
{
  return (ADC_Start(data, num, &ADC1));
}

/*Function Name : ADC1_Stop*/
static int32_t ADC1_Stop(void)
{
  return (ADC_Stop(&ADC1));
}

/*Function Name : ADC1_PowerControl*/
static int32_t ADC1_PowerControl(ARM_POWER_STATE status)
{
  return(ADC_PowerControl(status, &ADC1));
}

/*Function Name : ADC1_Control*/
static int32_t ADC1_Control(uint32_t Control, uint32_t arg)
{
  return (ADC_Control(Control, arg, &ADC1));
}

/*Function Name : ADC1_INTR_DONE_IRQHandler*/
void ADC1_INTR_DONE_IRQHandler(void)
{
  return (ADC_intr_done_IRQ_handler(&ADC1));
}

extern ARM_DRIVER_ADC Driver_ADC1;
ARM_DRIVER_ADC Driver_ADC1 ={
    ADC1_GetVersion,
    ADC1_GetCapabilities,
    ADC1_Initialize,
    ADC1_Uninitialize,
    ADC1_Start,
    ADC1_Stop,
    ADC1_PowerControl,
    ADC1_Control
};
#endif /* RTE_ADC1 */

/* RTE_ADC2 */
#if (RTE_ADC2)

static ADC_config_info_t ADC2_Config ={
        .user_input             = RTE_ADC2_INPUT_NUM,                    /* user input             */
        .clock_div              = RTE_ADC2_CLOCK_DIV,                    /* clock divisor          */
        .sample_width           = RTE_ADC2_SAMPLE_WIDTH,                 /* sample width           */
        .avg_sample_num         = RTE_ADC2_AVG_SAMPLE_NUM,               /* average sample number  */
        .shift_n_bit            = RTE_ADC2_SHIFT_N_BIT,                  /* number of shift bit    */
        .shift_left_or_right    = RTE_ADC2_SHIFT_LEFT_OR_RIGHT,          /* shifting left to right */
        .comp_ctrl_status       = 0,                                     /* comparator status      */
        .last_read_channel      = 0                                      /* last read channel      */
};

static conv_info_t ADC2_conv_info ={0};

static ADC_resources_t ADC2 = {
        .cb_event                   = NULL,                                  /* ARM_ADC_SignalEvent_t        */
        .config                     = &ADC2_Config,                          /* ADC configuration (run time) */
        .reg_base                   = (ADC_TypeDef *) ADC2_BASE,             /* ADC register base address    */
        .conv_info                  = &ADC2_conv_info,                       /* ADC conversion info          */
        .conv_status                = {0},                                   /* ADC conversion status        */
        .flags                      = 0,                                     /* flag                         */
        .intr_done_irq_num          = (IRQn_Type) ADC2_INTR_DONE_IRQ,        /* ADC IRQ number               */
        .intr_done_irq_priority     = (uint32_t) RTE_ADC0_IRQ_PRIORITY,      /* ADC irq priority             */
        .adc_reg1_value             = (RTE_ADC2_TEST_EN << 12)                    |
                                      (RTE_ADC2_DIFFERENTIAL_EN << 13)            |
                                      (RTE_ADC2_COMPARATOR_EN << 14)              |
                                      (RTE_ADC2_COMPARATOR_BIAS << 15)            |
                                      (RTE_ADC2_VCM_RDIV_EN << 17)                |
                                      (RTE_ADC_CONFG_RESERVED_bits_18_23 << 18)   |
                                      (RTE_ADC_CONFG_amux_cont << 24)
};

/**
 @fn       ARM_DRIVER_VERSION ADC1_GetVersion(void)
 @brief    Get ADC1 VERSION
 @return   DriverVersion
**/
static ARM_DRIVER_VERSION ADC2_GetVersion(void)
{
  return DriverVersion;
}
/**
 @fn       ARM_ADC2_CAPABILITIES ADC2_GetCapabilities(void)
 @brief    Get ADC2 CAPABILITIES
 @return   DriverCapabilities
**/
static ARM_ADC_CAPABILITIES ADC2_GetCapabilities(void)
{
  return DriverCapabilities;
}

/*Function Name : ADC2_Intialize*/
static int32_t ADC2_Initialize(ARM_ADC_SignalEvent_t cb_event)
{
  return (ADC_Initialize(cb_event, &ADC2));
}

/*Function Name : ADC2_Unintialize*/
static int32_t ADC2_Uninitialize(void)
{
  return (ADC_Uninitialize(&ADC2));
}

/*Function Name : ADC2_Start*/
static int32_t ADC2_Start(uint32_t *data, uint32_t num)
{
  return (ADC_Start(data, num, &ADC2));
}

/*Function Name : ADC2_Stop*/
static int32_t ADC2_Stop(void)
{
  return (ADC_Stop(&ADC2));
}

/*Function Name : ADC2_PowerControl*/
static int32_t ADC2_PowerControl(ARM_POWER_STATE status)
{
  return(ADC_PowerControl(status, &ADC2));
}

/*Function Name : ADC2_Control*/
static int32_t ADC2_Control(uint32_t Control, uint32_t arg)
{
  return (ADC_Control(Control, arg, &ADC2));
}

/*Function Name : ADC2_INTR_DONE_IRQHandler*/
void ADC2_INTR_DONE_IRQHandler(void)
{
  return (ADC_intr_done_IRQ_handler(&ADC2));
}

extern ARM_DRIVER_ADC Driver_ADC2;
ARM_DRIVER_ADC Driver_ADC2 ={
    ADC2_GetVersion,
    ADC2_GetCapabilities,
    ADC2_Initialize,
    ADC2_Uninitialize,
    ADC2_Start,
    ADC2_Stop,
    ADC2_PowerControl,
    ADC2_Control
};
#endif /* RTE_ADC2 */
