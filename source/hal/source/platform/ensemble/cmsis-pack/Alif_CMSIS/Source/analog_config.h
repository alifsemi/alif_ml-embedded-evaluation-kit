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
