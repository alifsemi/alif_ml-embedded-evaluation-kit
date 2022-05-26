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
 * @file     Driver_MIPI_DPHY.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     24-Feb-2022
 * @brief    Driver Specific Header file for MIPI DPHY Driver.
 ******************************************************************************/

#ifndef DRIVER_MIPI_DPHY_H_
#define DRIVER_MIPI_DPHY_H_

#include "Driver_Common.h"

#define ARM_MIPI_DPHY_API_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)

/**
\brief MIPI PHY physical lanes stop state status
*/
typedef enum {
	DPHY_STOPSTATE_LANE0 = (1U << 0),
	DPHY_STOPSTATE_LANE1 = (1U << 1),
	DPHY_STOPSTATE_CLOCK = (1U << 2),
}DPHY_STOPSTATE_Type;

/**
\brief MIPI PHY PLL lock status
*/
typedef enum {
	DPHY_NO_PLL_LOCK,
	DPHY_PLL_LOCK,
}DPHY_PLL_Status;

/**
\brief MIPI PHY RX-related signals
*/
typedef enum {
	DPHY_ULPM_LANE0     = (1U << 0),
	DPHY_ULPM_LANE1     = (1U << 1),
	DPHY_ULPM_CLOCK     = (1U << 2),
	DPHY_CLOCKACTIVE_HS = (1U << 3),
}DPHY_RX_Type;

/**
\brief MIPI PHY clock lane supports LP mode or not
*/
typedef enum {
	CONTINUOUS_CLOCK_MODE,
	NON_CONTINUOUS_CLOCK_MODE,
}DPHY_CLK_Mode;

/**
\brief MIPI PHY mode RX/TX
*/
typedef enum {
	DPHY_SLAVE,
	DPHY_MASTER,
}DPHY_Mode;

/**
\brief MIPI PHY number of data lanes
*/
typedef enum {
	DPHY_ONE_LANE = 1,
	DPHY_TWO_LANE,
}DPHY_N_Lanes;

/**
\brief DPHY controls required from CSI2/DSI interface
*/
typedef struct {
	void    (*MIPI_DPHY_shutdown)  (uint8_t state);
	void    (*MIPI_DPHY_rst)       (uint8_t state);
	void    (*MIPI_DPHY_enableclk) (uint8_t state);
	void    (*MIPI_DPHY_testclr)   (uint8_t state);
	uint8_t (*MIPI_DPHY_PLL_Lock)  (void);
	uint8_t (*MIPI_DPHY_rx)        (void);
	uint8_t (*MIPI_DPHY_stopstate) (void);
	uint8_t (*MIPI_DPHY_read)      (uint16_t address);
	void    (*MIPI_DPHY_write)     (uint16_t address, uint8_t data);
}ARM_DPHY_NEED_CTRL;

/**
\brief MIPI DPHY Driver Capabilities.
*/
typedef struct {
	uint32_t reentrant_operation         :1;
	uint32_t test_and_control_interface  :1;
	uint32_t internal_loopback           :1;
	uint32_t external_loopback           :1;
	uint32_t reserved                    :28;
}ARM_MIPI_DPHY_CAPABILITIES;

/**
\brief MIPI DPHY Device control
*/
typedef struct {
	ARM_DPHY_NEED_CTRL        *dphy_control_event;
	DPHY_Mode                 mode;
	DPHY_N_Lanes              n_lanes;
	DPHY_CLK_Mode             clock_mode;
	uint32_t                  clock_frequency;
}ARM_MIPI_DPHY_CFG;

/**
\brief Access structure of the MIPI DPHY Driver.
*/
typedef struct {
	ARM_DRIVER_VERSION                  (*GetVersion)      (void);
	ARM_MIPI_DPHY_CAPABILITIES          (*GetCapabilities) (void);
	int32_t                             (*Initialize)      (ARM_MIPI_DPHY_CFG *dphy_cfg);
	int32_t                             (*Uninitialize)    (void);
	int32_t                             (*PowerControl)    (ARM_POWER_STATE state);
	int32_t                             (*Setup)           (void);
}ARM_DRIVER_MIPI_DPHY;

#endif /* DPHY_DRIVER_MIPI_DPHY_H_ */
