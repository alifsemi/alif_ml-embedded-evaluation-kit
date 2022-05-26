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
 * @file     Driver_MIPI_DSI.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     24-March-2022
 * @brief    Driver Specific Header file for MIPI DSI Driver.
 ******************************************************************************/
#ifndef DRIVER_MIPI_DSI_H_
#define DRIVER_MIPI_DSI_H_

#include "Driver_Common.h"

#define ARM_MIPI_DSI_API_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)  /* API version */

// Function documentation
/**
  \fn          ARM_DRIVER_VERSION ARM_MIPI_DSI_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION

  \fn          int32_t  ARM_MIPI_DSI_Initialize (uint32_t frequency)
  \brief       Initialize MIPI DSI Interface.
  \param[in]   frequency to configure DPHY PLL.
  \return      \ref execution_status

  \fn          int32_t ARM_MIPI_DSI_Uninitialize (void)
  \brief       uninitialize MIPI DSI Interface.
  \return      \ref execution_status

  \fn          int32_t ARM_MIPI_DSI_PowerControl (ARM_POWER_STATE state)
  \brief       Control MIPI DSI Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status

  \fn          int32_t ARM_MIPI_DSI_StartPHY (void)
  \brief       Configure MIPI DSI PHY Interface.
  \return      \ref execution_status
*/

/**
\brief Access structure of the MIPI DSI Driver
*/
typedef struct {
	ARM_DRIVER_VERSION                  (*GetVersion)      (void);                   ///< Pointer to \ref ARM_MIPI_DSI_GetVersion : Get driver version.
	int32_t                             (*Initialize)      (uint32_t frequency);     ///< Pointer to \ref ARM_MIPI_DSI_Initialize : Initialize MIPI DSI Interface.
	int32_t                             (*Uninitialize)    (void);                   ///< Pointer to \ref ARM_MIPI_DSI_Uninitialize : Uninitialize MIPI DSI Interface.
	int32_t                             (*PowerControl)    (ARM_POWER_STATE state);  ///< Pointer to \ref ARM_MIPI_DSI_PowerControl : Control MIPI DSI Interface Power.
	int32_t                             (*StartPHY)        (void);                   ///< Pointer to \ref ARM_MIPI_DSI_StartPHY : Configure MIPI DSI PHY Interface.
}ARM_DRIVER_MIPI_DSI;


#endif /* DRIVER_MIPI_DSI_H_ */
