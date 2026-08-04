/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
/*******************************************************************************
 * @file     OV5640_Camera_Sensor.c
 * @author   Chandra Bhushan Singh
 * @email    chandrabhushan.singh@alifsemi.com
 * @version  V1.0.0
 * @date     11-March-2026
 * @brief    Ovti OV5640 Camera Sensor driver.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* System Includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include "Camera_Sensor.h"
#include "Camera_Sensor_i2c.h"
#include "Driver_Common.h"
#include "Driver_CPI.h"
#include "sys_utils.h"
#include "sys_ctrl_cpi.h"
#include "Driver_IO.h"

#include "board_config.h"

#define CHIP_ID_REG 0x300a
#define CHIP_ID_VAL 0x5640

#define SYS_CTRL0_REG        0x3008
#define SYS_CTRL0_SW_PWDN    0x42
#define SYS_CTRL0_SW_PWUP    0x02
#define SYS_CTRL0_SW_RST     0x82

#define SYS_RESET00_REG      0x3000
#define SYS_RESET02_REG      0x3002
#define SYS_CLK_ENABLE00_REG 0x3004
#define SYS_CLK_ENABLE02_REG 0x3006
#define SYSTEM_CONTROL1_REG  0x302e
#define SCCB_SYS_CTRL1_REG   0x3103
#define TIMING_TC_REG20_REG  0x3820
#define TIMING_TC_REG21_REG  0x3821
#define HZ5060_CTRL01_REG    0x3c01
#define ISP_CTRL01_REG       0x5001

#define AEC_PK_REAL_GAIN     0x350a
#define AEC_PK_MANUAL        0x3503
#define AEC_CTRL00_REG       0x3a00
#define AEC_CTRL0F_REG       0x3a0f
#define AEC_CTRL10_REG       0x3a10
#define AEC_CTRL11_REG       0x3a11
#define AEC_CTRL1B_REG       0x3a1b
#define AEC_CTRL1E_REG       0x3a1e
#define AEC_CTRL1F_REG       0x3a1f

#define BLC_CTRL01_REG       0x4001
#define BLC_CTRL04_REG       0x4004
#define BLC_CTRL05_REG       0x4005

#define AWB_CTRL00_REG       0x5180
#define AWB_CTRL01_REG       0x5181
#define AWB_CTRL02_REG       0x5182
#define AWB_CTRL03_REG       0x5183
#define AWB_CTRL04_REG       0x5184
#define AWB_CTRL05_REG       0x5185
#define AWB_CTRL17_REG       0x5191
#define AWB_CTRL18_REG       0x5192
#define AWB_CTRL19_REG       0x5193
#define AWB_CTRL20_REG       0x5194
#define AWB_CTRL21_REG       0x5195
#define AWB_CTRL22_REG       0x5196
#define AWB_CTRL23_REG       0x5197
#define AWB_CTRL30_REG       0x519e

#define SDE_CTRL0_REG        0x5580
#define SDE_CTRL3_REG        0x5583
#define SDE_CTRL4_REG        0x5584
#define SDE_CTRL6_REG        0x5586
#define SDE_CTRL7_REG        0x5587
#define SDE_CTRL8_REG        0x5588
#define SDE_CTRL9_REG        0x5589
#define SDE_CTRL10_REG       0x558a
#define SDE_CTRL11_REG       0x558b

/* OV5640 Camera Sensor Slave Address. */
#define OV5640_PARALLEL_CAMERA_SENSOR_SLAVE_ADDR              0x3C

/* OV5640 Camera Sensor Camera Output Format Control registers */
#define OV5640_CAM_OUTPUT_FORMAT_REGISTER                     0x4300
#define OV5640_CAM_OUTPUT_MUX_REGISTER                        0x501f
#define OV5640_CAM_OUTPUT_FORMAT_MUX_RGB                      0x01
#define OV5640_CAM_OUTPUT_FORMAT_REGISTER_FORMAT_RGB          0x6f

/* Wrapper function for
 * Delay for millisecond:
 * Provide busy loop delay
 */
#define OV5640_DELAY_mSEC(msec)                               sys_busy_loop_us(msec * 1000)

/* OV5640 Camera reset GPIO port */
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(BOARD_LPCAM_RESET_GPIO_PORT);
static ARM_DRIVER_GPIO *GPIO_Driver_LPCAM_RST =
    &ARM_Driver_GPIO_(BOARD_LPCAM_RESET_GPIO_PORT);

/* OV5640 Camera power GPIO port */
extern ARM_DRIVER_GPIO  ARM_Driver_GPIO_(BOARD_LPCAM_POWER_GPIO_PORT);
static ARM_DRIVER_GPIO *GPIO_Driver_LPCAM_PWR =
    &ARM_Driver_GPIO_(BOARD_LPCAM_POWER_GPIO_PORT);

typedef struct _OV5640_REG {
    uint16_t reg_addr;  /* OV5640 Camera Sensor Register Address                     */
    uint32_t reg_value; /* OV5640 Camera Sensor Register Value                       */
    uint8_t  reg_size;  /* OV5640 Camera Sensor Register Size: only valid 1/2/4 Byte */
} OV5640_REG;

static const OV5640_REG init_params_common[] = {
    /* Power down */
    {SYS_CTRL0_REG, SYS_CTRL0_SW_PWDN, 1},

    /* System setting. */
    {SCCB_SYS_CTRL1_REG, 0x13, 1},
    {SCCB_SYS_CTRL1_REG, 0x03, 1},
    {SYS_RESET00_REG, 0x00, 1},
    {SYS_CLK_ENABLE00_REG, 0xff, 1},
    {SYS_RESET02_REG, 0x1c, 1},
    {SYS_CLK_ENABLE02_REG, 0xc3, 1},
    {SYSTEM_CONTROL1_REG, 0x08, 1},
    {0x3618, 0x00, 1},
    {0x3612, 0x29, 1},
    {0x3708, 0x64, 1},
    {0x3709, 0x52, 1},
    {0x370c, 0x03, 1},
    {TIMING_TC_REG20_REG, 0x41, 1},
    {TIMING_TC_REG21_REG, 0x07, 1},
    {0x3630, 0x36, 1},
    {0x3631, 0x0e, 1},
    {0x3632, 0xe2, 1},
    {0x3633, 0x12, 1},
    {0x3621, 0xe0, 1},
    {0x3704, 0xa0, 1},
    {0x3703, 0x5a, 1},
    {0x3715, 0x78, 1},
    {0x3717, 0x01, 1},
    {0x370b, 0x60, 1},
    {0x3705, 0x1a, 1},
    {0x3905, 0x02, 1},
    {0x3906, 0x10, 1},
    {0x3901, 0x0a, 1},
    {0x3731, 0x12, 1},
    {0x3600, 0x08, 1},
    {0x3601, 0x33, 1},
    {0x302d, 0x60, 1},
    {0x3620, 0x52, 1},
    {0x371b, 0x20, 1},
    {0x471c, 0x50, 1},
    {0x3a13, 0x43, 1},
    {0x3a18, 0x00, 1},
    {0x3a19, 0x7c, 1},
    {0x3635, 0x13, 1},
    {0x3636, 0x03, 1},
    {0x3634, 0x40, 1},
    {0x3622, 0x01, 1},
    {HZ5060_CTRL01_REG, 0x00, 1},
    {AEC_CTRL00_REG, 0x58, 1},
    {BLC_CTRL01_REG, 0x02, 1},
    {BLC_CTRL04_REG, 0x02, 1},
    {BLC_CTRL05_REG, 0x1a, 1},
    {ISP_CTRL01_REG, 0xa3, 1},

    /* AEC */
    {AEC_CTRL0F_REG, 0x30, 1},
    {AEC_CTRL10_REG, 0x28, 1},
    {AEC_CTRL1B_REG, 0x30, 1},
    {AEC_CTRL1E_REG, 0x26, 1},
    {AEC_CTRL11_REG, 0x60, 1},
    {AEC_CTRL1F_REG, 0x14, 1},

    /* AWB */
    {AWB_CTRL00_REG, 0xff, 1},
    {AWB_CTRL01_REG, 0xf2, 1},
    {AWB_CTRL02_REG, 0x00, 1},
    {AWB_CTRL03_REG, 0x14, 1},
    {AWB_CTRL04_REG, 0x25, 1},
    {AWB_CTRL05_REG, 0x24, 1},
    {0x5186, 0x09, 1},
    {0x5187, 0x09, 1},
    {0x5188, 0x09, 1},
    {0x5189, 0x88, 1},
    {0x518a, 0x54, 1},
    {0x518b, 0xee, 1},
    {0x518c, 0xb2, 1},
    {0x518d, 0x50, 1},
    {0x518e, 0x34, 1},
    {0x518f, 0x6b, 1},
    {0x5190, 0x46, 1},
    {AWB_CTRL17_REG, 0xf8, 1},
    {AWB_CTRL18_REG, 0x04, 1},
    {AWB_CTRL19_REG, 0x70, 1},
    {AWB_CTRL20_REG, 0xf0, 1},
    {AWB_CTRL21_REG, 0xf0, 1},
    {AWB_CTRL22_REG, 0x03, 1},
    {AWB_CTRL23_REG, 0x01, 1},
    {0x5198, 0x04, 1},
    {0x5199, 0x6c, 1},
    {0x519a, 0x04, 1},
    {0x519b, 0x00, 1},
    {0x519c, 0x09, 1},
    {0x519d, 0x2b, 1},
    {AWB_CTRL30_REG, 0x38, 1},

    /* Color Matrix */
    {0x5381, 0x1e, 1},
    {0x5382, 0x5b, 1},
    {0x5383, 0x08, 1},
    {0x5384, 0x0a, 1},
    {0x5385, 0x7e, 1},
    {0x5386, 0x88, 1},
    {0x5387, 0x7c, 1},
    {0x5388, 0x6c, 1},
    {0x5389, 0x10, 1},
    {0x538a, 0x01, 1},
    {0x538b, 0x98, 1},

    /* Sharp */
    {0x5300, 0x08, 1},
    {0x5301, 0x30, 1},
    {0x5302, 0x10, 1},
    {0x5303, 0x00, 1},
    {0x5304, 0x08, 1},
    {0x5305, 0x30, 1},
    {0x5306, 0x08, 1},
    {0x5307, 0x16, 1},
    {0x5309, 0x08, 1},
    {0x530a, 0x30, 1},
    {0x530b, 0x04, 1},
    {0x530c, 0x06, 1},

    /* Gamma */
    {0x5480, 0x01, 1},
    {0x5481, 0x08, 1},
    {0x5482, 0x14, 1},
    {0x5483, 0x28, 1},
    {0x5484, 0x51, 1},
    {0x5485, 0x65, 1},
    {0x5486, 0x71, 1},
    {0x5487, 0x7d, 1},
    {0x5488, 0x87, 1},
    {0x5489, 0x91, 1},
    {0x548a, 0x9a, 1},
    {0x548b, 0xaa, 1},
    {0x548c, 0xb8, 1},
    {0x548d, 0xcd, 1},
    {0x548e, 0xdd, 1},
    {0x548f, 0xea, 1},
    {0x5490, 0x1d, 1},

    /* UV adjust. */
    {SDE_CTRL0_REG, 0x02, 1},
    {SDE_CTRL3_REG, 0x40, 1},
    {SDE_CTRL4_REG, 0x10, 1},
    {SDE_CTRL9_REG, 0x10, 1},
    {SDE_CTRL10_REG, 0x00, 1},
    {SDE_CTRL11_REG, 0xf8, 1},

    /* Lens correction. */
    {0x5800, 0x23, 1},
    {0x5801, 0x14, 1},
    {0x5802, 0x0f, 1},
    {0x5803, 0x0f, 1},
    {0x5804, 0x12, 1},
    {0x5805, 0x26, 1},
    {0x5806, 0x0c, 1},
    {0x5807, 0x08, 1},
    {0x5808, 0x05, 1},
    {0x5809, 0x05, 1},
    {0x580a, 0x08, 1},
    {0x580b, 0x0d, 1},
    {0x580c, 0x08, 1},
    {0x580d, 0x03, 1},
    {0x580e, 0x00, 1},
    {0x580f, 0x00, 1},
    {0x5810, 0x03, 1},
    {0x5811, 0x09, 1},
    {0x5812, 0x07, 1},
    {0x5813, 0x03, 1},
    {0x5814, 0x00, 1},
    {0x5815, 0x01, 1},
    {0x5816, 0x03, 1},
    {0x5817, 0x08, 1},
    {0x5818, 0x0d, 1},
    {0x5819, 0x08, 1},
    {0x581a, 0x05, 1},
    {0x581b, 0x06, 1},
    {0x581c, 0x08, 1},
    {0x581d, 0x0e, 1},
    {0x581e, 0x29, 1},
    {0x581f, 0x17, 1},
    {0x5820, 0x11, 1},
    {0x5821, 0x11, 1},
    {0x5822, 0x15, 1},
    {0x5823, 0x28, 1},
    {0x5824, 0x46, 1},
    {0x5825, 0x26, 1},
    {0x5826, 0x08, 1},
    {0x5827, 0x26, 1},
    {0x5828, 0x64, 1},
    {0x5829, 0x26, 1},
    {0x582a, 0x24, 1},
    {0x582b, 0x22, 1},
    {0x582c, 0x24, 1},
    {0x582d, 0x24, 1},
    {0x582e, 0x06, 1},
    {0x582f, 0x22, 1},
    {0x5830, 0x40, 1},
    {0x5831, 0x42, 1},
    {0x5832, 0x24, 1},
    {0x5833, 0x26, 1},
    {0x5834, 0x24, 1},
    {0x5835, 0x22, 1},
    {0x5836, 0x22, 1},
    {0x5837, 0x26, 1},
    {0x5838, 0x44, 1},
    {0x5839, 0x24, 1},
    {0x583a, 0x26, 1},
    {0x583b, 0x28, 1},
    {0x583c, 0x42, 1},
    {0x583d, 0xce, 1},
    {0x5000, 0xa7, 1},
};

static const OV5640_REG init_params_dvp[] = {
    {0x4740, 0x21, 1},
    {0x4050, 0x6e, 1},
    {0x4051, 0x8f, 1},
    {0x3017, 0xff, 1},
    {0x3018, 0xff, 1},
    {0x302c, 0x02, 1},
    {0x3108, 0x01, 1},
    {0x3630, 0x2e, 1},
    {0x3a18, 0x00, 1},
    {0x3a19, 0xf8, 1},
    {0x3635, 0x1c, 1},
    {0x3c04, 0x28, 1},
    {0x3c05, 0x98, 1},
    {0x3c06, 0x00, 1},
    {0x3c07, 0x08, 1},
    {0x3c08, 0x00, 1},
    {0x3c09, 0x1c, 1},
    {0x3c0a, 0x9c, 1},
    {0x3c0b, 0x40, 1},
    {TIMING_TC_REG20_REG, 0x47, 1},
    {TIMING_TC_REG21_REG, 0x01, 1},
    {0x3800, 0x00, 1},
    {0x3801, 0x00, 1},
    {0x3802, 0x00, 1},
    {0x3803, 0x04, 1},
    {0x3804, 0x0a, 1},
    {0x3805, 0x3f, 1},
    {0x3806, 0x07, 1},
    {0x3807, 0x9b, 1},
    {0x3808, 0x05, 1},
    {0x3809, 0x00, 1},
    {0x380a, 0x03, 1},
    {0x380b, 0xc0, 1},
    {0x3810, 0x00, 1},
    {0x3811, 0x10, 1},
    {0x3812, 0x00, 1},
    {0x3813, 0x06, 1},
    {0x3814, 0x31, 1},
    {0x3815, 0x31, 1},
    {0x3034, 0x1a, 1},
    {0x3035, 0x01, 1},
    {0x3036, 0x64, 1},
    {0x3037, 0x11, 1},
    {0x3038, 0x00, 1},
    {0x3039, 0x00, 1},
    {0x380c, 0x07, 1},
    {0x380d, 0x68, 1},
    {0x380e, 0x03, 1},
    {0x380f, 0xd8, 1},
    {0x3c01, 0xb4, 1},
    {0x3c00, 0x04, 1},
    {0x3a08, 0x00, 1},
    {0x3a09, 0x93, 1},
    {0x3a0e, 0x06, 1},
    {0x3a0a, 0x00, 1},
    {0x3a0b, 0x7b, 1},
    {0x3a0d, 0x08, 1},
    {0x3a00, 0x38, 1},
    {0x3a02, 0x05, 1},
    {0x3a03, 0xc4, 1},
    {0x3a14, 0x05, 1},
    {0x3a15, 0xc4, 1},
    {0x300e, 0x58, 1},
    {0x302e, 0x00, 1},
    {0x4300, 0x30, 1},
    {0x501f, 0x00, 1},
    {0x4713, 0x04, 1},
    {0x4407, 0x04, 1},
    {0x460b, 0x35, 1},
    {0x460c, 0x22, 1},
    {0x3824, 0x01, 1},
    {0x3406, 0x01, 1},
    {0x3400, 0x06, 1},
    {0x3401, 0x80, 1},
    {0x3402, 0x04, 1},
    {0x3403, 0x00, 1},
    {0x3404, 0x06, 1},
    {0x3405, 0x00, 1},
    {0x5688, 0x22, 1},
    {0x5689, 0x22, 1},
    {0x568a, 0x42, 1},
    {0x568b, 0x24, 1},
    {0x568c, 0x42, 1},
    {0x568d, 0x24, 1},
    {0x568e, 0x22, 1},
    {0x568f, 0x22, 1},
    {0x5025, 0x00, 1},
    {0x3406, 0x00, 1},
    {0x3503, 0x01, 1},
    {0x3500, 0x00, 1},
    {0x3501, 0x25, 1},
    {0x3502, 0x80, 1},
    {0x3008, 0x02, 1},
    {0x3a02, 0x07, 1},
    {0x3a03, 0xae, 1},
    {0x3a08, 0x01, 1},
    {0x3a09, 0x27, 1},
    {0x3a0a, 0x00, 1},
    {0x3a0b, 0xf6, 1},
    {0x3a0e, 0x06, 1},
    {0x3a0d, 0x08, 1},
    {0x3a14, 0x07, 1},
    {0x3a15, 0xae, 1},
};

/* Initialization sequence for QQVGA resolution (160x120) */
static const OV5640_REG dvp_160x120_res_params[] = {
    {0x3800, 0x00, 1}, {0x3801, 0x08, 1}, {0x3802, 0x00, 1}, {0x3803, 0x02, 1}, {0x3804, 0x0a, 1},
    {0x3805, 0x37, 1}, {0x3806, 0x07, 1}, {0x3807, 0xa1, 1}, {0x3808, 0x00, 1}, {0x3809, 0xa0, 1},
    {0x380a, 0x00, 1}, {0x380b, 0x78, 1}, {0x380c, 0x06, 1}, {0x380d, 0x14, 1}, {0x380e, 0x03, 1},
    {0x380f, 0xe8, 1}, {0x3810, 0x00, 1}, {0x3811, 0x04, 1}, {0x3812, 0x00, 1}, {0x3813, 0x02, 1},
    {0x3814, 0x31, 1}, {0x3815, 0x31, 1}, {0x3820, 0x47, 1}, {0x3821, 0x01, 1}, {0x4602, 0x00, 1},
    {0x4603, 0xa0, 1}, {0x4604, 0x00, 1}, {0x4605, 0x78, 1}};

/* Initialization sequence for QVGA resolution (320x240).
 * Shares the same sensor windowing / sub-sampling as the 160x120 base table;
 * only the scaler output-size registers (0x3808-0x380b) and the DVP output-size
 * registers (0x4602-0x4605) differ, so the on-chip scaler produces 320x240.
 */
static const OV5640_REG dvp_320x240_res_params[] = {
    {0x3800, 0x00, 1}, {0x3801, 0x08, 1}, {0x3802, 0x00, 1}, {0x3803, 0x02, 1}, {0x3804, 0x0a, 1},
    {0x3805, 0x37, 1}, {0x3806, 0x07, 1}, {0x3807, 0xa1, 1}, {0x3808, 0x01, 1}, {0x3809, 0x40, 1},
    {0x380a, 0x00, 1}, {0x380b, 0xf0, 1}, {0x380c, 0x06, 1}, {0x380d, 0x14, 1}, {0x380e, 0x03, 1},
    {0x380f, 0xe8, 1}, {0x3810, 0x00, 1}, {0x3811, 0x04, 1}, {0x3812, 0x00, 1}, {0x3813, 0x02, 1},
    {0x3814, 0x31, 1}, {0x3815, 0x31, 1}, {0x3820, 0x47, 1}, {0x3821, 0x01, 1}, {0x4602, 0x01, 1},
    {0x4603, 0x40, 1}, {0x4604, 0x00, 1}, {0x4605, 0xf0, 1}};

/* Table of OV5640 DVP output resolutions supported by this driver. The
 * resolution applied at init is selected from the sensor's configured frame
 * width/height (see ov5640_set_format).
 */
typedef struct {
    uint16_t          width;    /* DVP output width in pixels  */
    uint16_t          height;   /* DVP output height in pixels */
    const OV5640_REG *regs;     /* Resolution register table   */
    uint32_t          num_regs; /* Number of register entries  */
} OV5640_RES_CONFIG;

static const OV5640_RES_CONFIG ov5640_res_configs[] = {
    {160, 120, dvp_160x120_res_params,
     sizeof(dvp_160x120_res_params) / sizeof(dvp_160x120_res_params[0])},
    {320, 240, dvp_320x240_res_params,
     sizeof(dvp_320x240_res_params) / sizeof(dvp_320x240_res_params[0])},
};

int32_t ov5640_modify_reg(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg, const uint16_t addr,
                          const uint8_t mask, const uint8_t val)
{
    uint32_t regVal = 0;
    int32_t ret = 0;

    ret = camera_sensor_i2c_read(i2c_cfg, addr, &regVal, 1);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    return camera_sensor_i2c_write(i2c_cfg, addr, (regVal & ~mask) | (val & mask), 1);
}

/*
 * \fn           int32_t ov5640_bulk_write_reg(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg,
 *                                              const OV5640_REG ov5640_reg[],
 *                                              uint32_t total_num)
 * \brief        write array of registers value to OV5640 Camera Sensor registers.
 * \param[in]    i2c_cfg  : Pointer to Camera Sensor i2c configuration.
 * \param[in]    ov5640_reg : OV5640 Camera Sensor Register Array Structure
 * \param[in]    total_num   : total number of registers(size of array)
 * \return       \ref execution_status
 */
static int32_t ov5640_bulk_write_reg(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg,
                                     const OV5640_REG ov5640_reg[],
                                     uint32_t total_num)
{
    uint32_t i   = 0;
    int32_t  ret = 0;

    for (i = 0; i < total_num; i++) {
        ret = camera_sensor_i2c_write(i2c_cfg,
                                      ov5640_reg[i].reg_addr,
                                      ov5640_reg[i].reg_value,
                                      ov5640_reg[i].reg_size);
        if (ret != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR;
        }
    }

    return ARM_DRIVER_OK;
}

/*
 * \fn           int32_t ov5640_power_on(void)
 * \brief        Power On OV5640 Camera Sensor.
 * \param[in]    none
 * \return       \ref execution_status
 */
int32_t ov5640_power_on(void)
{
    int32_t ret = 0;

    ret = GPIO_Driver_LPCAM_PWR->Initialize(BOARD_LPCAM_POWER_GPIO_PIN, NULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = GPIO_Driver_LPCAM_PWR->PowerControl(BOARD_LPCAM_POWER_GPIO_PIN,
                                            ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = GPIO_Driver_LPCAM_PWR->SetDirection(BOARD_LPCAM_POWER_GPIO_PIN,
                                            GPIO_PIN_DIRECTION_OUTPUT);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = GPIO_Driver_LPCAM_RST->Initialize(BOARD_LPCAM_RESET_GPIO_PIN, NULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = GPIO_Driver_LPCAM_RST->PowerControl(BOARD_LPCAM_RESET_GPIO_PIN,
                                            ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = GPIO_Driver_LPCAM_RST->SetDirection(BOARD_LPCAM_RESET_GPIO_PIN,
                                            GPIO_PIN_DIRECTION_OUTPUT);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = GPIO_Driver_LPCAM_PWR->SetValue(BOARD_LPCAM_POWER_GPIO_PIN,
                                        GPIO_PIN_OUTPUT_STATE_HIGH);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = GPIO_Driver_LPCAM_RST->SetValue(BOARD_LPCAM_RESET_GPIO_PIN,
                                        GPIO_PIN_OUTPUT_STATE_LOW);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    OV5640_DELAY_mSEC(5);

    ret = GPIO_Driver_LPCAM_PWR->SetValue(BOARD_LPCAM_POWER_GPIO_PIN,
                                        GPIO_PIN_OUTPUT_STATE_LOW);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    OV5640_DELAY_mSEC(1);

    ret = GPIO_Driver_LPCAM_RST->SetValue(BOARD_LPCAM_RESET_GPIO_PIN,
                                        GPIO_PIN_OUTPUT_STATE_HIGH);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    OV5640_DELAY_mSEC(20);

    return ARM_DRIVER_OK;
}

/*
 * \fn           int32_t ov5640_stream_start(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
 * \brief        Start 0V5640 Camera Sensor Streaming.
 * \param[in]    i2c_cfg  : Pointer to Camera Sensor i2c configuration
 * \return       \ref execution_status
 */
static int32_t ov5640_stream_start(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
{
    return camera_sensor_i2c_write(i2c_cfg, SYS_CTRL0_REG, SYS_CTRL0_SW_PWUP, 1);
}

/*
 * \fn           int32_t ov5640_stream_stop(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
 * \brief        Stop OV5640 Camera Sensor Streaming.
 * \param[in]    i2c_cfg  : Pointer to Camera Sensor i2c configuration
 * \return       \ref execution_status
 */
static int32_t ov5640_stream_stop(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
{
    return camera_sensor_i2c_write(i2c_cfg, SYS_CTRL0_REG, SYS_CTRL0_SW_PWDN, 1);
}

/*
 * \fn           int32_t ov5640_set_format(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg,
 *                                          int width, int height)
 * \brief        Set output resolution and pixel format for OV5640 Camera Sensor.
 * \param[in]    i2c_cfg  : Pointer to Camera Sensor i2c configuration.
 * \param[in]    width    : Requested DVP output width in pixels.
 * \param[in]    height   : Requested DVP output height in pixels.
 * \return       \ref execution_status
 */
int32_t ov5640_set_format(CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg, int width, int height)
{
    uint32_t ret;
    uint32_t total_num = 0;
    const OV5640_RES_CONFIG *res_cfg = NULL;

    /* Select the resolution register table matching the configured frame size. */
    for (uint32_t i = 0; i < (sizeof(ov5640_res_configs) / sizeof(ov5640_res_configs[0])); i++) {
        if ((ov5640_res_configs[i].width == width) && (ov5640_res_configs[i].height == height)) {
            res_cfg = &ov5640_res_configs[i];
            break;
        }
    }

    if (res_cfg == NULL) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Set resolution */
    total_num = res_cfg->num_regs;
    ret = ov5640_bulk_write_reg(i2c_cfg, res_cfg->regs, total_num);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    /* Set pixel format */
    ret = camera_sensor_i2c_write(i2c_cfg, OV5640_CAM_OUTPUT_FORMAT_REGISTER,
                                  OV5640_CAM_OUTPUT_FORMAT_REGISTER_FORMAT_RGB, 1);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    ret = camera_sensor_i2c_write(i2c_cfg, OV5640_CAM_OUTPUT_MUX_REGISTER,
                                  OV5640_CAM_OUTPUT_FORMAT_MUX_RGB, 1);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    ret = ov5640_modify_reg(i2c_cfg, TIMING_TC_REG21_REG, 0x20, 0x00);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    ret = ov5640_modify_reg(i2c_cfg, SYS_RESET02_REG, 0x1C, 0x1C);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    ret = ov5640_modify_reg(i2c_cfg, SYS_CLK_ENABLE02_REG, 0x28, 0x00);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/*
 * \fn           int32_t ov5640_Init(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
 *                                    CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
 * \brief        Initialize OV5640 Camera Sensor
 * \param[in]    cpi_ov5640_camera_sensor  : Pointer to camera sensor.
 * \param[in]    i2c_cfg  : Pointer to Camera Sensor i2c configuration.
 * \return       \ref execution_status
 */
int32_t ov5640_Init(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
                    CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
{
    uint16_t chip_id;
    uint32_t rcv_data = 0;
    uint32_t total_num = 0;
    uint16_t output_format = 0;
    int32_t ret = 0;

#ifdef RTE_OV5640_CAMERA_SENSOR_LPCPI_CLK_DIV
    set_lpcpi_pixel_clk(RTE_OV5640_CAMERA_SENSOR_LPCPI_CLK_DIV);
#endif

    /* Initialize i2c driver instance depending on
     *  OV5640 Camera Sensor specific i2c configurations
     *   \ref ov5640_camera_sensor_i2c_cnfg
     */
    ret = camera_sensor_i2c_init(i2c_cfg);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    ret = ov5640_power_on();
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    /* Reset all registers */
    ret = camera_sensor_i2c_write(i2c_cfg, SCCB_SYS_CTRL1_REG, 0x11, 1);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    /* Software reset */
    ret = camera_sensor_i2c_write(i2c_cfg, SYS_CTRL0_REG, SYS_CTRL0_SW_RST, 1);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    OV5640_DELAY_mSEC(5);

    /* Initialize register values */
    total_num = (sizeof(init_params_common) / sizeof((init_params_common)[0]));
    ret = ov5640_bulk_write_reg(i2c_cfg, init_params_common, total_num);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    total_num = (sizeof(init_params_dvp) / sizeof((init_params_dvp)[0]));
    ret = ov5640_bulk_write_reg(i2c_cfg, init_params_dvp, total_num);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    /* Check sensor chip id */
    ret = camera_sensor_i2c_read(i2c_cfg, CHIP_ID_REG, &rcv_data, 2);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    /* Proceed only if CHIP ID is correct. */
    if (rcv_data != CHIP_ID_VAL) {
        return ARM_DRIVER_ERROR;
    }

    /* Set pixel format. */
    ret = ov5640_set_format(i2c_cfg,
                            cpi_ov5640_camera_sensor->width,
                            cpi_ov5640_camera_sensor->height);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/*
 * \fn           int32_t ov5640_Start(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
 *                                     CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
 * \brief        Start OV5640 Camera Sensor Streaming.
 * \param[in]    cpi_ov5640_camera_sensor  : Poter to camera sensor.
 * \param[in]    i2c_cfg  : Pointer to Camera Sensor i2c configuration.
 * \return       \ref execution_status
 */
static int32_t ov5640_Start(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
                             CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
{
    int32_t ret = 0;

    ARG_UNUSED(cpi_ov5640_camera_sensor);
    ARG_UNUSED(i2c_cfg);

    /* Start streaming */
    ret = ov5640_stream_start(i2c_cfg);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/*
 * \fn           int32_t ov5640_Stop(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
 *                                    CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
 * \brief        Stop OV5640 Camera Sensor Streaming.
 * \param[in]    cpi_ov5640_camera_sensor  : Poter to camera sensor.
 * \param[in]    i2c_cfg  : Pointer to Camera Sensor i2c configuration.
 * \return       \ref execution_status
 */
static int32_t ov5640_Stop(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
                            CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
{
    int32_t ret = 0;

    ARG_UNUSED(cpi_ov5640_camera_sensor);
    ARG_UNUSED(i2c_cfg);

    /* Suspend any stream */
    ret = ov5640_stream_stop(i2c_cfg);
    if (ret != ARM_DRIVER_OK) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/*
 * \fn           int32_t ov5640_Control(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
 *                                       CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg,
 *                                       uint32_t control,
 *                                       uint32_t arg)
 * \brief        Control OV5640 Camera Sensor.
 * \param[in]    cpi_ov5640_camera_sensor  : Poter to camera sensor.
 * \param[in]    i2c_cfg  : Pointer to Camera Sensor i2c configuration.
 * \param[in]    control  : Operation
 * \param[in]    arg      : Argument of operation
 * \return       \ref execution_status
 */
static int32_t ov5640_Control(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
                               CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg,
                               uint32_t control,
                               uint32_t arg)
{
    ARG_UNUSED(cpi_ov5640_camera_sensor);
    ARG_UNUSED(i2c_cfg);
    ARG_UNUSED(control);
    ARG_UNUSED(arg);

    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/*
 * \fn           int32_t ov5640_Uninit(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
 *                                      CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
 * \brief        Un-initialize OV5640 Camera Sensor.
 * \param[in]    none
 * \return       \ref execution_status
 */
static int32_t ov5640_Uninit(CAMERA_SENSOR_DEVICE *cpi_ov5640_camera_sensor,
                             CAMERA_SENSOR_SLAVE_I2C_CONFIG *i2c_cfg)
{
    ARG_UNUSED(cpi_ov5640_camera_sensor);
    ARG_UNUSED(i2c_cfg);

#ifdef RTE_OV5640_CAMERA_SENSOR_LPCPI_CLK_DIV
    clear_lpcpi_pixel_clk();
#endif

    return ARM_DRIVER_OK;
}

#if RTE_OV5640_CAMERA_SENSOR_LPCPI_ENABLE

/* OV5640 LPCPI Initialize */
static int32_t ov5640_LPCPI_Init(void);

/* OV5640 LPCPI Un-initialize */
static int32_t ov5640_LPCPI_Uninit(void);

/* OV5640 LPCPI Start */
static int32_t ov5640_LPCPI_Start(void);

/* OV5640 LPCPI Stop */
static int32_t ov5640_LPCPI_Stop(void);

/* OV5640 LPCPI Control */
static int32_t ov5640_LPCPI_Control(uint32_t control, uint32_t arg);

/* I2C Instance */
#if (RTE_OV5640_CAMERA_SENSOR_LPCPI_I2C_INSTANCE == 4)
#define CAMERA_SENSOR_I2C_LPCPI_INSTANCE I3C
#else
#define CAMERA_SENSOR_I2C_LPCPI_INSTANCE BOARD_LPCAM_I2C_INSTANCE
#endif

/* I2C Driver Instance */
extern ARM_DRIVER_I2C ARM_Driver_I2C_(CAMERA_SENSOR_I2C_LPCPI_INSTANCE);

/*
 * \brief OV5640 Camera Sensor slave i2c Configuration
 * \ref CAMERA_SENSOR_SLAVE_I2C_CONFIG
 */
CAMERA_SENSOR_SLAVE_I2C_CONFIG lpcpi_ov5640_camera_sensor_i2c_cnfg = {
    .drv_i2c                        = &ARM_Driver_I2C_(CAMERA_SENSOR_I2C_LPCPI_INSTANCE),
    .bus_speed                      = ARM_I2C_BUS_SPEED_STANDARD,
    .cam_sensor_slave_addr          = OV5640_PARALLEL_CAMERA_SENSOR_SLAVE_ADDR,
    .cam_sensor_slave_reg_addr_type = CAMERA_SENSOR_I2C_REG_ADDR_TYPE_16BIT,
};

/*
 * \brief LPCPI OV5640 Camera Sensor Configurations
 * \ref CPI_INFO
 */
static CPI_INFO lpcpi_ov5640_config = {
    .pixelclk_pol    = RTE_OV5640_CAMERA_SENSOR_LPCPI_PIXEL_CLK_POL,
    .hsync_pol       = RTE_OV5640_CAMERA_SENSOR_LPCPI_HSYNC_POL,
    .vsync_pol       = RTE_OV5640_CAMERA_SENSOR_LPCPI_VSYNC_POL,
    .vsync_wait      = RTE_OV5640_CAMERA_SENSOR_LPCPI_VSYNC_WAIT,
    .vsync_mode      = RTE_OV5640_CAMERA_SENSOR_LPCPI_VSYNC_MODE,
    .data_mode       = RTE_OV5640_CAMERA_SENSOR_LPCPI_DATA_MODE,
    .data_endianness = RTE_OV5640_CAMERA_SENSOR_LPCPI_DATA_ENDIANNESS,
    .code10on8       = RTE_OV5640_CAMERA_SENSOR_LPCPI_CODE10ON8,
};

/*
 * \brief OV5640 Camera Sensor Operations
 * \ref CAMERA_SENSOR_OPERATIONS
 */
static CAMERA_SENSOR_OPERATIONS lpcpi_ov5640_ops = {
    .Init    = ov5640_LPCPI_Init,
    .Uninit  = ov5640_LPCPI_Uninit,
    .Start   = ov5640_LPCPI_Start,
    .Stop    = ov5640_LPCPI_Stop,
    .Control = ov5640_LPCPI_Control,
};

/*
 * \brief LPCPI OV5640 Camera Sensor Device Structure
 *  Contains:
 *    - LPCPI OV5640 Camera Sensor Configurations
 *    - OV5640 Camera Sensor Operations
 * \ref CAMERA_SENSOR_DEVICE
 */
static CAMERA_SENSOR_DEVICE lpcpi_ov5640_camera_sensor = {
    .interface = CAMERA_SENSOR_INTERFACE_PARALLEL,
    .width     = RTE_OV5640_CAMERA_SENSOR_LPCPI_FRAME_WIDTH,
    .height    = RTE_OV5640_CAMERA_SENSOR_LPCPI_FRAME_HEIGHT,
    .cpi_info  = &lpcpi_ov5640_config,
    .ops       = &lpcpi_ov5640_ops,
};

static int32_t ov5640_LPCPI_Init(void)
{
    return ov5640_Init(&lpcpi_ov5640_camera_sensor, &lpcpi_ov5640_camera_sensor_i2c_cnfg);
}

static int32_t ov5640_LPCPI_Uninit(void)
{
    return ov5640_Uninit(&lpcpi_ov5640_camera_sensor, &lpcpi_ov5640_camera_sensor_i2c_cnfg);
}

static int32_t ov5640_LPCPI_Start(void)
{
    return ov5640_Start(&lpcpi_ov5640_camera_sensor, &lpcpi_ov5640_camera_sensor_i2c_cnfg);
}

static int32_t ov5640_LPCPI_Stop(void)
{
    return ov5640_Stop(&lpcpi_ov5640_camera_sensor, &lpcpi_ov5640_camera_sensor_i2c_cnfg);
}

static int32_t ov5640_LPCPI_Control(uint32_t control, uint32_t arg)
{
    return ov5640_Control(&lpcpi_ov5640_camera_sensor,
                           &lpcpi_ov5640_camera_sensor_i2c_cnfg,
                           control,
                           arg);
}

/* Registering CPI sensor */
LPCAMERA_SENSOR(lpcpi_ov5640_camera_sensor)
#endif

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
