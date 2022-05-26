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
 * @file     Driver_MIPI_DPHY.c
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     24-Feb-2022
 * @brief    CMSIS-Driver for MIPI DPHY.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#include "MIPI_DPHY_ctrl_regs.h"
#include "MIPI_DPHY_dev.h"
#include "Driver_MIPI_DPHY.h"
#include "RTE_Device.h"

#define ARM_MIPI_DPHY_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static const ARM_DRIVER_VERSION DriverVersion =
{
	ARM_MIPI_DPHY_API_VERSION,
	ARM_MIPI_DPHY_DRV_VERSION
};

#if  (RTE_MIPI_DPHY0)
static ARM_MIPI_DPHY_HANDLE DPHY0 =
{
	.clk_mode           = RTE_MIPI_DPHY0_NON_CONTINUOUS_CLOCK_MODE,
	.ctrl_reg           = MIPI_DPHY_CTRL_CFG_BASE,
	.pll_N_factor       = NULL,
};
#endif

#if (RTE_MIPI_DPHY1)
static ARM_MIPI_DPHY_HANDLE DPHY1 =
{
	.clk_mode           = RTE_MIPI_DPHY1_NON_CONTINUOUS_CLOCK_MODE,
	.ctrl_reg           = MIPI_DPHY_CTRL_CFG_BASE,
	.pll_N_factor       = RTE_MIPI_DPHY1_PLL_INPUT_DIV_FACTOR_N,
};
#endif

/*hsfreqrange and osc_freq_target range*/
static const ARM_DPHY_FREQ_RANGE frequency_range[] =
{
	{ 80, 0x00, 0x1B6 }, { 90, 0x10, 0x1B6 }, { 100, 0x20, 0x1B6 },
	{ 110, 0x30, 0x1B6 }, { 120, 0x01, 0x1B6 }, { 130, 0x11, 0x1B6 },
	{ 140, 0x21, 0x1B6 }, { 150, 0x31, 0x1B6 }, { 160, 0x02, 0x1B6 },
	{ 170, 0x12, 0x1B6 }, { 180, 0x22, 0x1B6 }, { 190, 0x32, 0x1B6 },
	{ 205, 0x03, 0x1B6 }, { 220, 0x13, 0x1B6 }, { 235, 0x23, 0x1B6 },
	{ 250, 0x33, 0x1B6 }, { 275, 0x04, 0x1B6 }, { 300, 0x14, 0x1B6 },
	{ 325, 0x25, 0x1B6 }, { 350, 0x35, 0x1B6 }, { 400, 0x05, 0x1B6 },
	{ 450, 0x16, 0x1B6 }, { 500, 0x26, 0x1B6 }, { 550, 0x37, 0x1B6 },
	{ 600, 0x07, 0x1B6 }, { 650, 0x18, 0x1B6 }, { 700, 0x28, 0x1B6 },
	{ 750, 0x39, 0x1B6 }, { 800, 0x09, 0x1B6 }, { 850, 0x19, 0x1B6 },
	{ 900, 0x29, 0x1B6 }, { 950, 0x3A, 0x1B6 }, { 1000, 0x0A, 0x1B6 },
	{ 1050, 0x1A, 0x1B6 }, { 1100, 0x2A, 0x1B6 }, { 1150, 0x3B, 0x1B6 },
	{ 1200, 0x0B, 0x1B6 }, { 1250, 0x1B, 0x1B6 }, { 1300, 0x2B, 0x1B6 },
	{ 1350, 0x3C, 0x1B6 }, { 1400, 0x0C, 0x1B6 }, { 1450, 0x1C, 0x1B6 },
	{ 1500, 0x2C, 0x1B6 }, { 1550, 0x3D, 0x10F }, { 1600, 0x0D, 0x118 },
	{ 1650, 0x1D, 0x121 }, { 1700, 0x2E, 0x12A }, { 1750, 0x3E, 0x132 },
	{ 1800, 0x0E, 0x13B }, { 1850, 0x1E, 0x144 }, { 1900, 0x2F, 0x14D },
	{ 1950, 0x3F, 0x155 }, { 2000, 0x0F, 0x15E }, { 2050, 0x40, 0x167 },
	{ 2100, 0x41, 0x170 }, { 2150, 0x42, 0x178 }, { 2200, 0x43, 0x181 },
	{ 2250, 0x44, 0x18A }, { 2300, 0x45, 0x193 }, { 2350, 0x46, 0x19B },
	{ 2400, 0x47, 0x1A4 }, { 2450, 0x48, 0x1AD }, { 2500, 0x49, 0x1B6 }
};

/*vco_cntrl range*/
static const ARM_DPHY_PLL_VCO_CTRL vco_ctrl_range[] =
{
	{ 1170, 0x03 }, { 975, 0x07 }, { 853.125, 0x08 }, { 706.875, 0x08 },
	{ 585, 0x0B }, { 487.5, 0x0F }, { 426.56, 0x10 }, { 353.4, 0x10 },
	{ 292.5, 0x13 }, { 243.75, 0x17 }, { 213.3, 0x18 }, { 176.72, 0x18 },
	{ 146.25, 0x1B }, { 121.88, 0x1F }, { 106.64, 0x20 }, { 88.36, 0x20 },
	{ 73.13, 0x23}, { 60.93, 0x27 }, { 53.32, 0x28 }, { 44.18, 0x28 },
	{ 40, 0x2B}
};

/*Output division factor range*/
static const ARM_DPHY_PLL_OUTPUT_DIVISION_FACTOR pll_p_factor[] =
{
	{ 1000, 2 }, { 500, 4 }, { 250, 8 }, { 125, 16 }, { 62.5, 32 }, { 40, 64 }
};

/* Driver Capabilities */
static const ARM_MIPI_DPHY_CAPABILITIES DriverCapabilities =
{
	0, /* Not supports  reentrant_operation */
	1, /* configure via test and control interface*/
	0, /* supports internal loopback (need to implement)*/
	0, /* supports external loopback (need to implement)*/
	0  /* reserved (must be zero) */
};

//
//  Functions
//

/**
  \fn          ARM_DRIVER_VERSION DPHY_GetVersion (void)
  \brief       Get MIPI DPHY driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION DPHY_GetVersion (void)
{
	return DriverVersion;
}

/**
  \fn          ARM_MIPI_DPHY_CAPABILITIES DPHY_GetCapabilities (void)
  \brief       Get MIPI DPHY driver capabilities
  \return      \ref ARM_MIPI_DPHY_CAPABILITIES
*/
static ARM_MIPI_DPHY_CAPABILITIES DPHY_GetCapabilities (void)
{
	return DriverCapabilities;
}

/**
  \fn          int32_t DPHY_Initialize (ARM_MIPI_DPHY_CFG *dphy_cfg,
                                        ARM_MIPI_DPHY_HANDLE *dphy)
  \brief       Initialize MIPI DPHY Interface.
  \param[in]   dphy_cfg Pointer to configure DPHY resource.
  \param[in]   dphy Pointer to DPHY resources
  \return      \ref execution_status
*/
static int32_t DPHY_Initialize (ARM_MIPI_DPHY_CFG *dphy_cfg,
                                ARM_MIPI_DPHY_HANDLE *dphy)
{

	if(dphy_cfg == NULL)
		return ARM_DRIVER_ERROR_PARAMETER;

	if(dphy->flags & DPHY_FLAG_DRV_INIT_DONE)
		return ARM_DRIVER_OK;

	dphy->dphy_control_event = dphy_cfg->dphy_control_event;
	dphy->mode = dphy_cfg->mode;
	dphy->n_lanes = dphy_cfg->n_lanes;
	dphy->clock_frequency = dphy_cfg->clock_frequency;

	dphy->flags = DPHY_FLAG_DRV_INIT_DONE;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DPHY_Uninitialize (ARM_MIPI_DPHY_HANDLE *dphy)
  \brief       uninitialize MIPI DPHY Interface.
  \param[in]   dphy Pointer to DPHY resources
  \return      \ref execution_status
*/
static int32_t DPHY_Uninitialize (ARM_MIPI_DPHY_HANDLE *dphy)
{
	dphy->dphy_control_event = NULL;
	dphy->clock_frequency = 0;
	dphy->mode = 0;
	dphy->n_lanes = 0;
	dphy->flags = 0;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DPHY_PowerControl (ARM_POWER_STATE state, ARM_MIPI_DPHY_HANDLE *dphy)
  \brief       Control DPHY Interface Power.
  \param[in]   state  Power state
  \param[in]   dphy  Pointer to DPHY resources
  \return      \ref execution_status
*/
static int32_t DPHY_PowerControl (ARM_POWER_STATE state, ARM_MIPI_DPHY_HANDLE *dphy)
{
	volatile uint32_t *reg = NULL;

	switch (state)
	{
		case ARM_POWER_OFF:

			reg = (uint32_t *)VBAT_ANA_REG2_BASE;
			MIPI_DPHY_CLEAR_BIT(*reg, MIPI_DPHY_PLL_LDO_EN_Msk);

			reg = (uint32_t *)LPAON_CLK_RST_4_BASE;
			MIPI_DPHY_CLEAR_BIT(*reg, CLK_XTAL_SEL_Msk);

			reg = (uint32_t *)LPAON_ANA_XTAL_OSCILLATOR_REG_BASE;
			MIPI_DPHY_CLEAR_BIT(*reg, XO_DIGITAL_CLOCK_Msk);

			dphy->flags &= ~DPHY_FLAG_CLKSRC_ENABLED;
			break;

		case ARM_POWER_FULL:

			if (!(dphy->flags & DPHY_FLAG_DRV_INIT_DONE))
					return ARM_DRIVER_ERROR;
			if (dphy->flags & DPHY_FLAG_CLKSRC_ENABLED)
					return ARM_DRIVER_OK;

			reg = (uint32_t *)LPAON_ANA_XTAL_OSCILLATOR_REG_BASE;
			MIPI_DPHY_SET_BIT(*reg, XO_DIGITAL_CLOCK_Msk);

			reg = (uint32_t *)LPAON_CLK_RST_4_BASE;
			MIPI_DPHY_SET_BIT(*reg, CLK_XTAL_SEL_Msk);

			reg = (uint32_t *)VBAT_ANA_REG2_BASE;
			MIPI_DPHY_SET_BIT(*reg, MIPI_DPHY_PLL_LDO_EN_Msk);

			dphy->flags |= DPHY_FLAG_CLKSRC_ENABLED;
			break;

		case ARM_POWER_LOW:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ARM_DRIVER_OK;
}

/**
  \fn          void DPHY_Write_Mask (uint16_t address,
                                     uint8_t  data,
                                     uint8_t  pos,
                                     uint8_t  width,
                                     ARM_MIPI_DPHY_HANDLE *dphy)
  \brief       write Mask DPHY registers.
  \param[in]   address is register index
  \param[in]   data is value to be write to the DPHY register.
  \param[in]   pos  is start bit position.
  \param[in]   width is number bits to write.
  \param[in]   dphy  Pointer to DPHY resources
*/
static void DPHY_Write_Mask (uint16_t address,
                             uint8_t  data,
                             uint8_t  pos,
                             uint8_t  width,
                             ARM_MIPI_DPHY_HANDLE *dphy)
{
	uint8_t reg_data = 0;
	uint8_t mask = (1U << width) - 1;

	ARM_DPHY_NEED_CTRL *dphy_ctrl = (ARM_DPHY_NEED_CTRL *)dphy->dphy_control_event;
	reg_data = dphy_ctrl->MIPI_DPHY_read(address);
	reg_data &= ~(mask << pos);
	reg_data |= (data & mask) << pos;
	dphy_ctrl->MIPI_DPHY_write(address,reg_data);
}

/**
  \fn          int32_t DPHY_ConfigurePLL(ARM_MIPI_DPHY_HANDLE *dphy)
  \brief       configuring MIPI TX DPHY PLL.
  \param[in]   dphy Pointer to DPHY resources
  \return      \ref execution_status
*/
static int32_t DPHY_ConfigurePLL(ARM_MIPI_DPHY_HANDLE *dphy)
{
	MIPI_DPHY_ctrl_cfg_Type *ctrl_reg = (MIPI_DPHY_ctrl_cfg_Type *)dphy->ctrl_reg;
	float frequency_in_mhz = dphy->clock_frequency/1000000;
	uint8_t pll_n = dphy->pll_N_factor;
	uint32_t pll_m = 0;
	uint8_t pll_p = 0;
	uint8_t vco_ctrl = 0;
	uint32_t reg_data = 0;
	uint8_t range = 0;

	if(((DPHY_FCLKIN_MHZ/pll_n) > 24) || ((DPHY_FCLKIN_MHZ/pll_n) < 8))
		return ARM_DRIVER_ERROR_PARAMETER;

	for( range = 0; (range < ARRAY_SIZE(vco_ctrl_range) - 1) &&
		((dphy->clock_frequency/1000000) < vco_ctrl_range[range].frequency_mhz);
		++range);

	vco_ctrl = vco_ctrl_range[range].vco_ctrl;

	for( range = 0; (range < ARRAY_SIZE(pll_p_factor) - 1) &&
	((dphy->clock_frequency/1000000) <= pll_p_factor[range].frequency_mhz);
	++range);

	pll_p = pll_p_factor[range].p;

	pll_m = ((frequency_in_mhz * pll_n * pll_p * 2) / DPHY_FCLKIN_MHZ);

	MIPI_DPHY_CTRL_CFG_SET_BIT(ctrl_reg->dphy_pll_ctrl0, PLL_SOC_SHADOW_CONTROL_Msk);

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->dphy_pll_ctrl0);
	reg_data &= ~(PLL_SOC_CLKSEL_Msk);
	reg_data |= _VAL2FLD(PLL_SOC_CLKSEL,1);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->dphy_pll_ctrl0, reg_data);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_28, (uint8_t)pll_m, 0, 8, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_29, (uint8_t)(pll_m >> 8), 0, 2, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_30, 0x1, 0, 1, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_27, pll_n, 3, 4, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_27, 0x1, 7, 1, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_30, vco_ctrl, 1, 6, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_30, 0x1, 7, 1, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_1, DPHY_CPBIAS_CNTRL, 0, 7, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_5, DPHY_GMP_CNTRL, 0, 2, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_5, DPHY_INT_CNTRL, 2, 6, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_17, DPHY_PROP_CNTRL, 0, 6, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_17, 0x1, 7, 1, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_17, 0x1, 6, 1, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_CB_3, DPHY_CB_VREF_MPLL_REG_REL_RW, 0, 3, dphy);

	return ARM_DRIVER_OK;

}

/**
  \fn          int32_t DPHY_MasterSetup (ARM_MIPI_DPHY_HANDLE *dphy)
  \brief       MIPI DPHY Tx startup sequence.
  \param[in]   dphy Pointer to DPHY resources
  \return      \ref execution_status
*/
static int32_t DPHY_MasterSetup (ARM_MIPI_DPHY_HANDLE *dphy)
{
	ARM_DPHY_NEED_CTRL *dphy_ctrl = (ARM_DPHY_NEED_CTRL *)dphy->dphy_control_event;
	MIPI_DPHY_ctrl_cfg_Type *ctrl_reg = (MIPI_DPHY_ctrl_cfg_Type *)dphy->ctrl_reg;
	uint32_t bitrate_mbps = (dphy->clock_frequency * 2)/1000000;
	uint8_t hsfreqrange = 0;
	uint8_t cfgclkfreqrange = 0;
	uint32_t osc_freq_target = 0;
	uint8_t range = 0;
	uint32_t reg_data = 0;
	uint8_t stopstate_check = 0;
	uint32_t lp_count = 0;

	for(range = 0; (range < ARRAY_SIZE(frequency_range) - 1) &&
		((bitrate_mbps) > frequency_range[range].bitrate_in_mbps);
		++range);

	hsfreqrange = frequency_range[range].hsfreqrange;

	dphy_ctrl->MIPI_DPHY_rst(DISABLE);

	dphy_ctrl->MIPI_DPHY_shutdown(DISABLE);

	MIPI_DPHY_CTRL_CFG_SET_BIT(ctrl_reg->tx_dphy_ctrl0, TXRXZ_Msk);

	MIPI_DPHY_CTRL_CFG_SET_BIT(ctrl_reg->tx_dphy_ctrl0, TESTPORT_SEL_Msk);
	dphy_ctrl->MIPI_DPHY_testclr(ENABLE);
	MIPI_DPHY_CTRL_CFG_CLEAR_BIT(ctrl_reg->tx_dphy_ctrl0, TESTPORT_SEL_Msk);
	dphy_ctrl->MIPI_DPHY_testclr(ENABLE);

	PMU_delay_loop_us(1);

	MIPI_DPHY_CTRL_CFG_SET_BIT(ctrl_reg->tx_dphy_ctrl0, TESTPORT_SEL_Msk);
	dphy_ctrl->MIPI_DPHY_testclr(DISABLE);
	MIPI_DPHY_CTRL_CFG_CLEAR_BIT(ctrl_reg->tx_dphy_ctrl0, TESTPORT_SEL_Msk);
	dphy_ctrl->MIPI_DPHY_testclr(DISABLE);


	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->tx_dphy_ctrl0);
	reg_data &= ~(HSFREQRANGE_Msk);
	reg_data |= _VAL2FLD(HSFREQRANGE,hsfreqrange);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->tx_dphy_ctrl0, reg_data);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_13, 0x3, 0, 2, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_CB_1, 0x2, 0, 2, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_CB_0, 0x2, 5, 2, dphy);

	if(bitrate_mbps < 450)
		DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_CB_2, 0x1, 4, 1, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_CLK_TERMLOWCAP, 0x2, 0, 2, dphy);

	if(bitrate_mbps <= 1000)
	{
		DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_SLEW_5,
                               (uint8_t)DPHY_LESS_THEN_1GBPS_SR_OSC_FREQ_TARGET,
                               0, 8, dphy);
		DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_SLEW_6,
                               (uint8_t)(DPHY_LESS_THEN_1GBPS_SR_OSC_FREQ_TARGET >> 8),
                               0, 4, dphy);
		DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_SLEW_7, 0x1, 4, 1, dphy);
		DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_SLEW_7, 0x1, 0, 1, dphy);
	}
	else if ((bitrate_mbps > 1000) && (bitrate_mbps <= 1500))
	{
		DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_SLEW_5,
                               (uint8_t)DPHY_MORE_THEN_1GBPS_SR_OSC_FREQ_TARGET,
                               0, 8, dphy);
		DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_SLEW_6,
                               (uint8_t)(DPHY_MORE_THEN_1GBPS_SR_OSC_FREQ_TARGET >> 8),
                               0, 4, dphy);
	}

	cfgclkfreqrange = (DPHY_FCFG_CLOCK_MHZ - 17) * 4;

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->tx_dphy_ctrl0);
	reg_data &= ~(CFGCLKFREQRANGE_Msk);
	reg_data |= _VAL2FLD(CFGCLKFREQRANGE, cfgclkfreqrange);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->tx_dphy_ctrl0, reg_data);

	if(DPHY_ConfigurePLL(dphy) != ARM_DRIVER_OK)
		return ARM_DRIVER_ERROR_PARAMETER;

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->tx_dphy_ctrl0);
	reg_data &= ~(BASEDIR_Msk);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->tx_dphy_ctrl0, reg_data);

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->tx_dphy_ctrl1);
	reg_data &= ~(FORCERXMODE_Msk);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->tx_dphy_ctrl1, reg_data);

	PMU_delay_loop_us(1);

	dphy_ctrl->MIPI_DPHY_enableclk(ENABLE);

	PMU_delay_loop_us(1);

	dphy_ctrl->MIPI_DPHY_shutdown(ENABLE);

	PMU_delay_loop_us(1);

	dphy_ctrl->MIPI_DPHY_rst(ENABLE);


	while(dphy_ctrl->MIPI_DPHY_PLL_Lock() != DPHY_PLL_LOCK)
	{
		if(lp_count++ < 1000)
			PMU_delay_loop_us(1);
		else
			return ARM_DRIVER_ERROR;
	}


	if(dphy->clk_mode == NON_CONTINUOUS_CLOCK_MODE)
		stopstate_check |= DPHY_STOPSTATE_CLOCK;

	stopstate_check |= (dphy->n_lanes == 1 ? (DPHY_STOPSTATE_LANE0) :
                                             (DPHY_STOPSTATE_LANE0) | (DPHY_STOPSTATE_LANE1) );

	lp_count = 0;
	while(dphy_ctrl->MIPI_DPHY_stopstate() != stopstate_check)
	{
		if(lp_count++ < 1000)
			PMU_delay_loop_us(1);
		else
			return ARM_DRIVER_ERROR;
	}

	dphy->flags |= DPHY_FLAG_DRV_MASTER_SETUP_DONE;

	return ARM_DRIVER_OK;

}

/**
  \fn          int32_t DPHY_SlaveSetup (ARM_MIPI_DPHY_HANDLE *dphy)
  \brief       MIPI DPHY Rx startup sequence.
  \param[in]   dphy Pointer to DPHY resources
  \return      \ref execution_status
*/
static int32_t DPHY_SlaveSetup (ARM_MIPI_DPHY_HANDLE *dphy)
{
	ARM_DPHY_NEED_CTRL *dphy_ctrl = (ARM_DPHY_NEED_CTRL *)dphy->dphy_control_event;
	MIPI_DPHY_ctrl_cfg_Type *ctrl_reg = (MIPI_DPHY_ctrl_cfg_Type *)dphy->ctrl_reg;
	uint32_t bitrate = (dphy->clock_frequency * 2);
	uint8_t hsfreqrange = 0;
	uint8_t cfgclkfreqrange = 0;
	uint32_t osc_freq_target = 0;
	uint8_t range = 0;
	uint32_t reg_data = 0;
	uint8_t stopstate_check =0;
	uint32_t lp_count = 0;

	for(range = 0; (range < ARRAY_SIZE(frequency_range) - 1) &&
		((bitrate/1000000) > frequency_range[range].bitrate_in_mbps);
		++range);

	hsfreqrange = frequency_range[range].hsfreqrange;
	osc_freq_target = frequency_range[range].osc_freq_target;

	dphy_ctrl->MIPI_DPHY_rst(DISABLE);

	dphy_ctrl->MIPI_DPHY_shutdown(DISABLE);

	MIPI_DPHY_CTRL_CFG_CLEAR_BIT(ctrl_reg->rx_dphy_ctrl0, TXRXZ_Msk);

	MIPI_DPHY_CTRL_CFG_SET_BIT(ctrl_reg->rx_dphy_ctrl0, TESTPORT_SEL_Msk);
	dphy_ctrl->MIPI_DPHY_testclr(ENABLE);
	MIPI_DPHY_CTRL_CFG_CLEAR_BIT(ctrl_reg->rx_dphy_ctrl0, TESTPORT_SEL_Msk);
	dphy_ctrl->MIPI_DPHY_testclr(ENABLE);

	PMU_delay_loop_us(1);

	MIPI_DPHY_CTRL_CFG_SET_BIT(ctrl_reg->rx_dphy_ctrl0, TESTPORT_SEL_Msk);
	dphy_ctrl->MIPI_DPHY_testclr(DISABLE);
	MIPI_DPHY_CTRL_CFG_CLEAR_BIT(ctrl_reg->rx_dphy_ctrl0, TESTPORT_SEL_Msk);
	dphy_ctrl->MIPI_DPHY_testclr(DISABLE);

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->rx_dphy_ctrl0);
	reg_data &= ~(HSFREQRANGE_Msk);
	reg_data |= _VAL2FLD(HSFREQRANGE, hsfreqrange);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->rx_dphy_ctrl0, reg_data);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_PLL_13, 0x3, 0, 2, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_CB_1, 0x2, 0, 2, dphy);

	DPHY_Write_Mask(dphy4txtester_DIG_RDWR_TX_CB_0, 0x2, 5, 2, dphy);

	MIPI_DPHY_CTRL_CFG_SET_BIT(ctrl_reg->rx_dphy_ctrl0, TESTPORT_SEL_Msk);

	DPHY_Write_Mask(dphy4rxtester_DIG_RDWR_RX_CLKLANE_LANE_6, 0x1, 7, 1, dphy);

	if((bitrate/1000000) == 80)
		DPHY_Write_Mask(dphy4rxtester_DIG_RD_RX_SYS_1, 0x85, 0, 8, dphy);

	DPHY_Write_Mask(dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_2, (uint8_t)osc_freq_target, 0, 8, dphy);

	DPHY_Write_Mask(dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_3, (uint8_t)(osc_freq_target >> 8), 0, 4, dphy);

	DPHY_Write_Mask(dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_4, 0x1, 0, 1, dphy);

	cfgclkfreqrange = (DPHY_FCFG_CLOCK_MHZ - 17) * 4;

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->rx_dphy_ctrl0);
	reg_data &= ~(CFGCLKFREQRANGE_Msk);
	reg_data |= _VAL2FLD(CFGCLKFREQRANGE, cfgclkfreqrange);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->rx_dphy_ctrl0, reg_data);

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->rx_dphy_ctrl0);
	reg_data &= ~(BASEDIR_Msk);
	reg_data |= (((1U << dphy->n_lanes) - 1) << BASEDIR_Pos);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->rx_dphy_ctrl0, reg_data);

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->rx_dphy_ctrl1);
	reg_data &= ~(FORCERXMODE_Msk);
	reg_data |= (((1U << dphy->n_lanes) - 1) << FORCERXMODE_Pos);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->rx_dphy_ctrl1, reg_data);

	PMU_delay_loop_us(1);

	dphy_ctrl->MIPI_DPHY_shutdown(ENABLE);

	PMU_delay_loop_us(1);

	dphy_ctrl->MIPI_DPHY_rst(ENABLE);

	if(dphy->clk_mode == NON_CONTINUOUS_CLOCK_MODE)
		stopstate_check |= DPHY_STOPSTATE_CLOCK;

	stopstate_check |= (dphy->n_lanes == 1 ? (DPHY_STOPSTATE_LANE0) :
                                             (DPHY_STOPSTATE_LANE0) | (DPHY_STOPSTATE_LANE1) );

	while(dphy_ctrl->MIPI_DPHY_stopstate() != stopstate_check)
	{
		if(lp_count++ < 1000)
			PMU_delay_loop_us(1);
		else
			return ARM_DRIVER_ERROR;
	}

	reg_data = MIPI_DPHY_CTRL_CFG_READ_REG(ctrl_reg->rx_dphy_ctrl1);
	reg_data &= ~(FORCERXMODE_Msk);
	MIPI_DPHY_CTRL_CFG_WRITE_REG(ctrl_reg->rx_dphy_ctrl1, reg_data);

	dphy->flags |= DPHY_FLAG_DRV_SLAVE_SETUP_DONE;

	return ARM_DRIVER_OK;

}

#if (RTE_MIPI_DPHY0)
/**
  \fn          ARM_DRIVER_VERSION DPHY0_GetVersion (void)
  \brief       Get MIPI DPHY driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION DPHY0_GetVersion (void)
{
	return DPHY_GetVersion();
}

/**
  \fn          ARM_MIPI_DPHY_CAPABILITIES DPHY0_GetCapabilities (void)
  \brief       Get MIPI DPHY driver capabilities
  \return      \ref ARM_MIPI_DPHY_CAPABILITIES
*/
static ARM_MIPI_DPHY_CAPABILITIES DPHY0_GetCapabilities (void)
{
	return DPHY_GetCapabilities();
}

/**
  \fn          int32_t DPHY0_Initialize (ARM_MIPI_DPHY_CFG *dphy_cfg)
  \brief       Initialize MIPI DPHY Interface.
  \param[in]   dphy_cfg Pointer to configure DPHY resource.
  \param[in]   frequency to configure the PLL.
  \return      \ref execution_status
*/
static int32_t DPHY0_Initialize (ARM_MIPI_DPHY_CFG *dphy_cfg)
{
	return DPHY_Initialize (dphy_cfg, &DPHY0);
}

/**
  \fn          int32_t DPHY0_Uninitialize (void)
  \brief       uninitialize MIPI DPHY Interface.
  \return      \ref execution_status
*/
static int32_t DPHY0_Uninitialize (void)
{
	return DPHY_Uninitialize (&DPHY0);
}

/**
  \fn          int32_t DPHY0_PowerControl (ARM_POWER_STATE state)
  \brief       Control DPHY Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t DPHY0_PowerControl (ARM_POWER_STATE state)
{
	return DPHY_PowerControl(state, &DPHY0);
}

/**
  \fn          int32_t DPHY0_Setup (void)
  \brief       Run MIPI DPHY Rx startup sequence.
  \return      \ref execution_status
*/
static int32_t DPHY0_Setup (void)
{
	return DPHY_SlaveSetup(&DPHY0);
}

/**
\brief Access structure of the MIPI DPHY0 Driver.
*/
extern ARM_DRIVER_MIPI_DPHY Driver_MIPI_DPHY0;

ARM_DRIVER_MIPI_DPHY Driver_MIPI_DPHY0 =
{
	DPHY0_GetVersion,
	DPHY0_GetCapabilities,
	DPHY0_Initialize,
	DPHY0_Uninitialize,
	DPHY0_PowerControl,
	DPHY0_Setup,
};
#endif

#if (RTE_MIPI_DPHY1)
/**
  \fn          ARM_DRIVER_VERSION DPHY1_GetVersion (void)
  \brief       Get MIPI DPHY driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION DPHY1_GetVersion (void)
{
	return DPHY_GetVersion();
}

/**
  \fn          ARM_MIPI_DPHY_CAPABILITIES DPHY1_GetCapabilities (void)
  \brief       Get MIPI DPHY driver capabilities
  \return      \ref ARM_MIPI_DPHY_CAPABILITIES
*/
static ARM_MIPI_DPHY_CAPABILITIES DPHY1_GetCapabilities (void)
{
	return DPHY_GetCapabilities();
}

/**
  \fn          int32_t DPHY1_Initialize (ARM_MIPI_DPHY_CFG *dphy_cfg)
  \brief       Initialize MIPI DPHY Interface.
  \param[in]   dphy_cfg Pointer to configure DPHY resource.
  \param[in]   frequency to configure the PLL.
  \return      \ref execution_status
*/
static int32_t DPHY1_Initialize (ARM_MIPI_DPHY_CFG *dphy_cfg)
{
	return DPHY_Initialize (dphy_cfg, &DPHY1);
}

/**
  \fn          int32_t DPHY1_Uninitialize (void)
  \brief       uninitialize MIPI DPHY Interface.
  \return      \ref execution_status
*/
static int32_t DPHY1_Uninitialize (void)
{
	return DPHY_Uninitialize (&DPHY1);
}

/**
  \fn          int32_t DPHY1_PowerControl (ARM_POWER_STATE state)
  \brief       Control DPHY Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t DPHY1_PowerControl (ARM_POWER_STATE state)
{
	return DPHY_PowerControl(state, &DPHY1);
}

/**
  \fn          int32_t DPHY1_Setup (void)
  \brief       Run MIPI DPHY Tx startup sequence.
  \return      \ref execution_status
*/
static int32_t DPHY1_Setup (void)
{
	return DPHY_MasterSetup(&DPHY1);
}

/**
\brief Access structure of the MIPI DPHY1 Driver.
*/
extern ARM_DRIVER_MIPI_DPHY Driver_MIPI_DPHY1;

ARM_DRIVER_MIPI_DPHY Driver_MIPI_DPHY1 =
{
	DPHY1_GetVersion,
	DPHY1_GetCapabilities,
	DPHY1_Initialize,
	DPHY1_Uninitialize,
	DPHY1_PowerControl,
	DPHY1_Setup,
};
#endif



