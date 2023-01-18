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
 * @file     UTIMER_ll_drv.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for UTIMER.
 * @bug      None.
 * @Note     None
 ******************************************************************************/
#include "UTIMER_ll_drv.h"

void QEC_ll_IRQHandler_Compare_A (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_COMPARE_A;
    info_ptr[channel].CB_function_ptr(ARM_QEC_COMPARE_A_EVENT);
}

void QEC_ll_IRQHandler_Compare_B (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_COMPARE_B;
    info_ptr[channel].CB_function_ptr(ARM_QEC_COMPARE_B_EVENT);
}

void UTIMER_ll_IRQHandler_Compare_A (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_COMPARE_A;
    info_ptr[channel].CB_function_ptr(ARM_UTIMER_CAPTURE_A_EVENT);
}

void UTIMER_ll_IRQHandler_Compare_B (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_COMPARE_B;
    info_ptr[channel].CB_function_ptr(ARM_UTIMER_CAPTURE_B_EVENT);
}

void UTIMER_ll_IRQHandler_Compare_C (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_COMPARE_C;
    info_ptr[channel].CB_function_ptr(ARM_UTIMER_CAPTURE_C_EVENT);
}

void UTIMER_ll_IRQHandler_Compare_D (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_COMPARE_D;
    info_ptr[channel].CB_function_ptr(ARM_UTIMER_CAPTURE_D_EVENT);
}

void UTIMER_ll_IRQHandler_Compare_E (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_COMPARE_E;
    info_ptr[channel].CB_function_ptr(ARM_UTIMER_CAPTURE_E_EVENT);
}

void UTIMER_ll_IRQHandler_Compare_F (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_COMPARE_F;
    info_ptr[channel].CB_function_ptr(ARM_UTIMER_CAPTURE_F_EVENT);
}

void UTIMER_ll_IRQHandler_Compare_OverFlow (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_OVER_FLOW;
    info_ptr[channel].CB_function_ptr(ARM_UTIMER_OVER_FLOW_EVENT);
}

void UTIMER_ll_IRQHandler_Compare_UnderFlow (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt |= CHAN_INTERRUPT_UNDER_FLOW;
    info_ptr[channel].CB_function_ptr(ARM_UTIMER_UNDER_FLOW_EVENT);
}

static void UTIMER_ll_NVIC_Disable (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    if (info_ptr[channel].utimer_mode)
    {
        switch (info_ptr[channel].channel_direction_backup)
        {
            case ARM_UTIMER_COUNT_DIRECTION_UP:
            {
                NVIC_ClearPendingIRQ (UTIMER_OVERFLOW_IRQ(channel));
                NVIC_DisableIRQ (UTIMER_OVERFLOW_IRQ(channel));
                break;
            }

            case ARM_UTIMER_COUNT_DIRECTION_DOWN:
            {
                NVIC_ClearPendingIRQ (UTIMER_UNDERFLOW_IRQ(channel));
                NVIC_DisableIRQ (UTIMER_UNDERFLOW_IRQ(channel));
                break;
            }

            case ARM_UTIMER_COUNT_DIRECTION_TRIANGLE:
            {
                NVIC_ClearPendingIRQ (UTIMER_OVERFLOW_IRQ(channel));
                NVIC_ClearPendingIRQ (UTIMER_UNDERFLOW_IRQ(channel));
                NVIC_DisableIRQ (UTIMER_OVERFLOW_IRQ(channel));
                NVIC_DisableIRQ (UTIMER_UNDERFLOW_IRQ(channel));
                break;
            }
        }

        switch (info_ptr[channel].channel_mode_backup)
        {
            case ARM_UTIMER_MODE_CAPTURING:
            {
                if (info_ptr[channel].driver_A) {
                    NVIC_ClearPendingIRQ (UTIMER_CAPTURE_A_IRQ(channel));
                    NVIC_DisableIRQ (UTIMER_CAPTURE_A_IRQ(channel));
                }

                if (info_ptr[channel].driver_B) {
                    NVIC_ClearPendingIRQ (UTIMER_CAPTURE_B_IRQ(channel));
                    NVIC_DisableIRQ (UTIMER_CAPTURE_B_IRQ(channel));
                }
                break;
            }
            case ARM_UTIMER_MODE_COMPARING:
            {
                if (info_ptr[channel].driver_A) {
                    NVIC_ClearPendingIRQ (UTIMER_CAPTURE_A_IRQ(channel));
                    NVIC_ClearPendingIRQ (UTIMER_CAPTURE_C_IRQ(channel));
                    NVIC_ClearPendingIRQ (UTIMER_CAPTURE_D_IRQ(channel));
                    NVIC_DisableIRQ (UTIMER_CAPTURE_A_IRQ(channel));
                    NVIC_DisableIRQ (UTIMER_CAPTURE_C_IRQ(channel));
                    NVIC_DisableIRQ (UTIMER_CAPTURE_D_IRQ(channel));
                 }

               if (info_ptr[channel].driver_B) {
                    NVIC_ClearPendingIRQ (UTIMER_CAPTURE_B_IRQ(channel));
                    NVIC_ClearPendingIRQ (UTIMER_CAPTURE_E_IRQ(channel));
                    NVIC_ClearPendingIRQ (UTIMER_CAPTURE_F_IRQ(channel));
                    NVIC_DisableIRQ (UTIMER_CAPTURE_B_IRQ(channel));
                    NVIC_DisableIRQ (UTIMER_CAPTURE_E_IRQ(channel));
                    NVIC_DisableIRQ (UTIMER_CAPTURE_F_IRQ(channel));
                }
                break;
            }

            case ARM_UTIMER_MODE_BASIC:
            case ARM_UTIMER_MODE_BUFFERING:
            case ARM_UTIMER_MODE_TRIGGERING:
            case ARM_UTIMER_MODE_DEAD_TIME:
            break;
        }
    }

    else
    {
        switch(info_ptr[channel].channel_mode_backup)
        {
            case ARM_UTIMER_MODE_CAPTURING:
            case ARM_UTIMER_MODE_COMPARING:
            {
                if (info_ptr[channel].driver_A) {
                    NVIC_ClearPendingIRQ (QEC_COMPARE_A_IRQ(channel));
                    NVIC_DisableIRQ (QEC_COMPARE_A_IRQ(channel));
                }

                if (info_ptr[channel].driver_B) {
                    NVIC_ClearPendingIRQ (QEC_COMPARE_B_IRQ(channel));
                    NVIC_DisableIRQ (QEC_COMPARE_B_IRQ(channel));
                }
                break;
            }

            case ARM_UTIMER_MODE_BASIC:
            case ARM_UTIMER_MODE_BUFFERING:
            case ARM_UTIMER_MODE_TRIGGERING:
            case ARM_UTIMER_MODE_DEAD_TIME:
            break;
        }
    }
}

static int32_t UTIMER_ll_NVIC_Enable (uint8_t channel, uint32_t priorty, uint8_t irq_type)
{
    IRQn_Type IRQnx =0;

    switch (irq_type) {
        case ARM_UTIMER_IRQ_TYPE_CAPTURE_A:
        {
            IRQnx = UTIMER_CAPTURE_A_IRQ(channel);
            break;
        }
        case ARM_UTIMER_IRQ_TYPE_CAPTURE_B:
        {
            IRQnx = UTIMER_CAPTURE_B_IRQ(channel);
            break;
        }
        case ARM_UTIMER_IRQ_TYPE_CAPTURE_C:
        {
            IRQnx = UTIMER_CAPTURE_C_IRQ(channel);
            break;
        }
        case ARM_UTIMER_IRQ_TYPE_CAPTURE_D:
        {
            IRQnx = UTIMER_CAPTURE_D_IRQ(channel);
            break;
        }
        case ARM_UTIMER_IRQ_TYPE_CAPTURE_E:
        {
            IRQnx = UTIMER_CAPTURE_E_IRQ(channel);
            break;
        }
        case ARM_UTIMER_IRQ_TYPE_CAPTURE_F:
        {
            IRQnx = UTIMER_CAPTURE_F_IRQ(channel);
            break;
        }
        case ARM_UTIMER_IRQ_TYPE_UNDER_FLOW:
        {
            IRQnx = UTIMER_UNDERFLOW_IRQ(channel);
            break;
        }
        case ARM_UTIMER_IRQ_TYPE_OVER_FLOW:
        {
            IRQnx = UTIMER_OVERFLOW_IRQ(channel);
            break;
        }
        case ARM_QEC_IRQ_TYPE_COMPARE_A:
        {
            IRQnx = QEC_COMPARE_A_IRQ(channel);
            break;
        }
        case ARM_QEC_IRQ_TYPE_COMPARE_B:
        {
            IRQnx = QEC_COMPARE_B_IRQ(channel);
            break;
        }
    }

    NVIC_SetPriority(IRQnx, priorty);

    if (NVIC_GetPriority(IRQnx) != priorty)
    {
        return ARM_DRIVER_ERROR;
    }
    NVIC_EnableIRQ(IRQnx);

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_PowerOff (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    reg_ptr->TIMER_Channel_RegInfo[channel].cntr_start_src = 0;
    reg_ptr->TIMER_Channel_RegInfo[channel].cntr_stop_src = 0;
    reg_ptr->TIMER_Channel_RegInfo[channel].cntr_clear_src = 0;
    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt_mask = ARM_UTIMER_INTRRUPT_MASK;
    reg_ptr->TIMER_Glb_RegInfo.glb_driver_oen |= (3U<<((uint32_t)(channel*2U)));

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_PowerFull (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    reg_ptr->TIMER_Channel_RegInfo[channel].cntr_start_src = CNTR_SRC_PGM_EN;
    reg_ptr->TIMER_Channel_RegInfo[channel].cntr_stop_src = CNTR_SRC_PGM_EN;
    reg_ptr->TIMER_Channel_RegInfo[channel].cntr_clear_src = CNTR_SRC_PGM_EN;
    reg_ptr->TIMER_Channel_RegInfo[channel].chan_interrupt_mask = ARM_UTIMER_INTRRUPT_UNMASK;
    reg_ptr->TIMER_Glb_RegInfo.glb_driver_oen &= ~(3U<<((uint32_t)(channel*2U)));

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_Mode_Config (UTIMER_resources_t *UTIMER, uint8_t channel, UTIMER_MODE_CONFIG *arg)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    info_ptr[channel].channel_direction_backup = arg->direction;
    info_ptr[channel].channel_mode_backup = arg->mode;

    switch (arg->direction)
    {
        case ARM_UTIMER_COUNT_DIRECTION_UP:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ptr = arg->count_array[1];
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr = arg->count_array[0];
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ctrl = CNTR_CTRL_EN;

            if (info_ptr[channel].fixed_buffer) {
                reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ctrl |= CNTR_CTRL_TYPE0;
            }

            if (info_ptr[channel].utimer_mode) {
                if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].over_flow_irq_priority, ARM_UTIMER_IRQ_TYPE_OVER_FLOW)) {
                    return ARM_DRIVER_ERROR;
                }
            }
            break;
        }

        case ARM_UTIMER_COUNT_DIRECTION_DOWN:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ptr = arg->count_array[1];
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr = arg->count_array[0];
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ctrl = (CNTR_CTRL_EN | CNTR_CTRL_DIR);

            if (info_ptr[channel].fixed_buffer) {
                reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ctrl |= CNTR_CTRL_TYPE0;
            }

            if (info_ptr[channel].utimer_mode) {
                if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].under_flow_irq_priority, ARM_UTIMER_IRQ_TYPE_UNDER_FLOW)) {
                    return ARM_DRIVER_ERROR;
                }
            }
            break;
        }
        case ARM_UTIMER_COUNT_DIRECTION_TRIANGLE:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ptr = arg->count_array[1];
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr = arg->count_array[0];
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ctrl = (CNTR_CTRL_EN | (info_ptr[channel].buf_trough_n_crest<<2));

            if (info_ptr[channel].utimer_mode) {
                if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].over_flow_irq_priority, ARM_UTIMER_IRQ_TYPE_UNDER_FLOW)) {
                    return ARM_DRIVER_ERROR;
                }
                if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].under_flow_irq_priority, ARM_UTIMER_IRQ_TYPE_OVER_FLOW)) {
                    return ARM_DRIVER_ERROR;
                }
            }
            break;
        }

        default:
        {
            return ARM_DRIVER_ERROR;
        }
    }

    switch (arg->mode) {
        case ARM_UTIMER_MODE_BASIC:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl = (BUF_OP_CTRL_CAPTURE_BUF_DISABLE|BUF_OP_CTRL_COMPARE_BUF_DISABLE|
                                                                   BUF_OP_CTRL_DUTY_CYCLE_OP_DISABLE|BUF_OP_CTRL_CNTR_BUF_DISABLE);
            break;
        }
        case ARM_UTIMER_MODE_BUFFERING:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ptr_buf1 = arg->count_array[2];
            reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl = (BUF_OP_CTRL_CAPTURE_BUF_DISABLE|
                                                                   BUF_OP_CTRL_COMPARE_BUF_DISABLE|
                                                                   BUF_OP_CTRL_DUTY_CYCLE_OP_DISABLE);

            if (info_ptr[channel].buffering_type) {//SET: Double Buffering
                reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ptr_buf2 = arg->count_array[3];
                reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl = (BUF_OP_CTRL_CNTR_BUF_OP_BIT1);
            } else {//CLEAR: Single Buffering
                reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl = (BUF_OP_CTRL_CNTR_BUF_OP_BIT0);
            }
            break;
        }
        case ARM_UTIMER_MODE_TRIGGERING:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl = 0xF;
            break;
        }
        case ARM_UTIMER_MODE_CAPTURING:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl   =  (BUF_OP_CTRL_CNTR_BUF_DISABLE|
                                                                      BUF_OP_CTRL_COMPARE_BUF_DISABLE|
                                                                      BUF_OP_CTRL_DUTY_CYCLE_OP_DISABLE|
                                                                     (info_ptr[channel].buffering_type_A<<16U)|
                                                                     (info_ptr[channel].buffering_type_B<<18U));

            if (info_ptr[channel].driver_A) {
                if (info_ptr[channel].utimer_mode) { //utimer
                    if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_A_irq_priority, ARM_UTIMER_IRQ_TYPE_CAPTURE_A)) {
                        return ARM_DRIVER_ERROR;
                    }
                } else { //QEC
                    if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_A_irq_priority, ARM_QEC_IRQ_TYPE_COMPARE_A)) {
                        return ARM_DRIVER_ERROR;
                    }
                }
            }

            if (info_ptr[channel].driver_B) {
                if (info_ptr[channel].utimer_mode) { //utimer
                    if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_B_irq_priority, ARM_UTIMER_IRQ_TYPE_CAPTURE_B)) {
                        return ARM_DRIVER_ERROR;
                    }
                } else { //QEC
                    if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_B_irq_priority, ARM_QEC_IRQ_TYPE_COMPARE_B)) {
                        return ARM_DRIVER_ERROR;
                    }
                }
            }
            break;
        }

        case ARM_UTIMER_MODE_COMPARING:
        {
            if (info_ptr[channel].driver_A) {
                reg_ptr->TIMER_Channel_RegInfo[channel].compare_a = arg->count_array[2];
                reg_ptr->TIMER_Channel_RegInfo[channel].compare_a_buf1 = arg->count_array[3];
                reg_ptr->TIMER_Channel_RegInfo[channel].compare_a_buf2 = arg->count_array[4];

                reg_ptr->TIMER_Channel_RegInfo[channel].compare_ctrl_a = (COMPARE_CTRL_DRV_COMPARE_EN|
                                                                          COMPARE_CTRL_DRV_DRIVER_EN|
                                                                         (info_ptr[channel].cmp_drive_start_state<<4U)|
                                                                         (info_ptr[channel].cmp_drive_stop_state<<6U)|
                                                                         (info_ptr[channel].drive_op_at_cycle_end<<2U)|
                                                                         (info_ptr[channel].drive_op_at_match_count));

                reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl = (BUF_OP_CTRL_CNTR_BUF_DISABLE|
                                                                       BUF_OP_CTRL_CAPTURE_BUF_DISABLE|
                                                                       BUF_OP_CTRL_DUTY_CYCLE_OP_DISABLE|
                                                                      (info_ptr[channel].buffering_event_at_crest<<24U)|
                                                                      (info_ptr[channel].buffering_event_at_trough<<25U)|
                                                                      (info_ptr[channel].buffering_type<<26U));

                if (!(info_ptr[channel].buffer_operation)) {
                    reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl |= BUF_OP_CTRL_COMPARE_BUF_DISABLE;
                }

                if (info_ptr[channel].fixed_buffer) {
                    reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl |= BUF_OP_CTRL_FORCE_COMPARE_BUF_OP;
                }

                if (info_ptr[channel].utimer_mode) { //utimer
                    if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_A_irq_priority, ARM_UTIMER_IRQ_TYPE_CAPTURE_A)) {
                        return ARM_DRIVER_ERROR;
                    }
                    if (info_ptr[channel].buffer_operation) {
                        if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_C_irq_priority, ARM_UTIMER_IRQ_TYPE_CAPTURE_C)) {
                        return ARM_DRIVER_ERROR;
                        }
                         if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_D_irq_priority, ARM_UTIMER_IRQ_TYPE_CAPTURE_D)) {
                         return ARM_DRIVER_ERROR;
                        }
                    }
                } else { //QEC
                    if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_A_irq_priority, ARM_QEC_IRQ_TYPE_COMPARE_A)) {
                    return ARM_DRIVER_ERROR;
                   }
                }
            }

            if (info_ptr[channel].driver_B) {
                reg_ptr->TIMER_Channel_RegInfo[channel].compare_b = arg->count_array[2];
                reg_ptr->TIMER_Channel_RegInfo[channel].compare_b_buf1 = arg->count_array[3];
                reg_ptr->TIMER_Channel_RegInfo[channel].compare_b_buf2 = arg->count_array[4];

                reg_ptr->TIMER_Channel_RegInfo[channel].compare_ctrl_b = (COMPARE_CTRL_DRV_COMPARE_EN|
                                                                          COMPARE_CTRL_DRV_DRIVER_EN|
                                                                         (info_ptr[channel].cmp_drive_start_state<<4U)|
                                                                         (info_ptr[channel].cmp_drive_stop_state<<6U)|
                                                                         (info_ptr[channel].drive_op_at_cycle_end<<2U)|
                                                                         (info_ptr[channel].drive_op_at_match_count));

                reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl = (BUF_OP_CTRL_CNTR_BUF_DISABLE|
                                                                       BUF_OP_CTRL_CAPTURE_BUF_DISABLE|
                                                                       BUF_OP_CTRL_DUTY_CYCLE_OP_DISABLE|
                                                                      (info_ptr[channel].buffering_event_at_crest<<28)|
                                                                      (info_ptr[channel].buffering_event_at_trough<<29)|
                                                                      (info_ptr[channel].buffering_type<<30));

                if (!(info_ptr[channel].buffer_operation)) {
                    reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl |= BUF_OP_CTRL_COMPARE_BUF_DISABLE;
                }

                if (info_ptr[channel].fixed_buffer) {
                    reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl |= BUF_OP_CTRL_FORCE_COMPARE_BUF_OP;
                }

                if (info_ptr[channel].utimer_mode) { //utimer
                    if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_B_irq_priority, ARM_UTIMER_IRQ_TYPE_CAPTURE_B)) {
                        return ARM_DRIVER_ERROR;
                    }
                    if (info_ptr[channel].buffer_operation) {
                        if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_E_irq_priority, ARM_UTIMER_IRQ_TYPE_CAPTURE_E)) {
                        return ARM_DRIVER_ERROR;
                        }
                         if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_F_irq_priority, ARM_UTIMER_IRQ_TYPE_CAPTURE_F)) {
                         return ARM_DRIVER_ERROR;
                        }
                    }
                } else { //QEC
                    if (UTIMER_ll_NVIC_Enable(channel, info_ptr[channel].capture_B_irq_priority, ARM_QEC_IRQ_TYPE_COMPARE_B)) {
                    return ARM_DRIVER_ERROR;
                   }
                }
            }
            break;
        }

        case ARM_UTIMER_MODE_DEAD_TIME:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].buf_op_ctrl = (BUF_OP_CTRL_CNTR_BUF_DISABLE|
                                                                   BUF_OP_CTRL_CAPTURE_BUF_DISABLE|
                                                                   BUF_OP_CTRL_DUTY_CYCLE_OP_DISABLE|
                                                                   BUF_OP_CTRL_COMPARE_BUF_DISABLE);

            reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_ctrl = DEAD_TIME_CTRL_DT_EN;
            if (info_ptr[channel].buffer_operation) {
                reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_ctrl |= DEAD_TIME_CTRL_DT_BUF_EN;
            }
            reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_up = arg->count_array[2];
            reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_down = arg->count_array[3];
            reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_up_buf = arg->count_array[4];
            reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_down_buf = arg->count_array[5];

            if (info_ptr[channel].driver_A) {

                reg_ptr->TIMER_Channel_RegInfo[channel].compare_a = arg->count_array[6];
                reg_ptr->TIMER_Channel_RegInfo[channel].compare_ctrl_a = (COMPARE_CTRL_DRV_COMPARE_EN|
                                                                         COMPARE_CTRL_DRV_DRIVER_EN|
                                                                         (info_ptr[channel].cmp_drive_start_state<<4U)|
                                                                         (info_ptr[channel].cmp_drive_stop_state<<6U)|
                                                                         (info_ptr[channel].drive_op_at_cycle_end<<2U)|
                                                                         (info_ptr[channel].drive_op_at_match_count));
            }

            if (info_ptr[channel].driver_B) {

                reg_ptr->TIMER_Channel_RegInfo[channel].compare_b = arg->count_array[7];
                reg_ptr->TIMER_Channel_RegInfo[channel].compare_ctrl_b =  (COMPARE_CTRL_DRV_COMPARE_EN|
                                                                          COMPARE_CTRL_DRV_DRIVER_EN|
                                                                          ((uint32_t)(!(info_ptr[channel].cmp_drive_start_state)) << 4U)|
                                                                          (info_ptr[channel].cmp_drive_stop_state<<6U)|
                                                                          (info_ptr[channel].drive_op_at_cycle_end<<2U)|
                                                                          (info_ptr[channel].drive_op_at_match_count));
            }
            break;
        }
        default:
        {
            return ARM_DRIVER_ERROR;
        }
    }

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_Set_Count (UTIMER_resources_t *UTIMER, uint8_t channel, UTIMER_SET_OPERATION_CONFIG *arg)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    switch (arg->operation_type)
    {
        case ARM_UTIMER_SET_COUNT_FOR_NORMAL_OPERATION:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ptr   =  *(arg->count);
            break;
        }
        case ARM_UTIMER_SET_COUNT_FOR_SINGLE_BUFFER_OPERATION:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ptr_buf1 = *(arg->count);
            break;
        }
        case ARM_UTIMER_SET_COUNT_FOR_DOUBLE_BUFFER_OPERATION:
        {
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ptr_buf2 = *(arg->count);
            break;
        }
        case ARM_UTIMER_SET_COUNT_FOR_DT_UP:
        {
            if (reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_up_buf == 0) {

            } else {
                while(reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_up_buf != reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_up);
            }
            reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_up_buf = *(arg->count);
            break;
        }
        case ARM_UTIMER_SET_COUNT_FOR_DT_DOWN:
        {
            if (reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_down_buf == 0) {

            } else {
                while(reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_down_buf != reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_down);
            }
            reg_ptr->TIMER_Channel_RegInfo[channel].dead_time_down_buf = *(arg->count);
            break;
        }
        default:
        {
            return ARM_DRIVER_ERROR;
        }
    }

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_Get_Count (UTIMER_resources_t *UTIMER, uint8_t channel, UTIMER_GET_OPERATION_CONFIG *arg)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    switch (arg->operation_type)
    {
        case ARM_UTIMER_GET_COUNT_OF_CURRENT_RUNNING_TIMER:
        {
            *(arg->count) = reg_ptr->TIMER_Channel_RegInfo[channel].cntr;
            break;
        }
        case ARM_UTIMER_GET_COUNT_OF_DRIVE_A_CAPTURE_VALUE:
        {
            *(arg->count) =  reg_ptr->TIMER_Channel_RegInfo[channel].capture_a;
            break;
        }
        case ARM_UTIMER_GET_COUNT_OF_DRIVE_B_CAPTURE_VALUE:
        {
            *(arg->count) =  reg_ptr->TIMER_Channel_RegInfo[channel].capture_b;
            break;
        }
        case ARM_UTIMER_GET_COUNT_OF_DRIVE_A_CAPTURE1_VALUE:
        {
            *(arg->count) =  reg_ptr->TIMER_Channel_RegInfo[channel].capture_a_buf1;
            break;
        }
        case ARM_UTIMER_GET_COUNT_OF_DRIVE_B_CAPTURE1_VALUE:
        {
            *(arg->count) = reg_ptr->TIMER_Channel_RegInfo[channel].capture_b_buf1;
            break;
        }
        case ARM_UTIMER_GET_COUNT_OF_DRIVE_A_CAPTURE2_VALUE:
        {
            *(arg->count) = reg_ptr->TIMER_Channel_RegInfo[channel].capture_a_buf2;
            break;
        }
        case ARM_UTIMER_GET_COUNT_OF_DRIVE_B_CAPTURE2_VALUE:
        {
            *(arg->count) = reg_ptr->TIMER_Channel_RegInfo[channel].capture_b_buf2;
            break;
        }
        default:
        {
            return ARM_DRIVER_ERROR;
        }
    }

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_Get_Direction (UTIMER_resources_t *UTIMER, uint8_t channel, uint32_t *arg)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    if (reg_ptr->TIMER_Channel_RegInfo[channel].cntr_ctrl & CNTR_CTRL_DIR) {
        *arg = ARM_UTIMER_DIRECTION_UP;
    } else {
        *arg = ARM_UTIMER_DIRECTION_DOWN;
    }

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_Configure_Trigger (UTIMER_resources_t *UTIMER, uint8_t channel, uint8_t triggerFor, uint32_t triggerType)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    switch (triggerFor)
    {
        case ARM_UTIMER_TRIGGER_FOR_START:
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_start_src &= ~(CNTR_SRC_PGM_EN);   //pgm_en =0;
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_start_src |= triggerType;
        break;
        case ARM_UTIMER_TRIGGER_FOR_STOP:
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_stop_src &= ~(CNTR_SRC_PGM_EN);    //pgm_en =0;
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_stop_src |= triggerType;
        break;
        case ARM_UTIMER_TRIGGER_FOR_CLEAR:
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_clear_src &= ~(CNTR_SRC_PGM_EN);   //pgm_en =0;
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_clear_src |= triggerType;
        break;
        case ARM_UTIMER_TRIGGER_FOR_UPCOUNT:
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_up_src |= triggerType;
        break;
        case ARM_UTIMER_TRIGGER_FOR_DOWNCOUNT:
            reg_ptr->TIMER_Channel_RegInfo[channel].cntr_down_src |= triggerType;
        break;
        case ARM_UTIMER_TRIGGER_FOR_CAPTURE_A:
            reg_ptr->TIMER_Channel_RegInfo[channel].trig_capture_src_a |= triggerType;
        break;
        case ARM_UTIMER_TRIGGER_FOR_CAPTURE_B:
            reg_ptr->TIMER_Channel_RegInfo[channel].trig_capture_src_b |= triggerType;
        break;
    }

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_Start (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    reg_ptr->TIMER_Glb_RegInfo.glb_cntr_start |= (1<<channel);

    if (reg_ptr->TIMER_Glb_RegInfo.glb_cntr_running & (1<<channel)) {
        return ARM_DRIVER_OK;
    }

    return ARM_DRIVER_ERROR;
}

int32_t UTIMER_ll_Stop (UTIMER_resources_t *UTIMER, uint8_t channel, bool count_clear_option)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    reg_ptr->TIMER_Glb_RegInfo.glb_cntr_stop |= (1<<channel);

    if (count_clear_option) {
        reg_ptr->TIMER_Glb_RegInfo.glb_cntr_clear |= (1<<channel);
    }

    if (reg_ptr->TIMER_Glb_RegInfo.glb_cntr_running & (1<<channel)) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

int32_t UTIMER_ll_Uninitialize (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    TIMER_RegInfo *reg_ptr = (TIMER_RegInfo*)(UTIMER->reg_base);

    UTIMER_ll_NVIC_Disable (UTIMER, channel);

    reg_ptr->TIMER_Glb_RegInfo.glb_cntr_stop &= ~(1<<channel);
    reg_ptr->TIMER_Glb_RegInfo.glb_cntr_start &= ~(1<<channel);
    reg_ptr->TIMER_Glb_RegInfo.glb_cntr_clear &= ~(1<<channel);

    return ARM_DRIVER_OK;
}
