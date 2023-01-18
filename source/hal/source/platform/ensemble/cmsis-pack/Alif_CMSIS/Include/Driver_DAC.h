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
 * @file     Driver_DAC.h
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     22-Feb-2022
 * @brief    DAC(Digital to Analog Converter) driver definitions.
 ******************************************************************************/

#ifndef DRIVER_DAC_H
#define DRIVER_DAC_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include "Driver_Common.h"

#define ARM_DAC_API_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)  /* API version */

/* DAC Control Code */
#define ARM_DAC_RESET    (0x01UL)  /* To reset the DAC */

/* Function documentation */
/**
  @fn          ARM_DRIVER_VERSION GetVersion (void)
  @brief       Get DAC driver version.
  @return      @ref DAC_DRIVER_VERSION

  @fn          ARM_DAC_CAPABILITIES GetCapabilities (void)
  @brief       Get DAC driver capabilities
  @return      @ref DAC_CAPABILITIES

  @fn          int32_t DAC_Initialize (DAC_DRV_INFO      *dac)
  @brief       Initialize the DAC interface
  @param[in]   dac : Pointer to dac resources
  @return      \ref execution_status

  @fn          int32_t DAC_Uninitialize (DAC_DRV_INFO *dac)
  @brief       De-Initialize the DAC interface
  @param[in]   dac : Pointer to dac resources
  @return      \ref execution_status

  @fn          int32_t DAC_PowerControl (ARM_POWER_STATE   state)
  @brief       CMSIS-DRIVER DAC power control
  @param[in]   state : Power state
  @param[in]   DAC   : Pointer to DAC resources
  @return      \ref execution_status

  @fn          int32_t DAC_Control (uint32_t control,uint32_t arg,DAC_resources_t *dac)
  @brief       CMSIS-Driver dac control
  @param[in]   control : Operation \ref Driver_DAC.h : DAC control codes
  @param[in]   arg     : Argument of operation (optional)
  @param[in]   dac     : Pointer to dac resources
  @return      common \ref execution_status

  @fn          DAC_Start (DAC_resources_t *dac)
  @brief       CMSIS-Driver DAC Start
  @param[in]   dac  : Pointer to dac resources
  @return      \ref execution_status

  @fn          DAC_Stop (DAC_resources_t *dac)
  @brief       CMSIS-Driver DAC Stop
  @param[in]   dac : Pointer to dac resources
  @return      \ref execution_status

  @fn          DAC_SetInput(uint32_t num, DAC_resources_t *dac)
  @brief       CMSIS-Driver to set the DAC input and output will be in GPIO pins
  @param[in]   Input : Operation
  @param[in]   value : DAC input
  @param[in]   dac   : Pointer to dac device resources
  @return      \ref execution_status
 */

/**
@brief DAC Device Driver Capabilities.
*/
typedef struct ARM_DAC_CAPABILITIES {
	uint32_t resolution : 1;   /* DAC Resolution */
	uint32_t reserved   : 31;  /* Reserved (must be Zero) */
}ARM_DAC_CAPABILITIES;

/**
@brief Access structure of the DAC Driver.
*/
typedef struct ARM_Driver_DAC
{
    ARM_DRIVER_VERSION    (*GetVersion)      (void);                           /* pointer is pointing to DAC_GetVersion : used to get the driver version */
    ARM_DAC_CAPABILITIES  (*GetCapabilities) (void);                           /* pointer is pointing to DAC_Capabilities : used to get the driver capabilities */
    int32_t               (*Initialize)      (void);                           /* Initialize the DAC Interface */
    int32_t               (*Uninitialize)    (void);                           /* Pointer to DAC_Uninitialize : De-initialize DAC Interface */
    int32_t               (*PowerControl)    (ARM_POWER_STATE state);          /* Pointer to DAC_PowerControl : Control DAC Interface Power */
    int32_t               (*Control)         (uint32_t control, uint32_t arg); /* Pointer to DAC_Control : Control DAC Interface */
    int32_t               (*Start)           (void);                           /* Pointer to start DAC */
    int32_t               (*Stop)            (void);                           /* Pointer to stop the DAC */
    int32_t               (*SetInput)        (uint32_t value);                 /* Pointer to Set_input */
}const ARM_DRIVER_DAC;

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_DAC_H_ */
