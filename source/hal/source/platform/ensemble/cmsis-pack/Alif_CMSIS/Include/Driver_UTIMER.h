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

typedef void (*ARM_UTIMER_SignalEvent_t) (uint32_t event);   /**< Initialization UPTIMER call back function declaration >*/

/**< UTIMER Events >*/
#define ARM_UTIMER_CAPTURE_A_EVENT                  0x01    /**< Bit0:CAPTURE_A >*/
#define ARM_UTIMER_CAPTURE_B_EVENT                  0x02    /**< Bit1:CAPTURE_B >*/
#define ARM_UTIMER_CAPTURE_C_EVENT                  0x04    /**< Bit2:CAPTURE_C >*/
#define ARM_UTIMER_CAPTURE_D_EVENT                  0x08    /**< Bit3:CAPTURE_D >*/
#define ARM_UTIMER_CAPTURE_E_EVENT                  0x10    /**< Bit4:CAPTURE_E >*/
#define ARM_UTIMER_CAPTURE_F_EVENT                  0x20    /**< Bit5:CAPTURE_F >*/
#define ARM_UTIMER_UNDER_FLOW_EVENT                 0x40    /**< Bit6:UNDER_FLOW >*/
#define ARM_UTIMER_OVER_FLOW_EVENT                  0x80    /**< Bit7:OVER_FLOW >*/
#define ARM_QEC_COMPARE_A_EVENT                     0x100   /**< Bit8:QEC COMPARE A >*/
#define ARM_QEC_COMPARE_B_EVENT                     0x200   /**< Bit9:QEC COMPARE B >*/

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
#define ARM_UTIMER_CHANNEL12                        (12)   /**< UTIMER Channel 12 >*/
#define ARM_UTIMER_CHANNEL13                        (13)   /**< UTIMER Channel 13 >*/
#define ARM_UTIMER_CHANNEL14                        (14)   /**< UTIMER Channel 14 >*/
#define ARM_UTIMER_CHANNEL15                        (15)   /**< UTIMER Channel 15 >*/

#define ARM_UTIMER_MAX_CHANNEL                      (16)

#define ARM_UTIMER_MODE_BASIC_COUNT_NUMBER                 (2)    /**< UTIMER Basic mode counts number >*/
#define ARM_UTIMER_MODE_SINGLE_BUFFERING_COUNT_NUMBER      (3)    /**< UTIMER single_Buffering mode counts number >*/
#define ARM_UTIMER_MODE_DOUBLE_BUFFERING_COUNT_NUMBER      (4)    /**< UTIMER single_Buffering mode counts number >*/
#define ARM_UTIMER_MODE_TRIGGERING_COUNT_NUMBER            (2)    /**< UTIMER Triggering mode counts number >*/
#define ARM_UTIMER_MODE_CAPTURING_COUNT_NUMBER             (2)    /**< UTIMER Capturing mode counts number >*/
#define ARM_UTIMER_MODE_COMPARING_COUNT_NUMBER             (5)    /**< UTIMER Comparing mode counts number >*/
#define ARM_UTIMER_MODE_DEAD_TIME_COUNT_NUMBER             (8)    /**< UTIMER Dead timer mode counts number >*/
#define ARM_QEC_COUNT_NUMBER                               (2)    /**< QEC counts number >*/

#define ARM_UTIMER_TRIGGER_FOR_START                (1)    /**< UTIMER trigger for counter start >*/
#define ARM_UTIMER_TRIGGER_FOR_STOP                 (2)    /**< UTIMER trigger for counter stop >*/
#define ARM_UTIMER_TRIGGER_FOR_CLEAR                (3)    /**< UTIMER trigger for counter clear >*/
#define ARM_UTIMER_TRIGGER_FOR_UPCOUNT              (4)    /**< UTIMER trigger for counter upcount >*/
#define ARM_UTIMER_TRIGGER_FOR_DOWNCOUNT            (5)    /**< UTIMER trigger for counter downcount >*/
#define ARM_UTIMER_TRIGGER_FOR_CAPTURE_A            (6)    /**< UTIMER trigger for input capture a >*/
#define ARM_UTIMER_TRIGGER_FOR_CAPTURE_B            (7)    /**< UTIMER trigger for input capture b >*/

#define ARM_UTIMER_TRIG0_RISING                     ((uint32_t)0x00000001)    /**< UTIMER trigger0 rising >*/
#define ARM_UTIMER_TRIG0_FALLING                    ((uint32_t)0x00000002)    /**< UTIMER trigger0 falling >*/
#define ARM_UTIMER_TRIG1_RISING                     ((uint32_t)0x00000004)    /**< UTIMER trigger1 rising >*/
#define ARM_UTIMER_TRIG1_FALLING                    ((uint32_t)0x00000008)    /**< UTIMER trigger1 falling >*/
#define ARM_UTIMER_TRIG2_RISING                     ((uint32_t)0x00000010)    /**< UTIMER trigger2 rising >*/
#define ARM_UTIMER_TRIG2_FALLING                    ((uint32_t)0x00000020)    /**< UTIMER trigger2 falling >*/
#define ARM_UTIMER_TRIG3_RISING                     ((uint32_t)0x00000040)    /**< UTIMER trigger3 rising >*/
#define ARM_UTIMER_TRIG3_FALLING                    ((uint32_t)0x00000080)    /**< UTIMER trigger3 falling >*/
#define ARM_UTIMER_DRIVE_A_RISING_B_0               ((uint32_t)0x00000100)    /**< UTIMER chan_event_a rising, chan_event_b 0 >*/
#define ARM_UTIMER_DRIVE_A_RISING_B_1               ((uint32_t)0x00000200)    /**< UTIMER chan_event_a rising, chan_event_b 1 >*/
#define ARM_UTIMER_DRIVE_A_FALLING_B_0              ((uint32_t)0x00000400)    /**< UTIMER chan_event_a falling, chan_event_b 0 >*/
#define ARM_UTIMER_DRIVE_A_FALLING_B_1              ((uint32_t)0x00000800)    /**< UTIMER chan_event_a falling, chan_event_b 0 >*/
#define ARM_UTIMER_DRIVE_B_RISING_A_0               ((uint32_t)0x00001000)    /**< UTIMER chan_event_a 0, chan_event_b rising >*/
#define ARM_UTIMER_DRIVE_B_RISING_A_1               ((uint32_t)0x00002000)    /**< UTIMER chan_event_a 1, chan_event_b rising >*/
#define ARM_UTIMER_DRIVE_B_FALLING_A_0              ((uint32_t)0x00004000)    /**< UTIMER chan_event_a 0, chan_event_b falling >*/
#define ARM_UTIMER_DRIVE_B_FALLING_A_1              ((uint32_t)0x00008000)    /**< UTIMER chan_event_a 1, chan_event_b falling >*/
#define ARM_UTIMER_GLB0_EVENT                       ((uint32_t)0x00010000)    /**< UTIMER glb_event_0 */
#define ARM_UTIMER_GLB1_EVENT                       ((uint32_t)0x00020000)    /**< UTIMER glb_event_1 */
#define ARM_UTIMER_GLB2_EVENT                       ((uint32_t)0x00040000)    /**< UTIMER glb_event_2 */
#define ARM_UTIMER_GLB3_EVENT                       ((uint32_t)0x00080000)    /**< UTIMER glb_event_3 */
#define ARM_UTIMER_GLB4_EVENT                       ((uint32_t)0x00100000)    /**< UTIMER glb_event_4 */
#define ARM_UTIMER_GLB5_EVENT                       ((uint32_t)0x00200000)    /**< UTIMER glb_event_5 */
#define ARM_UTIMER_GLB6_EVENT                       ((uint32_t)0x00400000)    /**< UTIMER glb_event_6 */
#define ARM_UTIMER_GLB7_EVENT                       ((uint32_t)0x00800000)    /**< UTIMER glb_event_7 */

#define UTIMER_COUNTER_CLEAR                        (1)    /**<UTIMER clear count >*/
#define UTIMER_COUNTER_DO_NOT_CLEAR                 (0)    /**<UTIMER Do not clear count >*/

#define ARM_UTIMER_CHANNEL_RUNNING                  (1)    /**< counter running >*/
#define ARM_UTIMER_CHANNEL_STOPPED                  (!ARM_UTIMER_CHANNEL_RUNNING)    /**< counter stopped >*/

#define ARM_UTIMER_DIRECTION_DOWN                   (1)    /**< down counter direction >*/
#define ARM_UTIMER_DIRECTION_UP                     (!ARM_UTIMER_DIRECTION_DOWN)    /**< up counter direction >*/

#define ARM_UTIMER_INTRRUPT_UNMASK                  (uint32_t)0x00000000    /**< interrupt unmask >*/
#define ARM_UTIMER_INTRRUPT_MASK                    (uint32_t)0x000000FF    /**< interrupt mask >*/

#define ARM_UTIMER_IRQ_TYPE_CAPTURE_A               (1)    /**< A - compare/capture_a match >*/
#define ARM_UTIMER_IRQ_TYPE_CAPTURE_B               (2)    /**< B - compare/capture_b match >*/
#define ARM_UTIMER_IRQ_TYPE_CAPTURE_C               (3)    /**< C - compare_a_buf1 match >*/
#define ARM_UTIMER_IRQ_TYPE_CAPTURE_D               (4)    /**< D - compare_a_buf2 match >*/
#define ARM_UTIMER_IRQ_TYPE_CAPTURE_E               (5)    /**< E - compare_b_buf1 match >*/
#define ARM_UTIMER_IRQ_TYPE_CAPTURE_F               (6)    /**< F - compare_b_buf2 match >*/
#define ARM_UTIMER_IRQ_TYPE_UNDER_FLOW              (7)    /**<   - UNDER FLOW >*/
#define ARM_UTIMER_IRQ_TYPE_OVER_FLOW               (8)    /**<   - OVER FLOW >*/
#define ARM_QEC_IRQ_TYPE_COMPARE_A                  (9)    /**< A - compare for QEC >*/
#define ARM_QEC_IRQ_TYPE_COMPARE_B                  (10)   /**< B - compare for QEC >*/

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
\param[in]  channel : Utimer supports 12 channels(0-11), configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  CB_event: call back function.
\param[out] int32_t : execution_status.

\fn         int32_t ARM_UTIMER_PowerControl (uint8_t channel, ARM_POWER_STATE state);
\brief      Control UTIMER channel interface power.
\param[in]  channel : Utimer supports 12 channels(0-11), configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  state   : Power state
                    - \ref ARM_POWER_OFF :  power off: no operation possible
                    - \ref ARM_POWER_LOW :  low power mode: retain state, detect and signal wake-up events
                    - \ref ARM_POWER_FULL : power on: full operation at maximum performance
\param[out] int32_t : execution_status.

\fn         int32_t ARM_UTIMER_Control (uint8_t channel, uint32_t control, void *arg)
\brief      Control UTIMER interface.
\param[in]  channel : Utimer supports 12 channels(0-11), configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  control_code: Operation
                        - ARM_UTIMER_MODE_CONFIG:   UTIMER channel mode configure
                        - ARM_UTIMER_SET_COUNT:     UTIMER Channel setting the count for operation
                        - ARM_UTIMER_GET_COUNT:     UTIMER Channel get the count
                        - ARM_UTIMER_GET_DIRECTION: UTIMER Channel count direction
 param[in]  arg         : Pointer to an argument of operation.
 param[out] int32_t     : execution_status

\fn         int32_t ARM_UTIMER_Configure_Trigger (uint8_t channel, uint8_t triggerFor, uint32_t triggerType);
\brief      Configure the UTIMER Channel for trigger operation.
\param[in]  channel     : Utimer have total 12 channels, configure by feeding TIMER_CHANNEL0,1,2,3....
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
\param[in]  channel : Utimer supports 12 channels(0-11), configure by feeding TIMER_CHANNEL0,1,2,3....
\param[out] int32_t : execution_status.

\fn         int32_t ARM_UTIMER_Stop (uint8_t channel, bool count_clear_option);
\brief      Stop the UTMER Channel running count.
\param[in]  channel : Utimer supports 12 channels(0-11), configure by feeding TIMER_CHANNEL0,1,2,3....
\param[in]  count_clear_option : Utimer set = count clear, clear = count unclear
\param[out] int32_t : execution_status.

\fn         int32_t ARM_UTIMER_Uninitialize (uint8_t channel);
\brief      Un-initialize the named channel configuration
\param[in]  channel : Utimer supports 12 channels(0-11), configure by feeding TIMER_CHANNEL0,1,2,3....
\param[out] int32_t : execution_status.
*/

typedef struct _ARM_DRIVER_UTIMER {
    int32_t (*Initialize)              (uint8_t channel, ARM_UTIMER_SignalEvent_t cb_event);        /**< Pointer to \ref ARM_TIMER_Initialize   : Initialize the UTIMER Interface>*/
    int32_t (*PowerControl)            (uint8_t channel, ARM_POWER_STATE state);                    /**< Pointer to \ref ARM_TIMER_PowerControl : Control UTIMER interface power>*/
    int32_t (*Control)                 (uint8_t channel, uint32_t control_code, void *arg);         /**< Pointer to \ref ARM_TIMER_Control      : Control UTIMER interface.>*/
    int32_t (*Configure_Trigger)       (uint8_t channel, uint8_t triggerFor, uint32_t triggerType); /**< Pointer to \ref ARM_TIMER_Configure_Trigger: configure the triggers>*/
    int32_t (*Start)                   (uint8_t channel);                                           /**< Pointer to \ref ARM_TIMER_Start        : global starts the UTIMER channel>*/
    int32_t (*Stop)                    (uint8_t channel, bool count_clear_option);                  /**< Pointer to \ref ARM_TIMER_Stop         : global stops  the UTIMER channel>*/
    int32_t (*Uninitialize)            (uint8_t channel);                                           /**< Pointer to \ref ARM_TIMER_Uninitialize : Uninitialized the UTIMER Interface>*/
} ARM_DRIVER_UTIMER;

#endif /*< __DRIVER_UTIMER_H__>*/
