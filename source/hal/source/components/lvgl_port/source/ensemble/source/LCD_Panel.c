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
#include "board.h"
#include "delay.h"

extern ARM_DRIVER_CDC200 Driver_CDC200;

static void cdc_event_handler(uint32_t int_event)
{
	(void)int_event;
}


#define SCANLINE0_IRQ 1

static void (*tear_handler)(void);

void CDC200_SCANLINE0_IRQHandler(void)
{
    CDC200->global_reg.irq_clear = SCANLINE0_IRQ;
    (void)CDC200->global_reg.irq_clear;
    __DSB();
    tear_handler();
}

int LCD_current_v_pos(void)
{
    return (int) (CDC200->global_reg.position_status & 0xffff) - RTE_PANEL_VSYNC_LINE - RTE_PANEL_VBP_LINE;
}

void LCD_enable_tear_interrupt(void (*handler)(void), uint8_t prio)
{
    tear_handler = handler;
    CDC200->global_reg.line_irq_position_control = RTE_PANEL_VSYNC_LINE + RTE_PANEL_VBP_LINE + RTE_PANEL_VACTIVE_LINE;
    CDC200->global_reg.irq_enable |= SCANLINE0_IRQ;
#if RTE_SILICON_REV_A
    NVIC_SetPriority(CDC200_SCANLINE0_IRQ, prio);
    NVIC_EnableIRQ(CDC200_SCANLINE0_IRQ);
#else
    NVIC_SetPriority(CDC_SCANLINE0_IRQ_IRQn, prio);
    NVIC_EnableIRQ(CDC_SCANLINE0_IRQ_IRQn);
#endif
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
