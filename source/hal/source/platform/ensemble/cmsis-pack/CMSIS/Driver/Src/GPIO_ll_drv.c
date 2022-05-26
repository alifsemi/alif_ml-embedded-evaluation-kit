/* Copyright (c) 2020 - 2021 ALIF SEMICONDUCTOR

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
 * @file     GPIO_ll_drv.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for GPIO module.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "GPIO_ll_drv.h"

void GPIO_ll_IRQ_Handler (GPIO_resources_t *GPIO, uint8_t pin_no) {

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*)GPIO->reg_base;

    if (reg_ptr->gpio_intstatus != 0)
    {
        reg_ptr->gpio_porta_eoi |= reg_ptr->gpio_intstatus;
    }

    if (GPIO->cb_event[pin_no] != NULL)
    {
        GPIO->cb_event[pin_no](ARM_GPIO_IRQ_EVENT_EXTERNAL);
    }
}

int32_t GPIO_ll_PowerControl (GPIO_resources_t *GPIO, uint8_t pin_no, ARM_POWER_STATE state) {

    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)   {   return ARM_DRIVER_ERROR_PARAMETER;  }

    switch (state)
    {
        case ARM_POWER_OFF:
        {
            //TODO: clock handling; and pinmux handling
            break;
        }
        case ARM_POWER_FULL:
        {
            //TODO: clock handling; and pinmux handling
            break;
        }
        default:
        {
            return ARM_DRIVER_ERROR_UNSUPPORTED;
            break;
        }
    }
    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_SetDirection (GPIO_resources_t *GPIO, uint8_t pin_no, uint32_t dir) {

    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)   {   return ARM_DRIVER_ERROR_PARAMETER;  }

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;

    switch (dir)
    {
        case GPIO_PIN_DIRECTION_INPUT:
        {
	     reg_ptr->gpio_swport_ddr &= ~(1 << pin_no);     /**< Configuring Pin's to Input Direction >*/
	     break;
        }
        case GPIO_PIN_DIRECTION_OUTPUT:
        {
	     reg_ptr->gpio_swport_ddr |= (1 << pin_no);   /**< Configuring Pin's to Output Direction >*/
	     break;
        }
        default:
        {
	     return ARM_DRIVER_ERROR_PARAMETER;
	     break;
        }
    }
    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_GetDirection (GPIO_resources_t *GPIO, uint8_t pin_no, uint32_t *dir) {

    if (dir == NULL)                            {   return ARM_DRIVER_ERROR_PARAMETER;  }
    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)     {   return ARM_DRIVER_ERROR_PARAMETER;  }

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;

    if (reg_ptr->gpio_swport_ddr & (1 << pin_no)) {
        *dir = GPIO_PIN_DIRECTION_OUTPUT;
    }
    else {
        *dir = GPIO_PIN_DIRECTION_INPUT;
    }
    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_SetValue (GPIO_resources_t *GPIO, uint8_t pin_no, uint32_t value) {
   GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;
   if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)             {   return ARM_DRIVER_ERROR_PARAMETER;  }

   switch (value)
    {
        case GPIO_PIN_OUTPUT_STATE_LOW:
        {
		reg_ptr->gpio_swport_dr &= ~(1 << pin_no);    /**< Set LOW(0) to the pin >*/
		break;
        }
        case GPIO_PIN_OUTPUT_STATE_HIGH:
        {
		reg_ptr->gpio_swport_dr |= (1 << pin_no);    /**< Set HIGH(1) to the pin >*/
		break;
        }
        case GPIO_PIN_OUTPUT_STATE_TOGGLE:
        {
		reg_ptr->gpio_swport_dr ^= (1 << pin_no);    /**< Toggles Pin's value >*/
		break;
        }
        default:
        {
		return ARM_DRIVER_ERROR_PARAMETER;
		break;
        }
    }
    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_GetValue (GPIO_resources_t *GPIO, uint8_t pin_no, uint32_t *value) {

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;

    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER) {   return ARM_DRIVER_ERROR_PARAMETER;  }

    if (reg_ptr->gpio_ext_port & (1 << pin_no))
    {
        *value = GPIO_PIN_STATE_HIGH;
    } else {
        *value = GPIO_PIN_STATE_LOW;
    }

    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_Debounce_Config (GPIO_resources_t *GPIO, uint8_t pin_no, bool option) {

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;

    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)   {   return ARM_DRIVER_ERROR_PARAMETER;  }

    if (option)
    {
        reg_ptr->gpio_debounce |= (1 << pin_no);       /**< Enable Denounce operation >*/
    }
    else
    {
        reg_ptr->gpio_debounce &= ~(1 << pin_no);      /**< Disable Denounce operation >*/
    }
    return ARM_DRIVER_OK;
    }

int32_t GPIO_ll_Read_Config_Value (GPIO_resources_t *GPIO, bool option, uint32_t *value) {

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;

    if (option)
    {
        *value = reg_ptr->gpio_config_reg2;     /**< Read Configuration from Reg1>*/
    }
    else
    {
        *value = reg_ptr->gpio_config_reg1;     /**< Read Configuration from Reg2>*/
    }
    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_Enable_Interrupt (GPIO_resources_t *GPIO, uint8_t pin_no, uint32_t arg) {

    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)   {   return ARM_DRIVER_ERROR_PARAMETER;  }

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;

    reg_ptr->gpio_inten     |= (1 << pin_no);
    reg_ptr->gpio_intmask   &= ~(1 << pin_no);

    if (ARM_GPIO_BIT_IRQ_BOTH_EDGE(arg))
    {
        reg_ptr->gpio_int_bothedge |= (1 << pin_no);    /**< Configure interrupt for both edge >*/
    }
    else
    {
        if (ARM_GPIO_BIT_IRQ_POLARITY(arg))
        {
            reg_ptr->gpio_int_polarity |= (1 << pin_no);    /**< Configure interrupt for low edge>*/
        }
        else
        {
            reg_ptr->gpio_int_polarity &= ~(1 << pin_no);    /**< Configure interrupt for high edge >*/
        }

        if (ARM_GPIO_BIT_IRQ_SENSITIVE(arg))
        {
            reg_ptr->gpio_inttype_level |= (1 << pin_no);    /**< Configure interrupt as edge sensitive >*/
        }
        else
        {
            reg_ptr->gpio_inttype_level &= ~(1 << pin_no);    /**< Configure interrupt as level sensitive >*/
        }
    }

    NVIC_SetPriority ((GPIO->IRQ_base_num + pin_no), GPIO->IRQ_priority[pin_no]);

    if (NVIC_GetPriority (GPIO->IRQ_base_num + pin_no) != GPIO->IRQ_priority[pin_no])
    {
        return ARM_DRIVER_ERROR;
    }

    NVIC_EnableIRQ (GPIO->IRQ_base_num + pin_no);

    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_Disable_Interrupt   (GPIO_resources_t *GPIO, uint8_t pin_no) {

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;

    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)   {   return ARM_DRIVER_ERROR_PARAMETER;  }


    reg_ptr->gpio_inten &= ~(1 << pin_no);           /**< Setting back to default value >*/
    reg_ptr->gpio_intmask &= ~(1 << pin_no);         /**< Setting back to default value >*/
    reg_ptr->gpio_int_polarity &= ~(1 << pin_no);    /**< Setting back to default value >*/
    reg_ptr->gpio_inttype_level &= ~(1 << pin_no);   /**< Setting back to default value >*/

    NVIC_ClearPendingIRQ (GPIO->IRQ_base_num + pin_no);  /**< Clearing the IRQ pending list>*/
    NVIC_DisableIRQ (GPIO->IRQ_base_num + pin_no);       /**< Disabling the Interrupt >*/
    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_Uninitialize (GPIO_resources_t *GPIO, uint8_t pin_no) {

    GPIO->cb_event[pin_no] = NULL;

    return ARM_DRIVER_OK;
}
