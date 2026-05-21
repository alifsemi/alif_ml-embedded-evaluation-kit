/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "isp_param.h"

#if RTE_ISP
#include "vsios_type.h"
#include "vsi_comm_isp.h"
#include "vsi_comm_sns.h"
#include "isp.h"
#include "mpi_isp_calib.h"
#include "vsi_comm_awb.h"

/* ---------------------------------------------------------------------------
 * ISP Calibration Data & Sensor Configuration
 * Sensor-specific calibration is pulled in via #include based on the
 * enabled camera sensor.  Each file defines `calibration_data`.
 * The CAMERA_SENSOR_* macros set the sensor native resolution and pixel
 * format used by port_attr below.
 * --------------------------------------------------------------------------- */
#if RTE_ARX3A0_CAMERA_SENSOR_CSI_ENABLE
#define CAMERA_SENSOR_FRAME_WIDTH   RTE_ARX3A0_CAMERA_SENSOR_FRAME_WIDTH
#define CAMERA_SENSOR_FRAME_HEIGHT  RTE_ARX3A0_CAMERA_SENSOR_FRAME_HEIGHT
#define CAMERA_SENSOR_PIXEL_FORMAT  PIXEL_FORMAT_GRBG8
#include "isp_calib_data_arx3a0.c"
#elif RTE_MT9M114_CAMERA_SENSOR_MIPI_ENABLE
#define CAMERA_SENSOR_FRAME_WIDTH   RTE_MT9M114_CAMERA_SENSOR_FRAME_WIDTH
#define CAMERA_SENSOR_FRAME_HEIGHT  RTE_MT9M114_CAMERA_SENSOR_FRAME_HEIGHT
#define CAMERA_SENSOR_PIXEL_FORMAT  PIXEL_FORMAT_GRBG8
#include "isp_calib_data_mt9m114.c"
#elif RTE_OV5675_CAMERA_SENSOR_ENABLE
#define CAMERA_SENSOR_FRAME_WIDTH   RTE_OV5675_CAMERA_SENSOR_FRAME_WIDTH
#define CAMERA_SENSOR_FRAME_HEIGHT  RTE_OV5675_CAMERA_SENSOR_FRAME_HEIGHT
#define CAMERA_SENSOR_PIXEL_FORMAT  PIXEL_FORMAT_GRBG8
#include "isp_calib_data_ov5675.c"
#else
#error "No supported ISP camera sensor enabled."
#endif

/* ---------------------------------------------------------------------------
 * ISP Port Attribute
 * Describes what is coming through the pipeline into the ISP IP.
 *
 * snsRect, inFormRect, iSRect: set to full sensor native resolution.
 *   Note: snsRect.width and snsRect.height are further overridden at runtime
 *   by the camera sensor driver's reported dimensions.
 *
 * outFormRect: set to full sensor dimensions (cropping disabled).
 * --------------------------------------------------------------------------- */

ISP_PORT_ATTR_S port_attr = {
    .ispInputType = INPUT_TYPE_SENSOR,
    .ispMode      = ISP_MODE_RAW,
    .hdrMode      = HDR_MODE_LINEAR,
    .pixelFormat  = CAMERA_SENSOR_PIXEL_FORMAT,
    .snsRect = {
        .top    = 0,
        .left   = 0,
        .width  = CAMERA_SENSOR_FRAME_WIDTH,
        .height = CAMERA_SENSOR_FRAME_HEIGHT,
    },
    .inFormRect = {
        .top    = 0,
        .left   = 0,
        .width  = CAMERA_SENSOR_FRAME_WIDTH,
        .height = CAMERA_SENSOR_FRAME_HEIGHT,
    },
    .iSRect = {
        .top    = 0,
        .left   = 0,
        .width  = CAMERA_SENSOR_FRAME_WIDTH,
        .height = CAMERA_SENSOR_FRAME_HEIGHT,
    },
    .outFormRect = {
        .top    = 0,
        .left   = 0,
        .width  = CAMERA_SENSOR_FRAME_WIDTH,
        .height = CAMERA_SENSOR_FRAME_HEIGHT,
    },
};

/* ---------------------------------------------------------------------------
 * ISP Channel Attribute
 * Describes the ISP output format sent to memory:
 * output width, height, and pixel format are taken from RTE_Device.h macros.
 * Set RTE_ISP_OUTPUT_WIDTH, RTE_ISP_OUTPUT_HEIGHT, RTE_ISP_OUTPUT_FORMAT
 * in RTE_Device.h to change the output resolution and format.
 * --------------------------------------------------------------------------- */
ISP_CHN_ATTR_S chan_attr = {
    .transBus = TRANS_BUS_ONLINE,
    .chnFormat = {
        .width       = RTE_ISP_OUTPUT_WIDTH,
        .height      = RTE_ISP_OUTPUT_HEIGHT,
        .pixelFormat = RTE_ISP_OUTPUT_FORMAT,
    },
};


void isp_param_set_crop(vsi_u32_t top, vsi_u32_t left, vsi_u32_t width, vsi_u32_t height)
{
    /* RECT_S field naming is swapped in the libisp:
     * RECT_S.top  -> ISP_OUT_H_OFFS (horizontal/left offset)
     * RECT_S.left -> ISP_OUT_V_OFFS (vertical/top offset)
     * We swap here so our API uses conventional image coordinates. */
    port_attr.outFormRect.top    = left;
    port_attr.outFormRect.left   = top;
    port_attr.outFormRect.width  = width;
    port_attr.outFormRect.height = height;
}


void isp_param_set_square_crop(void)
{
    vsi_u32_t side = CAMERA_SENSOR_FRAME_WIDTH < CAMERA_SENSOR_FRAME_HEIGHT
                   ? CAMERA_SENSOR_FRAME_WIDTH
                   : CAMERA_SENSOR_FRAME_HEIGHT;
    vsi_u32_t top  = (CAMERA_SENSOR_FRAME_HEIGHT - side) / 2;
    vsi_u32_t left = (CAMERA_SENSOR_FRAME_WIDTH - side) / 2;
    isp_param_set_crop(top, left, side, side);
}

void isp_param_set_output_dimensions(vsi_u32_t width, vsi_u32_t height)
{
    chan_attr.chnFormat.width  = width;
    chan_attr.chnFormat.height = height;
}

#endif /* RTE_ISP */