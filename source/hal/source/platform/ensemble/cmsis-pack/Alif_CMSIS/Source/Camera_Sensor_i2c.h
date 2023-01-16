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
 * @file     Camera_Sensor_i2c.h
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     21-Sep-2021
 * @brief    i2c (using i3c) definitions for Camera Sensor Slave Device
 *           Communication.
 ******************************************************************************/

#ifndef CAMERA_SENSOR_I2C_H_
#define CAMERA_SENSOR_I2C_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
\brief Camera Sensor i2c using i3cx instance
\Note  i3c has only one instance i3c0,
       which can be used for i2c and i3c.
*/
typedef enum _CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE {
  CAMERA_SENSOR_I2C_USING_I3C0_INSTANCE = 0,         /* Camera Sensor i2c using I3C0 instance */
} CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE;

/**
\brief Camera Sensor i2c Speed Mode
*/
typedef enum _CAMERA_SENSOR_I2C_SPEED_MODE {
  CAMERA_SENSOR_I2C_SPEED_FMP_1_MBPS  = 1,           /* Camera Sensor i2c Speed Mode : Fast Mode Plus   1 MBPS */
  CAMERA_SENSOR_I2C_SPEED_FM_400_KBPS = 2,           /* Camera Sensor i2c Speed Mode : Fast Mode      400 KBPS */
  CAMERA_SENSOR_I2C_SPEED_SS_100_KBPS = 3,           /* Camera Sensor i2c Speed Mode : Standard Mode  100 KBPS */
} CAMERA_SENSOR_I2C_SPEED_MODE;

/**
\brief Camera Sensor i2c Register Address Type
*/
typedef enum _CAMERA_SENSOR_I2C_REG_ADDR_TYPE {
  CAMERA_SENSOR_I2C_REG_ADDR_TYPE_8BIT  = 1,         /* Camera Sensor i2c Register Address type :  8-bit */
  CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT = 2,         /* Camera Sensor i2c Register Address type : 16-bit */
} CAMERA_SENSOR_I2C_REG_ADDR_TYPE;

/**
\brief Camera Sensor i2c Register Size
*/
typedef enum _CAMERA_SENSOR_I2C_REG_SIZE {
  CAMERA_SENSOR_I2C_REG_SIZE_8BIT  = 1,              /* Camera Sensor i2c Register Size :  8-bit */
  CAMERA_SENSOR_I2C_REG_SIZE_16BIT = 2,              /* Camera Sensor i2c Register Size : 16-bit */
  CAMERA_SENSOR_I2C_REG_SIZE_32BIT = 4,              /* Camera Sensor i2c Register Size : 32-bit */
} CAMERA_SENSOR_I2C_REG_SIZE;

/**
\brief Camera Sensor Slave i2c Configuration
*/
typedef struct CAMERA_SENSOR_SLAVE_I2C_CONFIG {
  CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE I3Cx_instance;                   /* Camera Sensor slave i2c using I3Cx instance   */
  CAMERA_SENSOR_I2C_SPEED_MODE          speed_mode;                      /* Camera Sensor slave i2c Speed Mode            */
  uint8_t                               cam_sensor_slave_addr;           /* Camera Sensor slave i2c Address               */
  CAMERA_SENSOR_I2C_REG_ADDR_TYPE       cam_sensor_slave_reg_addr_type;  /* Camera Sensor slave i2c Register Address type */
} CAMERA_SENSOR_SLAVE_I2C_CONFIG;


/* initialize i2c driver using i3cx instance. */
extern int32_t camera_sensor_i2c_init(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c);

/* write value to Camera Sensor slave register using i2c. */
extern int32_t camera_sensor_i2c_write(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c,
                                       uint32_t                        reg_addr,
                                       uint32_t                        reg_value,
                                       CAMERA_SENSOR_I2C_REG_SIZE      reg_size);

/* read value from Camera Sensor slave register using i2c. */
extern int32_t camera_sensor_i2c_read(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c,
                                      uint32_t                        reg_addr,
                                      uint32_t                       *reg_value,
                                      CAMERA_SENSOR_I2C_REG_SIZE      reg_size);

#ifdef  __cplusplus
}
#endif

#endif /* CAMERA_SENSOR_I2C_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
