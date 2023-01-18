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
 * @file     SPI_ll_drv.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     14-Jun-2021
 * @brief    Low level SPI driver.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "SPI_ll_drv.h"

static uint32_t getSpiCoreClock(void)
{
    return AHB_CLOCK;
}

/**
 * @fn      int32_t SPI_ll_CoreClock(SPI_RESOURCES *SPI, bool option).
 * @brief   Low level function call used to enable or disable spi core clock.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   option : enable/disable.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_CoreClock(SPI_RESOURCES *SPI, bool option)
{
    uint32_t *ptr = (uint32_t*) CFGMST0_SSI;

    switch (SPI->drv_instance)
    {
        case SPI_INSTANCE_0:
        {
            if (option)
            {
                *ptr |= (CR_SSI_SS_IN_VAL_0 | CR_SSI_SS_IN_SEL_0);
            }
            else {
                *ptr &= ~(CR_SSI_SS_IN_VAL_0 | CR_SSI_SS_IN_SEL_0);
            }
            break;
        }
        case SPI_INSTANCE_1:
        {
            if (option)
            {
                *ptr |= (CR_SSI_SS_IN_VAL_1 | CR_SSI_SS_IN_SEL_1);
            }
            else {
                *ptr &= ~(CR_SSI_SS_IN_VAL_1 | CR_SSI_SS_IN_SEL_1);
            }
            break;
        }
        case SPI_INSTANCE_2:
        {
            if (option)
            {
                *ptr |= (CR_SSI_SS_IN_VAL_2 | CR_SSI_SS_IN_SEL_2);
            }
            else {
                *ptr &= ~(CR_SSI_SS_IN_VAL_2 | CR_SSI_SS_IN_SEL_2);
            }
            break;
        }
        case SPI_INSTANCE_3:
        {
            if (option)
            {
                *ptr |= (CR_SSI_SS_IN_VAL_3 | CR_SSI_SS_IN_SEL_3);
            }
            else {
                *ptr &= ~(CR_SSI_SS_IN_VAL_3 | CR_SSI_SS_IN_SEL_3);
            }
            break;
        }
    }
    return ARM_DRIVER_OK;
}

/**
 * @fn      void SPI_ll_Disable(SPI_RESOURCES *SPI).
 * @brief   Low level function call used to disable spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  none.
 */
void SPI_ll_Disable(SPI_RESOURCES *SPI)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;
    reg_ptr->ssienr = SPI_DISABLE;
}

/**
 * @fn      void SPI_ll_Enable(SPI_RESOURCES *SPI).
 * @brief   Low level function call used to enable spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  none.
 */
void SPI_ll_Enable(SPI_RESOURCES *SPI)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;
    reg_ptr->ssienr = SPI_ENABLE;
}

/**
 * @fn      int32_t SPI_ll_SetMasterMode(SPI_RESOURCES *SPI)
 * @brief   Low level function call used to set spi on master mode.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_SetMasterMode(SPI_RESOURCES *SPI)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Enable the SPI core clock */
    SPI_ll_CoreClock(SPI, 1);

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    /* Configuring SPI as master */
    reg_ptr->ctrlr0 |= SPI_CTRLR0_SSI_IS_MST_MASTER;

    /* Enable SPI */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_SetSlaveMode(SPI_RESOURCES *SPI)
 * @brief   Low level function call used to set spi on slave mode.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_SetSlaveMode(SPI_RESOURCES *SPI)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Enable the SPI core clock */
    SPI_ll_CoreClock(SPI, 0);

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    /* Configuring SPI as slave */
    reg_ptr->ctrlr0 &= ~(SPI_CTRLR0_SSI_IS_MST_MASTER);

    /* Enable SPI */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_SetBusSpeed(SPI_RESOURCES *SPI, uint32_t arg)
 * @brief   Low level function call used to set spi bus speed.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   arg : spi baud rate.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_SetBusSpeed(SPI_RESOURCES *SPI, uint32_t arg)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;
    uint32_t clk;

    if (arg == 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    clk = getSpiCoreClock();

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    reg_ptr->baudr = (clk / arg);

    /* Enable SPI */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      uint32_t SPI_ll_GetBusSpeed(SPI_RESOURCES *SPI)
 * @brief   Low level function call used to get spi bus speed.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi baud rate.
 */
uint32_t SPI_ll_GetBusSpeed(SPI_RESOURCES *SPI)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    if (reg_ptr->baudr == 0)
    {
        return getSpiCoreClock();
    }
    else
    {
        return (getSpiCoreClock() / reg_ptr->baudr);
    }
}

/**
 * @fn      int32_t SPI_ll_SetSpiMode(SPI_RESOURCES *SPI, SPI_MODE mode)
 * @brief   Low level function call used to configure spi communication mode.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   mode : Communication mode.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_SetSpiMode(SPI_RESOURCES *SPI, SPI_MODE mode)
{
    uint32_t temp =0;
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    switch(mode)
    {
        /* Clock Polarity 0, Clock Phase 0 */
        case SPI_MODE_0:
        {
            temp = reg_ptr->ctrlr0;
            temp &= ~(SPI_CTRLR0_SCPOL_HIGH |SPI_CTRLR0_SCPH_HIGH);
            reg_ptr->ctrlr0 = temp;
            break;
        }

        /* Clock Polarity 0, Clock Phase 1 */
        case SPI_MODE_1:
        {
            temp = reg_ptr->ctrlr0;
            temp &= ~(SPI_CTRLR0_SCPOL_HIGH |SPI_CTRLR0_SCPH_HIGH);
            temp |= (SPI_CTRLR0_SCPOL_LOW |SPI_CTRLR0_SCPH_HIGH);
            reg_ptr->ctrlr0 = temp;
            break;
        }

        /* Clock Polarity 1, Clock Phase 0 */
        case SPI_MODE_2:
        {
            temp = reg_ptr->ctrlr0;
            temp &= ~(SPI_CTRLR0_SCPOL_HIGH |SPI_CTRLR0_SCPH_HIGH);
            temp |= (SPI_CTRLR0_SCPOL_HIGH |SPI_CTRLR0_SCPH_LOW);
            reg_ptr->ctrlr0 = temp;
            break;
        }

        /* Clock Polarity 1, Clock Phase 1 */
        case SPI_MODE_3:
        {
            temp = reg_ptr->ctrlr0;
            temp |= (SPI_CTRLR0_SCPOL_HIGH |SPI_CTRLR0_SCPH_HIGH);
            reg_ptr->ctrlr0 = temp;
            break;
        }
    }

    /* Enable SPI */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_SetTiMode(SPI_RESOURCES *SPI)
 * @brief   Low level function call used to configure spi Texas instrument mode.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_SetTiMode(SPI_RESOURCES *SPI)
{
    uint32_t temp = 0;
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    temp = reg_ptr->ctrlr0;
    temp &= ~(SPI_CTRLR0_FRF_MASK);
    temp |= (SPI_CTRLR0_FRF_TI);
    reg_ptr->ctrlr0 = temp;

    /* Enable SPI */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_SetMicroWireMode(SPI_RESOURCES *SPI)
 * @brief   Low level function call used to configure spi for Micro-Wire mode.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_SetMicroWireMode(SPI_RESOURCES *SPI)
{
    uint32_t temp = 0;
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    temp = reg_ptr->ctrlr0;
    temp &= ~(SPI_CTRLR0_FRF_MASK);
    temp |= (SPI_CTRLR0_FRF_MICROWIRE);
    reg_ptr->ctrlr0 = temp;

    /* Enable SPI */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_SetDataFrameSize(SPI_RESOURCES *SPI, uint8_t size)
 * @brief   Low level function call used to configure spi data frame size.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   size : data frame size.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_SetDataFrameSize(SPI_RESOURCES *SPI, uint8_t size)
{
    uint32_t temp = 0;
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    if ((size < SPI_DATA_FRAME_SIZE_MIN) || (size > SPI_DATA_FRAME_SIZE_MAX))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    temp = reg_ptr->ctrlr0;
    temp &= ~SPI_CTRLR0_DFS_MASK;
    temp |= (size - 1);
    reg_ptr->ctrlr0 = temp;

    /* Enable SPI */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      void SPI_ll_SetTransmissionMode(SPI_RESOURCES *SPI, uint32_t mode)
 * @brief   Low level function call used to configure spi transmission mode.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   mode : transmission mode.
 * @retval  \ref spi driver status.
 */
void SPI_ll_SetTransmissionMode(SPI_RESOURCES *SPI, uint32_t mode)
{
    uint32_t temp = 0;
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    temp = reg_ptr->ctrlr0;
    temp &= ~(SPI_CTRLR0_TMOD_MASK);
    temp |= mode;
    reg_ptr->ctrlr0 = temp;

    /* Enable SPI */
    SPI_ll_Enable(SPI);
}

/**
 * @fn      void SPI_ll_SetSpiFrameFormat(SPI_RESOURCES *SPI).
 * @brief   Low level function call used to configure spi frame format.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  none.
 */
void SPI_ll_SetSpiFrameFormat(SPI_RESOURCES *SPI)
{
    uint32_t temp = 0;
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Disable SPI to configure */
    SPI_ll_Disable(SPI);

    switch (SPI->spi_frf)
    {
        case SPI_SPI_FRAME_FORMAT_STANDARD:
        {
            temp = reg_ptr->ctrlr0;
            temp &= ~(SPI_CTRLR0_SPI_FRF_MASK);
            reg_ptr->ctrlr0 = temp;
            break;
        }
        case SPI_SPI_FRAME_FORMAT_DUAL:
        {
            temp = reg_ptr->ctrlr0;
            temp &= ~(SPI_CTRLR0_SPI_FRF_MASK);
            temp |= SPI_CTRLR0_SPI_FRF_DUAL;
            reg_ptr->ctrlr0 = temp;
            break;
        }
        case SPI_SPI_FRAME_FORMAT_QUAD:
        {
            temp = reg_ptr->ctrlr0;
            temp &= ~(SPI_CTRLR0_SPI_FRF_MASK);
            temp |= SPI_CTRLR0_SPI_FRF_QUAD;
            reg_ptr->ctrlr0 = temp;
            break;
        }
        case SPI_SPI_FRAME_FORMAT_OCTAL:
        {
            temp = reg_ptr->ctrlr0;
            temp &= ~(SPI_CTRLR0_SPI_FRF_MASK);
            temp |= SPI_CTRLR0_SPI_FRF_OCTAL;
            reg_ptr->ctrlr0 = temp;
            break;
        }
    }

    /* Enable SPI */
    SPI_ll_Enable(SPI);
}

/**
 * @fn      int32_t SPI_ll_Irq_Enable(SPI_RESOURCES *SPI)
 * @brief   low level function call used to configure NVIC for spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_Irq_Enable(SPI_RESOURCES *SPI)
{
    NVIC_ClearPendingIRQ(SPI_IRQ_NUM(SPI->drv_instance));

    NVIC_SetPriority(SPI_IRQ_NUM(SPI->drv_instance), SPI->irq_priority);

    NVIC_EnableIRQ(SPI_IRQ_NUM(SPI->drv_instance));

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_Irq_Disable(SPI_RESOURCES *SPI)
 * @brief   low level function call used to de-configure NVIC for spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_Irq_Disable(SPI_RESOURCES *SPI)
{
    NVIC_ClearPendingIRQ(SPI_IRQ_NUM(SPI->drv_instance));

    NVIC_DisableIRQ(SPI_IRQ_NUM(SPI->drv_instance));

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_MaskAllInterrupt(SPI_RESOURCES *SPI)
 * @brief   Low level function call used to mask all interrupt.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_MaskAllInterrupt(SPI_RESOURCES *SPI)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Mask all Interrupt */
    reg_ptr->imr = 0;

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_Uninitialize(SPI_RESOURCES *SPI)
 * @brief   Low level function call used to un-initialize spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_Uninitialize(SPI_RESOURCES *SPI)
{
    int32_t ret;

    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* Disable the SPI clock on AHB */
    ret = SPI_ll_CoreClock(SPI, 0);

    /* Mask all Interrupt */
    reg_ptr->imr = 0;

    return ret;
}

/**
 * @fn      int32_t SPI_ll_Control_SlaveSelect(SPI_RESOURCES *SPI, uint32_t device, uint32_t ss_state)
 * @brief   low level function call Used to configure slave select for spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   device : each bit represent chip selection line.
 * @param   ss_state : Set to active or inactive.
 * @retval  \ref spi driver status.
 */

int32_t SPI_ll_Control_SlaveSelect(SPI_RESOURCES *SPI, uint32_t device, uint32_t ss_state)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    SPI_ll_Disable(SPI);

    if (ss_state)
    {
        reg_ptr->ser |= (1 << device);
    }
    else
    {
        reg_ptr->ser &= ~ (1 << device);
    }

    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      void SPI_ll_IRQHandler(SPI_RESOURCES *SPI)
 * @brief   Low level interrupt handler for spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
void SPI_ll_IRQHandler(SPI_RESOURCES *SPI)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    uint32_t event, tx_data, index;
    uint8_t tx_count, rx_count, frame_size;

    event = reg_ptr->isr;

    /* Transfer Empty event */
    if (event & SPI_TX_FIFO_EMPTY_EVENT)
    {
        /* Reading the SPI frame size */
        frame_size = (SPI_CTRLR0_DFS_MASK & reg_ptr->ctrlr0);

        /* Calculating data count to transfer */
        if (SPI->total_cnt >= (SPI->tx_current_cnt + SPI_TX_FIFO_DEPTH))
        {
            tx_count = SPI_TX_FIFO_DEPTH;
        }
        else
        {
            tx_count = (SPI->total_cnt - SPI->tx_current_cnt);
        }

        for (index = 0; index < tx_count; index++)
        {
            tx_data = 0;

            if (SPI->tx_buff == 0)
            {
                /* Checking the default buffer transmit is enabled */
                if (SPI->tx_default_enable == 1)
                {
                    tx_data = SPI->tx_default_buff;
                }
            }
            else
            {
                /* Computing the data to send */
                if (frame_size > 15)
                {
                    tx_data = (uint32_t)(SPI->tx_buff[0] | (SPI->tx_buff[1] << 8) | (SPI->tx_buff[2] << 16) | (SPI->tx_buff[3] << 24));
                    SPI->tx_buff = (SPI->tx_buff+4U);
                }
                else if(frame_size > 7)
                {
                    tx_data = (uint32_t)(SPI->tx_buff[0] | (SPI->tx_buff[1] << 8));
                    SPI->tx_buff = (SPI->tx_buff+2);
                }
                else
                {
                    tx_data = SPI->tx_buff[0];
                    SPI->tx_buff = (SPI->tx_buff+1);
                }
            }

            /* Loading send data to SPI tx fifo*/
            reg_ptr->data_reg = tx_data;

            /* Incrementing transmit data count */
            SPI->tx_current_cnt++;
        }
    }

    /* Receive Full event */
    if (event & SPI_RX_FIFO_FULL_EVENT)
    {
        rx_count = reg_ptr->rxflr;
        frame_size = (SPI_CTRLR0_DFS_MASK & reg_ptr->ctrlr0);

        if (frame_size > 15)
        {
            for (index = 0; index < rx_count; index++)
            {
                *((uint32_t *) SPI->rx_buff) = reg_ptr->data_reg;

                SPI->rx_buff += sizeof(uint32_t);
                SPI->rx_current_cnt++;
            }
        }
        else if (frame_size > 7)
        {
            for (index = 0; index < rx_count; index++)
            {
                *((uint16_t *) SPI->rx_buff) = (uint16_t) (reg_ptr->data_reg);

                SPI->rx_buff += sizeof(uint16_t);
                SPI->rx_current_cnt++;
            }
        }
        else
        {
            for (index = 0; index < rx_count; index++)
            {
                *((uint8_t *) SPI->rx_buff) = (uint8_t) (reg_ptr->data_reg);

                SPI->rx_buff += sizeof(uint8_t);
                SPI->rx_current_cnt++;
            }
        }
    }

    if (event & (SPI_RX_FIFO_OVER_FLOW_EVENT | SPI_TX_FIFO_OVER_FLOW_EVENT))
    {
        /* Disabling and Enabling the SPI will Reset the FIFO */

        /* Disable SPI instance */
        SPI_ll_Disable(SPI);

        /* Enable SPI instance */
        SPI_ll_Enable(SPI);

        /* Updating the data lost status */
        SPI->status.data_lost = 1;

        /* calling the call back function */
        SPI->cb_event(ARM_SPI_EVENT_DATA_LOST);
    }

    if (event & SPI_MULTI_MASTER_CONTENTION_EVENT)
    {
        /* Updating the data lost status */
        SPI->status.data_lost = 1;

        /* calling the call back function */
        SPI->cb_event(ARM_SPI_EVENT_MODE_FAULT);
    }

    if (event & SPI_RX_FIFO_UNDER_FLOW_EVENT)
    {
        /* Rx buffer empty, if still try to access generate this interrupt */

        /* calling the call back function */
        SPI->cb_event(ARM_SPI_EVENT_MODE_FAULT);
    }

    /* SEND ONLY mode : if transmit finished */
    if (((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_SEND_ONLY) && (SPI->total_cnt <= SPI->tx_current_cnt))
    {
        /* waiting for Transfer to complete */
        if ((reg_ptr->sr & 1) == 0)
        {
            /* Masking the TX interrupts */
            reg_ptr->imr &= ~(SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK |
                             SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK |
                             SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

            /* Loading counts to default value */
            SPI->tx_current_cnt = 0;

            /* Updating the driver status*/
            SPI->status.busy = 0;

            /* calling the call back function */
            SPI->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
        }
    }

    /* RECEIVE ONLY mode : if receive finished */
    if (((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_RECEIVE_ONLY) && (SPI->total_cnt <= SPI->rx_current_cnt))
    {
        /* loading default values */
        SPI->rx_current_cnt = 0;

        /* updating the receiving finished status */
        SPI->status.busy = 0;

        /* Mask all Interrupts */
        reg_ptr->imr = 0;

        /* calling the call back function */
        SPI->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
    }

    /* TRANSFER mode : if transfer finished */
    if (((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_TRANSFER) &&
        ((SPI->total_cnt <= SPI->tx_current_cnt) && (SPI->total_cnt <= SPI->rx_current_cnt)))
    {
         /* waiting for Transfer to complete */
        if ((reg_ptr->sr & 1) == 0)
        {
            SPI->tx_current_cnt = 0;
            SPI->rx_current_cnt = 0;

             /* Updating the driver status */
            SPI->status.busy = 0;

            /* Mask all Interrupts */
            reg_ptr->imr = 0;

             /* calling the call back function */
            SPI->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
        }
    }

    /* Clear SPI interrupt status */
    (void) reg_ptr->txoicr;
    (void) reg_ptr->rxoicr;
    (void) reg_ptr->rxuicr;
    (void) reg_ptr->msticr;
    (void) reg_ptr->icr;
}

/**
 * @fn      int32_t SPI_ll_Send(SPI_RESOURCES *SPI, const void *data, uint32_t num)
 * @brief   low level function call Used to send the data from SPI.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   data : Pointer to data to send.
 * @param   num : number of data to send.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_Send(SPI_RESOURCES *SPI, const void *data, uint32_t num)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    SPI->tx_buff        = (uint8_t *) data;
    SPI->total_cnt      = num;
    SPI->tx_current_cnt = 0;

    /* Set transmission mode for send and receive */
    SPI_ll_SetTransmissionMode(SPI, SPI_CTRLR0_TMOD_SEND_ONLY);

    /* Disable SPI instance */
    SPI_ll_Disable(SPI);

    reg_ptr->ctrlr0 |= SPI_CTRLR0_SSTE_ENABLE;

    /* Set Tx fifo threshold + Transfer start fifo level values */
    reg_ptr->txftlr = (uint32_t)((SPI->tx_fifo_start_level << 16U) | SPI->tx_fifo_threshold);

    /* Enable the SPI interrupts for Send */
    reg_ptr->imr = (SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK |
                    SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK |
                    SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

    /* Updating the driver Busy status */
    SPI->status.busy = 1;

    /* Enable the SPI instance */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_Receive(SPI_RESOURCES *SPI, void *data, uint32_t num)
 * @brief   low level function call Used receive data from spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   data : Pointer to data to be receive.
 * @param   num : number of data to receive.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_Receive(SPI_RESOURCES *SPI, void *data, uint32_t num)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    SPI->rx_buff        = (uint32_t *) data;
    SPI->total_cnt      = num;
    SPI->tx_buff        = 0;
    SPI->rx_current_cnt = 0;
    SPI->tx_current_cnt = 0;

    /* Set transmission mode as read only */
    SPI_ll_SetTransmissionMode(SPI, SPI_CTRLR0_TMOD_RECEIVE_ONLY);

    /* Disable SPI instance */
    SPI_ll_Disable(SPI);

    reg_ptr->ctrlr0 |= SPI_CTRLR0_SSTE_ENABLE;

    /* Set Rx fifo threshold values */
    reg_ptr->rxftlr = SPI->rx_fifo_threshold;

    /* Enable the SPI interrupts for Receive */
    reg_ptr->imr = (SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK |
                    SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK |
                    SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK |
                    SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK |
                    SPI_IMR_RX_FIFO_FULL_INTERRUPT_MASK |
                    SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

    /* Updating the driver Busy status */
    SPI->status.busy = 1;

    /* Enable the SPI instance */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t SPI_ll_Receive(SPI_RESOURCES *SPI, const void *data_out, void *data_in, uint32_t num)
 * @brief   Low level function call used Transmit and Receive the data from spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   data_out : Pointer to data to be send.
 * @param   data_in : Pointer to data to be receive.
 * @param   num : number of data to transmit.
 * @retval  \ref spi driver status.
 */
int32_t SPI_ll_Transfer(SPI_RESOURCES *SPI, const void *data_out, void *data_in, uint32_t num)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    SPI->tx_buff            = (uint8_t *) data_out;
    SPI->rx_buff            = (uint32_t *) data_in;
    SPI->total_cnt          = num;
    SPI->tx_current_cnt     = 0;
    SPI->rx_current_cnt     = 0;

    /* Set transmission mode for send and receive */
    SPI_ll_SetTransmissionMode(SPI, SPI_CTRLR0_TMOD_TRANSFER);

    /* Disable SPI instance */
    SPI_ll_Disable(SPI);

    reg_ptr->ctrlr0 |= SPI_CTRLR0_SSTE_ENABLE;

    /* Set Tx fifo threshold + Transfer start fifo level values */
    reg_ptr->txftlr = (uint32_t)((SPI->tx_fifo_start_level << 16U) | SPI->tx_fifo_threshold);

    /* Set Rx fifo threshold values */
    reg_ptr->rxftlr = SPI->rx_fifo_threshold;

    /* Enable the SPI interrupts for Transfer */
    reg_ptr->imr = (SPI_IMR_TX_FIFO_EMPTY_INTERRUPT_MASK |
                    SPI_IMR_TX_FIFO_OVER_FLOW_INTERRUPT_MASK |
                    SPI_IMR_RX_FIFO_UNDER_FLOW_INTERRUPT_MASK |
                    SPI_IMR_RX_FIFO_OVER_FLOW_INTERRUPT_MASK |
                    SPI_IMR_RX_FIFO_FULL_INTERRUPT_MASK |
                    SPI_IMR_MULTI_MASTER_CONTENTION_INTERRUPT_MASK);

    /* Updating the driver Busy status */
    SPI->status.busy = 1;

    /* Enable the SPI instance */
    SPI_ll_Enable(SPI);

    return ARM_DRIVER_OK;
}
