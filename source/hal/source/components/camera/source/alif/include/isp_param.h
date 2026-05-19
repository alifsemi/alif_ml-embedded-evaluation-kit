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
 * @file     isp_param.h
 * @author   Shivakumar Malke
 * @email    shivakumar.malke@alifsemi.com
 * @date     2026-04-15
 * @brief    Header file for ISP parameter configuration.
 *           This header declares the ISP calibration data, port attributes,
 *           and channel attributes that are defined in isp_param.c. These
 *           structures are included by Driver_ISP.c allowing application-level
 *           customization of ISP pipeline settings.
 ******************************************************************************/

#ifndef ISP_PARAM_H_
#define ISP_PARAM_H_

#include "RTE_Device.h"
#if RTE_ISP
#include "vsios_type.h"
#include "vsi_comm_isp.h"
#include "vsi_comm_sns.h"
#include "mpi_isp_calib.h"

/* ISP calibration data - contains WBM, WB, CCM, DMSC module settings.
 * Edit the definition in isp_param.c to change calibration values. */
extern ISP_CALIB_DATA_S calibration_data;

/* ISP port attribute - describes the input pipeline configuration.
 * Edit the definition in isp_param.c to match your sensor format. */
extern ISP_PORT_ATTR_S port_attr;

/* ISP channel attribute - describes the output format and bus configuration.
 * Edit the definition in isp_param.c to change output format. */
extern ISP_CHN_ATTR_S chan_attr;

/* Set the ISP output crop rectangle */
void isp_param_set_crop(vsi_u32_t top, vsi_u32_t left, vsi_u32_t width, vsi_u32_t height);

/* Set a centred square crop using the maximum square that fits the sensor. (helpful for keeping the aspect ratio) */
void isp_param_set_square_crop(void);

/* Set the ISP channel output dimensions (rescaling) the cropped area */
void isp_param_set_output_dimensions(vsi_u32_t width, vsi_u32_t height);
#endif /* RTE_ISP */

#endif /* ISP_PARAM_H_ */
