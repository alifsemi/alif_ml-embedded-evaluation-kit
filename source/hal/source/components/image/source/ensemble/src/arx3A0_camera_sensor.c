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

/* Project Includes */
#include "arx3A0_camera_sensor.h"
#include "arx3A0_camera_sensor_conf.h"
#include "Driver_PINMUX_AND_PINPAD.h"
#include "Driver_GPIO.h"

#include "Driver_Common.h"
#include "delay.h"
#include "base_def.h"

extern ARM_DRIVER_GPIO Driver_GPIO4;

CAMERA_SENSOR_SLAVE_I2C_CONFIG arx3A0_camera_sensor_i2c_cnfg =
{
  .I3Cx_instance                  = RTE_ARX3A0_CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE,
  .speed_mode                     = CAMERA_SENSOR_I2C_SPEED_SS_100_KBPS,
  .cam_sensor_slave_addr          = ARX3A0_CAMERA_SENSOR_SLAVE_ADDR,
  .cam_sensor_slave_reg_addr_type = CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT,
};

/* Wrapper function for Delay
 * Delay for millisecond:
 *  Provide delay in terms of sleep or wait for millisecond
 *  depending on RTOS availability.
 */
#define ARX3A0_DELAY_mSEC(msec)       sleep_or_wait_msec(msec)

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
  \fn           int32_t arx3A0_PIN_configuration(void)
  \brief        Pin configuration ARX3A0 Camera Sensor
  \param[in]    none
  \return       \ref execution_status
*/

static int32_t arx3A0_PIN_configuration(void)
{
	// Camera Reset -> pin P4_5
	Driver_GPIO4.Initialize(PIN_NUMBER_5,NULL);
	Driver_GPIO4.PowerControl(PIN_NUMBER_5,  ARM_POWER_FULL);
	Driver_GPIO4.SetDirection(PIN_NUMBER_5, GPIO_PIN_DIRECTION_OUTPUT);

	// I3C_SDA_B  -> pin P3_8
	// I3C_SCL_B  -> pin P3_9

	/* Configure GPIO Pin : P3_8 as I3C_SDA_B */
	PINMUX_Config (PORT_NUMBER_3, PIN_NUMBER_8, PINMUX_ALTERNATE_FUNCTION_3);
	PINPAD_Config (PORT_NUMBER_3, PIN_NUMBER_8, (0x09 | PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_04_MILI_AMPS)); //SDA

	/* Configure GPIO Pin : P3_9 as I3C_SCL_B */
	PINMUX_Config (PORT_NUMBER_3, PIN_NUMBER_9, PINMUX_ALTERNATE_FUNCTION_4);
	PINPAD_Config (PORT_NUMBER_3, PIN_NUMBER_9, (0x09 | PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_04_MILI_AMPS)); //SCL

	/* Configure P2_7 as pixel clock output */
	PINMUX_Config(PORT_NUMBER_2, PIN_NUMBER_7, PINMUX_ALTERNATE_FUNCTION_6);

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t arx3A0_bulk_write_reg(const ARX3A0_REG arx3A0_reg[],
                                                        uint32_t total_num)
  \brief        write array of registers value to ARX3A0 Camera Sensor registers.
  \param[in]    arx3A0_reg : ARX3A0 Camera Sensor Register Array Structure
                              \ref ARX3A0_REG
  \param[in]    total_num   : total number of registers(size of array)
  \return       \ref execution_status
*/
static int32_t arx3A0_bulk_write_reg(const struct regval_list_16 arx3A0_reg[],
                                               uint32_t total_num, uint32_t reg_size)
{
  uint32_t i  = 0;
  int32_t ret = 0;

  for(i = 0; i < total_num; i++)
  {
    ret = ARX3A0_WRITE_REG(arx3A0_reg[i].address, arx3A0_reg[i].val, \
                            reg_size);
    if(ret != ARM_DRIVER_OK)
      return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t arx3A0_Camera_Hard_reseten(void)
  \brief        Hard Reset ARX3A0 Camera Sensor
  \param[in]    none
  \return       \ref execution_status
*/
static int32_t arx3A0_Camera_Hard_reseten(void)
{

	Driver_GPIO4.SetValue(PIN_NUMBER_5, GPIO_PIN_OUTPUT_STATE_HIGH);
	ARX3A0_DELAY_mSEC(2);
	Driver_GPIO4.SetValue(PIN_NUMBER_5, GPIO_PIN_OUTPUT_STATE_LOW);
	ARX3A0_DELAY_mSEC(2);
	Driver_GPIO4.SetValue(PIN_NUMBER_5, GPIO_PIN_OUTPUT_STATE_HIGH);
	ARX3A0_DELAY_mSEC(50);

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t arx3A0_soft_reset(void)
  \brief        Software Reset ARX3A0 Camera Sensor
  \param[in]    none
  \return       \ref execution_status
*/
static int32_t arx3A0_soft_reset(void)
{
  int32_t ret = 0;

  ret = ARX3A0_WRITE_REG(ARX3A0_SOFTWARE_RESET_REGISTER, 0x01, 1);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* @Observation: more delay is required for Camera Sensor
   *               to setup after Soft Reset.
   */
  ARX3A0_DELAY_mSEC(100);

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t arx3A0_camera_init(ARM_CAMERA_RESOLUTION cam_resolution,
                                                          uint8_t cam_output_format)
  \brief        Initialize ARX3A0 Camera Sensor.
                 this function will
                  - configure Camera Sensor resolution registers as per input parameter.
                     (currently supports only 560x560(WxH) Camera resolution)
                  - configure Camera Sensor output format registers as per input parameter
                     \ref ARX3A0_USER_SELECT_CAMERA_OUTPUT_FORMAT.
                     (currently supports only RAW Bayer10 Format)
                  - configure Camera Sensor slew rate.
  \param[in]    cam_resolution    : Camera Sensor Resolution
                                     \ref ARX3A0_Camera_Resolution

  \return       \ref execution_status
*/
int32_t arx3A0_camera_init(ARX3A0_Camera_Resolution cam_resolution)
{
  uint32_t total_num     = 0;
  uint16_t output_format = 0;
  int32_t  ret = 0;

  /* Configure Camera Sensor Resolution */
  switch(cam_resolution)
  {
  /* Camera Sensor Resolution: VGA 640x480(WxH) */
  case ARX3A0_CAMERA_RESOLUTION_560x560:
    total_num = (sizeof(arx3a0_560_regs) / sizeof(struct regval_list_16));
    ret = arx3A0_bulk_write_reg(arx3a0_560_regs, total_num, 2);
    if(ret != ARM_DRIVER_OK)
      return ARM_DRIVER_ERROR;
    break;

  default:
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t arx3A0_Init(ARM_CAMERA_RESOLUTION cam_resolution)
  \brief        Initialize ARX3A0 Camera Sensor
                 this function will
                  - initialize i2c using i3c instance
                  - software reset ARX3A0 Camera Sensor
                  - read ARX3A0 chip-id, proceed only it is correct.
                  - initialize ARX3A0 Camera Sensor as per input parameter
                    - Camera Resolution
                       (currently supports only 560x560(WxH) Camera resolution)
                    - Camera Output Format
                       (currently supports only RAW Bayer10 format)
                    - Issue Change-Config Command to re-configure
                       all the ARX3A0 Camera Sensor sub-system and registers.
                       this command must be issued after any change in
                       sensor registers to take effect, for detail refer data-sheet.
  \param[in]    cam_resolution  : Camera Resolution \ref ARM_CAMERA_RESOLUTION
  \return       \ref execution_status
*/
int32_t arx3A0_Init(ARX3A0_Camera_Resolution cam_resolution)
{
  int32_t  ret = 0;
  uint32_t rcv_data = 0;

  /*
   * The camera pixel clock is wider to P2_6, but this signal is not propagating from the base board
   * P2_7 must be wired on the base board to P2_6 as a workaround!
   *
   * EXTCLK - 20Mhz camera xvclk - P2_7, select 6
   */
  HW_REG_WORD(0x4903F000,0x00) =  0x00140001;//set camera pix clock//sub system 4

  /*pin configuration*/
  arx3A0_PIN_configuration();

  /*camera sensor resten*/
  arx3A0_Camera_Hard_reseten();

  /* Initialize i2c using i3c driver instance depending on
   *  ARX3A0 Camera Sensor specific i2c configurations
   *   \ref arx3A0_camera_sensor_i2c_cnfg
   */
  ret = camera_sensor_i2c_init(&arx3A0_camera_sensor_i2c_cnfg);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Soft Reset ARX3A0 Camera Sensor */
  ret = arx3A0_soft_reset();
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Read ARX3A0 Camera Sensor CHIP ID */
  ret = ARX3A0_READ_REG(ARX3A0_CHIP_ID_REGISTER, &rcv_data, 2);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Proceed only if CHIP ID is correct. */
  if(rcv_data != ARX3A0_CHIP_ID_REGISTER_VALUE)
    return ARM_DRIVER_ERROR;

  /* @NOTE: By-default after Soft-Reset Camera Sensor will be in streaming state,
   *        As per Hardware Jumper(P2 jumper) settings,
   *        if required then Stop Streaming using \ref arx3A0_stream_stop.
   *
   *        Suspend any stream
   *        ret = arx3A0_stream_stop();
   *        if(ret != ARM_DRIVER_OK)
   *          return ARM_DRIVER_ERROR;
   *        ARX3A0_DELAY_mSEC(10);
   */

  ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x00, 1);
  if(ret != ARM_DRIVER_OK)
	  return ARM_DRIVER_ERROR;

  ret = ARX3A0_READ_REG(ARX3A0_MIPI_CONFIG_REGISTER, &rcv_data, 2);
  if(ret != ARM_DRIVER_OK)
	  return ARM_DRIVER_ERROR;

  ret = ARX3A0_WRITE_REG(ARX3A0_MIPI_CONFIG_REGISTER, rcv_data | (1U << 7), 2);
  if(ret != ARM_DRIVER_OK)
	  return ARM_DRIVER_ERROR;

  ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x01, 1);
  if(ret != ARM_DRIVER_OK)
	  return ARM_DRIVER_ERROR;

  ARX3A0_DELAY_mSEC(200);

  ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x00, 1);
  if(ret != ARM_DRIVER_OK)
	  return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}



/**
  \fn           int32_t arx3A0_Start(void)
  \brief        Start ARX3A0 Camera Sensor Streaming.
  \param[in]    none
  \return       \ref execution_status
*/
int32_t arx3A0_Start(void)
{
  int32_t ret = 0;

  /* Start streaming */
  ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x01, 1);
  if(ret != ARM_DRIVER_OK)
	  return ARM_DRIVER_ERROR;

  /* @Observation: Proper Delay is required for
   *               Camera Sensor Lens to come-out from Shutter and gets steady,
   *               otherwise captured image will not be proper.
   *               adjust delay if captured image is less bright/dull.
   */
  ARX3A0_DELAY_mSEC(200);

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t arx3A0_Stop(void)
  \brief        Stop ARX3A0 Camera Sensor Streaming.
  \param[in]    none
  \return       \ref execution_status
*/
int32_t arx3A0_Stop(void)
{
  int32_t ret = 0;

  /* Suspend any stream */
  ret = ARX3A0_WRITE_REG(ARX3A0_MODE_SELECT_REGISTER, 0x00, 1);
  if(ret != ARM_DRIVER_OK)
	  return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}
