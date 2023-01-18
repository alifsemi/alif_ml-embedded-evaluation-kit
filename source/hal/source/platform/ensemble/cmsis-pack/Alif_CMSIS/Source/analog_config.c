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
 * @file     analog_config.c
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     22-Feb-2022
 * @brief    Analog configuration registers.
 ******************************************************************************/

/* System includes */
#include "RTE_Device.h"

/* Project includes */
#include "analog_config.h"

#if !(RTE_ANALOG_CONFIG)
#error "Analog configuration is not enabled in RTE_device.h"
#endif

/* Analog configurations */
#if (RTE_ANALOG_CONFIG)
/**
 @fn          void analog_config_vbat_reg2 ()
 @brief       Assigning Vbat registers values to the Vbat register2 base address
 @param[in]   none
 @return      none
 */
void analog_config_vbat_reg2 (void)
{
    /* Analog configuration Vbat register2 */
    uint32_t *vbat_reg2_addr = (uint32_t *)VBAT_ANA_REG2_BASE;

    *vbat_reg2_addr = RTE_VBAT_ANA_REG2_VAL;
}

/**
 @fn          void analog_config_vbat_reg2 ()
 @brief       Assigning comparator register2 values to the comparator
              register2 base address
 @param[in]   none
 @return      none
 */
void analog_config_comp_reg2 (void)
{
    /* Analog configuration comparator register2 */
    uint32_t *comp_reg2_addr = ((uint32_t *)(COMP_REG2_BASE));

    *comp_reg2_addr = RTE_COMP_REG2_VAL;
}

#endif
