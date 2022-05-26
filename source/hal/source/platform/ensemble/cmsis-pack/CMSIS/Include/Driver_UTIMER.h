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
 * @file     Driver_UTIMER.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for UTIMER.
 * @bug      None.
 * @Note     None
 ******************************************************************************/
#ifndef __DRIVER_UTIMER_H__
#define __DRIVER_UTIMER_H__

#include "Driver_Common.h"

typedef void (*ARM_UTIMER_SignalEvent_t) (uint8_t event);   /**< Initialization UPTIMER call back function declaration >*/

/**< UTIMER Events >*/
#define ARM_UTIMER_CAPTURE_A_EVENT                  0x01    /**< Bit0:CAPTURE_A >*/
#define ARM_UTIMER_CAPTURE_B_EVENT                  0x02    /**< Bit1:CAPTURE_B >*/
#define ARM_UTIMER_CAPTURE_C_EVENT                  0x04    /**< Bit2:CAPTURE_C >*/
#define ARM_UTIMER_CAPTURE_D_EVENT                  0x08    /**< Bit3:CAPTURE_D >*/
#define ARM_UTIMER_CAPTURE_E_EVENT                  0x10    /**< Bit4:CAPTURE_E >*/
#define ARM_UTIMER_CAPTURE_F_EVENT                  0x20    /**< Bit5:CAPTURE_F >*/
#define ARM_UTIMER_UNDER_FLOW_EVENT                 0x40    /**< Bit6:UNDER_FLOW >*/
#define ARM_UTIMER_OVER_FLOW_EVENT                  0x80    /**< Bit7:OVER_FLOW >*/

/**< UTIMER Channel declaration >*/
#define ARM_UTIMER_CHANNEL0                         (0)    /**< UTIMER Channel 0 >*/
#define ARM_UTIMER_CHANNEL1                         (1)    /**< UTIMER Channel 1 >*/
#define ARM_UTIMER_CHANNEL2                         (2)    /**< UTIMER Channel 2 >*/
#define ARM_UTIMER_CHANNEL3                         (3)    /**< UTIMER Channel 3 >*/
#define ARM_UTIMER_CHANNEL4                         (4)    /**< UTIMER Channel 4 >*/
#define ARM_UTIMER_CHANNEL5                         (5)    /**< UTIMER Channel 5 >*/
#define ARM_UTIMER_CHANNEL6                         (6)    /**< UTIMER Channel 6 >*/
#define ARM_UTIMER_CHANNEL7                         (7)    /**< UTIMER Channel 7 >*/
#define ARM_UTIMER_CHANNEL8                         (8)    /**< UTIMER Channel 8 >*/
#define ARM_UTIMER_CHANNEL9                         (9)    /**< UTIMER Channel 9 >*/
#define ARM_UTIMER_CHANNEL10                        (10)   /**< UTIMER Channel 10 >*/
#define ARM_UTIMER_CHANNEL11                        (11)   /**< UTIMER Channel 11 >*/

#define ARM_UTIMER_MODE_BASIC_COUNT_NUMBER                 (2)    /**< UTIMER Basic mode counts number >*/
#define ARM_UTIMER_MODE_SINGLE_BUFFERING_COUNT_NUMBER      (3)    /**< UTIMER single_Buffering mode counts number >*/
#define ARM_UTIMER_MODE_DOUBLE_BUFFERING_COUNT_NUMBER      (4)    /**< UTIMER single_Buffering mode counts number >*/
#define ARM_UTIMER_MODE_TRIGGERING_COUNT_NUMBER            (2)    /**< UTIMER Triggering mode counts number >*/
#define ARM_UTIMER_MODE_CAPTUREING_COUNT_NUMBER            (2)    /**< UTIMER Capturing mode counts number >*/
#define ARM_UTIMER_MODE_COMPARING_COUNT_NUMBER             (5)    /**< UTIMER Comparing mode counts number >*/
#define ARM_UTIMER_MODE_DEAD_TIME_COUNT_NUMBER             (5)    /**< UTIMER Dead timer mode counts number >*/

#define UTIMER_COUNTER_CLEAR                               (1)    /**<UTIMER clear count >*/
#define UTIMER_COUNTER_DO_NOT_CLEAR                        (0)    /**<UTIMER Do not clear count >*/

typedef enum _UTIMER_OPERATION {
    ARM_UTIMER_MODE_CONFIG,                                 /**< UTIMER channel mode configure >*/
    ARM_UTIMER_SET_COUNT,                                   /**< UTIMER Channel setting the count for operation >*/
    ARM_UTIMER_GET_COUNT,                                   /**< UTIMER Channel get the count >*/
    ARM_UTIMER_GET_DIRECTION                                /**< UTIMER Channel count direction >*/
} UTIMER_OPERATION;

typedef enum _UTIMER_MODE {
    ARM_UTIMER_MODE_BASIC,                                  /**< UTIMER Channel mode configure to Basic mode >*/
    ARM_UTIMER_MODE_BUFFERING,                              /**< UTIMER Channel mode configure to Buffering mode >*/
    ARM_UTIMER_MODE_TRIGGERING,                             /**< UTIMER Channel mode configure to Triggering mode >*/
    ARM_UTIMER_MODE_CAPTURING,                              /**< UTIMER Channel mode configure to Capturing mode >*/
    ARM_UTIMER_MODE_COMPARING,                              /**< UTIMER Channel mode configure to Comparing mode >*/
    ARM_UTIMER_MODE_DEAD_TIME                               /**< UTIMER Channel mode configure to Dead Timer mode >*/
} UTIMER_MODE;

typedef enum _UTIMER_DIRECTION {
    ARM_UTIMER_COUNT_DIRECTION_UP,                          /**< UTIMER Channel counting direction up >*/
    ARM_UTIMER_COUNT_DIRECTION_DOWN,                        /**< UTIMER Channel counting direction down >*/
    ARM_UTIMER_COUNT_DIRECTION_TRIANGLE,                    /**< UTIMER Channel counting direction triangle >*/
} UTIMER_DIRECTION;

typedef enum _UTIMER_SET_OPERATION {
    ARM_UTIMER_SET_COUNT_FOR_NORMAL_OPERATION,              /**< load the count for Normal timer operation >*/
    ARM_UTIMER_SET_COUNT_FOR_SINGLE_BUFFER_OPERATION,       /**< load count for Single buffer operation >*/
    ARM_UTIMER_SET_COUNT_FOR_DOUBLE_BUFFER_OPERATION,       /**< load count for Double buffer operation >*/
    ARM_UTIMER_SET_COUNT_FOR_DT_UP,                         /**< load count for DT UP timer operation >*/
    ARM_UTIMER_SET_COUNT_FOR_DT_DOWN,                       /**< load count for DT DOWN timer operation >*/
} UTIMER_SET_OPERATION;

typedef enum _UTIMER_GET_OPERATION {
    ARM_UTIMER_GET_COUNT_OF_CURRENT_RUNNING_TIMER,          /**< to get the running timer count value >*/
    ARM_UTIMER_GET_COUNT_OF_DRIVE_A_CAPTURE_VALUE,          /**< to get the capture buffer value >*/
    ARM_UTIMER_GET_COUNT_OF_DRIVE_A_CAPTURE1_VALUE,         /**< to get the capture buffer1 value >*/
    ARM_UTIMER_GET_COUNT_OF_DRIVE_A_CAPTURE2_VALUE,         /**< to get the capture buffer2 value >*/
    ARM_UTIMER_GET_COUNT_OF_DRIVE_B_CAPTURE_VALUE,          /**< to get the capture buffer value >*/
    ARM_UTIMER_GET_COUNT_OF_DRIVE_B_CAPTURE1_VALUE,         /**< to get the capture buffer1 value >*/
    ARM_UTIMER_GET_COUNT_OF_DRIVE_B_CAPTURE2_VALUE,         /**< to get the capture buffer2 value >*/
} UTIMER_GET_OPERATION;

typedef enum _UTIMER_STATUS {
    UTIMER_CHANNEL_UNINITIALIZED,                           /**< UTIMER Channel Un-Initialize state>*/
    UTIMER_CHANNEL_INITIALIZED,                             /**< UTIMER Channel Initialize state>*/
    UTIMER_CHANNEL_POWERED_OFF,                             /**< UTIMER Channel Power Off state>*/
    UTIMER_CHANNEL_POWERED_FULL,                            /**< UTIMER Channel Power Full state>*/
    UTIMER_CHANNEL_CONTROL,                                 /**< UTIMER Channel Control state>*/
    UTIMER_CHANNEL_TRIGGERED,                               /**< UTIMER Channel Trigger state>*/
    UTIMER_CHANNEL_STARTED,                                 /**< UTIMER Channel Start state>*/
    UTIMER_CHANNEL_STOPED                                   /**< UTIMER Channel Stop state>*/
} UTIMER_STATUS;

typedef struct _UTIMER_MODE_CONFIG {
    UTIMER_MODE                 mode;                       /**< UTIMER Channel mode >*/
    UTIMER_DIRECTION            direction;                  /**< UTIMER Channel direction >*/
    uint32_t                   *count_array;                /**< UTIMER Channel Counts >*/
    uint8_t                     count_number;               /**< UTIMER Channel Counts number >*/
} UTIMER_MODE_CONFIG;

typedef struct _UTIMER_SET_OPERATION_CONFIG {
    UTIMER_SET_OPERATION        operation_type;             /**< UTIMER Channel Set operation >*/
    uint32_t                    *count;                     /**< UTIMER Channel Counts >*/
} UTIMER_SET_OPERATION_CONFIG;

typedef struct _UTIMER_GET_OPERATION_CONFIG {
    UTIMER_GET_OPERATION        operation_type;             /**< UTIMER Channel Get operation >*/
    uint32_t                    *count;                     /**< UTIMER Channel Counts >*/
} UTIMER_GET_OPERATION_CONFIG;

/**
\fn         int32_t ARM_UTIMER_Initialize (uint8_t channel, ARM_UTIMER_CBEvent CB_event)
\brief      Initialize UTIMER interface and register signal(call back) functions.
\param[in]  channel :Utimer have total 12 channel, configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  CB_event: call back function.
\param[out] int32_t : execution_status.

\fn         int32_t ARM_UTIMER_PowerControl (uint8_t channel, ARM_POWER_STATE state);
\brief      Control UTIMER channel interface power.
\param[in]  channel : Utimer have total 16 channel, configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  state   : Power state
                    - \ref ARM_POWER_OFF :  power off: no operation possible
                    - \ref ARM_POWER_LOW :  low power mode: retain state, detect and signal wake-up events
                    - \ref ARM_POWER_FULL : power on: full operation at maximum performance
\param[out] int32_t : execution_status.

\fn         int32_t ARM_CAN_Control (uint8_t channel, uint32_t control, uint32_t arg)
\brief      Control UTIMER interface.
\param[in]  channel     : Utimer have total 16 channel, configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  control_code: Operation
                        - ARM_UTIMER_MODE_CONFIG:   UTIMER channel mode configure
                        - ARM_UTIMER_SET_COUNT:     UTIMER Channel setting the count for operation
                        - ARM_UTIMER_GET_COUNT:     UTIMER Channel get the count
                        - ARM_UTIMER_GET_DIRECTION: UTIMER Channel count direction
 param[in]  arg         : Argument of operation.
 param[out] int32_t     : execution_status

\fn         int32_t ARM_UTIMER_Configure_Trigger (uint8_t channel, uint8_t triggerFor, uint32_t triggerType);
\brief      Configure the UTMER Channel for trigger operation.
\param[in]  channel     : Utimer have total 16 channel, configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  triggerFor  : trigger configuration for which operation.
                        - ARM_UTIMER_TRIGGER_FOR_START:     Trigger for UTIMER Channel start.
                        - ARM_UTIMER_TRIGGER_FOR_STOP:      Trigger for UTIMER Channel stop.
                        - ARM_UTIMER_TRIGGER_FOR_CLEAR:     Trigger for UTIMER Channel clear.
                        - ARM_UTIMER_TRIGGER_FOR_UPCOUNT:   Trigger for UTIMER Channel Up count.
                        - ARM_UTIMER_TRIGGER_FOR_DOWNCOUNT: Trigger for UTIMER Channel Down count.
                        - ARM_UTIMER_TRIGGER_FOR_CAPTURE_A: Trigger for UTIMER Channel to capture a value in Drive A.
                        - ARM_UTIMER_TRIGGER_FOR_CAPTURE_B  Trigger for UTIMER Channel to capture a value in Drive B.
\param[in]  triggerType : type of trigger.
\param[out] int32_t     : execution_status.

\fn         int32_t ARM_UTIMER_Start (uint8_t channel);
\brief      Start the UTMER Channel counting.
\param[in]  channel : Utimer have total 16 channel, configure by feeding TIMER_CHANNEL0,1,2,3....
\param[out] int32_t : execution_status.

\fn         int32_t ARM_UTIMER_Stop (uint8_t channel, bool count_clear_option);
\brief      Stop the UTMER Channel running count.
\param[in]  channel : Utimer have total 16 channel, configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  count_clear_option : Utimer set = count clear, clear = count unclear
\param[out] int32_t : execution_status.

\fn         int32_t ARM_UTIMER_Uninitialize (uint8_t channel);
\brief      Un-initialize the named channel configuration
\param[in]  channel: Utimer have total 16 channel, configure by feeding TIMER_CHANNEL0,1,2,3....
\param[out] int32_t : execution_status.
*/

typedef struct _ARM_DRIVER_UTIMER{
    int32_t (*Initialize)              (uint8_t channel, ARM_UTIMER_SignalEvent_t cb_event);        /**< Pointer to \ref ARM_TIMER_Initialize   : Initialize the UTIMER Interface>*/
    int32_t (*PowerControl)            (uint8_t channel, ARM_POWER_STATE state);                    /**< Pointer to \ref ARM_TIMER_PowerControl : Control UTIMER interface power>*/
    int32_t (*Control)                 (uint8_t channel, uint32_t control_code, void *arg);         /**< Pointer to \ref ARM_TIMER_Control      : Control UTIMER interface.>*/
    int32_t (*Configure_Trigger)       (uint8_t channel, uint8_t triggerFor, uint32_t triggerType); /**< Pointer to \ref ARM_TIMER_Configure_Trigger: configure the triggers>*/
    int32_t (*Start)                   (uint8_t channel);                                           /**< Pointer to \ref ARM_TIMER_Start        : global starts the UTIMER channel>*/
    int32_t (*Stop)                    (uint8_t channel, bool count_clear_option);                  /**< Pointer to \ref ARM_TIMER_Stop         : global stops  the UTIMER channel>*/
    int32_t (*Uninitialize)            (uint8_t channel);                                           /**< Pointer to \ref ARM_TIMER_Uninitialize : Uninitialized the UTIMER Interface>*/
} ARM_DRIVER_UTIMER;

#endif /*< __DRIVER_UTIMER_H__>*/
