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
 * @file     DAC_dev.h
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     28-Feb-2022
 * @brief    Device Specific Header file for DAC Driver.
 ******************************************************************************/

#ifndef DAC_DEV_H_
#define DAC_DEV_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/* System includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

#define RESET       2  /* To Reset both DAC0 and DAC1 */

/* Control register of DAC */
#define DAC_EN                (1<<0)   /* Enable DAC */
#define DAC_RESET_OR_ENABLE   (1<<27)  /* 0=Reset,1=enable;this will reset/enable both the instance of DAC */
#define DAC_MAX_INPUT         (0xFFF)  /* Maximum input for the DAC is 4095(DAC 12 bit resolution) */

/**
 @brief   : DAC flags to check the DAC initialization,DAC power done and DAC started.
 */
typedef enum {
	DAC_FLAG_DRV_INIT_DONE    = (1U << 0),  /*DAC Driver is Initialized */
	DAC_FLAG_DRV_POWER_DONE   = (1U << 1),  /*DAC Driver is Powered */
	DAC_STARTED               = (1U << 2)  /*DAC Driver is Started */
} DAC_FLAG_Type;

/**
 @brief struct DAC_TypeDef:- Register map for DAC
 */
typedef struct DAC_TypeDef
{
	__IOM uint32_t REG1;   /*DAC REGISTER1*/
	__IOM uint32_t INPUT;  /*DAC INPUT register*/

}DAC_TypeDef_t;

/**
 * struct DAC_DRV_INFO: structure representing a DAC device
 * @reg_base          : Register address of the DAC
 * @flags             : DAC driver flags
 * @config            : DAC configuration information
 */
typedef struct DAC_resources
{
	DAC_TypeDef_t    *reg_base;  /*DAC register address */
	uint8_t          flags;      /*DAC Driver Flags */
	uint32_t         config;     /*DAC configuration information */
}DAC_resources_t;

#endif /* DAC_DEV_H_ */
