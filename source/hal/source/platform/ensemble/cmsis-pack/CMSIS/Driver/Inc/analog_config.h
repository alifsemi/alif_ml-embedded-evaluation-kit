/* Copyright (c)  2022 ALIF SEMICONDUCTOR

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
 * @file     analog_config.h
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     28-Feb-2022
 * @brief    Device specific header files for Common Analog configuration
 *           registers.
 ******************************************************************************/

#ifndef ANALOG_CONFIG_H_
#define ANALOG_CONFIG_H_

/* System Includes */
#include "RTE_Components.h"
#include CMSIS_device_header

#define VBAT_ANA_REG2_BASE        (VBAT_BASE  + 0x0004001C)  /* Vbat register2 base address */
#define COMP_REG2_BASE            (COMP0_BASE + 0x00000004)  /* comparator register2 base address */

/* Analog configuration:
   Vbat register2 contains below parameters:
   -pmubg_vref_cont,
   -dig_ldo_18_en,
   -dig_ldo_cont,
   -phy_ldo_en,
   -phy_ldo_cont,
   -osc_38Mrc_cont,
   -ana_periph_bg_en,
   -ana_periph_bg_cont,
   -ana_periph_ldo_cont,
   -ana_periph_ldo_en,

    User can modify these parameters using RTE_VBAT_ANA_REG2_VAL(RTE_device.h).
*/
void analog_config_vbat_reg2(void);

/* Analog configuration:
   comparator register2 contains below parameters:
   -comp_lpo_in_p_sel,
   -comp_lpo_in_m_sel,
   -comp_lpo_hyst,
   -comp_lp_en,
   -dac6_en,
   -dac6_vref_scale,
   -dac6_cont,
   -adc_vref_cont,
   -adc_vref_buf_rdiv_en,
   -adc_vref_buf_en

    User can modify these parameters using RTE_COMP_REG2_VAL(RTE_device.h).
*/
void analog_config_comp_reg2(void);

#endif /* ANALOG_CONFIG_H_ */
