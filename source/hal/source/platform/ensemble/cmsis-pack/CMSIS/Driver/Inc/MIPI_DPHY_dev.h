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
 * @file     MIPI_DPHY_dev.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     24-Feb-2022
 * @brief    Device Specific Header file for MIPI DPHY Driver.
 ******************************************************************************/

#ifndef MIPI_DPHY_DEV_H_
#define MIPI_DPHY_DEV_H_

#include "Driver_MIPI_DPHY.h"

/*CFG_CLK frequency*/
#define DPHY_FCFG_CLOCK_MHZ                           25
/*Input Reference Clock Frequency*/
#define DPHY_FCLKIN_MHZ                               38.4
/*Charge-pump Programmability*/
#define DPHY_CPBIAS_CNTRL                             0x00
#define DPHY_GMP_CNTRL                                0x01
#define DPHY_INT_CNTRL                                0x04
#define DPHY_PROP_CNTRL                               0x10
#define DPHY_CB_VREF_MPLL_REG_REL_RW                  0x02
/*Oscillation target for slew rate calibration*/
#define DPHY_LESS_THEN_1GBPS_SR_OSC_FREQ_TARGET       657
#define DPHY_MORE_THEN_1GBPS_SR_OSC_FREQ_TARGET       920

/*DPHY TX register index*/
#define dphy4txtester_DIG_RD_TX_SYS_0                 (0x01EU)
#define dphy4txtester_DIG_RD_TX_SYS_1                 (0x01FU)
#define dphy4txtester_DIG_RDWR_TX_CB_0                (0x1AAU)
#define dphy4txtester_DIG_RDWR_TX_CB_1                (0x1ABU)
#define dphy4txtester_DIG_RDWR_TX_CB_2                (0x1ACU)
#define dphy4txtester_DIG_RDWR_TX_CB_3                (0x1ADU)
#define dphy4txtester_DIG_RDWR_TX_SLEW_5              (0x270U)
#define dphy4txtester_DIG_RDWR_TX_SLEW_6              (0x271U)
#define dphy4txtester_DIG_RDWR_TX_SLEW_7              (0x272U)
#define dphy4txtester_DIG_RDWR_TX_CLK_TERMLOWCAP      (0x402U)
#define dphy4txtester_DIG_RDWR_TX_LANE1_SLEWRATE_0    (0x70BU)
#define dphy4txtester_DIG_RDWR_TX_LANE2_SLEWRATE_0    (0x90BU)
#define dphy4txtester_DIG_RDWR_TX_LANE3_SLEWRATE_0    (0xB0BU)

/*TX PLL register index*/
#define dphy4txtester_DIG_RDWR_TX_PLL_1               (0x15EU)
#define dphy4txtester_DIG_RDWR_TX_PLL_5               (0x162U)
#define dphy4txtester_DIG_RDWR_TX_PLL_13              (0x16AU)
#define dphy4txtester_DIG_RDWR_TX_PLL_17              (0x16EU)
#define dphy4txtester_DIG_RDWR_TX_PLL_27              (0x178U)
#define dphy4txtester_DIG_RDWR_TX_PLL_28              (0x179U)
#define dphy4txtester_DIG_RDWR_TX_PLL_29              (0x17AU)
#define dphy4txtester_DIG_RDWR_TX_PLL_30              (0x17BU)
#define dphy4txtester_DIG_RD_TX_PLL_0                 (0x191U)

/*DPHY RX register index*/
#define dphy4rxtester_DIG_RD_RX_SYS_1                 (0x01FU)
#define dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_2    (0x0E2U)
#define dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_3    (0x0E3U)
#define dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_4    (0x0E4U)
#define dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_17   (0x0F1U)
#define dphy4rxtester_DIG_RDWR_RX_CLKLANE_LANE_6      (0x307U)
#define dphy4rxtester_DIG_RD_RX_CLKLANE_OFFSET_CAL_3  (0x39CU)
#define dphy2rxtester_DIG_RD_RX_CLKLANE_OFFSET_CAL_0  (0x39DU)
#define dphy2rxtester_DIG_RD_RX_LANE0_OFFSET_CAL_0    (0x58DU)
#define dphy2rxtester_DIG_RD_RX_LANE0_OFFSET_CAL_2    (0x5A1U)
#define dphy2rxtester_DIG_RD_RX_LANE0_DDL_0           (0x5E0U)
#define dphy2rxtester_DIG_RD_RX_LANE0_DDL_5           (0x5E5U)
#define dphy2rxtester_DIG_RD_RX_LANE1_OFFSET_CAL_0    (0x79FU)
#define dphy2rxtester_DIG_RD_RX_LANE1_OFFSET_CAL_2    (0x7A1U)
#define dphy4rxtester_DIG_RD_RX_LANE1_DDL_0           (0x7E0U)
#define dphy2rxtester_DIG_RD_RX_LANE1_DDL_5           (0x7E5U)

/*DPHY power and clock configuration parameters*/
#define MIPI_DPHY_PLL_LDO_EN_Pos                      10U
#define MIPI_DPHY_PLL_LDO_EN_Msk                      (1U << MIPI_DPHY_PLL_LDO_EN_Pos)

#define CLK_XTAL_SEL_Pos                              1U
#define CLK_XTAL_SEL_Msk                              (1U << CLK_XTAL_SEL_Pos)

#define XO_DIGITAL_CLOCK_Pos                          5U
#define XO_DIGITAL_CLOCK_Msk                          (1U << XO_DIGITAL_CLOCK_Pos)

/*MIPI DPHY helper macros*/
#define MIPI_DPHY_SET_BIT(REG, BIT_Msk)               SET_BIT(REG, BIT_Msk)
#define MIPI_DPHY_CLEAR_BIT(REG, BIT_Msk)             CLEAR_BIT(REG, BIT_Msk)
#define MIPI_DPHY_READ_BIT(REG, BIT_Msk)              READ_BIT(REG, BIT_Msk)
#define MIPI_DPHY_CLEAR_REG(REG)                      CLEAR_REG(REG)
#define MIPI_DPHY_WRITE_REG(REG, VAL)                 WRITE_REG(REG, VAL)
#define MIPI_DPHY_READ_REG(REG)                       READ_REG(REG)

/**
\brief hsfreqrange and osc_freq_target range
*/
typedef struct {
	uint32_t bitrate_in_mbps;
	uint8_t  hsfreqrange;
	uint32_t osc_freq_target;
}ARM_DPHY_FREQ_RANGE;

/**
\brief PLL vco_cntrl range
*/
typedef struct {
	float frequency_mhz;
	int8_t   vco_ctrl;
}ARM_DPHY_PLL_VCO_CTRL;

/**
\brief PLL Output division factor range
*/
typedef struct {
	float frequency_mhz;
	int8_t   p;
}ARM_DPHY_PLL_OUTPUT_DIVISION_FACTOR;

/**
\brief MIPI PHY Driver Flag status
*/
typedef enum {
	DPHY_FLAG_DRV_INIT_DONE         = (1U << 0),
	DPHY_FLAG_CLKSRC_ENABLED        = (1U << 1),
	DPHY_FLAG_DRV_SLAVE_SETUP_DONE  = (1U << 2),
	DPHY_FLAG_DRV_MASTER_SETUP_DONE = (1U << 3),
} DPHY_FLAG_Type;

/**
\brief MIPI DPHY Device control
*/
typedef struct {
	ARM_DPHY_NEED_CTRL        *dphy_control_event;
	void                      *ctrl_reg;
	DPHY_Mode                 mode;
	DPHY_N_Lanes              n_lanes;
	DPHY_CLK_Mode             clk_mode;
	uint8_t                   pll_N_factor;
	uint32_t                  clock_frequency;
	uint8_t                   flags;
}ARM_MIPI_DPHY_HANDLE;

#endif /* DPHY_MIPI_DPHY_DEV_H_ */
