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
 * @file     OSPI_ll_drv.c
 * @author   Khushboo Singh
 * @email    khushboo.singh@alifsemi.com
 * @version  V1.0.0
 * @date     21-Oct-2022
 * @brief    Low level OSPI driver derived from SPI_ll_drv.c.
 * @bug      None
 * @Note     None
 ******************************************************************************/

#include "OSPI_ll_drv.h"
#include "clk.h"

static uint32_t getOSpiCoreClock(void)
{
    return AXI_CLOCK;
}

/**
 * @fn      void OSPI_ll_Disable(OSPI_RESOURCES *OSPI).
 * @brief   Low level function call used to disable spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  none.
 */
void OSPI_ll_Disable(OSPI_RESOURCES *OSPI)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;
    reg_ptr->ssienr = OSPI_DISABLE;
}

/**
 * @fn      void OSPI_ll_Enable(OSPI_RESOURCES *OSPI).
 * @brief   Low level function call used to enable spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  none.
 **/
void OSPI_ll_Enable(OSPI_RESOURCES *OSPI)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;
    reg_ptr->ssienr = OSPI_ENABLE;
}

/**
 * @fn      int32_t OSPI_ll_SetMasterMode(OSPI_RESOURCES *OSPI)
 * @brief   Low level function call used to set spi on master mode.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_SetMasterMode(OSPI_RESOURCES *OSPI)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    OSPI_ll_Disable(OSPI);

    reg_ptr->ctrlr0 |= SPI_CTRLR0_SSI_IS_MST_MASTER;

    OSPI_ll_Enable(OSPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_SetBusSpeed(OSPI_RESOURCES *OSPI, uint32_t arg)
 * @brief   Low level function call used to set spi bus speed.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   arg : spi baud rate.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_SetBusSpeed(OSPI_RESOURCES *OSPI, uint32_t arg)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;
    uint32_t clk;

    if (arg == 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    clk = getOSpiCoreClock();

    OSPI_ll_Disable(OSPI);

    reg_ptr->baudr = clk / arg;

    OSPI_ll_Enable(OSPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      uint32_t OSPI_ll_GetBusSpeed(OSPI_RESOURCES *OSPI)
 * @brief   Low level function call used to get spi bus speed.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref spi baud rate.
 */
uint32_t OSPI_ll_GetBusSpeed(OSPI_RESOURCES *OSPI)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    if (reg_ptr->baudr == 0)
    {
        return 0;
    }
    else
    {
        return (getOSpiCoreClock() / reg_ptr->baudr);
    }
}

/**
 * @fn      int32_t OSPI_ll_SetSpiMode(OSPI_RESOURCES *OSPI, OSPI_MODE mode)
 * @brief   Low level function call used to configure spi communication mode.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   mode : Communication mode.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_SetSpiMode(OSPI_RESOURCES *OSPI, SPI_MODE mode)
{
    uint32_t val =0;
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    OSPI_ll_Disable(OSPI);

    switch(mode)
    {
        /* Clock Polarity 0, Clock Phase 0 */
        case SPI_MODE_0:
        {
            val = reg_ptr->ctrlr0;
            val &= ~(SPI_CTRLR0_SCPOL_HIGH | SPI_CTRLR0_SCPH_HIGH);
            reg_ptr->ctrlr0 = val;
            break;
        }

        /* Clock Polarity 0, Clock Phase 1 */
        case SPI_MODE_1:
        {
            val = reg_ptr->ctrlr0;
            val &= ~(SPI_CTRLR0_SCPOL_HIGH | SPI_CTRLR0_SCPH_HIGH);
            val |= (SPI_CTRLR0_SCPOL_LOW |SPI_CTRLR0_SCPH_HIGH);
            reg_ptr->ctrlr0 = val;
            break;
        }

        /* Clock Polarity 1, Clock Phase 0 */
        case SPI_MODE_2:
        {
            val = reg_ptr->ctrlr0;
            val &= ~(SPI_CTRLR0_SCPOL_HIGH | SPI_CTRLR0_SCPH_HIGH);
            val |= (SPI_CTRLR0_SCPOL_HIGH |SPI_CTRLR0_SCPH_LOW);
            reg_ptr->ctrlr0 = val;
            break;
        }

        /* Clock Polarity 1, Clock Phase 1 */
        case SPI_MODE_3:
        {
            val = reg_ptr->ctrlr0;
            val |= (SPI_CTRLR0_SCPOL_HIGH | SPI_CTRLR0_SCPH_HIGH);
            reg_ptr->ctrlr0 = val;
            break;
        }
    }

    OSPI_ll_Enable(OSPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_SetDataFrameSize(OSPI_RESOURCES *OSPI, uint8_t size)
 * @brief   Low level function call used to configure spi data frame size.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   size : data frame size.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_SetDataFrameSize(OSPI_RESOURCES *OSPI, uint8_t size)
{
    uint32_t val = 0;
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    if ((size < OSPI_DATA_FRAME_SIZE_MIN) || (size > OSPI_DATA_FRAME_SIZE_MAX))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    OSPI_ll_Disable(OSPI);

    val = reg_ptr->ctrlr0;
    val &= ~SPI_CTRLR0_DFS_MASK;
    val |= (size - 1);
    reg_ptr->ctrlr0 = val;

    OSPI_ll_Enable(OSPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      void OSPI_ll_SetTransmissionMode(OSPI_RESOURCES *OSPI, uint32_t mode)
 * @brief   Low level function call used to configure spi transmission mode.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   mode : transmission mode.
 * @retval  \ref spi driver status.
 */
void OSPI_ll_SetTransmissionMode(OSPI_RESOURCES *OSPI, uint32_t mode)
{
    uint32_t val = 0;
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    OSPI_ll_Disable(OSPI);

    val = reg_ptr->ctrlr0;
    val &= ~(SPI_CTRLR0_TMOD_MASK);
    val |= mode;
    reg_ptr->ctrlr0 = val;

    OSPI_ll_Enable(OSPI);
}

/**
 * @fn      void OSPI_ll_SetSpiFrameFormat(OSPI_RESOURCES *OSPI).
 * @brief   Low level function call used to configure spi frame format.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   arg : Frame format
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_SetSpiFrameFormat(OSPI_RESOURCES *OSPI, uint32_t arg)
{
    OSPI->spi_frf = arg;
    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_SetSpiDDR(OSPI_RESOURCES *OSPI, uint32_t arg)
 * @brief   Low level function call used to configure DDR feature.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   arg : DDR enable/ disable
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_SetSpiDDR(OSPI_RESOURCES *OSPI, uint32_t arg)
{
    OSPI->ddr = arg;
    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_SetAddrLength(OSPI_RESOURCES *OSPI, uint32_t arg).
 * @brief   Low level function call used to configure address length.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   arg : Address length
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_SetAddrLength(OSPI_RESOURCES *OSPI, uint32_t arg)
{
    OSPI->addr_len = 0xF & arg;
    OSPI->dummy_cycle = 0xFF & (arg >> ARM_OSPI_WAIT_CYCLE_POS);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_Irq_Enable(OSPI_RESOURCES *OSPI)
 * @brief   low level function call used to configure NVIC for spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_Irq_Enable(OSPI_RESOURCES *OSPI)
{
    NVIC_ClearPendingIRQ(OSPI->irq);

    NVIC_SetPriority(OSPI->irq, OSPI->irq_priority);

    NVIC_EnableIRQ(OSPI->irq);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_Irq_Disable(OSPI_RESOURCES *OSPI)
 * @brief   low level function call used to de-configure NVIC for spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_Irq_Disable(OSPI_RESOURCES *OSPI)
{
    NVIC_ClearPendingIRQ(OSPI->irq);

    NVIC_DisableIRQ(OSPI->irq);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_MaskAllInterrupt(OSPI_RESOURCES *OSPI)
 * @brief   Low level function call used to mask all interrupt.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_MaskAllInterrupt(OSPI_RESOURCES *OSPI)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    /* Mask all Interrupts */
    reg_ptr->imr = 0;

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_Uninitialize(OSPI_RESOURCES *OSPI)
 * @brief   Low level function call used to un-initialize spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_Uninitialize(OSPI_RESOURCES *OSPI)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    /* Mask all Interrupts */
    reg_ptr->imr = 0;

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_Control_SlaveSelect(OSPI_RESOURCES *OSPI, uint32_t device, uint32_t ss_state)
 * @brief   low level function call Used to configure slave select for spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   device : each bit represent chip selection line.
 * @param   ss_state : Set to active or inactive.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_Control_SlaveSelect(OSPI_RESOURCES *OSPI, uint32_t device, uint32_t ss_state)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    OSPI_ll_Disable(OSPI);

    if (ss_state)
    {
        reg_ptr->ser |= 1 << device;
    }
    else
    {
        reg_ptr->ser &= ~(1 << device);
    }

    OSPI_ll_Enable(OSPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      void OSPI_ll_IRQHandler(OSPI_RESOURCES *OSPI)
 * @brief   Low level interrupt handler for OSPI.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  none.
 */
void OSPI_ll_IRQHandler(OSPI_RESOURCES *OSPI)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;

    uint32_t event, tx_data, index, rx_count, tx_count;
    uint16_t frame_size;

    event = reg_ptr->isr;

    if (event & SPI_TX_FIFO_EMPTY_EVENT)
    {
        frame_size = (SPI_CTRLR0_DFS_MASK & reg_ptr->ctrlr0);

        /* Calculate data count to transfer */
        if (OSPI->tx_total_cnt >= (OSPI->tx_current_cnt + OSPI_TX_FIFO_DEPTH))
        {
            tx_count = (uint16_t) OSPI_TX_FIFO_DEPTH;
        }
        else
        {
            tx_count = (OSPI->tx_total_cnt - OSPI->tx_current_cnt);
        }

        reg_ptr->txftlr &= ~(0xFFU << SPI_TXFTLR_TXFTHR_POS);
        reg_ptr->txftlr |= ((tx_count - 1U) << SPI_TXFTLR_TXFTHR_POS);

        for (index = 0; index < tx_count; index++)
        {
            tx_data = 0;

            if (OSPI->tx_buff == 0)
            {
                /* Check if the default buffer transmit is enabled */
                if (OSPI->tx_default_enable == 1)
                {
                    tx_data = OSPI->tx_default_buff;
                }
            }
            else
            {
                tx_data = OSPI->tx_buff[0];
                OSPI->tx_buff = (OSPI->tx_buff + 1);
            }
            /* Load the data to tx FIFO*/
            reg_ptr->data_reg = tx_data;

            OSPI->tx_current_cnt++;
        }
    }

    if (event & SPI_RX_FIFO_FULL_EVENT)
    {
        frame_size = (SPI_CTRLR0_DFS_MASK & reg_ptr->ctrlr0);

        rx_count = reg_ptr->rxflr;

        if (frame_size > SPI_CTRLR0_DFS_16bit)
        {
            for (index = 0; index < rx_count; index++)
            {
                *((uint32_t *) OSPI->rx_buff) = reg_ptr->data_reg;

                OSPI->rx_buff += sizeof(uint32_t);
                OSPI->rx_current_cnt++;
            }
        }
        else if (frame_size > SPI_CTRLR0_DFS_8bit)
        {
            for (index = 0; index < rx_count; index++)
            {
                *((uint16_t *) OSPI->rx_buff) = (uint16_t) (reg_ptr->data_reg);

                OSPI->rx_buff += sizeof(uint16_t);
                OSPI->rx_current_cnt++;
            }
        }
        else
        {
            for (index = 0; index < rx_count; index++)
            {
                *((uint8_t *) OSPI->rx_buff) = (uint8_t) (reg_ptr->data_reg);

                OSPI->rx_buff += sizeof(uint8_t);

                OSPI->rx_current_cnt++;
            }
        }
    }

    if (event & (SPI_RX_FIFO_OVER_FLOW_EVENT | SPI_TX_FIFO_OVER_FLOW_EVENT))
    {
        /* Disabling and Enabling the OSPI will Reset the FIFO */

        OSPI_ll_Disable(OSPI);

        OSPI_ll_Enable(OSPI);

        OSPI->status.data_lost = 1;

        OSPI->cb_event(ARM_OSPI_EVENT_DATA_LOST);
    }

    if (event & SPI_MULTI_MASTER_CONTENTION_EVENT)
    {
        OSPI->status.data_lost = 1;

        OSPI->cb_event(ARM_OSPI_EVENT_MODE_FAULT);
    }

    /* Rx FIFO was accessed when it was empty */
    if (event & SPI_RX_FIFO_UNDER_FLOW_EVENT)
    {
    	reg_ptr->imr &= ~(SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK);

        OSPI->cb_event(ARM_OSPI_EVENT_MODE_FAULT);
    }

    /* SEND ONLY mode : if transmit finished */
    if (((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_SEND_ONLY) &&
           (OSPI->tx_total_cnt == OSPI->tx_current_cnt))
    {
        /* Wait for the transfer to complete */
        if((reg_ptr->sr & (SSI_BUSY_FLAG | TX_FIFO_EMPTY)) == TX_FIFO_EMPTY)
        {
            /* Mask the TX interrupts */
            reg_ptr->imr &= ~(SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK |
                                SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK |
                                SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

            OSPI->status.busy = 0;

            OSPI->cb_event(ARM_OSPI_EVENT_TRANSFER_COMPLETE);
        }
    }
    /* RECEIVE ONLY mode : check if the transfer is finished */
    if (((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_RECEIVE_ONLY) &&
             (OSPI->rx_total_cnt <= OSPI->rx_current_cnt))
    {
        /* Mask the RX interrupts */
        reg_ptr->imr &= ~(SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK |
                                SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK |
                                SPI_IMR_RX_FIFO_FULL_INTERRUPT_MASK |
                                SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

        OSPI->status.busy = 0;

        OSPI->cb_event(ARM_OSPI_EVENT_TRANSFER_COMPLETE);
    }

    if(((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_TRANSFER) &&
              (OSPI->rx_total_cnt == (OSPI->rx_current_cnt)))
    {
    	/* Mask all the interrupts */
        reg_ptr->imr = 0;

         OSPI->status.busy = 0;

         OSPI->cb_event(ARM_OSPI_EVENT_TRANSFER_COMPLETE);
    }

    if(((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_TRANSFER) && (OSPI->tx_total_cnt == OSPI->tx_current_cnt))
    {
        if((reg_ptr->sr & TX_FIFO_EMPTY) == TX_FIFO_EMPTY)
        {
            OSPI->status.busy = 0;

            /* Mask the TX interrupts */
            reg_ptr->imr &= ~(SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK
                             | SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK
                             | SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);
        }
    }

    /* Read interrupt clear registers */
    (void) reg_ptr->txoicr;
    (void) reg_ptr->msticr;
    (void) reg_ptr->rxoicr;
    (void) reg_ptr->rxuicr;
    (void) reg_ptr->icr;
}

/**
 * @fn      int32_t OSPI_ll_OSPI_RESOURCES *OSPI, const void *data, uint32_t num)
 * @brief   low level function call Used to send the data from OSPI.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   data : Pointer to data to send.
 * @param   num : number of data to send.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_Send(OSPI_RESOURCES *OSPI, const void *data, uint32_t num)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;
    uint32_t reg_val;

    OSPI->tx_buff           = data;
    OSPI->tx_total_cnt      = num;
    OSPI->tx_current_cnt    = 0;

    OSPI->status.busy = 1;
    OSPI->mode = SPI_CTRLR0_TMOD_SEND_ONLY;

    OSPI_ll_Disable(OSPI);

    reg_ptr->txftlr = OSPI->tx_fifo_threshold;

    reg_val = reg_ptr->ctrlr0;

    reg_val &= ~(SPI_CTRLR0_SPI_FRF_MASK | (SPI_CTRLR0_TMOD_MASK | SPI_CTRLR0_SSTE_MASK));
    reg_val |= ((OSPI->spi_frf << SPI_CTRLR0_SPI_FRF) | SPI_CTRLR0_TMOD_SEND_ONLY);

    reg_ptr->ctrlr0 = reg_val;

    reg_ptr->ctrlr1 = 0;

    reg_val = (SPI_TRANS_TYPE_FRF_DEFINED
              | SPI_CTRLR0_SPI_RXDS_DISABLE << SPI_CTRLR0_SPI_RXDS_EN_OFFSET)
              | (OSPI->ddr << SPI_CTRLR0_SPI_DDR_EN_OFFSET)
              | (SPI_CTRLR0_INST_L_8bit << SPI_CTRLR0_INST_L_OFFSET)
              | (OSPI->addr_len << SPI_CTRLR0_ADDR_L_OFFSET)
              | (OSPI->dummy_cycle << SPI_CTRLR0_WAIT_CYCLES_OFFSET);

    reg_ptr->spi_ctrlr0 = reg_val;

    /* Enable the OSPI interrupts for Transfer */
    reg_ptr->imr = (SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK
                   | SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK
                   | SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

    OSPI_ll_Enable(OSPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_Receive(OSPI_RESOURCES *OSPI, void *data, uint32_t num)
 * @brief   low level function call Used receive data from spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   data : Pointer to data to be receive.
 * @param   num : number of data to receive.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_Receive(OSPI_RESOURCES *OSPI, void *data, uint32_t num)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;
    uint32_t reg_val;

    OSPI->tx_buff            = 0;
    OSPI->rx_buff            = (uint8_t *) data;
    OSPI->tx_total_cnt       = 0;
    OSPI->rx_current_cnt     = 0;
    OSPI->tx_current_cnt     = 0;

    OSPI->status.busy = 1;
    OSPI->mode = SPI_CTRLR0_TMOD_RECEIVE_ONLY;

    OSPI_ll_Disable(OSPI);

    reg_ptr->rxftlr = OSPI->rx_fifo_threshold;

    reg_val = reg_ptr->ctrlr0;

    reg_val &= ~(SPI_CTRLR0_SPI_FRF_MASK | (SPI_CTRLR0_TMOD_MASK | SPI_CTRLR0_SSTE_MASK));
    reg_val |= ((OSPI->spi_frf << SPI_CTRLR0_SPI_FRF) | SPI_CTRLR0_TMOD_RECEIVE_ONLY);

    reg_ptr->ctrlr0 = reg_val;

    OSPI->reg_base->ctrlr1 = num - 1;

    reg_val = SPI_TRANS_TYPE_FRF_DEFINED
              | (SPI_CTRLR0_SPI_RXDS_DISABLE << SPI_CTRLR0_SPI_RXDS_EN_OFFSET)
              | (OSPI->ddr << SPI_CTRLR0_SPI_DDR_EN_OFFSET)
              | (SPI_CTRLR0_INST_L_0bit << SPI_CTRLR0_INST_L_OFFSET)
              | (OSPI->addr_len << SPI_CTRLR0_ADDR_L_OFFSET)
              | (OSPI->dummy_cycle << SPI_CTRLR0_WAIT_CYCLES_OFFSET);

    OSPI->reg_base->spi_ctrlr0 = reg_val;

    /* Enable the SPI interrupts for Receive */
    reg_ptr->imr = (SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK
                   | SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK
                   | SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK
                   | SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK
                   | SPI_IMR_RX_FIFO_FULL_INTERRUPT_MASK
                   | SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

    OSPI_ll_Enable(OSPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t OSPI_ll_Receive(OSPI_RESOURCES *OSPI, const void *data_out, void *data_in, uint32_t num)
 * @brief   Low level function call used Transmit and Receive the data from spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   data_out : Pointer to data to be send.
 * @param   data_in : Pointer to data to be receive.
 * @param   num : number of data to transfer.
 * @retval  \ref spi driver status.
 */
int32_t OSPI_ll_Transfer(OSPI_RESOURCES *OSPI, const void *data_out, void *data_in, uint32_t num)
{
    OSPI_RegInfo *reg_ptr = (OSPI_RegInfo*) OSPI->reg_base;
    uint32_t reg_val;

    OSPI->tx_buff = data_out;
    OSPI->rx_buff = data_in;

    /* Tx total count based on address length */
    if(OSPI->addr_len == ARM_OSPI_ADDR_LENGTH_0_BITS)
        OSPI->tx_total_cnt = 1;
    else if(OSPI->addr_len == ARM_OSPI_ADDR_LENGTH_24_BITS)
        OSPI->tx_total_cnt = 4;
    else if(OSPI->addr_len == ARM_OSPI_ADDR_LENGTH_32_BITS)
        OSPI->tx_total_cnt = 2;

    OSPI->tx_current_cnt = 0;
    OSPI->rx_current_cnt = 0;
    OSPI->rx_total_cnt   = num;

    OSPI->status.busy = 1;
    OSPI->mode = SPI_CTRLR0_TMOD_TRANSFER;

    OSPI_ll_Disable(OSPI);

    reg_ptr->rxftlr = OSPI->rx_fifo_threshold;
    reg_ptr->txftlr = OSPI->tx_fifo_threshold;

    reg_val = reg_ptr->ctrlr0;

    reg_val &= ~(SPI_CTRLR0_SPI_FRF_MASK | (SPI_CTRLR0_TMOD_MASK | SPI_CTRLR0_SSTE_MASK ));
    reg_val |= ((OSPI->spi_frf << SPI_CTRLR0_SPI_FRF) | SPI_CTRLR0_TMOD_TRANSFER);

    reg_ptr->ctrlr0 = reg_val;

    OSPI->reg_base->ctrlr1 = num - 1;

    reg_val = SPI_TRANS_TYPE_FRF_DEFINED
              | (SPI_CTRLR0_SPI_RXDS_DISABLE << SPI_CTRLR0_SPI_RXDS_EN_OFFSET)
              | (OSPI->ddr << SPI_CTRLR0_SPI_DDR_EN_OFFSET)
              | (SPI_CTRLR0_INST_L_8bit << SPI_CTRLR0_INST_L_OFFSET)
              | (OSPI->addr_len << SPI_CTRLR0_ADDR_L_OFFSET)
              | (OSPI->dummy_cycle << SPI_CTRLR0_WAIT_CYCLES_OFFSET);

    OSPI->reg_base->spi_ctrlr0 = reg_val;

    /* Enable the OSPI interrupts for Transfer */
    reg_ptr->imr = (SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK
                   | SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK
                   | SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK
                   | SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK
                   | SPI_IMR_RX_FIFO_FULL_INTERRUPT_MASK
                   | SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

    OSPI_ll_Enable(OSPI);

    return ARM_DRIVER_OK;
}
