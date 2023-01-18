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

/* GPIO EXPMST0 Configuration: Control GPIO de-bounce clock. */
static int32_t GPIO_ll_expmst0_ctrl_debounce_clk (GPIO_resources_t *GPIO, bool enable)
{
    /* GPIO EXPMST0 Configuration */
    __IOM uint32_t *reg_gpio_ctrl = (uint32_t *) CFGMST0_GPIO_CTRL;

    uint32_t gpio_base = (uint32_t) GPIO->reg_base;

    if(enable) /* enable EXPMST0 GPIO de-bounce clock. */
    {
        if(gpio_base == GPIO1_BASE)
        {
            *reg_gpio_ctrl |= (1 << 0);
        }
        else if(gpio_base == GPIO2_BASE)
        {
            *reg_gpio_ctrl |= (1 << 4);
        }
        else if(gpio_base == GPIO3_BASE)
        {
            *reg_gpio_ctrl |= (1 << 8);
        }
        else
        {
            return ARM_DRIVER_ERROR;
        }
    }
    else /* disable EXPMST0 GPIO de-bounce clock. */
    {
        if(gpio_base == GPIO1_BASE)
        {
            *reg_gpio_ctrl &= ( ~ (1 << 0) );
        }
        else if(gpio_base == GPIO2_BASE)
        {
            *reg_gpio_ctrl &= ( ~ (1 << 4) );
        }
        else if(gpio_base == GPIO3_BASE)
        {
            *reg_gpio_ctrl &= ( ~ (1 << 8) );
        }
        else
        {
            return ARM_DRIVER_ERROR;
        }
    }

    return ARM_DRIVER_OK;
}

int32_t GPIO_ll_Debounce_Config (GPIO_resources_t *GPIO, uint8_t pin_no, bool option) {

    GPIO_RegInfo *reg_ptr = (GPIO_RegInfo*) GPIO->reg_base;
    int32_t ret = ARM_DRIVER_OK;

    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)   {   return ARM_DRIVER_ERROR_PARAMETER;  }

    ret = GPIO_ll_expmst0_ctrl_debounce_clk(GPIO, option);
    if(ret != ARM_DRIVER_OK)
    {
        return ARM_DRIVER_ERROR;
    }

    if (option)
    {
        reg_ptr->gpio_debounce |= (1 << pin_no);       /**< Enable De-bounce operation >*/
    }
    else
    {
        reg_ptr->gpio_debounce &= ~(1 << pin_no);      /**< Disable De-bounce operation >*/
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
            reg_ptr->gpio_int_polarity |= (1 << pin_no);    /**< Configure interrupt for high edge>*/
        }
        else
        {
            reg_ptr->gpio_int_polarity &= ~(1 << pin_no);    /**< Configure interrupt for low edge >*/
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
