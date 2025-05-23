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
#include "RTE_Components.h"
#include "RTE_Device.h"
#include "board.h"
#include "delay.h"
#include "platform_drivers.h"
#include "sys_utils.h"

extern ARM_DRIVER_CDC200 Driver_CDC200;

static void (*tear_handler)(void);

static void cdc_event_handler(uint32_t int_event)
{
    if (int_event == ARM_CDC_SCANLINE0_EVENT) {
        tear_handler();
    }
}

int LCD_current_v_pos(void)
{
    return Driver_CDC200.GetVerticalPosition();
}

void LCD_enable_tear_interrupt(void (*handler)(void), uint8_t prio)
{
    tear_handler = handler;
    Driver_CDC200.Control(CDC200_SCANLINE0_EVENT, ENABLE);
    NVIC_SetPriority(CDC_SCANLINE0_IRQ_IRQn, prio);
    NVIC_EnableIRQ(CDC_SCANLINE0_IRQ_IRQn);
}

int Display_initialization(uint8_t *buffer)
{
    int32_t ret = (int32_t)enable_peripheral_clocks();
    if(ret != ARM_DRIVER_OK)
    {
        printf("Display_initialization enable_peripheral_clocks failed: %" PRIi32 "\n", ret);
        return 1;
    }

    enable_mipi_power();

	////////////////////////////////////////////////////////////////////////////
	// MIPI DPI Controller Setup (CDC200)
	////////////////////////////////////////////////////////////////////////////
	ret = Driver_CDC200.Initialize(cdc_event_handler);
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

    while(test) {
        BOARD_LED1_Control(BOARD_LED_STATE_HIGH);
        sleep_or_wait_msec(300);
        BOARD_LED1_Control(BOARD_LED_STATE_LOW);
        sleep_or_wait_msec(300);
        test--;
    }

    static volatile int dinit = 0;

    dinit = Display_initialization(buffer);
    if (dinit != 0) {
        while(1) {
            BOARD_LED2_Control(BOARD_LED_STATE_LOW);
            sleep_or_wait_msec(300);
            BOARD_LED2_Control(BOARD_LED_STATE_HIGH);
            sleep_or_wait_msec(300);
        }
    }
    BOARD_LED2_Control(BOARD_LED_STATE_HIGH);

    return 0;
}
