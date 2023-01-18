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
 * @file     SPI_ll_drv.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     14-Jun-2021
 * @brief    Low level SPI driver.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef __SPI_LL_DRV_H__
#define __SPI_LL_DRV_H__

#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "RTE_Components.h"
#include CMSIS_device_header

#define SPI_ENABLE                                      1
#define SPI_DISABLE                                     0

#define SPI_IRQ_NUM(instance)                           (SPI0_IRQ + instance)

#define SPI_TX_FIFO_DEPTH                               16
#define SPI_RX_FIFO_DEPTH                               16

#define SPI_MAX_SLAVE_SELECT_PINS                       4
#define SPI_SLAVE_SELECT_PIN_MASK                       0xFFFFFFF0

#define SPI_DATA_FRAME_SIZE_MIN                         4
#define SPI_DATA_FRAME_SIZE_MAX                         32

//TODO: AHB clock enable macros
#define CR_SSI_SS_IN_SEL_0                              (1U  <<  0U)
#define CR_SSI_SS_IN_SEL_1                              (1U  <<  1U)
#define CR_SSI_SS_IN_SEL_2                              (1U  <<  2U)
#define CR_SSI_SS_IN_SEL_3                              (1U  <<  3U)
#define CR_SSI_SS_IN_VAL_0                              (1U  <<  8U)
#define CR_SSI_SS_IN_VAL_1                              (1U  <<  9U)
#define CR_SSI_SS_IN_VAL_2                              (1U  << 10U)
#define CR_SSI_SS_IN_VAL_3                              (1U  << 11U)

/* SPI Control Register 0 (CTRLR0) bit Definition, Macros, Offsets and Masks
 * these include DFS, FRF, SCPH, SCPOL, TMOD, etc
 */
/* Data Frame DFS bit[4:0]*/
#define SPI_CTRLR0_DFS                                  0U
#define SPI_CTRLR0_DFS_MASK                             (0x1FU << SPI_CTRLR0_DFS)

/* Frame Format FRF bit[7:6] */
#define SPI_CTRLR0_FRF                                  6
#define SPI_CTRLR0_FRF_MASK                             (0x3UL << SPI_CTRLR0_FRF)
#define SPI_CTRLR0_FRF_MOTOROLA                         0x00     /* 0x0 Motorola SPI Frame Format */
#define SPI_CTRLR0_FRF_TI                               0x40     /* 0x1 Texas Instruments SSP Frame Format */
#define SPI_CTRLR0_FRF_MICROWIRE                        0x80     /* 0X2 National Semiconductors Microwire Frame Format */

/* Serial Clock Polarity SCPOL | Serial Clock Phase SCPH bit[9:8] */
#define SPI_CTRLR0_SC                                   8
#define SPI_CTRLR0_SC_MASK                              (0x3UL << SPI_CTRLR0_SC)
#define SPI_CTRLR0_SCPH_HIGH                            0x100U     /* 0x1 SPI SCPH high */
#define SPI_CTRLR0_SCPH_LOW	                         0x000U     /* 0x0 SPI SCPH low */
#define SPI_CTRLR0_SCPOL_HIGH                           0x200U     /* 0x2 SPI SCPOL high */
#define SPI_CTRLR0_SCPOL_LOW                            0x000U     /* 0x0 SPI SCPOL low */

/* Transfer Mode TMOD bit[11:10] */
#define SPI_CTRLR0_TMOD                                 10
#define SPI_CTRLR0_TMOD_MASK                            (0x3UL << SPI_CTRLR0_TMOD)
#define SPI_CTRLR0_TMOD_TRANSFER                        0x000     /* 0x0 SPI transfer mode */
#define SPI_CTRLR0_TMOD_SEND_ONLY                       0x400     /* 0x1 SPI send only mode */
#define SPI_CTRLR0_TMOD_RECEIVE_ONLY                    0x800     /* 0x2 SPI receive only mode */
#define SPI_CTRLR0_TMOD_EEPROM_READ_ONLY                0xC00     /* 0x3 SPI EEPROM read only mode */

/* Useful when SPI Controller acting as a Slave
 * When configured as a serial master, this
 * bit-field has no functionality.
 */
/*Slave Output Enable SLV_OE bit[12] */
#define SPI_CTRLR0_SLV_OE                               12
#define SPI_CTRLR0_SLV_OE_MASK                          (0x1UL << SPI_CTRLR0_SLV_OE)
#define SPI_CTRLR0_SLV_OE_ENABLE                        0x1000     /* 0x1 SPI slave output enable */
#define SPI_CTRLR0_SLV_OE_DISABLE                       0x0000     /* 0x0 SPI slave output disable */

/* Slave Select toggle Enable bit[14]
 * While operating in SPI mode with clock phase (SCPH) set to
 * 0, this register controls the behavior of the slave select line
 *  between data frames.
 */
/* Slave Select toggle Enable SSTE bit[14] */
#define SPI_CTRLR0_SSTE                                 14
#define SPI_CTRLR0_SSTE_MASK                            (1<< SPI_CTRLR0_SSTE)
#define SPI_CTRLR0_SSTE_ENABLE                          0x4000     /* 0x1 SPI slave select toggle enable */
#define SPI_CTRLR0_SSTE_DISABLE                         0x0000     /* 0x0 SPI slave select toggle disable */

/* Control Frame Size for the Microwire frame format CFS bit[19:16]*/
#define SPI_CTRLR0_CFS                                  16
#define SPI_CTRLR0_CFS_MASK                             (0xF << SPI_CTRLR0_CFS)

/* SPI Frame Format SPI_FRF bit[23:22]*/
#define SPI_CTRLR0_SPI_FRF                              22U
#define SPI_CTRLR0_SPI_FRF_MASK                         (0xCU << SPI_CTRLR0_SPI_FRF)
#define SPI_CTRLR0_SPI_FRF_STANDRAD                     0x000000    /* 0x0 Standard SPI Format */
#define SPI_CTRLR0_SPI_FRF_DUAL                         0x400000    /* 0x1 Dual SPI Format */
#define SPI_CTRLR0_SPI_FRF_QUAD                         0x800000    /* 0X2 Quad SPI Format */
#define SPI_CTRLR0_SPI_FRF_OCTAL                        0xC00000    /* 0X2 Octal SPI Format */

/* SPI Hyperbus Frame format enable SPI_HYPERBUS_EN bit[24] */
#define SPI_CTRLR0_SPI_HYPERBUS_EN                      24
#define SPI_CTRLR0_SPI_HYPERBUS_EN_SSTE_MASK            (1 << SPI_CTRLR0_SPI_HYPERBUS_EN)
#define SPI_CTRLR0_SPI_HYPERBUS_ENABLE                  0x4000      /* 0x1 SPI Hyperbus Frame format enable */
#define SPI_CTRLR0_SPI_HYPERBUS_DISABLE                 0x0000      /* 0x0 SPI Hyperbus Frame format disable */

/* SPI is working in Master or Slave SSI_IS_MST bit[31] */
#define SPI_CTRLR0_SSI_IS_MST                           31
#define SPI_CTRLR0_SSI_IS_MST_MASK                      (0x1UL << SPI_CTRLR0_SSI_IS_MST)
#define SPI_CTRLR0_SSI_IS_MST_MASTER                    0x80000000  /* 0x1 SPI master */
#define SPI_CTRLR0_SSI_IS_MST_SLAVE                     0x00000000  /* 0x0 SPI slave  */

/* Quad SPI MODE Macros */
#define SPI_CTRLR0_SPI_QUAD_ENABLE                     (0x2UL << SPI_CTRLR0_SPI_FRF)
#define SPI_CTRLR0_SPI_QUAD_TX_MODE                     SPI_TMOD_SEND_ONLY

#define SPI_CTRLR0_SPI_QUAD_RX_MODE                     SPI_TMOD_RECEIVE_ONLY

/* Octal SPI MODE Macros */
#define SPI_CTRLR0_SPI_OCTAL_ENABLE                     (0x3UL << SPI_CTRLR0_SPI_FRF)
#define SPI_CTRLR0_SPI_OCTAL_TX_RX_MODE                 SPI_TMOD_TRANSFER
#define SPI_CTRLR0_SPI_OCTAL_TX_MODE                    SPI_TMOD_SEND_ONLY
#define SPI_CTRLR0_SPI_OCTAL_RX_MODE                    SPI_TMOD_RECEIVE_ONLY

#define SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK            0x00000001U  /* Transmit fifo empty interrupt mask*/
#define SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK        0x00000002U  /* Transmit fifo overflow interrupt mask*/
#define SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK       0x00000004U  /* Receive fifo underflow interrupt mask*/
#define SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK        0x00000008U  /* Receive fifo Overflow interrupt mask*/
#define SPI_IMR_RX_FIFO_FULL_INTERRUPT_MASK             0x00000010U  /* Receive fifo full interrupt mask*/
#define SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK  0x00000020U  /* Multi-Master contention interrupt mask.*/

/****** SPI events *****/
#define SPI_TX_FIFO_EMPTY_EVENT                         (0x01)      /* Transmit fifo empty interrupt mask*/
#define SPI_TX_FIFO_OVER_FLOW_EVENT                     (0x02)      /* Transmit fifo overflow interrupt mask*/
#define SPI_RX_FIFO_UNDER_FLOW_EVENT                    (0x04)      /* Receive fifo underflow interrupt mask*/
#define SPI_RX_FIFO_OVER_FLOW_EVENT                     (0x08)      /* Receive fifo Overflow interrupt mask*/
#define SPI_RX_FIFO_FULL_EVENT                          (0x10)      /* Receive fifo full interrupt mask*/
#define SPI_MULTI_MASTER_CONTENTION_EVENT               (0x20)      /* Multi-Master contention interrupt mask.*/

/* SPI Register Structure */
typedef struct
{
    volatile uint32_t ctrlr0;           /* SPI Control Register 0  (0x0) */
    volatile uint32_t ctrlr1;           /* SPI Control Register 1  (0x4) */
    volatile uint32_t ssienr;           /* SPI Enable Register  (0x8) */
    volatile uint32_t mwcr;             /* SPI Microwire Control Register  (0xC) */
    volatile uint32_t ser;              /* SPI Slave Enable Register  (0x10) */
    volatile uint32_t baudr;            /* SPI Baud Rate Select Register  (0x14) */
    volatile uint32_t txftlr;           /* SPI Transmit FIFO Threshold Level Register (0x18) */
    volatile uint32_t rxftlr;           /* SPI Receive  FIFO Threshold Level Register (0x1C) */
    volatile uint32_t txflr;            /* SPI Transmit FIFO Level Register  (0x20)*/
    volatile uint32_t rxflr;            /* SPI Receive  FIFO Level Register  (0x24)*/
    volatile uint32_t sr;               /* SPI Status   Register  (0x28) */
    volatile uint32_t imr;              /* SPI Interrupt Mask Register  (0x2C) */
    volatile uint32_t isr;              /* SPI Interrupt Status Register  (0x30) */
    volatile uint32_t risr;             /* SPI Raw Interrupt Status Register (0x34)*/
    volatile uint32_t txoicr;           /* SPI Transmit FIFO Overflow Interrupt Clear Register  (0x38) */
    volatile uint32_t rxoicr;           /* SPI Receive  FIFO Overflow Interrupt Clear Register  (0x3C) */
    volatile uint32_t rxuicr;           /* SPI Receive FIFO Underflow Interrupt Clear Register  (0x40) */
    volatile uint32_t msticr;           /* SPI Multi-Master Interrupt Clear Register (0x44) */
    volatile uint32_t icr;              /* SPI Interrupt Clear Register  (0x48) */
    volatile uint32_t dmacr;            /* DMA Control Register  (0x4C) */
    volatile uint32_t dmatxdlr;         /* DMA Transmit Data Level  (0x50) */
    volatile uint32_t dmarxdlr;         /* DMA Receive Data Level  (0x54) */
    volatile uint32_t spi_idr;          /* SPI Identification Register  (0x58) */
    volatile uint32_t spi_ver_id;       /* Synopsys component version (0x5C) */
    volatile uint32_t data_reg;         /* SPI DATA Register for both Read and Write  (0x60) */
    volatile uint32_t drs[35];          /* SPI DATA Register for both Read and Write  (0x64-0xEC) */
    volatile uint32_t rx_sample_dly;    /* Rx Sample Delay Register (0xF0) */
    volatile uint32_t spi_ctrlr0;       /* SPI Control Register (0xF4) */
    volatile uint32_t txd_drive_edge;   /* Transmit Drive Edge Register (0xF8) */
    volatile uint32_t xip_mode_bits;    /* eXecute in Place - Mode bits (0xFC) */
} SPI_RegInfo;

typedef enum _SPI_INSTANCE
{
    SPI_INSTANCE_0,                         /* SPI instance - 0 */
    SPI_INSTANCE_1,                         /* SPI instance - 1 */
    SPI_INSTANCE_2,                         /* SPI instance - 2 */
    SPI_INSTANCE_3                          /* SPI instance - 3 */
} SPI_INSTANCE;

typedef enum _SPI_SPI_FRAME_FORMAT
{
    SPI_SPI_FRAME_FORMAT_STANDARD,          /* Standard SPI frame format */
    SPI_SPI_FRAME_FORMAT_DUAL,              /* Dual SPI frame format */
    SPI_SPI_FRAME_FORMAT_QUAD,              /* Quad SPI frame format */
    SPI_SPI_FRAME_FORMAT_OCTAL              /* Octal SPI frame format */
} SPI_SPI_FRAME_FORMAT;

typedef enum _SPI_MODE
{
    SPI_MODE_0,                             /* SPI Mode - 0 : CPHA = 0, CPOL = 0 */
    SPI_MODE_1,                             /* SPI Mode - 1 : CPHA = 1, CPOL = 0 */
    SPI_MODE_2,                             /* SPI Mode - 2 : CPHA = 0, CPOL = 1 */
    SPI_MODE_3                              /* SPI Mode - 3 : CPHA = 1, CPOL = 1 */
} SPI_MODE;

typedef enum _SPI_TMODE
{
    SPI_TMODE_TX_AND_RX     = 0x00,         /* SPI mode to Tx and Rx */
    SPI_TMODE_TX            = 0x01,         /* SPI mode to Tx  */
    SPI_TMODE_RX            = 0x02,         /* SPI mode to Rx */
    SPI_TMODE_EPROM_READ    = 0x03          /* SPI mode to read EPROM */
} SPI_TMODE;

typedef volatile struct _ARM_SPI_FLAG {
    uint32_t initialized: 1;                /* SPI driver initialized status */
    uint32_t powered    : 1;                /* SPI driver powered status */
    uint32_t reserved   : 30;               /* Reserved */
} ARM_SPI_FLAG;

typedef struct _SPI_RESOURCES
{
    SPI_RegInfo                 *reg_base;          /* Pointer to reg_base */
    ARM_SPI_SignalEvent_t       cb_event;           /* Pointer to call back function */
    uint32_t                    irq_priority;       /* Interrupt priority */
    SPI_INSTANCE                drv_instance;       /* Driver instance */
    SPI_SPI_FRAME_FORMAT        spi_frf;            /* SPI frame format - Standard/Dual/Quad/Octal */
    ARM_SPI_STATUS              status;             /* SPI driver status */
    ARM_SPI_FLAG                flag;               /* SPI driver status */
    uint32_t                    sw_slave_select;    /* operate Slave select pin through SW */
    uint32_t                    chip_selection_pin; /* chip selection pin from 0-3 */
    uint32_t                    tx_current_cnt;     /* Current Transfer count */
    uint32_t                    rx_current_cnt;     /* Current Transfer count */
    uint32_t                    total_cnt;          /* Total count to transfer */
    uint8_t                     *tx_buff;           /* Pointer to TX buffer */
    void                        *rx_buff;           /* Pointer to Rx buffer */
    uint32_t                    tx_default_buff;    /* Default value to Transfer */
    uint32_t                    tx_default_enable;  /* Default value enable to transfer */
    uint8_t                     rx_fifo_threshold;  /* Rx fifo threshold */
    uint8_t                     tx_fifo_threshold;  /* Tx fifo threshold */
    uint8_t                     tx_fifo_start_level;/* Tx fifo level to start communication */
} SPI_RESOURCES;

int32_t         SPI_ll_CoreClock            (SPI_RESOURCES *SPI, bool option);
void            SPI_ll_Disable              (SPI_RESOURCES *SPI);
void            SPI_ll_Enable               (SPI_RESOURCES *SPI);
int32_t         SPI_ll_SetMasterMode        (SPI_RESOURCES *SPI);
int32_t         SPI_ll_SetSlaveMode         (SPI_RESOURCES *SPI);
int32_t         SPI_ll_SetSpiMode           (SPI_RESOURCES *SPI, SPI_MODE mode);
int32_t         SPI_ll_SetBusSpeed          (SPI_RESOURCES *SPI, uint32_t arg);
uint32_t        SPI_ll_GetBusSpeed          (SPI_RESOURCES *SPI);
int32_t         SPI_ll_SetTiMode            (SPI_RESOURCES *SPI);
int32_t         SPI_ll_SetMicroWireMode     (SPI_RESOURCES *SPI);
int32_t         SPI_ll_SetDataFrameSize     (SPI_RESOURCES *SPI, uint8_t size);
void            SPI_ll_SetTransmissionMode  (SPI_RESOURCES *SPI, uint32_t mode);
void            SPI_ll_SetSpiFrameFormat    (SPI_RESOURCES *SPI);
int32_t         SPI_ll_Control_SlaveSelect  (SPI_RESOURCES *SPI, uint32_t device, uint32_t ss_state);
int32_t         SPI_ll_Irq_Enable           (SPI_RESOURCES *SPI);
int32_t         SPI_ll_Irq_Disable          (SPI_RESOURCES *SPI);
void            SPI_ll_IRQHandler           (SPI_RESOURCES *SPI);
int32_t         SPI_ll_MaskAllInterrupt     (SPI_RESOURCES *SPI);
int32_t         SPI_ll_Uninitialize         (SPI_RESOURCES *SPI);
int32_t         SPI_ll_Send                 (SPI_RESOURCES *SPI, const void *data, uint32_t num);
int32_t         SPI_ll_Receive              (SPI_RESOURCES *SPI, void *data, uint32_t num);
int32_t         SPI_ll_Transfer             (SPI_RESOURCES *SPI, const void *data_out, void *data_in, uint32_t num);
#endif /* __SPI_LL_DRV_H__ */
