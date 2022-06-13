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
 * @file     arx3A0_camera_sensor.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     1-April-2022
 * @brief    ONsemi ARX3A0 Camera Sensor driver User Configurations and defines.
 ******************************************************************************/

#ifndef ARX3A0_CAMERA_SENSOR_H_
#define ARX3A0_CAMERA_SENSOR_H_

/* System Includes */
#include "RTE_Device.h"

/* Proceed only if ARX3A0 Camera Sensor is enabled. */
#if RTE_ARX3A0_CAMERA_SENSOR_ENABLE

#include "Camera_Sensor.h"
#include "Camera_Sensor_i2c.h"

/* ARX3A0 Camera Sensor Slave Address. */
#define ARX3A0_CAMERA_SENSOR_SLAVE_ADDR                      0x36

/* Supported ARX3A0 Camera Sensor Output Format.
 *  (Currently supports only RAW BAYER10 Format.)
 */
#define ARX3A0_CAMERA_OUTPUT_FORMAT_RAW_BAYER10              0

/* User can select from supported ARX3A0 Camera Sensor Output Format. */
#define ARX3A0_USER_SELECT_CAMERA_OUTPUT_FORMAT              ARX3A0_CAMERA_OUTPUT_FORMAT_RAW_BAYER10

/* ARX3A0 Camera Sensor CHIP-ID registers */
#define ARX3A0_CHIP_ID_REGISTER                              0x3000
#define ARX3A0_CHIP_ID_REGISTER_VALUE                        0x0353

/* ARX3A0 Camera Sensor registers index */
#define ARX3A0_SOFTWARE_RESET_REGISTER                       0x0103
#define ARX3A0_MODE_SELECT_REGISTER                          0x0100
#define ARX3A0_MIPI_CONFIG_REGISTER                          0x31BE
#define ARX3A0_RESET_REGISTER                                0x301A

/*Camera Sensor clock source register parameters*/
#define ARX3A0_CLK_EN_Msk                                    1U
#define ARX3A0_CLK_DIV_Msk                                   MASK(24,16)
#define ARX3A0_CLK_DIV_Pos                                   16U

/**
  \brief ARX3A0 Camera Sensor Register Array Structure
  used for Camera Resolution Configuration.
  */
typedef struct _ARX3A0_REG {
	uint16_t reg_addr;             /* ARX3A0 Camera Sensor Register Address*/
	uint32_t reg_value;            /* ARX3A0 Camera Sensor Register Value*/
} ARX3A0_REG;

/**
  \brief ARX3A0 Camera Sensor Resolution 560x560
  */
static const ARX3A0_REG arx3a0_560_regs[] = {

	/*LOAD= PLL_360Fps_80MHz_80MHz_20MHz*/
	{0x300, 0xA    },
	{0x302, 0x1    },
	{0x304, 0x101  },
	{0x306, 0x2828 },
	{0x308, 0xA    },
	{0x30A, 0x1    },

	/*LOAD= MIPI_TIMING_10bit*/
	{0x31B0, 0x47   },
	{0x31B2, 0x26   },
	{0x31B4, 0x328C },
	{0x31B6, 0x32E8 },
	{0x31B8, 0x1C12 },
	{0x31BA, 0x1452 },
	{0x31BC, 0x8488 },

	/*LOAD= Analog_Setup_Recommended*/
	{0x3ED0, 0x748  },
	{0x3ED6, 0x3136 },
	{0x3EDC, 0x1020 },
	{0x3EDE, 0x1D2A },
	{0x3EE0, 0x282A },
	{0x3EE2, 0x2821 },
	{0x3EC8, 0x401  },
	{0x3ED2, 0x3903 },
	{0x3EC0, 0x0011 },
	{0x3ECA, 0x826F },
	{0x3EBC, 0xA8AA },
	{0x3EC4, 0x1000 },
	{0x3EBA, 0x44   },

	/*LOAD= Corrections_Recommended*/
	{0x3ED0, 0x745  },
	{0x3ED4, 0x16   },
	{0x3EC6, 0x80F2 },
	{0x3ED8, 0x55FF },
	{0x3EE6, 0x8000 },
	{0x30D2, 0x0    },
	{0x31E0, 0x00F1 },
	{0x31E6, 0xA35F },
	{0x3180, 0x9096 },
	{0x3120, 0x1    },
	{0x301E, 0x2A   },

	/*LOAD= Pixel_Timing_Recommended_10bit*/
	{0x3D00, 0x0436 },
	{0x3D02, 0x435A },
	{0x3D04, 0xFFFF },
	{0x3D06, 0xFFFF },
	{0x3D08, 0x2180 },
	{0x3D0A, 0x0005 },
	{0x3D0C, 0x108F },
	{0x3D0E, 0x0802 },
	{0x3D10, 0x5248 },
	{0x3D12, 0x801B },
	{0x3D14, 0x006F },
	{0x3D16, 0x8269 },
	{0x3D18, 0x6A82 },
	{0x3D1A, 0x5148 },
	{0x3D1C, 0x5A80 },
	{0x3D1E, 0x5902 },
	{0x3D20, 0x8082 },
	{0x3D22, 0x3060 },
	{0x3D24, 0x8567 },
	{0x3D26, 0x5C20 },
	{0x3D28, 0x4880 },
	{0x3D2A, 0x0284 },
	{0x3D2C, 0x6084 },
	{0x3D2E, 0x5C91 },
	{0x3D30, 0x5980 },
	{0x3D32, 0x5883 },
	{0x3D34, 0x6462 },
	{0x3D36, 0x8056 },
	{0x3D38, 0x8162 },
	{0x3D3A, 0x8422 },
	{0x3D3C, 0x20A2 },
	{0x3D3E, 0x2220 },
	{0x3D40, 0x804B },
	{0x3D42, 0x8110 },
	{0x3D44, 0x0943 },
	{0x3D46, 0x9243 },
	{0x3D48, 0x8050 },
	{0x3D4A, 0x9A4B },
	{0x3D4C, 0x8563 },
	{0x3D4E, 0x8363 },
	{0x3D50, 0x8422 },
	{0x3D52, 0x20A2 },
	{0x3D54, 0x61C6 },
	{0x3D56, 0x6F99 },
	{0x3D58, 0x3009 },
	{0x3D5A, 0x1FF6 },
	{0x3D5C, 0x20ED },
	{0x3D5E, 0x0874 },
	{0x3D60, 0x8230 },
	{0x3D62, 0x609B },
	{0x3D64, 0x3060 },
	{0x3D66, 0x4600 },
	{0x3D68, 0x3783 },
	{0x3D6A, 0x7070 },
	{0x3D6C, 0x8040 },
	{0x3D6E, 0x4A44 },
	{0x3D70, 0x8003 },
	{0x3D72, 0x0086 },
	{0x3D74, 0x4588 },
	{0x3D76, 0x46BA },
	{0x3D78, 0x0300 },
	{0x3D7A, 0xFFD7 },
	{0x3D7C, 0x4688 },
	{0x3D7E, 0x4588 },
	{0x3D80, 0x4492 },
	{0x3D82, 0x4A9B },
	{0x3D84, 0x4070 },
	{0x3D86, 0x8040 },
	{0x3D88, 0x4AAD },
	{0x3D8A, 0x0070 },
	{0x3D8C, 0xAE47 },
	{0x3D8E, 0x8547 },
	{0x3D90, 0xAD78 },
	{0x3D92, 0x6B85 },
	{0x3D94, 0x6A80 },
	{0x3D96, 0x6984 },
	{0x3D98, 0x6B8A },
	{0x3D9A, 0x6B80 },
	{0x3D9C, 0x6980 },
	{0x3D9E, 0x6A85 },
	{0x3DA0, 0x7C93 },
	{0x3DA2, 0x846B },
	{0x3DA4, 0x8465 },
	{0x3DA6, 0x46FF },
	{0x3DA8, 0xAA65 },
	{0x3DAA, 0x9C79 },
	{0x3DAC, 0x4A00 },
	{0x3DAE, 0x2180 },
	{0x3DB0, 0x44AC },
	{0x3DB2, 0x7070 },
	{0x3DB4, 0x2180 },
	{0x3DB6, 0x0005 },
	{0x3DB8, 0x108F },
	{0x3DBA, 0x0802 },
	{0x3DBC, 0x5248 },
	{0x3DBE, 0x801B },
	{0x3DC0, 0x006F },
	{0x3DC2, 0x8269 },
	{0x3DC4, 0x6A82 },
	{0x3DC6, 0x5148 },
	{0x3DC8, 0x5A80 },
	{0x3DCA, 0x5902 },
	{0x3DCC, 0x8082 },
	{0x3DCE, 0x3060 },
	{0x3DD0, 0x8567 },
	{0x3DD2, 0x5C20 },
	{0x3DD4, 0x4880 },
	{0x3DD6, 0x0284 },
	{0x3DD8, 0x6084 },
	{0x3DDA, 0x5C91 },
	{0x3DDC, 0x5980 },
	{0x3DDE, 0x5883 },
	{0x3DE0, 0x6462 },
	{0x3DE2, 0x8056 },
	{0x3DE4, 0x8162 },
	{0x3DE6, 0x8422 },
	{0x3DE8, 0x209C },
	{0x3DEA, 0x2220 },
	{0x3DEC, 0x514B },
	{0x3DEE, 0x8110 },
	{0x3DF0, 0x0943 },
	{0x3DF2, 0x9843 },
	{0x3DF4, 0x8050 },
	{0x3DF6, 0x8B51 },
	{0x3DF8, 0x8D4B },
	{0x3DFA, 0x9063 },
	{0x3DFC, 0x8363 },
	{0x3DFE, 0x8422 },
	{0x3E00, 0x209C },
	{0x3E02, 0x61D3 },
	{0x3E04, 0x1FB6 },
	{0x3E06, 0x20ED },
	{0x3E08, 0x3809 },
	{0x3E0A, 0x524B },
	{0x3E0C, 0x0014 },
	{0x3E0E, 0x4580 },
	{0x3E10, 0x4681 },
	{0x3E12, 0x3060 },
	{0x3E14, 0x9D30 },
	{0x3E16, 0x6083 },
	{0x3E18, 0x4645 },
	{0x3E1A, 0x0017 },
	{0x3E1C, 0x8170 },
	{0x3E1E, 0x7070 },
	{0x3E20, 0x7070 },
	{0x3E22, 0x7070 },
	{0x3E24, 0x7070 },
	{0x3E26, 0x7070 },
	{0x3E28, 0x7070 },
	{0x3E2A, 0x7070 },
	{0x3E2C, 0x7070 },
	{0x3E2E, 0x7070 },
	{0x3E30, 0x7070 },
	{0x3E32, 0x7070 },
	{0x3E34, 0x7070 },
	{0x3E36, 0x7070 },
	{0x3E38, 0x7070 },
	{0x3E3A, 0x7070 },
	{0x3E3C, 0x7070 },
	{0x3E3E, 0x7070 },
	{0x3E40, 0x7070 },
	{0x3E42, 0x7070 },
	{0x3E44, 0x7070 },
	{0x3E46, 0x7070 },
	{0x3E48, 0x7070 },
	{0x3E4A, 0x7070 },
	{0x3E4C, 0x7070 },
	{0x3E4E, 0x7070 },
	{0x3E50, 0x7070 },
	{0x3E52, 0x7070 },
	{0x3E54, 0x7070 },
	{0x3E56, 0x7070 },
	{0x3E58, 0x7070 },
	{0x3E5A, 0x7070 },
	{0x3E5C, 0x7070 },
	{0x3E5E, 0x7070 },
	{0x3E60, 0x7070 },
	{0x3E62, 0x7070 },
	{0x3E64, 0x7070 },
	{0x3E66, 0x7070 },
	{0x3E68, 0x7070 },
	{0x3E6A, 0x7070 },
	{0x3E6C, 0x7070 },
	{0x3E6E, 0x7070 },
	{0x3E70, 0x7070 },
	{0x3E72, 0x7070 },
	{0x3E74, 0x7070 },
	{0x3E76, 0x7070 },
	{0x3E78, 0x7070 },
	{0x3E7A, 0x7070 },
	{0x3E7C, 0x7070 },
	{0x3E7E, 0x7070 },
	{0x3E80, 0x7070 },
	{0x3E82, 0x7070 },
	{0x3E84, 0x7070 },
	{0x3E86, 0x7070 },
	{0x3E88, 0x7070 },
	{0x3E8A, 0x7070 },
	{0x3E8C, 0x7070 },
	{0x3E8E, 0x7070 },
	{0x3E90, 0x7070 },
	{0x3E92, 0x7070 },
	{0x3E94, 0x7070 },
	{0x3E96, 0x7070 },
	{0x3E98, 0x7070 },
	{0x3E9A, 0x7070 },
	{0x3E9C, 0x7070 },
	{0x3E9E, 0x7070 },
	{0x3EA0, 0x7070 },
	{0x3EA2, 0x7070 },
	{0x3EA4, 0x7070 },
	{0x3EA6, 0x7070 },
	{0x3EA8, 0x7070 },
	{0x3EAA, 0x7070 },
	{0x3EAC, 0x7070 },
	{0x3EAE, 0x7070 },
	{0x3EB0, 0x7070 },
	{0x3EB2, 0x7070 },
	{0x3EB4, 0x7070 },
	{0x0104, 0x0001 }, //Group_Parameter Hold
	{0x0344, 0x0004 }, //X_ADDR_START
	{0x0346, 0x0004 }, //Y_ADDR_START
	{0x0348, 0x0233 }, //X_ADDR_END
	{0x034A, 0x0233 }, //Y_ADDR_END
	{0x034C, 0x0230 }, //X_OUTPUT_SIZE
	{0x034E, 0x0230 }, //Y_OUTPUT_SIZE
	{0x3040, 0x41   }, //Y_ODD_INC
	{0x30A4, 0x1    }, //Y_EVEN_INC
	{0x342,  0x2F8  }, //line_length_pck
	{0x340,  0x248  }, //frame_length_lines ??
	{0x3012, 0x0107 }, //coarse_integration_time ??
	{0x112 , 0xA0A  }, //RAW10
	{0x202 , 0x366A },
	{0x300 , 0xA    },
	{0x302 , 0x1    },
	{0x304 , 0x101  },
	{0x306 , 0x2828 },
	{0x308 , 0xA    },
	{0x30A , 0x1    },
	{0x340 , 0xA47B },
	{0x342 , 0x2F8  },
	{0x344 , 0x4    },
	{0x346 , 0x4    },
	{0x348 , 0x233  },
	{0x34A , 0x233  },
	{0x34C , 0x230  },
	{0x34E , 0x230  },
	{0x382 , 0x1    },
	{0x386 , 0x1    },
	{0x400 , 0x0    },
	{0x402 , 0x0    },
	{0x404 , 0x10   },
	{0x3000, 0x353  },
	{0x3002, 0x4    },
	{0x3004, 0x4    },
	{0x3006, 0x233  },
	{0x3008, 0x233  },
	{0x300A, 0xA47B },
	{0x300C, 0x2F8  },
	{0x3012, 0x366A },
	{0x3018, 0x0    },
	{0x301A, 0x1C   },
	{0x301C, 0x1    },
	{0x301D, 0x0    },
	{0x301E, 0x2A   },
	{0x3021, 0x0    },
	{0x3022, 0x0    },
	{0x3023, 0x0    },
	{0x3026, 0xFFFF },
	{0x3028, 0x4    },
	{0x3032, 0x100  },
	{0x303A, 0xA    },
	{0x303B, 0xF7   },
	{0x303C, 0x0    },
	{0x303E, 0x0    },
	{0x3040, 0x41   },
	{0x3044, 0x10C0 },
	{0x3046, 0x608  },
	{0x3048, 0x8    },
	{0x304A, 0x60   },
	{0x304C, 0x200  },
	{0x305E, 0x2000 },
	{0x3064, 0x5840 },
	{0x3068, 0x0    },
	{0x306A, 0x0    },
	{0x306E, 0x9000 },
	{0x3070, 0x0    },
	{0x3072, 0x0    },
	{0x3074, 0x0    },
	{0x3076, 0x0    },
	{0x3078, 0x0    },
	{0x307A, 0x0    },
	{0x307E, 0x20   },
	{0x3088, 0x1    },
	{0x30A0, 0x1    },
	{0x30A2, 0x1    },
	{0x30A4, 0x1    },
	{0x30A6, 0x1    },
	{0x30AA, 0x0    },
	{0x30B0, 0x400  },
	{0x30BC, 0x0    },
	{0x30BE, 0x0    },
	{0x30C0, 0x2000 },
	{0x30C2, 0x0    },
	{0x30E8, 0x0    },
	{0x30EA, 0x0    },
	{0x30EC, 0x5AE7 },
	{0x30F8, 0x33   },
	{0x30FA, 0xFC4C },
	{0x3120, 0x1    },
	{0x3122, 0x7    },
	{0x3124, 0x1A7  },
	{0x3126, 0x0    },
	{0x3128, 0x1CF  },
	{0x312A, 0x4567 },
	{0x312C, 0x89AB },
	{0x312E, 0xCDEF },
	{0x3152, 0x10   },
	{0x3154, 0x3200 },
	{0x3156, 0xC8F7 },
	{0x3158, 0x0    },
	{0x315A, 0x0    },
	{0x315C, 0x0    },
	{0x315E, 0x0    },
	{0x3160, 0xEC   },
	{0x3162, 0x317  },
	{0x3164, 0x0    },
	{0x0104, 0x0000 }, //Group_Parameter Hold
};

/* Initialize ARX3A0 Camera Sensor, also
 * initialize i2c using i3c driver instance */
int32_t ARX3A0_Init(ARM_CAMERA_RESOLUTION cam_resolution);

/* Un-initialize ARX3A0 Camera Sensor. */
int32_t ARX3A0_Uninit(void);

/* Start ARX3A0 Camera Sensor Streaming. */
int32_t ARX3A0_Start(void);

/* Stop ARX3A0 Camera Sensor Streaming. */
int32_t ARX3A0_Stop(void);

/* Control ARX3A0 Camera Sensor. */
int32_t ARX3A0_Control(uint32_t control, uint32_t arg);

/**
  \brief ARX3A0 Camera Sensor slave i2c Configuration
  \ref CAMERA_SENSOR_SLAVE_I2C_CONFIG
  */
CAMERA_SENSOR_SLAVE_I2C_CONFIG arx3A0_camera_sensor_i2c_cnfg =
{
	.I3Cx_instance                  = RTE_ARX3A0_CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE,
	.speed_mode                     = CAMERA_SENSOR_I2C_SPEED_SS_100_KBPS,
	.cam_sensor_slave_addr          = ARX3A0_CAMERA_SENSOR_SLAVE_ADDR,
	.cam_sensor_slave_reg_addr_type = CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT,
};

/**
  \brief ARX3A0 Camera Sensor Information
  \ref CAMERA_SENSOR_INFO
  */
CAMERA_SENSOR_INFO arx3a0_info =
{
	.output_format     = ARX3A0_USER_SELECT_CAMERA_OUTPUT_FORMAT,
	.additional_width  = 0,
	.additional_height = 0,
};

/**
  \brief ARX3A0 Camera Sensor Configurations
  \ref CAMERA_SENSOR_CONFIG
  */
CAMERA_SENSOR_CONFIG arx3a0_config =
{
	.interface    = CAMERA_SENSOR_INTERFACE_MIPI_CSI,
	.clock_source = RTE_ARX3A0_CAMERA_CLOCK_SOURCE,
	.pixelclk_pol = RTE_ARX3A0_CAMERA_SENSOR_PIXEL_CLK_POL,
	.hsync_pol    = RTE_ARX3A0_CAMERA_SENSOR_HSYNC_POL,
	.vsync_pol    = RTE_ARX3A0_CAMERA_SENSOR_VSYNC_POL,
	.hsync_mode   = RTE_ARX3A0_CAMERA_SENSOR_HSYNC_MODE,
	.data_mode    = RTE_ARX3A0_CAMERA_SENSOR_DATA_MODE,
	.data_mask    = RTE_ARX3A0_CAMERA_SENSOR_DATA_MASK,
};

/**
  \brief ARX3A0 Camera Sensor Operations
  \ref CAMERA_SENSOR_OPERATIONS
  */
CAMERA_SENSOR_OPERATIONS arx3a0_ops =
{
	.Init    = ARX3A0_Init,
	.Uninit  = ARX3A0_Uninit,
	.Start   = ARX3A0_Start,
	.Stop    = ARX3A0_Stop,
	.Control = ARX3A0_Control,
};

/**
  \brief ARX3A0 Camera Sensor Device Structure
Contains:
- ARX3A0 Camera Sensor Information
- ARX3A0 Camera Sensor Configurations
- ARX3A0 Camera Sensor Operations
\ref CAMERA_SENSOR_DEVICE
*/
extern CAMERA_SENSOR_DEVICE arx3a0_camera_sensor;
CAMERA_SENSOR_DEVICE arx3a0_camera_sensor =
{
	.Info    = &arx3a0_info,
	.Config  = &arx3a0_config,
	.Ops     = &arx3a0_ops,
};

#endif /* RTE_ARX3A0_CAMERA_SENSOR_ENABLE */

#endif /* ARX3A0_CAMERA_SENSOR_H_ */