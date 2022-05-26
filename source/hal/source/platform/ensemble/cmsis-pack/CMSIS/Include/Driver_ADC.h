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
/***************************************************************************
 * @file     Driver_ADC.h
 * @author   Prabhakar Kumar
 * @email    prabhakar.kumar@alifsemi.com
 * @version  V1.0.0
 * @date     22-feb-2022
 * @brief    ADC (Analog to digital conversion) Driver Definition.
******************************************************************************/
#ifndef DRIVER_ADC_H_
#define DRIVER_ADC_H_

#include"Driver_Common.h"

#ifdef _cplusplus
extern "c"
{
#endif


#define ARM_ADC_API_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR (1,0) /*API VERSION*/

/**********ADC CONTROL CODE************/
#define ARM_SET_SHIFT_CONTROL                  (0x01UL)
#define ARM_SET_SEQUENCER_CTRL                 (0x02UL)

/**********ADC EVENT**********/
#define ARM_ADC_EVENT_CONVERSION_COMPLETE      (1UL << 0)

// Function documentation
/**
    @func         : ARM_DRIVER_VERSION GetVersion (void)
    @brief        : Get ADC driver version.
    @return       : \ref ARM_DRIVER_VERSION

    @func         : ARM_ADC_CAPABILITIES GetCapabilities (void)
    @brief        : Get ADC driver capabilities
    @return       : \ref ADC_CAPABILITIES

    @func         : int32_t Initialize (ARM_ADC_SignalEvent_t cb_event)
    @brief        : Initialize ADC INterface
    @parameter[1] : cb_event    : Pointer to ADC Event \ref ARM_ADC_Signal_Event
    @return       : Execution status

    @func         : int32_t Uninitialize (void)
    @brief        : De-initialize the ADC interface
    @return       : Execution Status

    @func         : int32_t start (uint32_t *data, uint32_t num)
    @brief        : Start the ADC and start collecting data
    @parameter[1] : data    : pointer to unsigned int
    @parameter[2] : num     : store the amount data
    @return       : Execution status

    @func         : int32_t stop (void)
    @brief        : Stop the ADC interface
    @PARAMETER    : NONE
    @Return       : Execution Status

    @func         : intt32_t PowerControl(ARM_Power_status state)
    @brief        : Control ADC interface power
    @parameter[1] : state    :power state
    @return       : execution status

    @func         : int32_t control(uint32_t control, uint32_t arg)
    @brief        : control ADC_interface
    @parameter[1] : control    :operation
    @parameter[2] : arg        :Argument of operation (optional)
    @return       : Execution status
*/

typedef void (*ARM_ADC_SignalEvent_t) (uint32_t event);    /*Pointer to \ref ADC_SignalEvent : Signal ADC Event*/

typedef struct _ARM_ADC_CAPABILITIES{
    uint32_t Resolution         :1;     /* Resolution 12 or 20 bits*/
    uint32_t Reserved           :31;    /* Reserved                */
}ARM_ADC_CAPABILITIES;

/* Access Structure Of ADC */
typedef struct ARM_DRIVER_ADC{
    ARM_DRIVER_VERSION      (*GetVersion)       (void);                             /*pointer pointing to \ref ADC_get_verion                         */
    ARM_ADC_CAPABILITIES    (*GetCapabilities)  (void);                             /*Pointer  to ADC_get_capabilities                                */
    int32_t                 (*Initialize)       (ARM_ADC_SignalEvent_t cb_event);   /*Pointer pointing to \ref ADC_intialize                          */
    int32_t                 (*Uninitialize)     (void);                             /*Pointer pointing to \ref ADC_Unintialize	                      */
    int32_t                 (*Start)            (uint32_t *data, uint32_t num);     /*Pointer to \ref ADC_Start                                       */
    int32_t                 (*Stop)             (void);                             /*pointer to \ref ADC_Stop                                        */
    int32_t                 (*PowerControl)     (ARM_POWER_STATE state);            /*Pointer to \ref ADC_PowerControl : Control ADC Interface Power  */
    int32_t                 (*Control)          (uint32_t Control, uint32_t arg);   /*Pointer to \ref ADC_Control : Control ADC Interface             */
}const ARM_DRIVER_ADC;

#ifdef _cplusplus
}
#endif
#endif /* DRIVER_ADC_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
