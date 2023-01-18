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
 * @file     Driver_UTIMER.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for UTIMER.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "UTIMER_ll_drv.h"

#if !(RTE_UTIMER)
#error "UTIMER is not enabled in RTE_Device.h"
#endif

#if (defined(RTE_Drivers_UTIMER) && !RTE_UTIMER)
#error "UTIMER not configured in RTE_Device.h!"
#endif

UTIMER_TIMER_INFO Utimer_info[ARM_UTIMER_MAX_CHANNEL] = {
    {//CHANNEL0
        .fixed_buffer              = RTE_UTIMER_CHANNEL0_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL0_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL0_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL0_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL0_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL0_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL0_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL0_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL0_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL0_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL0_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL0_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL0_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL0_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL0_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL0_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL0_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL0_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL0_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL0_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL0_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL0_UNDER_FLOW_IRQ_PRIORITY,
    },
    {//CHANNEL1
        .fixed_buffer              = RTE_UTIMER_CHANNEL1_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL1_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL1_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL1_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL1_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL1_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL1_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL1_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL1_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL1_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL1_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL1_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL1_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL1_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL1_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL1_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL1_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL1_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL1_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL1_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL1_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL1_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL2
        .fixed_buffer              = RTE_UTIMER_CHANNEL2_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL2_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL2_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL2_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL2_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL2_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL2_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL2_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL2_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL2_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL2_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL2_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL2_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL2_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL2_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL2_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL2_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL2_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL2_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL2_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL2_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL2_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL3
        .fixed_buffer              = RTE_UTIMER_CHANNEL3_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL3_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL3_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL3_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL3_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL3_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL3_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL3_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL3_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL3_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL3_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL3_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL3_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL3_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL3_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL3_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL3_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL3_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL3_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL3_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL3_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL3_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL4
        .fixed_buffer              = RTE_UTIMER_CHANNEL4_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL4_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL4_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL4_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL4_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL4_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL4_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL4_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL4_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL4_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL4_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL4_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL4_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL4_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL4_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL4_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL4_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL4_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL4_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL4_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL4_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL4_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL5
        .fixed_buffer              = RTE_UTIMER_CHANNEL5_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL5_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL5_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL5_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL5_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL5_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL5_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL5_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL5_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL5_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL5_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL5_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL5_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL5_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL5_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL5_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL5_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL5_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL5_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL5_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL5_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL5_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL6
        .fixed_buffer              = RTE_UTIMER_CHANNEL6_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL6_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL6_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL6_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL6_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL6_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL6_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL6_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL6_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL6_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL6_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL6_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL6_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL6_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL6_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL6_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL6_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL6_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL6_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL6_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL6_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL6_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL7
        .fixed_buffer              = RTE_UTIMER_CHANNEL7_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL7_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL7_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL7_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL7_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL7_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL7_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL7_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL7_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL7_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL7_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL7_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL7_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL7_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL7_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL7_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL7_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL7_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL7_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL7_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL7_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL7_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL8
        .fixed_buffer              = RTE_UTIMER_CHANNEL8_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL8_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL8_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL8_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL8_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL8_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL8_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL8_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL8_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL8_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL8_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL8_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL8_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL8_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL8_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL8_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL8_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL8_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL8_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL8_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL8_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL8_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL9
        .fixed_buffer              = RTE_UTIMER_CHANNEL9_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL9_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL9_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL9_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL9_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL9_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL9_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL9_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL9_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL9_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL9_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL9_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL9_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL9_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL9_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL9_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL9_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL9_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL9_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL9_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL9_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL9_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL10
        .fixed_buffer              = RTE_UTIMER_CHANNEL10_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL10_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL10_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL10_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL10_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL10_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL10_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL10_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL10_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL10_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL10_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL10_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL10_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL10_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL10_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL10_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL10_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL10_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL10_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL10_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL10_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL10_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL11
        .fixed_buffer              = RTE_UTIMER_CHANNEL11_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL11_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL11_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL11_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL11_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL11_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL11_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL11_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL11_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL11_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL11_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL11_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL11_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL11_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL11_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL11_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL11_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL11_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL11_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL11_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL11_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL11_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL12
        .fixed_buffer              = RTE_UTIMER_CHANNEL12_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL12_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL12_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL12_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL12_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL12_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL12_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL12_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL12_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL12_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL12_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL12_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL12_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL12_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL12_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL12_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL12_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL12_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL12_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL12_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL12_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL12_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL13
        .fixed_buffer              = RTE_UTIMER_CHANNEL13_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL13_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL13_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL13_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL13_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL13_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL13_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL13_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL13_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL13_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL13_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL13_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL13_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL13_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL13_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL13_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL13_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL13_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL13_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL13_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL13_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL13_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL14
        .fixed_buffer              = RTE_UTIMER_CHANNEL14_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL14_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL14_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL14_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL14_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL14_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL14_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL14_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL14_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL14_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL14_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL14_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL14_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL14_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL14_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL14_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL14_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL14_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL14_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL14_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL14_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL14_UNDER_FLOW_IRQ_PRIORITY
    },
    {//CHANNEL15
        .fixed_buffer              = RTE_UTIMER_CHANNEL15_FIXED_BUFFER,
        .buf_trough_n_crest        = RTE_UTIMER_CHANNEL15_BUF_TROUGH_N_CREST,
        .driver_A                  = RTE_UTIMER_CHANNEL15_DRIVER_A,
        .driver_B                  = RTE_UTIMER_CHANNEL15_DRIVER_B,
        .cmp_drive_start_state     = RTE_UTIMER_CHANNEL15_CMP_START_STATE,
        .cmp_drive_stop_state      = RTE_UTIMER_CHANNEL15_CMP_STOP_STATE,
        .drive_op_at_match_count   = RTE_UTIMER_CHANNEL15_DRV_OP_AT_MATCH_COUNT,
        .drive_op_at_cycle_end     = RTE_UTIMER_CHANNEL15_DRV_OP_AT_CYCLE_END,
        .buffering_event_at_crest  = RTE_UTIMER_CHANNEL15_EVENT_AT_CREST,
        .buffering_event_at_trough = RTE_UTIMER_CHANNEL15_EVENT_AT_TROUGH,
        .buffering_type            = RTE_UTIMER_CHANNEL15_BUFFERING_TYPE,
        .buffer_operation          = RTE_UTIMER_CHANNEL15_BUFFER_OPERATION,
        .buffering_type_A          = RTE_UTIMER_CHANNEL15_BUFFERING_TYPE_A,
        .buffering_type_B          = RTE_UTIMER_CHANNEL15_BUFFERING_TYPE_B,
        .flag                      = UTIMER_CHANNEL_UNINITIALIZED,
        .capture_A_irq_priority    = RTE_UTIMER_CHANNEL15_CAPTURE_A_IRQ_PRIORITY,
        .capture_B_irq_priority    = RTE_UTIMER_CHANNEL15_CAPTURE_B_IRQ_PRIORITY,
        .capture_C_irq_priority    = RTE_UTIMER_CHANNEL15_CAPTURE_C_IRQ_PRIORITY,
        .capture_D_irq_priority    = RTE_UTIMER_CHANNEL15_CAPTURE_D_IRQ_PRIORITY,
        .capture_E_irq_priority    = RTE_UTIMER_CHANNEL15_CAPTURE_E_IRQ_PRIORITY,
        .capture_F_irq_priority    = RTE_UTIMER_CHANNEL15_CAPTURE_F_IRQ_PRIORITY,
        .over_flow_irq_priority    = RTE_UTIMER_CHANNEL15_OVER_FLOW_IRQ_PRIORITY,
        .under_flow_irq_priority   = RTE_UTIMER_CHANNEL15_UNDER_FLOW_IRQ_PRIORITY
    }
};

static int32_t ARM_UTIMER_Initialize (UTIMER_resources_t *UTIMER, uint8_t channel, ARM_UTIMER_SignalEvent_t cb_unit_event)
{
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    if ((UTIMER->info == NULL) || (cb_unit_event == NULL))  {   return ARM_DRIVER_ERROR_PARAMETER;  }
    if (channel > ARM_UTIMER_MAX_CHANNEL)                   {   return ARM_DRIVER_ERROR_PARAMETER;  }

    if (channel < ARM_UTIMER_CHANNEL12) {
        info_ptr[channel].utimer_mode = 1;
    } else {
        info_ptr[channel].utimer_mode = 0;
    }

    if(info_ptr[channel].flag == UTIMER_CHANNEL_INITIALIZED )           {   return ARM_DRIVER_OK;   }

    info_ptr[channel].CB_function_ptr = cb_unit_event;

    info_ptr[channel].flag = UTIMER_CHANNEL_INITIALIZED;

    return ARM_DRIVER_OK;
}

static int32_t ARM_UTIMER_PowerControl (UTIMER_resources_t *UTIMER, uint8_t channel, ARM_POWER_STATE state)
{
    int32_t ret;
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    if (UTIMER->info == NULL)                           {   return ARM_DRIVER_ERROR_PARAMETER;  }
    if (channel > ARM_UTIMER_MAX_CHANNEL)               {   return ARM_DRIVER_ERROR_PARAMETER;  }
    if (state   > ARM_POWER_FULL)                       {   return ARM_DRIVER_ERROR_PARAMETER;  }

    switch (state)
    {
        case ARM_POWER_OFF:
        {
            ret = UTIMER_ll_PowerOff (UTIMER, channel);
            if (ret == ARM_DRIVER_OK) {
                info_ptr[channel].flag = UTIMER_CHANNEL_POWERED_OFF;
            }
            break;
        }
        case ARM_POWER_FULL:
        {
            if (info_ptr[channel].flag == UTIMER_CHANNEL_POWERED_FULL)  {   return ARM_DRIVER_OK;   }
            if (info_ptr[channel].flag != UTIMER_CHANNEL_INITIALIZED)   {   return ARM_DRIVER_ERROR;   }

            ret = UTIMER_ll_PowerFull (UTIMER, channel);
            if (ret == ARM_DRIVER_OK) {
                info_ptr[channel].flag = UTIMER_CHANNEL_POWERED_FULL;
            }
            break;
        }
        case ARM_POWER_LOW:
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
    return ret;
}

static int32_t ARM_UTIMER_Control (UTIMER_resources_t *UTIMER, uint8_t channel, uint32_t control_code, void *arg)
{
    int32_t ret;
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    if ((UTIMER->info == NULL) || (arg == NULL))     {   return ARM_DRIVER_ERROR_PARAMETER;  }
    if (channel > ARM_UTIMER_MAX_CHANNEL)            {   return ARM_DRIVER_ERROR_PARAMETER;  }
    if (control_code > ARM_UTIMER_GET_DIRECTION)     {   return ARM_DRIVER_ERROR_PARAMETER;  }

    switch (control_code)
    {
        case ARM_UTIMER_MODE_CONFIG:
        {
            if(info_ptr[channel].flag != UTIMER_CHANNEL_POWERED_FULL)         {   return ARM_DRIVER_ERROR;    }

            ret = UTIMER_ll_Mode_Config (UTIMER, channel, (UTIMER_MODE_CONFIG*)arg);
            if (ret == ARM_DRIVER_OK) {
                info_ptr[channel].flag = UTIMER_CHANNEL_CONTROL;
            }
            break;
        }
        case ARM_UTIMER_SET_COUNT:
        {
            if(info_ptr[channel].flag < UTIMER_CHANNEL_CONTROL)                {   return ARM_DRIVER_ERROR;    }

            ret = UTIMER_ll_Set_Count (UTIMER, channel, (UTIMER_SET_OPERATION_CONFIG *) arg);
            break;
        }
        case ARM_UTIMER_GET_COUNT:
        {
            if(info_ptr[channel].flag < UTIMER_CHANNEL_CONTROL)                {   return ARM_DRIVER_ERROR;    }

            ret = UTIMER_ll_Get_Count (UTIMER, channel, (UTIMER_GET_OPERATION_CONFIG *) arg);
            break;
        }
        case ARM_UTIMER_GET_DIRECTION:
        {
            if(info_ptr[channel].flag < UTIMER_CHANNEL_CONTROL)                {   return ARM_DRIVER_ERROR;    }

            ret = UTIMER_ll_Get_Direction (UTIMER, channel, (uint32_t *) arg);
            break;
        }
        default:
        {
            return ARM_DRIVER_ERROR;
        }
    }

    return ret;
}

static int32_t ARM_UTIMER_Configure_Trigger (UTIMER_resources_t *UTIMER, uint8_t channel, uint8_t triggerFor, uint32_t triggerType)
{
    int32_t ret;
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    if (UTIMER->info == NULL)                           {  return ARM_DRIVER_ERROR_PARAMETER;  }
    if (channel > ARM_UTIMER_MAX_CHANNEL)               {  return ARM_DRIVER_ERROR_PARAMETER;  }
    if (triggerFor > ARM_UTIMER_TRIGGER_FOR_CAPTURE_B)  {  return ARM_DRIVER_ERROR_PARAMETER;  }
    if ((triggerType < ARM_UTIMER_TRIG0_RISING) && (triggerType > ARM_UTIMER_GLB7_EVENT))  {  return ARM_DRIVER_ERROR_PARAMETER;  }

    if (info_ptr[channel].flag < UTIMER_CHANNEL_CONTROL)   {   return ARM_DRIVER_ERROR;    }

    ret = UTIMER_ll_Configure_Trigger (UTIMER, channel, triggerFor, triggerType);

    if (ret == ARM_DRIVER_OK) {
        info_ptr[channel].flag = UTIMER_CHANNEL_TRIGGERED;
    }
    return ret;
}

static int32_t ARM_UTIMER_Start (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    int32_t ret;
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    if (UTIMER->info == NULL)                        {   return ARM_DRIVER_ERROR_PARAMETER;  }
    if (channel > ARM_UTIMER_MAX_CHANNEL)            {   return ARM_DRIVER_ERROR_PARAMETER;  }

    if(info_ptr[channel].flag < UTIMER_CHANNEL_CONTROL)    {   return ARM_DRIVER_ERROR;    }

    ret = UTIMER_ll_Start (UTIMER, channel);

    if (ret == ARM_DRIVER_OK) {
        info_ptr[channel].flag = UTIMER_CHANNEL_STARTED;
    }
    return ret;
}

static int32_t ARM_UTIMER_Stop (UTIMER_resources_t *UTIMER, uint8_t channel, bool count_clear_option)
{
    int32_t ret;
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    if (channel > ARM_UTIMER_MAX_CHANNEL)     {   return ARM_DRIVER_ERROR_PARAMETER;  }

    if(!((info_ptr[channel].flag == UTIMER_CHANNEL_STARTED)|| \
         (info_ptr[channel].flag == UTIMER_CHANNEL_TRIGGERED))) {  return ARM_DRIVER_ERROR;  }

    ret = UTIMER_ll_Stop (UTIMER, channel, count_clear_option);

    if (ret == ARM_DRIVER_OK) {
        info_ptr[channel].flag = UTIMER_CHANNEL_STOPPED;
    }
    return ret;
}

static int32_t ARM_UTIMER_Uninitialize (UTIMER_resources_t *UTIMER, uint8_t channel)
{
    int32_t ret = 0;
    UTIMER_TIMER_INFO *info_ptr = (UTIMER_TIMER_INFO*)(UTIMER->info);

    if (UTIMER->info == NULL)                     {   return ARM_DRIVER_ERROR_PARAMETER;  }
    if (channel > ARM_UTIMER_MAX_CHANNEL)         {   return ARM_DRIVER_ERROR_PARAMETER;  }

    info_ptr[channel].CB_function_ptr = NULL;
    info_ptr[channel].channel_direction_backup = 0;
    info_ptr[channel].channel_mode_backup = 0;

    ret = UTIMER_ll_Uninitialize (UTIMER, channel);

    if (ret == ARM_DRIVER_OK) {
        info_ptr[channel].flag = UTIMER_CHANNEL_STOPPED;
    }

    return ret;
}

UTIMER_resources_t UTIMER0_Resources =
{
    .reg_base   = (uint32_t)UTIMER_BASE,
    .info       = (UTIMER_TIMER_INFO*)&Utimer_info
};

static void QEC_IRQHandler_Compare_A (uint8_t channel)
{
    QEC_ll_IRQHandler_Compare_A (&UTIMER0_Resources, channel);
}

static void QEC_IRQHandler_Compare_B (uint8_t channel)
{
    QEC_ll_IRQHandler_Compare_A (&UTIMER0_Resources, channel);
}

static void UTIMER_IRQHandler_Compare_A (uint8_t channel)
{
    UTIMER_ll_IRQHandler_Compare_A (&UTIMER0_Resources, channel);
}

static void UTIMER_IRQHandler_Compare_B (uint8_t channel)
{
    UTIMER_ll_IRQHandler_Compare_B (&UTIMER0_Resources, channel);
}

static void UTIMER_IRQHandler_Compare_C (uint8_t channel)
{
    UTIMER_ll_IRQHandler_Compare_C (&UTIMER0_Resources, channel);
}

static void UTIMER_IRQHandler_Compare_D (uint8_t channel)
{
    UTIMER_ll_IRQHandler_Compare_D (&UTIMER0_Resources, channel);
}

static void UTIMER_IRQHandler_Compare_E (uint8_t channel)
{
    UTIMER_ll_IRQHandler_Compare_E (&UTIMER0_Resources, channel);
}

static void UTIMER_IRQHandler_Compare_F (uint8_t channel)
{
    UTIMER_ll_IRQHandler_Compare_F (&UTIMER0_Resources, channel);
}

static void UTIMER_IRQHandler_Compare_OverFlow (uint8_t channel)
{
    UTIMER_ll_IRQHandler_Compare_OverFlow (&UTIMER0_Resources, channel);
}

static void UTIMER_IRQHandler_Compare_UnderFlow (uint8_t channel)
{
    UTIMER_ll_IRQHandler_Compare_UnderFlow (&UTIMER0_Resources, channel);
}

extern void QEC0_INTR_CMP_A_IRQHandler  (void);
void QEC0_INTR_CMP_A_IRQHandler  (void) {    QEC_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL12);   }
extern void QEC0_INTR_CMP_B_IRQHandler  (void);
void QEC0_INTR_CMP_B_IRQHandler  (void) {    QEC_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL12);   }
extern void QEC1_INTR_CMP_A_IRQHandler  (void);
void QEC1_INTR_CMP_A_IRQHandler  (void) {    QEC_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL13);   }
extern void QEC1_INTR_CMP_B_IRQHandler  (void);
void QEC1_INTR_CMP_B_IRQHandler  (void) {    QEC_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL13);   }
extern void QEC2_INTR_CMP_A_IRQHandler  (void);
void QEC2_INTR_CMP_A_IRQHandler  (void) {    QEC_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL14);   }
extern void QEC2_INTR_CMP_B_IRQHandler  (void);
void QEC2_INTR_CMP_B_IRQHandler  (void) {    QEC_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL14);   }
extern void QEC3_INTR_CMP_A_IRQHandler  (void);
void QEC3_INTR_CMP_A_IRQHandler  (void) {    QEC_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL15);   };
extern void QEC3_INTR_CMP_B_IRQHandler  (void);
void QEC3_INTR_CMP_B_IRQHandler  (void) {    QEC_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL15);   }

extern void UTIMER_IRQHandler0  (void);
void UTIMER_IRQHandler0  (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL0);   }
extern void UTIMER_IRQHandler1  (void);
void UTIMER_IRQHandler1  (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL0);   }
extern void UTIMER_IRQHandler2  (void);
void UTIMER_IRQHandler2  (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL0);   }
extern void UTIMER_IRQHandler3  (void);
void UTIMER_IRQHandler3  (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL0);   }
extern void UTIMER_IRQHandler4  (void);
void UTIMER_IRQHandler4  (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL0);   }
extern void UTIMER_IRQHandler5  (void);
void UTIMER_IRQHandler5  (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL0);   }
extern void UTIMER_IRQHandler6  (void);
void UTIMER_IRQHandler6  (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL0);   }
extern void UTIMER_IRQHandler7  (void);
void UTIMER_IRQHandler7  (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL0);   }

extern void UTIMER_IRQHandler8  (void);
void UTIMER_IRQHandler8  (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL1);   }
extern void UTIMER_IRQHandler9  (void);
void UTIMER_IRQHandler9  (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL1);   }
extern void UTIMER_IRQHandler10 (void);
void UTIMER_IRQHandler10 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL1);   }
extern void UTIMER_IRQHandler11 (void);
void UTIMER_IRQHandler11 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL1);   }
extern void UTIMER_IRQHandler12 (void);
void UTIMER_IRQHandler12 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL1);   }
extern void UTIMER_IRQHandler13 (void);
void UTIMER_IRQHandler13 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL1);   }
extern void UTIMER_IRQHandler14 (void);
void UTIMER_IRQHandler14 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL1);   }
extern void UTIMER_IRQHandler15 (void);
void UTIMER_IRQHandler15 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL1);   }

extern void UTIMER_IRQHandler16 (void);
void UTIMER_IRQHandler16 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL2);   }
extern void UTIMER_IRQHandler17 (void);
void UTIMER_IRQHandler17 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL2);   }
extern void UTIMER_IRQHandler18 (void);
void UTIMER_IRQHandler18 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL2);   }
extern void UTIMER_IRQHandler19 (void);
void UTIMER_IRQHandler19 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL2);   }
extern void UTIMER_IRQHandler20 (void);
void UTIMER_IRQHandler20 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL2);   }
extern void UTIMER_IRQHandler21 (void);
void UTIMER_IRQHandler21 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL2);   }
extern void UTIMER_IRQHandler22 (void);
void UTIMER_IRQHandler22 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL2);   }
extern void UTIMER_IRQHandler23 (void);
void UTIMER_IRQHandler23 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL2);   }

extern void UTIMER_IRQHandler24 (void);
void UTIMER_IRQHandler24 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL3);   }
extern void UTIMER_IRQHandler25 (void);
void UTIMER_IRQHandler25 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL3);   }
extern void UTIMER_IRQHandler26 (void);
void UTIMER_IRQHandler26 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL3);   }
extern void UTIMER_IRQHandler27 (void);
void UTIMER_IRQHandler27 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL3);   }
extern void UTIMER_IRQHandler28 (void);
void UTIMER_IRQHandler28 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL3);   }
extern void UTIMER_IRQHandler29 (void);
void UTIMER_IRQHandler29 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL3);   }
extern void UTIMER_IRQHandler30 (void);
void UTIMER_IRQHandler30 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL3);   }
extern void UTIMER_IRQHandler31 (void);
void UTIMER_IRQHandler31 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL3);   }

extern void UTIMER_IRQHandler32 (void);
void UTIMER_IRQHandler32 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL4);   }
extern void UTIMER_IRQHandler33 (void);
void UTIMER_IRQHandler33 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL4);   }
extern void UTIMER_IRQHandler34 (void);
void UTIMER_IRQHandler34 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL4);   }
extern void UTIMER_IRQHandler35 (void);
void UTIMER_IRQHandler35 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL4);   }
extern void UTIMER_IRQHandler36 (void);
void UTIMER_IRQHandler36 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL4);   }
extern void UTIMER_IRQHandler37 (void);
void UTIMER_IRQHandler37 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL4);   }
extern void UTIMER_IRQHandler38 (void);
void UTIMER_IRQHandler38 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL4);   }
extern void UTIMER_IRQHandler39 (void);
void UTIMER_IRQHandler39 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL4);   }

extern void UTIMER_IRQHandler40 (void);
void UTIMER_IRQHandler40 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL5);   }
extern void UTIMER_IRQHandler41 (void);
void UTIMER_IRQHandler41 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL5);   }
extern void UTIMER_IRQHandler42 (void);
void UTIMER_IRQHandler42 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL5);   }
extern void UTIMER_IRQHandler43 (void);
void UTIMER_IRQHandler43 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL5);   }
extern void UTIMER_IRQHandler44 (void);
void UTIMER_IRQHandler44 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL5);   }
extern void UTIMER_IRQHandler45 (void);
void UTIMER_IRQHandler45 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL5);   }
extern void UTIMER_IRQHandler46 (void);
void UTIMER_IRQHandler46 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL5);   }
extern void UTIMER_IRQHandler47 (void);
void UTIMER_IRQHandler47 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL5);   }

extern void UTIMER_IRQHandler48 (void);
void UTIMER_IRQHandler48 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL6);   }
extern void UTIMER_IRQHandler49 (void);
void UTIMER_IRQHandler49 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL6);   }
extern void UTIMER_IRQHandler50 (void);
void UTIMER_IRQHandler50 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL6);   }
extern void UTIMER_IRQHandler51 (void);
void UTIMER_IRQHandler51 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL6);   }
extern void UTIMER_IRQHandler52 (void);
void UTIMER_IRQHandler52 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL6);   }
extern void UTIMER_IRQHandler53 (void);
void UTIMER_IRQHandler53 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL6);   }
extern void UTIMER_IRQHandler54 (void);
void UTIMER_IRQHandler54 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL6);   }
extern void UTIMER_IRQHandler55 (void);
void UTIMER_IRQHandler55 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL6);   }

extern void UTIMER_IRQHandler56 (void);
void UTIMER_IRQHandler56 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL7);   }
extern void UTIMER_IRQHandler57 (void);
void UTIMER_IRQHandler57 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL7);   }
extern void UTIMER_IRQHandler58 (void);
void UTIMER_IRQHandler58 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL7);   }
extern void UTIMER_IRQHandler59 (void);
void UTIMER_IRQHandler59 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL7);   }
extern void UTIMER_IRQHandler60 (void);
void UTIMER_IRQHandler60 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL7);   }
extern void UTIMER_IRQHandler61 (void);
void UTIMER_IRQHandler61 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL7);   }
extern void UTIMER_IRQHandler62 (void);
void UTIMER_IRQHandler62 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL7);   }
extern void UTIMER_IRQHandler63 (void);
void UTIMER_IRQHandler63 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL7);   }

extern void UTIMER_IRQHandler64 (void);
void UTIMER_IRQHandler64 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL8);   }
extern void UTIMER_IRQHandler65 (void);
void UTIMER_IRQHandler65 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL8);   }
extern void UTIMER_IRQHandler66 (void);
void UTIMER_IRQHandler66 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL8);   }
extern void UTIMER_IRQHandler67 (void);
void UTIMER_IRQHandler67 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL8);   }
extern void UTIMER_IRQHandler68 (void);
void UTIMER_IRQHandler68 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL8);   }
extern void UTIMER_IRQHandler69 (void);
void UTIMER_IRQHandler69 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL8);   }
extern void UTIMER_IRQHandler70 (void);
void UTIMER_IRQHandler70 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL8);   }
extern void UTIMER_IRQHandler71 (void);
void UTIMER_IRQHandler71 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL8);   }

extern void UTIMER_IRQHandler72 (void);
void UTIMER_IRQHandler72 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL9);   }
extern void UTIMER_IRQHandler73 (void);
void UTIMER_IRQHandler73 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL9);   }
extern void UTIMER_IRQHandler74 (void);
void UTIMER_IRQHandler74 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL9);   }
extern void UTIMER_IRQHandler75 (void);
void UTIMER_IRQHandler75 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL9);   }
extern void UTIMER_IRQHandler76 (void);
void UTIMER_IRQHandler76 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL9);   }
extern void UTIMER_IRQHandler77 (void);
void UTIMER_IRQHandler77 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL9);   }
extern void UTIMER_IRQHandler78 (void);
void UTIMER_IRQHandler78 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL9);   }
extern void UTIMER_IRQHandler79 (void);
void UTIMER_IRQHandler79 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL9);   }

extern void UTIMER_IRQHandler80 (void);
void UTIMER_IRQHandler80 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL10);   }
extern void UTIMER_IRQHandler81 (void);
void UTIMER_IRQHandler81 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL10);   }
extern void UTIMER_IRQHandler82 (void);
void UTIMER_IRQHandler82 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL10);   }
extern void UTIMER_IRQHandler83 (void);
void UTIMER_IRQHandler83 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL10);   }
extern void UTIMER_IRQHandler84 (void);
void UTIMER_IRQHandler84 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL10);   }
extern void UTIMER_IRQHandler85 (void);
void UTIMER_IRQHandler85 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL10);   }
extern void UTIMER_IRQHandler86 (void);
void UTIMER_IRQHandler86 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL10);   }
extern void UTIMER_IRQHandler87 (void);
void UTIMER_IRQHandler87 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL10);   }

extern void UTIMER_IRQHandler88 (void);
void UTIMER_IRQHandler88 (void) {    UTIMER_IRQHandler_Compare_A        (ARM_UTIMER_CHANNEL11);   }
extern void UTIMER_IRQHandler89 (void);
void UTIMER_IRQHandler89 (void) {    UTIMER_IRQHandler_Compare_B        (ARM_UTIMER_CHANNEL11);   }
extern void UTIMER_IRQHandler90 (void);
void UTIMER_IRQHandler90 (void) {    UTIMER_IRQHandler_Compare_C        (ARM_UTIMER_CHANNEL11);   }
extern void UTIMER_IRQHandler91 (void);
void UTIMER_IRQHandler91 (void) {    UTIMER_IRQHandler_Compare_D        (ARM_UTIMER_CHANNEL11);   }
extern void UTIMER_IRQHandler92 (void);
void UTIMER_IRQHandler92 (void) {    UTIMER_IRQHandler_Compare_E        (ARM_UTIMER_CHANNEL11);   }
extern void UTIMER_IRQHandler93 (void);
void UTIMER_IRQHandler93 (void) {    UTIMER_IRQHandler_Compare_F        (ARM_UTIMER_CHANNEL11);   }
extern void UTIMER_IRQHandler94 (void);
void UTIMER_IRQHandler94 (void) {    UTIMER_IRQHandler_Compare_UnderFlow(ARM_UTIMER_CHANNEL11);   }
extern void UTIMER_IRQHandler95 (void);
void UTIMER_IRQHandler95 (void) {    UTIMER_IRQHandler_Compare_OverFlow (ARM_UTIMER_CHANNEL11);   }

static int32_t ARM_UTIMER0_Initialize (uint8_t channel, ARM_UTIMER_SignalEvent_t cb_unit_event)
{
    return ARM_UTIMER_Initialize (&UTIMER0_Resources, channel, cb_unit_event);
}

static int32_t ARM_UTIMER0_PowerControl (uint8_t channel, ARM_POWER_STATE state)
{
    return ARM_UTIMER_PowerControl (&UTIMER0_Resources, channel, state);
}

static int32_t ARM_UTIMER0_Control (uint8_t channel, uint32_t control_code, void *arg)
{
    return ARM_UTIMER_Control (&UTIMER0_Resources, channel, control_code, arg);
}

static int32_t ARM_UTIMER0_Configure_Trigger (uint8_t channel, uint8_t triggerFor, uint32_t triggerType)
{
    return ARM_UTIMER_Configure_Trigger (&UTIMER0_Resources, channel, triggerFor, triggerType);
}

static int32_t ARM_UTIMER0_Start (uint8_t channel)
{
    return ARM_UTIMER_Start (&UTIMER0_Resources, channel);
}

static int32_t ARM_UTIMER0_Stop (uint8_t channel, bool count_clear_option)
{
    return ARM_UTIMER_Stop (&UTIMER0_Resources, channel, count_clear_option);
}

static int32_t ARM_UTIMER0_Uninitialize (uint8_t channel)
{
    return ARM_UTIMER_Uninitialize (&UTIMER0_Resources, channel);
}

/*UTIMER Control Block */
ARM_DRIVER_UTIMER DRIVER_UTIMER0 = {
    ARM_UTIMER0_Initialize,
    ARM_UTIMER0_PowerControl,
    ARM_UTIMER0_Control,
    ARM_UTIMER0_Configure_Trigger,
    ARM_UTIMER0_Start,
    ARM_UTIMER0_Stop,
    ARM_UTIMER0_Uninitialize
};
