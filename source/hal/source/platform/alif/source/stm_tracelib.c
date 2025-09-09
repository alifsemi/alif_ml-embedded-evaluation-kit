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
#include "stm_tracelib.h"

#include <RTE_Components.h>
#include CMSIS_device_header

typedef struct
{
    uint32_t reserved_000[896];          /* STM-500 doesn't have basic stimulus ports */
    __IOM uint32_t STMSPER;              /*!< Offset: 0xE00 (R/W)  Stimulus Port Enable Register */
    uint32_t reserved_E04[7];
    __IOM uint32_t STMSPTER;             /*!< Offset: 0xE20 (R/W)  Stimulus Port Trigger Enable Register */
    uint32_t reserved_E24[7];
    __IOM uint32_t STMPRIVMASKR;         /*!< Offset: 0xE40 (R/W)  Trace Privilege Register */
    uint32_t reserved_E44[7];
    __IOM uint32_t STMSPSCR;             /*!< Offset: 0xE60 (R/W)  Stimulus Port Select Configuration Register */
    __IOM uint32_t STMSPMSCR;            /*!< Offset: 0xE64 (R/W)  Stimulus Port Master Select Configuration Register */
    __IOM uint32_t STMSPOVERRIDER;       /*!< Offset: 0xE68 (R/W)  Stimulus Port Override Register */
    __IOM uint32_t STMSPMOVERRIDER;      /*!< Offset: 0xE6C (R/W)  Stimulus Port Master Override Register */
    __IOM uint32_t STMSPTRIGCSR;         /*!< Offset: 0xE70 (R/W)  Stimulus Port Trigger Control and Status Register */
    uint32_t reserved_E74[3];
    __IOM uint32_t STMTCSR;              /*!< Offset: 0xE80 (R/W)  Trace Control and Status Register */
    __OM  uint32_t STMTSSTIMR;           /*!< Offset: 0xE84 ( /W)  Timestamp Stimulus Register */
    uint32_t reserved_E88[1];
    __IOM uint32_t STMTSFREQR;           /*!< Offset: 0xE8C (R/W)  Timestamp Frequency Register */
    __IOM uint32_t STMSYNCR;             /*!< Offset: 0xE90 (R/W)  Synchronization Control Register */
    __IOM uint32_t STMAUXCR;             /*!< Offset: 0xE94 (R/W)  Auxiliary Control Register */
    uint32_t reserved_E98[2];
    __IM  uint32_t STMFEAT1R;            /*!< Offset: 0xEA0 (R/ )  Features 1 Register */
    __IM  uint32_t STMFEAT2R;            /*!< Offset: 0xEA4 (R/ )  Features 2 Register */
    __IM  uint32_t STMFEAT3R;            /*!< Offset: 0xEA8 (R/ )  Features 3 Register */
} STM_APB_Type;

#define STMTCSR_EN 0x1

typedef union
{
    __OM  uint8_t   u8;
    __OM  uint16_t  u16;
    __OM  uint32_t  u32;
    __OM  uint64_t  u64;
} STM_Ext_Stim_Type;

typedef struct
{
    __OM  STM_Ext_Stim_Type G_DMTS;     /*!< Offset: 0x00 ( /W)  Data, marked with timestamp, guaranteed */
    __OM  STM_Ext_Stim_Type G_DM;       /*!< Offset: 0x08 ( /W)  Data, marked, guaranteed */
    __OM  STM_Ext_Stim_Type G_DTS;      /*!< Offset: 0x10 ( /W)  Data, with timestamp, guaranteed */
    __OM  STM_Ext_Stim_Type G_D;        /*!< Offset: 0x18 ( /W)  Data, guaranteed */
    uint32_t reserved_20[16];
    __OM  STM_Ext_Stim_Type G_FLAGTS;   /*!< Offset: 0x60 ( /W)  Flag with timestamp, guaranteed */
    __OM  STM_Ext_Stim_Type G_FLAG;     /*!< Offset: 0x68 ( /W)  Flag, guaranteed */
    __OM  STM_Ext_Stim_Type G_TRIGTS;   /*!< Offset: 0x70 ( /W)  Trigger with timestamp, guaranteed */
    __OM  STM_Ext_Stim_Type G_TRIG;     /*!< Offset: 0x78 ( /W)  Trigger, guaranteed */
    __OM  STM_Ext_Stim_Type I_DMTS;     /*!< Offset: 0x80 ( /W)  Data, marked with timestamp, invariant timing */
    __OM  STM_Ext_Stim_Type I_DM;       /*!< Offset: 0x88 ( /W)  Data, marked, invariant timing */
    __OM  STM_Ext_Stim_Type I_DTS;      /*!< Offset: 0x90 ( /W)  Data, with timestamp, invariant timing */
    __OM  STM_Ext_Stim_Type I_D;        /*!< Offset: 0x98 ( /W)  Data, invariant timing */
    uint32_t reserved_A0[16];
    __OM  STM_Ext_Stim_Type I_FLAGTS;   /*!< Offset: 0xE0 ( /W)  Flag with timestamp, invariant timing */
    __OM  STM_Ext_Stim_Type I_FLAG;     /*!< Offset: 0xE8 ( /W)  Flag, invariant timing */
    __OM  STM_Ext_Stim_Type I_TRIGTS;   /*!< Offset: 0xF0 ( /W)  Trigger with timestamp, invariant timing */
    __OM  STM_Ext_Stim_Type I_TRIG;     /*!< Offset: 0xF8 ( /W)  Trigger, invariant timing */
} STM_Ext_StimPort_Type;


#define STM_APB_BASE 0x10170000
#define STM_APB      ((STM_APB_Type *) STM_APB_BASE)

#define STM_EXT_BASE 0x1D000000
#define STM_EXT      ((STM_Ext_StimPort_Type *) STM_EXT_BASE)

void stm_init(uint16_t channel)
{
    (void) channel;

    STM_APB->STMSPMSCR = 0;
    STM_APB->STMSPER = 0xFFFFFFFF;
    STM_APB->STMTCSR |= STMTCSR_EN;
}

void stm_send(uint16_t channel, const char *str, uint32_t len)
{
    STM_Ext_StimPort_Type *port = &STM_EXT[channel];

    /* Use "kernel log message" format - first a timestamped data packet */
    if (len >= 4) {
        port->G_DTS.u32 = str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24);
        str += 4;
        len -= 4;
    } else if (len >= 2) {
        port->G_DTS.u16 = str[0] | (str[1] << 8);
        str += 2;
        len -= 2;
    } else if (len >= 1) {
        port->G_DTS.u8 = str[0];
        str += 1;
        len -= 1;
    } else {
        return;
    }

    /* Followed by data packets */
    while (len >= 4) {
        port->G_D.u32 = str[0] | (str[1] << 8) | (str[2] << 16) | (str[3] << 24);
        str += 4;
        len -= 4;
    }

    if (len >= 2) {
        port->G_D.u16 = str[0] | (str[1] << 8);
        str += 2;
        len -= 2;
    }

    if (len >= 1) {
        port->G_D.u8 = str[0];
        str += 1;
        len -= 1;
    }

    /* And an end-of-message flag */
    port->G_FLAG.u32 = 0;
}

