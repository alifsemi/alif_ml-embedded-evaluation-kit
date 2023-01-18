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
 * @file     Driver_GPIO.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for GPIO.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "GPIO_ll_drv.h"

#if !(RTE_GPIO1 || RTE_GPIO2 || RTE_GPIO3 || RTE_GPIO4)
#error "GPIO is not enabled in the RTE_Device.h"
#endif

#if (defined(RTE_Drivers_GPIO1) && !RTE_GPIO1)
#error "GPIO1 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_GPIO2) && !RTE_GPIO2)
#error "GPIO2 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_GPIO3) && !RTE_GPIO3)
#error "GPIO3 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_GPIO4) && !RTE_GPIO4)
#error "GPIO4 not configured in RTE_Device.h!"
#endif


int32_t GPIO_Initialize (GPIO_resources_t *GPIO, ARM_GPIO_SignalEvent_t cb_event, uint8_t pin_no)
{
    if (pin_no >= GPIO_PORT_MAX_PIN_NUMBER)   {   return ARM_DRIVER_ERROR_PARAMETER;  }
    GPIO->cb_event[pin_no] = cb_event;
    return ARM_DRIVER_OK;
}

int32_t GPIO_PowerControl (GPIO_resources_t *GPIO, uint8_t pin_no, ARM_POWER_STATE state)
{
    return GPIO_ll_PowerControl (GPIO, pin_no, state);
}

int32_t GPIO_SetDirection (GPIO_resources_t *GPIO, uint8_t pin_no, GPIO_PIN_DIRECTION dir)
{
    return GPIO_ll_SetDirection (GPIO, pin_no, dir);
}

int32_t GPIO_GetDirection (GPIO_resources_t *GPIO, uint8_t pin_no, uint32_t *dir)
{
    return GPIO_ll_GetDirection (GPIO, pin_no, dir);
}

int32_t GPIO_SetValue (GPIO_resources_t *GPIO, uint8_t pin_no, uint32_t value)
{
    return GPIO_ll_SetValue (GPIO, pin_no, value);
}

int32_t GPIO_GetValue (GPIO_resources_t *GPIO, uint8_t pin_no, uint32_t *value)
{
    return GPIO_ll_GetValue (GPIO, pin_no, value);
}

int32_t GPIO_Control (GPIO_resources_t *GPIO, uint8_t pin_no, GPIO_OPERATION control_code, uint32_t *arg)
{
    switch (control_code)
    {
        case ARM_GPIO_CONFIG_DEBOUNCE :
        {
            return GPIO_ll_Debounce_Config (GPIO, pin_no, *arg);
            break;
        }
        case ARM_GPIO_ENABLE_INTERRUPT :
        {
            return GPIO_ll_Enable_Interrupt (GPIO, pin_no, *arg);
            break;
        }
        case ARM_GPIO_DISABLE_INTERRUPT :
        {
            return GPIO_ll_Disable_Interrupt (GPIO, pin_no);
            break;
        }
        case ARM_GPIO_GET_CONFIG_VALUE1 :
        {
            return GPIO_ll_Read_Config_Value (GPIO, 1, arg);
            break;
        }
        case ARM_GPIO_GET_CONFIG_VALUE2 :
        {
            return GPIO_ll_Read_Config_Value (GPIO, 0, arg);
            break;
        }
        default:
        {
            return ARM_DRIVER_ERROR;
            break;
        }
    }

    return ARM_DRIVER_OK;
}

int32_t GPIO_Uninitialize (GPIO_resources_t *GPIO, uint8_t pin_no)
{
    return GPIO_ll_Uninitialize (GPIO, pin_no);
}

/**<GPIO Instance 1>*/
GPIO_resources_t GPIO1 = {
    .reg_base = (uint32_t*) GPIO1_BASE,
    .IRQ_base_num = GPIO1_PIN0_IRQ,
    .IRQ_priority = {
        RTE_GPIO1_PIN0_IRQ_PRIORITY,
        RTE_GPIO1_PIN1_IRQ_PRIORITY,
        RTE_GPIO1_PIN2_IRQ_PRIORITY,
        RTE_GPIO1_PIN3_IRQ_PRIORITY,
        RTE_GPIO1_PIN4_IRQ_PRIORITY,
        RTE_GPIO1_PIN5_IRQ_PRIORITY,
        RTE_GPIO1_PIN6_IRQ_PRIORITY,
        RTE_GPIO1_PIN7_IRQ_PRIORITY,
        RTE_GPIO1_PIN8_IRQ_PRIORITY,
        RTE_GPIO1_PIN9_IRQ_PRIORITY,
        RTE_GPIO1_PIN10_IRQ_PRIORITY,
        RTE_GPIO1_PIN11_IRQ_PRIORITY,
        RTE_GPIO1_PIN12_IRQ_PRIORITY,
        RTE_GPIO1_PIN13_IRQ_PRIORITY,
        RTE_GPIO1_PIN14_IRQ_PRIORITY,
        RTE_GPIO1_PIN15_IRQ_PRIORITY,
        RTE_GPIO1_PIN16_IRQ_PRIORITY,
        RTE_GPIO1_PIN17_IRQ_PRIORITY,
        RTE_GPIO1_PIN18_IRQ_PRIORITY,
        RTE_GPIO1_PIN19_IRQ_PRIORITY,
        RTE_GPIO1_PIN20_IRQ_PRIORITY,
        RTE_GPIO1_PIN21_IRQ_PRIORITY,
        RTE_GPIO1_PIN22_IRQ_PRIORITY,
        RTE_GPIO1_PIN23_IRQ_PRIORITY,
        RTE_GPIO1_PIN24_IRQ_PRIORITY,
        RTE_GPIO1_PIN25_IRQ_PRIORITY,
        RTE_GPIO1_PIN26_IRQ_PRIORITY,
        RTE_GPIO1_PIN27_IRQ_PRIORITY,
        RTE_GPIO1_PIN28_IRQ_PRIORITY,
        RTE_GPIO1_PIN29_IRQ_PRIORITY,
        RTE_GPIO1_PIN30_IRQ_PRIORITY,
        RTE_GPIO1_PIN31_IRQ_PRIORITY
    }
};

void GPIO1_PIN0_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 0);    }
void GPIO1_PIN1_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 1);    }
void GPIO1_PIN2_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 2);    }
void GPIO1_PIN3_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 3);    }
void GPIO1_PIN4_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 4);    }
void GPIO1_PIN5_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 5);    }
void GPIO1_PIN6_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 6);    }
void GPIO1_PIN7_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 7);    }
void GPIO1_PIN8_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 8);    }
void GPIO1_PIN9_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 9);    }
void GPIO1_PIN10_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 10);   }
void GPIO1_PIN11_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 11);   }
void GPIO1_PIN12_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 12);   }
void GPIO1_PIN13_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 13);   }
void GPIO1_PIN14_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 14);   }
void GPIO1_PIN15_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 15);   }
void GPIO1_PIN16_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 16);   }
void GPIO1_PIN17_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 17);   }
void GPIO1_PIN18_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 18);   }
void GPIO1_PIN19_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 19);   }
void GPIO1_PIN20_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 20);   }
void GPIO1_PIN21_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 21);   }
void GPIO1_PIN22_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 22);   }
void GPIO1_PIN23_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 23);   }
void GPIO1_PIN24_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 24);   }
void GPIO1_PIN25_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 25);   }
void GPIO1_PIN26_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 26);   }
void GPIO1_PIN27_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 27);   }
void GPIO1_PIN28_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 28);   }
void GPIO1_PIN29_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 29);   }
void GPIO1_PIN30_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 30);   }
void GPIO1_PIN31_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO1, 31);   }

int32_t ARM_GPIO1_Initialize (uint8_t pin_no, ARM_GPIO_SignalEvent_t cb_event)
{
    return GPIO_Initialize (&GPIO1, cb_event, pin_no);
}

int32_t ARM_GPIO1_PowerControl (uint8_t pin_no, ARM_POWER_STATE state)
{
    return GPIO_PowerControl (&GPIO1, pin_no, state);
}

int32_t ARM_GPIO1_SetDirection (uint8_t pin_no, uint32_t dir)
{
    return GPIO_SetDirection (&GPIO1, pin_no, dir);
}

int32_t ARM_GPIO1_GetDirection (uint8_t pin_no, uint32_t *dir)
{
    return GPIO_GetDirection (&GPIO1, pin_no, dir);
}

int32_t ARM_GPIO1_SetValue (uint8_t pin_no, uint32_t value)
{
    return GPIO_SetValue (&GPIO1, pin_no, value);
}

int32_t ARM_GPIO1_GetValue (uint8_t pin_no, uint32_t *value)
{
    return GPIO_GetValue (&GPIO1, pin_no, value);
}

int32_t ARM_GPIO1_Control (uint8_t pin_no, GPIO_OPERATION control_code, uint32_t *arg)
{
    return GPIO_Control (&GPIO1, pin_no, control_code, arg);
}

int32_t ARM_GPIO1_Uninitialize (uint8_t pin_no) {

    return GPIO_Uninitialize (&GPIO1, pin_no);
}

extern ARM_DRIVER_GPIO Driver_GPIO1;
ARM_DRIVER_GPIO Driver_GPIO1 = {
    ARM_GPIO1_Initialize,
    ARM_GPIO1_PowerControl,
    ARM_GPIO1_SetDirection,
    ARM_GPIO1_GetDirection,
    ARM_GPIO1_SetValue,
    ARM_GPIO1_GetValue,
    ARM_GPIO1_Control,
    ARM_GPIO1_Uninitialize
};



/**<GPIO Instance 2>*/

GPIO_resources_t GPIO2 = {
    .reg_base = (uint32_t*) GPIO2_BASE,
    .IRQ_base_num = GPIO2_PIN0_IRQ,
    .IRQ_priority = {
        RTE_GPIO2_PIN0_IRQ_PRIORITY,
        RTE_GPIO2_PIN1_IRQ_PRIORITY,
        RTE_GPIO2_PIN2_IRQ_PRIORITY,
        RTE_GPIO2_PIN3_IRQ_PRIORITY,
        RTE_GPIO2_PIN4_IRQ_PRIORITY,
        RTE_GPIO2_PIN5_IRQ_PRIORITY,
        RTE_GPIO2_PIN6_IRQ_PRIORITY,
        RTE_GPIO2_PIN7_IRQ_PRIORITY,
        RTE_GPIO2_PIN8_IRQ_PRIORITY,
        RTE_GPIO2_PIN9_IRQ_PRIORITY,
        RTE_GPIO2_PIN10_IRQ_PRIORITY,
        RTE_GPIO2_PIN11_IRQ_PRIORITY,
        RTE_GPIO2_PIN12_IRQ_PRIORITY,
        RTE_GPIO2_PIN13_IRQ_PRIORITY,
        RTE_GPIO2_PIN14_IRQ_PRIORITY,
        RTE_GPIO2_PIN15_IRQ_PRIORITY,
        RTE_GPIO2_PIN16_IRQ_PRIORITY,
        RTE_GPIO2_PIN17_IRQ_PRIORITY,
        RTE_GPIO2_PIN18_IRQ_PRIORITY,
        RTE_GPIO2_PIN19_IRQ_PRIORITY,
        RTE_GPIO2_PIN20_IRQ_PRIORITY,
        RTE_GPIO2_PIN21_IRQ_PRIORITY,
        RTE_GPIO2_PIN22_IRQ_PRIORITY,
        RTE_GPIO2_PIN23_IRQ_PRIORITY,
        RTE_GPIO2_PIN24_IRQ_PRIORITY,
        RTE_GPIO2_PIN25_IRQ_PRIORITY,
        RTE_GPIO2_PIN26_IRQ_PRIORITY,
        RTE_GPIO2_PIN27_IRQ_PRIORITY,
        RTE_GPIO2_PIN28_IRQ_PRIORITY,
        RTE_GPIO2_PIN29_IRQ_PRIORITY,
        RTE_GPIO2_PIN30_IRQ_PRIORITY,
        RTE_GPIO2_PIN31_IRQ_PRIORITY
    }
};

void GPIO2_PIN0_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 0);    }
void GPIO2_PIN1_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 1);    }
void GPIO2_PIN2_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 2);    }
void GPIO2_PIN3_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 3);    }
void GPIO2_PIN4_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 4);    }
void GPIO2_PIN5_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 5);    }
void GPIO2_PIN6_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 6);    }
void GPIO2_PIN7_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 7);    }
void GPIO2_PIN8_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 8);    }
void GPIO2_PIN9_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 9);    }
void GPIO2_PIN10_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 10);   }
void GPIO2_PIN11_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 11);   }
void GPIO2_PIN12_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 12);   }
void GPIO2_PIN13_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 13);   }
void GPIO2_PIN14_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 14);   }
void GPIO2_PIN15_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 15);   }
void GPIO2_PIN16_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 16);   }
void GPIO2_PIN17_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 17);   }
void GPIO2_PIN18_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 18);   }
void GPIO2_PIN19_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 19);   }
void GPIO2_PIN20_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 20);   }
void GPIO2_PIN21_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 21);   }
void GPIO2_PIN22_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 22);   }
void GPIO2_PIN23_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 23);   }
void GPIO2_PIN24_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 24);   }
void GPIO2_PIN25_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 25);   }
void GPIO2_PIN26_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 26);   }
void GPIO2_PIN27_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 27);   }
void GPIO2_PIN28_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 28);   }
void GPIO2_PIN29_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 29);   }
void GPIO2_PIN30_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 30);   }
void GPIO2_PIN31_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO2, 31);   }

int32_t ARM_GPIO2_Initialize (uint8_t pin_no, ARM_GPIO_SignalEvent_t cb_event)
{
    return GPIO_Initialize (&GPIO2, cb_event, pin_no);
}

int32_t ARM_GPIO2_PowerControl (uint8_t pin_no, ARM_POWER_STATE state)
{
    return GPIO_PowerControl (&GPIO2, pin_no, state);
}

int32_t ARM_GPIO2_SetDirection (uint8_t pin_no, uint32_t dir)
{
    return GPIO_SetDirection (&GPIO2, pin_no, dir);
}

int32_t ARM_GPIO2_GetDirection (uint8_t pin_no, uint32_t *dir)
{
    return GPIO_GetDirection (&GPIO2, pin_no, dir);
}

int32_t ARM_GPIO2_SetValue (uint8_t pin_no, uint32_t value)
{
    return GPIO_SetValue (&GPIO2, pin_no, value);
}

int32_t ARM_GPIO2_GetValue (uint8_t pin_no, uint32_t *value)
{
    return GPIO_GetValue (&GPIO2, pin_no, value);
}

int32_t ARM_GPIO2_Control (uint8_t pin_no, GPIO_OPERATION control_code, uint32_t *arg)
{
    return GPIO_Control (&GPIO2, pin_no, control_code, arg);
}

int32_t ARM_GPIO2_Uninitialize (uint8_t pin_no)
{
    return GPIO_Uninitialize (&GPIO2, pin_no);
}

extern ARM_DRIVER_GPIO Driver_GPIO2;
ARM_DRIVER_GPIO Driver_GPIO2 = {
    ARM_GPIO2_Initialize,
    ARM_GPIO2_PowerControl,
    ARM_GPIO2_SetDirection,
    ARM_GPIO2_GetDirection,
    ARM_GPIO2_SetValue,
    ARM_GPIO2_GetValue,
    ARM_GPIO2_Control,
    ARM_GPIO2_Uninitialize
};


/**<GPIO Instance 3>*/

GPIO_resources_t GPIO3 = {
    .reg_base = (uint32_t*) GPIO3_BASE,
    .IRQ_base_num = GPIO3_PIN0_IRQ,
    .IRQ_priority = {
        RTE_GPIO3_PIN0_IRQ_PRIORITY,
        RTE_GPIO3_PIN1_IRQ_PRIORITY,
        RTE_GPIO3_PIN2_IRQ_PRIORITY,
        RTE_GPIO3_PIN3_IRQ_PRIORITY,
        RTE_GPIO3_PIN4_IRQ_PRIORITY,
        RTE_GPIO3_PIN5_IRQ_PRIORITY,
        RTE_GPIO3_PIN6_IRQ_PRIORITY,
        RTE_GPIO3_PIN7_IRQ_PRIORITY,
        RTE_GPIO3_PIN8_IRQ_PRIORITY,
        RTE_GPIO3_PIN9_IRQ_PRIORITY,
        RTE_GPIO3_PIN10_IRQ_PRIORITY,
        RTE_GPIO3_PIN11_IRQ_PRIORITY,
        RTE_GPIO3_PIN12_IRQ_PRIORITY,
        RTE_GPIO3_PIN13_IRQ_PRIORITY,
        RTE_GPIO3_PIN14_IRQ_PRIORITY,
        RTE_GPIO3_PIN15_IRQ_PRIORITY,
        RTE_GPIO3_PIN16_IRQ_PRIORITY,
        RTE_GPIO3_PIN17_IRQ_PRIORITY,
        RTE_GPIO3_PIN18_IRQ_PRIORITY,
        RTE_GPIO3_PIN19_IRQ_PRIORITY,
        RTE_GPIO3_PIN20_IRQ_PRIORITY,
        RTE_GPIO3_PIN21_IRQ_PRIORITY,
        RTE_GPIO3_PIN22_IRQ_PRIORITY,
        RTE_GPIO3_PIN23_IRQ_PRIORITY,
    }
};

void GPIO3_PIN0_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 0);    }
void GPIO3_PIN1_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 1);    }
void GPIO3_PIN2_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 2);    }
void GPIO3_PIN3_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 3);    }
void GPIO3_PIN4_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 4);    }
void GPIO3_PIN5_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 5);    }
void GPIO3_PIN6_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 6);    }
void GPIO3_PIN7_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 7);    }
void GPIO3_PIN8_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 8);    }
void GPIO3_PIN9_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 9);    }
void GPIO3_PIN10_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 10);   }
void GPIO3_PIN11_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 11);   }
void GPIO3_PIN12_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 12);   }
void GPIO3_PIN13_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 13);   }
void GPIO3_PIN14_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 14);   }
void GPIO3_PIN15_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 15);   }
void GPIO3_PIN16_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 16);   }
void GPIO3_PIN17_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 17);   }
void GPIO3_PIN18_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 18);   }
void GPIO3_PIN19_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 19);   }
void GPIO3_PIN20_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 20);   }
void GPIO3_PIN21_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 21);   }
void GPIO3_PIN22_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 22);   }
void GPIO3_PIN23_IRQHandler (void) {   GPIO_ll_IRQ_Handler (&GPIO3, 23);   }

int32_t ARM_GPIO3_Initialize (uint8_t pin_no, ARM_GPIO_SignalEvent_t cb_event)
{
    return GPIO_Initialize (&GPIO3, cb_event, pin_no);
}

int32_t ARM_GPIO3_PowerControl (uint8_t pin_no, ARM_POWER_STATE state)
{
    return GPIO_PowerControl (&GPIO3, pin_no, state);
}

int32_t ARM_GPIO3_SetDirection (uint8_t pin_no, uint32_t dir)
{
    return GPIO_SetDirection (&GPIO3, pin_no, dir);
}

int32_t ARM_GPIO3_GetDirection (uint8_t pin_no, uint32_t *dir)
{
    return GPIO_GetDirection (&GPIO3, pin_no, dir);
}

int32_t ARM_GPIO3_SetValue (uint8_t pin_no, uint32_t value)
{
    return GPIO_SetValue (&GPIO3, pin_no, value);
}

int32_t ARM_GPIO3_GetValue (uint8_t pin_no, uint32_t *value)
{
    return GPIO_GetValue (&GPIO3, pin_no, value);
}

int32_t ARM_GPIO3_Control (uint8_t pin_no, GPIO_OPERATION control_code, uint32_t *arg)
{
    return GPIO_Control (&GPIO3, pin_no, control_code, arg);
}

int32_t ARM_GPIO3_Uninitialize (uint8_t pin_no)
{
    return GPIO_Uninitialize (&GPIO3, pin_no);
}

extern ARM_DRIVER_GPIO Driver_GPIO3;
ARM_DRIVER_GPIO Driver_GPIO3 = {
    ARM_GPIO3_Initialize,
    ARM_GPIO3_PowerControl,
    ARM_GPIO3_SetDirection,
    ARM_GPIO3_GetDirection,
    ARM_GPIO3_SetValue,
    ARM_GPIO3_GetValue,
    ARM_GPIO3_Control,
    ARM_GPIO3_Uninitialize
};

/**<GPIO Instance 3>*/

GPIO_resources_t GPIO4 = {
    .reg_base = (uint32_t*) GPIO4_BASE,
    .IRQ_base_num = GPIO4_PIN0_IRQ,
    .IRQ_priority = {
            RTE_GPIO4_PIN0_IRQ_PRIORITY,
            RTE_GPIO4_PIN1_IRQ_PRIORITY,
            RTE_GPIO4_PIN2_IRQ_PRIORITY,
            RTE_GPIO4_PIN3_IRQ_PRIORITY,
            RTE_GPIO4_PIN4_IRQ_PRIORITY,
            RTE_GPIO4_PIN5_IRQ_PRIORITY,
            RTE_GPIO4_PIN6_IRQ_PRIORITY,
            RTE_GPIO4_PIN7_IRQ_PRIORITY,
    }
};

void GPIO4_PIN0_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO4, 0);    }
void GPIO4_PIN1_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO4, 1);    }
void GPIO4_PIN2_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO4, 2);    }
void GPIO4_PIN3_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO4, 3);    }
void GPIO4_PIN4_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO4, 4);    }
void GPIO4_PIN5_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO4, 5);    }
void GPIO4_PIN6_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO4, 6);    }
void GPIO4_PIN7_IRQHandler  (void) {   GPIO_ll_IRQ_Handler (&GPIO4, 7);    }

int32_t ARM_GPIO4_Initialize (uint8_t pin_no, ARM_GPIO_SignalEvent_t cb_event)
{
    return GPIO_Initialize (&GPIO4, cb_event, pin_no);
}

int32_t ARM_GPIO4_PowerControl (uint8_t pin_no, ARM_POWER_STATE state)
{
    return GPIO_PowerControl (&GPIO4, pin_no, state);
}

int32_t ARM_GPIO4_SetDirection (uint8_t pin_no, uint32_t dir)
{
    if (RTE_SILICON_REV_A)
    {
	dir = dir ? GPIO_PIN_DIRECTION_INPUT : GPIO_PIN_DIRECTION_OUTPUT;
    }
    return GPIO_SetDirection (&GPIO4, pin_no, dir);
}

int32_t ARM_GPIO4_GetDirection (uint8_t pin_no, uint32_t *dir)
{
    int32_t ret =  GPIO_GetDirection (&GPIO4, pin_no, dir);
    if (RTE_SILICON_REV_A)
    {
	*dir = *dir ? GPIO_PIN_DIRECTION_INPUT : GPIO_PIN_DIRECTION_OUTPUT;
    }
    return ret;
}

int32_t ARM_GPIO4_SetValue (uint8_t pin_no, uint32_t value)
{
    return GPIO_SetValue (&GPIO4, pin_no, value);
}

int32_t ARM_GPIO4_GetValue (uint8_t pin_no, uint32_t *value)
{
    return GPIO_GetValue (&GPIO4, pin_no, value);
}

int32_t ARM_GPIO4_Control (uint8_t pin_no, GPIO_OPERATION control_code, uint32_t *arg)
{
    return GPIO_Control (&GPIO4, pin_no, control_code, arg);
}

int32_t ARM_GPIO4_Uninitialize (uint8_t pin_no)
{
    return GPIO_Uninitialize (&GPIO4, pin_no);
}

extern ARM_DRIVER_GPIO Driver_GPIO4;
ARM_DRIVER_GPIO Driver_GPIO4 = {
    ARM_GPIO4_Initialize,
    ARM_GPIO4_PowerControl,
    ARM_GPIO4_SetDirection,
    ARM_GPIO4_GetDirection,
    ARM_GPIO4_SetValue,
    ARM_GPIO4_GetValue,
    ARM_GPIO4_Control,
    ARM_GPIO4_Uninitialize
};
