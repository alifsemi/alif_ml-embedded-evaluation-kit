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
 * @file     Driver_CDC200.c
 * @author   Girish BN and Prasanna Ravi
 * @email    girish.bn@alifsemi.com and prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     30-Sep-2021
 * @brief    Display controller CDC200 driver source file.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#include "RTE_Device.h"
#include "Driver_CDC200.h"
#include "CDC200_dev.h"
#include "display.h"

#define ARM_CDC200_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /*driver version*/

#if !(RTE_CDC200)
#error "CDC200 is not enabled in the RTE_Device.h"
#endif

#if (!defined(RTE_Drivers_CDC200))
#error "CDC200 not configured in RTE_Components.h!"
#endif

#if (RTE_MIPI_DSI)
#include "Driver_MIPI_DSI.h"
extern ARM_DRIVER_MIPI_DSI Driver_MIPI_DSI;
#endif

/*Driver Version*/
static const ARM_DRIVER_VERSION DriverVersion =
{
	ARM_CDC200_API_VERSION,
	ARM_CDC200_DRV_VERSION
};

#if (RTE_CDC200)
CDC200_FRAME_INFO FRAME_INFO =
{
	.hsync_time   = RTE_PANEL_HSYNC_TIME,
	.hbp_time     = RTE_PANEL_HBP_TIME,
	.hfp_time     = RTE_PANEL_HFP_TIME,
	.hactive_time = RTE_PANEL_HACTIVE_TIME,
	.vsync_line   = RTE_PANEL_VSYNC_LINE,
	.vbp_line     = RTE_PANEL_VBP_LINE,
	.vfp_line     = RTE_PANEL_VFP_LINE,
	.vactive_line = RTE_PANEL_VACTIVE_LINE,
};

CDC200_DRV_INFO CDC =
{
	.reg_base       = CDC200,
	.frame_info     = &FRAME_INFO,
	.pixel_format   = RTE_CDC200_PIXEL_FORMAT,
	.flags          = 0,
};
#endif

/* Driver Capabilities */
static const ARM_CDC200_CAPABILITIES DriverCapabilities =
{
	0, /* Not supports reentrant_operation */
	1, /* DPI interface supported */
	0  /* reserved (must be zero) */
};

//
//  Functions
//
/**
  \fn          ARM_DRIVER_VERSION ARM_CDC200_GetVersion (void)
  \brief       Get CDC200 driver version.
  \return      \ref ARM_DRIVER_VERSION.
*/
static ARM_DRIVER_VERSION ARM_CDC200_GetVersion (void)
{
	return DriverVersion;
}

/**
  \fn          ARM_CDC200_CAPABILITIES ARM_CDC200_GetCapabilities (void)
  \brief       Get CDC200 driver capabilities.
  \return      \ref ARM_CDC200_CAPABILITIES.
*/
static ARM_CDC200_CAPABILITIES ARM_CDC200_GetCapabilities (void)
{
	return DriverCapabilities;
}

#if (RTE_MIPI_DSI)
/**
  \fn          void ARM_MIPI_DSI_Event_Callback (uint32_t int_event)
  \brief       Signal MIPI DSI Events.
  \param[in]   int_event  \ref MIPI DSI event types.
  \return      none.
*/
void ARM_MIPI_DSI_Event_Callback (uint32_t int_event)
{
	CDC.cb_event(ARM_CDC_DSI_ERROR_EVENT);
}
#endif

/**
  \fn          int32_t CDC200_Initialize (ARM_CDC200_SignalEvent_t cb_event, CDC200_DRV_INFO *cdc)
  \brief       Initialize CDC200 Interface.
  \param[in]   cb_event Pointer to ARM_CDC200_SignalEvent_t.
  \param[in]   cdc Pointer to CDC resources.
  \return      \ref execution_status.
*/
static int32_t CDC200_Initialize (ARM_CDC200_SignalEvent_t cb_event, CDC200_DRV_INFO *cdc)
{
	int32_t ret = ARM_DRIVER_OK;

	if(cdc->flags & CDC200_FLAG_DRV_INIT_DONE)
		return ARM_DRIVER_OK;

	if (!cb_event)
		return ARM_DRIVER_ERROR_PARAMETER;

	cdc->cb_event = cb_event;

#if (RTE_MIPI_DSI)
	/*Initializing MIPI DSI, if the LCD Panel is MIPI DSI LCD Panel*/
	ret = Driver_MIPI_DSI.Initialize(ARM_MIPI_DSI_Event_Callback);
	if(ret != ARM_DRIVER_OK)
		return ret;
#endif

	cdc->flags = CDC200_FLAG_DRV_INIT_DONE;

	return ret;
}

/**
  \fn          int32_t CDC200_Uninitialize (CDC200_DRV_INFO *cdc)
  \brief       uninitialize CDC200 Interface.
  \param[in]   cdc Pointer to CDC resources.
  \return      \ref execution_status.
  */
static int32_t CDC200_Uninitialize (CDC200_DRV_INFO *cdc)
{
	int32_t ret = ARM_DRIVER_OK;
	cdc->cb_event = NULL;
	cdc->flags = 0;

#if (RTE_MIPI_DSI)
	/*Uninitializing MIPI DSI, if the LCD Panel is MIPI DSI LCD Panel*/
	ret = Driver_MIPI_DSI.Uninitialize();
	if(ret != ARM_DRIVER_OK)
		return ret;
#endif

	return ret;
}

/**
  \fn          int32_t CDC200_PowerControl (ARM_POWER_STATE state, CDC200_DRV_INFO *cdc)
  \brief       Control CDC200 Interface Power.
  \param[in]   state Power state.
  \param[in]   cdc Pointer to CDC resources.
  \return      \ref execution_status.
  */
static int32_t CDC200_PowerControl (ARM_POWER_STATE state, CDC200_DRV_INFO *cdc)
{
	volatile uint32_t *cdc_pixclk_reg = (volatile uint32_t *)CDC200_PIXCLK_CTRL;
	uint32_t pixclk, htotal, vtotal;
	float pixclk_div;
	int32_t ret = ARM_DRIVER_OK;

	switch (state)
	{
		case ARM_POWER_OFF:

#if (RTE_MIPI_DSI)
			/*Disable MIPI DSI*/
			ret = Driver_MIPI_DSI.PowerControl(ARM_POWER_OFF);
			if(ret != ARM_DRIVER_OK)
				return ret;
#endif
			CDC200_CLEAR_BIT(*cdc_pixclk_reg, CDC200_CLK_EN_Msk);
			*cdc_pixclk_reg &= ~CDC200_CLK_DIV_Msk;

			cdc->flags &= ~CDC200_FLAG_POWER_ENABLED;
			break;

		case ARM_POWER_FULL:

			if (!(cdc->flags & CDC200_FLAG_DRV_INIT_DONE))
				return ARM_DRIVER_ERROR;
			if (cdc->flags & CDC200_FLAG_POWER_ENABLED)
				return ARM_DRIVER_OK;

#if (RTE_MIPI_DSI)
			/*Enable MIPI DSI*/
			ret = Driver_MIPI_DSI.PowerControl(ARM_POWER_FULL);
			if(ret != ARM_DRIVER_OK)
				return ret;
#endif

			/* LCD Manufacturer provides the Frame timing values
			 *     HTOTAL = WIDTH + HSYNC + HFP + HBP
			 *     VTOTAL = HEIGHT + VSYNC + VFP + VBP
			 * Calculate the pixel clock for DPI controller
			 *     PIXCLK = FPS x HTOTAL x VTOTAL
			 * Calculate the pixel clock divider
			 *     PIXCLK_DIV = CDC200_PIXCLK_SOURCE / PIXCLK
			 */
			htotal = (cdc->frame_info->hsync_time
					  + cdc->frame_info->hbp_time
					  + cdc->frame_info->hfp_time
					  + cdc->frame_info->hactive_time);

			vtotal = (cdc->frame_info->vsync_line
					  + cdc->frame_info->vbp_line
					  + cdc->frame_info->vfp_line
					  + cdc->frame_info->vactive_line);

			pixclk = (htotal * vtotal * RTE_CDC200_DPI_FPS);

			pixclk_div = (float)CDC200_PIXCLK / (htotal * vtotal * RTE_CDC200_DPI_FPS);

			/*Checking clk divider is less than 2 because 0 and 1 are illegal value*/
			if(pixclk_div < 2)
				return ARM_DRIVER_ERROR_PARAMETER;

			pixclk_div = (int)pixclk_div + ((pixclk_div - (int)pixclk_div) > 0.5);

			CDC200_SET_BIT(*cdc_pixclk_reg, CDC200_CLK_EN_Msk);
			*cdc_pixclk_reg &= ~CDC200_CLK_DIV_Msk;
			*cdc_pixclk_reg |= _VAL2FLD(CDC200_CLK_DIV, pixclk_div);

			cdc->flags |= CDC200_FLAG_POWER_ENABLED;

			break;

		case ARM_POWER_LOW:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ARM_DRIVER_OK;
}

/**
 \fn            void Set_Display_Controller_Image_Dimension (CDC200_DRV_INFO *cdc)
 \brief         Configuring the display controller for Image dimension.
 \param[in]     cdc Pointer to CDC resources.
 \return        \ref execution_status.
 */
static int32_t Set_Display_Controller_Image_Dimension (CDC200_DRV_INFO *cdc)
{
	CDC200_Type *regbase = (CDC200_Type *) cdc->reg_base;
	CDC200_FRAME_INFO *frame_info = (CDC200_FRAME_INFO *)cdc->frame_info;
	uint32_t pixel_format = cdc->pixel_format;
	uint16_t width = frame_info->hactive_time;
	uint16_t height = frame_info->vactive_line;

	switch (pixel_format)
	{
		case CDC200_ARGB8888: /*ARGB8888*/
		{
			// In ARGB8888 standard One pixel handled by 4 byte, so width size multiplied with 4 //
			regbase->layer1_reg.fb_length = (((width * 4) << 16) | ((width * 4) + BUS_WIDTH));
			break;
		}
		case CDC200_RGB888: /*RGB888*/
		{
			// In RGB888 standard One pixel handled by 3 byte, so width size multiplied with 3 //
			regbase->layer1_reg.fb_length = (((width * 3) << 16) | ((width * 3) + BUS_WIDTH));

			//Alpha constant
			regbase->layer1_reg.alpha = ALPHA_CONSTANT;
			break;
		}
		case CDC200_RGB565: /*RGB565*/
		{
			// In ARGB8888 standard One pixel handled by 2 byte, so width size multiplied with 2 //
			regbase->layer1_reg.fb_length = (((width * 2) << 16) | ((width * 2) + BUS_WIDTH));

			//Alpha constant
			regbase->layer1_reg.alpha = ALPHA_CONSTANT;
			break;
		}
		case CDC200_RGBA8888:
		case CDC200_AL44:
		case CDC200_AL8:
		case CDC200_ARGB1555:
		case CDC200_ARGB4444:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;

	}

	regbase->layer1_reg.pixel_format = pixel_format;

	regbase->layer1_reg.fb_lines = height;

	return ARM_DRIVER_OK;
}

/**
 \fn            int32_t Display_Controller_Setup (uint32_t image_buff_address, CDC200_DRV_INFO *cdc)
 \brief         Configuring the display controller.
 \param[in]     image_buff_address: Image stored memory starting address.
 \param[in]     cdc Pointer to CDC resources.
 \return        \ref execution_status.
 */
static int32_t Display_Controller_Setup (uint32_t image_buff_address, CDC200_DRV_INFO *cdc)
{
	CDC200_Type *regbase = (CDC200_Type *) cdc->reg_base;
	CDC200_FRAME_INFO *frame_info = (CDC200_FRAME_INFO *)cdc->frame_info;
	uint32_t sync_size = (((frame_info->hsync_time - 1) <<16 ) + (frame_info->vsync_line - 1));
	uint32_t back_porch = (frame_info->hbp_time << 16) + frame_info->vbp_line + sync_size;
	uint32_t active_width = (frame_info->hactive_time <<16) + frame_info->vactive_line + back_porch;
	uint32_t total_width = (frame_info->hfp_time <<16) + frame_info->vfp_line + active_width;
	uint32_t ret = ARM_DRIVER_OK;
	uint32_t rd_data = 0;

	if (image_buff_address == 0)
		return ARM_DRIVER_ERROR_PARAMETER;
	if ((frame_info->hbp_time == 0) || (frame_info->vbp_line == 0))
		return ARM_DRIVER_ERROR_PARAMETER;
	if ((frame_info->hactive_time == 0) || (frame_info->vactive_line == 0))
		return ARM_DRIVER_ERROR_PARAMETER;
	if ((frame_info->hfp_time == 0) || (frame_info->vfp_line == 0))
		return ARM_DRIVER_ERROR_PARAMETER;

	/*enable single frame mode*/
	rd_data = regbase->global_reg.global_control;
	rd_data = ((rd_data & 0xF0000000) | 0x00000001);
	regbase->global_reg.global_control = rd_data;

	regbase->global_reg.background_color = 0;

	regbase->global_reg.irq_enable = 0;

	regbase->global_reg.sync_size = sync_size;

	regbase->global_reg.back_porch = back_porch;

	regbase->global_reg.active_width = active_width;

	regbase->global_reg.total_width = total_width;

	regbase->layer1_reg.control = 0x00000001;

	regbase->layer1_reg.window_h = ((active_width & 0xffff0000u) | ((back_porch >> 16)+1));

	regbase->layer1_reg.window_v = (((active_width & 0xffffu) << 16) | ((back_porch & 0xffffu) + 1));

	regbase->layer1_reg.fb_start = image_buff_address;

	ret = Set_Display_Controller_Image_Dimension (cdc);
	if(ret != ARM_DRIVER_OK)
		return ret;

	regbase->global_reg.shadow_reload_control = 0x00000001;

	regbase->global_reg.irq_enable = 0x00000001;

	regbase->global_reg.irq_clear = 0x00000001;

	regbase->global_reg.global_configuration_1 = 0;

	rd_data = regbase->global_reg.global_configuration_2;
	regbase->global_reg.global_configuration_2 = (rd_data | 0x00001000);

	return ret;
}

/**
 \fn            int32_t CDC200_control (ARM_CDC200_CONTROL control, uint32_t arg, CDC200_DRV_INFO *cdc)
 \brief         Control the display controller.
 \param[in]     control CDC200 Configuration.
 \param[in]     arg Argument of operation (optional).
 \param[in]     cdc Pointer to CDC resources.
 \return        \ref execution_status.
 */
static int32_t CDC200_control (ARM_CDC200_CONTROL control, uint32_t arg, CDC200_DRV_INFO *cdc)
{
	uint32_t ret = ARM_DRIVER_OK;
	CDC200_Type *cdc_regs = (CDC200_Type *) CDC200_BASE;
	CDC200_GlobalRegInfo *cdc_global_regs = &cdc_regs->global_reg;
	CDC200_LayerRegInfo *cdc_layer_regs = &cdc_regs->layer1_reg;

	if (!(cdc->flags & CDC200_FLAG_POWER_ENABLED))
		return ARM_DRIVER_ERROR;

	if ((cdc->flags & CDC200_FLAG_STARTED) && (control & CDC200_CONFIGURE_DISPLAY))
		return ARM_DRIVER_ERROR_BUSY;

	switch (control)
	{
		case CDC200_CONFIGURE_DISPLAY:
			/*Setup display controller*/
			ret = Display_Controller_Setup(arg, cdc);
			if(ret != ARM_DRIVER_OK)
				return ret;

#if (RTE_MIPI_DSI)
			/*MIPI DSI Configure Host*/
			ret = Driver_MIPI_DSI.Control(DSI_CONFIGURE_HOST, 0);
			if(ret != ARM_DRIVER_OK)
				return ret;

			/*Start Command mode and  Configure LCD Panel*/
			ret = Driver_MIPI_DSI.StartCommandMode();
			if(ret != ARM_DRIVER_OK)
				return ret;

			/*MIPI DSI Configure DPI*/
			ret = Driver_MIPI_DSI.Control(DSI_CONFIGURE_DPI, 0);
			if(ret != ARM_DRIVER_OK)
				return ret;
#endif
			break;
		case CDC200_FRAMEBUF_UPDATE:
			/*Update the buffer start address for new buffer content*/
			cdc_layer_regs->fb_start = (uint32_t) arg;
			break;

		case CDC200_SHADOW_RELOAD_REGISTER:
			if(arg == ENABLE)
				/*immediate reload of shadow register content into main register*/
				cdc_global_regs->shadow_reload_control = 1;
			break;
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}
	cdc->flags |= CDC200_FLAG_CONFIGURED;

	return ret;
}

/**
 \fn            int32_t CDC200_Start (CDC200_DRV_INFO *cdc)
 \brief         Start the display controller.
 \param[in]     cdc Pointer to CDC resources.
 \return        \ref execution_status.
 */
static int32_t CDC200_Start (CDC200_DRV_INFO *cdc)
{
	uint32_t ret = ARM_DRIVER_OK;

	CDC200_Type *regbase = (CDC200_Type *) cdc->reg_base;

#if (RTE_MIPI_DSI)
	/*Start MIPI DSI and LCD Panel*/
	ret = Driver_MIPI_DSI.StartVideoMode();
	if(ret != ARM_DRIVER_OK)
		return ret;
#endif

	/*Enable Display Controller CDC200*/
	CDC200_SET_BIT(regbase->global_reg.global_control,CDC200_GLOBAL_EN_Pos);

	cdc->flags |= CDC200_FLAG_STARTED;

	return ret;
}

/**
 \fn            int32_t CDC200_Stop (CDC200_DRV_INFO *cdc)
 \brief         Stop the display controller.
 \param[in]     cdc Pointer to CDC resources.
 \return        \ref execution_status.
 */
static int32_t CDC200_Stop (CDC200_DRV_INFO *cdc)
{
	uint32_t ret = ARM_DRIVER_OK;

	CDC200_Type *regbase = (CDC200_Type *) cdc->reg_base;

	/*Disable Display Controller CDC200*/
	CDC200_CLEAR_BIT(regbase->global_reg.global_control,CDC200_GLOBAL_EN_Pos);

#if (RTE_MIPI_DSI)
	/*Stop MIPI DSI and LCD Panel*/
	ret = Driver_MIPI_DSI.Stop();
	if(ret != ARM_DRIVER_OK)
		return ret;
#endif

	cdc->flags &= ~CDC200_FLAG_STARTED;

	return ret;
}

#if (RTE_CDC200)
/**
  \fn          int32_t ARM_CDC200_Initialize (ARM_CDC200_SignalEvent_t cb_event)
  \brief       Initialize CDC200 Interface.
  \param[in]   cb_event Pointer to ARM_CDC200_SignalEvent_t.
  \return      \ref execution_status.
  */
static int32_t ARM_CDC200_Initialize (ARM_CDC200_SignalEvent_t cb_event)
{
	return CDC200_Initialize (cb_event, &CDC);
}

/**
  \fn          int32_t ARM_CDC200_Uninitialize (void)
  \brief       Uninitialize CDC200 Interface.
  \return      \ref execution_status.
  */
static int32_t ARM_CDC200_Uninitialize (void)
{
	return CDC200_Uninitialize (&CDC);
}

/**
  \fn          int32_t ARM_CDC200_PowerControl (ARM_POWER_STATE state)
  \brief       Control CDC200 Interface Power.
  \param[in]   state  Power state.
  \return      \ref execution_status.
  */
static int32_t ARM_CDC200_PowerControl (ARM_POWER_STATE state)
{
	return CDC200_PowerControl (state, &CDC);
}

/**
 \fn            int32_t ARM_CDC200_Control (ARM_CDC200_CONTROL control, uint32_t arg)
 \brief         Control the display controller.
 \param[in]     control CDC200 Configuration.
 \param[in]     arg Argument of operation (optional).
 \return        \ref execution_status.
 */
static int32_t ARM_CDC200_Control (ARM_CDC200_CONTROL control, uint32_t arg)
{
	return CDC200_control (control, arg, &CDC);
}

/**
 \fn            int32_t ARM_CDC200_StartDisplay (void)
 \brief         Start the display controller.
 \return        \ref execution_status.
 */
static int32_t ARM_CDC200_StartDisplay (void)
{
	return CDC200_Start(&CDC);
}

/**
 \fn            int32_t ARM_CDC200_StopDisplay (void)
 \brief         Stop the display controller.
 \return        \ref execution_status.
 */
static int32_t ARM_CDC200_StopDisplay (void)
{
	return CDC200_Stop(&CDC);
}

extern ARM_DRIVER_CDC200 Driver_CDC200;

ARM_DRIVER_CDC200 Driver_CDC200 =
{
	ARM_CDC200_GetVersion,
	ARM_CDC200_GetCapabilities,
	ARM_CDC200_Initialize,
	ARM_CDC200_Uninitialize,
	ARM_CDC200_PowerControl,
	ARM_CDC200_Control,
	ARM_CDC200_StartDisplay,
	ARM_CDC200_StopDisplay
};
#endif
