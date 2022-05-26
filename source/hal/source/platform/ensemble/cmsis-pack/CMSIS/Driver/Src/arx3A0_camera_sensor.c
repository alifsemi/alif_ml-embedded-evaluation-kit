/* Copyright (c) 2022 ALIF SEMICONDUCTOR

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
 * @file     arx3A0_camera_sensor.c
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     1-April-2022
 * @brief    ONsemi ARX3A0 Camera Sensor driver.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/
/* System Includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

/* Proceed only if ARX3A0 Camera Sensor is enabled. */
#if RTE_ARX3A0_CAMERA_SENSOR_ENABLE

/* Project Includes */
#include "arx3A0_camera_sensor.h"
#include "Driver_GPIO.h"
#include "Driver_Camera_Controller.h"

#if (RTE_ARX3A0_CAMERA_RESET_GPIO_PORT == 1)
extern ARM_DRIVER_GPIO Driver_GPIO1;
ARM_DRIVER_GPIO *GPIO_Driver_CAM = &Driver_GPIO1;
#elif (RTE_ARX3A0_CAMERA_RESET_GPIO_PORT == 2)
extern ARM_DRIVER_GPIO Driver_GPIO2;
ARM_DRIVER_GPIO *GPIO_Driver_CAM = &Driver_GPIO2;
#elif (RTE_ARX3A0_CAMERA_RESET_GPIO_PORT == 3)
extern ARM_DRIVER_GPIO Driver_GPIO3;
ARM_DRIVER_GPIO *GPIO_Driver_CAM = &Driver_GPIO3;
#elif (RTE_ARX3A0_CAMERA_RESET_GPIO_PORT == 4)
extern ARM_DRIVER_GPIO Driver_GPIO4;
ARM_DRIVER_GPIO *GPIO_Driver_CAM = &Driver_GPIO4;
#else
#error "GPIO Port for Camera Reset not found"
#endif


/* Wrapper function for Delay
 * Delay for microsecond:
 * Provide delay using PMU(Performance Monitoring Unit).
 */
#define ARX3A0_DELAY_mSEC(msec)       PMU_delay_loop_us(msec)

/* Wrapper function for i2c read
 *  read register value from ARX3A0 Camera Sensor registers
 *   using i2c read API \ref camera_sensor_i2c_read
 *
 *  for ARX3A0 Camera Sensor specific i2c configurations
 *   see \ref ARX3A0_camera_sensor_i2c_cnfg
 */
#define ARX3A0_READ_REG(reg_addr, reg_value, reg_size) \
	camera_sensor_i2c_read(&arx3A0_camera_sensor_i2c_cnfg, \
			reg_addr,  \
			reg_value, \
			reg_size);

/* Wrapper function for i2c write
 *  write register value to ARX3A0 Camera Sensor registers
 *   using i2c write API \ref camera_sensor_i2c_write.
 *
 *  for ARX3A0 Camera Sensor specific i2c configurations
 *   see \ref ARX3A0_camera_sensor_i2c_cnfg
 */
#define ARX3A0_WRITE_REG(reg_addr, reg_value, reg_size) \
	camera_sensor_i2c_write(&arx3A0_camera_sensor_i2c_cnfg, \
			reg_addr,  \
			reg_value, \
			reg_size);

/**
  \fn           int32_t ARX3A0_Bulk_Write_Reg(const ARX3A0_REG arx3A0_reg[],
  uint32_t total_num)
  \brief        write array of registers value to ARX3A0 Camera Sensor registers.
  \param[in]    arx3A0_reg : ARX3A0 Camera Sensor Register Array Structure
  \ref ARX3A0_REG
  \param[in]    total_num   : total number of registers(size of array)
  \return       \ref execution_status
  */
static int32_t ARX3A0_Bulk_Write_Reg(const ARX3A0_REG arx3A0_reg[],
		uint32_t total_num, uint32_t reg_size)
{
	uint32_t i  = 0;
	int32_t ret = 0;

	for(i = 0; i < total_num; i++)
	{
		ret = ARX3A0_WRITE_REG(arx3A0_reg[i].reg_addr, arx3A0_reg[i].reg_value, \
				reg_size);
		if(ret != ARM_DRIVER_OK)
			return ARM_DRIVER_ERROR;
	}

	return ARM_DRIVER_OK;
}

/**
  \fn           void ARX3A0_Sensor_Clk_Src(void)
  \brief        ARX3A0 Camera Sensor external clock source configuration.
  \param[in]    none
  \return       none
  */
static void ARX3A0_Sensor_Clk_Src(void)
{
	volatile uint32_t *reg = NULL;

	reg = (uint32_t *)CFGSLV1_BASE;
	*reg &= ~(ARX3A0_CLK_DIV_Msk);
	*reg |= _VAL2FLD(ARX3A0_CLK_DIV, RTE_ARX3A0_CAMERA_SENSOR_CLK_SCR_DIV);
	SET_BIT(*reg, ARX3A0_CLK_EN_Msk);
}



/**
  \fn           int32_t ARX3A0_Camera_Hard_Reseten(void)
  \brief        Hard Reset ARX3A0 Camera Sensor
  \param[in]    none
  \return       \ref execution_status
  */
static int32_t ARX3A0_Camera_Hard_Reseten(void)
{
	int32_t ret = 0;

	if(GPIO_Driver_CAM == NULL)
		return ARM_DRIVER_ERROR_PARAMETER;

	ret = GPIO_Driver_CAM->Initialize(RTE_ARX3A0_CAMERA_RESET_PIN_NO,NULL);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ret = GPIO_Driver_CAM->PowerControl(RTE_ARX3A0_CAMERA_RESET_PIN_NO,  ARM_POWER_FULL);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ret = GPIO_Driver_CAM->SetDirection(RTE_ARX3A0_CAMERA_RESET_PIN_NO, GPIO_PIN_DIRECTION_OUTPUT);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ret = GPIO_Driver_CAM->SetValue(RTE_ARX3A0_CAMERA_RESET_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ARX3A0_DELAY_mSEC(2000);

	ret = GPIO_Driver_CAM->SetValue(RTE_ARX3A0_CAMERA_RESET_PIN_NO, GPIO_PIN_OUTPUT_STATE_LOW);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ARX3A0_DELAY_mSEC(2000);

	ret = GPIO_Driver_CAM->SetValue(RTE_ARX3A0_CAMERA_RESET_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ARX3A0_DELAY_mSEC(100000);

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t ARX3A0_Camera_Soft_Reseten(void)
  \brief        Software Reset ARX3A0 Camera Sensor
  \param[in]    none
  \return       \ref execution_status
  */
static int32_t ARX3A0_Camera_Soft_Reseten(void)
{
	int32_t ret = 0;

	ret = ARX3A0_WRITE_REG(ARX3A0_SOFTWARE_RESET_REGISTER, 0x01, 1);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	/* @Observation: more delay is required for Camera Sensor
	 *               to setup after Soft Reset.
	 */
	ARX3A0_DELAY_mSEC(1000);

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t ARX3A0_Camera_Cfg(ARM_CAMERA_RESOLUTION cam_resolution)
  \brief        Initialize ARX3A0 Camera Sensor.
  this function will
  - configure Camera Sensor resolution registers as per input parameter.
  (currently supports only 560x560(WxH) Camera resolution)
  - configure Camera Sensor slew rate.
  \param[in]    cam_resolution    : Camera Sensor Resolution
  \ref ARX3A0_Camera_Resolution

  \return       \ref execution_status
  */
static int32_t ARX3A0_Camera_Cfg(ARM_CAMERA_RESOLUTION cam_resolution)
{
	uint32_t total_num     = 0;
	uint16_t output_format = 0;
	int32_t  ret = 0;

	/* Configure Camera Sensor Resolution */
	switch(cam_resolution)
	{
		/* Camera Sensor Resolution: 560x560(WxH) */
		case CAMERA_RESOLUTION_560x560:
			total_num = (sizeof(arx3a0_560_regs) / sizeof(ARX3A0_REG));
			ret = ARX3A0_Bulk_Write_Reg(arx3a0_560_regs, total_num, 2);
			if(ret != ARM_DRIVER_OK)
				return ARM_DRIVER_ERROR;
			break;

		default:
			return ARM_DRIVER_ERROR_PARAMETER;
	}

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t ARX3A0_Init(ARM_CAMERA_RESOLUTION cam_resolution)
  \brief        Initialize ARX3A0 Camera Sensor
  this function will
  - initialize i2c using i3c instance
  - software reset ARX3A0 Camera Sensor
  - read ARX3A0 chip-id, proceed only it is correct.
  \param[in]    cam_resolution  : Camera Resolution \ref ARM_CAMERA_RESOLUTION
  \return       \ref execution_status
  */
int32_t ARX3A0_Init(ARM_CAMERA_RESOLUTION cam_resolution)
{
	int32_t  ret = 0;
	uint32_t rcv_data = 0;

	/*camera sensor clock source config*/
	ARX3A0_Sensor_Clk_Src();

	/*camera sensor resten*/
	ARX3A0_Camera_Hard_Reseten();

	/* Initialize i2c using i3c driver instance depending on
	 *  ARX3A0 Camera Sensor specific i2c configurations
	 *   \ref arx3A0_camera_sensor_i2c_cnfg
	 */

	ret = camera_sensor_i2c_init(&arx3A0_camera_sensor_i2c_cnfg);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	/* Soft Reset ARX3A0 Camera Sensor */
	ret = ARX3A0_Camera_Soft_Reseten();
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	/* Read ARX3A0 Camera Sensor CHIP ID */
	ret = ARX3A0_READ_REG(ARX3A0_CHIP_ID_REGISTER, &rcv_data, 2);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	/* Proceed only if CHIP ID is correct. */
	if(rcv_data != ARX3A0_CHIP_ID_REGISTER_VALUE)
		return ARM_DRIVER_ERROR;

	/*Putting sensor in standby mode*/
	ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x00, 1);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ret = ARX3A0_READ_REG(ARX3A0_MIPI_CONFIG_REGISTER, &rcv_data, 2);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ret = ARX3A0_WRITE_REG(ARX3A0_MIPI_CONFIG_REGISTER, rcv_data | (1U << 7), 2);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	/*start streaming*/
	ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x01, 1);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ARX3A0_DELAY_mSEC(50000);

	/*stop streaming*/
	ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x00, 1);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t ARX3A0_Start(void)
  \brief        Start ARX3A0 Camera Sensor Streaming.
  \param[in]    none
  \return       \ref execution_status
  */
int32_t ARX3A0_Start(void)
{
	int32_t ret = 0;

	/* Start streaming */
	ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x01, 1);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	ARX3A0_DELAY_mSEC(1000);

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t ARX3A0_Stop(void)
  \brief        Stop ARX3A0 Camera Sensor Streaming.
  \param[in]    none
  \return       \ref execution_status
  */
int32_t ARX3A0_Stop(void)
{
	int32_t ret = 0;

	/* Suspend any stream */
	ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x00, 1);
	if(ret != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR;

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t ARX3A0_Control(uint32_t control, uint32_t arg)
  \brief        Control ARX3A0 Camera Sensor.
  \param[in]    control  : Operation
  \param[in]    arg      : Argument of operation
  \return       \ref execution_status
  */
int32_t ARX3A0_Control(uint32_t control, uint32_t arg)
{
	int32_t ret = 0;

	switch (control)
	{
		case CAMERA_SENSOR_CONFIG:
			ret = ARX3A0_Camera_Cfg(arg);
			if(ret != ARM_DRIVER_OK)
				return ARM_DRIVER_ERROR;
			break;
		default:
			return ARM_DRIVER_ERROR_PARAMETER;
	}
	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t ARX3A0_Uninit(void)
  \brief        Un-initialize ARX3A0 Camera Sensor.
  \param[in]    none
  \return       \ref execution_status
  */
int32_t ARX3A0_Uninit(void)
{
	return ARM_DRIVER_OK;
}

#endif /* RTE_ARX3A0_CAMERA_SENSOR_ENABLE */
