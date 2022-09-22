

#ifndef ARX3A0_CAMERA_SENSOR_ARX3A0_CAMERA_SENSOR_H_
#define ARX3A0_CAMERA_SENSOR_ARX3A0_CAMERA_SENSOR_H_

/*Need to Move this to RTE_device.h*/
#define RTE_ARX3A0_CAMERA_SENSOR_ENABLE 1

#define RTE_ARX3A0_CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE  0

/* System Includes */
#include "RTE_Device.h"

/* Proceed only if ARX3A0 Camera Sensor is enabled. */
#if RTE_ARX3A0_CAMERA_SENSOR_ENABLE

#include "Camera_Sensor.h"
#include "Camera_Sensor_i2c.h"

#define ARX3A0_CAMERA_SENSOR_SLAVE_ADDR                      0x36

#define ARX3A0_SOFTWARE_RESET_REGISTER                       0x0103
#define ARX3A0_CHIP_ID_REGISTER                              0x3000
#define ARX3A0_CHIP_ID_REGISTER_VALUE                        0x0353
#define ARX3A0_MODE_SELECT_REGISTER                          0x0100
#define ARX3A0_MIPI_CONFIG_REGISTER                          0x31BE
#define ARX3A0_RESET_REGISTER                                0x301A

typedef enum {
	ARX3A0_CAMERA_RESOLUTION_560x560,
}ARX3A0_Camera_Resolution;

int32_t arx3A0_Init(ARX3A0_Camera_Resolution cam_resolution);
int32_t arx3A0_camera_init(ARX3A0_Camera_Resolution cam_resolution);
int32_t arx3A0_Start(void);
int32_t arx3A0_Stop(void);

#endif

#endif /* ARX3A0_CAMERA_SENSOR_ARX3A0_CAMERA_SENSOR_H_ */
