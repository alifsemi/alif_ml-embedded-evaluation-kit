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
 * @file     Driver_MIPI_DSI.c
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     18-April-2022
 * @brief    CMSIS-Driver for MIPI DSI.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#include "Driver_MIPI_DSI.h"
#include "Driver_MIPI_DPHY.h"
#include "MIPI_DSI_dev.h"
#include "RTE_Device.h"

#define ARM_MIPI_DSI_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /*driver version*/

extern ARM_DRIVER_MIPI_DPHY Driver_MIPI_DPHY1;
ARM_DPHY_NEED_CTRL dsi_dphy_ctrl;

/*Driver Version*/
static const ARM_DRIVER_VERSION DriverVersion =
{
	ARM_MIPI_DSI_API_VERSION,
	ARM_MIPI_DSI_DRV_VERSION
};


#if (RTE_MIPI_DSI)
MIPI_DSI_DRV_INFO DSI = {
	.reg_base = MIPI_DSI,
	.n_lanes  = RTE_MIPI_DSI_N_LANES,
	.flags    = NULL,
};


/**
  \fn          static void MIPI_DSI_DPHY_Shutdown (uint8_t state)
  \brief       PHY shutdown line control callback function.
  \param[in]   state ENABLE/DISABLE the line.
  */
static void MIPI_DSI_DPHY_Shutdown (uint8_t state)
{
	if(state == ENABLE)
		MIPI_DSI_SET_BIT(DSI.reg_base->PHY_RSTZ, DSI_PHY_SHUTDOWNZ_Msk);
	else
		MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_RSTZ, DSI_PHY_SHUTDOWNZ_Msk);
}

/**
  \fn          static void MIPI_DSI_DPHY_Rst (uint8_t state)
  \brief       PHY reset line control callback function.
  \param[in]   state ENABLE/DISABLE the line.
  */
static void MIPI_DSI_DPHY_Rst (uint8_t state)
{
	if(state == ENABLE)
		MIPI_DSI_SET_BIT(DSI.reg_base->PHY_RSTZ, DSI_PHY_RSTZ_Msk);
	else
		MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_RSTZ, DSI_PHY_RSTZ_Msk);
}

/**
  \fn          static void MIPI_DSI_DPHY_Enableclk (uint8_t state)
  \brief       PHY enable clock line control callback function.
  \param[in]   state ENABLE/DISABLE the line.
  */
static void MIPI_DSI_DPHY_Enableclk (uint8_t state)
{
	if(state == ENABLE)
		MIPI_DSI_SET_BIT(DSI.reg_base->PHY_RSTZ, DSI_PHY_ENABLECLK_Msk);
	else
		MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_RSTZ, DSI_PHY_ENABLECLK_Msk);
}

/**
  \fn          static void MIPI_DSI_DPHY_Testclr (uint8_t state)
  \brief       PHY testclr line control callback function.
  \param[in]   state ENABLE/DISABLE the line.
  */
static void MIPI_DSI_DPHY_Testclr (uint8_t state)
{
	if(state == ENABLE)
		MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLR_Msk);
	else
		MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLR_Msk);

}

/**
  \fn          static uint8_t MIPI_DSI_DPHY_PLL_Lock (void)
  \brief       PHY testclr line control callback function.
  \return      return status of the PLL lock.
  */
static uint8_t MIPI_DSI_DPHY_PLL_Lock (void)
{
	if(MIPI_DSI_READ_BIT(DSI.reg_base->PHY_STATUS, DSI_PHY_LOCK_Msk))
		return DPHY_PLL_LOCK;
	else
		return DPHY_NO_PLL_LOCK;
}

/**
  \fn          static uint8_t MIPI_DSI_DPHY_Stopstate (void)
  \brief       status of stopstate from PHY
  \return      ret status of stopstate.
  */
static uint8_t MIPI_DSI_DPHY_Stopstate (void)
{
	uint32_t reg_status = 0;
	uint8_t ret = 0;

	reg_status = MIPI_DSI_READ_REG(DSI.reg_base->PHY_STATUS);

	if(MIPI_DSI_READ_BIT(reg_status, DSI_PHY_STOPSTATELANE_0_Msk))
		ret |= DPHY_STOPSTATE_LANE0;

	if(MIPI_DSI_READ_BIT(reg_status, DSI_PHY_STOPSTATELANE_1_Msk))
		ret |= DPHY_STOPSTATE_LANE1;

	if(MIPI_DSI_READ_BIT(reg_status, DSI_PHY_STOPSTATECLKLANE_Msk))
		ret |= DPHY_STOPSTATE_CLOCK;

	return ret;

}

/**
  \fn          static uint8_t MIPI_DSI_DPHY_Read (uint16_t address)
  \brief       Test and control interface protocol to read DPHY registers.
  \param[in]   address index on DPHY register.
  \return      ret register value.
  */
static uint8_t MIPI_DSI_DPHY_Read (uint16_t address)
{
	uint8_t ret = 0;
	uint32_t read_reg = 0;

	/*Ensure that t(r)x_testclk and t(r)x_testen is set to low*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Set t(r)x_testen to high. */
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Set t(r)x_testen to high. */
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Place 0x00 in t(r)x_testdin.*/
	read_reg = MIPI_DSI_READ_REG(DSI.reg_base->PHY_TST_CTRL1);
	read_reg &= ~(DSI_PHY_TESTDIN_Msk);
	MIPI_DSI_WRITE_REG(DSI.reg_base->PHY_TST_CTRL1, read_reg);
	/*Set t(r)x_testclk to low (with the falling edge on t(r)x_testclk,
	  the t(r)x_testdin signal content is latched internally).*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Place the 8-bit word corresponding to the testcode MSBs in t(r)x_testdin.*/
	read_reg = MIPI_DSI_READ_REG(DSI.reg_base->PHY_TST_CTRL1);
	read_reg &= ~(DSI_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(DSI_PHY_TESTDIN, (address >> 8));
	MIPI_DSI_WRITE_REG(DSI.reg_base->PHY_TST_CTRL1, read_reg);
	/*Set t(r)x_testclk to high.*/
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Set t(r)x_testclk to low*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to high*/
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Set t(r)x_testclk to high.*/
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Place the 8-bit word test data in t(r)x_testdin.*/
	read_reg = MIPI_DSI_READ_REG(DSI.reg_base->PHY_TST_CTRL1);
	read_reg &= ~(DSI_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(DSI_PHY_TESTDIN, address);
	MIPI_DSI_WRITE_REG(DSI.reg_base->PHY_TST_CTRL1, read_reg);
	/*Set t(r)x_testclk to low (with the falling edge on t(r)x_testclk,
	  the t(r)x_testdin signal content is latched internally).*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	read_reg = MIPI_DSI_READ_REG(DSI.reg_base->PHY_TST_CTRL1);
	read_reg &= (DSI_PHY_TESTDOUT_Msk);
	ret = _FLD2VAL(DSI_PHY_TESTDOUT, read_reg);
	/*Set t(r)x_testen to low.*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);

	return ret;
}

/**
  \fn          static void MIPI_DSI_DPHY_Write (uint16_t address, uint8_t data)
  \brief       Test and control interface protocol to Write DPHY registers.
  \param[in]   address index on DPHY register.
  \param[in]   data register value.
  */
static void MIPI_DSI_DPHY_Write (uint16_t address, uint8_t data)
{
	uint32_t read_reg = 0;

	/*Ensure that t(r)x_testclk and t(r)x_testen is set to low*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Set t(r)x_testen to high. */
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Set t(r)x_testen to high. */
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Place 0x00 in t(r)x_testdin.*/
	read_reg = MIPI_DSI_READ_REG(DSI.reg_base->PHY_TST_CTRL1);
	read_reg &= ~(DSI_PHY_TESTDIN_Msk);
	MIPI_DSI_WRITE_REG(DSI.reg_base->PHY_TST_CTRL1, read_reg);
	/*Set t(r)x_testclk to low (with the falling edge on t(r)x_testclk,
	  the t(r)x_testdin signal content is latched internally).*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to low*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Place the 8-bit word corresponding to the testcode MSBs in t(r)x_testdin.*/
	read_reg = MIPI_DSI_READ_REG(DSI.reg_base->PHY_TST_CTRL1);
	read_reg &= ~(DSI_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(DSI_PHY_TESTDIN, (address >> 8));
	MIPI_DSI_WRITE_REG(DSI.reg_base->PHY_TST_CTRL1, read_reg);
	/*Set t(r)x_testclk to high.*/
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Set t(r)x_testclk to low*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to high*/
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Set t(r)x_testclk to high.*/
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Place the 8-bit word test data in t(r)x_testdin.*/
	read_reg = MIPI_DSI_READ_REG(DSI.reg_base->PHY_TST_CTRL1);
	read_reg &= ~(DSI_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(DSI_PHY_TESTDIN, address);
	MIPI_DSI_WRITE_REG(DSI.reg_base->PHY_TST_CTRL1, read_reg);
	/*Set t(r)x_testclk to low (with the falling edge on t(r)x_testclk,
	  the t(r)x_testdin signal content is latched internally).*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to low.*/
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL1, DSI_PHY_TESTEN_Msk);
	/*Place the 8-bit word corresponding to the page offset in t(r)x_testdin.*/
	read_reg = MIPI_DSI_READ_REG(DSI.reg_base->PHY_TST_CTRL1);
	read_reg &= ~(DSI_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(DSI_PHY_TESTDIN, data);
	MIPI_DSI_WRITE_REG(DSI.reg_base->PHY_TST_CTRL1, read_reg);
	/*Set t(r)x_testclk to high (test data is programmed internally).*/
	MIPI_DSI_SET_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
	MIPI_DSI_CLEAR_BIT(DSI.reg_base->PHY_TST_CTRL0, DSI_PHY_TESTCLK_Msk);
}
#endif
//
//  Functions
//

/**
  \fn          ARM_DRIVER_VERSION ARM_MIPI_DSI_GetVersion (void)
  \brief       Get MIPI DSI driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION ARM_MIPI_DSI_GetVersion (void)
{
	return DriverVersion;
}

/**
  \fn          int32_t DSI_Initialize (uint32_t frequency,
  MIPI_DSI_DRV_INFO *dsi)
  \brief       Initialize MIPI DSI Interface.
  \param[in]   frequency to configure DPHY PLL.
  \param[in]   dsi Pointer to DSI resources
  \return      \ref execution_status
  */
static int32_t DSI_Initialize ( uint32_t frequency,
		MIPI_DSI_DRV_INFO *dsi)
{
	ARM_MIPI_DPHY_CFG dphy_cfg;
	int32_t ret = 0;

#if (RTE_MIPI_DSI)
	dsi_dphy_ctrl.MIPI_DPHY_shutdown = MIPI_DSI_DPHY_Shutdown ;
	dsi_dphy_ctrl.MIPI_DPHY_rst = MIPI_DSI_DPHY_Rst;
	dsi_dphy_ctrl.MIPI_DPHY_testclr = MIPI_DSI_DPHY_Testclr;
	dsi_dphy_ctrl.MIPI_DPHY_enableclk = MIPI_DSI_DPHY_Enableclk;
	dsi_dphy_ctrl.MIPI_DPHY_PLL_Lock = MIPI_DSI_DPHY_PLL_Lock;
	dsi_dphy_ctrl.MIPI_DPHY_stopstate = MIPI_DSI_DPHY_Stopstate;
	dsi_dphy_ctrl.MIPI_DPHY_read = MIPI_DSI_DPHY_Read;
	dsi_dphy_ctrl.MIPI_DPHY_write = MIPI_DSI_DPHY_Write;
#endif
	dphy_cfg.clock_frequency = frequency;
	dphy_cfg.dphy_control_event = &dsi_dphy_ctrl;
	dphy_cfg.mode = DPHY_MASTER;
	dphy_cfg.n_lanes = (DPHY_N_Lanes)dsi->n_lanes;

	ret = Driver_MIPI_DPHY1.Initialize(&dphy_cfg);
	if(ret != ARM_DRIVER_OK)
		return ret;

	dsi->flags = DSI_FLAG_DRV_INIT_DONE;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DSI_Uninitialize (MIPI_DSI_DRV_INFO *dsi)
  \brief       uninitialize MIPI DSI Interface.
  \param[in]   dsi Pointer to DSI resources
  \return      \ref execution_status
  */
static int32_t DSI_Uninitialize (MIPI_DSI_DRV_INFO *dsi)
{
	int32_t ret = 0;
	dsi->flags = 0;

	Driver_MIPI_DPHY1.Uninitialize();
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DSI_PowerControl (ARM_POWER_STATE state, MIPI_DSI_DRV_INFO *dsi)
  \brief       Control DSI Interface Power.
  \param[in]   state  Power state
  \param[in]   dsi  Pointer to DSI resources
  \return      \ref execution_status
  */
static int32_t DSI_PowerControl (ARM_POWER_STATE state, MIPI_DSI_DRV_INFO *dsi)
{
	volatile uint32_t *csi_pixclk_reg = (volatile uint32_t *)CSI_PIXCLK_CTRL;
	int32_t ret = 0;

	switch (state)
	{
		case ARM_POWER_OFF:
			Driver_MIPI_DPHY1.PowerControl(ARM_POWER_OFF);
			if(ret != ARM_DRIVER_OK)
				return ret;

			dsi->flags &= ~DSI_FLAG_CLKSRC_ENABLED;
			break;

		case ARM_POWER_FULL:

			if (!(dsi->flags & DSI_FLAG_DRV_INIT_DONE))
				return ARM_DRIVER_ERROR;
			if (dsi->flags & DSI_FLAG_CLKSRC_ENABLED)
				return ARM_DRIVER_OK;

			Driver_MIPI_DPHY1.PowerControl(ARM_POWER_FULL);
			if(ret != ARM_DRIVER_OK)
				return ret;

			dsi->flags |= DSI_FLAG_CLKSRC_ENABLED;

			break;

		case ARM_POWER_LOW:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DSI_StartPHY (MIPI_DSI_DRV_INFO *dsi)
  \brief       Configure MIPI DSI PHY Interface.
  \param[in]   dsi  Pointer to DSI resources
  \return      \ref execution_status
  */
static int32_t DSI_StartPHY (MIPI_DSI_DRV_INFO *dsi)
{
	int32_t ret = 0;

	if(!(dsi->flags & DSI_FLAG_CLKSRC_ENABLED))
		return ARM_DRIVER_ERROR;

	ret = Driver_MIPI_DPHY1.Setup();
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ARM_DRIVER_OK;
}

#if (RTE_MIPI_DSI)
/**
  \fn          int32_t  ARM_MIPI_DSI_Initialize (uint32_t frequency)
  \brief       Initialize MIPI DSI Interface.
  \param[in]   frequency to configure DPHY PLL.
  \return      \ref execution_status
  */
static int32_t ARM_MIPI_DSI_Initialize (uint32_t frequency)
{
	return DSI_Initialize(frequency, &DSI);
}

/**
  \fn          int32_t ARM_MIPI_DSI_Uninitialize (void)
  \brief       uninitialize MIPI DSI Interface.
  \return      \ref execution_status
  */
static int32_t ARM_MIPI_DSI_Uninitialize (void)
{
	return DSI_Uninitialize(&DSI);
}

/**
  \fn          int32_t ARM_MIPI_DSI_PowerControl (ARM_POWER_STATE state)
  \brief       Control DSI Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
  */
static int32_t ARM_MIPI_DSI_PowerControl (ARM_POWER_STATE state)
{
	return DSI_PowerControl (state, &DSI);
}

/**
  \fn          int32_t ARM_MIPI_DSI_StartPHY (void)
  \brief       Configure MIPI DSI PHY Interface.
  \return      \ref execution_status
  */
static int32_t ARM_MIPI_DSI_StartPHY (void)
{
	return DSI_StartPHY(&DSI);
}

/**
  \brief Access structure of the  MIPI DSI Driver.
  */
extern ARM_DRIVER_MIPI_DSI Driver_MIPI_DSI;

ARM_DRIVER_MIPI_DSI Driver_MIPI_DSI =
{
	ARM_MIPI_DSI_GetVersion,
	ARM_MIPI_DSI_Initialize,
	ARM_MIPI_DSI_Uninitialize,
	ARM_MIPI_DSI_PowerControl,
	ARM_MIPI_DSI_StartPHY,
};

#endif
