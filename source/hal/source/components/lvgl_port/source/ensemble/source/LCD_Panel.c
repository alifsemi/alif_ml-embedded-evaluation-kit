/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include <stdio.h>
#include <inttypes.h>
#include "Driver_CDC200.h"
#include "CDC200_dev.h"
#include "RTE_Device.h"
#include "Driver_PINMUX_AND_PINPAD.h"
#include "Driver_GPIO.h"
#include "delay.h"

extern ARM_DRIVER_CDC200 Driver_CDC200;
extern ARM_DRIVER_GPIO Driver_GPIO1;

static void SetupLEDs()
{
    // Green LED
    Driver_GPIO1.Initialize(PIN_NUMBER_15,NULL);
    Driver_GPIO1.PowerControl(PIN_NUMBER_15,  ARM_POWER_FULL);
    Driver_GPIO1.SetValue(PIN_NUMBER_15, GPIO_PIN_OUTPUT_STATE_LOW);
    Driver_GPIO1.SetDirection(PIN_NUMBER_15, GPIO_PIN_DIRECTION_OUTPUT);
    PINMUX_Config (PORT_NUMBER_1, PIN_NUMBER_15, PINMUX_ALTERNATE_FUNCTION_0);

    // Red LED
    Driver_GPIO1.Initialize(PIN_NUMBER_14,NULL);
    Driver_GPIO1.PowerControl(PIN_NUMBER_14,  ARM_POWER_FULL);
    Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_LOW);
    Driver_GPIO1.SetDirection(PIN_NUMBER_14, GPIO_PIN_DIRECTION_OUTPUT);
    PINMUX_Config (PORT_NUMBER_1, PIN_NUMBER_14, PINMUX_ALTERNATE_FUNCTION_0);
}

static void cdc_event_handler(uint32_t int_event)
{
	(void)int_event;
}


#define SCANLINE0_IRQ 1

static void (*tear_handler)(void);

void CDC200_SCANLINE0_IRQHandler(void)
{
    CDC200_Type *regbase = (CDC200_Type *) CDC200_BASE;

    regbase->global_reg.irq_clear = SCANLINE0_IRQ;
    (void)regbase->global_reg.irq_clear;
    __DSB();
    tear_handler();
}

int LCD_current_v_pos(void)
{
    CDC200_Type *regbase = (CDC200_Type *) CDC200_BASE;
    return (int) (regbase->global_reg.position_status & 0xffff) - RTE_PANEL_VSYNC_LINE - RTE_PANEL_VBP_LINE;
}

void LCD_enable_tear_interrupt(void (*handler)(void), uint8_t prio)
{
    CDC200_Type *regbase = (CDC200_Type *) CDC200_BASE;
    tear_handler = handler;
    regbase->global_reg.line_irq_position_control = RTE_PANEL_VSYNC_LINE + RTE_PANEL_VBP_LINE + RTE_PANEL_VACTIVE_LINE;
    regbase->global_reg.irq_enable |= SCANLINE0_IRQ;
    NVIC_SetPriority(CDC200_SCANLINE0_IRQ, prio);
    NVIC_EnableIRQ(CDC200_SCANLINE0_IRQ);
}

int Display_initialization(uint8_t *buffer)
{
	////////////////////////////////////////////////////////////////////////////
	// MIPI DPI Controller Setup (CDC200)
	////////////////////////////////////////////////////////////////////////////
	int32_t ret = Driver_CDC200.Initialize(cdc_event_handler);
	if(ret != ARM_DRIVER_OK)
	{
		printf("Driver_CDC200.Initialize: %" PRIi32 "\n", ret);
		return 1;
	}

	ret = Driver_CDC200.PowerControl(ARM_POWER_FULL);
	if(ret != ARM_DRIVER_OK)
	{
		printf("Driver_CDC200.PowerControl: %" PRIi32 "\n", ret);
		return 1;
	}

	ret = Driver_CDC200.Control(CDC200_CONFIGURE_DISPLAY, (uint32_t) buffer);
	if(ret != ARM_DRIVER_OK)
	{
		printf("Driver_CDC200.Control: %" PRIi32 "\n", ret);
		return 1;
	}

	ret = Driver_CDC200.Start();
	if(ret != ARM_DRIVER_OK)
	{
		printf("Driver_CDC200.Start: %" PRIi32 "\n", ret);
		return 1;
	}

	return 0;
}

int LCD_Panel_init(uint8_t *buffer)
{
    volatile int test = 3;
    SetupLEDs();

    while(test) {
        Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_HIGH);
        sleep_or_wait_msec(300);
        Driver_GPIO1.SetValue(PIN_NUMBER_14, GPIO_PIN_OUTPUT_STATE_LOW);
        sleep_or_wait_msec(300);
        test--;
    }

    static volatile int dinit = 0;

    dinit = Display_initialization(buffer);

    if (dinit != 0) {
        while(1) {
            Driver_GPIO1.SetValue(PIN_NUMBER_15, GPIO_PIN_OUTPUT_STATE_LOW);
            sleep_or_wait_msec(300);
            Driver_GPIO1.SetValue(PIN_NUMBER_15, GPIO_PIN_OUTPUT_STATE_HIGH);
            sleep_or_wait_msec(300);
        }
    }
    Driver_GPIO1.SetValue(PIN_NUMBER_15, GPIO_PIN_OUTPUT_STATE_HIGH);

    return 0;
}
