/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef DMA_MAPPING_H
#define DMA_MAPPING_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                         DMA0 peripheral map                              */
/******************************************************************************/
#define UART0_DMA 0
#define UART0_RX_PERIPH_REQ 0
#define UART0_TX_PERIPH_REQ 1

#define UART1_DMA 0
#define UART1_RX_PERIPH_REQ 2
#define UART1_TX_PERIPH_REQ 3

#define UART2_DMA 0
#define UART2_RX_PERIPH_REQ 4
#define UART2_TX_PERIPH_REQ 5

#define UART3_DMA 0
#define UART3_RX_PERIPH_REQ 6
#define UART3_TX_PERIPH_REQ 7

#define SPI0_DMA 0
#define SPI0_RX_PERIPH_REQ 8
#define SPI0_TX_PERIPH_REQ 9

#define SPI1_DMA 0
#define SPI1_RX_PERIPH_REQ 10
#define SPI1_TX_PERIPH_REQ 11

#define SPI2_DMA 0
#define SPI2_RX_PERIPH_REQ 12
#define SPI2_TX_PERIPH_REQ 13

#define SPI3_DMA 0
#define SPI3_RX_PERIPH_REQ 14
#define SPI3_TX_PERIPH_REQ 15

#define I3C0_DMA 0
#define I3C0_RX_PERIPH_REQ 22
#define I3C0_TX_PERIPH_REQ 23


#define I2S0_DMA 0
#define I2S0_RX_PERIPH_REQ 24
#define I2S0_TX_PERIPH_REQ 25

#define I2S1_DMA 0
#define I2S1_RX_PERIPH_REQ 26
#define I2S1_TX_PERIPH_REQ 27

#define I2S2_DMA 0
#define I2S2_RX_PERIPH_REQ 28
#define I2S2_TX_PERIPH_REQ 29

#define I2S3_DMA 0
#define I2S3_RX_PERIPH_REQ 30
#define I2S3_TX_PERIPH_REQ 31


/******************************************************************************/
/*                         DMA1 peripheral map                              */
/******************************************************************************/
#define UART4_DMA 1
#define UART4_RX_PERIPH_REQ 0
#define UART4_TX_PERIPH_REQ 1

#define UART5_DMA 1
#define UART5_RX_PERIPH_REQ 2
#define UART5_TX_PERIPH_REQ 3

#define UART6_DMA 1
#define UART6_RX_PERIPH_REQ 4
#define UART6_TX_PERIPH_REQ 5

#define UART7_DMA 1
#define UART7_RX_PERIPH_REQ 6
#define UART7_TX_PERIPH_REQ 7


#ifdef __cplusplus
}
#endif

#endif /* DMA_MAPPING_H */
