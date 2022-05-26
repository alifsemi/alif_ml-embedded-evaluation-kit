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
