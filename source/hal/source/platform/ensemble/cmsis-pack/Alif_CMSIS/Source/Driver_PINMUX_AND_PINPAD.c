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
 * @file     Driver_PINMUX_AND_PINPAD.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    Driver for PINMUX and PINPAD.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#include "PINMUX_AND_PINPAD_Internal.h"

/**
 \fn            int32_t PINMUX_Config (uint8_t port, uint8_t pin_no, uint8_t AF_number);
 \brief         Configuring the PINMUX.
 \param[in]     port: GPIO PORT.
 \param[in]     pin_no: GPIO Pin .
 \param[in]     AF_number: Alternate function number.
 \param[out]    int32_t: execution status.
 */

int32_t PINMUX_Config (uint8_t port, uint8_t pin_no, uint8_t AF_number) {

    volatile uint32_t *ptr;
    uint32_t value = 0;

    if (port > PORT_NUMBER_4)                       { return ARM_DRIVER_ERROR_PARAMETER; }
    if (pin_no > PIN_NUMBER_31)                     { return ARM_DRIVER_ERROR_PARAMETER; }
    if (AF_number > PINMUX_ALTERNATE_FUNCTION_7)    { return ARM_DRIVER_ERROR_PARAMETER; }

    if (port == PORT_NUMBER_4)
	{
		if (pin_no > PIN_NUMBER_7)                  { return ARM_DRIVER_ERROR_PARAMETER; }

	           /* Configure GPIO_SWPORTA_CTL register - select AUX or Data Register signal */
	        ptr = (volatile uint32_t*) (GPIO4_BASE + 0x08);
	        if (AF_number == PINMUX_ALTERNATE_FUNCTION_0)
	        {
	               /* 0 indicates software mode - GPIO pin is driven by Data Register value set by software */
	          *ptr &= ~(1 << pin_no);
	        }
	        else if (AF_number == PINMUX_ALTERNATE_FUNCTION_1)
	        {
	               /* 1 indicates hardware mode - GPIO pin is driven by Auxiliary signal - connected to LPTIMER input or output */
	                  *ptr |= (1 << pin_no);
	        }
	        return ARM_DRIVER_OK;
	 }

    port = (port *16) + 16;

    if(pin_no >= 24)
    {
        ptr = (volatile uint32_t*) (PINMUX_BASE + port + 0x0C);
        value = pin_no-24;
    }
    else if(pin_no >= 16)
    {
        ptr = (volatile uint32_t*) (PINMUX_BASE + port + 0x08);
        value = pin_no-16;
    }
    else if(pin_no >= 8)
    {
        ptr = (volatile uint32_t*) (PINMUX_BASE + port + 0x04);
        value = pin_no-8;
    }
    else
    {
        ptr = (volatile uint32_t*) (PINMUX_BASE + port);
        value = pin_no;
    }

    value *= 4;
    *ptr &= ~(0xF << value);
    *ptr |= (AF_number << value);
    return ARM_DRIVER_OK;
}

/**
 \fn            int32_t PINMUX_read (uint8_t port, uint8_t pin_no, uint32_t *AF_number);
 \brief         read the PINMUX value.
 \param[in]     port: GPIO PORT.
 \param[in]     pin_no: GPIO Pin .
 \param[in]     AF_number: pointer to Alternate function number.
 \param[out]    int32_t: execution status.
 */

int32_t PINMUX_read (uint8_t port, uint8_t pin_no, uint32_t *AF_number) {

    volatile uint32_t *ptr;
    uint32_t value = 0;

    if (port > PORT_NUMBER_4)                       { return ARM_DRIVER_ERROR_PARAMETER; }
    if (pin_no > PIN_NUMBER_31)                     { return ARM_DRIVER_ERROR_PARAMETER; }
    if (AF_number == 0)                             { return ARM_DRIVER_ERROR_PARAMETER; }

    if (port == PORT_NUMBER_4)
	{
		if (pin_no > PIN_NUMBER_7)                  { return ARM_DRIVER_ERROR_PARAMETER; }

	        ptr = (volatile uint32_t*) (GPIO4_BASE + 0x08);
	        *AF_number = (*ptr & (1 << pin_no)) ? PINMUX_ALTERNATE_FUNCTION_1 : PINMUX_ALTERNATE_FUNCTION_0;
	        return ARM_DRIVER_OK;
	}

    if (port > PORT_NUMBER_4)                       { return ARM_DRIVER_ERROR_PARAMETER; }
    if (pin_no > PIN_NUMBER_31)                     { return ARM_DRIVER_ERROR_PARAMETER; }
    if (AF_number == 0)                             { return ARM_DRIVER_ERROR_PARAMETER; }

    port = (port *16) + 16;

    if(pin_no >= 24)
    {
        ptr = (volatile uint32_t*) (PINMUX_BASE + port + 0x0C);
        value = pin_no-24;
    }
    else if(pin_no >= 16)
    {
        ptr = (volatile uint32_t*) (PINMUX_BASE + port + 0x08);
        value = pin_no-16;
    }
    else if(pin_no >= 8)
    {
        ptr = (volatile uint32_t*) (PINMUX_BASE + port + 0x04);
        value = pin_no-8;
    }
    else
    {
        ptr = (volatile uint32_t*) (PINMUX_BASE + port);
        value = pin_no;
    }

    value *= 4;

    *AF_number = (((*ptr) >> value) & 0xF);

    return ARM_DRIVER_OK;
}

/**
 \fn            int32_t PINPAD_Config (uint8_t port, uint8_t pin_no, uint8_t function);
 \brief         Configuring the PINPAD.
 \param[in]     port: GPIO PORT.
 \param[in]     pin_no: GPIO Pin .
 \param[in]     function: function to be configure.
 \param[out]    int32_t: execution status.
 */
int32_t PINPAD_Config (uint8_t port, uint8_t pin_no, uint8_t function) {

    Pad_RegInfo *reg_ptr = (Pad_RegInfo*)(PADCTRL_BASE);
    Pad_RegInfo *reg_port4 = (Pad_RegInfo*)(VBAT_GPIOV_REGS);

    if (port > PORT_NUMBER_4)                       { return ARM_DRIVER_ERROR_PARAMETER; }
    if (pin_no > PIN_NUMBER_31)                     { return ARM_DRIVER_ERROR_PARAMETER; }

    switch (port)
    {
        case PORT_NUMBER_1:
        {
            reg_ptr->port_1[pin_no].pad = function;
            break;
        }
        case PORT_NUMBER_2:
        {
            reg_ptr->port_2[pin_no].pad = function;
            break;
        }
        case PORT_NUMBER_3:
        {
            if (pin_no >= PORT3_MAX_PIN_NUMBER)
            {
                return ARM_DRIVER_ERROR_PARAMETER;   // Port-3 have 0 to 23 pins, throwing error for other pin index
            }
            reg_ptr->port_3[pin_no].pad = function;
            break;
        }
        case PORT_NUMBER_4:
        {
            if (pin_no >= PORT4_MAX_PIN_NUMBER)
            {
                return ARM_DRIVER_ERROR_PARAMETER;    // Port-4 have 0 to 7 pins, throwing error for other pin index
            }
            reg_port4->port_4[pin_no].pad = function;
            break;
        }
    }
    return ARM_DRIVER_OK;
}

/**
 \fn            int32_t PINPAD_read (uint8_t port, uint8_t pin_no, uint32_t *function);
 \brief         reading the PINPAD value.
 \param[in]     port: GPIO PORT.
 \param[in]     pin_no: GPIO Pin.
 \param[in]     function: pointer to function configured.
 \param[out]    int32_t: execution status.
 */
int32_t PINPAD_read (uint8_t port, uint8_t pin_no, uint32_t *function) {

    Pad_RegInfo *reg_ptr = (Pad_RegInfo*)(PADCTRL_BASE);
    Pad_RegInfo *reg_port4 = (Pad_RegInfo*)(VBAT_GPIOV_REGS);

    if (port > PORT_NUMBER_4)                       { return ARM_DRIVER_ERROR_PARAMETER; }
    if (pin_no > PIN_NUMBER_31)                     { return ARM_DRIVER_ERROR_PARAMETER; }
    if (function == 0)                              { return ARM_DRIVER_ERROR_PARAMETER; }

    switch (port)
    {
        case PORT_NUMBER_1:
        {
            *function = reg_ptr->port_1[pin_no].pad;
            break;
        }
        case PORT_NUMBER_2:
        {
            *function = reg_ptr->port_2[pin_no].pad;
            break;
        }
        case PORT_NUMBER_3:
        {
            if (pin_no >= PORT3_MAX_PIN_NUMBER)
            {
                return ARM_DRIVER_ERROR_PARAMETER;   // Port-3 have 0 to 23 pins, throwing error for other pin index
            }
            *function = reg_ptr->port_3[pin_no].pad;
            break;
        }
        case PORT_NUMBER_4:
        {
            if (pin_no >= PORT4_MAX_PIN_NUMBER)
            {
                return ARM_DRIVER_ERROR_PARAMETER;    // Port-4 have 0 to 7 pins, throwing error for other pin index
            }
            *function = reg_port4->port_4[pin_no].pad;
            break;
        }
    }
    return ARM_DRIVER_OK;
}

