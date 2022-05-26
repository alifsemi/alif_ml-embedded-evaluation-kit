/* Copyright (c)  2022 ALIF SEMICONDUCTOR

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
