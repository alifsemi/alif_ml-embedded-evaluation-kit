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
 * @file     LPTIMER_ll_drv.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for TIMER.
 * @bug      None.
 * @Note	 None
 ******************************************************************************/
#ifndef __LPTIMER_LL_DRIVER_H__
#define __LPTIMER_LL_DRIVER_H__

#include "RTE_Device.h"
#include "Driver_LPTIMER.h"
#include "RTE_Components.h"
#include CMSIS_device_header

#define LPTIMER_MAX_CHANNEL_NUMBER                          (4)

#define LPTIMER_CONTROL_REG_TIMER_ENABLE_BIT                0x00000001U
#define LPTIMER_CONTROL_REG_TIMER_MODE_BIT                  0x00000002U
#define LPTIMER_CONTROL_REG_TIMER_INTERRUPT_MASK_BIT        0x00000004U
#define LPTIMER_CONTROL_REG_TIMER_PWM_BIT                   0x00000008U
#define LPTIMER_CONTROL_REG_TIMER_ON_100PWM_BIT             0x00000010U

#define LPTIMER_FREE_RUN_MODE                               1
#define LPTIMER_USER_RUN_MODE                               0
#define LPTIMER_CHANNEL_IRQ(chnl)                           (LPTIMER_CHANNEL0_IRQ + chnl) /* Get corresponding irq number */

//TODO: change it to bit wise flags as above
typedef enum _LPTIMER_DRIVER_STATUS {
    LPTIMER_CHANNEL_INITIALIZED = 0x01,                     /**< LPTIMER Channel Initialize state>*/
    LPTIMER_CHANNEL_POWERED = 0x02,                         /**< LPTIMER Channel Power Full state>*/
    LPTIMER_CHANNEL_STARTED = 0x04,                         /**< LPTIMER Channel Start state>*/
    LPTIMER_CHANNEL_CONFIGURED_COUNT1 = 0x08,               /**< LPTIMER Channel COUNT-1 load state>*/
    LPTIMER_CHANNEL_CONFIGURED_COUNT2 = 0x10,               /**< LPTIMER Channel COUNT-2 load state>*/
} LPTIMER_DRIVER_STATUS;

typedef enum _LPTIMER_CLK_SRC {
        LPTIMER_CLK_SOURCE_0 = 0,                           /**< LPTIMER 32KHz Clock Source>*/
        LPTIMER_CLK_SOURCE_1 = 1,                           /**< LPTIMER 128KHz Clock Source>*/
        LPTIMER_CLK_SOURCE_2 = 2,                           /**< LPTIMER External Clock Source>*/
        LPTIMER_CLK_SOURCE_3 = 3,                           /**< LPTIMER Cascade Timer Clock Source>*/
}LPTIMER_CLK_SRC;

typedef struct _CHANNEL_CONTROL_REG {
    __IOM uint32_t load_count;                              /**< Channel load count register >*/
    __IM  uint32_t current_value;                           /**< Channel current running count register >*/
    __IOM uint32_t control_reg;                             /**< Channel operation control register >*/
    __IM  uint32_t eoi;                                     /**< Channel end of interrupt register>*/
    __IM  uint32_t int_status;                              /**< Channel Interrupt status register>*/
} CHANNEL_CONTROL_REG;

typedef struct _LPTIMER_reg_info {
    CHANNEL_CONTROL_REG ch_cntrl_reg[LPTIMER_MAX_CHANNEL_NUMBER]; /**< 8 Channels register instance>*/
    __IM uint32_t int_status;                               /**< Interrupt status register >*/
    __IM uint32_t eoi;                                      /**< Interrupt end of interrupt register >*/
    __IM uint32_t raw_int_status;                           /**< raw Interrupt status register >*/
    __IM uint32_t comp_ver;                                 /**< Timer component version info >*/
    __IOM uint32_t load_count2[LPTIMER_MAX_CHANNEL_NUMBER]; /**< 8 channel instance of load count 2 register >*/
} LPTIMER_reg_info;

typedef struct _LPTIMER_CHANNEL_INFO {
    uint32_t irq_priority;                                  /**< channel IRQ priority information >*/
    bool enable_PWM;                                        /**< channel enabling PWM mode>*/
    bool mode;                                              /**< channel mode user or free run >*/
    LPTIMER_CLK_SRC clk_src;                                /**< channel clock source >*/
    LPTIMER_DRIVER_STATUS flag;                             /**< channel status flag >*/
    ARM_LPTIMER_SignalEvent_t CB_function_ptr;              /**< channel call back function >*/
} LPTIMER_CHANNEL_INFO;

typedef struct _LPTIMER_RESOURCES {
    uint32_t reg_base;                                        /**< LPTIMER Register Offset address >*/
    LPTIMER_CHANNEL_INFO ch_info[LPTIMER_MAX_CHANNEL_NUMBER]; /**< Pointer to Info structure of LPTIMER>*/
} LPTIMER_RESOURCES;

int32_t LPTIMER_ll_Clk_Select       (LPTIMER_RESOURCES *LPTIMER, uint8_t channel );
int32_t LPTIMER_ll_Initialize       (LPTIMER_RESOURCES *LPTIMER, uint8_t channel);
int32_t LPTIMER_ll_Irq_Enable       (LPTIMER_RESOURCES *LPTIMER, uint8_t channel);
int32_t LPTIMER_ll_Set_Count1_Value (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, uint32_t *arg);
int32_t LPTIMER_ll_Set_Count2_Value (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, uint32_t *arg);
void    LPTIMER_ll_Get_Count_Value  (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, uint32_t *arg);
void    LPTIMER_ll_Irq_Disable      (uint8_t channel);
void    LPTIMER_ll_Start            (LPTIMER_RESOURCES *LPTIMER, uint8_t channel);
void    LPTIMER_ll_Stop             (LPTIMER_RESOURCES *LPTIMER, uint8_t channel);

#endif /*<__LPTIMER_LL_DRIVER_H__>*/
