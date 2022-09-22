/*
 * Driver_Camera.h
 *
 */

#ifndef DRIVER_CPI_H_
#define DRIVER_CPI_H_

#include <stdint.h>

#define CAM_CTRL		0x00
#define CAM_INTR		0x04
#define CAM_INTR_ENA	0x08
#define CAM_CFG			0x10
#define CAM_FIFO_CTRL	0x14
#define CAM_AXI_ERR		0x18
#define CAM_VIDEO_HCFG	0x20
#define CAM_VIDEO_VCFG	0x24
#define CAM_VIDEO_FCFG	0x28
#define CAM_CSI_CMCFG	0x2C
#define CAM_FRAME_ADDR	0x30

#define CAMERA_MODE_VIDEO 		0x0001
#define CAMERA_MODE_SNAPSHOT 	0x0011


int32_t camera_init(uint8_t *buffer);
void camera_start(uint32_t mode);
int32_t camera_vsync(uint32_t timeout_ms);
int32_t camera_wait(uint32_t timeout_ms);


#endif /* DRIVER_CPI_H_ */
