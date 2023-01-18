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
 * @file     Driver_CDC200.h
 * @author   Girish BN and Prasanna Ravi
 * @email    girish.bn@alifsemi.com and prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     30-Sep-2021
 * @brief    Display controller driver header.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#ifndef DRIVER_CDC200_H_
#define DRIVER_CDC200_H_

#include "Driver_Common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define ARM_CDC200_API_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)  /* API version */

/**
\brief CDC200 event types
*/
#define ARM_CDC_DSI_ERROR_EVENT          (1U << 0)    ///< DSI error event

// Function documentation
/**
  \fn          ARM_DRIVER_VERSION ARM_CDC200_GetVersion (void)
  \brief       Get CDC200 driver version.
  \return      \ref ARM_DRIVER_VERSION.

  \fn          ARM_CDC200_CAPABILITIES ARM_CDC200_GetCapabilities (void)
  \brief       Get CDC200 driver capabilities.
  \return      \ref ARM_CDC200_CAPABILITIES.

  \fn          int32_t ARM_CDC200_Initialize (ARM_CDC200_SignalEvent_t cb_event)
  \brief       Initialize CDC200 Interface.
  \param[in]   cb_event Pointer to ARM_CDC200_SignalEvent_t.
  \return      \ref execution_status.

  \fn          int32_t ARM_CDC200_Uninitialize (void)
  \brief       Uninitialize CDC200 Interface.
  \return      \ref execution_status.

  \fn          int32_t ARM_CDC200_PowerControl (ARM_POWER_STATE state)
  \brief       Control CDC200 Interface Power.
  \param[in]   state  Power state.
  \return      \ref execution_status.

  \fn          int32_t ARM_CDC200_Control (ARM_CDC200_CONTROL control, uint32_t arg)
  \brief       Control the display controller.
  \param[in]   control CDC200 Configuration.
  \param[in]   arg Argument of operation (optional).
  \return      \ref execution_status.

  \fn          int32_t ARM_CDC200_StartDisplay (void)
  \brief       Start the display controller.
  \return      \ref execution_status.

  \fn          int32_t ARM_CDC200_StopDisplay (void)
  \brief       Stop the display controller.
  \return      \ref execution_status.

  \fn          void ARM_CDC200_SignalEvent (uint32_t int_event)
  \brief       Signal CDC200 Events.
  \param[in]   int_event  \ref CDC200 event types.
  \return      none.
*/

/**
  \brief CDC200 Configuration control.
  */
typedef enum _ARM_CDC200_CONTROL {
	CDC200_CONFIGURE_DISPLAY      = (1U << 0),
	CDC200_FRAMEBUF_UPDATE        = (1U << 2),
	CDC200_SHADOW_RELOAD_REGISTER = (1U << 3)
}ARM_CDC200_CONTROL;

/**
\brief CDC200 signal event.
*/
typedef void (*ARM_CDC200_SignalEvent_t) (uint32_t  int_event); ///< Pointer to \ref ARM_CDC200_SignalEvent : Signal CDC200 Event.

/**
  \brief CDC200 driver capabilities.
  */
typedef struct {
	uint32_t reentrant_operation         :1;    ///< Support for reentrant calls
	uint32_t dpi_interface               :1;    ///< Support video mode Interface
	uint32_t reserved                    :30;   ///< Reserved (must be zero)
}ARM_CDC200_CAPABILITIES;

/**
  \brief Access structure of the CDC200 Driver.
  */
typedef struct {
	ARM_DRIVER_VERSION           (*GetVersion)      (void);                                     ///< Pointer to \ref ARM_CDC200_GetVersion : Get driver version.
	ARM_CDC200_CAPABILITIES      (*GetCapabilities) (void);                                     ///< Pointer to \ref ARM_CDC200_GetCapabilities : Get CDC200 driver capabilities.
	int32_t                      (*Initialize)      (ARM_CDC200_SignalEvent_t cb_event);        ///< Pointer to \ref ARM_CDC200_Initialize : Initialize CDC200 Interface.
	int32_t                      (*Uninitialize)    (void);                                     ///< Pointer to \ref ARM_CDC200_Uninitialize : Uninitialize CDC200 Interface.
	int32_t                      (*PowerControl)    (ARM_POWER_STATE state);                    ///< Pointer to \ref ARM_CDC200_PowerControl : Control CDC200 Interface Power.
	int32_t                      (*Control)         (ARM_CDC200_CONTROL control, uint32_t arg); ///< Pointer to \ref ARM_CDC200_Control:  Control CDC200 Interface.
	int32_t                      (*Start)           (void);                                     ///< Pointer to \ref ARM_CDC200_StartDisplay : Configure CDC200 to start Displaying.
	int32_t                      (*Stop)            (void);                                     ///< Pointer to \ref ARM_CDC200_StopDisplay : Configure CDC200 to stop Displaying.
}ARM_DRIVER_CDC200;

#ifdef  __cplusplus
}
#endif

#endif /* DRIVER_CDC200_H_ */
