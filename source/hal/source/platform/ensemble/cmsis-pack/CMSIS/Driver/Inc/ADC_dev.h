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
 * @file     ADC_dev.h
 * @author   Prabhakar Kumar
 * @email    prabhakar.kumar@alifsemi.com
 * @version  V1.0.0
 * @date     22-feb-2022
 * @brief    Device Specific Header file for ADC Driver.
 ******************************************************************************/

#ifndef ADC_DEV_H_
#define ADC_DEV_H_

/*---System include ----*/
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

/****ADC Register macros****/
#define ADC_START_SRC                           (1 << 6)     /* start the ADC driver                  */
#define ADC_SHIFT_CONTROL_RIGHT_OR_LEFT         (1 << 16)    /* Enable the right or left shift        */
#define ADC_SEQUENCER_CTRL_SELECTONE_OR_ROTATE  (1 << 16)    /* ENable the to select particular value */

/***Interrupt mask macro****/
#define ADC_INTERRUPT_MSK                       (1 << 0)

/****Interrupt****/
#define ADC_INT_AVG_SAMPLE_RDY                  (1U)
#define ADC_INT_AVG_SAMPLE_TAKEN                (2U)

typedef enum {
    ADC_FLAG_DRV_INIT_DONE    = (1U << 0),                   /* ADC Driver is Initialized */
    ADC_FLAG_DRV_POWER_DONE   = (1U << 1),                   /* ADC Driver is Powered     */
} ADC_FLAG_Type;

/* Structure ADC_TypeDef : Register map for ADC */
typedef struct _ADC_TypeDef
{
    __IOM uint32_t START_SRC;                                 /* start ADC register at (0x00) */
    __IOM uint32_t COMP_THRESH_A;                             /* Threshold register A for interrupt generation at (0x04) */
    __IOM uint32_t COMP_THRESH_B;                             /* Threshold register B for interrupt generation at (0x08) */
    __IOM uint32_t CLK_DIVISOR;                               /* Clock divisor register at (0x0C) */
    __IOM uint32_t INTERRUPT;                                 /* interrupt register at (0x10) */
    __IOM uint32_t INTERRUPT_MASK;                            /* interrupt mask register at (0x14) */
    __IOM uint32_t SAMPLE_WIDTH;                              /* sample width register at (0x18)Width in divided clocks of the sample output signal, valid values are 2-32 */
    uint32_t RESERVED1[1];                                    /* reserved at (0x1C) */
    __IOM uint32_t AVG_NUM;                                   /* average number register at (0x20)
                                                               Program up to 256, number of samples that are collected,
                                                               result is loaded into sample registers. Must be power of 2.. */
    __IOM uint32_t SHIFT_CONTROL;                             /* shift control register at (0x24) */
    uint32_t RESERVED2[2];                                    /* reserved at (0x28-0x2C) */
    __IOM uint32_t CONTROL;                                   /* control register at (0x30) */
    __IOM uint32_t SEQUENCER_CTRL;                            /* sequence control register at (0x34) */
    __IOM uint32_t REG1;                                      /* Output register at (0x38) used to drive configuration bits to analog portion. */
    __IM uint32_t ADC_SEL;                                    /* adc select register at (0x3C) */
    uint32_t RESERVED3[4];                                    /* reserved at (0x40-0x4C) */
    __IOM uint32_t SAMPLE_REG_0;                              /* sample register0 at (0x50) */
    __IOM uint32_t SAMPLE_REG_1;                              /* sample register1 at (0x54) */
    __IOM uint32_t SAMPLE_REG_2;                              /* sample register2 at (0x58) */
    __IOM uint32_t SAMPLE_REG_3;                              /* sample register3 at (0x5C) */
    __IOM uint32_t SAMPLE_REG_4;                              /* sample register4 at (0x60) */
    __IOM uint32_t SAMPLE_REG_5;                              /* sample register5 at (0x64) */
    __IOM uint32_t SAMPLE_REG_6;                              /* sample register6 at (0x68) */
    __IOM uint32_t SAMPLE_REG_7;                              /* sample register7 at (0x6C) */
    __IOM uint32_t SAMPLE_REG_8;                              /* sample register8 at (0x70) */
}ADC_TypeDef;

/* Structure to store the conversion info */
typedef struct conv_info{
    uint32_t   *conv_buff;              /* pointer to input buffer   */
    uint32_t   total_cnt;               /* conversion total count    */
    uint32_t   current_cnt;             /* conversion current count  */
}conv_info_t;

/****Structure for conversion status****/
typedef struct conv_status{
    uint8_t conv_busy;                  /* conversion busy flag */
}conv_status_t;

/* Access structure for saving the ADC configuration*/
typedef struct ADC_config_info{
    uint32_t    user_input;             /* user channel input      */
    uint32_t    clock_div;              /* clock divisor           */
    uint32_t    sample_width;           /* sample width            */
    uint32_t    avg_sample_num;         /* average sample number   */
    uint32_t    shift_n_bit;            /* number of bits to shift */
    uint32_t    shift_left_or_right;    /* shift bit left or right */
}ADC_config_info_t;

/* Access structure for the saving the ADC Setting and status*/
typedef struct ADC_resources{
    ARM_ADC_SignalEvent_t   cb_event;                        /* ADC APPLICATION CALLBACK EVENT */
    ADC_config_info_t       *config;                         /* ADC Configuration address      */
    ADC_TypeDef             *reg_base;                       /* ADC register base address      */
    conv_info_t             *conv_info;                      /* ADC conversion information     */
    conv_status_t           conv_status;                     /* ADC conversion status          */
    uint32_t                flags;                           /* ADC flag event                 */
    IRQn_Type               intr_done_irq_num;           /* ADC interrupt number           */
    uint32_t                intr_done_irq_priority;      /* Irq Priority                   */
    uint32_t                confg;                           /* ADC configuration              */
}ADC_resources_t;

#endif /* ADC_DEV_H_ */
