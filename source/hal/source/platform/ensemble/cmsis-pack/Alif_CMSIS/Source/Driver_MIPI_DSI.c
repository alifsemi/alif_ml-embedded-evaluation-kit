/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

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
#include "MIPI_DSI_dev.h"
#include "RTE_Device.h"
#include "DPHY_init.h"
#include "display.h"
#include "DSI_LCD_Panels.h"

#define ARM_MIPI_DSI_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /*driver version*/

#if !(RTE_MIPI_DSI)
#error "MIPI DSI is not enabled in the RTE_Device.h"
#endif

#if (!defined(RTE_Drivers_MIPI_DSI))
#error "MIPI DSI not configured in RTE_Components.h!"
#endif

/*Driver Version*/
static const ARM_DRIVER_VERSION DriverVersion =
{
	ARM_MIPI_DSI_API_VERSION,
	ARM_MIPI_DSI_DRV_VERSION
};

#if (RTE_MIPI_DSI)
MIPI_DSI_FRAME_INFO DSI_FRAME_INFO;

MIPI_DSI_DPI_POL DPI_POL =
{
	.dataen_active_low = RTE_MIPI_DSI_DPI_DATAEN_ACTIVE_LOW,
	.vsync_active_low  = RTE_MIPI_DSI_DPI_VSYNC_ACTIVE_LOW,
	.hsync_active_low  = RTE_MIPI_DSI_DPI_HSYNC_ACTIVE_LOW,
	.shutd_active_low  = RTE_MIPI_DSI_DPI_SHUTD_ACTIVE_LOW,
	.colorm_active_low = RTE_MIPI_DSI_DPI_COLORM_ACTIVE_LOW,
};

MIPI_DSI_DPI_CFG DPI_CFG =
{
	.color_code     = RTE_MIPI_DSI_DPI_COLOR_CODE,
	.vid_mode       = RTE_MIPI_DSI_VID_MODE_TYPE,
	.vid_num_chunks = RTE_MIPI_DSI_VID_NUM_CHUNKS,
	.vid_null_size  = RTE_MIPI_DSI_VID_NULL_SIZE,
	.dpi_pol        = &DPI_POL,
	.frame_info     = &DSI_FRAME_INFO,
};

MIPI_DSI_DPHY_TMR_CFG DPHY_TMR_CFG =
{
	.clklp2hs_time = RTE_MIPI_DSI_PHY_CLKLP2HS_TIME,
	.clkhs2lp_time = RTE_MIPI_DSI_PHY_CLKHS2LP_TIME,
	.lp2hs_time    = RTE_MIPI_DSI_PHY_LP2HS_TIME,
	.hs2lp_time    = RTE_MIPI_DSI_PHY_HS2LP_TIME,
};

MIPI_DSI_DRV_INFO DSI =
{
	.reg_base       = MIPI_DSI,
	.n_lanes        = RTE_MIPI_DSI_N_LANES,
	.vc_id          = RTE_MIPI_DSI_VC_ID,
	.tx_ecs_clk_div = RTE_MIPI_DSI_TX_ESC_CLK_DIVISION,
	.dphy_tmr       = &DPHY_TMR_CFG,
	.dpi_cfg        = &DPI_CFG,
	.irq            = (IRQn_Type)MIPI_DSI_IRQ,
	.irq_priority   = RTE_MIPI_DSI_IRQ_PRI,
	.flags          = 0,
};
#endif
//
//  Functions
//
/**
  \fn          void DSI_DCS_Short_Write (uint8_t cmd, uint8_t data)
  \brief       Perform MIPI DSI DCS Short write.
  \param[in]   cmd is DCS command info.
  \param[in]   data to send.
*/
void DSI_DCS_Short_Write (uint8_t cmd, uint8_t data)
{
	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)DSI.reg_base;

	dsi_reg->GEN_HDR = _VAL2FLD(DSI_GEN_DT, DSI_DCS_SHORT_WRITE_DATA_TYPE) | \
                          _VAL2FLD(DSI_GEN_VC, DSI.vc_id)                     | \
                          _VAL2FLD(DSI_GEN_WC_LSBYTE, cmd)                    | \
                          _VAL2FLD(DSI_GEN_WC_MSBYTE, data);

	PMU_delay_loop_us(100);
}

/**
  \fn          void DSI_DCS_CMD_Short_Write (uint8_t cmd)
  \brief       Perform MIPI DSI DCS Short write only command.
  \param[in]   cmd is DCS command info.
*/
void DSI_DCS_CMD_Short_Write (uint8_t cmd)
{
	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)DSI.reg_base;

	dsi_reg->GEN_HDR = _VAL2FLD(DSI_GEN_DT, DSI_DCS_SHORT_WRITE_NODATA_TYPE) | \
                          _VAL2FLD(DSI_GEN_VC, DSI.vc_id)                     | \
                          _VAL2FLD(DSI_GEN_WC_LSBYTE, cmd);

	PMU_delay_loop_us(100);
}

/**
  \fn          void DSI_DCS_Long_Write (uint8_t cmd, uint32_t data)
  \brief       Perform MIPI DSI DCS Short write.
  \param[in]   cmd is DCS command info.
  \param[in]   data of four bytes to send.
*/
void DSI_DCS_Long_Write (uint8_t cmd, uint32_t data)
{
	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)DSI.reg_base;

	dsi_reg->GEN_PLD_DATA = _VAL2FLD(DSI_GEN_PLD_B1,cmd)       | \
                               _VAL2FLD(DSI_GEN_PLD_B2,cmd)       | \
                               _VAL2FLD(DSI_GEN_PLD_B3, data)     | \
                               _VAL2FLD(DSI_GEN_PLD_B4, data >> 8);

	dsi_reg->GEN_PLD_DATA = _VAL2FLD(DSI_GEN_PLD_B1, data >> 16)  | \
                               _VAL2FLD(DSI_GEN_PLD_B2, data >> 24);

	dsi_reg->GEN_HDR = _VAL2FLD(DSI_GEN_DT, DSI_DCS_LONG_WRITE_DATA_TYPE) | \
                          _VAL2FLD(DSI_GEN_VC, DSI.vc_id)                    | \
                          _VAL2FLD(DSI_GEN_WC_LSBYTE, DSI_DCS_LONG_WRITE_DATA_LEN);

	PMU_delay_loop_us(100);
}

/* Driver Capabilities */
static const ARM_MIPI_DSI_CAPABILITIES DriverCapabilities =
{
	0, /* Not supports reentrant_operation */
	1, /* DPI interface supported*/
	0, /* DBI interface not supported*/
	0  /* reserved (must be zero) */
};


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
  \fn          ARM_MIPI_DSI_CAPABILITIES MIPI_DSI_GetCapabilities (void)
  \brief       Get MIPI DSI driver capabilities
  \return      \ref ARM_MIPI_DPHY_CAPABILITIES
*/
static ARM_MIPI_DSI_CAPABILITIES ARM_MIPI_DSI_GetCapabilities (void)
{
	return DriverCapabilities;
}

/**
  \fn          int32_t DSI_Initialize (ARM_MIPI_DSI_SignalEvent_t cb_event,
                                       DISPLAY_PANEL_DEVICE *display_panel,
                                       MIPI_DSI_DRV_INFO *dsi)
  \brief       Initialize MIPI DSI Interface.
  \param[in]   cb_event Pointer to ARM_MIPI_DSI_SignalEvent_t.
  \param[in]   display_panel Pointer to display panel resources.
  \param[in]   dsi Pointer to DSI resources.
  \return      \ref execution_status.
  */
static int32_t DSI_Initialize (ARM_MIPI_DSI_SignalEvent_t cb_event,
                               DISPLAY_PANEL_DEVICE *display_panel,
                               MIPI_DSI_DRV_INFO *dsi)
{
	int32_t ret = ARM_DRIVER_OK;
	DISPLAY_PANEL_FRAME_INFO *frame_info = display_panel->Frame_info;
	uint32_t pixclk, htotal, vtotal;
	uint32_t frequency;

	if(dsi->flags & DSI_FLAG_DRV_INIT_DONE)
		return ARM_DRIVER_OK;

	if (!cb_event)
		return ARM_DRIVER_ERROR_PARAMETER;

	dsi->cb_event = cb_event;

	/* LCD Manufacturer provides the Frame timing values
	 *     HTOTAL = WIDTH + HSYNC + HFP + HBP
	 *     VTOTAL = HEIGHT + VSYNC + VFP + VBP
	 * Calculate the pixel clock for DPI controller
	 *     PIXCLK = FPS x HTOTAL x VTOTAL
	 * Calculate the pixel clock divider
	 *     PIXCLK_DIV = CDC200_PIXCLK_SOURCE / PIXCLK
	 */
	htotal = (frame_info->hsync_time
              + frame_info->hbp_time
              + frame_info->hfp_time
              + frame_info->hactive_time);

	vtotal = (frame_info->vsync_line
              + frame_info->vbp_line
              + frame_info->vfp_line
              + frame_info->vactive_line);

	pixclk = (htotal * vtotal * RTE_CDC200_DPI_FPS);

	/* SCALE = LANEBYTECLK / PIXCLK
	 * MIPI data rate must be exactly equal, not greater than, for 1.5 scale to work
	 * MIPI data rate + 33% allows for scaling times 2
	 *    24 x 1.333 / 16 = 2
	 * LANEBYTECLK = PIXCLK * SCALE
	 * lanebyteclk frequency is 1/4th of the DPHY frequency
	 * PLL frequency = LANEBYTECLK * 4
	 *               = PIXCLK * SCALE * 4
	 */
	frequency = pixclk * 2 * 4;

	/*Checking LCD Panel supports MIPI DSI DPHY data rate*/
	if((frequency * 2) > (display_panel->Info->max_bitrate * 1000000))
		return ARM_DRIVER_ERROR_PARAMETER;

	/*DPHY initialization*/
	ret  = DSI_DPHY_Initialize(frequency);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/*LCD Panel Initialization*/
	ret = display_panel->Ops->Init();
	if(ret != ARM_DRIVER_OK)
		return ret;

	dsi->dpi_cfg->vid_pkt_size = display_panel->Frame_info->hactive_time;
	dsi->dpi_cfg->frame_info->hsa_time = display_panel->Frame_info->hsync_time << 1;
	dsi->dpi_cfg->frame_info->hbp_time = display_panel->Frame_info->hbp_time << 1;
	dsi->dpi_cfg->frame_info->hfp_time = display_panel->Frame_info->hfp_time << 1;
	dsi->dpi_cfg->frame_info->hactive_time = display_panel->Frame_info->hactive_time << 1;
	dsi->dpi_cfg->frame_info->vsa_line = display_panel->Frame_info->vsync_line;
	dsi->dpi_cfg->frame_info->vbp_line = display_panel->Frame_info->vbp_line;
	dsi->dpi_cfg->frame_info->vfp_line = display_panel->Frame_info->vfp_line;
	dsi->dpi_cfg->frame_info->vactive_line = display_panel->Frame_info->vactive_line;

	dsi->flags = DSI_FLAG_DRV_INIT_DONE;

	return ret;
}

/**
  \fn          int32_t DSI_Uninitialize (DISPLAY_PANEL_DEVICE *display_panel,
                                         MIPI_DSI_DRV_INFO *dsi)
  \brief       uninitialize MIPI DSI Interface.
  \param[in]   display_panel Pointer to display panel resources.
  \param[in]   dsi Pointer to DSI resources.
  \return      \ref execution_status.
  */
static int32_t DSI_Uninitialize (DISPLAY_PANEL_DEVICE *display_panel,
                                 MIPI_DSI_DRV_INFO *dsi)
{
	int32_t ret = ARM_DRIVER_OK;
	dsi->cb_event = NULL;
	dsi->flags = 0;

	/*DPHY Uninitialization*/
	ret  = DSI_DPHY_Uninitialize();
	if(ret != ARM_DRIVER_OK)
		return ret;

	/*LCD Panel Un-Initialization*/
	ret = display_panel->Ops->Uninit();
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ret;
}

/**
  \fn          int32_t DSI_PowerControl (ARM_POWER_STATE state, MIPI_DSI_DRV_INFO *dsi)
  \brief       Control DSI Interface Power.
  \param[in]   state  Power state.
  \param[in]   dsi Pointer to DSI resources.
  \return      \ref execution_status.
  */
static int32_t DSI_PowerControl (ARM_POWER_STATE state, MIPI_DSI_DRV_INFO *dsi)
{

	switch (state)
	{
		case ARM_POWER_OFF:

			NVIC_DisableIRQ (dsi->irq);
			NVIC_ClearPendingIRQ (dsi->irq);

			dsi->flags &= ~DSI_FLAG_POWER_ENABLED;
			break;

		case ARM_POWER_FULL:

			if (!(dsi->flags & DSI_FLAG_DRV_INIT_DONE))
				return ARM_DRIVER_ERROR;
			if (dsi->flags & DSI_FLAG_POWER_ENABLED)
				return ARM_DRIVER_OK;

			NVIC_ClearPendingIRQ (dsi->irq);
			NVIC_SetPriority (dsi->irq, dsi->irq_priority);
			NVIC_EnableIRQ (dsi->irq);

			dsi->flags |= DSI_FLAG_POWER_ENABLED;

			break;

		case ARM_POWER_LOW:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DSI_ConfigureHost (MIPI_DSI_DRV_INFO *dsi)
  \brief       Configure DSI Host Interface.
  \param[in]   dsi  Pointer to DSI resources.
  \return      \ref execution_status.
  */
static int32_t DSI_ConfigureHost (MIPI_DSI_DRV_INFO *dsi)
{
	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)dsi->reg_base;

	if(!(dsi->flags & DSI_FLAG_POWER_ENABLED))
		return ARM_DRIVER_ERROR;

	dsi_reg->INT_MSK0 = DSI_ACK_WITH_ERR_0_Msk  | \
                           DSI_ACK_WITH_ERR_1_Msk  | \
                           DSI_ACK_WITH_ERR_2_Msk  | \
                           DSI_ACK_WITH_ERR_3_Msk  | \
                           DSI_ACK_WITH_ERR_4_Msk  | \
                           DSI_ACK_WITH_ERR_5_Msk  | \
                           DSI_ACK_WITH_ERR_6_Msk  | \
                           DSI_ACK_WITH_ERR_7_Msk  | \
                           DSI_ACK_WITH_ERR_8_Msk  | \
                           DSI_ACK_WITH_ERR_9_Msk  | \
                           DSI_ACK_WITH_ERR_10_Msk | \
                           DSI_ACK_WITH_ERR_11_Msk | \
                           DSI_ACK_WITH_ERR_12_Msk | \
                           DSI_ACK_WITH_ERR_13_Msk | \
                           DSI_ACK_WITH_ERR_14_Msk | \
                           DSI_ACK_WITH_ERR_15_Msk | \
                           DSI_DPHY_ERRORS_0_Msk   | \
                           DSI_DPHY_ERRORS_1_Msk   | \
                           DSI_DPHY_ERRORS_2_Msk   | \
                           DSI_DPHY_ERRORS_3_Msk   | \
                           DSI_DPHY_ERRORS_4_Msk;

	dsi_reg->INT_MSK1 = DSI_TO_HS_TX_Msk          | \
                           DSI_TO_LP_RX_Msk          | \
                           DSI_ECC_SINGLE_ERR_Msk    | \
                           DSI_ECC_MILTI_ERR_Msk     | \
                           DSI_CRC_ERR_Msk           | \
                           DSI_PKT_SIZE_ERR_Msk      | \
                           DSI_EOPT_ERR_Msk          | \
                           DSI_DPI_PLD_WR_ERR_Msk    | \
                           DSI_GEN_CMD_WR_ERR_Msk    | \
                           DSI_GEN_PLD_WR_ERR_Msk    | \
                           DSI_GEN_PLD_SEND_ERR_Msk  | \
                           DSI_GEN_PLD_RD_ERR_Msk    | \
                           DSI_GEN_PLD_RECEV_ERR_Msk | \
                           DSI_TEAR_REQUEST_ERR_Msk;

	dsi->flags |= DSI_FLAG_HOST_CONFIGURED;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t  DSI_ConfigureDPI (MIPI_DSI_DRV_INFO *dsi)
  \brief       Configure DSI DPI Interface.
  \param[in]   dsi  Pointer to DSI resources.
  \return      \ref execution_status.
*/
static int32_t DSI_ConfigureDPI (MIPI_DSI_DRV_INFO *dsi)
{
	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)dsi->reg_base;
	MIPI_DSI_DPI_CFG *dpi_cfg = (MIPI_DSI_DPI_CFG *)dsi->dpi_cfg;
	MIPI_DSI_DPI_POL *dpi_pol = (MIPI_DSI_DPI_POL *)dsi->dpi_cfg->dpi_pol;
	MIPI_DSI_FRAME_INFO *frame_info = (MIPI_DSI_FRAME_INFO *)dsi->dpi_cfg->frame_info;
	if(!((dsi->flags & DSI_FLAG_POWER_ENABLED) && (dsi->flags & DSI_FLAG_HOST_CONFIGURED)))
		return ARM_DRIVER_ERROR;

	dsi_reg->DPI_COLOR_CODING &= ~DSI_DPI_COLOR_CODING_Msk;
	dsi_reg->DPI_COLOR_CODING |= _VAL2FLD(DSI_DPI_COLOR_CODING, dpi_cfg->color_code);

	dsi_reg->DPI_CFG_POL &= ~(DSI_DATAEN_ACTIVE_LOW_Msk | \
                                 DSI_VSYNC_ACTIVE_LOW_Msk  | \
                                 DSI_HSYNC_ACTIVE_LOW_Msk  | \
                                 DSI_SHUTD_ACTIVE_LOW_Msk  | \
                                 DSI_COLORM_ACTIVE_LOW_Msk);

	if(dpi_pol->dataen_active_low == MIPI_DSI_ACTIVE_LOW)
		MIPI_DSI_SET_BIT(dsi_reg->DPI_CFG_POL, DSI_DATAEN_ACTIVE_LOW_Msk);

	if(dpi_pol->vsync_active_low == MIPI_DSI_ACTIVE_LOW)
		MIPI_DSI_SET_BIT(dsi_reg->DPI_CFG_POL, DSI_VSYNC_ACTIVE_LOW_Msk);

	if(dpi_pol->hsync_active_low == MIPI_DSI_ACTIVE_LOW)
		MIPI_DSI_SET_BIT(dsi_reg->DPI_CFG_POL, DSI_HSYNC_ACTIVE_LOW_Msk);

	if(dpi_pol->shutd_active_low == MIPI_DSI_ACTIVE_LOW)
		MIPI_DSI_SET_BIT(dsi_reg->DPI_CFG_POL, DSI_SHUTD_ACTIVE_LOW_Msk);

	if(dpi_pol->colorm_active_low == MIPI_DSI_ACTIVE_LOW)
		MIPI_DSI_SET_BIT(dsi_reg->DPI_CFG_POL, DSI_COLORM_ACTIVE_LOW_Msk);

	dsi_reg->VID_PKT_SIZE &= ~DSI_VID_PKT_SIZE_Msk;
	dsi_reg->VID_PKT_SIZE |= _VAL2FLD(DSI_VID_PKT_SIZE, dpi_cfg->vid_pkt_size);

	dsi_reg->VID_NUM_CHUNKS &= ~DSI_VID_NUM_CHUNKS_Msk;
	dsi_reg->VID_NUM_CHUNKS |= _VAL2FLD(DSI_VID_NUM_CHUNKS, dpi_cfg->vid_num_chunks);

	dsi_reg->VID_NULL_SIZE &= ~DSI_VID_NULL_SIZE_Msk;
	dsi_reg->VID_NULL_SIZE |= _VAL2FLD(DSI_VID_NULL_SIZE, dpi_cfg->vid_null_size);

	dsi_reg->VID_HSA_TIME &= ~DSI_VID_HSA_TIME_Msk;
	dsi_reg->VID_HSA_TIME |= _VAL2FLD(DSI_VID_HSA_TIME, frame_info->hsa_time);

	dsi_reg->VID_HBP_TIME &= DSI_VID_HBP_TIME_Msk;
	dsi_reg->VID_HBP_TIME |= _VAL2FLD(DSI_VID_HBP_TIME, frame_info->hbp_time);

	dsi_reg->VID_HLINE_TIME &= DSI_VID_HLINE_TIME_Msk;
	dsi_reg->VID_HLINE_TIME |= _VAL2FLD(DSI_VID_HLINE_TIME, (frame_info->hsa_time   \
                                                                + frame_info->hbp_time \
                                                                + frame_info->hfp_time \
                                                                + frame_info->hactive_time));

	dsi_reg->VID_VSA_LINES &= DSI_VSA_LINES_Msk;
	dsi_reg->VID_VSA_LINES |= _VAL2FLD(DSI_VSA_LINES, frame_info->vsa_line);

	dsi_reg->VID_VBP_LINES &= DSI_VBP_LINES_Msk;
	dsi_reg->VID_VBP_LINES |= _VAL2FLD(DSI_VBP_LINES, frame_info->vbp_line);

	dsi_reg->VID_VFP_LINES &= DSI_VFP_LINES_Msk;
	dsi_reg->VID_VFP_LINES |= _VAL2FLD(DSI_VFP_LINES, frame_info->vfp_line);

	dsi_reg->VID_VACTIVE_LINES &= DSI_V_ACTIVE_LINES_Msk;
	dsi_reg->VID_VACTIVE_LINES |= _VAL2FLD(DSI_V_ACTIVE_LINES, frame_info->vactive_line);

	dsi->flags |= DSI_FLAG_DPI_CONFIGURED;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t DSI_Control (ARM_MIPI_DSI_CONTROL control,
                                    uint32_t arg,
                                    MIPI_DSI_DRV_INFO *dsi)
  \brief       Control DSI Interface.
  \param[in]   control DSI host and DPI Configuration.
  \param[in]   arg Argument of operation (optional)
  \param[in]   dsi Pointer to DSI resources.
  \return      \ref execution_status.
  */
static int32_t DSI_Control (ARM_MIPI_DSI_CONTROL control,
                            uint32_t arg,
                            MIPI_DSI_DRV_INFO *dsi)
{
	int32_t ret = ARM_DRIVER_OK;

	switch(control)
	{
		case DSI_CONFIGURE_HOST:
			ret = DSI_ConfigureHost(dsi);
			if(ret != ARM_DRIVER_OK)
				return ret;
			break;
		case DSI_CONFIGURE_DPI:
			ret = DSI_ConfigureDPI(dsi);
			if(ret != ARM_DRIVER_OK)
				return ret;
			break;
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ret;
}

/**
  \fn          int32_t  DSI_StartCommandMode (DISPLAY_PANEL_DEVICE *display_panel,
                                              MIPI_DSI_DRV_INFO *dsi)
  \brief       Configure DSI to start Command mode.
  \param[in]   display_panel Pointer to display panel resources.
  \param[in]   dsi  Pointer to DSI resources.
  \return      \ref execution_status.
*/
static int32_t DSI_StartCommandMode (DISPLAY_PANEL_DEVICE *display_panel,
                                     MIPI_DSI_DRV_INFO *dsi)
{
	int32_t ret  = ARM_DRIVER_OK;

	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)dsi->reg_base;
	MIPI_DSI_DPI_CFG *dpi_cfg = (MIPI_DSI_DPI_CFG *)dsi->dpi_cfg;
	MIPI_DSI_DPHY_TMR_CFG *dphy_tmr = (MIPI_DSI_DPHY_TMR_CFG *)dsi->dphy_tmr;

	if(!((dsi->flags & DSI_FLAG_POWER_ENABLED) && (dsi->flags & DSI_FLAG_HOST_CONFIGURED)))
		return ARM_DRIVER_ERROR;

	MIPI_DSI_CLEAR_BIT(dsi_reg->PWR_UP, DSI_SHUTDOWNZ_Msk);

	MIPI_DSI_SET_BIT(dsi_reg->LPCLK_CTRL, DSI_AUTO_CLKLANE_CTRL_Msk);

	dsi_reg->PHY_TMR_LPCLK_CFG &= ~DSI_PHY_CLKLP2HS_TIME_Msk;
	dsi_reg->PHY_TMR_LPCLK_CFG |= _VAL2FLD(DSI_PHY_CLKLP2HS_TIME, dphy_tmr->clklp2hs_time);

	dsi_reg->PHY_TMR_LPCLK_CFG &= ~DSI_PHY_CLKHS2LP_TIME_Msk;
	dsi_reg->PHY_TMR_LPCLK_CFG |= _VAL2FLD(DSI_PHY_CLKHS2LP_TIME, dphy_tmr->clkhs2lp_time);

	dsi_reg->PHY_TMR_CFG &= ~DSI_PHY_LP2HS_TIME_Msk;
	dsi_reg->PHY_TMR_CFG |= _VAL2FLD(DSI_PHY_LP2HS_TIME, dphy_tmr->lp2hs_time);

	dsi_reg->PHY_TMR_CFG &= ~DSI_PHY_HS2LP_TIME_Msk;
	dsi_reg->PHY_TMR_CFG |= _VAL2FLD(DSI_PHY_HS2LP_TIME, dphy_tmr->hs2lp_time);

	MIPI_DSI_SET_BIT(dsi_reg->MODE_CFG, DSI_CMD_VIDEO_MODE_Msk);

	dsi_reg->VID_MODE_CFG &= ~DSI_VID_MODE_TYPE_Msk;
	dsi_reg->VID_MODE_CFG |= _VAL2FLD(DSI_VID_MODE_TYPE, dpi_cfg->vid_mode);

	dsi_reg->VID_MODE_CFG |= DSI_LP_CMD_EN_Msk;

	dsi_reg->CLKMGR_CFG &= ~DSI_TX_ESC_CLK_DIVISION_Msk;
	dsi_reg->CLKMGR_CFG |= _VAL2FLD(DSI_TX_ESC_CLK_DIVISION, dsi->tx_ecs_clk_div);

	dsi_reg->CMD_MODE_CFG = (DSI_GEN_SW_0P_TX_Msk | \
                                DSI_GEN_SW_1P_TX_Msk | \
                                DSI_GEN_SW_2P_TX_Msk | \
                                DSI_GEN_SR_0P_TX_Msk | \
                                DSI_GEN_SR_1P_TX_Msk | \
                                DSI_GEN_SR_2P_TX_Msk | \
                                DSI_GEN_LW_TX_Msk    | \
                                DSI_DCS_SW_0P_TX_Msk | \
                                DSI_DCS_SW_1P_TX_Msk | \
                                DSI_DCS_SR_0P_TX_Msk | \
                                DSI_DCS_LW_TX_Msk    | \
                                DSI_MAX_RD_PKT_SIZE_Msk);

	MIPI_DSI_SET_BIT(dsi_reg->PWR_UP, DSI_SHUTDOWNZ_Msk);

	/*Configure LCD Panel*/
	ret = display_panel->Ops->Control(DISPALY_PANEL_CONFIG);
	if(ret != ARM_DRIVER_OK)
		return ret;

	dsi->flags |= DSI_FLAG_CMD_MODE_DONE;

	return ret;
}

/**
  \fn          int32_t  DSI_StartVideoMode (DISPLAY_PANEL_DEVICE *display_panel,
                                            MIPI_DSI_DRV_INFO *dsi)
  \brief       Configure DSI to start Video mode.
  \param[in]   display_panel Pointer to display panel resources.
  \param[in]   dsi  Pointer to DSI resources.
  \return      \ref execution_status.
*/
static int32_t DSI_StartVideoMode (DISPLAY_PANEL_DEVICE *display_panel,
                                   MIPI_DSI_DRV_INFO *dsi)
{
	int32_t ret  = ARM_DRIVER_OK;

	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)dsi->reg_base;

	if(!((dsi->flags & DSI_FLAG_CMD_MODE_DONE) && (dsi->flags & DSI_FLAG_DPI_CONFIGURED)))
		return ARM_DRIVER_ERROR;

	MIPI_DSI_CLEAR_BIT(dsi_reg->PWR_UP, DSI_SHUTDOWNZ_Msk);

	MIPI_DSI_CLEAR_BIT(dsi_reg->LPCLK_CTRL, DSI_AUTO_CLKLANE_CTRL_Msk);
	MIPI_DSI_SET_BIT(dsi_reg->LPCLK_CTRL, DSI_PHY_TXREQUESTCLKHS_Msk);

	MIPI_DSI_CLEAR_BIT(dsi_reg->MODE_CFG, DSI_CMD_VIDEO_MODE_Msk);

	MIPI_DSI_SET_BIT(dsi_reg->PWR_UP, DSI_SHUTDOWNZ_Msk);

	/*Start LCD Panel*/
	ret = display_panel->Ops->Start();
	if(ret != ARM_DRIVER_OK)
		return ret;

	dsi->flags |= DSI_FLAG_VID_MODE_DONE;

	return ret;
}

/**
  \fn          int32_t  DSI_Stop (DISPLAY_PANEL_DEVICE *display_panel,
                                  MIPI_DSI_DRV_INFO *dsi)
  \brief       Shutdown DSI.
  \param[in]   display_panel Pointer to display panel resources.
  \param[in]   dsi  Pointer to DSI resources
  \return      \ref execution_status
*/
static int32_t DSI_Stop (DISPLAY_PANEL_DEVICE *display_panel,
                         MIPI_DSI_DRV_INFO *dsi)
{
	int32_t ret  = ARM_DRIVER_OK;

	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)dsi->reg_base;

	if(!((dsi->flags & DSI_FLAG_VID_MODE_DONE) || (dsi->flags & DSI_FLAG_CMD_MODE_DONE)))
			return ARM_DRIVER_ERROR;

	MIPI_DSI_CLEAR_BIT(dsi_reg->PWR_UP, DSI_SHUTDOWNZ_Msk);

	/*Stop LCD Panel*/
	ret = display_panel->Ops->Stop();
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ret;
}

/**
  \fn          void MIPI_DSI_ISR (MIPI_DSI_DRV_INFO *dsi)
  \brief       MIPI DSI interrupt service routine
  \param[in]   dsi  Pointer to DSI resources
*/
static void MIPI_DSI_ISR (MIPI_DSI_DRV_INFO *dsi)
{
	MIPI_DSI_Type *dsi_reg = (MIPI_DSI_Type *)dsi->reg_base;

	dsi->int_st0 = dsi_reg->INT_ST0;

	if(dsi->int_st0 & DSI_MASK_PHY_ERROR_EVENT)
		dsi->cb_event(DSI_PHY_ERROR_EVENT);

	if(dsi->int_st0 & DSI_MASK_ACK_ERROR_EVENT)
		dsi->cb_event(DSI_ACK_ERROR_EVENT);

	dsi->int_st1 = dsi_reg->INT_ST1;

	if(dsi->int_st1 & DSI_MASK_PKT_ERROR_EVENT)
		dsi->cb_event(DSI_PKT_ERROR_EVENT);

	if(dsi->int_st1 & DSI_MASK_DPI_ERROR_EVENT)
		dsi->cb_event(DSI_DPI_ERROR_EVENT);
}

#if (RTE_MIPI_DSI)
/**
  \fn          int32_t  ARM_MIPI_DSI_Initialize (ARM_MIPI_DSI_SignalEvent_t cb_event)
  \brief       Initialize MIPI DSI Interface.
  \param[in]   cb_event Pointer to ARM_MIPI_DSI_SignalEvent_t
  \return      \ref execution_status
  */
static int32_t ARM_MIPI_DSI_Initialize (ARM_MIPI_DSI_SignalEvent_t cb_event)
{
	return DSI_Initialize (cb_event, display_panel, &DSI);
}

/**
  \fn          int32_t ARM_MIPI_DSI_Uninitialize (void)
  \brief       uninitialize MIPI DSI Interface.
  \return      \ref execution_status
  */
static int32_t ARM_MIPI_DSI_Uninitialize (void)
{
	return DSI_Uninitialize (display_panel, &DSI);
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
  \fn          int32_t ARM_MIPI_DSI_Control (ARM_MIPI_DSI_CONTROL control, uint32_t arg)
  \brief       Control DSI Interface.
  \param[in]   control DSI host and DPI Configuration.
  \param[in]   arg Argument of operation (optional)
  \return      \ref execution_status
  */
static int32_t ARM_MIPI_DSI_Control (ARM_MIPI_DSI_CONTROL control, uint32_t arg)
{
	return DSI_Control (control, arg, &DSI);
}

/**
  \fn          int32_t  ARM_MIPI_DSI_StartCommandMode (void)
  \brief       Configure DSI to start Command mode.
  \return      \ref execution_status
*/
static int32_t ARM_MIPI_DSI_StartCommandMode (void)
{
	return DSI_StartCommandMode (display_panel, &DSI);
}

/**
  \fn          int32_t  ARM_MIPI_DSI_StartVideoMode (void)
  \brief       Configure DSI to start Video mode.
  \return      \ref execution_status
*/
static int32_t ARM_MIPI_DSI_StartVideoMode (void)
{
	return DSI_StartVideoMode (display_panel, &DSI);
}

/**
  \fn          int32_t  ARM_MIPI_DSI_Stop (void)
  \brief       Shutdown DSI.
  \return      \ref execution_status
*/
static int32_t ARM_MIPI_DSI_Stop (void)
{
	return DSI_Stop (display_panel, &DSI);
}

/**
  \fn          void MIPI_DSI_IRQHandler (void)
  \brief       DSi IRQ Handler.
*/
void MIPI_DSI_IRQHandler(void)
{
	MIPI_DSI_ISR (&DSI);
}

/**
  \brief Access structure of the  MIPI DSI Driver.
  */
extern ARM_DRIVER_MIPI_DSI Driver_MIPI_DSI;

ARM_DRIVER_MIPI_DSI Driver_MIPI_DSI =
{
	ARM_MIPI_DSI_GetVersion,
	ARM_MIPI_DSI_GetCapabilities,
	ARM_MIPI_DSI_Initialize,
	ARM_MIPI_DSI_Uninitialize,
	ARM_MIPI_DSI_PowerControl,
	ARM_MIPI_DSI_Control,
	ARM_MIPI_DSI_StartCommandMode,
	ARM_MIPI_DSI_StartVideoMode,
	ARM_MIPI_DSI_Stop
};
#endif
