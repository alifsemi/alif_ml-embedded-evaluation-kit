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
 * @file     Driver_LPTIMER.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for LPTIMER.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "LPTIMER_ll_drv.h"

#if !(RTE_LPTIMER)
#error "LPTIMER is not enabled in RTE_Device.h"
#endif

#if (defined(RTE_Drivers_LPTIMER) && !RTE_LPTIMER)
#error "LPTIMER not configured in RTE_Device.h!"
#endif

static int32_t LPTIMER_Irq_Handler (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    LPTIMER_reg_info *reg_ptr = (LPTIMER_reg_info*) LPTIMER->reg_base;

    (void) (reg_ptr->ch_cntrl_reg[channel].eoi);

    LPTIMER->ch_info[channel].CB_function_ptr(ARM_LPTIMER_EVENT_UNDERFLOW);

    return ARM_DRIVER_OK;
}

static int32_t ARM_LPTIMER_Initialize (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, ARM_LPTIMER_SignalEvent_t cb_event) {

    int32_t ret = ARM_DRIVER_OK;

    if (cb_event == NULL)                                 {   return ARM_DRIVER_ERROR_PARAMETER;    }
    if (channel >= LPTIMER_MAX_CHANNEL_NUMBER)            {   return ARM_DRIVER_ERROR_PARAMETER;    }

    if (LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_INITIALIZED)  {    return ARM_DRIVER_OK;  }

    LPTIMER->ch_info[channel].CB_function_ptr = cb_event;

    ret = LPTIMER_ll_Initialize (LPTIMER, channel);
    if (ret != ARM_DRIVER_OK)                             {    return ret;    }

    LPTIMER->ch_info[channel].flag |= LPTIMER_CHANNEL_INITIALIZED;

    return ARM_DRIVER_OK;
}

static int32_t ARM_LPTIMER_PowerControl (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, ARM_POWER_STATE state) {

    if (channel >= LPTIMER_MAX_CHANNEL_NUMBER)             {   return ARM_DRIVER_ERROR_PARAMETER;    }
    if (state > ARM_POWER_FULL)                            {   return ARM_DRIVER_ERROR_PARAMETER;    }

    switch (state)
    {
        case ARM_POWER_OFF:
        {
            if ((LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_POWERED) == 0) {    return ARM_DRIVER_OK;  }

            LPTIMER_ll_Irq_Disable (channel);

            LPTIMER->ch_info[channel].flag &= ~LPTIMER_CHANNEL_POWERED;

            break;
        }
        case ARM_POWER_FULL:
        {
            if (LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_POWERED)  {    return ARM_DRIVER_OK;  }

            if (LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_INITIALIZED)
            {
                if (LPTIMER_ll_Irq_Enable (LPTIMER, channel) != ARM_DRIVER_OK)
                {
                    return ARM_DRIVER_ERROR;
                }
                LPTIMER->ch_info[channel].flag |= LPTIMER_CHANNEL_POWERED;
            }
            else
            {
                return ARM_DRIVER_ERROR;
            }
            break;
        }
        case ARM_POWER_LOW:
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        }
    return ARM_DRIVER_OK;
}

static int32_t ARM_LPTIMER_Control (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, uint32_t control_code, void *arg) {

    int32_t ret = 0;

    if (arg == NULL)                            {   return ARM_DRIVER_ERROR_PARAMETER;    }
    if (channel >= LPTIMER_MAX_CHANNEL_NUMBER)  {   return ARM_DRIVER_ERROR_PARAMETER;    }
    if (control_code > ARM_LPTIMER_GET_COUNT)   {   return ARM_DRIVER_ERROR_PARAMETER;    }

    if ((LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_POWERED) == 0)  {    return ARM_DRIVER_ERROR;  }

    switch (control_code)
    {
        case ARM_LPTIMER_SET_COUNT1:
        {
            if (LPTIMER->ch_info[channel].mode == LPTIMER_FREE_RUN_MODE)  {    return ARM_DRIVER_ERROR;  }

            ret = LPTIMER_ll_Set_Count1_Value (LPTIMER, channel, arg);

            LPTIMER->ch_info[channel].flag |= LPTIMER_CHANNEL_CONFIGURED_COUNT1;
            break;
        }
        case ARM_LPTIMER_SET_COUNT2:
        {
            if (LPTIMER->ch_info[channel].enable_PWM)
            {
                ret = LPTIMER_ll_Set_Count2_Value (LPTIMER, channel, arg);

                LPTIMER->ch_info[channel].flag |= LPTIMER_CHANNEL_CONFIGURED_COUNT2;
            }
            else
            {
                return ARM_DRIVER_ERROR;
            }
            break;
        }
        case ARM_LPTIMER_GET_COUNT:
        {
            if ((LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_STARTED) == 0)  {    return ARM_DRIVER_ERROR;  }

            LPTIMER_ll_Get_Count_Value (LPTIMER, channel, arg);
            break;
        }
        default:
        {
            return ARM_DRIVER_ERROR;
        }
    }
    return ret;
}

static int32_t ARM_LPTIMER_Start (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    if (channel >= LPTIMER_MAX_CHANNEL_NUMBER)
    {   return ARM_DRIVER_ERROR_PARAMETER;    }

    if (LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_STARTED)
    {   return ARM_DRIVER_OK;    }

    if ((LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_POWERED) == 0)
    {    return ARM_DRIVER_ERROR;  }

    if ((LPTIMER->ch_info[channel].mode == LPTIMER_USER_RUN_MODE) && ((LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_CONFIGURED_COUNT1) == 0))
    {    return ARM_DRIVER_ERROR;  }

    if ((LPTIMER->ch_info[channel].enable_PWM) && ((LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_CONFIGURED_COUNT1) == 0))
    {    return ARM_DRIVER_ERROR;  }

    if ((LPTIMER->ch_info[channel].enable_PWM) && ((LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_CONFIGURED_COUNT2) == 0))
    {    return ARM_DRIVER_ERROR;  }

    LPTIMER_ll_Start (LPTIMER, channel);

    LPTIMER->ch_info[channel].flag |= LPTIMER_CHANNEL_STARTED;

    return ARM_DRIVER_OK;
}

static int32_t ARM_LPTIMER_Stop (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    if (channel >= LPTIMER_MAX_CHANNEL_NUMBER)  {   return ARM_DRIVER_ERROR_PARAMETER;    }

    if ((LPTIMER->ch_info[channel].flag & LPTIMER_CHANNEL_STARTED) == 0)      {   return ARM_DRIVER_OK;    }

    LPTIMER_ll_Stop (LPTIMER, channel);

    LPTIMER->ch_info[channel].flag &= ~LPTIMER_CHANNEL_STARTED;

    return ARM_DRIVER_OK;
}

static int32_t ARM_LPTIMER_Uninitialize (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    if (channel >= LPTIMER_MAX_CHANNEL_NUMBER)  {   return ARM_DRIVER_ERROR_PARAMETER;    }

    if (LPTIMER->ch_info[channel].flag == 0)    {   return ARM_DRIVER_OK;    }

    LPTIMER->ch_info[channel].CB_function_ptr = NULL;

    LPTIMER->ch_info[channel].flag = 0;

    return ARM_DRIVER_OK;
}

LPTIMER_RESOURCES LPTIMER0_Resources = {
    .reg_base   = (uint32_t)LPTIMER_BASE,
    .ch_info[0] = {
        .enable_PWM = RTE_LPTIMER_CHANNEL0_PWM,
        .mode = RTE_LPTIMER_CHANNEL0_FREE_RUN_MODE,
        .clk_src = RTE_LPTIMER_CHANNEL0_CLK_SRC,
        .flag = 0,
        .irq_priority = RTE_LPTIMER_CHANNEL0_IRQ_PRIORITY
    },
    .ch_info[1] = {
        .enable_PWM = RTE_LPTIMER_CHANNEL1_PWM,
        .mode = RTE_LPTIMER_CHANNEL1_FREE_RUN_MODE,
        .clk_src = RTE_LPTIMER_CHANNEL1_CLK_SRC,
        .flag = 0,
        .irq_priority = RTE_LPTIMER_CHANNEL1_IRQ_PRIORITY
    },
    .ch_info[2] = {
        .enable_PWM = RTE_LPTIMER_CHANNEL2_PWM,
        .mode = RTE_LPTIMER_CHANNEL2_FREE_RUN_MODE,
        .clk_src = RTE_LPTIMER_CHANNEL2_CLK_SRC,
        .flag = 0,
        .irq_priority = RTE_LPTIMER_CHANNEL2_IRQ_PRIORITY
    },
    .ch_info[3] = {
        .enable_PWM = RTE_LPTIMER_CHANNEL3_PWM,
        .mode = RTE_LPTIMER_CHANNEL3_FREE_RUN_MODE,
        .clk_src = RTE_LPTIMER_CHANNEL3_CLK_SRC,
        .flag = 0,
        .irq_priority = RTE_LPTIMER_CHANNEL3_IRQ_PRIORITY
    }
};

extern void LPTIMER_CHANNEL0_IRQHandler (void);
void LPTIMER_CHANNEL0_IRQHandler (void) {
    LPTIMER_Irq_Handler (&LPTIMER0_Resources, 0);
}

extern void LPTIMER_CHANNEL1_IRQHandler (void);
void LPTIMER_CHANNEL1_IRQHandler (void) {
    LPTIMER_Irq_Handler (&LPTIMER0_Resources, 1);
}

extern void LPTIMER_CHANNEL2_IRQHandler (void);
void LPTIMER_CHANNEL2_IRQHandler (void) {
    LPTIMER_Irq_Handler (&LPTIMER0_Resources, 2);
}

extern void LPTIMER_CHANNEL3_IRQHandler (void);
void LPTIMER_CHANNEL3_IRQHandler (void) {
    LPTIMER_Irq_Handler (&LPTIMER0_Resources, 3);
}

static int32_t ARM_LPTIMER0_Initialize (uint8_t channel, ARM_LPTIMER_SignalEvent_t cb_unit_event) {

    return ARM_LPTIMER_Initialize (&LPTIMER0_Resources, channel, cb_unit_event);
}

static int32_t ARM_LPTIMER0_PowerControl (uint8_t channel, ARM_POWER_STATE state) {

    return ARM_LPTIMER_PowerControl (&LPTIMER0_Resources, channel, state);
}

static int32_t ARM_LPTIMER0_Control (uint8_t channel, uint32_t control_code, void *arg) {

    return ARM_LPTIMER_Control (&LPTIMER0_Resources, channel, control_code, arg);
}

static int32_t ARM_LPTIMER0_Start (uint8_t channel) {

    return ARM_LPTIMER_Start (&LPTIMER0_Resources, channel);
}

static int32_t ARM_LPTIMER0_Stop (uint8_t channel) {

    return ARM_LPTIMER_Stop (&LPTIMER0_Resources, channel);
}

static int32_t ARM_LPTIMER0_Uninitialize (uint8_t channel) {

    return ARM_LPTIMER_Uninitialize (&LPTIMER0_Resources, channel);
}

/*LPTIMER Resources Control Block */
ARM_DRIVER_LPTIMER  DRIVER_LPTIMER0 = {
    ARM_LPTIMER0_Initialize,
    ARM_LPTIMER0_PowerControl,
    ARM_LPTIMER0_Control,
    ARM_LPTIMER0_Start,
    ARM_LPTIMER0_Stop,
    ARM_LPTIMER0_Uninitialize
};
