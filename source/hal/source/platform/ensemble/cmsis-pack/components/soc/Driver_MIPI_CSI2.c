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
#include "RTE_Device.h"
#include "DPHY_init.h"

#if !(RTE_MIPI_CSI2)
#error "MIPI CSI2 is not enabled in the RTE_Device.h"
#endif

#if (!defined(RTE_Drivers_MIPI_CSI2))
#error "MIPI CSI2 not configured in RTE_Components.h!"
#endif

#define ARM_MIPI_CSI2_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)/*Driver version*/

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

ARM_MIPI_CSI2_IPI_ADV_INFO CSI2_IPI_ADV_FEATURES =
{
	.sync_evnt_mode   = RTE_MIPI_CSI2_SYNC_ET_MODE,
	.event_sel        = RTE_MIPI_CSI2_SYNC_ET_SEL,
	.en_embedded      = RTE_MIPI_CSI2_EN_EMBEDDED,
	.en_blanking      = RTE_MIPI_CSI2_EN_BLANKING,
	.en_null          = RTE_MIPI_CSI2_EN_NULL,
	.en_line_start    = RTE_MIPI_CSI2_EN_LINE_START,
	.en_video         = RTE_MIPI_CSI2_EN_VIDEO,
	.ipi_dt           = RTE_MIPI_CSI2_EN_DT,
	.ipi_dt_overwrite = RTE_MIPI_CSI2_EN_DT_OVERWRITE,
};

ARM_MIPI_CSI2_IPI_INFO CSI2_IPI_CFG =
{
	.ipi_mode      = RTE_MIPI_CSI2_IPI_MODE,
	.ipi_color_cop = RTE_MIPI_CSI2_COLOR_COP,
	.ipi_memflush  = RTE_MIPI_CSI2_MEMFLUSH,
	.frame_info    = &CSI2_FRAME_INFO,
	.adv_features  = &CSI2_IPI_ADV_FEATURES
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
static const uint8_t mipi_csi2_data_type[] =
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
	int32_t ret = ARM_DRIVER_OK;

	if(csi2->flags & CSI2_FLAG_DRV_INIT_DONE)
		return ARM_DRIVER_OK;

	if (!cb_event)
		return ARM_DRIVER_ERROR_PARAMETER;

	csi2->cb_event = cb_event;
#if (RTE_MIPI_CSI2)
	/*DPHY initialization*/
	ret  = CSI2_DPHY_Initialize(frequency);
	if(ret != ARM_DRIVER_OK)
		return ret;
#endif
	csi2->flags = CSI2_FLAG_DRV_INIT_DONE;

	return ret;
}

/**
  \fn          int32_t CSI2_Uninitialize (ARM_MIPI_CSI2_DRV_INFO *csi2)
  \brief       uninitialize MIPI CSI2 Interface.
  \param[in]   csi2 Pointer to CSI2 resources
  \return      \ref execution_status
*/
static int32_t CSI2_Uninitialize (ARM_MIPI_CSI2_DRV_INFO *csi2)
{
	int32_t ret = ARM_DRIVER_OK;
	csi2->cb_event = NULL;
	csi2->flags = 0;

#if (RTE_MIPI_CSI2)
	/*DPHY Uninitialization*/
	ret  = CSI2_DPHY_Uninitialize();
	if(ret != ARM_DRIVER_OK)
		return ret;
#endif
	return ret;
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
	MIPI_CSI2_Type *csi2_reg = (MIPI_CSI2_Type *)csi2->reg_base;

	switch (state)
	{
		case ARM_POWER_OFF:

			/*Disabling the IRQs*/
			csi2_reg->INT_MSK_PHY_FATAL = 0;
			csi2_reg->INT_MSK_PKT_FATAL = 0;
			csi2_reg->INT_MSK_PHY = 0;
			csi2_reg->INT_MSK_LINE = 0;
			csi2_reg->INT_MSK_IPI_FATAL = 0;
			csi2_reg->INT_MSK_BNDRY_FRAME_FATAL = 0;
			csi2_reg->INT_MSK_SEQ_FRAME_FATAL = 0;
			csi2_reg->INT_MSK_CRC_FRAME_FATAL = 0;
			csi2_reg->INT_MSK_PLD_CRC_FATAL = 0;
			csi2_reg->INT_MSK_DATA_ID = 0;
			csi2_reg->INT_MSK_ECC_CORRECT = 0;

			NVIC_DisableIRQ (csi2->irq);
			NVIC_ClearPendingIRQ (csi2->irq);

			MIPI_CSI2_CLEAR_BIT(*csi_pixclk_reg, CSI2_CLK_EN_Msk);
			*csi_pixclk_reg &= ~CSI2_CLK_DIV_Msk;

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

	uint32_t global_EVENT_status = 0;

	global_EVENT_status = csi2_reg->INT_ST_MAIN;

	/*If cb_event is not registered Disable and clear the interrupt*/
	if(!csi2->cb_event)
		NVIC_DisableIRQ (csi2->irq);

	if(global_EVENT_status & CSI2_STATUS_INT_PHY_FATAL_Msk)
	{
		csi2->int_st = csi2_reg->INT_ST_PHY_FATAL;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_PHY_FATAL);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_PKT_FATAL_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_PKT_FATAL;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_PKT_FATAL);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_BNDRY_FRAME_FATAL_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_BNDRY_FRAME_FATAL;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_BNDRY_FRAME_FATAL);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_SEQ_FRAME_FATAL_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_SEQ_FRAME_FATAL;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_SEQ_FRAME_FATAL);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_CRC_FRAME_FATAL_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_CRC_FRAME_FATAL;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_CRC_FRAME_FATAL);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_PLD_CRC_FATAL_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_PLD_CRC_FATAL;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_PLD_CRC_FATAL);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_DATA_ID_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_DATA_ID;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_DATA_ID);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_ECC_CORRECTED_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_ECC_CORRECT;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_ECC_CORRECT);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_PHY_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_PHY;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_PHY);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_LINE_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_LINE;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_LINE);
	}

	if(global_EVENT_status & CSI2_STATUS_INT_IPI_FATAL_Msk)
	{
		csi2->int_st  = csi2_reg->INT_ST_IPI_FATAL;
		if(csi2->cb_event)
			csi2->cb_event(CSI2_EVENT_IPI_FATAL);
	}

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
	ARM_MIPI_CSI2_IPI_ADV_INFO *adv_features = (ARM_MIPI_CSI2_IPI_ADV_INFO *)ipi_info->adv_features;
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

	if(adv_features->sync_evnt_mode == MIPI_CSI2_IPI_SYNCEVFS)
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_SYNC_EVENT_MODE_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_SYNC_EVENT_MODE_Msk);

	if(adv_features->event_sel == MIPI_CSI2_IPI_EVSELPROG)
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_LINE_EVENT_SELECTION_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_LINE_EVENT_SELECTION_Msk);

	if(adv_features->en_embedded == ENABLE)
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_EMBEDDED_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_EMBEDDED_Msk);

	if(adv_features->en_blanking == ENABLE)
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_BLANKING_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_BLANKING_Msk);

	if(adv_features->en_null == ENABLE)
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_NULL_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_NULL_Msk);

	if(adv_features->en_line_start == ENABLE)
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_LINE_START_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_LINE_START_Msk);

	if(adv_features->en_video == ENABLE)
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_VIDEO_Msk);
	else
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_EN_VIDEO_Msk);

	if(adv_features->ipi_dt_overwrite == ENABLE)
	{
		MIPI_CSI2_SET_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_DT_OVERWRITE_Msk);
		csi2_reg->IPI_ADV_FEATURES &= ~CSI2_IPI_DT_Msk;
		csi2_reg->IPI_ADV_FEATURES |= _VAL2FLD(CSI2_IPI_VCID, mipi_csi2_data_type[adv_features->ipi_dt]);
	}
	else
	{
		MIPI_CSI2_CLEAR_BIT(csi2_reg->IPI_ADV_FEATURES, CSI2_IPI_DT_OVERWRITE_Msk);
	}

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
	MIPI_CSI2_ConfigureIPI,
	MIPI_CSI2_StartIPI,
	MIPI_CSI2_StopIPI,
};
#endif
