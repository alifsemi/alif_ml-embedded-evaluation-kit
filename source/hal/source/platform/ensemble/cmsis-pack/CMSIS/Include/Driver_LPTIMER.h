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
 * @file     Driver_LPTIMER.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for LPTIMER.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef __DRIVER_LPTIMER_H__
#define __DRIVER_LPTIMER_H__

#include "Driver_Common.h"

typedef void (*ARM_LPTIMER_SignalEvent_t) (uint8_t event);	/**< Initialization LPTIMER call back function declaration >*/

/****** LPTIMER Events *****/
#define ARM_LPTIMER_EVENT_UNDERFLOW         (1)     /**< LPTIMER Under flow event>*/

/****** LPTIMER Control Codes *****/
#define ARM_LPTIMER_SET_COUNT1              (1)     /**< Set Count; arg = pointer of count var>*/
#define ARM_LPTIMER_SET_COUNT2              (2)     /**< Set Count; arg = pointer of count var>*/
#define ARM_LPTIMER_GET_COUNT               (3)     /**< get Count; arg = pointer of count var>*/

/**
\fn         int32_t ARM_LPTIMER_Initialize (uint8_t channel, ARM_LPTIMER_CBEvent CB_event)
\brief      Initialize LPTIMER interface and register signal(call back) functions.
\param[in]  channel :LPTIMER channel number (ie. 0,1,2,3.... should be less than RTE_LPTIMER_MAX_CHANNEL)
\param[in]  CB_event: call back function.
\param[out] int32_t : execution_status.

\fn         int32_t ARM_LPTIMER_PowerControl (uint8_t channel, ARM_POWER_STATE state);
\brief      Control LPTIMER channel interface power.
\param[in]  channel :LPTIMER channel number (ie. 0,1,2,3.... should be less than RTE_LPTIMER_MAX_CHANNEL)
\param[in]  state   : Power state
                        - \ref ARM_POWER_OFF  :  power off: no operation possible
                        - \ref ARM_POWER_LOW  :  low power mode: retain state, detect and signal wake-up events
                        - \ref ARM_POWER_FULL : power on: full operation at maximum performance
\param[out] int32_t : execution_status.

\fn         int32_t ARM_LPTIMER_Control (uint8_t channel, uint32_t control, uint32_t arg)
\brief      Control LPTIMER interface.
\param[in]  channel :LPTIMER channel number (ie. 0,1,2,3.... should be less than RTE_LPTIMER_MAX_CHANNEL)
\param[in]  control_code : Operation
                            -ARM_LPTIMER_CONTROL_CODE_SET_COUNT1     LPTIMER channel setting the count1 for operation
                            -ARM_LPTIMER_CONTROL_CODE_SET_COUNT2     LPTIMER Channel setting the count2 for operation
                            -ARM_LPTIMER_CONTROL_CODE_GET_COUNT      LPTIMER Channel get the count
\param[in]  arg : Argument of operation.
\param[out] int32_t : execution_status

\fn         int32_t ARM_LPTIMER_Start (uint8_t channel);
\brief      Start the UTMER Channel counting.
\param[in]  channel :LPTIMER channel number (ie. 0,1,2,3.... should be less than RTE_LPTIMER_MAX_CHANNEL)
\param[out] int32_t : execution_status.

\fn         int32_t ARM_LPTIMER_Stop (uint8_t channel);
\brief      Stop the LPTMER Channel running count.
\param[in]  channel :LPTIMER channel number (ie. 0,1,2,3.... should be less than RTE_LPTIMER_MAX_CHANNEL)
\param[out] int32_t : execution_status.

\fn         int32_t ARM_LPTIMER_Uninitialize (uint8_t channel);
\brief      Un-initialize the named channel configuration
\param[in]  channel :LPTIMER channel number (ie. 0,1,2,3.... should be less than RTE_LPTIMER_MAX_CHANNEL)
\param[out] int32_t : execution_status.
*/

typedef struct _ARM_DRIVER_LPTIMER{
    int32_t (*Initialize)   (uint8_t channel, ARM_LPTIMER_SignalEvent_t cb_event);  /**< Pointer to \ref ARM_LPTIMER_Initialize     : Initialize the LPTIMER Interface>*/
    int32_t (*PowerControl) (uint8_t channel, ARM_POWER_STATE state);               /**< Pointer to \ref ARM_LPTIMER_PowerControl   : Control LPIMER interface power>*/
    int32_t (*Control)      (uint8_t channel, uint32_t control_code, void *arg);    /**< Pointer to \ref ARM_LPTIMER_Control        : Control LPTIMER interface.>*/
    int32_t (*Start)        (uint8_t channel);                                      /**< Pointer to \ref ARM_LPTIMER_Start          : global starts the LPTIMER channel>*/
    int32_t (*Stop)         (uint8_t channel);                                      /**< Pointer to \ref ARM_LPTIMER_Stop           : global stops  the LPTIMER channel>*/
    int32_t (*Uninitialize) (uint8_t channel);                                      /**< Pointer to \ref ARM_LPTIMER_Uninitialize   : Uninitialized the LPTIMER Interface>*/
} ARM_DRIVER_LPTIMER;

#endif /*< __DRIVER_LPTIMER_H__>*/
