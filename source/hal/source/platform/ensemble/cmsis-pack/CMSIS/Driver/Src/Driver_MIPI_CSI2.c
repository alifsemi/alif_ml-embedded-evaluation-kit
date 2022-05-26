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
 * @file     Driver_MIPI_CSI2.c
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     24-March-2022
 * @brief    CMSIS-Driver for MIPI CSI2.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#include "RTE_Components.h"
#include CMSIS_device_header
#include "MIPI_CSI2_dev.h"
#include "Driver_MIPI_CSI2.h"
#include "Driver_MIPI_DPHY.h"
#include "RTE_Device.h"

#define ARM_MIPI_CSI2_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)/*Driver version*/

extern ARM_DRIVER_MIPI_DPHY Driver_MIPI_DPHY0;
ARM_DPHY_NEED_CTRL dphy_ctrl;

/*Driver version*/
static const ARM_DRIVER_VERSION DriverVersion =
{
	ARM_MIPI_CSI2_API_VERSION,
	ARM_MIPI_CSI2_DRV_VERSION
};

#if (RTE_MIPI_CSI2)
ARM_MIPI_CSI2_FRAME_INFO CSI2_FRAME_INFO =
{
	.hsa_time     = RTE_MIPI_CSI2_IPI_HSA_TIME,
	.hbp_time     = RTE_MIPI_CSI2_IPI_HBP_TIME,
	.hsd_time     = RTE_MIPI_CSI2_IPI_HSD_TIME,
	.hactive_time = RTE_MIPI_CSI2_IPI_HACTIVE_TIME,
	.vsa_line     = RTE_MIPI_CSI2_IPI_VSA_LINE,
	.vbp_line     = RTE_MIPI_CSI2_IPI_VBP_LINE,
	.vfp_line     = RTE_MIPI_CSI2_IPI_VFP_LINE,
	.vactive_line = RTE_MIPI_CSI2_IPI_VACTIVE_LINE,
};

ARM_MIPI_CSI2_IPI_INFO CSI2_IPI_CFG =
{
	.ipi_mode      = RTE_MIPI_CSI2_IPI_MODE,
	.ipi_color_cop = RTE_MIPI_CSI2_COLOR_COP,
	.ipi_memflush  = RTE_MIPI_CSI2_MEMFLUSH,
	.frame_info    = &CSI2_FRAME_INFO,
};

ARM_MIPI_CSI2_DRV_INFO CSI2 =
{
	.reg_base           = MIPI_CSI2,
	.cb_event           = NULL,
	.csi_pixclk_div     = RTE_MIPI_CSI2_PIXCLK_DIV,
	.n_lanes            = RTE_MIPI_CSI2_N_LANES,
	.vc_id              = RTE_MIPI_CSI2_VC_ID,
	.pixel_data_type    = RTE_MIPI_CSI2_DATA_TYPE,
	.ipi_info           = &CSI2_IPI_CFG,
	.irq                = (IRQn_Type)MIPI_CSI2_IRQ,
	.irq_priority       = RTE_MIPI_CSI2_IRQ_PRI,
};
#endif

/*csi2 pixel data type*/
uint8_t mipi_csi2_data_type[] =
{
	[MIPI_CSI2_DT_YUV420_8] = 0x18,
	[MIPI_CSI2_DT_YUV420_10] = 0x19,
	[MIPI_CSI2_DT_YUV420_8_LEGACY] = 0x1A,
	[MIPI_CSI2_DT_YUV420_8_SHIFT] = 0x1C,
	[MIPI_CSI2_DT_YUV420_10_SHIFT] = 0x1D,
	[MIPI_CSI2_DT_YUV422_8] = 0x1E,
	[MIPI_CSI2_DT_YUV422_10] = 0x1F,
	[MIPI_CSI2_DT_RGB444] = 0x20,
	[MIPI_CSI2_DT_RGB555] = 0x21,
	[MIPI_CSI2_DT_RGB565] = 0x22,
	[MIPI_CSI2_DT_RGB666] = 0x23,
	[MIPI_CSI2_DT_RGB888] = 0x24,
	[MIPI_CSI2_DT_RAW6] = 0x28,
	[MIPI_CSI2_DT_RAW7] = 0x29,
	[MIPI_CSI2_DT_RAW8] = 0x2A,
	[MIPI_CSI2_DT_RAW10] = 0x2B,
	[MIPI_CSI2_DT_RAW12] = 0x2C,
	[MIPI_CSI2_DT_RAW14] = 0x2D,
	[MIPI_CSI2_DT_RAW16] = 0x2E,
	[MIPI_CSI2_DT_USER_DEFINED_1] = 0x30,
	[MIPI_CSI2_DT_USER_DEFINED_2] = 0x31,
	[MIPI_CSI2_DT_USER_DEFINED_3] = 0x32,
	[MIPI_CSI2_DT_USER_DEFINED_4] = 0x33,
	[MIPI_CSI2_DT_USER_DEFINED_5] = 0x34,
	[MIPI_CSI2_DT_USER_DEFINED_6] = 0x35,
	[MIPI_CSI2_DT_USER_DEFINED_7] = 0x36,
	[MIPI_CSI2_DT_USER_DEFINED_8] = 0x37,
};

/* Driver Capabilities */
static const ARM_MIPI_CSI2_CAPABILITIES DriverCapabilities =
{
	0, /* Not supports reentrant_operation */
	1, /* IPI interface supported*/
	0, /* IDI interface not supported*/
	0  /* reserved (must be zero) */
};

//
//  Functions
//

/**
  \fn          ARM_DRIVER_VERSION MIPI_CSI2_GetVersion (void)
  \brief       Get MIPI CSI2 driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION MIPI_CSI2_GetVersion (void)
{
	return DriverVersion;
}

/**
  \fn          ARM_MIPI_CSI2_CAPABILITIES MIPI_CSI2_GetCapabilities (void)
  \brief       Get MIPI CSI2 driver capabilities
  \return      \ref ARM_MIPI_DPHY_CAPABILITIES
*/
static ARM_MIPI_CSI2_CAPABILITIES MIPI_CSI2_GetCapabilities (void)
{
	return DriverCapabilities;
}

#if (RTE_MIPI_CSI2)
/**
  \fn          static void MIPI_CSI2_DPHY_Shutdown (uint8_t state)
  \brief       PHY shutdown line control callback function.
  \param[in]   state ENABLE/DISABLE the line.
*/
static void MIPI_CSI2_DPHY_Shutdown (uint8_t state)
{
	if(state == ENABLE)
		MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_SHUTDOWNZ, CSI2_PHY_SHUTDOWNZ_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_SHUTDOWNZ, CSI2_PHY_SHUTDOWNZ_Msk);
}

/**
  \fn          static void MIPI_CSI2_DPHY_Testclr (uint8_t state)
  \brief       PHY testclr line control callback function.
  \param[in]   state ENABLE/DISABLE the line.
*/
static void MIPI_CSI2_DPHY_Testclr (uint8_t state)
{
	if(state == ENABLE)
		MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLR_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLR_Msk);
}

/**
  \fn          static void MIPI_CSI2_DPHY_Rst (uint8_t state)
  \brief       PHY reset line control callback function.
  \param[in]   state ENABLE/DISABLE the line.
*/
static void MIPI_CSI2_DPHY_Rst (uint8_t state)
{
	if(state == ENABLE)
		MIPI_CSI2_SET_BIT(CSI2.reg_base->DPHY_RSTZ, CSI2_PHY_RSTZ_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->DPHY_RSTZ, CSI2_PHY_RSTZ_Msk);
}

/**
  \fn          static uint8_t MIPI_CSI2_DPHY_Rx (void)
  \brief       status of RX-related signals from PHY
  \return      ret status of RX-related signals.
*/
static uint8_t MIPI_CSI2_DPHY_Rx (void)
{
	uint32_t reg_status = 0;
	uint8_t ret = 0;

	reg_status = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_RX);

	if(MIPI_CSI2_READ_BIT(reg_status, CSI2_PHY_RXULPSESC_LANE_0_Msk))
		ret |= DPHY_ULPM_LANE0;

	if(MIPI_CSI2_READ_BIT(reg_status, CSI2_PHY_RXULPSESC_LANE_1_Msk))
		ret |= DPHY_ULPM_LANE1;

	if(MIPI_CSI2_READ_BIT(reg_status, CSI2_PHY_RXULPSCLKNOT_Msk))
		ret |= DPHY_ULPM_CLOCK;

	if(MIPI_CSI2_READ_BIT(reg_status, CSI2_PHY_RXCLKACTIVEHS_Msk))
		ret |= DPHY_CLOCKACTIVE_HS;

	return ret;
}

/**
  \fn          static uint8_t MIPI_CSI2_DPHY_Stopstate (void)
  \brief       status of stopstate from PHY
  \return      ret status of stopstate.
*/
static uint8_t MIPI_CSI2_DPHY_Stopstate (void)
{
	uint32_t reg_status = 0;
	uint8_t ret = 0;

	reg_status = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_STOPSTATE);

	if(MIPI_CSI2_READ_BIT(reg_status, CSI2_PHY_STOPSTATEDATA_0_Msk))
		ret |= DPHY_STOPSTATE_LANE0;

	if(MIPI_CSI2_READ_BIT(reg_status, CSI2_PHY_STOPSTATEDATA_1_Msk))
		ret |= DPHY_STOPSTATE_LANE1;

	if(MIPI_CSI2_READ_BIT(reg_status, CSI2_PHY_STOPSTATECLK_Msk))
		ret |= DPHY_STOPSTATE_CLOCK;

	return ret;
}

/**
  \fn          static uint8_t MIPI_CSI2_DPHY_Read (uint16_t address)
  \brief       Test and control interface protocol to read DPHY registers.
  \param[in]   address index on DPHY register.
  \return      ret register value.
*/
static uint8_t MIPI_CSI2_DPHY_Read (uint16_t address)
{
	uint8_t ret = 0;
	uint32_t read_reg = 0;

	/*Ensure that t(r)x_testclk and t(r)x_testen is set to low*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Set t(r)x_testen to high. */
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Set t(r)x_testen to high. */
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Place 0x00 in t(r)x_testdin.*/
	read_reg = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_TEST_CTRL1);
	read_reg &= ~(CSI2_PHY_TESTDIN_Msk);
	MIPI_CSI2_WRITE_REG(CSI2.reg_base->PHY_TEST_CTRL1, read_reg);
	/*Set t(r)x_testclk to low (with the falling edge on t(r)x_testclk,
	the t(r)x_testdin signal content is latched internally).*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to low*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Place the 8-bit word corresponding to the testcode MSBs in t(r)x_testdin.*/
	read_reg = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_TEST_CTRL1);
	read_reg &= ~(CSI2_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(CSI2_PHY_TESTDIN, (address >> 8));
	MIPI_CSI2_WRITE_REG(CSI2.reg_base->PHY_TEST_CTRL1, read_reg);
	/*Set t(r)x_testclk to high.*/
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Set t(r)x_testclk to low*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to high*/
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Set t(r)x_testclk to high.*/
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Place the 8-bit word test data in t(r)x_testdin.*/
	read_reg = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_TEST_CTRL1);
	read_reg &= ~(CSI2_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(CSI2_PHY_TESTDIN, address);
	MIPI_CSI2_WRITE_REG(CSI2.reg_base->PHY_TEST_CTRL1, read_reg);
	/*Set t(r)x_testclk to low (with the falling edge on t(r)x_testclk,
	the t(r)x_testdin signal content is latched internally).*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	read_reg = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_TEST_CTRL1);
	read_reg &= (CSI2_PHY_TESTDOUT_Msk);
	ret = _FLD2VAL(CSI2_PHY_TESTDOUT, read_reg);
	/*Set t(r)x_testen to low.*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);

	return ret;
}

/**
  \fn          static void MIPI_CSI2_DPHY_Write (uint16_t address, uint8_t data)
  \brief       Test and control interface protocol to Write DPHY registers.
  \param[in]   address index on DPHY register.
  \param[in]   data register value.
*/
static void MIPI_CSI2_DPHY_Write (uint16_t address, uint8_t data)
{
	uint32_t read_reg = 0;

	/*Ensure that t(r)x_testclk and t(r)x_testen is set to low*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Set t(r)x_testen to high. */
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Set t(r)x_testen to high. */
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Place 0x00 in t(r)x_testdin.*/
	read_reg = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_TEST_CTRL1);
	read_reg &= ~(CSI2_PHY_TESTDIN_Msk);
	MIPI_CSI2_WRITE_REG(CSI2.reg_base->PHY_TEST_CTRL1, read_reg);
	/*Set t(r)x_testclk to low (with the falling edge on t(r)x_testclk,
	the t(r)x_testdin signal content is latched internally).*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to low*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Place the 8-bit word corresponding to the testcode MSBs in t(r)x_testdin.*/
	read_reg = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_TEST_CTRL1);
	read_reg &= ~(CSI2_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(CSI2_PHY_TESTDIN, (address >> 8));
	MIPI_CSI2_WRITE_REG(CSI2.reg_base->PHY_TEST_CTRL1, read_reg);
	/*Set t(r)x_testclk to high.*/
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Set t(r)x_testclk to low*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to high*/
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Set t(r)x_testclk to high.*/
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Place the 8-bit word test data in t(r)x_testdin.*/
	read_reg = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_TEST_CTRL1);
	read_reg &= ~(CSI2_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(CSI2_PHY_TESTDIN, address);
	MIPI_CSI2_WRITE_REG(CSI2.reg_base->PHY_TEST_CTRL1, read_reg);
	/*Set t(r)x_testclk to low (with the falling edge on t(r)x_testclk,
	the t(r)x_testdin signal content is latched internally).*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	/*Set t(r)x_testen to low.*/
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL1, CSI2_PHY_TESTEN_Msk);
	/*Place the 8-bit word corresponding to the page offset in t(r)x_testdin.*/
	read_reg = MIPI_CSI2_READ_REG(CSI2.reg_base->PHY_TEST_CTRL1);
	read_reg &= ~(CSI2_PHY_TESTDIN_Msk);
	read_reg |= _VAL2FLD(CSI2_PHY_TESTDIN, data);
	MIPI_CSI2_WRITE_REG(CSI2.reg_base->PHY_TEST_CTRL1, read_reg);
	/*Set t(r)x_testclk to high (test data is programmed internally).*/
	MIPI_CSI2_SET_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
	MIPI_CSI2_CLEAR_BIT(CSI2.reg_base->PHY_TEST_CTRL0, CSI2_PHY_TESTCLK_Msk);
}
#endif

/**
  \fn          int32_t CSI2_Initialize (ARM_MIPI_CSI2_SignalEvent_t cb_event,
                                        uint32_t frequency,
                                        ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       Initialize MIPI CSI2 Interface.
  \param[in]   cb_event Pointer to ARM_MIPI_CSI2_SignalEvent_t
  \param[in]   frequency to configure DPHY PLL.
  \param[in]   csi2 Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_Initialize (ARM_MIPI_CSI2_SignalEvent_t cb_event,
                                uint32_t frequency,
                                ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	ARM_MIPI_DPHY_CFG dphy_cfg;
	int32_t ret = 0;

	if(csi2->flags & CSI2_FLAG_DRV_INIT_DONE)
		return ARM_DRIVER_OK;

	if (!cb_event)
		return ARM_DRIVER_ERROR_PARAMETER;

	csi2->cb_event = cb_event;

#if (RTE_MIPI_CSI2)
	dphy_ctrl.MIPI_DPHY_shutdown = MIPI_CSI2_DPHY_Shutdown ;
	dphy_ctrl.MIPI_DPHY_rst = MIPI_CSI2_DPHY_Rst;
	dphy_ctrl.MIPI_DPHY_testclr = MIPI_CSI2_DPHY_Testclr;
	dphy_ctrl.MIPI_DPHY_rx = MIPI_CSI2_DPHY_Rx;
	dphy_ctrl.MIPI_DPHY_stopstate = MIPI_CSI2_DPHY_Stopstate;
	dphy_ctrl.MIPI_DPHY_read = MIPI_CSI2_DPHY_Read;
	dphy_ctrl.MIPI_DPHY_write = MIPI_CSI2_DPHY_Write;
#endif
	dphy_cfg.clock_frequency = frequency;
	dphy_cfg.dphy_control_event = &dphy_ctrl;
	dphy_cfg.mode = DPHY_SLAVE;
	dphy_cfg.n_lanes = (DPHY_N_Lanes)csi2->n_lanes;

	ret = Driver_MIPI_DPHY0.Initialize(&dphy_cfg);
	if(ret != ARM_DRIVER_OK)
		return ret;

	csi2->flags = CSI2_FLAG_DRV_INIT_DONE;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t CSI2_Uninitialize (ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       uninitialize MIPI CSI2 Interface.
  \param[in]   csi2 Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_Uninitialize (ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	int32_t ret = 0;
	csi2->cb_event = NULL;
	csi2->flags = 0;

	Driver_MIPI_DPHY0.Uninitialize();
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t CSI2_PowerControl (ARM_POWER_STATE state, ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       Control CSI2 Interface Power.
  \param[in]   state  Power state
  \param[in]   csi2  Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_PowerControl (ARM_POWER_STATE state, ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	volatile uint32_t *csi_pixclk_reg = (volatile uint32_t *)CSI_PIXCLK_CTRL;
	int32_t ret = 0;

	switch (state)
	{
		case ARM_POWER_OFF:

			NVIC_DisableIRQ (csi2->irq);
			NVIC_ClearPendingIRQ (csi2->irq);

			MIPI_CSI2_CLEAR_BIT(*csi_pixclk_reg, CSI2_CLK_EN_Msk);
			*csi_pixclk_reg &= ~CSI2_CLK_DIV_Msk;

			Driver_MIPI_DPHY0.PowerControl(ARM_POWER_OFF);
			if(ret != ARM_DRIVER_OK)
				return ret;

			csi2->flags &= ~CSI2_FLAG_CLKSRC_ENABLED;
			break;

		case ARM_POWER_FULL:

			if (!(csi2->flags & CSI2_FLAG_DRV_INIT_DONE))
					return ARM_DRIVER_ERROR;
			if (csi2->flags & CSI2_FLAG_CLKSRC_ENABLED)
					return ARM_DRIVER_OK;

			NVIC_ClearPendingIRQ (csi2->irq);
			NVIC_SetPriority (csi2->irq, csi2->irq_priority);
			NVIC_EnableIRQ (csi2->irq);

			MIPI_CSI2_SET_BIT(*csi_pixclk_reg, CSI2_CLK_EN_Msk);
			*csi_pixclk_reg &= ~CSI2_CLK_DIV_Msk;
			*csi_pixclk_reg |= _VAL2FLD(CSI2_CLK_DIV, csi2->csi_pixclk_div);

			csi2->flags |= CSI2_FLAG_CLKSRC_ENABLED;

			break;

		case ARM_POWER_LOW:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t CSI2_ConfigureHost (uint32_t int_event, ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       Configure CSI2 Host Interface.
  \param[in]   int_event interrupt event to be enabled.
  \param[in]   csi2  Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_ConfigureHost (uint32_t int_event, ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	MIPI_CSI2_Type *csi2_reg = (MIPI_CSI2_Type *)csi2->reg_base;

	if(!(csi2->flags & CSI2_FLAG_CLKSRC_ENABLED))
		return ARM_DRIVER_ERROR;

	MIPI_CSI2_CLEAR_BIT(csi2_reg->CSI2_RESETN,CSI2_RESETN_Msk);

	csi2_reg->N_LANES &= ~CSI2_N_LANES_Msk;
	csi2_reg->N_LANES |= _VAL2FLD(CSI2_N_LANES, (csi2->n_lanes - 1));

	if(int_event & CSI2_EVENT_PHY_FATAL)
		csi2_reg->INT_MSK_PHY_FATAL |= MASK(8,0);

	if(int_event & CSI2_EVENT_PKT_FATAL)
		csi2_reg->INT_MSK_PKT_FATAL |= MASK(1,0);

	if(int_event & CSI2_EVENT_PHY)
		csi2_reg->INT_MSK_PHY |= MASK(7,0) | MASK(23,16);

	if(int_event & CSI2_EVENT_LINE)
		csi2_reg->INT_MSK_LINE |= MASK(7,0) | MASK(23,16);

	if(int_event & CSI2_EVENT_IPI_FATAL)
		csi2_reg->INT_MSK_IPI_FATAL |= MASK(5,0);

	if(int_event & CSI2_EVENT_BNDRY_FRAME_FATAL)
		csi2_reg->INT_MSK_BNDRY_FRAME_FATAL |= MASK(31,0);

	if(int_event & CSI2_EVENT_SEQ_FRAME_FATAL)
		csi2_reg->INT_MSK_SEQ_FRAME_FATAL |= MASK(31,0);

	if(int_event & CSI2_EVENT_CRC_FRAME_FATAL)
		csi2_reg->INT_MSK_CRC_FRAME_FATAL |= MASK(31,0);

	if(int_event & CSI2_EVENT_PLD_CRC_FATAL)
		csi2_reg->INT_MSK_PLD_CRC_FATAL |= MASK(31,0);

	if(int_event & CSI2_EVENT_DATA_ID)
		csi2_reg->INT_MSK_DATA_ID |= MASK(31,0);

	if(int_event & CSI2_EVENT_ECC_CORRECT)
		csi2_reg->INT_MSK_ECC_CORRECT |= MASK(31,0);

	return ARM_DRIVER_OK;
}


/**
  \fn          void MIPI_CSI2_ISR (ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       MIPI CSI2 interrupt service routine
  \param[in]   csi2  Pointer to CSI2 resources
*/
static void MIPI_CSI2_ISR (ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	MIPI_CSI2_Type *csi2_reg = (MIPI_CSI2_Type *)csi2->reg_base;

	uint32_t global_EVENT_status = 0, i_sts = 0;

	global_EVENT_status = csi2_reg->INT_ST_MAIN;

	if(global_EVENT_status & CSI2_STATUS_INT_PHY_FATAL_Msk)
	{
		i_sts = csi2_reg->INT_ST_PHY_FATAL;
		csi2->cb_event(CSI2_EVENT_PHY_FATAL, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_PKT_FATAL_Msk)
	{
		i_sts = csi2_reg->INT_ST_PKT_FATAL;
		csi2->cb_event(CSI2_EVENT_PKT_FATAL, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_BNDRY_FRAME_FATAL_Msk)
	{
		i_sts = csi2_reg->INT_ST_BNDRY_FRAME_FATAL;
		csi2->cb_event(CSI2_EVENT_BNDRY_FRAME_FATAL, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_SEQ_FRAME_FATAL_Msk)
	{
		i_sts = csi2_reg->INT_ST_SEQ_FRAME_FATAL;
		csi2->cb_event(CSI2_EVENT_SEQ_FRAME_FATAL, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_CRC_FRAME_FATAL_Msk)
	{
		i_sts = csi2_reg->INT_ST_CRC_FRAME_FATAL;
		csi2->cb_event(CSI2_EVENT_CRC_FRAME_FATAL, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_PLD_CRC_FATAL_Msk)
	{
		i_sts = csi2_reg->INT_ST_PLD_CRC_FATAL;
		csi2->cb_event(CSI2_EVENT_PLD_CRC_FATAL, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_DATA_ID_Msk)
	{
		i_sts = csi2_reg->INT_ST_DATA_ID;
		csi2->cb_event(CSI2_EVENT_DATA_ID, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_ECC_CORRECTED_Msk)
	{
		i_sts = csi2_reg->INT_ST_ECC_CORRECT;
		csi2->cb_event(CSI2_EVENT_ECC_CORRECT, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_PHY_Msk)
	{
		i_sts = csi2_reg->INT_ST_PHY;
		csi2->cb_event(CSI2_EVENT_PHY, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_LINE_Msk)
	{
		i_sts = csi2_reg->INT_ST_LINE;
		csi2->cb_event(CSI2_EVENT_LINE, i_sts);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_IPI_FATAL_Msk)
	{
		i_sts = csi2_reg->INT_ST_IPI_FATAL;
		csi2->cb_event(CSI2_EVENT_IPI_FATAL, i_sts);
	}

}

/**
  \fn          int32_t CSI2_StartPHY (ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       Configure CSI2 PHY Interface.
  \param[in]   csi2  Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_StartPHY (ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	int32_t ret = 0;

	if((!(csi2->flags & CSI2_FLAG_CLKSRC_ENABLED)) && (!(csi2->flags & CSI2_FLAG_HOST_CONFIGURED)))
		return ARM_DRIVER_ERROR;

	ret = Driver_MIPI_DPHY0.Setup();
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t  CSI2_ConfigureIPI (ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       Configure CSI2 IPI Interface.
  \param[in]   csi2  Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_ConfigureIPI (ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	MIPI_CSI2_Type *csi2_reg = (MIPI_CSI2_Type *)csi2->reg_base;
	ARM_MIPI_CSI2_IPI_INFO * ipi_info = (ARM_MIPI_CSI2_IPI_INFO *) csi2->ipi_info;
	ARM_MIPI_CSI2_FRAME_INFO *frame_info = (ARM_MIPI_CSI2_FRAME_INFO *) ipi_info->frame_info;
	if((!(csi2->flags & CSI2_FLAG_CLKSRC_ENABLED)) && (!(csi2->flags & CSI2_FLAG_HOST_CONFIGURED)))
		return ARM_DRIVER_ERROR;

	if(ipi_info->ipi_mode == MIPI_CSI2_IPI_CAMMODE)
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_MODE, CSI2_IPI_MODE_Msk);
	else
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_MODE, CSI2_IPI_MODE_Msk);

	if(ipi_info->ipi_color_cop == MIPI_CSI2_IPI_COLOR48)
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_MODE, CSI2_IPI_COLOR_COM_Msk);
	else
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_MODE, CSI2_IPI_COLOR_COM_Msk);

	if(ipi_info->ipi_memflush == MIPI_CSI2_IPI_AUTO)
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_MEM_FLUSH, CSI2_IPI_AUTO_FLUSH_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_MEM_FLUSH, CSI2_IPI_AUTO_FLUSH_Msk);


	csi2_reg->IPI_VCID &= ~CSI2_IPI_VCID_Msk;
	csi2_reg->IPI_VCID |= _VAL2FLD(CSI2_IPI_VCID, csi2->vc_id);

	csi2_reg->IPI_DATA_TYPE &= ~CSI2_IPI_DATA_TYPE_Msk;
	csi2_reg->IPI_DATA_TYPE |= _VAL2FLD(CSI2_IPI_DATA_TYPE, mipi_csi2_data_type[csi2->pixel_data_type]);

	csi2_reg->IPI_HSA_TIME &= ~CSI2_IPI_HSA_TIME_Msk;
	csi2_reg->IPI_HSA_TIME |= _VAL2FLD(CSI2_IPI_HSA_TIME, frame_info->hsa_time);

	csi2_reg->IPI_HBP_TIME &= ~CSI2_IPI_HBP_TIME_Msk;
	csi2_reg->IPI_HBP_TIME |= _VAL2FLD(CSI2_IPI_HBP_TIME, frame_info->hbp_time);

	csi2_reg->IPI_HSD_TIME &= ~CSI2_IPI_HSD_TIME_Msk;
	csi2_reg->IPI_HSD_TIME |= _VAL2FLD(CSI2_IPI_HSD_TIME, frame_info->hsd_time);

	csi2_reg->IPI_HLINE_TIME &= ~CSI2_IPI_HLINE_TIME_Msk;
	csi2_reg->IPI_HLINE_TIME |= _VAL2FLD(CSI2_IPI_HLINE_TIME, (frame_info->hactive_time
                                                            + frame_info->hsa_time
                                                            + frame_info->hbp_time
                                                            + frame_info->hsd_time));

	csi2_reg->IPI_VSA_LINES &= ~CSI2_IPI_VSA_LINE_Msk;
	csi2_reg->IPI_VSA_LINES |= _VAL2FLD(CSI2_IPI_VSA_LINE, frame_info->vsa_line);

	csi2_reg->IPI_VBP_LINES &= ~CSI2_IPI_VBP_LINE_Msk;
	csi2_reg->IPI_VBP_LINES |= _VAL2FLD(CSI2_IPI_VBP_LINE, frame_info->vbp_line);

	csi2_reg->IPI_VFP_LINES &= ~CSI2_IPI_VFP_LINE_Msk;
	csi2_reg->IPI_VFP_LINES |= _VAL2FLD(CSI2_IPI_VFP_LINE, frame_info->vfp_line);

	csi2_reg->IPI_VACTIVE_LINES &= ~CSI2_IPI_VACTIVE_LINE_Msk;
	csi2_reg->IPI_VACTIVE_LINES |= _VAL2FLD(CSI2_IPI_VACTIVE_LINE, frame_info->vactive_line);

	MIPI_CSI2_SET_BIT(csi2_reg->CSI2_RESETN,CSI2_RESETN_Msk);

	csi2->flags |= CSI2_FLAG_IPI_CONFIGURED;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t  CSI2_StartIPI (ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       Enable CSI2 IPI Interface.
  \param[in]   csi2  Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_StartIPI (ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	MIPI_CSI2_Type *csi2_reg = (MIPI_CSI2_Type *)csi2->reg_base;
	if (!(csi2->flags & CSI2_FLAG_IPI_CONFIGURED))
		return ARM_DRIVER_ERROR;

	MIPI_CSI2_SET_BIT(csi2_reg->IPI_MODE, CSI2_IPI_ENABLE_Msk);

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t  CSI2_StopIPI (ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       Disable CSI2 IPI Interface.
  \param[in]   csi2  Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_StopIPI (ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	MIPI_CSI2_Type *csi2_reg = (MIPI_CSI2_Type *)csi2->reg_base;
	if (!(csi2->flags & CSI2_FLAG_IPI_CONFIGURED))
		return ARM_DRIVER_ERROR;

	MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_MODE, CSI2_IPI_ENABLE_Msk);

	return ARM_DRIVER_OK;
}

#if (RTE_MIPI_CSI2)
/**
  \fn          int32_t  MIPI_CSI2_Initialize (ARM_MIPI_CSI2_SignalEvent_t cb_event,
                                              uint32_t frequency)
  \brief       Initialize MIPI CSI2 Interface.
  \param[in]   Pointer to ARM_MIPI_CSI2_SignalEvent_t
  \param[in]   frequency to configure DPHY PLL.
  \return      \ref execution_status
*/
static int32_t MIPI_CSI2_Initialize (ARM_MIPI_CSI2_SignalEvent_t cb_event,
		                             uint32_t frequency)
{
	return CSI2_Initialize(cb_event, frequency, &CSI2);
}

/**
  \fn          int32_t MIPI_CSI2_Uninitialize (void)
  \brief       uninitialize MIPI CSI2 Interface.
  \return      \ref execution_status
*/
static int32_t MIPI_CSI2_Uninitialize (void)
{
	return CSI2_Uninitialize(&CSI2);
}

/**
  \fn          int32_t MIPI_CSI2_PowerControl (ARM_POWER_STATE state)
  \brief       Control CSI2 Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t MIPI_CSI2_PowerControl (ARM_POWER_STATE state)
{
	return CSI2_PowerControl (state, &CSI2);
}

/**
  \fn          int32_t MIPI_CSI2_ConfigureHost (uint32_t int_even)
  \brief       Configure CSI2 Host Interface.
  \param[in]   int_event interrupt event to be enabled.
  \return      \ref execution_status
*/
static int32_t MIPI_CSI2_ConfigureHost (uint32_t int_event)
{
	return CSI2_ConfigureHost (int_event, &CSI2);
}

/**
  \fn          int32_t MIPI_CSI2_ConfigureIPI (void)
  \brief       Configure CSI2 IPI Interface.
  \return      \ref execution_status
*/
static int32_t MIPI_CSI2_ConfigureIPI (void)
{
	return CSI2_ConfigureIPI(&CSI2);
}

/**
  \fn          int32_t MIPI_CSI2_StartPHY (void)
  \brief       Configure CSI2 PHY Interface.
  \return      \ref execution_status
*/
static int32_t MIPI_CSI2_StartPHY (void)
{
	return CSI2_StartPHY(&CSI2);
}

/**
  \fn          int32_t MIPI_CSI2_StartIPI(void)
  \brief       Enable CSI2 IPI Interface.
  \return      \ref execution_status
*/
static int32_t MIPI_CSI2_StartIPI(void)
{
	return CSI2_StartIPI(&CSI2);
}

/**
  \fn          int32_t  MIPI_CSI2_StopIPI(void)
  \brief       Disable CSI2 IPI Interface.
  \return      \ref execution_status
*/
static int32_t MIPI_CSI2_StopIPI(void)
{
	return CSI2_StopIPI(&CSI2);
}

/**
  \fn          void MIPI_CSI2_IRQHandler (void)
  \brief       CSI2 IRQ Handler.
*/
void MIPI_CSI2_IRQHandler (void)
{
	MIPI_CSI2_ISR(&CSI2);
}

/**
\brief Access structure of the  MIPI CSI2 Driver.
*/
extern ARM_DRIVER_MIPI_CSI2 Driver_MIPI_CSI2;

ARM_DRIVER_MIPI_CSI2 Driver_MIPI_CSI2 =
{
	MIPI_CSI2_GetVersion,
	MIPI_CSI2_GetCapabilities,
	MIPI_CSI2_Initialize,
	MIPI_CSI2_Uninitialize,
	MIPI_CSI2_PowerControl,
	MIPI_CSI2_ConfigureHost,
	MIPI_CSI2_StartPHY,
	MIPI_CSI2_ConfigureIPI,
	MIPI_CSI2_StartIPI,
	MIPI_CSI2_StopIPI,
};
#endif

