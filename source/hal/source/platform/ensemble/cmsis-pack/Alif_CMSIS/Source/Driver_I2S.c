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
 * @file     Driver_I2S.c
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     21-Aug-2020
 * @brief    CMSIS-Driver for I2S
 * @bug      None
 * @Note     None
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Driver_I2S.h"

#include "math.h"

#define ARM_SAI_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0) /*!< I2S Driver Version */

static const ARM_DRIVER_VERSION DriverVersion = {
		ARM_SAI_API_VERSION,
		ARM_SAI_DRV_VERSION
};


#if !(RTE_I2S0 || RTE_I2S1 || RTE_I2S2 || RTE_I2S3)
#error "I2S is not enabled in the RTE_Device.h"
#endif

#if !defined(RTE_Drivers_SAI)
#error "I2S is not enabled in RTE_Components.h!"
#endif

/* Driver Capabilities */
static const ARM_SAI_CAPABILITIES DriverCapabilities = {
    1, /* supports asynchronous Transmit/Receive */
    1, /* supports synchronous Transmit/Receive */
    0, /* supports user defined Protocol */
    1, /* supports I2S Protocol */
    0, /* supports MSB/LSB justified Protocol */
    0, /* supports PCM short/long frame Protocol */
    0, /* supports AC'97 Protocol */
    1, /* supports Mono mode */
    0, /* supports Companding */
    0, /* supports MCLK (Master Clock) pin */
    0, /* supports Frame error event: \ref ARM_SAI_EVENT_FRAME_ERROR */
    0  /* reserved (must be zero) */
};

//
//  Functions
//

/**
  \fn          ARM_DRIVER_VERSION ARM_SAI_GetVersion (void)
  \brief       Get I2S driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION I2S_GetVersion (void)
{
  return DriverVersion;
}

/**
  \fn          ARM_SAI_CAPABILITIES ARM_SAI_GetCapabilities (void)
  \brief       Get I2S driver capabilities
  \return      \ref ARM_SAI_CAPABILITIES
*/
static ARM_SAI_CAPABILITIES I2S_GetCapabilities (void)
{
  return DriverCapabilities;
}

/**
  \fn          int32_t I2S_Configure_ClockSource (bool enable, I2S_DRV_INFO *i2s)
  \brief       Control I2S Interface Power.
  \param[in]   enable  Clock state
  \param[in]   i2s     Pointer to I2S resources
  \return      \ref execution_status
*/
static int32_t I2S_Configure_ClockSource (bool enable, I2S_DRV_INFO *i2s)
{
	uint32_t div = 0;
	uint32_t sclk = 0;
	const uint32_t clock_cycles[WSS_CLOCK_CYCLES_MAX] = {16, 24, 32};


	if (enable) {
		if (!i2s->sample_rate)
			return ARM_DRIVER_ERROR_PARAMETER;

		/* Calculate sclk = 2* WSS * Sample Rate*/
		sclk = 2 * clock_cycles[i2s->cfg->wss_len] * (i2s->sample_rate);
		if (i2s->cfg->clk_source == I2S_CLK_SOURCE_0)
			div = round((float)I2S_CLK_38P4MHZ/(float)sclk);
		else if (i2s->cfg->clk_source == I2S_CLK_SOURCE_1)
			div = round((float)I2S_CLK_160MHZ/(float)sclk);
		else
			return ARM_DRIVER_ERROR_PARAMETER;

		if ((div > I2S_CLK_DIVISOR_MAX)|| (div < I2S_CLK_DIVISOR_MIN))
			return ARM_DRIVER_ERROR_PARAMETER;

		*(i2s->clkreg_paddr) &= ~I2S_CLKREG_DIVISOR_Msk;
		*(i2s->clkreg_paddr) |=  _VAL2FLD(I2S_CLKREG_DIVISOR, div);
	}

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_DMA_Initialize (DMA_PERIPHERAL_CONFIG *dma_periph)
  \brief       Initialize DMA for I2S
  \param[in]   dma_periph     Pointer to DMA resources
  \return      \ref execution_status
*/
__STATIC_INLINE int32_t I2S_DMA_Initialize (DMA_PERIPHERAL_CONFIG *dma_periph)
{
    int32_t        status;
    ARM_DRIVER_DMA *dma_drv = dma_periph->dma_drv;

    /* Initializes DMA interface */
    status = dma_drv->Initialize();
    if(status) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_DMA_Allocate (DMA_PERIPHERAL_CONFIG *dma_periph)
  \brief       Allocate a channel for I2S
  \param[in]   dma_periph     Pointer to DMA resources
  \return      \ref execution_status
*/
__STATIC_INLINE int32_t I2S_DMA_Allocate (DMA_PERIPHERAL_CONFIG *dma_periph)
{
    int32_t        status;
    ARM_DRIVER_DMA *dma_drv = dma_periph->dma_drv;

    /* Allocate handle for peripheral */
    status = dma_drv->Allocate(&dma_periph->dma_handle);
    if(status)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_DMA_DeAllocate (DMA_PERIPHERAL_CONFIG *dma_periph)
  \brief       De-allocate channel of I2S
  \param[in]   dma_periph     Pointer to DMA resources
  \return      \ref execution_status
*/
__STATIC_INLINE int32_t I2S_DMA_DeAllocate (DMA_PERIPHERAL_CONFIG *dma_periph)
{
    int32_t        status;
    ARM_DRIVER_DMA *dma_drv = dma_periph->dma_drv;

    /* De-Allocate handle  */
    status = dma_drv->DeAllocate(&dma_periph->dma_handle);
    if(status)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_DMA_EnableMono (DMA_PERIPHERAL_CONFIG *dma_periph)
  \brief       Enable I2S DMA Mono transfer
  \param[in]   dma_periph     Pointer to DMA resources
  \return      \ref execution_status
*/
__STATIC_INLINE int32_t I2S_DMA_EnableMono (DMA_PERIPHERAL_CONFIG *dma_periph)
{
    int32_t        status;
    ARM_DRIVER_DMA *dma_drv = dma_periph->dma_drv;

    /* Stop transfer */
    status = dma_drv->Control(&dma_periph->dma_handle, ARM_DMA_I2S_MONO_MODE, NULL);
    if(status)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_DMA_Start (DMA_PERIPHERAL_CONFIG *dma_periph,
                                      ARM_DMA_PARAMS *dma_params)
  \brief       Start I2S DMA transfer
  \param[in]   dma_periph     Pointer to DMA resources
  \param[in]   dma_params     Pointer to DMA parameters
  \return      \ref execution_status
*/
__STATIC_INLINE int32_t I2S_DMA_Start (DMA_PERIPHERAL_CONFIG *dma_periph,
                                       ARM_DMA_PARAMS *dma_params)
{
    int32_t        status;
    ARM_DRIVER_DMA *dma_drv = dma_periph->dma_drv;

    /* Start transfer */
    status = dma_drv->Start(&dma_periph->dma_handle, dma_params);
    if(status)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_DMA_Stop (DMA_PERIPHERAL_CONFIG *dma_periph)
  \brief       Stop I2S DMA transfer
  \param[in]   dma_periph     Pointer to DMA resources
  \return      \ref execution_status
*/
__STATIC_INLINE int32_t I2S_DMA_Stop (DMA_PERIPHERAL_CONFIG *dma_periph)
{
    int32_t        status;
    ARM_DRIVER_DMA *dma_drv = dma_periph->dma_drv;

    /* Stop transfer */
    status = dma_drv->Stop(&dma_periph->dma_handle);
    if(status)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

/**
  \fn          void I2S_GlobalEnable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Global Enable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_GlobalEnable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->IER = _VAL2FLD(I2S_IER_IEN, 1U);
}

/**
  \fn          void I2S_GlobalDisable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Global Disable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_GlobalDisable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->IER &= ~I2S_IER_IEN_Msk;
}

/**
  \fn          void I2S_RxBlockEnable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Receiver Block Enable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_RxBlockEnable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->IRER = _VAL2FLD(I2S_IRER_RXEN, 1U);
}

/**
  \fn          void I2S_RxBlockDisable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Receiver Block Disable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_RxBlockDisable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->IRER &= ~I2S_IRER_RXEN_Msk;
}

/**
  \fn          void I2S_TxBlockEnable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Transmitter Block Enable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_TxBlockEnable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->ITER = _VAL2FLD(I2S_ITER_TXEN, 1U);
}

/**
  \fn          void I2S_TxBlockDisable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Transmitter Block Disable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_TxBlockDisable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->ITER &= ~I2S_ITER_TXEN_Msk;
}

/**
  \fn          void I2S_ClockEnable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Clock Enable in Master Mode
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_ClockEnable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->CER = _VAL2FLD(I2S_CER_CLKEN, 1U);
}

/**
  \fn          void I2S_ClockDisable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Clock Disable in Master Mode
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_ClockDisable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->CER &= ~I2S_CER_CLKEN_Msk;
}

/**
  \fn          void I2S_ConfigureClock (I2S_DRV_INFO *i2s)
  \brief       Control I2S Configure WSS and SCLKG in Master Mode.
               Should be called with Clock disabled.
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_ConfigureClock (I2S_DRV_INFO *i2s)
{
    i2s->paddr->CCR = _VAL2FLD(I2S_CCR_SCLKG, i2s->cfg->sclkg) | \
                      _VAL2FLD(I2S_CCR_WSS, i2s->cfg->wss_len);
}

/**
  \fn          uint32_t I2S_ReadLeftRx (I2S_DRV_INFO *i2s)
  \brief       Read Left Receive Buffer Register
  \param[in]   i2s     Pointer to I2S resources
  \return      data
*/
__STATIC_INLINE uint32_t I2S_ReadLeftRx (I2S_DRV_INFO *i2s)
{
	return i2s->paddr->LRBR;
}


/**
  \fn          void I2S_WriteLeftTx (uint32_t data, I2S_DRV_INFO *i2s)
  \brief       Write to Left Transmit Holding Register
  \param[in]   data    data to write
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_WriteLeftTx (uint32_t data, I2S_DRV_INFO *i2s)
{
	i2s->paddr->LTHR = data;
}

/**
  \fn          uint32_t I2S_ReadRightRx (I2S_DRV_INFO *i2s)
  \brief       Read from Right Receive Buffer Register
  \param[in]   i2s     Pointer to I2S resources
  \return      data
*/
__STATIC_INLINE uint32_t I2S_ReadRightRx (I2S_DRV_INFO *i2s)
{
	return i2s->paddr->RRBR;
}

/**
  \fn          void I2S_WriteRightTx (uint32_t data, I2S_DRV_INFO *i2s)
  \brief       Write to Right Transmit Holding Register
  \param[in]   data    data to write
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_WriteRightTx (uint32_t data, I2S_DRV_INFO *i2s)
{
	i2s->paddr->RTHR = data;
}

/**
  \fn          void I2S_RxChannelEnable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Receiver Channel Enable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_RxChannelEnable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->RER = _VAL2FLD(I2S_RER_RXCHEN, 1U);
}

/**
  \fn          void I2S_RxChannelDisable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Receiver Channel Disable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_RxChannelDisable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->RER &= ~I2S_RER_RXCHEN_Msk;
}

/**
  \fn          void I2S_TxChannelEnable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Transmit Channel Enable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_TxChannelEnable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->TER = _VAL2FLD(I2S_TER_TXCHEN, 1U);
}

/**
  \fn          void I2S_TxChannelDisable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Transmit Channel Disable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_TxChannelDisable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->TER &= ~I2S_TER_TXCHEN_Msk;
}

/**
  \fn          void I2S_RxConfigWlen (I2S_DRV_INFO *i2s)
  \brief       Set Wlen in Receive Configuration Register
               Should be called with RXCHEN disabled.
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_RxConfigWlen (I2S_DRV_INFO *i2s)
{
	i2s->paddr->RCR = _VAL2FLD(I2S_RCR_WLEN, i2s->cfg->wlen);
}

/**
  \fn          void I2S_TxConfigWlen (I2S_DRV_INFO *i2s)
  \brief       Set Wlen in Transmit Configuration Register
               Should be called with TXCHEN disabled.
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_TxConfigWlen (I2S_DRV_INFO *i2s)
{
    i2s->paddr->TCR = _VAL2FLD(I2S_TCR_WLEN, i2s->cfg->wlen);
}

/**
  \fn          void I2S_GetInterruptStatus (I2S_DRV_INFO *i2s)
  \brief       Read the Interrupt Status Register for the channel
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_GetInterruptStatus (I2S_DRV_INFO *i2s)
{
	i2s->int_status = i2s->paddr->ISR;
}

/**
  \fn          void I2S_SetInterruptMask (uint32_t imr, I2S_DRV_INFO *i2s)
  \brief       Set the Interrupt Mask Register for the channel
  \param[in]   imr     Mask Value
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_SetInterruptMask (uint32_t imr, I2S_DRV_INFO *i2s)
{
	i2s->paddr->IMR = imr;
}

/**
  \fn          void I2S_GetInterruptMask (I2S_DRV_INFO *i2s)
  \brief       Get the Interrupt Mask Register for the channel
  \param[in]   i2s     Pointer to I2S resources
  \return      Interrupt Mask value
*/
__STATIC_INLINE uint32_t I2S_GetInterruptMask (I2S_DRV_INFO *i2s)
{
	return i2s->paddr->IMR;
}

/**
  \fn          void I2S_ClearRxOverrun (I2S_DRV_INFO *i2s)
  \brief       Clear Receiver FIFO Data Overrun Interrupt
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_ClearRxOverrun (I2S_DRV_INFO *i2s)
{
	(void) (i2s->paddr->ROR);
}

/**
  \fn          void I2S_ClearTxOverrun (I2S_DRV_INFO *i2s)
  \brief       Clear Transmit FIFO Data Overrun Interrupt
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_ClearTxOverrun (I2S_DRV_INFO *i2s)
{
	(void) (i2s->paddr->TOR);
}

/**
  \fn          void I2S_SetRxTriggerLevel (I2S_DRV_INFO *i2s)
  \brief       Program the Trigger Level in RxFIFO
               The channel must be disabled before doing this
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_SetRxTriggerLevel (I2S_DRV_INFO *i2s)
{
    i2s->paddr->RFCR = _VAL2FLD(I2S_RFCR_RXCHDT, i2s->cfg->rx_fifo_trg_lvl);
}

/**
  \fn          void I2S_SetTxTriggerLevel (I2S_DRV_INFO *i2s)
  \brief       Program the Trigger Level in TxFIFO
               The channel must be disabled before doing this
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_SetTxTriggerLevel (I2S_DRV_INFO *i2s)
{
    i2s->paddr->TFCR = _VAL2FLD(I2S_TFCR_TXCHET, i2s->cfg->tx_fifo_trg_lvl);
}

/**
  \fn          void I2S_RxFifoReset (I2S_DRV_INFO *i2s)
  \brief       Flush the Rx Channel FIFO
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_RxFifoReset (I2S_DRV_INFO *i2s)
{
    i2s->paddr->RFF = _VAL2FLD(I2S_RFF_RXCHFR, 1U);
}

/**
  \fn          void I2S_TxFifoReset (I2S_DRV_INFO *i2s)
  \brief       Flush the Tx Channel FIFO
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_TxFifoReset (I2S_DRV_INFO *i2s)
{
    i2s->paddr->TFF = _VAL2FLD(I2S_TFF_TXCHFR, 1U);
}

/**
  \fn          void I2S_RxDMAEnable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Rx DMA Block Enable
  \param[in]   i2s     Pointer to I2S resources
*/
__USED __STATIC_INLINE void I2S_RxDMAEnable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->DMACR |= _VAL2FLD(I2S_DMACR_DMAEN_RXBLOCK, 1U);
}

/**
  \fn          void I2S_RxDMADisable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Rx DMA Block Disable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_RxDMADisable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->DMACR &= ~I2S_DMACR_DMAEN_RXBLOCK_Msk;
}

/**
  \fn          void I2S_TxDMAEnable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Tx DMA Block Enable
  \param[in]   i2s     Pointer to I2S resources
*/
__USED __STATIC_INLINE void I2S_TxDMAEnable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->DMACR |= _VAL2FLD(I2S_DMACR_DMAEN_TXBLOCK, 1U);
}

/**
  \fn          void I2S_TxDMADisable (I2S_DRV_INFO *i2s)
  \brief       Control I2S Tx DMA Block Disable
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_TxDMADisable (I2S_DRV_INFO *i2s)
{
    i2s->paddr->DMACR &= ~I2S_DMACR_DMAEN_TXBLOCK_Msk;
}

/**
  \fn          void I2S_EnableTxInterrupt (I2S_DRV_INFO *i2s)
  \brief       Enable I2S Tx Interrupt
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_EnableTxInterrupt (I2S_DRV_INFO *i2s)
{
    uint32_t imr;

    imr = I2S_GetInterruptMask (i2s);
    imr &= ~(I2S_IMR_TXFEM_Msk | I2S_IMR_TXFOM_Msk);
    I2S_SetInterruptMask (imr, i2s);
}

/**
  \fn          void I2S_DisableTxInterrupt (I2S_DRV_INFO *i2s)
  \brief       Disable I2S Tx Interrupt
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_DisableTxInterrupt (I2S_DRV_INFO *i2s)
{
    uint32_t imr;

    imr = I2S_GetInterruptMask (i2s);
    imr |= _VAL2FLD(I2S_IMR_TXFEM, 1U) | _VAL2FLD(I2S_IMR_TXFOM, 1U);
    I2S_SetInterruptMask (imr, i2s);
}

/**
  \fn          void I2S_EnableRxFOInterrupt (I2S_DRV_INFO *i2s)
  \brief       Enable I2S Rx Overflow Interrupt
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_EnableRxFOInterrupt (I2S_DRV_INFO *i2s)
{
    uint32_t imr;

    imr = I2S_GetInterruptMask (i2s);
    imr &= ~(I2S_IMR_RXFOM_Msk);
    I2S_SetInterruptMask (imr, i2s);
}

/**
  \fn          void I2S_EnableRxInterrupt (I2S_DRV_INFO *i2s)
  \brief       Enable I2S Rx Interrupt
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_EnableRxInterrupt (I2S_DRV_INFO *i2s)
{
    uint32_t imr;

    imr = I2S_GetInterruptMask (i2s);
    imr &= ~(I2S_IMR_RXDAM_Msk | I2S_IMR_RXFOM_Msk);
    I2S_SetInterruptMask (imr, i2s);
}

/**
  \fn          void I2S_DisableRxFOInterrupt (I2S_DRV_INFO *i2s)
  \brief       Disable I2S Rx Overflow Interrupt
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_DisableRxFOInterrupt (I2S_DRV_INFO *i2s)
{
    uint32_t imr;

    imr = I2S_GetInterruptMask (i2s);
    imr |= _VAL2FLD(I2S_IMR_RXFOM, 1U);
    I2S_SetInterruptMask (imr, i2s);
}

/**
  \fn          void I2S_DisableRxInterrupt (I2S_DRV_INFO *i2s)
  \brief       Disable I2S Rx Interrupt
  \param[in]   i2s     Pointer to I2S resources
*/
__STATIC_INLINE void I2S_DisableRxInterrupt (I2S_DRV_INFO *i2s)
{
    uint32_t imr;

    imr = I2S_GetInterruptMask (i2s);
    imr |= _VAL2FLD(I2S_IMR_RXDAM, 1U) | _VAL2FLD(I2S_IMR_RXFOM, 1U);
    I2S_SetInterruptMask (imr, i2s);
}

/**
  \fn          int32_t I2S_PowerControl (ARM_POWER_STATE state, I2S_DRV_INFO *i2s)
  \brief       Control I2S Interface Power.
  \param[in]   state  Power state
  \param[in]   i2s       Pointer to I2S resources
  \return      \ref execution_status
*/
static int32_t I2S_PowerControl (ARM_POWER_STATE state, I2S_DRV_INFO *i2s)
{
	switch (state) {
	case ARM_POWER_OFF:
		/** Disable I2S IRQ */
		NVIC_DisableIRQ (i2s->irq);
		/* Disable the DMA */
		I2S_RxDMADisable (i2s);
		I2S_TxDMADisable (i2s);
		/* Disable the I2S Global Enable */
		I2S_RxBlockDisable (i2s);
		I2S_TxBlockDisable (i2s);
		I2S_GlobalDisable (i2s);
		/* Clear Any Pending IRQ*/
		NVIC_ClearPendingIRQ (i2s->irq);
		/* Reset the interrupt status and driver status */
		i2s->int_status = 0U;
		/* Mask all the interrupts */
		I2S_DisableTxInterrupt (i2s);
		I2S_DisableRxInterrupt (i2s);
		i2s->drv_status.status = 0U;
		/* Reset the power status of I2S */
		i2s->flags &= ~I2S_FLAG_CLKSRC_ENABLED;
		break;

	case ARM_POWER_FULL:

		if (!(i2s->flags & I2S_FLAG_DRV_INIT_DONE))
			return ARM_DRIVER_ERROR;
		if (i2s->flags & I2S_FLAG_CLKSRC_ENABLED)
			return ARM_DRIVER_OK;

		i2s->int_status   = 0U;
		i2s->drv_status.status = 0U;
		/* Mask all the interrupts */
		I2S_DisableTxInterrupt (i2s);
		I2S_DisableRxInterrupt (i2s);


		/* Enable I2S and IRQ*/
		I2S_GlobalEnable (i2s);
		NVIC_ClearPendingIRQ (i2s->irq);
		NVIC_SetPriority(i2s->irq, i2s->cfg->irq_priority);
		NVIC_EnableIRQ (i2s->irq);

		/* Set the power flag enabled */
		i2s->flags |= I2S_FLAG_CLKSRC_ENABLED;
		break;

	case ARM_POWER_LOW:
	default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;

	}
	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_Initialize (ARM_SAI_SignalEvent_t cb_event, I2S_DRV_INFO *i2s)
  \brief       Initialize I2S Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SAI_SignalEvent
  \param[in]   i2s       Pointer to I2S resources
  \return      \ref execution_status
*/
static int32_t I2S_Initialize (ARM_SAI_SignalEvent_t cb_event, I2S_DRV_INFO *i2s)
{
	int32_t ret = ARM_DRIVER_OK;
	__IOM uint32_t *clkreg_expmst0 = (uint32_t *) CFGMST0_BASE;

	if (i2s->flags & I2S_FLAG_DRV_INIT_DONE)
		return ARM_DRIVER_OK;

	if (!cb_event)
		return ARM_DRIVER_ERROR_PARAMETER;

	if(i2s->cfg->wss_len >= WSS_CLOCK_CYCLES_MAX)
		return ARM_DRIVER_ERROR_PARAMETER;

	if(i2s->cfg->sclkg >= SCLKG_CLOCK_CYCLES_MAX)
		return ARM_DRIVER_ERROR_PARAMETER;

	if(i2s->cfg->clk_source >= I2S_CLK_SOURCE_MAX)
		return ARM_DRIVER_ERROR_PARAMETER;

	if(i2s->cfg->rx_fifo_trg_lvl >= I2S_FIFO_TRIGGER_LEVEL_MAX)
		return ARM_DRIVER_ERROR_PARAMETER;

	if(i2s->cfg->tx_fifo_trg_lvl >= I2S_FIFO_TRIGGER_LEVEL_MAX)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* Initialize the driver elements*/
	i2s->cb_event = cb_event;
	i2s->drv_status.status = 0U;
	i2s->int_status   = 0U;

	i2s->tx_buff.buf =  NULL;
	i2s->tx_buff.ofs =  0;
	i2s->tx_buff.total_len =  0;

	i2s->rx_buff.buf =  NULL;
	i2s->rx_buff.ofs =  0;
	i2s->rx_buff.total_len =  0;

	if(i2s->cfg->dma_enable)
	{
		i2s->dma_cfg->dma_rx.dma_handle = -1;
		i2s->dma_cfg->dma_tx.dma_handle = -1;

		/* Initialize DMA for I2S-Tx */
		if(I2S_DMA_Initialize(&i2s->dma_cfg->dma_tx) != ARM_DRIVER_OK)
			return ARM_DRIVER_ERROR;

		/* Initialize DMA for I2S-Rx */
		if(I2S_DMA_Initialize(&i2s->dma_cfg->dma_rx) != ARM_DRIVER_OK)
			return ARM_DRIVER_ERROR;
	}


	i2s->flags = I2S_FLAG_DRV_INIT_DONE;

	/* Enable the clock Source */
	*clkreg_expmst0 |= (1 << 0) | (1 << 4);

	*(i2s->clkreg_paddr) &= ~I2S_CLKREG_DIVISOR_Msk;
	*(i2s->clkreg_paddr) |=  _VAL2FLD(I2S_CLKREG_DIVISOR, 0x3FF);
	if (i2s->cfg->clk_source == I2S_CLK_SOURCE_0)
		*(i2s->clkreg_paddr) &= ~I2S_CLKREG_CLKSOURCE_Msk;
	else
		*(i2s->clkreg_paddr) |=  _VAL2FLD(I2S_CLKREG_CLKSOURCE, I2S_CLK_SOURCE_1);

	return ret;
}

/**
  \fn          int32_t I2S_Uninitialize (I2S_DRV_INFO *i2s)
  \brief       De-initialize I2S Interface.
  \param[in]   i2s       Pointer to I2S resources
  \return      \ref execution_status
*/
static int32_t I2S_Uninitialize (I2S_DRV_INFO *i2s)
{
	i2s->cb_event = NULL;

	if(i2s->cfg->dma_enable)
	{
		i2s->dma_cfg->dma_rx.dma_handle = -1;
		i2s->dma_cfg->dma_tx.dma_handle = -1;
	}

	i2s->flags = 0U;
	i2s->drv_status.status = 0U;
	i2s->int_status   = 0U;

	i2s->tx_buff.buf =  NULL;
	i2s->tx_buff.ofs =  0;
	i2s->tx_buff.total_len =  0;

	i2s->rx_buff.buf =  NULL;
	i2s->rx_buff.ofs =  0;
	i2s->rx_buff.total_len =  0;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_Send (const void *data, uint32_t num, I2S_DRV_INFO *i2s)
  \brief       Start sending data to I2S transmitter.
  \param[in]   data  Location of the data buffer to be transmitted
  \param[in]   num   Number of data items to send
  \param[in]   i2s       Pointer to I2S resources
  \return      \ref execution_status
*/
static int32_t I2S_Send (const void *data, uint32_t num, I2S_DRV_INFO *i2s)
{
	ARM_DMA_PARAMS dma_params;
	int32_t status;

	/* Verify the input parameters */
	if (!data || !num)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* Verify whether the driver is configured and powered*/
	if (!((i2s->flags & I2S_FLAG_DRV_CONFIG_DONE) && (i2s->flags & I2S_FLAG_CLKSRC_ENABLED)))
		return ARM_DRIVER_ERROR;

	/* Check if any Transfer is in progress*/
	if (i2s->drv_status.status_b.tx_busy || i2s->drv_status.status_b.rx_busy)
		return ARM_DRIVER_ERROR_BUSY;

	/* If the WSS len is 16, check if it is aligned to 2 bytes */
	if ((i2s->cfg->wss_len == WSS_CLOCK_CYCLES_16) && ((uint32_t)data & 0x1U) != 0U)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* If the WSS len is greater than 16, check if it is aligned to 4 bytes */
	if ((i2s->cfg->wss_len > WSS_CLOCK_CYCLES_16) && ((uint32_t)data & 0x3U) != 0U)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* Set the Tx flags*/
	i2s->drv_status.status_b.tx_busy = 1U;
	i2s->drv_status.status_b.tx_underflow = 0U;

	/* Update buffer*/
	i2s->tx_buff.buf = data;

	if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
		i2s->tx_buff.total_len = num * sizeof(uint16_t);
	else
		i2s->tx_buff.total_len = num * sizeof(uint32_t);

	/* Initialize the offset */
	i2s->tx_buff.ofs = 0U;

	/* Clear Overrun interrupt if any */
	I2S_ClearTxOverrun (i2s);

	/* Enable Tx Channel */
	I2S_TxChannelEnable (i2s);

	/* Check if DMA is enabled for this */
	if(i2s->cfg->dma_enable)
	{
		dma_params.peri_reqno = i2s->dma_cfg->dma_tx.dma_periph_req;
		dma_params.dir        = ARM_DMA_MEM_TO_DEV;
		dma_params.cb_event   = i2s->dma_cb;
		dma_params.src_addr   = (void*)data;
		dma_params.dst_addr   = (void*)&i2s->paddr->TXDMA;
		dma_params.num_bytes  = i2s->tx_buff.total_len;
		dma_params.irq_priority  = i2s->cfg->dma_irq_priority;

		if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
			dma_params.burst_size = BS_BYTE_2;
		else
			dma_params.burst_size = BS_BYTE_4;

		if(i2s->flags & I2S_FLAG_DRV_MONO_MODE)
			dma_params.burst_len  = 1;
		else
			dma_params.burst_len  = I2S_FIFO_DEPTH - i2s->cfg->tx_fifo_trg_lvl;

		/* Start DMA transfer */
		status = I2S_DMA_Start(&i2s->dma_cfg->dma_tx, &dma_params);
		if(status)
			return ARM_DRIVER_ERROR;
	}
	else
	{
		/* Enable Tx Interrupt */
		I2S_EnableTxInterrupt (i2s);
	}

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t I2S_Receive (void *data, uint32_t num, I2S_DRV_INFO *i2s)
  \brief       Start receiving data from I2S receiver.
  \param[out]  data  Data pointer to store the received data from I2S
  \param[in]   num   Number of data items to receive
  \param[in]   i2s       Pointer to I2S resources
  \return      \ref execution_status
*/
static int32_t I2S_Receive (void *data, uint32_t num, I2S_DRV_INFO *i2s)
{
	ARM_DMA_PARAMS dma_params;
	int32_t status;

	/* Verify the input parameters */
	if (!data || !num)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* Verify whether the driver is configured and powered*/
	if (!((i2s->flags & I2S_FLAG_DRV_CONFIG_DONE) && (i2s->flags & I2S_FLAG_CLKSRC_ENABLED)))
		return ARM_DRIVER_ERROR;

	/* Check if any Transfer is in progress*/
	if (i2s->drv_status.status_b.tx_busy || i2s->drv_status.status_b.rx_busy)
		return ARM_DRIVER_ERROR_BUSY;

	/* If the WSS len is 16, check if it is aligned to 2 bytes */
	if ((i2s->cfg->wss_len == WSS_CLOCK_CYCLES_16) && ((uint32_t)data & 0x1U) != 0U)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* If the WSS len is greater than 16, check if it is aligned to 4 bytes */
	if ((i2s->cfg->wss_len > WSS_CLOCK_CYCLES_16) && ((uint32_t)data & 0x3U) != 0U)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* Set the Rx flags*/
	i2s->drv_status.status_b.rx_busy = 1U;
	i2s->drv_status.status_b.rx_overflow = 0U;

	/* Check if DMA & Mono is enabled for this */
	if(i2s->cfg->dma_enable && (i2s->flags & I2S_FLAG_DRV_MONO_MODE))
	{
		/*
		 * This is a hack for the mono+dma mode.
		 * Mono mode feature is not supported by I2S HW.
		 *
		 * To achieve this in software, we need to read
		 * Left and Right channel and ignore the right channel
		 * write to the buffer.
		 *
		 * But in DMA, we need to read the data from peripheral
		 * and write to the buffer. We can't simply ignore the write here.
		 * So we use the next buffer location to read the right channel
		 * and re-adjust the destination address to store the next
		 * left channel data in the same location.
		 *
		 * Due to this, there will be memory overwrite to num+1 buffer.
		 * To avoid this, instruct the DMA to copy the number of samples
		 * to (num - 1)
		 */
		num = num - 1;
	}

	/* Update buffer*/
	i2s->rx_buff.buf = data;
	if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
		i2s->rx_buff.total_len = num * sizeof(uint16_t);
	else
		i2s->rx_buff.total_len = num * sizeof(uint32_t);

	i2s->rx_buff.ofs = 0U;

	/* Clear Overrun interrupt if any */
	I2S_ClearRxOverrun (i2s);

	/* Enable Rx Channel */
	I2S_RxChannelEnable (i2s);

	/* Check if DMA is enabled for this */
	if(i2s->cfg->dma_enable)
	{
		dma_params.peri_reqno = i2s->dma_cfg->dma_rx.dma_periph_req;
		dma_params.dir        = ARM_DMA_DEV_TO_MEM;
		dma_params.cb_event   = i2s->dma_cb;
		dma_params.src_addr   = (void*)&i2s->paddr->RXDMA;
		dma_params.dst_addr   = (void*)data;
		dma_params.num_bytes  = i2s->rx_buff.total_len;
		dma_params.irq_priority  = i2s->cfg->dma_irq_priority;

		if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
			dma_params.burst_size = BS_BYTE_2;
		else
			dma_params.burst_size = BS_BYTE_4;

		if(i2s->flags & I2S_FLAG_DRV_MONO_MODE)
			dma_params.burst_len  = 1;
		else
			dma_params.burst_len  = i2s->cfg->rx_fifo_trg_lvl + 1;

		/* Enable the Rx Overflow interrupt */
		I2S_EnableRxFOInterrupt(i2s);

		/* Start DMA transfer */
		status = I2S_DMA_Start(&i2s->dma_cfg->dma_rx, &dma_params);
		if(status)
			return ARM_DRIVER_ERROR;
	}
	else
	{
		/* Enable RX Interrupts */
		I2S_EnableRxInterrupt (i2s);
	}

	return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t I2S_GetTxCount (I2S_DRV_INFO *i2s)
  \brief       Get the total transmitted items
  \param[in]   i2s       Pointer to I2S resources
  \return      number of data items transmitted
*/
static uint32_t I2S_GetTxCount (I2S_DRV_INFO *i2s)
{
	return i2s->tx_buff.ofs;
}

/**
  \fn          uint32_t I2S_GetRxCount (I2S_DRV_INFO *i2s)
  \brief       Get total items received
  \param[in]   i2s       Pointer to I2S resources
  \return      number of data items received
*/
static uint32_t I2S_GetRxCount (I2S_DRV_INFO *i2s)
{
	return i2s->rx_buff.ofs;
}

/**
  \fn          int32_t I2S_Control (uint32_t control, uint32_t arg1, uint32_t arg2, I2S_DRV_INFO *i2s)
  \brief       Control I2S Interface.
  \param[in]   control  Operation
  \param[in]   arg1     Argument 1 of operation (optional)
  \param[in]   arg2     Argument 2 of operation (optional)
  \param[in]   i2s      Pointer to I2S resources
  \return      common \ref execution_status and driver specific \ref sai_execution_status
*/

static int32_t I2S_Control (uint32_t control, uint32_t arg1, uint32_t arg2, I2S_DRV_INFO *i2s)
{
	uint16_t frame_length = 0;
	uint8_t datasize = 0;

	/* Verify whether the driver is initialized and powered*/
	if (!((i2s->flags & I2S_FLAG_DRV_INIT_DONE) && (i2s->flags & I2S_FLAG_CLKSRC_ENABLED)))
		return ARM_DRIVER_ERROR;

	/* Handle Control Codes */
	switch (control & ARM_SAI_CONTROL_Msk)
	{
	case ARM_SAI_CONFIGURE_TX:
		/* Set FIFO Trigger Level */
		I2S_SetTxTriggerLevel (i2s);

		break;
	case ARM_SAI_CONFIGURE_RX:
		/* Set FIFO Trigger Level */
		I2S_SetRxTriggerLevel (i2s);

		break;
	case ARM_SAI_CONTROL_TX:
		/* Enable TX */
		if (arg1 == true)
		{
			/* Configure the I2S Peripheral Clock */
			I2S_Configure_ClockSource (true, i2s);

			/* Reset the Tx FIFO */
			I2S_TxFifoReset (i2s);
			/* Set WLEN */
			I2S_TxConfigWlen (i2s);
			/* Enable Master Clock */
			I2S_ConfigureClock (i2s);
			I2S_ClockEnable (i2s);

			/* Enable Tx Block */
			I2S_TxBlockEnable (i2s);
			/* Disable Rx Channel */
			I2S_RxChannelDisable (i2s);

			/* Check if DMA is enabled for this */
			if(i2s->cfg->dma_enable)
			{
				/* Try to allocate a DMA channel */
				if(I2S_DMA_Allocate(&i2s->dma_cfg->dma_tx) == ARM_DRIVER_ERROR)
					return ARM_DRIVER_ERROR;

				if(i2s->flags & I2S_FLAG_DRV_MONO_MODE)
				{
					if(I2S_DMA_EnableMono(&i2s->dma_cfg->dma_tx) == ARM_DRIVER_ERROR)
						return ARM_DRIVER_ERROR;
				}

				/* Enable the DMA interface of I2S */
				I2S_TxDMAEnable(i2s);
			}

		} else if (arg1 == false)
		{
			if(i2s->cfg->dma_enable)
			{
				/* Disable the DMA interface of I2S */
				I2S_TxDMADisable(i2s);

				/* Deallocate DMA channel */
				if(I2S_DMA_DeAllocate(&i2s->dma_cfg->dma_tx) == ARM_DRIVER_ERROR)
					return ARM_DRIVER_ERROR;
			}

			/* Disable Tx Channel */
			I2S_TxChannelDisable (i2s);
			/* Disable Tx Block */
			I2S_TxBlockDisable (i2s);

			/* Disable Tx Interrupt */
			I2S_DisableTxInterrupt (i2s);

			/* Disable Master Clock */
			I2S_ClockDisable (i2s);

			/* Set the Tx flags*/
			i2s->drv_status.status_b.tx_busy = 0U;

		} else
			return ARM_DRIVER_ERROR;

		return ARM_DRIVER_OK;
	case ARM_SAI_CONTROL_RX:
		/* Enable RX */
		if (arg1 == true)
		{

			/* Configure the I2S Peripheral Clock */
			I2S_Configure_ClockSource (true, i2s);

			/* Reset the Rx FIFO */
			I2S_RxFifoReset (i2s);
			/* Set WLEN */
			I2S_RxConfigWlen (i2s);
			/* Enable Master Clock */
			I2S_ConfigureClock (i2s);
			I2S_ClockEnable (i2s);
			/* Enable Rx Block */
			I2S_RxBlockEnable (i2s);
			/* Disable Tx Channel */
			I2S_TxChannelDisable (i2s);

			/* Check if DMA is enabled for this */
			if(i2s->cfg->dma_enable)
			{
				/* Try to allocate a DMA channel */
				if(I2S_DMA_Allocate(&i2s->dma_cfg->dma_rx) == ARM_DRIVER_ERROR)
					return ARM_DRIVER_ERROR;

				if(i2s->flags & I2S_FLAG_DRV_MONO_MODE)
				{
					if(I2S_DMA_EnableMono(&i2s->dma_cfg->dma_rx) == ARM_DRIVER_ERROR)
						return ARM_DRIVER_ERROR;
				}

				/* Enable the DMA interface of I2S */
				I2S_RxDMAEnable(i2s);
			}
		}
		else if (arg1 == false)
		{
			/* Check if DMA is enabled for this */
			if(i2s->cfg->dma_enable)
			{
				/* Disable the DMA interface of I2S */
				I2S_RxDMADisable(i2s);

				/* Deallocate DMA channel */
				if(I2S_DMA_DeAllocate(&i2s->dma_cfg->dma_rx) == ARM_DRIVER_ERROR)
					return ARM_DRIVER_ERROR;
			}

			/* Disable Rx Channel */
			I2S_RxChannelDisable (i2s);
			/* Disable Rx Block */
			I2S_RxBlockDisable (i2s);

			/* Disable Rx Interrupt */
			I2S_DisableRxInterrupt (i2s);
			/* Disable Master Clock */
			I2S_ClockDisable (i2s);

			/* Set the rx flags*/
			i2s->drv_status.status_b.rx_busy = 0U;

		}else
			return ARM_DRIVER_ERROR;

		return ARM_DRIVER_OK;
	case ARM_SAI_ABORT_SEND:
		/* Check if DMA is enabled for this */
		if(i2s->cfg->dma_enable)
		{
			/* Stop DMA transfer */
			if(I2S_DMA_Stop(&i2s->dma_cfg->dma_tx) == ARM_DRIVER_ERROR)
				return ARM_DRIVER_ERROR;
		}

		/* Disable Tx Channel */
		I2S_TxChannelDisable (i2s);
		/* Disable Tx Interrupt */
		I2S_DisableTxInterrupt (i2s);
		/* Reset the Tx FIFO */
		I2S_TxFifoReset (i2s);
		/* Set the Tx flags*/
		i2s->drv_status.status_b.tx_busy = 0U;
		/* Enable Tx Channel */
		I2S_TxChannelEnable (i2s);
		return ARM_DRIVER_OK;
	case ARM_SAI_ABORT_RECEIVE:
		/* Check if DMA is enabled for this */
		if(i2s->cfg->dma_enable)
		{
			/* Disable the overflow interrupt */
			I2S_DisableRxFOInterrupt(i2s);

			/* Stop DMA transfer */
			if(I2S_DMA_Stop(&i2s->dma_cfg->dma_rx) == ARM_DRIVER_ERROR)
				return ARM_DRIVER_ERROR;
		}
		/* Disable Rx Channel */
		I2S_RxChannelDisable (i2s);
		/* Disable Rx Interrupt */
		I2S_DisableRxInterrupt (i2s);
		/* Reset the Rx FIFO */
		I2S_RxFifoReset (i2s);
		/* Set the rx flags*/
		i2s->drv_status.status_b.rx_busy = 0U;
		/* Enable Rx Channel */
		I2S_RxChannelEnable (i2s);
		return ARM_DRIVER_OK;
	case ARM_SAI_MASK_SLOTS_TX:
	case ARM_SAI_MASK_SLOTS_RX:
	default:
		return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	/* Handle I2S Modes */
	if ((control & ARM_SAI_MODE_Msk) != ARM_SAI_MODE_MASTER)
		return ARM_DRIVER_ERROR_UNSUPPORTED;

	/* Handle Synchronization */
	switch (control & ARM_SAI_SYNCHRONIZATION_Msk)
	{
	case ARM_SAI_ASYNCHRONOUS:
		break;
	case ARM_SAI_SYNCHRONOUS:
		if ((control & ARM_SAI_MODE_Msk) == ARM_SAI_MODE_MASTER)
			return ARM_SAI_ERROR_SYNCHRONIZATION;
		break;
	default:
		return ARM_SAI_ERROR_SYNCHRONIZATION;
	}

	/* Handle Protocol  */
	switch (control & ARM_SAI_PROTOCOL_Msk)
	{
	case ARM_SAI_PROTOCOL_I2S:
		break;
	default:
		return ARM_SAI_ERROR_PROTOCOL;
	}

	/* Handle DataSize */
	datasize = ((control & ARM_SAI_DATA_SIZE_Msk) >> ARM_SAI_DATA_SIZE_Pos) + 1;
	switch (datasize)
	{
	case 12:
		i2s->cfg->wlen    = RES_12_BIT;
		break;
	case 16:
		i2s->cfg->wlen    = RES_16_BIT;
		break;
	case 20:
		i2s->cfg->wlen    = RES_20_BIT;
		break;
	case 24:
		i2s->cfg->wlen    = RES_24_BIT;
		break;
	case 32:
		i2s->cfg->wlen    = RES_32_BIT;
		break;

	default:
		return ARM_SAI_ERROR_DATA_SIZE;
	}

	/* Handle Bit Order */
	switch (control & ARM_SAI_BIT_ORDER_Msk)
	{
		case ARM_SAI_MSB_FIRST:
		break;

		default:
			return ARM_SAI_ERROR_BIT_ORDER;
	}

	/* Handle Mono Mode */
	if (control & ARM_SAI_MONO_MODE)
	    i2s->flags |= I2S_FLAG_DRV_MONO_MODE;

	/* Unsupported Codes */
	if ((control & ARM_SAI_COMPANDING_Msk)
		|| (control & ARM_SAI_CLOCK_POLARITY_Msk)
		|| (control & ARM_SAI_MCLK_PIN_Msk))
		return ARM_DRIVER_ERROR_UNSUPPORTED;

	/* Handle Frame Length */
	frame_length =  ((arg1 & ARM_SAI_FRAME_LENGTH_Msk) >> ARM_SAI_FRAME_LENGTH_Pos);
	if (frame_length != (datasize*2))
		return ARM_SAI_ERROR_FRAME_LENGTH;

	/* Handle Sample Rate */
	if (arg2 & ARM_SAI_AUDIO_FREQ_Msk)
		i2s->sample_rate = arg2 & ARM_SAI_AUDIO_FREQ_Msk;
	else
		return ARM_SAI_ERROR_AUDIO_FREQ;

	i2s->flags |= I2S_FLAG_DRV_CONFIG_DONE;

	return ARM_DRIVER_OK;
}

/**
  \fn          ARM_SAI_STATUS I2S_GetStatus (I2S_DRV_INFO *i2s)
  \brief       Get I2S status.
  \param[in]   i2s       Pointer to I2S resources
  \return      SAI status \ref ARM_SAI_STATUS
*/
static ARM_SAI_STATUS I2S_GetStatus (I2S_DRV_INFO *i2s)
{

	return i2s->drv_status.status_b;
}

/**
  \fn          void I2S_RxIRQHandler (I2S_DRV_INFO *i2s)
  \brief       Run the Rx IRQ Handler
  \param[in]   i2s       Pointer to I2S resources
*/
static void I2S_RxIRQHandler (I2S_DRV_INFO *i2s)
{
	uint32_t rx_avail = i2s->cfg->rx_fifo_trg_lvl + 1; /* Data available in RX FIFO */
	void *const buff  = i2s->rx_buff.buf; /* Assign the buffer base address */
	uint8_t last_lap = 0, bytes = 0, cnt = 0, frames = 0;

	if (_FLD2VAL(I2S_ISR_RXFO, i2s->int_status))
	{
		/* Send event to application to handle it */
		i2s->cb_event (ARM_SAI_EVENT_RX_OVERFLOW);
	}

	/* If DMA is enabled no need to process the data from the FIFO */
	if(i2s->cfg->dma_enable)
		return;

	if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
		bytes = I2S_16BIT_BUF_TYPE;
	else
		bytes = I2S_32BIT_BUF_TYPE;

	/* Check if it is the last lap */
	if ((i2s->rx_buff.ofs + (2 * rx_avail * bytes)) >  i2s->rx_buff.total_len)
	{
		/* Assign the number of iterations required */
		frames = (i2s->rx_buff.total_len - i2s->rx_buff.ofs)/(2*bytes);
		last_lap = 1;
	} else
		frames = rx_avail;

	for (cnt = 0; cnt < frames; cnt++)
	{
		/* Assuming that application uses 16bit buffer for 16bit data resolution */
		if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
		{
		    if (i2s->flags & I2S_FLAG_DRV_MONO_MODE)
		    {
		        (*(uint16_t*)(buff + i2s->rx_buff.ofs)) = (uint16_t)I2S_ReadLeftRx (i2s);
		        I2S_ReadRightRx (i2s);
		        i2s->rx_buff.ofs = i2s->rx_buff.ofs + I2S_16BIT_BUF_TYPE;
		    }
		    else
		    {
		        (*(uint16_t*)(buff + i2s->rx_buff.ofs)) = (uint16_t)I2S_ReadLeftRx (i2s);
		        (*(uint16_t*)(buff + i2s->rx_buff.ofs + I2S_16BIT_BUF_TYPE)) = (uint16_t)I2S_ReadRightRx (i2s);
		        i2s->rx_buff.ofs = i2s->rx_buff.ofs + 2*I2S_16BIT_BUF_TYPE;
		    }
		}
		else /* For > 16bit data resolution consider as 32bit buffer*/
		{
		    if (i2s->flags & I2S_FLAG_DRV_MONO_MODE)
		    {
		        (*(uint32_t*)(buff + i2s->rx_buff.ofs)) = I2S_ReadLeftRx (i2s);
		        I2S_ReadRightRx (i2s);
		        i2s->rx_buff.ofs = i2s->rx_buff.ofs + I2S_32BIT_BUF_TYPE;
		    }
		    else
		    {
		        (*(uint32_t*)(buff + i2s->rx_buff.ofs)) = I2S_ReadLeftRx (i2s);
		        (*(uint32_t*)(buff + i2s->rx_buff.ofs + I2S_32BIT_BUF_TYPE)) = I2S_ReadRightRx (i2s);
		        i2s->rx_buff.ofs = i2s->rx_buff.ofs + 2*I2S_32BIT_BUF_TYPE;
		    }
		}
	}

	if (last_lap && (i2s->rx_buff.ofs < i2s->rx_buff.total_len))
	{
		if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
		{
			/* Read the last sample from left */
			(*(uint16_t*)(buff + i2s->rx_buff.ofs)) = (uint16_t)I2S_ReadLeftRx (i2s);
			I2S_ReadRightRx (i2s);
			i2s->rx_buff.ofs = i2s->rx_buff.ofs + I2S_16BIT_BUF_TYPE;
		}
		else
		{
			/* Read the last sample from left */
			(*(uint32_t*)(buff + i2s->rx_buff.ofs)) = I2S_ReadLeftRx (i2s);
			I2S_ReadRightRx (i2s);
			i2s->rx_buff.ofs = i2s->rx_buff.ofs + I2S_32BIT_BUF_TYPE;
		}
	}

	/* Once the buffer is full, send complete event with interrupt disabled */
	if (i2s->rx_buff.ofs >= i2s->rx_buff.total_len)
	{
		/* Disable Rx Interrupt */
		I2S_DisableRxInterrupt (i2s);
		/* Set the Rx flags*/
		i2s->drv_status.status_b.rx_busy = 0U;
		if (i2s->cb_event)
			i2s->cb_event (ARM_SAI_EVENT_RECEIVE_COMPLETE);
	}
}

/**
  \fn          void I2S_TxIRQHandler (I2S_DRV_INFO *i2s)
  \brief       Run the Tx IRQ Handler
  \param[in]   i2s       Pointer to I2S resources
*/
static void I2S_TxIRQHandler (I2S_DRV_INFO *i2s)
{
	uint32_t tx_avail = I2S_FIFO_DEPTH - i2s->cfg->tx_fifo_trg_lvl; /* Number of data that can copy to TX FIFO */
	const void *buff    = i2s->tx_buff.buf; /* Assign the buffer base address */
	uint8_t last_lap = 0, bytes = 0, cnt = 0, frames = 0;

	if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
		bytes = I2S_16BIT_BUF_TYPE;
	else
		bytes = I2S_32BIT_BUF_TYPE;

	/* Check if it is the last lap */
	if ((i2s->tx_buff.ofs + (2 * tx_avail * bytes)) >  i2s->tx_buff.total_len)
	{
		/* Assign the number of iterations required */
		frames = (i2s->tx_buff.total_len - i2s->tx_buff.ofs)/(2*bytes);
		last_lap = 1;
	} else
		frames = tx_avail;

	for (cnt = 0; cnt < frames; cnt++)
	{
		/* Assuming that application uses 16bit buffer for 16bit data resolution */
		if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
		{
		    if (i2s->flags & I2S_FLAG_DRV_MONO_MODE)
		    {
		        I2S_WriteLeftTx ((uint32_t)(*(uint16_t*)(buff + i2s->tx_buff.ofs)), i2s);
		        I2S_WriteRightTx (0, i2s);
		        i2s->tx_buff.ofs = i2s->tx_buff.ofs + I2S_16BIT_BUF_TYPE;
		    }
		    else
		    {
		        I2S_WriteLeftTx ((uint32_t)(*(uint16_t*)(buff + i2s->tx_buff.ofs)), i2s);
		        I2S_WriteRightTx ((uint32_t)(*(uint16_t*)(buff + i2s->tx_buff.ofs + I2S_16BIT_BUF_TYPE)), i2s);
		        i2s->tx_buff.ofs = i2s->tx_buff.ofs + 2*I2S_16BIT_BUF_TYPE;
		    }
		}
		else /* For > 16bit data resolution consider as 32bit buffer*/
		{
		    if (i2s->flags & I2S_FLAG_DRV_MONO_MODE)
		    {
		        I2S_WriteLeftTx (*(uint32_t*)(buff + i2s->tx_buff.ofs), i2s);
		        I2S_WriteRightTx (0, i2s);
		        i2s->tx_buff.ofs = i2s->tx_buff.ofs + I2S_32BIT_BUF_TYPE;
		    }
		    else
		    {
		        I2S_WriteLeftTx (*(uint32_t*)(buff + i2s->tx_buff.ofs), i2s);
		        I2S_WriteRightTx (*(uint32_t*)(buff + i2s->tx_buff.ofs + I2S_32BIT_BUF_TYPE), i2s);
		        i2s->tx_buff.ofs = i2s->tx_buff.ofs + 2*I2S_32BIT_BUF_TYPE;
		    }
		}
	}

	if (last_lap && (i2s->tx_buff.ofs < i2s->tx_buff.total_len))
	{
		if ((i2s->cfg->wlen > IGNORE_WLEN) && (i2s->cfg->wlen <= RES_16_BIT))
		{
			/* Write the Left sample and fill right with 0 */
			I2S_WriteLeftTx ((uint32_t)(*(uint16_t*)(buff + i2s->tx_buff.ofs)), i2s);
			I2S_WriteRightTx (0, i2s);
			i2s->tx_buff.ofs = i2s->tx_buff.ofs + I2S_16BIT_BUF_TYPE;
		} else
		{
			/* Write the Left sample and fill right with 0 */
			I2S_WriteLeftTx (*(uint32_t*)(buff + i2s->tx_buff.ofs), i2s);
			I2S_WriteRightTx (0, i2s);
			i2s->tx_buff.ofs = i2s->tx_buff.ofs + I2S_32BIT_BUF_TYPE;
		}
	}

	/* Send complete event once all the data is copied to FIFO */
	if (i2s->tx_buff.ofs >= i2s->tx_buff.total_len)
	{
		/* Disable Tx Interrupt */
		I2S_DisableTxInterrupt (i2s);
		/* Set the Tx flags*/
		i2s->drv_status.status_b.tx_busy = 0U;
		if (i2s->cb_event)
			i2s->cb_event (ARM_SAI_EVENT_SEND_COMPLETE);
	}
}
/**
  \fn          void I2S_IRQHandler (I2S_DRV_INFO *i2s)
  \brief       Run the IRQ Handler
  \param[in]   i2s       Pointer to I2S resources
*/
static void I2S_IRQHandler (I2S_DRV_INFO *i2s)
{
	/* Get the Current Interrupt Status*/
	I2S_GetInterruptStatus (i2s);

	if (i2s->drv_status.status_b.tx_busy &&
		(_FLD2VAL(I2S_ISR_TXFE, i2s->int_status))) /* Handle Tx Interrupt */
		I2S_TxIRQHandler (i2s);
	else if (i2s->drv_status.status_b.rx_busy) /* Handle Rx Interrupt */
		I2S_RxIRQHandler (i2s);

	/* This should not happen */
	if (_FLD2VAL(I2S_ISR_TXFO, i2s->int_status))
		I2S_ClearTxOverrun (i2s);

	if (_FLD2VAL(I2S_ISR_RXFO, i2s->int_status))
	{
		/* Clear overrun interrupt */
		I2S_ClearRxOverrun (i2s);

		/*
		 * Disable the Rx Overflow interrupt for now. This will
		 * be enabled again when Receive function is called
		 */
		I2S_DisableRxFOInterrupt(i2s);
	}

}

/**
  \fn          static void  I2S_DMACallback (uint32_t event, int8_t peri_num,
                                             I2S_DRV_INFO *i2s)
  \brief       Callback function from DMA for I2S
  \param[in]   event     Event from DMA
  \param[in]   peri_num  Peripheral number
  \param[in]   i2s       Pointer to I2S resources
*/
static void I2S_DMACallback (uint32_t event, int8_t peri_num,
		                          I2S_DRV_INFO *i2s)
{
    if (!i2s->cb_event)
        return;

    /* Transfer Completed */
    if(event & ARM_DMA_EVENT_COMPLETE)
    {
        switch(peri_num)
        {
        case I2S0_TX_PERIPH_REQ:
        case I2S1_TX_PERIPH_REQ:
        case I2S2_TX_PERIPH_REQ:
        case I2S3_TX_PERIPH_REQ:
            /* Set the Tx flags*/
            i2s->drv_status.status_b.tx_busy = 0U;
            i2s->cb_event (ARM_SAI_EVENT_SEND_COMPLETE);
            break;

        case I2S0_RX_PERIPH_REQ:
        case I2S1_RX_PERIPH_REQ:
        case I2S2_RX_PERIPH_REQ:
        case I2S3_RX_PERIPH_REQ:
            /* Set the Rx flags*/
            i2s->drv_status.status_b.rx_busy = 0U;
            /* Disable the Overflow interrupt */
            I2S_DisableRxFOInterrupt(i2s);
            i2s->cb_event (ARM_SAI_EVENT_RECEIVE_COMPLETE);
            break;

        default:
            break;
        }
    }

    /* Abort Occurred */
    if(event & ARM_DMA_EVENT_ABORT)
    {
	    /*
	    * There is no event for indicating error in SAI driver.
	    * Let the application get timeout and restart the I2S.
	    *
	    */
    }
}

#if (RTE_I2S0)

static void I2S0_DMACallback (uint32_t event, int8_t peri_num);

static I2S_CONFIG_INFO I2S0_CONFIG = {
    .wss_len         = RTE_I2S0_WSS_CLOCK_CYCLES,
    .sclkg           = RTE_I2S0_SCLKG_CLOCK_CYCLES,
    .clk_source      = RTE_I2S0_CLK_SOURCE,
    .rx_fifo_trg_lvl = RTE_I2S0_RX_TRIG_LVL,
    .tx_fifo_trg_lvl = RTE_I2S0_TX_TRIG_LVL,
    .irq_priority    = RTE_I2S0_IRQ_PRI,
    .dma_enable      = RTE_I2S0_DMA_ENABLE,
    .dma_irq_priority = RTE_I2S0_DMA_IRQ_PRI,
};

static I2S_DMA_HW_CONFIG I2S0_DMA_HW_CONFIG = {
    .dma_rx =
    {
        .dma_drv        = &ARM_Driver_DMA_(I2S0_DMA),
        .dma_periph_req = I2S0_RX_PERIPH_REQ,
    },
    .dma_tx =
    {
        .dma_drv        = &ARM_Driver_DMA_(I2S0_DMA),
        .dma_periph_req = I2S0_TX_PERIPH_REQ,
    },
};

static I2S_DRV_INFO I2S0 = {
    .cb_event  = NULL,
    .dma_cb    = I2S0_DMACallback,
    .cfg       = &I2S0_CONFIG,
    .dma_cfg   = &I2S0_DMA_HW_CONFIG,
    .paddr     = (I2S_TypeDef *) I2S0_BASE,
    .clkreg_paddr = (__IOM uint32_t *) I2S0_CLK_ADDR,
    .irq       = (IRQn_Type) I2S0_IRQ,
    .flags     = NULL,
};

/**
  \fn          int32_t I2S0_Initialize (ARM_SAI_SignalEvent_t cb_event)
  \brief       Initialize I2S Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SAI_SignalEvent
  \return      \ref execution_status
*/
static int32_t I2S0_Initialize (ARM_SAI_SignalEvent_t cb_event)
{
	return I2S_Initialize (cb_event, &I2S0);
}

/**
  \fn          int32_t I2S0_Uninitialize (void)
  \brief       De-initialize I2S Interface.
  \return      \ref execution_status
*/
static int32_t I2S0_Uninitialize (void)
{
	return I2S_Uninitialize (&I2S0);
}

/**
  \fn          int32_t I2S0_PowerControl (ARM_POWER_STATE state)
  \brief       Control I2S Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t I2S0_PowerControl (ARM_POWER_STATE state)
{
	return I2S_PowerControl (state, &I2S0);
}

/**
  \fn          int32_t I2S0_Send (const void *data, uint32_t num)
  \brief       Start sending data to I2S transmitter.
  \param[in]   data  Pointer to buffer with data to send to I2S transmitter
  \param[in]   num   Number of data items to send
  \return      \ref execution_status
*/
static int32_t I2S0_Send (const void *data, uint32_t num)
{
	return I2S_Send (data, num, &I2S0);
}

/**
  \fn          int32_t I2S0_Receive (void *data, uint32_t num)
  \brief       Start receiving data from I2S receiver.
  \param[out]  data  Pointer to buffer for data to receive from I2S receiver
  \param[in]   num   Number of data items to receive
  \return      \ref execution_status
*/
static int32_t I2S0_Receive (void *data, uint32_t num)
{
	return I2S_Receive (data, num, &I2S0);
}

/**
  \fn          uint32_t I2S0_GetTxCount (void)
  \brief       Get transmitted data count.
  \return      number of data items transmitted
*/
static uint32_t I2S0_GetTxCount (void)
{
	return I2S_GetTxCount (&I2S0);
}

/**
  \fn          uint32_t I2S0_GetRxCount (void)
  \brief       Get received data count.
  \return      number of data items received
*/
static uint32_t I2S0_GetRxCount (void)
{
	return I2S_GetRxCount (&I2S0);
}

/**
  \fn          int32_t I2S0_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
  \brief       Control I2S Interface.
  \param[in]   control  Operation
  \param[in]   arg1     Argument 1 of operation (optional)
  \param[in]   arg2     Argument 2 of operation (optional)
  \return      common \ref execution_status and driver specific \ref sai_execution_status
*/
static int32_t I2S0_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
{
	return I2S_Control (control, arg1, arg2, &I2S0);
}

/**
  \fn          ARM_SAI_STATUS I2S0_GetStatus (void)
  \brief       Get I2S status.
  \return      SAI status \ref ARM_SAI_STATUS
*/
static ARM_SAI_STATUS I2S0_GetStatus (void)
{
	return I2S_GetStatus (&I2S0);
}

/**
  \fn          void  I2S0_IRQHandler (void)
  \brief       Run the IRQ Handler for I2S0
*/
void I2S0_IRQHandler (void)
{
	I2S_IRQHandler (&I2S0);
}

/**
  \fn          static void  I2S0_DMACallback (uint32_t event, int8_t peri_num)
  \param[in]   event     Event from DMA
  \param[in]   peri_num  Peripheral number
  \brief       Callback function from DMA for I2S0
*/
static void I2S0_DMACallback (uint32_t event, int8_t peri_num)
{
	I2S_DMACallback (event, peri_num, &I2S0);
}

/**
\brief Access structure of the I2S0 Driver.
*/
extern \
ARM_DRIVER_SAI Driver_SAI0;
ARM_DRIVER_SAI Driver_SAI0 = {
    I2S_GetVersion,
    I2S_GetCapabilities,
    I2S0_Initialize,
    I2S0_Uninitialize,
    I2S0_PowerControl,
    I2S0_Send,
    I2S0_Receive,
    I2S0_GetTxCount,
    I2S0_GetRxCount,
    I2S0_Control,
    I2S0_GetStatus
};
#endif //RTE_I2S0

#if (RTE_I2S1)

static void I2S1_DMACallback (uint32_t event, int8_t peri_num);

static I2S_CONFIG_INFO I2S1_CONFIG = {
    .wss_len         = RTE_I2S1_WSS_CLOCK_CYCLES,
    .sclkg           = RTE_I2S1_SCLKG_CLOCK_CYCLES,
    .clk_source      = RTE_I2S1_CLK_SOURCE,
    .rx_fifo_trg_lvl = RTE_I2S1_RX_TRIG_LVL,
    .tx_fifo_trg_lvl = RTE_I2S1_TX_TRIG_LVL,
    .irq_priority    = RTE_I2S1_IRQ_PRI,
    .dma_enable      = RTE_I2S1_DMA_ENABLE,
    .dma_irq_priority = RTE_I2S1_DMA_IRQ_PRI,
};

static I2S_DMA_HW_CONFIG I2S1_DMA_HW_CONFIG = {
    .dma_rx =
    {
        .dma_drv        = &ARM_Driver_DMA_(I2S1_DMA),
        .dma_periph_req = I2S1_RX_PERIPH_REQ,
    },
    .dma_tx =
    {
        .dma_drv        = &ARM_Driver_DMA_(I2S1_DMA),
        .dma_periph_req = I2S1_TX_PERIPH_REQ,
    },
};

static I2S_DRV_INFO I2S1 = {
    .cb_event  = NULL,
    .dma_cb    = I2S1_DMACallback,
    .cfg       = &I2S1_CONFIG,
    .dma_cfg   = &I2S1_DMA_HW_CONFIG,
    .paddr     = (I2S_TypeDef *) I2S1_BASE,
    .clkreg_paddr = (__IOM uint32_t *) I2S1_CLK_ADDR,
    .irq       = (IRQn_Type) I2S1_IRQ,
    .flags     = NULL,
};

/**
  \fn          int32_t I2S1_Initialize (ARM_SAI_SignalEvent_t cb_event)
  \brief       Initialize I2S Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SAI_SignalEvent
  \return      \ref execution_status
*/
static int32_t I2S1_Initialize (ARM_SAI_SignalEvent_t cb_event)
{
	return I2S_Initialize (cb_event, &I2S1);
}

/**
  \fn          int32_t I2S1_Uninitialize (void)
  \brief       De-initialize I2S Interface.
  \return      \ref execution_status
*/
static int32_t I2S1_Uninitialize (void)
{
	return I2S_Uninitialize (&I2S1);
}

/**
  \fn          int32_t I2S1_PowerControl (ARM_POWER_STATE state)
  \brief       Control I2S Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t I2S1_PowerControl (ARM_POWER_STATE state)
{
	return I2S_PowerControl (state, &I2S1);
}

/**
  \fn          int32_t I2S1_Send (const void *data, uint32_t num)
  \brief       Start sending data to I2S transmitter.
  \param[in]   data  Pointer to buffer with data to send to I2S transmitter
  \param[in]   num   Number of data items to send
  \return      \ref execution_status
*/
static int32_t I2S1_Send (const void *data, uint32_t num)
{
	return I2S_Send (data, num, &I2S1);
}

/**
  \fn          int32_t I2S1_Receive (void *data, uint32_t num)
  \brief       Start receiving data from I2S receiver.
  \param[out]  data  Pointer to buffer for data to receive from I2S receiver
  \param[in]   num   Number of data items to receive
  \return      \ref execution_status
*/
static int32_t I2S1_Receive (void *data, uint32_t num)
{
	return I2S_Receive (data, num, &I2S1);
}

/**
  \fn          uint32_t I2S1_GetTxCount (void)
  \brief       Get transmitted data count.
  \return      number of data items transmitted
*/
static uint32_t I2S1_GetTxCount (void)
{
	return I2S_GetTxCount (&I2S1);
}

/**
  \fn          uint32_t I2S1_GetRxCount (void)
  \brief       Get received data count.
  \return      number of data items received
*/
static uint32_t I2S1_GetRxCount (void)
{
	return I2S_GetRxCount (&I2S1);
}

/**
  \fn          int32_t I2S1_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
  \brief       Control I2S Interface.
  \param[in]   control  Operation
  \param[in]   arg1     Argument 1 of operation (optional)
  \param[in]   arg2     Argument 2 of operation (optional)
  \return      common \ref execution_status and driver specific \ref sai_execution_status
*/
static int32_t I2S1_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
{
	return I2S_Control (control, arg1, arg2, &I2S1);
}

/**
  \fn          ARM_SAI_STATUS I2S1_GetStatus (void)
  \brief       Get I2S status.
  \return      SAI status \ref ARM_SAI_STATUS
*/
static ARM_SAI_STATUS I2S1_GetStatus (void)
{
	return I2S_GetStatus (&I2S1);
}

/**
  \fn          void  I2S1_IRQHandler (void)
  \brief       Run the IRQ Handler for I2S1
*/
void I2S1_IRQHandler (void)
{
	I2S_IRQHandler (&I2S1);
}

/**
  \fn          void  I2S1_DMACallback (uint32_t event, int8_t peri_num)
  \param[in]   event     Event from DMA
  \param[in]   peri_num  Peripheral number
  \brief       Callback function from DMA for I2S1
*/
void I2S1_DMACallback (uint32_t event, int8_t peri_num)
{
	I2S_DMACallback (event, peri_num, &I2S1);
}

/**
\brief Access structure of the I2S1 Driver.
*/
extern \
ARM_DRIVER_SAI Driver_SAI1;
ARM_DRIVER_SAI Driver_SAI1 = {
    I2S_GetVersion,
    I2S_GetCapabilities,
    I2S1_Initialize,
    I2S1_Uninitialize,
    I2S1_PowerControl,
    I2S1_Send,
    I2S1_Receive,
    I2S1_GetTxCount,
    I2S1_GetRxCount,
    I2S1_Control,
    I2S1_GetStatus
};
#endif //RTE_I2S1

#if (RTE_I2S2)

static void I2S2_DMACallback (uint32_t event, int8_t peri_num);

static I2S_CONFIG_INFO I2S2_CONFIG = {
    .wss_len         = RTE_I2S2_WSS_CLOCK_CYCLES,
    .sclkg           = RTE_I2S2_SCLKG_CLOCK_CYCLES,
    .clk_source      = RTE_I2S2_CLK_SOURCE,
    .rx_fifo_trg_lvl = RTE_I2S2_RX_TRIG_LVL,
    .tx_fifo_trg_lvl = RTE_I2S2_TX_TRIG_LVL,
    .irq_priority    = RTE_I2S2_IRQ_PRI,
    .dma_enable      = RTE_I2S2_DMA_ENABLE,
    .dma_irq_priority = RTE_I2S2_DMA_IRQ_PRI,
};

static I2S_DMA_HW_CONFIG I2S2_DMA_HW_CONFIG = {
    .dma_rx =
    {
        .dma_drv        = &ARM_Driver_DMA_(I2S2_DMA),
        .dma_periph_req = I2S2_RX_PERIPH_REQ,
    },
    .dma_tx =
    {
        .dma_drv        = &ARM_Driver_DMA_(I2S2_DMA),
        .dma_periph_req = I2S2_TX_PERIPH_REQ,
    },
};

static I2S_DRV_INFO I2S2 = {
    .cb_event  = NULL,
    .dma_cb    = I2S2_DMACallback,
    .cfg       = &I2S2_CONFIG,
    .dma_cfg   = &I2S2_DMA_HW_CONFIG,
    .paddr     = (I2S_TypeDef *) I2S2_BASE,
    .clkreg_paddr = (__IOM uint32_t *) I2S2_CLK_ADDR,
    .irq       = (IRQn_Type) I2S2_IRQ,
    .flags     = NULL,
};


/**
  \fn          int32_t I2S2_Initialize (ARM_SAI_SignalEvent_t cb_event)
  \brief       Initialize I2S Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SAI_SignalEvent
  \return      \ref execution_status
*/
static int32_t I2S2_Initialize (ARM_SAI_SignalEvent_t cb_event)
{
	return I2S_Initialize (cb_event, &I2S2);
}

/**
  \fn          int32_t I2S2_Uninitialize (void)
  \brief       De-initialize I2S Interface.
  \return      \ref execution_status
*/
static int32_t I2S2_Uninitialize (void)
{
	return I2S_Uninitialize (&I2S2);
}

/**
  \fn          int32_t I2S2_PowerControl (ARM_POWER_STATE state)
  \brief       Control I2S Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t I2S2_PowerControl (ARM_POWER_STATE state)
{
	return I2S_PowerControl (state, &I2S2);
}

/**
  \fn          int32_t I2S2_Send (const void *data, uint32_t num)
  \brief       Start sending data to I2S transmitter.
  \param[in]   data  Pointer to buffer with data to send to I2S transmitter
  \param[in]   num   Number of data items to send
  \return      \ref execution_status
*/
static int32_t I2S2_Send (const void *data, uint32_t num)
{
	return I2S_Send (data, num, &I2S2);
}

/**
  \fn          int32_t I2S2_Receive (void *data, uint32_t num)
  \brief       Start receiving data from I2S receiver.
  \param[out]  data  Pointer to buffer for data to receive from I2S receiver
  \param[in]   num   Number of data items to receive
  \return      \ref execution_status
*/
static int32_t I2S2_Receive (void *data, uint32_t num)
{
	return I2S_Receive (data, num, &I2S2);
}

/**
  \fn          uint32_t I2S2_GetTxCount (void)
  \brief       Get transmitted data count.
  \return      number of data items transmitted
*/
static uint32_t I2S2_GetTxCount (void)
{
	return I2S_GetTxCount (&I2S2);
}

/**
  \fn          uint32_t I2S2_GetRxCount (void)
  \brief       Get received data count.
  \return      number of data items received
*/
static uint32_t I2S2_GetRxCount (void)
{
	return I2S_GetRxCount (&I2S2);
}

/**
  \fn          int32_t I2S2_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
  \brief       Control I2S Interface.
  \param[in]   control  Operation
  \param[in]   arg1     Argument 1 of operation (optional)
  \param[in]   arg2     Argument 2 of operation (optional)
  \return      common \ref execution_status and driver specific \ref sai_execution_status
*/
static int32_t I2S2_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
{
	return I2S_Control (control, arg1, arg2, &I2S2);
}

/**
  \fn          ARM_SAI_STATUS I2S2_GetStatus (void)
  \brief       Get I2S status.
  \return      SAI status \ref ARM_SAI_STATUS
*/
static ARM_SAI_STATUS I2S2_GetStatus (void)
{
	return I2S_GetStatus (&I2S2);
}

/**
  \fn          void  I2S2_IRQHandler (void)
  \brief       Run the IRQ Handler for I2S2
*/
void I2S2_IRQHandler (void)
{
	I2S_IRQHandler (&I2S2);
}

/**
  \fn          void  I2S2_DMACallback (uint32_t event, int8_t peri_num)
  \param[in]   event     Event from DMA
  \param[in]   peri_num  Peripheral number
  \brief       Callback function from DMA for I2S2
*/
void I2S2_DMACallback (uint32_t event, int8_t peri_num)
{
	I2S_DMACallback (event, peri_num, &I2S2);
}

/**
\brief Access structure of the I2S2 Driver.
*/
extern \
ARM_DRIVER_SAI Driver_SAI2;
ARM_DRIVER_SAI Driver_SAI2 = {
    I2S_GetVersion,
    I2S_GetCapabilities,
    I2S2_Initialize,
    I2S2_Uninitialize,
    I2S2_PowerControl,
    I2S2_Send,
    I2S2_Receive,
    I2S2_GetTxCount,
    I2S2_GetRxCount,
    I2S2_Control,
    I2S2_GetStatus
};
#endif //RTE_I2S2

#if (RTE_I2S3)

static void I2S3_DMACallback (uint32_t event, int8_t peri_num);

static I2S_CONFIG_INFO I2S3_CONFIG = {
    .wss_len         = RTE_I2S3_WSS_CLOCK_CYCLES,
    .sclkg           = RTE_I2S3_SCLKG_CLOCK_CYCLES,
    .clk_source      = RTE_I2S3_CLK_SOURCE,
    .rx_fifo_trg_lvl = RTE_I2S3_RX_TRIG_LVL,
    .tx_fifo_trg_lvl = RTE_I2S3_TX_TRIG_LVL,
    .irq_priority    = RTE_I2S3_IRQ_PRI,
    .dma_enable      = RTE_I2S3_DMA_ENABLE,
    .dma_irq_priority = RTE_I2S3_DMA_IRQ_PRI,
};

static I2S_DMA_HW_CONFIG I2S3_DMA_HW_CONFIG = {
    .dma_rx =
    {
        .dma_drv        = &ARM_Driver_DMA_(I2S3_DMA),
        .dma_periph_req = I2S3_RX_PERIPH_REQ,
    },
    .dma_tx =
    {
        .dma_drv        = &ARM_Driver_DMA_(I2S3_DMA),
        .dma_periph_req = I2S3_TX_PERIPH_REQ,
    },
};

static I2S_DRV_INFO I2S3 = {
    .cb_event  = NULL,
    .dma_cb    = I2S3_DMACallback,
    .cfg       = &I2S3_CONFIG,
    .dma_cfg   = &I2S3_DMA_HW_CONFIG,
    .paddr     = (I2S_TypeDef *) I2S3_BASE,
    .clkreg_paddr = (__IOM uint32_t *) I2S3_CLK_ADDR,
    .irq       = (IRQn_Type) I2S3_IRQ,
    .flags     = NULL,
};


/**
  \fn          int32_t I2S3_Initialize (ARM_SAI_SignalEvent_t cb_event)
  \brief       Initialize I2S Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SAI_SignalEvent
  \return      \ref execution_status
*/
static int32_t I2S3_Initialize (ARM_SAI_SignalEvent_t cb_event)
{
	return I2S_Initialize (cb_event, &I2S3);
}

/**
  \fn          int32_t I2S3_Uninitialize (void)
  \brief       De-initialize I2S Interface.
  \return      \ref execution_status
*/
static int32_t I2S3_Uninitialize (void)
{
	return I2S_Uninitialize (&I2S3);
}

/**
  \fn          int32_t I2S3_PowerControl (ARM_POWER_STATE state)
  \brief       Control I2S Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t I2S3_PowerControl (ARM_POWER_STATE state)
{
	return I2S_PowerControl (state, &I2S3);
}

/**
  \fn          int32_t I2S3_Send (const void *data, uint32_t num)
  \brief       Start sending data to I2S transmitter.
  \param[in]   data  Pointer to buffer with data to send to I2S transmitter
  \param[in]   num   Number of data items to send
  \return      \ref execution_status
*/
static int32_t I2S3_Send (const void *data, uint32_t num)
{
	return I2S_Send (data, num, &I2S3);
}

/**
  \fn          int32_t I2S3_Receive (void *data, uint32_t num)
  \brief       Start receiving data from I2S receiver.
  \param[out]  data  Pointer to buffer for data to receive from I2S receiver
  \param[in]   num   Number of data items to receive
  \return      \ref execution_status
*/
static int32_t I2S3_Receive (void *data, uint32_t num)
{
	return I2S_Receive (data, num, &I2S3);
}

/**
  \fn          uint32_t I2S3_GetTxCount (void)
  \brief       Get transmitted data count.
  \return      number of data items transmitted
*/
static uint32_t I2S3_GetTxCount (void)
{
	return I2S_GetTxCount (&I2S3);
}

/**
  \fn          uint32_t I2S3_GetRxCount (void)
  \brief       Get received data count.
  \return      number of data items received
*/
static uint32_t I2S3_GetRxCount (void)
{
	return I2S_GetRxCount (&I2S3);
}

/**
  \fn          int32_t I2S3_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
  \brief       Control I2S Interface.
  \param[in]   control  Operation
  \param[in]   arg1     Argument 1 of operation (optional)
  \param[in]   arg2     Argument 2 of operation (optional)
  \return      common \ref execution_status and driver specific \ref sai_execution_status
*/
static int32_t I2S3_Control (uint32_t control, uint32_t arg1, uint32_t arg2)
{
	return I2S_Control (control, arg1, arg2, &I2S3);
}

/**
  \fn          ARM_SAI_STATUS I2S3_GetStatus (void)
  \brief       Get I2S status.
  \return      SAI status \ref ARM_SAI_STATUS
*/
static ARM_SAI_STATUS I2S3_GetStatus (void)
{
	return I2S_GetStatus (&I2S3);
}

/**
  \fn          void  I2S3_IRQHandler (void)
  \brief       Run the IRQ Handler for I2S3
*/
void I2S3_IRQHandler (void)
{
	I2S_IRQHandler (&I2S3);
}

/**
  \fn          void  I2S3_DMACallback (uint32_t event, int8_t peri_num)
  \param[in]   event     Event from DMA
  \param[in]   peri_num  Peripheral number
  \brief       Callback function from DMA for I2S3
*/
void I2S3_DMACallback (uint32_t event, int8_t peri_num)
{
	I2S_DMACallback (event, peri_num, &I2S3);
}

/**
\brief Access structure of the I2S3 Driver.
*/
extern \
ARM_DRIVER_SAI Driver_SAI3;
ARM_DRIVER_SAI Driver_SAI3 = {
    I2S_GetVersion,
    I2S_GetCapabilities,
    I2S3_Initialize,
    I2S3_Uninitialize,
    I2S3_PowerControl,
    I2S3_Send,
    I2S3_Receive,
    I2S3_GetTxCount,
    I2S3_GetRxCount,
    I2S3_Control,
    I2S3_GetStatus
};
#endif //RTE_I2S3
