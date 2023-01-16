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
#define ADC_START_SRC                            (1 << 6)     /* start the ADC driver                  */
#define ADC_PROCESS_CONTROL                      (1 << 0)     /* start the adc process control         */
#define ADC_SHIFT_CONTROL_RIGHT_OR_LEFT          (1 << 16)    /* Enable the right or left shift        */
#define ADC_SEQUENCER_CTRL_FIXED_OR_ROTATE       (1 << 16)    /* ENable the to select particular value */

/********Interrupt macro*******/
#define ADC_INTR_COMP0_MSK                       (0x04)       /* Interrupt comparator 0 mask   */
#define ADC_INTR_COMP1_MSK                       (0x08)       /* Interrupt comparator 1 mask   */
#define ADC_THRSHLD_COMP_MASK_BIT                (0x3)        /* Comparator threshold mask bit */

/****Interrupt clear macros****/
#define ADC_INTR_DONE_CLEAR                      (0x01)       /* Interrupt done clear bit  */
#define ADC_INTR_DONE2_CLEAR                     (0x02)       /* Interrupt done2 clear bit */
#define ADC_INTR_COMP0_CLEAR                     (0x04)       /* Interrupt comp0 clear bit */
#define ADC_INTR_COMP1_CLEAR                     (0x08)       /* Interrupt comp1 clear bit */

/****Comparator Macros****/
#define ADC_COMP_THRHLD_ABOVE_A                   0           /* ADC comparator threshold above A     */
#define ADC_COMP_THRHLD_BELOW_A                   1           /* ADC comparator threshold below A     */
#define ADC_COMP_THRHLD_BETWEEN_A_B               2           /* ADC comparator threshold between a_b */

#define ADC_COMP_THRHLD_ABOVE_B                   0           /* ADC comparator threshold above B     */
#define ADC_COMP_THRHLD_BELOW_B                   1           /* ADC comparator threshold above B     */
#define ADC_COMP_THRHLD_OUTSIDE_A_B               2           /* ADC comparator threshold outside A_B */

/****channels Macros****/
#define ADC_LAST_AVAILABLE_CHANNEL                8           /* ADC last available channels */

/****limit Macros****/
#define ADC_MOD_BY_TWO                            2           /* ADC Modulus by two                     */
#define ADC_CLOCK_DIV_MIN_VALUE                   2           /* ADC Clock divisor minimum value        */
#define ADC_CLOCK_DIV_MAX_VALUE                   64          /* ADC Clock divisor maximum value        */
#define ADC_AVG_SAMPLES_FOR_AVG_MIN               2           /* ADC Average sample for Avergae minimum */
#define ADC_AVG_SAMPLES_FOR_AVG_MAX               256         /* ADC Average sample for Avergae maximum */
#define ADC_SAMPLE_WIDTH_MIN_VALUE                2           /* ADC sample width minimum value         */
#define ADC_SAMPLE_WIDTH_MAX_VALUE                32          /* ADC sample width maximum value         */


/****Shift bit macro****/
#define ADC_SHIFT_BIT                             16          /* Shift bit         */
#define ADC_INIT_SHIFT_BIT                        12          /* Initial shift bit */

/****Sequencer Macros****/
#define ADC_SEQUENCER_MSK_BIT                    (0x01)       /* Sequencer mask bit   */
#define ADC_MAX_INIT_CHANNEL                     (0X100)      /* Initial max channel  */
#define ADC_MSK_INIT_CHANNEL                     (0X0F)       /* Initial mask channel */
#define ADC_MSK_ALL_CHANNELS                     (0X1FF)      /* Masking all channel  */

/****Interrupt****/
#define ADC_INT_AVG_SAMPLE_RDY                    (1U)        /* Interrupt for average sample ready */
#define ADC_INT_AVG_SAMPLE_TAKEN                  (2U)        /* Interrupt for all sample taken     */

typedef enum {
    ADC_FLAG_DRV_INIT_DONE    =                   (1U << 0),  /* ADC Driver is Initialized */
    ADC_FLAG_DRV_POWER_DONE   =                   (1U << 1),  /* ADC Driver is Powered     */
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
    uint32_t    user_input;             /* user channel input            */
    uint32_t    clock_div;              /* clock divisor                 */
    uint32_t    sample_width;           /* sample width                  */
    uint32_t    avg_sample_num;         /* average sample number         */
    uint32_t    shift_n_bit;            /* number of bits to shift       */
    uint32_t    shift_left_or_right;    /* shift bit left or right       */
    uint8_t     comp_ctrl_status;       /* ADC comparator control status */
    uint8_t     sequencer_ctrl_status;  /* sequencer control status      */
    uint8_t     last_read_channel;      /* last channel read             */
}ADC_config_info_t;

/* Access structure for the saving the ADC Setting and status*/
typedef struct ADC_resources{
    ARM_ADC_SignalEvent_t   cb_event;                  /* ADC APPLICATION CALLBACK EVENT                   */
    ADC_config_info_t       *config;                   /* ADC user Configuration                           */
    ADC_TypeDef             *reg_base;                 /* ADC register base address                        */
    conv_info_t             *conv_info;                /* ADC conversion information                       */
    conv_status_t           conv_status;               /* ADC conversion status                            */
    uint32_t                flags;                     /* ADC flag event                                   */
    IRQn_Type               intr_done_irq_num;         /* ADC interrupt number                             */
    uint32_t                intr_done_irq_priority;    /* Irq Priority                                     */
    uint32_t                adc_reg1_value;            /* Common Analog configuration for all ADC instance */
}ADC_resources_t;

#endif /* ADC_DEV_H_ */
