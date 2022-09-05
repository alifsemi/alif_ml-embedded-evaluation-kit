/* Copyright (c) 2021 ALIF SEMICONDUCTOR

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
 * @file     Driver_CDC200.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     30-Sep-2021
 * @brief    Display controller driver source file.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include <math.h>
#include "display.h"
#include "base_def.h"
#include "Driver_CDC200.h"
#include "RTE_Components.h"
#include CMSIS_device_header

void display_controller_enable(uint8_t enable)
{
	if (enable)
		HW_REG_WORD(CDC200_BASE,0x18) = 1;

	else
		HW_REG_WORD(CDC200_BASE,0x18) = 0;
}

/**
 \fn            int32_t display_controller_setup (uint32_t image_buff_address, uint32_t image_format, const display_panel *panel)
 \brief         Configuring the display controller.
 \param[in]     image_buff_address: Image stored memory starting address.
 \param[in]     image_format: Image standard or type
 \param[in]     panel: pointer to panel info.
 \param[out]    execution status.
 */
int32_t display_controller_setup (uint32_t image_buff_address, uint32_t image_format, const display_panel *panel)
{
	////////////////////////////////////////////////////////////////
	// LCD Controller Setup
	////////////////////////////////////////////////////////////////

	uint32_t pixel_clock = 60 * (HSA+HBP+HACT+HFP) * (VSA+VBP+VACT+VFP);
	uint32_t divider = roundf((float)400000000 / (float)pixel_clock);

	HW_REG_WORD(CFGSLV1_BASE,0x04) = (divider << 16) | 1;	// pixel clock = 400MHz / 14 = 28.57MHz

	display_controller_enable(0);

	HW_REG_WORD(CDC200_BASE,0x08) = (HSA-1)<<16 | (VSA-1);
	HW_REG_WORD(CDC200_BASE,0x0C) = (HSA+HBP-1)<<16 | (VSA+VBP-1);
	HW_REG_WORD(CDC200_BASE,0x10) = (HSA+HBP+HACT-1)<<16 | (VSA+VBP+VACT-1);
	HW_REG_WORD(CDC200_BASE,0x14) = (HSA+HBP+HACT+HFP-1)<<16 | (VSA+VBP+VACT+VFP-1);

	HW_REG_WORD(CDC200_BASE,0x10C) = 0;
	HW_REG_WORD(CDC200_BASE,0x110) = (HSA+HBP+HACT-1)<<16 | (HSA+HBP);
	HW_REG_WORD(CDC200_BASE,0x114) = (VSA+VBP+VACT-1)<<16 | (VSA+VBP);
	HW_REG_WORD(CDC200_BASE,0x11C) = 1;	// 0: 32-bit ARGB, 1: 24-bit RGB
	HW_REG_WORD(CDC200_BASE,0x134) = image_buff_address;
	HW_REG_WORD(CDC200_BASE,0x138) = (HACT*3)<<16 | ((HACT*3)+7);
	HW_REG_WORD(CDC200_BASE,0x13C) = VACT;
	HW_REG_WORD(CDC200_BASE,0x10C) = 1;

	HW_REG_WORD(CDC200_BASE,0x24) = 1;

	return DRIVER_OK;
}

