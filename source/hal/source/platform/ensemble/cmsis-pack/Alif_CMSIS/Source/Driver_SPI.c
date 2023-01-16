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
 * @file     Driver_SPI.c
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     14-Jun-2021
 * @brief    CMSIS-Driver for SPI.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "SPI_ll_drv.h"

#ifdef RTE_Drivers_SPI_MultiSlave
#include "SPI_MultiSlave.h"
#include "SPI_MultiSlave_Config.h"
#endif

#if !((RTE_SPI0) || (RTE_SPI1) || (RTE_SPI2) || (RTE_SPI3))
#error "SPI is not enabled in the RTE_Device.h"
#endif

#if !defined(RTE_Drivers_SPI)
#error "SPI is not enabled in the RTE_Components.h"
#endif

#define ARM_SPI_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_SPI_API_VERSION,
    ARM_SPI_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES DriverCapabilities = {
    1, /* Simplex Mode (Master and Slave) */
    1, /* TI Synchronous Serial Interface */
    1, /* Microwire Interface */
    0  /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
};

/**
 * @fn      ARM_DRIVER_VERSION ARM_SPI_GetVersion(void)
 * @brief   get spi version
 * @note    none
 * @param   none
 * @retval  driver version
 */
__STATIC_INLINE ARM_DRIVER_VERSION ARM_SPI_GetVersion(void)
{
    return DriverVersion;
}

/**
 * @fn      ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities(void)
 * @brief   get spi capabilities
 * @note    none
 * @param   none
 * @retval  driver capabilities
 */
__STATIC_INLINE ARM_SPI_CAPABILITIES ARM_SPI_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
 * @fn      void SPI_IRQHandler(SPI_RESOURCES *SPI)
 * @brief   Spi interrupt handler
 * @note    none
 * @param   SPI : Pointer to spi resources structure
 * @retval  none
 */
static void SPI_IRQHandler(SPI_RESOURCES *SPI)
{
    SPI_ll_IRQHandler(SPI);
}

/**
 * @fn      int32_t ARM_SPI_Initialize(SPI_RESOURCES *SPI, ARM_SPI_SignalEvent_t cb_event).
 * @brief   Initialize the Spi for communication.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   cb_event : Pointer to user callback function.
 * @retval  \ref execution_status
 */
static int32_t ARM_SPI_Initialize(SPI_RESOURCES *SPI, ARM_SPI_SignalEvent_t cb_event)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance already Initialized */
    if (SPI->flag.initialized == 1)
    {
        return ARM_DRIVER_OK;
    }

    /* Not supporting the poling mode, call back is mandatory. */
    if (cb_event == NULL)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating the tx fifo configuration */
    if ((SPI->tx_fifo_threshold > SPI_TX_FIFO_DEPTH) || (SPI->tx_fifo_start_level > SPI_TX_FIFO_DEPTH))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating the rx fifo configuration */
    if (SPI->rx_fifo_threshold > SPI_RX_FIFO_DEPTH)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Loading call back function for global pointer. */
    SPI->cb_event = cb_event;

    /* Loading default value Tx and RX resource. */
    SPI->tx_buff            = NULL;
    SPI->rx_buff            = NULL;

    SPI->tx_default_buff    = 0;
    SPI->tx_default_enable  = 0;

    SPI->total_cnt          = 0;
    SPI->tx_current_cnt     = 0;
    SPI->rx_current_cnt     = 0;

    /* Mask all interrupt */
    SPI_ll_MaskAllInterrupt(SPI);

    /* Updating the initialize status */
    SPI->flag.initialized = 1;

    return ret;
}

/**
 * @fn      int32_t ARM_SPI_Uninitialize(SPI_RESOURCES *SPI).
 * @brief   Un-Initialize the Spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref execution_status
 */
static int32_t ARM_SPI_Uninitialize(SPI_RESOURCES *SPI)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance already Un-initialized */
    if (SPI->flag.initialized == 0)
    {
        return ARM_DRIVER_OK;
    }

    /* Validating Instance still powered */
    if (SPI->flag.powered == 1)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Loading default value Call back pointer */
    SPI->cb_event = NULL;

    /* Loading default value Tx and Rx resource. */
    SPI->tx_buff            = NULL;
    SPI->rx_buff            = NULL;

    SPI->tx_default_buff    = 0;
    SPI->tx_default_enable  = 0;


    SPI->total_cnt          = 0;
    SPI->tx_current_cnt     = 0;
    SPI->rx_current_cnt     = 0;

    /* Un-Initialize Hardware */
    ret = SPI_ll_Uninitialize(SPI);

    /* Updating the Un-initialize status */
    SPI->flag.initialized = 0;

    return ret;
}

/**
 * @fn      int32_t ARM_SPI_PowerControl(SPI_RESOURCES *SPI, ARM_POWER_STATE state).
 * @brief   Handles the spi power.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   state : power state.
 * @retval  \ref execution_status
 */
static int32_t ARM_SPI_PowerControl(SPI_RESOURCES *SPI, ARM_POWER_STATE state)
{
    /* Validating Instance initialized to power Up/Off */
    if (SPI->flag.initialized == 0)
    {
        return ARM_DRIVER_ERROR;
    }

    switch (state)
    {
        case ARM_POWER_OFF:
        {
            /* Validating Instance already powered off*/
            if (SPI->flag.powered == 0)
            {
                return ARM_DRIVER_OK;
            }

            SPI_ll_Irq_Disable(SPI);

            /* Updating Instance powered off status */
            SPI->flag.powered = 0;

            break;
        }

        case ARM_POWER_FULL:
        {
            /* Validating Instance already powered Full*/
            if( SPI->flag.powered == 1)
            {
                return ARM_DRIVER_OK;
            }

            SPI_ll_Irq_Enable(SPI);

            /* Updating Instance powered Full status */
            SPI->flag.powered = 1;

            break;
        }

        case ARM_POWER_LOW:
        default :
        {
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        }
    }
    return ARM_DRIVER_OK;
}

/**
 * @fn      int32_t ARM_SPI_Send(SPI_RESOURCES *SPI, const void *data, uint32_t num).
 * @brief   Used to send through spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   data : Pointer to the data to send.
 * @param   num : Number of data byte to send.
 * @retval  \ref execution_status
 */
static int32_t ARM_SPI_Send(SPI_RESOURCES *SPI, const void *data, uint32_t num)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance initialized and powered */
    if ((SPI->flag.initialized == 0) || (SPI->flag.powered == 0))
    {
        return ARM_DRIVER_ERROR;
    }

    /* Validating function Argument */
    if ((SPI->tx_default_enable == 0) && (data == NULL))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating function Argument */
    if (num == 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Checking the driver busy status */
    if (SPI->status.busy)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Calling hardware configuration for SPI receive */
    ret = SPI_ll_Send(SPI, data, num);

    return ret;
}

/**
 * @fn      int32_t ARM_SPI_Receive(SPI_RESOURCES *SPI, void *data, uint32_t num).
 * @brief   Used to receive data through spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   data : Pointer to the data received.
 * @param   num : Number of data byte to receive.
 * @retval  \ref execution_status
 */
static int32_t ARM_SPI_Receive(SPI_RESOURCES *SPI, void *data, uint32_t num)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance initialized and powered */
    if ((SPI->flag.initialized == 0) || (SPI->flag.powered == 0))
    {
        return ARM_DRIVER_ERROR;
    }

    /* Validating function Argument */
    if ((data == NULL) || (num == 0))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Checking the driver busy status */
    if (SPI->status.busy)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Calling hardware configuration for SPI receive */
    ret = SPI_ll_Receive(SPI, data, num);

    return ret;
}

/**
 * @fn      int32_t ARM_SPI_Transfer(SPI_RESOURCES *SPI, const void *data_out, void *data_in, uint32_t num).
 * @brief   Used to Transfer and Receive data through spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   data_out : Pointer to the data send.
 * @param   data_in : Pointer to the data received.
 * @param   num : Number of data byte to transfer.
 * @retval  \ref execution_status
 */
static int32_t ARM_SPI_Transfer(SPI_RESOURCES *SPI, const void *data_out, void *data_in, uint32_t num)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance initialized and powered */
    if ((SPI->flag.initialized == 0) || (SPI->flag.powered == 0))
    {
        return ARM_DRIVER_ERROR;
    }

    /* If default value transfer is enabled, data_out NUll is also OK */
    if ((SPI->tx_default_enable == 0) && (data_out == NULL))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating function Argument */
    if ((data_out == NULL) || (data_in == NULL) || (num == 0))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Checking the driver busy status */
    if (SPI->status.busy)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Calling hardware configuration for SPI transfer */
    ret = SPI_ll_Transfer(SPI, data_out, data_in, num);

    return ret;
}

/**
 * @fn      int32_t ARM_SPI_GetDataCount(SPI_RESOURCES *SPI).
 * @brief   Used to get the data count on spi data transferring modes.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref data count
 */
static uint32_t ARM_SPI_GetDataCount(SPI_RESOURCES *SPI)
{
    uint32_t count_value;
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;

    /* SPI send count */
    if((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_SEND_ONLY)
    {
        count_value = SPI->tx_current_cnt;
    }

    /* SPI receive count */
    if((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_RECEIVE_ONLY)
    {
        count_value = SPI->rx_current_cnt;
    }

    /* SPI transfer count : take either tx current count value or rx current count value */
    if((reg_ptr->ctrlr0 & SPI_CTRLR0_TMOD_MASK) == SPI_CTRLR0_TMOD_TRANSFER)
    {
        count_value = SPI->rx_current_cnt;
    }

    return count_value;
}

/**
 * @fn      int32_t ARM_SPI_Control(SPI_RESOURCES *SPI, uint32_t control, uint32_t arg).
 * @brief   Used to configure spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   control : control code.
 * @param   arg : argument.
 * @retval  \ref execution_status
 */
static int32_t ARM_SPI_Control(SPI_RESOURCES *SPI, uint32_t control, uint32_t arg)
{
    SPI_RegInfo *reg_ptr = (SPI_RegInfo*)SPI->reg_base;
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance initialized and powered */
    if ((SPI->flag.initialized == 0) || (SPI->flag.powered == 0))
    {
        return ARM_DRIVER_ERROR;
    }

    reg_ptr->ctrlr0 = 0;

    switch (control & ARM_SPI_CONTROL_Msk)
    {
        /* SPI Inactive */
        case ARM_SPI_MODE_INACTIVE:
        {
            if( control == 0)
            {
                /* Disable the SPI core clock */
                SPI_ll_CoreClock(SPI, 0);

                /* Disable the SPI */
                SPI_ll_Disable(SPI);

                return ARM_DRIVER_OK;
            }
            break;
        }

        /* SPI Master (Output on MOSI, Input on MISO); arg = Bus Speed in bps */
        case ARM_SPI_MODE_MASTER:
        {
            SPI_ll_SetMasterMode(SPI);

            ret = SPI_ll_SetBusSpeed(SPI, arg);
            break;
        }

        /* SPI Slave  (Output on MISO, Input on MOSI) */
        case ARM_SPI_MODE_SLAVE:
        {
            SPI_ll_SetSlaveMode(SPI);

            ret = SPI_ll_SetBusSpeed(SPI, arg);
            break;
        }

        /* SPI Master (Output/Input on MOSI); arg = Bus Speed in bps */
        case ARM_SPI_MODE_MASTER_SIMPLEX:
        {
            //TODO: Implementation is pending
            break;
        }

        /* SPI Slave  (Output/Input on MISO) */
        case ARM_SPI_SET_BUS_SPEED:
        {
            ret = SPI_ll_SetBusSpeed(SPI, arg);
            return ARM_DRIVER_OK;
        }

        /* Get Bus Speed in bps */
        case ARM_SPI_GET_BUS_SPEED:
        {
            return (int32_t)SPI_ll_GetBusSpeed(SPI);
        }

        /* Set the default transmission value */
        case ARM_SPI_SET_DEFAULT_TX_VALUE:
        {
            SPI->tx_default_buff = arg;
            SPI->tx_default_enable = 1;

            return ARM_DRIVER_OK;
        }

        /* Control the Slave Select signal */
        case ARM_SPI_CONTROL_SS:
        {
            if (SPI->sw_slave_select == 1)
            {
                SPI_ll_Control_SlaveSelect(SPI, SPI->chip_selection_pin, arg);
                return ARM_DRIVER_OK;
            }
            return ARM_DRIVER_ERROR;
        }

        /* Abort the current data transfer */
        case ARM_SPI_ABORT_TRANSFER:
        {
            /* Loading default value Tx and Rxresource. */
            SPI->tx_buff            = NULL;
            SPI->rx_buff            = NULL;


            SPI->tx_default_buff    = 0;
            SPI->tx_default_enable  = 0;


            SPI->total_cnt          = 0;
            SPI->tx_current_cnt     = 0;
            SPI->rx_current_cnt     = 0;
            SPI->status.busy        = 0;

            /* Masking all Interrupts */
            SPI_ll_MaskAllInterrupt(SPI);

            /* Disable SPI */
            SPI_ll_Disable(SPI);

            /* Enable SPI */
            SPI_ll_Enable(SPI);

            return ARM_DRIVER_OK;
        }

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_SPI_FRAME_FORMAT_Msk)
    {
        /* SPI Mode configuration */
        case ARM_SPI_CPOL0_CPHA0:
        case ARM_SPI_CPOL0_CPHA1:
        case ARM_SPI_CPOL1_CPHA0:
        case ARM_SPI_CPOL1_CPHA1:
        {
            ret = SPI_ll_SetSpiMode(SPI, ((control & ARM_SPI_FRAME_FORMAT_Msk) >> ARM_SPI_FRAME_FORMAT_Pos));
            break;
        }

        /* Texas Instruments Frame Format */
        case ARM_SPI_TI_SSI:
        {
            ret = SPI_ll_SetTiMode(SPI);
            break;
        }

        /* National Microwire Frame Format */
        case ARM_SPI_MICROWIRE:
        {
            ret = SPI_ll_SetMicroWireMode(SPI);
            break;
        }

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* Configure frame size */
    if (control & ARM_SPI_DATA_BITS_Msk)
    {
        ret = SPI_ll_SetDataFrameSize(SPI, ((control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos));

        if (ret != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }

    switch (control & ARM_SPI_BIT_ORDER_Msk)
    {
        /* SPI Bit order from MSB to LSB (default) */
        case ARM_SPI_MSB_LSB:
        {
            break;
        }
        /* SPI Bit order from LSB to MSB */
        case ARM_SPI_LSB_MSB:
        {
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        }
    }

    switch (control & ARM_SPI_SS_MASTER_MODE_Msk)
    {
        /* SPI Slave Select when Master: Not used (default) */
        case ARM_SPI_SS_MASTER_UNUSED:
        {
            SPI_ll_Control_SlaveSelect(SPI, 0XF, 0);
            break;
        }

        /* SPI Slave Select when Master: Software controlled */
        case ARM_SPI_SS_MASTER_SW:
        {
            if((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_MODE_MASTER )
            {
                SPI->sw_slave_select = 1;
                return ARM_DRIVER_OK;
            }
            return ARM_DRIVER_ERROR;
        }

        /* SPI Slave Select when Master: Hardware controlled Output */
        case ARM_SPI_SS_MASTER_HW_OUTPUT:
        {
            /* This is the default state in the IP, No need to configure */
            return ARM_DRIVER_OK;
        }

        /* SPI Slave Select when Master: Hardware monitored Input */
        case ARM_SPI_SS_MASTER_HW_INPUT:
        {
            //TODO: Need to Implement
            break;
        }
    }

    switch (control & ARM_SPI_SS_SLAVE_MODE_Msk)
    {
        /* SPI Slave Select when Slave: Hardware monitored (default) */
        case ARM_SPI_SS_SLAVE_HW:
        {
            //TODO: Need to Implement
            break;
        }

        /* SPI Slave Select when Slave: Software controlled */
        case ARM_SPI_SS_SLAVE_SW:
        {
            //TODO: Need to Implement
            break;
        }
    }
    return ret;
}

/**
 * @fn      int32_t ARM_SPI_Control_SlaveSelect(SPI_RESOURCES *SPI, uint32_t device, uint32_t ss_state).
 * @brief   Used to configure spi.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @param   device : each bit represent chip selection line.
 * @param   ss_state : Set to active or inactive.
 * @retval  \ref execution_status
 */
static int32_t ARM_SPI_Control_SlaveSelect(SPI_RESOURCES *SPI, uint32_t device, uint32_t ss_state)
{
    /* Validating the instance initialized and powered */
    if ((SPI->flag.initialized == 0) || (SPI->flag.powered == 0))
    {
        return ARM_DRIVER_ERROR;
    }

    /* Validating the function parameters */
    if ((device & SPI_SLAVE_SELECT_PIN_MASK) || (ss_state > ARM_SPI_SS_ACTIVE))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating the SPI driver status */
    if (SPI->status.busy == 1)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    return SPI_ll_Control_SlaveSelect(SPI, device, ss_state);
}

/**
 * @fn      ARM_SPI_STATUS ARM_SPI_GetStatus(SPI_RESOURCES *SPI)
 * @brief   Used to get spi status.
 * @note    none.
 * @param   SPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
__STATIC_INLINE ARM_SPI_STATUS ARM_SPI_GetStatus(SPI_RESOURCES *SPI)
{
    return SPI->status;
}

/* SPI0 driver instance */
#if RTE_SPI0
SPI_RESOURCES SPI0 = {
    .reg_base               = (SPI_RegInfo*) SPI0_BASE,
    .cb_event               = NULL,
    .irq_priority           = RTE_SPI0_IRQ_PRIORITY,
    .drv_instance           = SPI_INSTANCE_0,
    .chip_selection_pin     = RTE_SPI0_CHIP_SELECTION_PIN,
    .spi_frf                = RTE_SPI0_SPI_FRAME_FORMAT,
    .tx_fifo_threshold      = RTE_SPI0_TX_FIFO_THRESHOLD,
    .tx_fifo_start_level    = RTE_SPI0_TX_FIFO_LEVEL_TO_START_TRANSFER,
    .rx_fifo_threshold      = RTE_SPI0_RX_FIFO_THRESHOLD
};

extern void SPI0_IRQHandler(void);
void SPI0_IRQHandler(void)
{
    SPI_IRQHandler(&SPI0);
}

static int32_t ARM_SPI0_Initialize(ARM_SPI_SignalEvent_t cb_event)
{
    return ARM_SPI_Initialize(&SPI0, cb_event);
}

static int32_t ARM_SPI0_Uninitialize(void)
{
    return ARM_SPI_Uninitialize(&SPI0);
}

static int32_t ARM_SPI0_PowerControl(ARM_POWER_STATE state)
{
    return ARM_SPI_PowerControl(&SPI0, state);
}

static int32_t ARM_SPI0_Send(const void *data, uint32_t num)
{
    return ARM_SPI_Send(&SPI0, data, num);
}

static int32_t ARM_SPI0_Receive(void *data, uint32_t num)
{
    return ARM_SPI_Receive(&SPI0, data, num);
}

static int32_t ARM_SPI0_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_SPI_Transfer(&SPI0, data_out, data_in, num);
}

static uint32_t ARM_SPI0_GetDataCount(void)
{
    return ARM_SPI_GetDataCount(&SPI0);
}

static int32_t ARM_SPI0_Control(uint32_t control, uint32_t arg)
{
    return ARM_SPI_Control(&SPI0, control, arg);
}

static ARM_SPI_STATUS ARM_SPI0_GetStatus(void)
{
    return ARM_SPI_GetStatus(&SPI0);
}

#ifdef RTE_Drivers_SPI_MultiSlave
    #if SPI_DRIVER == 0
    static int32_t ARM_SPI0_Control_SlaveSelect(uint32_t device, uint32_t ss_state)
    {
        return ARM_SPI_Control_SlaveSelect(&SPI0, device, ss_state);
    }
    #endif
#endif

extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI Driver_SPI0 = {
    ARM_SPI_GetVersion,
    ARM_SPI_GetCapabilities,
    ARM_SPI0_Initialize,
    ARM_SPI0_Uninitialize,
    ARM_SPI0_PowerControl,
    ARM_SPI0_Send,
    ARM_SPI0_Receive,
    ARM_SPI0_Transfer,
    ARM_SPI0_GetDataCount,
    ARM_SPI0_Control,
    ARM_SPI0_GetStatus
};
#endif /* RTE_SPI0 */

/* SPI1 driver instance */
#if RTE_SPI1
SPI_RESOURCES SPI1 = {
    .reg_base               = (SPI_RegInfo*) SPI1_BASE,
    .cb_event               = NULL,
    .irq_priority           = RTE_SPI1_IRQ_PRIORITY,
    .drv_instance           = SPI_INSTANCE_1,
    .chip_selection_pin     = RTE_SPI1_CHIP_SELECTION_PIN,
    .spi_frf                = RTE_SPI1_SPI_FRAME_FORMAT,
    .tx_fifo_threshold      = RTE_SPI1_TX_FIFO_THRESHOLD,
    .tx_fifo_start_level    = RTE_SPI1_TX_FIFO_LEVEL_TO_START_TRANSFER,
    .rx_fifo_threshold      = RTE_SPI1_RX_FIFO_THRESHOLD
};

extern void SPI1_IRQHandler(void);
void SPI1_IRQHandler(void)
{
    SPI_IRQHandler(&SPI1);
}

static int32_t ARM_SPI1_Initialize(ARM_SPI_SignalEvent_t cb_event)
{
    return ARM_SPI_Initialize(&SPI1, cb_event);
}

static int32_t ARM_SPI1_Uninitialize(void)
{
    return ARM_SPI_Uninitialize(&SPI1);
}

static int32_t ARM_SPI1_PowerControl(ARM_POWER_STATE state)
{
    return ARM_SPI_PowerControl(&SPI1, state);
}

static int32_t ARM_SPI1_Send(const void *data, uint32_t num)
{
    return ARM_SPI_Send(&SPI1, data, num);
}

static int32_t ARM_SPI1_Receive(void *data, uint32_t num)
{
    return ARM_SPI_Receive(&SPI1, data, num);
}

static int32_t ARM_SPI1_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_SPI_Transfer(&SPI1, data_out, data_in, num);
}

static uint32_t ARM_SPI1_GetDataCount(void)
{
    return ARM_SPI_GetDataCount(&SPI1);
}

static int32_t ARM_SPI1_Control(uint32_t control, uint32_t arg)
{
    return ARM_SPI_Control(&SPI1, control, arg);
}

static ARM_SPI_STATUS ARM_SPI1_GetStatus(void)
{
    return ARM_SPI_GetStatus(&SPI1);
}

#ifdef RTE_Drivers_SPI_MultiSlave
    #if SPI_DRIVER == 1
    static int32_t ARM_SPI1_Control_SlaveSelect(uint32_t device, uint32_t ss_state)
    {
        return ARM_SPI_Control_SlaveSelect(&SPI1, device, ss_state);
    }
    #endif
#endif

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI Driver_SPI1 = {
    ARM_SPI_GetVersion,
    ARM_SPI_GetCapabilities,
    ARM_SPI1_Initialize,
    ARM_SPI1_Uninitialize,
    ARM_SPI1_PowerControl,
    ARM_SPI1_Send,
    ARM_SPI1_Receive,
    ARM_SPI1_Transfer,
    ARM_SPI1_GetDataCount,
    ARM_SPI1_Control,
    ARM_SPI1_GetStatus
};
#endif /* RTE_SPI1 */

/* SPI2 driver instance */
#if RTE_SPI2
SPI_RESOURCES SPI2 = {
    .reg_base               = (SPI_RegInfo*) SPI2_BASE,
    .cb_event               = NULL,
    .irq_priority           = RTE_SPI2_IRQ_PRIORITY,
    .drv_instance           = SPI_INSTANCE_2,
    .chip_selection_pin     = RTE_SPI2_CHIP_SELECTION_PIN,
    .spi_frf                = RTE_SPI2_SPI_FRAME_FORMAT,
    .tx_fifo_threshold      = RTE_SPI2_TX_FIFO_THRESHOLD,
    .tx_fifo_start_level    = RTE_SPI2_TX_FIFO_LEVEL_TO_START_TRANSFER,
    .rx_fifo_threshold      = RTE_SPI2_RX_FIFO_THRESHOLD
};

extern void SPI2_IRQHandler(void);
void SPI2_IRQHandler(void)
{
    SPI_IRQHandler(&SPI2);
}

static int32_t ARM_SPI2_Initialize(ARM_SPI_SignalEvent_t cb_event)
{
    return ARM_SPI_Initialize(&SPI2, cb_event);
}

static int32_t ARM_SPI2_Uninitialize(void)
{
    return ARM_SPI_Uninitialize(&SPI2);
}

static int32_t ARM_SPI2_PowerControl(ARM_POWER_STATE state)
{
    return ARM_SPI_PowerControl(&SPI2, state);
}

static int32_t ARM_SPI2_Send(const void *data, uint32_t num)
{
    return ARM_SPI_Send(&SPI2, data, num);
}

static int32_t ARM_SPI2_Receive(void *data, uint32_t num)
{
    return ARM_SPI_Receive(&SPI2, data, num);
}

static int32_t ARM_SPI2_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_SPI_Transfer(&SPI2, data_out, data_in, num);
}

static uint32_t ARM_SPI2_GetDataCount(void)
{
    return ARM_SPI_GetDataCount(&SPI2);
}

static int32_t ARM_SPI2_Control(uint32_t control, uint32_t arg)
{
    return ARM_SPI_Control(&SPI2, control, arg);
}

static ARM_SPI_STATUS ARM_SPI2_GetStatus(void)
{
    return ARM_SPI_GetStatus(&SPI2);
}

#ifdef RTE_Drivers_SPI_MultiSlave
    #if SPI_DRIVER == 2
    int32_t ARM_SPI2_Control_SlaveSelect(uint32_t device, uint32_t ss_state)
    {
        return ARM_SPI_Control_SlaveSelect(&SPI2, device, ss_state);
    }
    #endif
#endif

extern ARM_DRIVER_SPI Driver_SPI2;
ARM_DRIVER_SPI Driver_SPI2 = {
    ARM_SPI_GetVersion,
    ARM_SPI_GetCapabilities,
    ARM_SPI2_Initialize,
    ARM_SPI2_Uninitialize,
    ARM_SPI2_PowerControl,
    ARM_SPI2_Send,
    ARM_SPI2_Receive,
    ARM_SPI2_Transfer,
    ARM_SPI2_GetDataCount,
    ARM_SPI2_Control,
    ARM_SPI2_GetStatus
};
#endif /* RTE_SPI2 */

/* SPI3 driver instance */
#if RTE_SPI3
SPI_RESOURCES SPI3 = {
    .reg_base               = (SPI_RegInfo*) SPI3_BASE,
    .cb_event               = NULL,
    .irq_priority           = RTE_SPI3_IRQ_PRIORITY,
    .drv_instance           = SPI_INSTANCE_3,
    .chip_selection_pin     = RTE_SPI3_CHIP_SELECTION_PIN,
    .spi_frf                = RTE_SPI3_SPI_FRAME_FORMAT,
    .tx_fifo_threshold      = RTE_SPI3_TX_FIFO_THRESHOLD,
    .tx_fifo_start_level    = RTE_SPI3_TX_FIFO_LEVEL_TO_START_TRANSFER,
    .rx_fifo_threshold      = RTE_SPI3_RX_FIFO_THRESHOLD
};

extern void SPI3_IRQHandler(void);
void SPI3_IRQHandler(void)
{
    SPI_IRQHandler(&SPI3);
}

static int32_t ARM_SPI3_Initialize(ARM_SPI_SignalEvent_t cb_event)
{
    return ARM_SPI_Initialize(&SPI3, cb_event);
}

static int32_t ARM_SPI3_Uninitialize(void)
{
    return ARM_SPI_Uninitialize(&SPI3);
}

static int32_t ARM_SPI3_PowerControl(ARM_POWER_STATE state)
{
    return ARM_SPI_PowerControl(&SPI3, state);
}

static int32_t ARM_SPI3_Send(const void *data, uint32_t num)
{
    return ARM_SPI_Send(&SPI3, data, num);
}

static int32_t ARM_SPI3_Receive(void *data, uint32_t num)
{
    return ARM_SPI_Receive(&SPI3, data, num);
}

static int32_t ARM_SPI3_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_SPI_Transfer(&SPI3, data_out, data_in, num);
}

static uint32_t ARM_SPI3_GetDataCount(void)
{
    return ARM_SPI_GetDataCount(&SPI3);
}

static int32_t ARM_SPI3_Control(uint32_t control, uint32_t arg)
{
    return ARM_SPI_Control(&SPI3, control, arg);
}

static ARM_SPI_STATUS ARM_SPI3_GetStatus(void)
{
    return ARM_SPI_GetStatus(&SPI3);
}

#ifdef RTE_Drivers_SPI_MultiSlave
    #if SPI_DRIVER == 3
    static int32_t ARM_SPI3_Control_SlaveSelect(uint32_t device, uint32_t ss_state)
    {
        return ARM_SPI_Control_SlaveSelect(&SPI3, device, ss_state);
    }
    #endif
#endif

extern ARM_DRIVER_SPI Driver_SPI3;
ARM_DRIVER_SPI Driver_SPI3 = {
    ARM_SPI_GetVersion,
    ARM_SPI_GetCapabilities,
    ARM_SPI3_Initialize,
    ARM_SPI3_Uninitialize,
    ARM_SPI3_PowerControl,
    ARM_SPI3_Send,
    ARM_SPI3_Receive,
    ARM_SPI3_Transfer,
    ARM_SPI3_GetDataCount,
    ARM_SPI3_Control,
    ARM_SPI3_GetStatus
};

#ifdef RTE_Drivers_SPI_MultiSlave
void SPI_Control_SlaveSelect(uint32_t device, uint32_t ss_state)
{
    #if SPI_DRIVER == 0
       ARM_SPI0_Control_SlaveSelect(device, ss_state);
    #elif SPI_DRIVER == 1
       ARM_SPI1_Control_SlaveSelect(device, ss_state);
    #elif SPI_DRIVER == 2
       ARM_SPI2_Control_SlaveSelect(device, ss_state);
    #elif SPI_DRIVER == 3
       ARM_SPI3_Control_SlaveSelect(device, ss_state);
    #endif
}
#endif
#endif /* RTE_SPI3 */
