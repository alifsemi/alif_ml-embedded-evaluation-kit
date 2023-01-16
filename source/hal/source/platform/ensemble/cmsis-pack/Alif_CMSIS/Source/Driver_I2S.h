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
 * @file     Driver_I2S.h
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for I2S.
 * @bug      None.
 * @Note     None
 ******************************************************************************/
#ifndef __DRIVER_I2S_H
#define __DRIVER_I2S_H
/* Includes ------------------------------------------------------------------*/
#include <Driver_SAI.h>
#include <DMA_Common.h>
#include "RTE_Device.h"
#include "RTE_Components.h"
#include CMSIS_device_header
#include <stdbool.h>

#ifdef  __cplusplus
extern "C"
{
#endif

/*!< Number of bytes for 16/32bit resolution*/
#define I2S_16BIT_BUF_TYPE       2
#define I2S_32BIT_BUF_TYPE       4

/*!< Max Audio Resolution for Tx/Rx Channel */
#define I2S_RX_WORDSIZE          32
#define I2S_TX_WORDSIZE          32

/*!< FIFO Depth for Tx & Rx  */
#define I2S_FIFO_DEPTH           16

/* I2S Clock Address */
#define I2S0_CLK_ADDR            (CFGMST0_BASE + 0x10)
#define I2S1_CLK_ADDR            (CFGMST0_BASE + 0x14)
#define I2S2_CLK_ADDR            (CFGMST0_BASE + 0x18)
#define I2S3_CLK_ADDR            (CFGMST0_BASE + 0x1C)


/*!< Clock divisor max/min value  */
#define I2S_CLK_DIVISOR_MAX      0x3FF
#define I2S_CLK_DIVISOR_MIN      2

/*!< I2S Input Clock Source */
#define I2S_CLK_38P4MHZ          38400000.0
#define I2S_CLK_160MHZ           160000000.0

/* Register fields and masks */

/* I2S CLKREG.DIV: Clock Divider */
#define I2S_CLKREG_DIVISOR_Pos   0U
#define I2S_CLKREG_DIVISOR_Msk   (0x3FFUL << I2S_CLKREG_DIVISOR_Pos)

/* I2S CLKREG.CLKSRC: Clock Source */
#define I2S_CLKREG_CLKSOURCE_Pos 16U
#define I2S_CLKREG_CLKSOURCE_Msk (0x1UL << I2S_CLKREG_CLKSOURCE_Pos)


/* I2S IER.IEN: Global Enable */
#define I2S_IER_IEN_Pos          0U
#define I2S_IER_IEN_Msk          (0x1UL << I2S_IER_IEN_Pos)

/* I2S IRER.RXEN: RX block Enable */
#define I2S_IRER_RXEN_Pos        0U
#define I2S_IRER_RXEN_Msk        (0x1UL << I2S_IRER_RXEN_Pos)

/* I2S ITER.TXEN: TX block Enable */
#define I2S_ITER_TXEN_Pos        0U
#define I2S_ITER_TXEN_Msk        (0x1UL << I2S_ITER_TXEN_Pos)

/* I2S CER.CLKEN: Clock Enable */
#define I2S_CER_CLKEN_Pos        0U
#define I2S_CER_CLKEN_Msk        (0x1UL << I2S_CER_CLKEN_Pos)

/* I2S CCR.SCLKG: Clock Gating */
#define I2S_CCR_SCLKG_Pos        0U
#define I2S_CCR_SCLKG_Msk        (0x7UL << I2S_CCR_SCLKG_Pos)

/* I2S CCR.WSS: Word Select length */
#define I2S_CCR_WSS_Pos          3U
#define I2S_CCR_WSS_Msk          (0x3UL << I2S_CCR_WSS_Pos)

/* I2S RXFFR.RXFFR: Rx Block FIFO Reset */
#define I2S_RXFFR_RXFFR_Pos      0U
#define I2S_RXFFR_RXFFR_Msk      (0x1UL << I2S_RXFFR_RXFFR_Pos)

/* I2S TXFFR.TXFFR: Tx Block FIFO Reset */
#define I2S_TXFFR_TXFFR_Pos      0U
#define I2S_TXFFR_TXFFR_Msk      (0x1UL << I2S_TXFFR_TXFFR_Pos)

/* I2S RER.RXCHEN: Rx Channel Enable */
#define I2S_RER_RXCHEN_Pos       0U
#define I2S_RER_RXCHEN_Msk       (0x1UL << I2S_RER_RXCHEN_Pos)

/* I2S TER.TXCHEN: Tx Channel Enable */
#define I2S_TER_TXCHEN_Pos       0U
#define I2S_TER_TXCHEN_Msk       (0x1UL << I2S_TER_TXCHEN_Pos)

/* I2S RCR.WLEN: Data Resolution of Rx */
#define I2S_RCR_WLEN_Pos         0U
#define I2S_RCR_WLEN_Msk         (0x7UL << I2S_RCR_WLEN_Pos)

/* I2S TCR.WLEN: Data Resolution of Tx */
#define I2S_TCR_WLEN_Pos         0U
#define I2S_TCR_WLEN_Msk         (0x7UL << I2S_TCR_WLEN_Pos)

/* I2S ISR.RXDA: Status of Rx Data Available interrupt */
#define I2S_ISR_RXDA_Pos         0U
#define I2S_ISR_RXDA_Msk         (0x1UL << I2S_ISR_RXDA_Pos)

/* I2S ISR.RXFO: Status of Data Overrun interrupt of Rx */
#define I2S_ISR_RXFO_Pos         1U
#define I2S_ISR_RXFO_Msk         (0x1UL << I2S_ISR_RXFO_Pos)

/* I2S ISR.TXFE: Status of Tx Empty Trigger Interrupt */
#define I2S_ISR_TXFE_Pos         4U
#define I2S_ISR_TXFE_Msk         (0x1UL << I2S_ISR_TXFE_Pos)

/* I2S ISR.TXFO: Status of Data Overrun Interrupt for Tx */
#define I2S_ISR_TXFO_Pos         5U
#define I2S_ISR_TXFO_Msk         (0x1UL << I2S_ISR_TXFO_Pos)

/* I2S IMR.RXDAM: Mask Rx Data Available interrupt */
#define I2S_IMR_RXDAM_Pos        0U
#define I2S_IMR_RXDAM_Msk        (0x1UL << I2S_IMR_RXDAM_Pos)

/* I2S IMR.RXFOM: Mask Data Overrun interrupt of Rx */
#define I2S_IMR_RXFOM_Pos        1U
#define I2S_IMR_RXFOM_Msk        (0x1UL << I2S_IMR_RXFOM_Pos)

/* I2S IMR.TXFEM: Mask Tx Empty Interrupt */
#define I2S_IMR_TXFEM_Pos        4U
#define I2S_IMR_TXFEM_Msk        (0x1UL << I2S_IMR_TXFEM_Pos)

/* I2S IMR.TXFOM: Mask Data Overrun Interrupt for Tx */
#define I2S_IMR_TXFOM_Pos        5U
#define I2S_IMR_TXFOM_Msk        (0x1UL << I2S_IMR_TXFOM_Pos)

/* I2S ROR.RXCHO: Clear Rx Data Overrun interrupt */
#define I2S_ROR_RXCHO_Pos        0U
#define I2S_ROR_RXCHO_Msk        (0x1UL << I2S_ROR_RXCHO_Pos)

/* I2S TOR.TXCHO: Clear Tx Data Overrun interrupt */
#define I2S_TOR_TXCHO_Pos        0U
#define I2S_TOR_TXCHO_Msk        (0x1UL << I2S_TOR_TXCHO_Pos)

/* I2S RFCR.RXCHDT: Rx FIFO Trigger Level */
#define I2S_RFCR_RXCHDT_Pos      0U
#define I2S_RFCR_RXCHDT_Msk      (0xFUL << I2S_RFCR_RXCHDT_Pos)

/* I2S TFCR.TXCHET: Tx FIFO Trigger Level */
#define I2S_TFCR_TXCHET_Pos      0U
#define I2S_TFCR_TXCHET_Msk      (0xFUL << I2S_TFCR_TXCHET_Pos)

/* I2S RFF.RXCHFR: Rx Channel FIFO Reset */
#define I2S_RFF_RXCHFR_Pos       0U
#define I2S_RFF_RXCHFR_Msk       (0x1UL << I2S_RFF_RXCHFR_Pos)

/* I2S TFF.TXCHFR: Tx Channel FIFO Reset */
#define I2S_TFF_TXCHFR_Pos       0U
#define I2S_TFF_TXCHFR_Msk       (0x1UL << I2S_TFF_TXCHFR_Pos)

/* I2S DMACR.DMAEN_RXBLOCK: DMA Enable for Rx Block */
#define I2S_DMACR_DMAEN_RXBLOCK_Pos       16U
#define I2S_DMACR_DMAEN_RXBLOCK_Msk       (0x1UL << I2S_DMACR_DMAEN_RXBLOCK_Pos)

/* I2S DMACR.DMAEN_TXBLOCK: DMA Enable for Tx Block */
#define I2S_DMACR_DMAEN_TXBLOCK_Pos       17U
#define I2S_DMACR_DMAEN_TXBLOCK_Msk       (0x1UL << I2S_DMACR_DMAEN_TXBLOCK_Pos)

/* Enums */
typedef enum {
    WSS_CLOCK_CYCLES_16 = 0,  /*!< 16 sclk cycles */
    WSS_CLOCK_CYCLES_24,      /*!< 24 sclk cycles */
    WSS_CLOCK_CYCLES_32,      /*!< 32 sclk cycles */

    WSS_CLOCK_CYCLES_MAX
} I2S_WSS_Type;

typedef enum {
    NO_CLOCK_GATING = 0,      /*!< Gating is disabled */
    SCLKG_CLOCK_CYCLES_12,    /*!< Gating after 12 sclk cycles */
    SCLKG_CLOCK_CYCLES_16,    /*!< Gating after 16 sclk cycles */
    SCLKG_CLOCK_CYCLES_20,    /*!< Gating after 20 sclk cycles */
    SCLKG_CLOCK_CYCLES_24,    /*!< Gating after 24 sclk cycles */

    SCLKG_CLOCK_CYCLES_MAX
} I2S_SCLKG_Type;

typedef enum {
    I2S_FLAG_CLKSRC_ENABLED   = (1U << 0), /*!< I2S ClockSource Status */
    I2S_FLAG_DRV_INIT_DONE    = (1U << 1), /*!< I2S Driver is Initialized */
    I2S_FLAG_DRV_CONFIG_DONE  = (1U << 2), /*!< I2S Driver is Configured */
    I2S_FLAG_DRV_MONO_MODE    = (1U << 3), /*!< I2S Driver in Mono Mode */
} I2S_FLAG_Type;

typedef enum {
    IGNORE_WLEN = 0,          /*!< Word length ignored */
    RES_12_BIT,               /*!< 12 bit Data Resolution */
    RES_16_BIT,               /*!< 16 bit Data Resolution */
    RES_20_BIT,               /*!< 20 bit Data Resolution */
    RES_24_BIT,               /*!< 24 bit Data Resolution */
    RES_32_BIT,               /*!< 32 bit Data Resolution */
} I2S_WLEN_Type;

typedef enum {
    I2S_TRANSMITTER,
    I2S_RECEIVER,
} I2S_TRANSFER_Type;

typedef enum {
    I2S_CLK_SOURCE_0,         /*!< I2S 38.4Mhz Clock Source */
    I2S_CLK_SOURCE_1,         /*!< I2S 160Mhz Clock Source */

    I2S_CLK_SOURCE_MAX
} I2S_CLK_SOURCE_Type;

typedef enum {
    I2S_FIFO_TRIGGER_LEVEL_1 = 0,         /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_2,             /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_3,             /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_4,             /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_5,             /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_6,             /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_7,             /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_8,             /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_9,             /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_10,            /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_11,            /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_12,            /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_13,            /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_14,            /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_15,            /*!< I2S FIFO Trigger level 1 */
    I2S_FIFO_TRIGGER_LEVEL_16,            /*!< I2S FIFO Trigger level 1 */

    I2S_FIFO_TRIGGER_LEVEL_MAX
} I2S_FIFO_TRIGGER_LEVEL_Type;

typedef struct
{
    __IOM uint32_t IER;       /*!< Offset:0x0 I2S Global Enable */
    __IOM uint32_t IRER;      /*!< Offset:0x4 I2S Rx Block Enable */
    __IOM uint32_t ITER;      /*!< Offset:0x8 I2S Tx Block Enable */
    __IOM uint32_t CER;       /*!< Offset:0xC I2S Clock Enable */
    __IOM uint32_t CCR;       /*!< Offset:0x10 I2S Clock Configuration */
    __OM  uint32_t RXFFR;     /*!< Offset:0x14 I2S  Rx Block FIFO Reset */
    __OM  uint32_t TXFFR;     /*!< Offset:0x18 I2S  Tx Block FIFO Reset */
    uint32_t RESERVED0;
    union {
      __IM uint32_t LRBR;     /*!< Offset:0x20, I2S Left Rx Buffer */
      __OM uint32_t LTHR;     /*!< Offset:0x20, I2S Left Tx Holding */
    };
    union {
      __IM uint32_t RRBR;     /*!< Offset:0x24, I2S Right Rx Buffer */
      __OM uint32_t RTHR;     /*!< Offset:0x24, I2S Right Tx Holding */
    };
    __IOM uint32_t RER;       /*!< Offset:0x28, I2S Rx Channel Enable */
    __IOM uint32_t TER;       /*!< Offset:0x2C, I2S Tx Channel Enable */
    __IOM uint32_t RCR;       /*!< Offset:0x30, I2S Rx Configuration */
    __IOM uint32_t TCR;       /*!< Offset:0x34, I2S Tx Configuration */
    __IM  uint32_t ISR;       /*!< Offset:0x38, I2S Interrupt Status */
    __IOM uint32_t IMR;       /*!< Offset:0x3C, I2S Interrupt Mask */
    __IM  uint32_t ROR;       /*!< Offset:0x40, I2S Rx Overrun */
    __IM  uint32_t TOR;       /*!< Offset:0x44, I2S Tx Overrun */
    __IOM uint32_t RFCR;      /*!< Offset:0x48, I2S Rx FIFO Configuration */
    __IOM uint32_t TFCR;      /*!< Offset:0x4C, I2S Tx FIFO Configuration */
    __OM  uint32_t RFF;       /*!< Offset:0x50, I2S Rx Channel FIFO Reset */
    __OM  uint32_t TFF;       /*!< Offset:0x54, I2S Tx Channel FIFO Reset */
    uint32_t RESERVED1[90];
    __IM  uint32_t RXDMA;     /*!< Offset:0x1C0, I2S Rx Block DMA */
    uint32_t RESERVED2;
    __OM  uint32_t TXDMA;     /*!< Offset:0x1C8, I2S Tx Block DMA */
    uint32_t RESERVED4[13];
    __IOM uint32_t DMACR;     /*!< Offset:0x200, I2S DMA Control */
} I2S_TypeDef;


typedef union _I2S_DRV_STATUS {
    uint32_t status;
    ARM_SAI_STATUS status_b;
} I2S_DRV_STATUS;

typedef struct _I2S_TX_BUFF {

    /*!< I2S tx buffer: can be 16/32 bit based on databits */
    const void *buf;

    /*!< I2S tx total buffer length in bytes */
    uint32_t total_len;

    /*!< I2S tx points to the current offset in the buffer in bytes */
    uint32_t ofs;
} I2S_TX_BUFF;

typedef struct _I2S_RX_BUFF {

    /*!< I2S rx buffer: can be 16/32 bit based on databits */
    void *buf;

    /*!< I2S rx total buffer length in bytes */
    uint32_t total_len;

    /*!< I2S rx points to the current offset in the buffer in bytes */
    uint32_t ofs;
} I2S_RX_BUFF;

typedef struct _I2S_CONFIG_INFO {

    /*!< I2S Word Select Size */
    const I2S_WSS_Type wss_len;

    /*!< I2S SCLK Gating */
    const I2S_SCLKG_Type sclkg;

    /*!< I2S word length */
    I2S_WLEN_Type wlen;

    /*!< I2S clock source */
    const I2S_CLK_SOURCE_Type clk_source;

    /*!< I2S Rx FIFO Trigger Level */
    const I2S_FIFO_TRIGGER_LEVEL_Type rx_fifo_trg_lvl;

    /*!< I2S Tx FIFO Trigger Level */
    const I2S_FIFO_TRIGGER_LEVEL_Type tx_fifo_trg_lvl;

    /*!< I2S dma enable */
    const bool dma_enable;

    /*!< DMA IRQ priority number */
    const uint32_t dma_irq_priority;

    /*!< I2S irq priority number */
    const uint32_t irq_priority;
} I2S_CONFIG_INFO;

typedef struct _I2S_DMA_HW_CONFIG {
    /*!< Tx interface */
    DMA_PERIPHERAL_CONFIG dma_tx;

    /*!< Rx interface */
    DMA_PERIPHERAL_CONFIG dma_rx;
} I2S_DMA_HW_CONFIG;

typedef struct _I2S_DRV_INFO {

    /*!< I2S Application Event Callback */
    ARM_SAI_SignalEvent_t cb_event;

    /*!< I2S DMA Callback */
    ARM_DMA_SignalEvent_t dma_cb;

    /*!< I2S ARM I2S Status */
    I2S_DRV_STATUS drv_status;

    /*!< I2S Audio Sample Rate */
    uint32_t sample_rate;

    /*!< I2S Controller configuration */
    I2S_CONFIG_INFO *cfg;

    /*!< DMA Controller configuration */
    I2S_DMA_HW_CONFIG *dma_cfg;

    /*!< I2S physical address */
    I2S_TypeDef *paddr;

    /*!< I2S Clock physical address */
    __IOM uint32_t *clkreg_paddr;

    /*!< I2S irq number */
    const IRQn_Type irq;

    /*!< I2S Driver Flags */
    uint8_t flags;

    /*!< I2S Interrupt Status */
    uint32_t int_status;

    /*!< I2S transmit buffer information */
    I2S_TX_BUFF tx_buff;

    /*!< I2S receive buffer information */
    I2S_RX_BUFF rx_buff;
} I2S_DRV_INFO;

#ifdef  __cplusplus
}
#endif

#endif //__DRIVER_I2S_H
