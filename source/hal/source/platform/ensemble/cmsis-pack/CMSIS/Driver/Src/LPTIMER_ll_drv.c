/* Copyright (c) 2021 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
/**************************************************************************//**
 * @file     LPTIMER_ll_drv.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for LPTIMER.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "LPTIMER_ll_drv.h"

int32_t LPTIMER_ll_Clk_Select (LPTIMER_RESOURCES *lptimer, uint8_t channel ) {

    if (!((lptimer->ch_info[channel].clk_src >= LPTIMER_CLK_SOURCE_0) &&
          (lptimer->ch_info[channel].clk_src <= LPTIMER_CLK_SOURCE_3) ))
    {
        return ARM_DRIVER_ERROR;
    }

    uint32_t *reg_clk_sel = (uint32_t*) (LPTIMER_CLK_SEL_REG);

    *reg_clk_sel &= ( ~ ( 0x3 << (2 * channel) ) );
    *reg_clk_sel |= ( lptimer->ch_info[channel].clk_src << (2 * channel) );

    return ARM_DRIVER_OK;
}

int32_t LPTIMER_ll_Initialize (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    int32_t ret = ARM_DRIVER_OK;

    LPTIMER_reg_info *reg_ptr = (LPTIMER_reg_info*) LPTIMER->reg_base;

    reg_ptr->ch_cntrl_reg[channel].control_reg &= ~LPTIMER_CONTROL_REG_TIMER_ENABLE_BIT;

    ret = LPTIMER_ll_Clk_Select (LPTIMER, channel);
    if(ret != ARM_DRIVER_OK)	 		{	return ret;	  }

    if (LPTIMER->ch_info[channel].mode) {
        reg_ptr->ch_cntrl_reg[channel].control_reg &= ~LPTIMER_CONTROL_REG_TIMER_MODE_BIT;
    } else {
        reg_ptr->ch_cntrl_reg[channel].control_reg |=  LPTIMER_CONTROL_REG_TIMER_MODE_BIT;
    }

    reg_ptr->ch_cntrl_reg[channel].control_reg &= ~LPTIMER_CONTROL_REG_TIMER_INTERRUPT_MASK_BIT;

    if (LPTIMER->ch_info[channel].enable_PWM) {
        reg_ptr->ch_cntrl_reg[channel].control_reg |= LPTIMER_CONTROL_REG_TIMER_PWM_BIT;
    }
    return ARM_DRIVER_OK;
}

int32_t LPTIMER_ll_Irq_Enable (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    NVIC_SetPriority(LPTIMER_CHANNEL_IRQ(channel), LPTIMER->ch_info[channel].irq_priority);

    if (NVIC_GetPriority(LPTIMER_CHANNEL_IRQ(channel)) != LPTIMER->ch_info[channel].irq_priority)
    {
        return ARM_DRIVER_ERROR;
    }
    NVIC_EnableIRQ(LPTIMER_CHANNEL_IRQ(channel));

    return ARM_DRIVER_OK;
}

void LPTIMER_ll_Irq_Disable (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    NVIC_ClearPendingIRQ(LPTIMER_CHANNEL_IRQ(channel));

    NVIC_DisableIRQ(LPTIMER_CHANNEL_IRQ(channel));
}

int32_t LPTIMER_ll_Set_Count1_Value (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, uint32_t *arg) {

    LPTIMER_reg_info *reg_ptr = (LPTIMER_reg_info*) LPTIMER->reg_base;

    if (LPTIMER->ch_info[channel].mode == 0)
    {
        reg_ptr->ch_cntrl_reg[channel].load_count = *arg;
        return ARM_DRIVER_OK;
    }
    return ARM_DRIVER_ERROR_PARAMETER;
}

int32_t LPTIMER_ll_Set_Count2_Value (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, uint32_t *arg) {

    LPTIMER_reg_info *reg_ptr = (LPTIMER_reg_info*) LPTIMER->reg_base;

    if (LPTIMER->ch_info[channel].enable_PWM != 0)
    {
        reg_ptr->load_count2[channel] = *arg;
        return ARM_DRIVER_OK;
    }
    return ARM_DRIVER_ERROR_PARAMETER;
}

void LPTIMER_ll_Get_Count_Value (LPTIMER_RESOURCES *LPTIMER, uint8_t channel, uint32_t *arg) {

    LPTIMER_reg_info *reg_ptr = (LPTIMER_reg_info*) LPTIMER->reg_base;

    *arg = reg_ptr->ch_cntrl_reg[channel].current_value;
}
void LPTIMER_ll_Start (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    LPTIMER_reg_info *reg_ptr = (LPTIMER_reg_info*) LPTIMER->reg_base;

    reg_ptr->ch_cntrl_reg[channel].control_reg |= LPTIMER_CONTROL_REG_TIMER_ENABLE_BIT;
}

void LPTIMER_ll_Stop (LPTIMER_RESOURCES *LPTIMER, uint8_t channel) {

    LPTIMER_reg_info *reg_ptr = (LPTIMER_reg_info*) LPTIMER->reg_base;

    reg_ptr->ch_cntrl_reg[channel].control_reg &= ~LPTIMER_CONTROL_REG_TIMER_ENABLE_BIT;
}
