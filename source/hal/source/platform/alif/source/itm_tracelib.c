/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdbool.h>
#include "itm_tracelib.h"

#include <RTE_Components.h>
#include CMSIS_device_header

#include "pinconf.h"

void itm_init(uint8_t channel)
{
    (void) channel;

#if 0
    // The debugger should do all this
    DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk;
    ITM->TER |= 1U << channel;
    ITM->TCR |= ITM_TCR_TSENA_Msk | ITM_TCR_ITMENA_Msk;
#endif

#if 0
    // Boardlib is the correct place to do this, if we want it on external pins, but leave this here as a template
    // or maybe there should be a separate "use trace pins" config
    pinconf_set(PORT_3, PIN_7, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST);     // P3_7: TRACECLK
    pinconf_set(PORT_4, PIN_0, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST);     // P4_0: TDATA0
    pinconf_set(PORT_4, PIN_1, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST);     // P4_1: TDATA1
    pinconf_set(PORT_4, PIN_2, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST);     // P4_2: TDATA2
    pinconf_set(PORT_4, PIN_3, PINMUX_ALTERNATE_FUNCTION_1, PADCTRL_OUTPUT_DRIVE_STRENGTH_12MA | PADCTRL_SLEW_RATE_FAST);     // P4_3: TDATA3
#endif
}

void itm_send(uint8_t channel, const char *str, uint32_t len)
{
    while (len >= 4) {
        while ((ITM->PORT[channel].u32 & (ITM_STIM_DISABLED_Msk | ITM_STIM_FIFOREADY_Msk)) == 0) {
        }
        ITM->PORT[channel].u32 = str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24);
        str += 4;
        len -= 4;
    }

    if (len >= 2) {
        while ((ITM->PORT[channel].u32 & (ITM_STIM_DISABLED_Msk | ITM_STIM_FIFOREADY_Msk)) == 0) {
        }
        ITM->PORT[channel].u16 = str[0] | (str[1] << 8);
        str += 2;
        len -= 2;
    }

    if (len >= 1) {
        while ((ITM->PORT[channel].u32 & (ITM_STIM_DISABLED_Msk | ITM_STIM_FIFOREADY_Msk)) == 0) {
        }
        ITM->PORT[channel].u8 = str[0];
        str += 1;
        len -= 1;
    }
}

