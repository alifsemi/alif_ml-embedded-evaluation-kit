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
 * @file     MT9M114_Camera_Sensor.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     29-Sep-2021
 * @brief    ONsemi MT9M114 Camera Sensor driver.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* System Includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header


/* Proceed only if MT9M114 Camera Sensor is enabled. */
#if RTE_MT9M114_CAMERA_SENSOR_ENABLE

/* Project Includes */
#include "MT9M114_Camera_Sensor.h"
#include "Driver_Common.h"


/* Wrapper function for Delay
 * Delay for millisecond:
 *  Provide delay using PMU(Performance Monitoring Unit).
 */
#define MT9M114_DELAY_mSEC(msec)       PMU_delay_loop_us(msec * 1000)

/* Wrapper function for i2c read
 *  read register value from MT9M114 Camera Sensor registers
 *   using i2c read API \ref camera_sensor_i2c_read
 *
 *  for MT9M114 Camera Sensor specific i2c configurations
 *   see \ref mt9m114_camera_sensor_i2c_cnfg
 */
#define MT9M114_READ_REG(reg_addr, reg_value, reg_size) \
        camera_sensor_i2c_read(&mt9m114_camera_sensor_i2c_cnfg, \
                                reg_addr,  \
                                reg_value, \
                                reg_size);

/* Wrapper function for i2c write
 *  write register value to MT9M114 Camera Sensor registers
 *   using i2c write API \ref camera_sensor_i2c_write.
 *
 *  for MT9M114 Camera Sensor specific i2c configurations
 *   see \ref mt9m114_camera_sensor_i2c_cnfg
 */
#define MT9M114_WRITE_REG(reg_addr, reg_value, reg_size) \
        camera_sensor_i2c_write(&mt9m114_camera_sensor_i2c_cnfg, \
                                 reg_addr,  \
                                 reg_value, \
                                 reg_size);

/**
  \fn           int32_t mt9m114_bulk_write_reg(const MT9M114_REG mt9m114_reg[],
                                                        uint32_t total_num)
  \brief        write array of registers value to MT9M114 Camera Sensor registers.
  \param[in]    mt9m114_reg : MT9M114 Camera Sensor Register Array Structure
                              \ref MT9M114_REG
  \param[in]    total_num   : total number of registers(size of array)
  \return       \ref execution_status
*/
static int32_t mt9m114_bulk_write_reg(const MT9M114_REG mt9m114_reg[],
                                               uint32_t total_num)
{
  uint32_t i  = 0;
  int32_t ret = 0;

  for(i = 0; i < total_num; i++)
  {
    ret = MT9M114_WRITE_REG(mt9m114_reg[i].reg_addr, mt9m114_reg[i].reg_value, \
                            mt9m114_reg[i].reg_size);
    if(ret != ARM_DRIVER_OK)
      return ARM_DRIVER_ERROR;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t mt9m114_soft_reset(void)
  \brief        Software Reset MT9M114 Camera Sensor
  \param[in]    none
  \return       \ref execution_status
*/
static int32_t mt9m114_soft_reset(void)
{
  int32_t ret = 0;

  ret = MT9M114_WRITE_REG(MT9M114_SYSCTL_REGISTER_RESET_AND_MISC_CONTROL, 0x0001, 2);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  MT9M114_DELAY_mSEC(10);

  ret = MT9M114_WRITE_REG(MT9M114_SYSCTL_REGISTER_RESET_AND_MISC_CONTROL, 0x0000, 2);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* @Observation: more delay is required for Camera Sensor
   *               to setup after Soft Reset.
   */
  MT9M114_DELAY_mSEC(100);

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t mt9m114_camera_init(ARM_CAMERA_RESOLUTION cam_resolution,
                                                          uint8_t cam_output_format)
  \brief        Initialize MT9M114 Camera Sensor.
                 this function will
                  - configure Camera Sensor resolution registers as per input parameter.
                     (currently supports only VGA 640x480(WxH) Camera resolution)
                  - configure Camera Sensor output format registers as per input parameter
                     \ref MT9M114_USER_SELECT_CAMERA_OUTPUT_FORMAT.
                     (currently supports only RAW Bayer10 Foramat)
                  - configure Camera Sensor slew rate.
  \param[in]    cam_resolution    : Camera Sensor Resolution
                                     \ref ARM_CAMERA_RESOLUTION
  \param[in]    cam_output_format : Camera Sensor Output Format
                                     \ref MT9M114_USER_SELECT_CAMERA_OUTPUT_FORMAT
  \return       \ref execution_status
*/
static int32_t mt9m114_camera_init(ARM_CAMERA_RESOLUTION cam_resolution,
                                                 uint8_t cam_output_format)
{
  uint32_t total_num     = 0;
  uint16_t output_format = 0;
  int32_t  ret = 0;

  /* Configure Camera Sensor Resolution */
  switch(cam_resolution)
  {
  /* Camera Sensor Resolution: VGA 640x480(WxH) */
  case CAMERA_RESOLUTION_VGA_640x480:
    total_num = (sizeof(mt9m114_cam_resolution_VGA_640x480) / sizeof(MT9M114_REG));
    ret = mt9m114_bulk_write_reg(mt9m114_cam_resolution_VGA_640x480, total_num);
    if(ret != ARM_DRIVER_OK)
      return ARM_DRIVER_ERROR;
    break;

  default:
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  /* Configure Camera Sensor slew rate */
  ret = MT9M114_WRITE_REG(MT9M114_SYSCTL_REGISTER_SLEW_RATE_CONTROL, 0x0, 2);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Configure Camera Sensor output format */
  switch(cam_output_format)
  {
  /* Camera Sensor output format: Raw Bayer10 format */
  case MT9M114_CAMERA_OUTPUT_FORMAT_RAW_BAYER10:
    output_format =  MT9M114_CAM_OUTPUT_FORMAT_REGISTER_FORMAT_BAYER         |   \
                     MT9M114_CAM_OUTPUT_FORMAT_REGISTER_BAYER_FORMAT_RAWR10;
    break;

  default:
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  ret = MT9M114_WRITE_REG(MT9M114_CAM_OUTPUT_FORMAT_REGISTER, output_format, 2);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}

/* if MT9M114 Camera Sensor Test-Pattern is Enabled. */
#if MT9M114_CAMERA_TEST_PATTERN_ENABLE

/**
  \fn           int32_t mt9m114_camera_testPattern_config(void)
  \brief        Configure MT9M114 Camera Test-Pattern.
  \param[in]    none
  \return       \ref execution_status
*/
static int32_t mt9m114_camera_testPattern_config(void)
{
  uint32_t total_num     = 0;
  int32_t  ret = 0;

  total_num = (sizeof(mt9m114_cam_testPattern) / sizeof(MT9M114_REG));
  ret = mt9m114_bulk_write_reg(mt9m114_cam_testPattern, total_num);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}
#endif /* end of MT9M114_CAMERA_TEST_PATTERN_ENABLE */

/**
  \fn           int32_t mt9m114_wait_for_command(uint32_t command)
  \brief        wait for System State command to complete.
  \param[in]    command  : MT9M114 Camera Sensor command
  \return       \ref execution_status
*/
static int32_t mt9m114_wait_for_command(uint32_t command)
{
  uint32_t i = 0;
  uint32_t reg_value = 0;
  int32_t  ret = 0;

  /* wait for System State command to complete. */
  for(i = 0; i < 2000; ++i)
  {
    ret = MT9M114_READ_REG(MT9M114_COMMAND_REGISTER, &reg_value,2);
    if(ret != ARM_DRIVER_OK)
      return ARM_DRIVER_ERROR;

    if(!(reg_value & command))
      break;

    MT9M114_DELAY_mSEC(1);
  }

  if(reg_value & command)
    return ARM_DRIVER_ERROR;

  if(!(reg_value & MT9M114_COMMAND_REGISTER_OK))
    return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t mt9m114_set_system_state(uint8_t next_state)
  \brief        Set the desired next System State.
  \param[in]    next_state  : System State which needs to be set.
                  - \ref Valid MT9M114 Camera Sensor System States:
                    - MT9M114_SYS_STATE_ENTER_CONFIG_CHANGE
                    - MT9M114_SYS_STATE_START_STREAMING
                    - MT9M114_SYS_STATE_ENTER_SUSPEND
                    - MT9M114_SYS_STATE_ENTER_STANDBY
                    - MT9M114_SYS_STATE_LEAVE_STANDBY
  \return       \ref execution_status
*/
static int32_t mt9m114_set_system_state(uint8_t next_state)
{
  int32_t ret = 0;

  /* Set the desired next System State */
  ret = MT9M114_WRITE_REG(MT9M114_SYSMGR_NEXT_STATE, next_state, 1);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Issue the Set State Command */
  ret = MT9M114_WRITE_REG(MT9M114_COMMAND_REGISTER,\
           MT9M114_COMMAND_REGISTER_OK | MT9M114_COMMAND_REGISTER_SET_STATE, 2);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Wait for the FW to complete the command. */
  ret = mt9m114_wait_for_command(MT9M114_COMMAND_REGISTER_SET_STATE);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t mt9m114_system_change_config(void)
  \brief        Issue Change-Config Command.
                This command must be issued after any change in
                 sensor sub-system registers to take effect,
                 for detail refer data-sheet.
                Change system state to MT9M114_SYS_STATE_ENTER_CONFIG_CHANGE.

                The Change−Config performs the following operations:
                 1. Requests the sensor to stop STREAMING
                 2. Waits until the sensor stops STREAMING (this
                    can take an entire frame time depending on when
                    the command was issued)
                 3. When the sensor stops streaming, reconfigures all
                    subsystems including the sensor
                 4. Restarts the sensor
                 5. Command completes
  \param[in]    none
  \return       \ref execution_status
*/
static __inline int32_t mt9m114_system_change_config(void)
{
  return mt9m114_set_system_state(MT9M114_SYS_STATE_ENTER_CONFIG_CHANGE);
}

/**
  \fn           int32_t mt9m114_stream_start(void)
  \brief        Start MT9M114 Camera Sensor Streaming,
                 change system state to MT9M114_SYS_STATE_START_STREAMING.
  \param[in]    none
  \return       \ref execution_status
*/
static __inline int32_t mt9m114_stream_start(void)
{
  return mt9m114_set_system_state(MT9M114_SYS_STATE_START_STREAMING);
}

/**
  \fn           int32_t mt9m114_stream_stop(void)
  \brief        Stop MT9M114 Camera Sensor Streaming,
                 change system state to MT9M114_SYS_STATE_ENTER_SUSPEND.
  \param[in]    none
  \return       \ref execution_status
*/
static __inline int32_t mt9m114_stream_stop(void)
{
  return mt9m114_set_system_state(MT9M114_SYS_STATE_ENTER_SUSPEND);
}


/**
  \fn           int32_t mt9m114_Init(ARM_CAMERA_RESOLUTION cam_resolution)
  \brief        Initialize MT9M114 Camera Sensor
                 this function will
                  - initialize i2c using i3c instance
                  - software reset MT9M114 Camera Sensor
                  - read MT9M114 chip-id, proceed only it is correct.
                  - initialize MT9M114 Camera Sensor as per input parameter
                    - Camera Resolution
                       (currently supports only VGA 640x480(WxH) Camera resolution)
                    - Camera Output Format
                       (currently supports only RAW Bayer10 format)
                    - Issue Change-Config Command to re-configure
                       all the MT9M114 Camera Sensor sub-system and registers.
                       this command must be issued after any change in
                       sensor registers to take effect, for detail refer data-sheet.
  \param[in]    cam_resolution  : Camera Resolution \ref ARM_CAMERA_RESOLUTION
  \return       \ref execution_status
*/
int32_t mt9m114_Init(ARM_CAMERA_RESOLUTION cam_resolution)
{
  int32_t  ret = 0;
  uint32_t rcv_data = 0;
  uint8_t  cam_output_format = mt9m114_info.output_format;

  /* Initialize i2c using i3c driver instance depending on
   *  MT9M114 Camera Sensor specific i2c configurations
   *   \ref mt9m114_camera_sensor_i2c_cnfg
   */
  ret = camera_sensor_i2c_init(&mt9m114_camera_sensor_i2c_cnfg);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Soft Reset MT9M114 Camera Sensor */
  ret = mt9m114_soft_reset();
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Read MT9M114 Camera Sensor CHIP ID */
  ret = MT9M114_READ_REG(MT9M114_CHIP_ID_REGISTER, &rcv_data, 2);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Proceed only if CHIP ID is correct. */
  if(rcv_data != MT9M114_CHIP_ID_REGISTER_VALUE)
    return ARM_DRIVER_ERROR;

  /* @NOTE: By-default after Soft-Reset Camera Sensor will be in streaming state,
   *        As per Hardware Jumper(P2 jumper) settings,
   *        if required then Stop Streaming using \ref mt9m114_stream_stop.
   *
   *        Suspend any stream
   *        ret = mt9m114_stream_stop();
   *        if(ret != ARM_DRIVER_OK)
   *          return ARM_DRIVER_ERROR;
   *        MT9M114_DELAY_mSEC(10);
   */

  /* Initialize the MT9M114 Camera Sensor */
  ret = mt9m114_camera_init(cam_resolution, cam_output_format);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Check if MT9M114 Camera Sensor Test-Pattern is Enabled? */
#if MT9M114_CAMERA_TEST_PATTERN_ENABLE

  /* yes then configure camera Test-Pattern. */
  ret = mt9m114_camera_testPattern_config();
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;
#endif  /* end of MT9M114_CAMERA_TEST_PATTERN_ENABLE */

  /* @NOTE: Issue Change-Config Command to re-configure
   *        all the MT9M114 Camera Sensor sub-system and registers.
   *
   *        This command must be issued after any change in
   *        sensor sub-system registers to take effect,
   *        for detail refer data-sheet.
   */
  ret = mt9m114_system_change_config();
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t mt9m114_Start(void)
  \brief        Start MT9M114 Camera Sensor Streaming.
  \param[in]    none
  \return       \ref execution_status
*/
int32_t mt9m114_Start(void)
{
  int32_t ret = 0;

  /* Start streaming */
  ret = mt9m114_stream_start();
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* @Observation: Proper Delay is required for
   *               Camera Sensor Lens to come-out from Shutter and gets steady,
   *               otherwise captured image will not be proper.
   *               adjust delay if captured image is less bright/dull.
   *
   *               As it is directly depends on Camera Setup(Light and other Environment parameters),
   *               user can add extra delay if required.
   *               MT9M114_DELAY_mSEC(2000);
   */

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t mt9m114_Stop(void)
  \brief        Stop MT9M114 Camera Sensor Streaming.
  \param[in]    none
  \return       \ref execution_status
*/
int32_t mt9m114_Stop(void)
{
  int32_t ret = 0;

  /* Suspend any stream */
  ret = mt9m114_stream_stop();
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t mt9m114_Control(uint32_t control, uint32_t arg)
  \brief        Control MT9M114 Camera Sensor.
  \param[in]    control  : Operation
  \param[in]    arg      : Argument of operation
  \return       \ref execution_status
*/
int32_t mt9m114_Control(uint32_t control, uint32_t arg)
{
  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t mt9m114_Uninit(void)
  \brief        Un-initialize MT9M114 Camera Sensor.
  \param[in]    none
  \return       \ref execution_status
*/
int32_t mt9m114_Uninit(void)
{
  return ARM_DRIVER_OK;
}

#endif /* RTE_MT9M114_CAMERA_SENSOR_ENABLE */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
