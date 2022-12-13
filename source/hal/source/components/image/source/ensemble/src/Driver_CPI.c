/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/* System Includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include "global_map.h"

/* Project Includes */
#include "Driver_CPI.h"
#include "arx3A0_camera_sensor.h"
#include "arx3A0_camera_sensor_conf.h"
#include "Driver_PINMUX_AND_PINPAD.h"
#include "Driver_GPIO.h"
#include "mipi_csi2_host.h"

#include "Driver_Common.h"
#include "delay.h"
#include "base_def.h"
#include "image_processing.h"

int32_t camera_init(uint8_t *buffer)
{
	//////////////////////////////////////////////////////////////////////////////
	//Camera initialization
	//////////////////////////////////////////////////////////////////////////////
	int retry = 5;
	do {
		arx3A0_Init(ARX3A0_CAMERA_RESOLUTION_560x560);

		if (rx_phyconfig() == 0)
			break;
		retry--;
	} while (retry > 0);

	if (retry <= 0) {
		return -1;
	}
	////////////////////////////////////////////////////////////////
	//camera setup
	////////////////////////////////////////////////////////////////

	arx3A0_camera_init(ARX3A0_CAMERA_RESOLUTION_560x560);
	arx3A0_Start();

	////////////////////////////////////////////////////////////////
	//3.3.1Start up DWC_mipi_csi2_host
	////////////////////////////////////////////////////////////////
	csi2_IPI_initialization();
	csi2_IPI_enable();

	// common settings for all camera
	HW_REG_WORD(CAMERA0_BASE, CAM_CSI_CMCFG) = 3 << 4;             	// MIPI_CSI 16-bit RAW data
	HW_REG_WORD(CAMERA0_BASE, CAM_INTR_ENA)  = 0x00000100;         	// enable irq: vsync
	HW_REG_WORD(CAMERA0_BASE, CAM_FIFO_CTRL) = 0x00001707;         	// camera fifo water mark

	// configure for common camera sensor

#if (PIXEL_BYTES == 2)
	HW_REG_WORD(CAMERA0_BASE, CAM_CFG) = (1<<28) | (1<<4);   		// select 16-bit data mode, select CSI
#else
	HW_REG_WORD(CAMERA0_BASE, CAM_CFG) = (1<<4);
#endif
	HW_REG_WORD(CAMERA0_BASE, CAM_VIDEO_FCFG) = (CIMAGE_X-1)<<16 | (1<<15) | (CIMAGE_Y-1);      // row, data
	HW_REG_WORD(CAMERA0_BASE, CAM_VIDEO_HCFG) = 0<<16 | (15-2);     // hfp, hbp

	// image buffer address, crossing 4KB boundary
	HW_REG_WORD(CAMERA0_BASE, CAM_FRAME_ADDR) = (uint32_t)buffer;
	sleep_or_wait_msec(1);
	// camera start sequence
	HW_REG_WORD(CAMERA0_BASE, CAM_CTRL) = 0x00000100;         // camera soft reset
	HW_REG_WORD(CAMERA0_BASE, CAM_CTRL) = 0x00000000;         // clear camera soft reset

	HW_REG_WORD(CAMERA0_BASE, CAM_INTR_ENA) = 1;              // enable STOP interrupt
	NVIC_EnableIRQ (CAMERA0_IRQ);

	return 0;
}

void camera_start(uint32_t mode)
{
	if (mode == CAMERA_MODE_SNAPSHOT) {
		HW_REG_WORD(CAMERA0_BASE, CAM_CTRL) = 0x00000100;         // camera soft reset
		sleep_or_wait_usec(50);
		HW_REG_WORD(CAMERA0_BASE, CAM_CTRL) = 0x00000000;         // clear camera soft reset
	}
	HW_REG_WORD(CAMERA0_BASE, CAM_CTRL) = mode;
}

// Don't reeally need to do anything apart from clear the interrupts;
// we're just generating the IRQ to wake from WFE.
void CAMERA0_IRQHandler(void)
{
	// Read which interrupts are pending
	uint32_t interrupts = HW_REG_WORD(CAMERA0_BASE, CAM_INTR);

	// Write back to acknowledge clear the ones we saw
	HW_REG_WORD(CAMERA0_BASE, CAM_INTR) = interrupts;

	// Read back to ensure that write reached the camera so we do
	// not return before it deasserts the IRQ.
	HW_REG_WORD(CAMERA0_BASE, CAM_INTR);
}

int32_t camera_vsync(uint32_t timeout_ms)
{
	int camera_irq;

	do {
		camera_irq = HW_REG_WORD(CAMERA0_BASE, CAM_INTR);
		camera_irq &= 1 << 8;                              	// mask all except vsync irq bit
	} while (camera_irq == 0);

	HW_REG_WORD(CAMERA0_BASE,CAM_INTR) = 1 << 8;			// Clear IRQ flag

	return ARM_DRIVER_OK;
}

int32_t camera_wait(uint32_t timeout_ms)
{
	while(HW_REG_WORD(CAMERA0_BASE, CAM_CTRL) & 0x2) {
		__WFE();
	};
	return ARM_DRIVER_OK;
}
