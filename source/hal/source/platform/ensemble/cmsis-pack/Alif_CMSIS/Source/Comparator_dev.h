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
 * @file     Comparator_dev.h
 * @author   Nisarga A M
 * @email    nisarga.am@alifsemi.com
 * @version  V1.0.0
 * @date     20-June-2022
 * @brief    Device Specific Header file for Analog Comparator Driver.
 ******************************************************************************/

#ifndef COMPARATOR_DEV_H_
#define COMPARATOR_DEV_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/* System includes */
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header

#define CMP0_ENABLE                    (1<<28)  /* To enable the Comp0 */
#define CMP1_ENABLE                    (1<<29)  /* To enable the Comp1 */
#define CMP2_ENABLE                    (1<<30)  /* To enable the Comp2 */
#define CMP3_ENABLE                    (1<<31)  /* To enable the Comp3 */

#define CMP_FILTER_CONTROL_ENABLE      (1<<0)  /* To enable the filter control */
#define CMP_WINDOW_CONTROL_A0_ENABLE   (1<<1)  /* To enable the window control for A0 board */
#define CMP_WINDOW_CONTROL_B0_ENABLE   (3<<0)  /* To enable the window control for B0 board */
#define CMP_PRESCALER_MAX_VALUE        (0x3F)  /* Maximum value of prescaler control */

#define CMP_INT_MASK                   (0x01UL)  /* Mask for the interrupt */
#define CMP_INTERRUPT_CLEAR            (0x01UL)  /* to clear the interrupt */

/**
 @brief : Comparator flags to check the Comparator initialization and Comparator power done.
 */
typedef enum {
    CMP_FLAG_DRV_INIT_DONE    = (1U << 0),  /*Comparator Driver is Initialized */
    CMP_FLAG_DRV_POWER_DONE   = (1U << 1),  /*Comparator Driver is Powered */
} CMP_FLAG_Type;

/**
 @brief struct CMP_TypeDef:- Register map for Analog Comparator
 */
typedef struct _CMP_TypeDef
{
    __IOM uint32_t reg1;           /* Output register value at (0x00),used to drive configuration bits to analog portion. */
    __IOM uint32_t reg2;           /* Output register value at (0x04),used to drive configuration bits to analog portion. */
    __IOM uint32_t polarity_ctrl;  /* Polarity control register at (0x08), If active, invert the value of cmp_in before processing. */
    __IOM uint32_t window_ctrl;    /* Window control register at (0x0C), always process comparator inputs or use 4 events_in inputs to define processing window */
    __IOM uint32_t filter_ctrl;    /* Filter control register at (0x10), Enable the filtering function, number filter taps, 2 - 8 */
    __IOM uint32_t prescaler_ctrl; /* Pre-scaler control register at (0x14), number of clocks between updating taps with new comparator input */
    uint32_t RESERVED1;            /* reserved at (0x18) */
    uint32_t RESERVED2;            /* reserved at (0x1C) */
    __IOM uint32_t interrupt;      /* interrupt register at (0x20), filter event occurred, ie number programmed taps was active. Write 1 to clear */
    __IOM uint32_t interrupt_mask; /* interrupt mask register at (0x24) */
}CMP_TypeDef;

/**
 * struct CMP_resources: structure representing a Analog comparator device
 * @reg_base           : Register address of the DAC
 * @flags              : DAC driver flags
 * @irq_num            : Comparator interrupt number
 * @config             : DAC configuration information
 * @irq_priority       : Comparator interrupt Priority
 * @output_mux_sel     : Comparator output mux
 * @input_enable       : To select the comparator
 */
typedef struct CMP_resources{
    ARM_Comparator_SignalEvent_t  cb_event;        /* Comparator application event callback */
    CMP_TypeDef                   *reg_base;       /* Comparator register base address */
    uint8_t                       flags;           /* Comparator Driver Flags */
    IRQn_Type                     irq_num;         /* Comparator interrupt number */
    uint32_t                      config;          /* Comparator configuration information */
    uint32_t                      irq_priority;    /* Comparator interrupt Priority */
    uint32_t                      output_mux_sel;  /* Comparator output mux */
    uint32_t                      input_enable;    /* To select the comparator */
}CMP_resources_t;

#endif /* COMPARATOR_DEV_H_ */
