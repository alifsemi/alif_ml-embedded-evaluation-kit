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
 * @file     Camera_Sensor_i2c.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     21-Sep-2021
 * @brief    i2c driver (using i3c) for Camera Sensor Slave Device Communication.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* System Includes */
#include "RTE_Components.h"
#include CMSIS_device_header

/* Project Includes */
#include "Camera_Sensor_i2c.h"
#include "Driver_I3C.h"


/* Wrapper function for Delay
 * Delay for millisecond:
 *  Provide delay using PMU(Performance Monitoring Unit).
 */
#define DELAY_mSEC(msec)       PMU_delay_loop_us(msec * 1000)

/* @Note: i3c has only one instance i3c0,
 *        which can be used for i2c and i3c.
 */

/* i3c Driver instance 0 */
extern ARM_DRIVER_I3C Driver_I3C0;

/* i3c Driver Access structure variable. */
static ARM_DRIVER_I3C *I3Cdrv;

/* i3c callback status macros */
#define CB_XferDone   1
#define CB_XferErr    2

/* i3c callback transfer completion flag :
 * used to verify status of previous
 * transferred i3c transmit/receive API calls.
 *
 * value can be CB_XferDone or CB_XferErr.
 */
static volatile uint8_t CB_XferCompletionFlag = 0;

/**
\brief Camera Sensor i3c driver errors
*/
typedef enum _CAMERA_SENSOR_I3C_DRIVER_ERROR {
  CAMERA_SENSOR_I3C_DRIVER_ERROR_DETACH  = 0,
  CAMERA_SENSOR_I3C_DRIVER_ERROR_POWEROFF,
  CAMERA_SENSOR_I3C_DRIVER_ERROR_UNINIT,
} CAMERA_SENSOR_I3C_DRIVER_ERROR;

/**
  \fn           void camera_sensor_i3c_callback(uint32_t event)
  \brief        i3c callback for Camera Sensor slave device.
                 - this is internal callback to verify status of
                   previous transferred transmit/receive API calls.
                 - mark callback_transfer_completion_flag as
                   transfer done or error as per event status.
  \param[in]    event: I3C Event
  \return       \ref execution_status
*/
static void camera_sensor_i3c_callback(uint32_t event)
{
  if (event & ARM_I3C_EVENT_TRANSFER_DONE)
  {
    /* Transfer Done. */
    CB_XferCompletionFlag = CB_XferDone;
  }

  if (event & ARM_I3C_EVENT_TRANSFER_ERROR)
  {
    /* Transfer Error. */
    CB_XferCompletionFlag = CB_XferErr;
  }
}

/**
  \fn           int32_t camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR i3c_driver_err,
                                                     uint8_t                        i2c_slave_addr)
  \brief        handle i3c driver error for Camera Sensor slave device in case any driver API call fails.
  \param[in]    i3c_driver_err  : i3c driver error \ref CAMERA_SENSOR_I3C_DRIVER_ERROR
  \param[in]    i2c_slave_addr  : Camera Sensor slave device i2c address which needs to detach from the i3c bus.
  \return       \ref execution_status
*/
static int32_t camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR i3c_driver_err,
                                            uint8_t                        i2c_slave_addr)
{
  int32_t ret;

  switch(i3c_driver_err)
  {
  case CAMERA_SENSOR_I3C_DRIVER_ERROR_DETACH:
    /* Detach attached i2c slave address */
    ret = I3Cdrv->Detachdev(i2c_slave_addr);

  /* Fall through */
  case CAMERA_SENSOR_I3C_DRIVER_ERROR_POWEROFF:
    /* Power off I3C peripheral */
    ret = I3Cdrv->PowerControl(ARM_POWER_OFF);

  /* Fall through */
  case CAMERA_SENSOR_I3C_DRIVER_ERROR_UNINIT:
    /* Un-initialize I3C driver */
    ret = I3Cdrv->Uninitialize();
  }

  ret = ARM_DRIVER_ERROR;
  return ret;
}

/**
  \fn           int32_t camera_sensor_i2c_init(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c)
  \brief        initialize i2c driver using i3cx instance.
                 this function will
                  - select i3c instance for i2c communication as per user input parameter.
                    - @Note: i3c has only one instance i3c0,
                             which can be used for i2c and i3c.
                  - initialize i3c driver with internal i3c callback.
                  - power up i3c peripheral.
                  - set i2c speed mode as per user input parameter.
                  - attach input Camera Sensor slave device i2c address to the i3c bus.
                  - call i2c driver error if any driver API call fails.
  \param[in]    i2c  : Pointer to Camera Sensor slave device i2c configurations structure
                        \ref CAMERA_SENSOR_SLAVE_I2C_CONFIG
  \return       \ref execution_status
*/
int32_t camera_sensor_i2c_init(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c)
{
  int32_t ret = 0;

  /* @Note: i3c has only one instance i3c0,
   *        which can be used for i2c and i3c.
   */
  if(i2c->I3Cx_instance != CAMERA_SENSOR_I2C_USING_I3C0_INSTANCE)
      return ARM_DRIVER_ERROR_PARAMETER;

  I3Cdrv = &Driver_I3C0;

  /* Initialize i3c driver */
  ret = I3Cdrv->Initialize(camera_sensor_i3c_callback);
  if(ret != ARM_DRIVER_OK)
    return ARM_DRIVER_ERROR;

  /* Power up i3c peripheral */
  ret = I3Cdrv->PowerControl(ARM_POWER_FULL);
  if(ret != ARM_DRIVER_OK)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_UNINIT,0));

  /* Set i2c speed mode as per user input */
  switch(i2c->speed_mode)
  {
  /* Camera Sensor slave i2c Speed Mode : Fast Mode Plus   1 MBPS */
  case CAMERA_SENSOR_I2C_SPEED_FMP_1_MBPS:
    ret = I3Cdrv->Control(I3C_MASTER_SET_BUS_MODE, I3C_BUS_MODE_MIXED_FAST_I2C_FMP_SPEED_1_MBPS);
    break;

  /* Camera Sensor slave i2c Speed Mode : Fast Mode      400 KBPS */
  case CAMERA_SENSOR_I2C_SPEED_FM_400_KBPS:
    ret = I3Cdrv->Control(I3C_MASTER_SET_BUS_MODE, I3C_BUS_MODE_MIXED_FAST_I2C_FM_SPEED_400_KBPS);
    break;

  /* Camera Sensor slave i2c Speed Mode : Standard Mode  100 KBPS */
  case CAMERA_SENSOR_I2C_SPEED_SS_100_KBPS:
    ret = I3Cdrv->Control(I3C_MASTER_SET_BUS_MODE, I3C_BUS_MODE_MIXED_SLOW_I2C_SS_SPEED_100_KBPS);
    break;

  default:
    return ARM_DRIVER_ERROR_PARAMETER;
  }
  if(ret != ARM_DRIVER_OK)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_POWEROFF,0));

  /* Attach input Camera Sensor slave device i2c address to the i3c bus. */
  ret = I3Cdrv->AttachI2Cdev(i2c->cam_sensor_slave_addr);
  if(ret != ARM_DRIVER_OK)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_POWEROFF,0));

  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t camera_sensor_i2c_write(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c,
                                                uint32_t                        reg_addr,
                                                uint32_t                        reg_value,
                                                CAMERA_SENSOR_I2C_REG_SIZE      reg_size)
  \brief        write value to Camera Sensor slave device register using i2c.
                 this function will
                  - fill data as per Camera Sensor slave device supported
                     register address type: 8-bit/16-bit.
                  - as Camera Sensor slave device supports Big endian mode:
                     - convert data from Little Endian to Big Endian
                       (only required if Camera Sensor slave device supports
                        - 16-bit register addressing or
                        - register size is >8-bit.)
                  - transmit data to already attached Camera Sensor slave i2c device.
  \param[in]    i2c        : Pointer to Camera Sensor slave device i2c configurations structure
                              \ref CAMERA_SENSOR_SLAVE_I2C_CONFIG
  \param[in]    reg_addr   : register address of Camera Sensor slave device
  \param[in]    reg_value  : register value
  \param[in]    reg_size   : register size \ref CAMERA_SENSOR_I2C_REG_SIZE
  \return       \ref execution_status
*/
int32_t camera_sensor_i2c_write(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c,
                                uint32_t                        reg_addr,
                                uint32_t                        reg_value,
                                CAMERA_SENSOR_I2C_REG_SIZE      reg_size)
{
  uint8_t addr_low  = reg_addr & 0xFF;
  uint8_t addr_high = (reg_addr >> 8) & 0xFF;
  uint8_t addr_len  = 0;

  /* Max size of tx data = Max slave reg_addr_type(16-bit/2 byte) +
   *                       Max reg_size(32-bit/4 byte)
   */
  uint8_t tx_data[6] = {0};
  uint8_t data_len = 0;

  uint8_t  i = 0;
  uint32_t temp = 0;
  uint32_t timeout = 0;
  int32_t  ret   = 0;
  CAMERA_SENSOR_I2C_REG_ADDR_TYPE reg_addr_type = i2c->cam_sensor_slave_reg_addr_type;

  /* supports only 8-bit/16-bit Camera Sensor slave register address type. */
  if( (reg_addr_type != CAMERA_SENSOR_I2C_REG_ADDR_TYPE_8BIT) && \
      (reg_addr_type != CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT) )
    return ARM_DRIVER_ERROR_PARAMETER;

  /* supports max 32-bit Camera Sensor slave register size. */
  if(reg_size > CAMERA_SENSOR_I2C_REG_SIZE_32BIT)
    return ARM_DRIVER_ERROR_PARAMETER;


  /* To write to any Camera Sensor slave register address:
   *
   * 1.) As Camera Sensor slave device supports Big endian mode,
   *      first convert input data from Little endian to Big endian.
   *      (only required if Camera Sensor slave device supports
   *        - 16-bit register addressing or
   *        - register size is >8-bit.)
   *
   * 2.) Then Transmit data to i3c TX FIFO as shown below:
   *      register address         +  register value
   *      (base on type 8/16 bit)     (base on register size 8/16/32 bit)
   */

  /*
   * @Note: Convert data from Little Endian to Big Endian
   *         (only required if Camera Sensor slave device supports
   *          - 16-bit register addressing or
   *          - register size is >8-bit.)
   *
   *   How much data(register address + actual data) user has to Transmit/Receive ?
   *   it depends on Slave's register address location bytes.
   *   Generally, Camera Slave supports     16-bit(2 Byte) register address and (8/16/32 bit) data
   *   Others Accelerometer/EEPROM supports  8-bit(1 Byte) register address and (8/16/32 bit) data
   *
   *   First LSB[7-0] will be added to TX FIFO and first transmitted on the i3c bus;
   *   remaining bytes will be added in LSB -> MSB order.
   *
   *   For Slave who supports 16-bit(2 Byte) register address and data:
   *   Register Address[15:8] : Needs to be Transmit First  to the i3c
   *   Register Address[07:0] : Needs to be Transmit Second to the i3c
   *
   *   That means,
   *
   *   While transmitting to i3c TX FIFO,
   *   MSB of TX data needs to be added first to the i3c TX FIFO.
   *
   *   While receiving from i3c RX FIFO,
   *   First MSB will be received from i3c RX FIFO.
   *
   *   START          I3C FIFO           END
   *   MSB                               LSB
   *   24-31 bit | 16-23 bit | 8-15 bit | 0-7 bit
   *
   *   So, USER has to modify(we have already done it below for you!)
   *   Transmit/Receive data (Little Endian <-> Big Endian and vice versa)
   *   before sending/after receiving to/from i3c TX/RX FIFO.
   */

  /* if Camera Sensor slave supports 8-bit register address type. */
  if(reg_addr_type == CAMERA_SENSOR_I2C_REG_ADDR_TYPE_8BIT)
  {
    /* @Note: 8-bit Register Address Type is not yet tested
     *        with any 8-bit supported Camera Sensor slave device.
     */
    tx_data[0] = addr_low;
    addr_len = 1;
  }

  /* if Camera Sensor slave supports 16-bit register address type. */
  if(reg_addr_type == CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT)
  {
    tx_data[0] = addr_high;
    tx_data[1] = addr_low;
    addr_len = 2;
  }

  /* total data length is register address type + register size */
  data_len = (addr_len + reg_size);

  /* For Transmit, Convert input data from Little Endian to Big Endian. */
  i    = reg_size;
  temp = reg_value;

  while(i--)
  {
    tx_data[addr_len + i] = (temp & 0xff);
    temp >>= 8;
  }

  /* clear i3c callback completion flag. */
  CB_XferCompletionFlag = 0;

  /* Transmit data to i3C TX FIFO. */
  ret = I3Cdrv->MasterTransmit(i2c->cam_sensor_slave_addr, tx_data, data_len);
  if(ret != ARM_DRIVER_OK)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_DETACH,i2c->cam_sensor_slave_addr));

  /* timeout in millisecond. */
  timeout = 100;

  /* wait for i3c callback within timeout. */
  while(timeout--)
  {
    /* received callback? */
    if(CB_XferCompletionFlag)
      break;

    /* sleep or wait for millisecond depending on RTOS availability. */
    DELAY_mSEC(1);
  }

  /* i3c module failed to respond? de-init i3c driver and return error. */
  if(!CB_XferCompletionFlag)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_DETACH,i2c->cam_sensor_slave_addr));

  /* return error, if received transfer error. */
  if(CB_XferCompletionFlag == CB_XferErr)
    return ARM_DRIVER_ERROR;

  /* received transfer success. */
  return ARM_DRIVER_OK;
}

/**
  \fn           int32_t camera_sensor_i2c_read(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c,
                                               uint32_t                        reg_addr,
                                               uint32_t                       *reg_value,
                                               CAMERA_SENSOR_I2C_REG_SIZE      reg_size)
  \brief        read value from Camera Sensor slave device register using i2c.
                 this function will
                  - to read from register,
                    - first transmit register address location,
                    - then receive register value from that location.
                  - fill data as per Camera Sensor slave device supported
                     register address type 8-bit/16-bit.
                  - transmit data to already attached Camera Sensor slave i2c device.
                  - receive data from attached Camera Sensor slave i2c device depending on
                    register size.
                  - As Camera Sensor slave device supports Big endian mode:
                    - convert received data from Big Endian to Little Endian
                       (only required if Camera Sensor slave supports
                        - 16-bit register addressing
                        - reg_size is >8-bit.)
  \param[in]    i2c        : Pointer to Camera Sensor slave device i2c configurations structure
                              \ref CAMERA_SENSOR_SLAVE_I2C_CONFIG
  \param[in]    reg_addr   : register address of Camera Sensor slave device
  \param[in]    reg_value  : pointer to register value
  \param[in]    reg_size   : register size \ref CAMERA_SENSOR_I2C_REG_SIZE
  \return       \ref execution_status
*/
int32_t camera_sensor_i2c_read(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c,
                               uint32_t                        reg_addr,
                               uint32_t                       *reg_value,
                               CAMERA_SENSOR_I2C_REG_SIZE      reg_size)
{
  uint8_t addr_low  = reg_addr & 0xFF;
  uint8_t addr_high = (reg_addr >> 8) & 0xFF;
  uint8_t addr_len  = 0;

  /* Max size of tx data = Max reg_addr type(16-bit/2 byte) */
  uint8_t tx_data[2] = {00};

  /* Max size of rx data = Max reg_size(32-bit/4 byte) */
  uint8_t rx_data[4] = {00};
  uint8_t data_len   = 0;

  uint8_t  i = 0;
  uint32_t temp = 0;
  uint32_t timeout = 0;
  int32_t  ret = 0;
  CAMERA_SENSOR_I2C_REG_ADDR_TYPE reg_addr_type = i2c->cam_sensor_slave_reg_addr_type;

  /* supports only 8-bit/16-bit Camera Sensor slave register address type. */
  if( (reg_addr_type != CAMERA_SENSOR_I2C_REG_ADDR_TYPE_8BIT) && \
      (reg_addr_type != CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT) )
    return ARM_DRIVER_ERROR_PARAMETER;

  /* supports max 32-bit Camera Sensor slave register size. */
  if(reg_size > CAMERA_SENSOR_I2C_REG_SIZE_32BIT)
    return ARM_DRIVER_ERROR_PARAMETER;


  /* To Read from any Camera Sensor slave register address:
   *
   * 1.) As Camera Sensor slave device supports Big endian mode,
   *      first convert input data from Little endian to Big endian.
   *       (only required if Camera Sensor slave device supports
   *         - 16-bit register addressing or
   *         - register size is >8-bit.)
   *
   * 2.) Then Transmit data(register address location) to i3c TX FIFO
   *     as shown below:
   *      register address
   *      (base on type 8/16 bit)
   *
   * 3.) Receive data from register location using i3c RX FIFO
   *     as shown below:
   *      read register value
   *      (base on register size 8/16/32 bit)
   *
   * 4.) Convert Receive data back from Big Endian to Little Endian
   *      (only required if Camera Sensor slave device supports
   *       - 16-bit register addressing or
   *       - register size is >8-bit.)
   */

  /*
   * @Note: Convert data from Big Endian to Little Endian.
   *         (only required if Camera Sensor slave device supports
   *           - 16-bit register addressing or
   *           - register size is >8-bit.)
   *
   *   How much data(register address + actual data) user has to Transmit/Receive ?
   *   it depends on Slave's register address location bytes.
   *   Generally, Camera Slave supports     16-bit(2 Byte) register address and (8/16/32 bit) data
   *   Others Accelerometer/EEPROM supports  8-bit(1 Byte) register address and (8/16/32 bit) data
   *
   *   First LSB[7-0] will be added to TX FIFO and first transmitted on the i3c bus;
   *   remaining bytes will be added in LSB -> MSB order.
   *
   *   For Slave who supports 16-bit(2 Byte) register address and data:
   *   Register Address[15:8] : Needs to be Transmit First  to the i3c
   *   Register Address[07:0] : Needs to be Transmit Second to the i3c
   *
   *   That means,
   *
   *   While transmitting to i3c TX FIFO,
   *   MSB of TX data needs to be added first to the i3c TX FIFO.
   *
   *   While receiving from i3c RX FIFO,
   *   First MSB will be received from i3c RX FIFO.
   *
   *   START          I3C FIFO           END
   *   MSB                               LSB
   *   24-31 bit | 16-23 bit | 8-15 bit | 0-7 bit
   *
   *   So, USER has to modify(we have already done it below for you!)
   *   Transmit/Receive data (Little Endian <-> Big Endian and vice versa)
   *   before sending/after receiving to/from i3c TX/RX FIFO.
   */

  /* if Camera Sensor slave supports 8-bit register address type. */
  if(reg_addr_type == CAMERA_SENSOR_I2C_REG_ADDR_TYPE_8BIT)
  {
    /* @Note: 8-bit Register Address Type is not yet tested
     *        with any 8-bit supported Camera Sensor slave device.
     */
    tx_data[0]  = addr_low;
    addr_len = 1;
  }

  /* if Camera Sensor slave supports 16-bit register address type. */
  if(reg_addr_type == CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT)
  {
    tx_data[0]  = addr_high;
    tx_data[1]  = addr_low;
    addr_len = 2;
  }

  /* transmit register location, data length is only register address type. */
  data_len = addr_len;

  /* clear i3c callback completion flag. */
  CB_XferCompletionFlag = 0;

  /* Transmit data to I3C TX FIFO. */
  ret = I3Cdrv->MasterTransmit(i2c->cam_sensor_slave_addr, tx_data, data_len);
  if(ret != ARM_DRIVER_OK)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_DETACH,i2c->cam_sensor_slave_addr));

  /* timeout in millisecond. */
  timeout = 100;

  /* wait for i3c callback within timeout. */
  while(timeout--)
  {
    /* received callback? */
    if(CB_XferCompletionFlag)
      break;

    /* sleep or wait for millisecond depending on RTOS availability. */
    DELAY_mSEC(1);
  }

  /* i3c module failed to respond? de-init i3c driver and return error. */
  if(!CB_XferCompletionFlag)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_DETACH,i2c->cam_sensor_slave_addr));

  /* return error, if received transfer error. */
  if(CB_XferCompletionFlag == CB_XferErr)
    return ARM_DRIVER_ERROR;


  /* received transfer success.
   * now, read from register address location.
   * data length will be register address size. */
  data_len = reg_size;

  /* clear i3c callback completion flag. */
  CB_XferCompletionFlag = 0;

  /* Receive data from I3C RX FIFO. */
  ret = I3Cdrv->MasterReceive(i2c->cam_sensor_slave_addr, rx_data, data_len);
  if(ret != ARM_DRIVER_OK)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_DETACH,i2c->cam_sensor_slave_addr));

  /* timeout in millisecond. */
  timeout = 100;

  /* wait for i3c callback within timeout. */
  while(timeout--)
  {
    /* received callback? */
    if(CB_XferCompletionFlag)
      break;

    /* sleep or wait for millisecond depending on RTOS availability. */
    DELAY_mSEC(1);
  }

  /* i3c module failed to respond? de-init i3c driver and return error. */
  if(!CB_XferCompletionFlag)
    return (camera_sensor_i3c_driver_err(CAMERA_SENSOR_I3C_DRIVER_ERROR_DETACH,i2c->cam_sensor_slave_addr));

  /* return error, if received transfer error. */
  if(CB_XferCompletionFlag == CB_XferErr)
    return ARM_DRIVER_ERROR;

  /* received transfer success.
   * now for Receive, Convert received data back from Big Endian to Little Endian. */
  i = 0;
  temp = 0;
  for(i=0; i<reg_size; i++)
  {
    temp <<= 8;
    temp |= rx_data[i];
  }

  /* update register value */
  *reg_value = temp;

  return ARM_DRIVER_OK;
}

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
