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
 * @file     Driver_OSPI.c
 * @author   Khushboo Singh
 * @email    khushboo.singh@alifsemi.com
 * @version  V1.0.0
 * @date     21-Oct-2022
 * @brief    CMSIS-Driver for OSPI derived from CMSIS SPI driver.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#include "OSPI_ll_drv.h"

#if !( (RTE_OSPI0) || (RTE_OSPI1) )
#error "OSPI is not enabled in the RTE_Device.h"
#endif

#if !defined(RTE_Drivers_OSPI)
#error "OSPI is not enabled in the RTE_Components.h"
#endif

#define ARM_OSPI_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0) /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_OSPI_API_VERSION,
    ARM_OSPI_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_OSPI_CAPABILITIES DriverCapabilities = {
    0, /* Simplex Mode (Master and Slave) Deprecated: Reserved (must be zero)*/
    0, /* TI Synchronous Serial Interface */
    0, /* Microwire Interface */
    0  /* Signal Mode Fault event: \ref ARM_OSPI_EVENT_MODE_FAULT */
};

/**
 * @fn      ARM_DRIVER_VERSION ARM_OSPI_GetVersion(void)
 * @brief   get spi version
 * @note    none
 * @param   none
 * @retval  driver version
 */
static ARM_DRIVER_VERSION ARM_OSPI_GetVersion(void)
{
    return DriverVersion;
}

/**
 * @fn      ARM_OSPI_CAPABILITIES ARM_OSPI_GetCapabilities(void)
 * @brief   get spi capabilities
 * @note    none
 * @param   none
 * @retval  driver capabilities
 */
static ARM_OSPI_CAPABILITIES ARM_OSPI_GetCapabilities(void)
{
    return DriverCapabilities;
}

/**
 * @fn      void OSPI_IRQHandler(OSPI_RESOURCES *OSPI)
 * @brief   Spi interrupt handler
 * @note    none
 * @param   OSPI : Pointer to spi resources structure
 * @retval  none
 */
static void OSPI_IRQHandler(OSPI_RESOURCES *OSPI)
{
    OSPI_ll_IRQHandler(OSPI);
}

/**
 * @fn      int32_t ARM_OSPI_Initialize(OSPI_RESOURCES *OSPI, ARM_OSPI_SignalEvent_t cb_event).
 * @brief   Initialize the Spi for communication.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   cb_event : Pointer to user callback function.
 * @retval  \ref execution_status
 */
static int32_t ARM_OSPI_Initialize(OSPI_RESOURCES *OSPI, ARM_OSPI_SignalEvent_t cb_event)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance already Initialized */
    if (OSPI->flag.initialized == 1)
    {
        return ARM_DRIVER_OK;
    }

    /* Not supporting the poling mode, call back is mandatory. */
    if (cb_event == NULL)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating the tx fifo configuration */
    if ((OSPI->tx_fifo_threshold > OSPI_TX_FIFO_DEPTH) || (OSPI->tx_fifo_start_level > OSPI_TX_FIFO_DEPTH))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating the rx fifo configuration */
    if (OSPI->rx_fifo_threshold > OSPI_RX_FIFO_DEPTH)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Loading call back function for global pointer. */
    OSPI->cb_event = cb_event;
    OSPI->addr_len = 0;
    OSPI->dummy_cycle = 0;

    /* Loading default value Tx and RX resource. */
    OSPI->tx_buff            = NULL;
    OSPI->rx_buff            = NULL;

    OSPI->tx_default_buff    = 0;
    OSPI->tx_default_enable  = 0;

    OSPI->tx_total_cnt          = 0;
    OSPI->rx_total_cnt          = 0;
    OSPI->tx_current_cnt        = 0;
    OSPI->rx_current_cnt        = 0;

    /* Mask all interrupt */
    OSPI_ll_MaskAllInterrupt(OSPI);

    /* Updating the initialize status */
    OSPI->flag.initialized = 1;

    return ret;
}

/**
 * @fn      int32_t ARM_OSPI_Uninitialize(OSPI_RESOURCES *OSPI).
 * @brief   Un-Initialize the Spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref execution_status
 */
static int32_t ARM_OSPI_Uninitialize(OSPI_RESOURCES *OSPI)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance already Un-initialized */
    if (OSPI->flag.initialized == 0)
    {
        return ARM_DRIVER_OK;
    }

    /* Validating Instance still powered */
    if (OSPI->flag.powered == 1)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Loading default value Call back pointer */
    OSPI->cb_event              = NULL;

    /* Loading default value Tx and Rx resource. */
    OSPI->tx_buff               = NULL;
    OSPI->rx_buff               = NULL;

    OSPI->tx_default_buff       = 0;
    OSPI->tx_default_enable     = 0;


    OSPI->tx_total_cnt          = 0;
    OSPI->rx_total_cnt          = 0;
    OSPI->tx_current_cnt        = 0;
    OSPI->rx_current_cnt        = 0;

    /* Un-Initialize Hardware */
    ret = OSPI_ll_Uninitialize(OSPI);

    /* Updating the Un-initialize status */
    OSPI->flag.initialized = 0;

    return ret;
}

/**
 * @fn      int32_t ARM_OSPI_PowerControl(OSPI_RESOURCES *OSPI, ARM_POWER_STATE state).
 * @brief   Handles the spi power.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   state : power state.
 * @retval  \ref execution_status
 */
static int32_t ARM_OSPI_PowerControl(OSPI_RESOURCES *OSPI, ARM_POWER_STATE state)
{
    /* Validating Instance initialized to power Up/Off */
    if (OSPI->flag.initialized == 0)
    {
        return ARM_DRIVER_ERROR;
    }

    switch (state)
    {
        case ARM_POWER_OFF:
        {
            /* Validating Instance already powered off*/
            if (OSPI->flag.powered == 0)
            {
                return ARM_DRIVER_OK;
            }

            OSPI_ll_Irq_Disable(OSPI);

            /* Updating Instance powered off status */
            OSPI->flag.powered = 0;

            break;
        }

        case ARM_POWER_FULL:
        {
            /* Validating Instance already powered Full*/
            if( OSPI->flag.powered == 1)
            {
                return ARM_DRIVER_OK;
            }

            OSPI_ll_Irq_Enable(OSPI);

            /* Updating Instance powered Full status */
            OSPI->flag.powered = 1;

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
 * @fn      int32_t ARM_OSPI_Send(OSPI_RESOURCES *OSPI, const void *data, uint32_t num).
 * @brief   Used to send through spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   data : Pointer to the data to send.
 * @param   num : Number of data byte to send.
 * @retval  \ref execution_status
 */
static int32_t ARM_OSPI_Send(OSPI_RESOURCES *OSPI, const void *data, uint32_t num)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance initialized and powered */
    if ((OSPI->flag.initialized == 0) || (OSPI->flag.powered == 0))
    {
        return ARM_DRIVER_ERROR;
    }

    /* Validating function Argument */
    if ((OSPI->tx_default_enable == 0) && (data == NULL))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating function Argument */
    if (num == 0)
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Checking the driver busy status */
    if (OSPI->status.busy)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Calling hardware configuration for OSPI send */
    ret = OSPI_ll_Send(OSPI, data, num);

    return ret;
}

/**
 * @fn      int32_t ARM_OSPI_Receive(OSPI_RESOURCES *OSPI, void *data, uint32_t num).
 * @brief   Used to receive data through spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   data : Pointer to the data received.
 * @param   num : Number of data byte to receive.
 * @retval  \ref execution_status
 */
static int32_t ARM_OSPI_Receive(OSPI_RESOURCES *OSPI, void *data, uint32_t num)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance initialized and powered */
    if ((OSPI->flag.initialized == 0) || (OSPI->flag.powered == 0))
    {
        return ARM_DRIVER_ERROR;
    }

    /* Validating function Argument */
    if ((data == NULL) || (num == 0))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Checking the driver busy status */
    if (OSPI->status.busy)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Calling hardware configuration for OSPI receive */
    ret = OSPI_ll_Receive(OSPI, data, num);

    return ret;
}

/**
 * @fn      int32_t ARM_OSPI_Transfer(OSPI_RESOURCES *OSPI, const void *data_out, void *data_in, uint32_t num).
 * @brief   Used to Transfer and Receive data through spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   data_out : Pointer to the data send.
 * @param   data_in : Pointer to the data received.
 * @param   num : Number of data byte to receive.
 * @retval  \ref execution_status
 */
static int32_t ARM_OSPI_Transfer(OSPI_RESOURCES *OSPI, const void *data_out, void *data_in, uint32_t num)
{
    int32_t ret = ARM_DRIVER_OK;

    /* Validating Instance initialized and powered */
    if ((OSPI->flag.initialized == 0) || (OSPI->flag.powered == 0))
    {
        return ARM_DRIVER_ERROR;
    }

    /* If default value transfer is enabled, data_out NUll is also OK */
    if ((OSPI->tx_default_enable == 0) && (data_out == NULL))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Validating function Argument */
    if ((data_out == NULL) || (data_in == NULL) || (num == 0))
    {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Checking the driver busy status */
    if (OSPI->status.busy)
    {
        return ARM_DRIVER_ERROR_BUSY;
    }

    /* Calling hardware configuration for OSPI transfer */
    ret = OSPI_ll_Transfer(OSPI, data_out, data_in, num);

    return ret;
}

/**
 * @fn      int32_t ARM_OSPI_GetDataCount(OSPI_RESOURCES *OSPI).
 * @brief   Used to get the data count on spi transfer.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref data count
 */
static uint32_t ARM_OSPI_GetDataCount(OSPI_RESOURCES *OSPI)
{
    if(OSPI->mode & SPI_CTRLR0_TMOD_SEND_ONLY)
        return OSPI->tx_current_cnt;
    else
        return OSPI->rx_current_cnt;
}

/**
 * @fn      int32_t ARM_OSPI_Control(OSPI_RESOURCES *OSPI, uint32_t control, uint32_t arg).
 * @brief   Used to configure spi.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @param   control : control code.
 * @param   arg : argument.
 * @retval  \ref execution_status
 */
static int32_t ARM_OSPI_Control(OSPI_RESOURCES *OSPI, uint32_t control, uint32_t arg)
{
    int32_t ret = ARM_DRIVER_OK;

    switch (control & ARM_OSPI_CONTROL_MSK)
    {
        /* OSPI Inactive */
        case ARM_OSPI_MODE_INACTIVE:
        {
            if( control == 0)
            {
                /* Disable the OSPI */
                OSPI_ll_Disable(OSPI);

                return ARM_DRIVER_OK;
            }
            break;
        }

        /* OSPI Master (Output on MOSI, Input on MISO); arg = Bus Speed in bps */
        case ARM_OSPI_MODE_MASTER:
        {
            OSPI_ll_SetMasterMode(OSPI);

            ret = OSPI_ll_SetBusSpeed(OSPI, arg);
            break;
        }

        /* Set Bus speed */
        case ARM_OSPI_SET_BUS_SPEED:
        {
            ret = OSPI_ll_SetBusSpeed(OSPI, arg);
            return ARM_DRIVER_OK;
        }

        /* Get Bus Speed in bps */
        case ARM_OSPI_GET_BUS_SPEED:
        {
            return (int32_t) OSPI_ll_GetBusSpeed(OSPI);
        }

        /* Set the default transmission value */
        case ARM_OSPI_SET_DEFAULT_TX_VALUE:
        {
            OSPI->tx_default_buff = arg;
            OSPI->tx_default_enable = 1;

            return ARM_DRIVER_OK;
        }

        /* Control the Slave Select signal */
        case ARM_OSPI_CONTROL_SS:
        {
            if (OSPI->sw_slave_select == 1)
            {
                OSPI_ll_Control_SlaveSelect(OSPI, OSPI->chip_selection_pin, arg);
                return ARM_DRIVER_OK;
            }
            return ARM_DRIVER_ERROR;
        }

        /* Abort the current data transfer */
        case ARM_OSPI_ABORT_TRANSFER:
        {
            /* Loading default value Tx and Rxresource. */
            OSPI->tx_buff            = NULL;
            OSPI->rx_buff            = NULL;

            OSPI->tx_default_buff    = 0;
            OSPI->tx_default_enable  = 0;

            OSPI->tx_total_cnt          = 0;
            OSPI->rx_total_cnt          = 0;
            OSPI->tx_current_cnt        = 0;
            OSPI->rx_current_cnt        = 0;
            OSPI->status.busy           = 0;

            /* Masking all Interrupts */
            OSPI_ll_MaskAllInterrupt(OSPI);

            /* Disable OSPI */
            OSPI_ll_Disable(OSPI);

            /* Enable OSPI */
            OSPI_ll_Enable(OSPI);

            return ARM_DRIVER_OK;
        }

        case ARM_OSPI_SET_ADDR_LENGTH_WAIT_CYCLE:
        {
            ret = OSPI_ll_SetAddrLength(OSPI, arg);
            break;
        }

        case ARM_OSPI_SET_FRAME_FORMAT:
        {
            ret = OSPI_ll_SetSpiFrameFormat(OSPI, arg);
            break;
        }

        case ARM_OSPI_SET_DDR_MODE:
        {
            ret = OSPI_ll_SetSpiDDR(OSPI, arg);
            break;
        }

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_OSPI_FRAME_FORMAT_MSK)
    {
        /* OSPI Mode configuration */
        case ARM_OSPI_CPOL0_CPHA0:
        case ARM_OSPI_CPOL0_CPHA1:
        case ARM_OSPI_CPOL1_CPHA0:
        case ARM_OSPI_CPOL1_CPHA1:
        {
            ret = OSPI_ll_SetSpiMode(OSPI, ((control & ARM_OSPI_FRAME_FORMAT_MSK) >> ARM_OSPI_FRAME_FORMAT_POS));
            break;
        }

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* Configure frame size */
    if (control & ARM_OSPI_DATA_BITS_MSK)
    {
        ret = OSPI_ll_SetDataFrameSize(OSPI, ((control & ARM_OSPI_DATA_BITS_MSK) >> ARM_OSPI_DATA_BITS_POS));

        if (ret != ARM_DRIVER_OK)
        {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }

    switch (control & ARM_OSPI_BIT_ORDER_MSK)
    {
        /* OSPI Bit order from MSB to LSB (default) */
        case ARM_OSPI_MSB_LSB:
        {
            break;
        }
        /* OSPI Bit order from LSB to MSB */
        case ARM_OSPI_LSB_MSB:
        {
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        }
    }

    switch (control & ARM_OSPI_SS_MASTER_MODE_MSK)
    {
        /* OSPI Slave Select when Master: Not used (default) */
        case ARM_OSPI_SS_MASTER_UNUSED:
        {
            OSPI_ll_Control_SlaveSelect(OSPI, 0XF, 0);
            break;
        }

        /* OSPI Slave Select when Master: Software controlled */
        case ARM_OSPI_SS_MASTER_SW:
        {
            if((control & ARM_OSPI_CONTROL_MSK) == ARM_OSPI_MODE_MASTER )
            {
                OSPI->sw_slave_select = 1;
                return ARM_DRIVER_OK;
            }
            return ARM_DRIVER_ERROR;
        }

        /* OSPI Slave Select when Master: Hardware controlled Output */
        case ARM_OSPI_SS_MASTER_HW_OUTPUT:
        {
            /* This is the default state in the IP, No need to configure */
            return ARM_DRIVER_OK;
        }

        /* OSPI Slave Select when Master: Hardware monitored Input */
        case ARM_OSPI_SS_MASTER_HW_INPUT:
        {
            //TODO: Need to Implement
            break;
        }
    }

    return ret;
}

/**
 * @fn      uint32_t ARM_OSPI_GetStatus(OSPI_RESOURCES *OSPI)
 * @brief   Used to get spi status.
 * @note    none.
 * @param   OSPI : Pointer to spi resources structure.
 * @retval  \ref spi driver status.
 */
static ARM_OSPI_STATUS ARM_OSPI_GetStatus(OSPI_RESOURCES *OSPI)
{
    return OSPI->status;
}

/* OSPI0 driver instance */
#if RTE_OSPI0
OSPI_RESOURCES OSPI0 = {
    .reg_base               = (OSPI_RegInfo*) OSPI0_BASE,
    .irq                    = XNVM_OSPI0_IRQ,
    .cb_event               = NULL,
    .irq_priority           = RTE_OSPI0_IRQ_PRIORITY,
    .drv_instance           = OSPI_INSTANCE_0,
    .chip_selection_pin     = RTE_OSPI0_CHIP_SELECTION_PIN,
    .spi_frf                = RTE_OSPI0_SPI_FRAME_FORMAT,
    .tx_fifo_threshold      = RTE_OSPI0_TX_FIFO_THRESHOLD,
    .tx_fifo_start_level    = RTE_OSPI0_TX_FIFO_LEVEL_TO_START_TRANSFER,
    .rx_fifo_threshold      = RTE_OSPI0_RX_FIFO_THRESHOLD
};

void XNVM_OSPI0_IRQHandler(void)
{
    OSPI_IRQHandler(&OSPI0);
}

static int32_t ARM_OSPI0_Initialize(ARM_OSPI_SignalEvent_t cb_event)
{
    return ARM_OSPI_Initialize(&OSPI0, cb_event);
}

static int32_t ARM_OSPI0_Uninitialize(void)
{
    return ARM_OSPI_Uninitialize(&OSPI0);
}

static int32_t ARM_OSPI0_PowerControl(ARM_POWER_STATE state)
{
    return ARM_OSPI_PowerControl(&OSPI0, state);
}

static int32_t ARM_OSPI0_Send(const void *data, uint32_t num)
{
    return ARM_OSPI_Send(&OSPI0, data, num);
}

static int32_t ARM_OSPI0_Receive(void *data, uint32_t num)
{
    return ARM_OSPI_Receive(&OSPI0, data, num);
}

static int32_t ARM_OSPI0_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_OSPI_Transfer(&OSPI0, data_out, data_in, num);
}

static uint32_t ARM_OSPI0_GetDataCount(void)
{
    return ARM_OSPI_GetDataCount(&OSPI0);
}

static int32_t ARM_OSPI0_Control(uint32_t control, uint32_t arg)
{
    return ARM_OSPI_Control(&OSPI0, control, arg);
}

static ARM_OSPI_STATUS ARM_OSPI0_GetStatus(void)
{
    return ARM_OSPI_GetStatus(&OSPI0);
}

extern ARM_DRIVER_OSPI Driver_OSPI0;
ARM_DRIVER_OSPI Driver_OSPI0 = {
    ARM_OSPI_GetVersion,
    ARM_OSPI_GetCapabilities,
    ARM_OSPI0_Initialize,
    ARM_OSPI0_Uninitialize,
    ARM_OSPI0_PowerControl,
    ARM_OSPI0_Send,
    ARM_OSPI0_Receive,
    ARM_OSPI0_Transfer,
    ARM_OSPI0_GetDataCount,
    ARM_OSPI0_Control,
    ARM_OSPI0_GetStatus
};
#endif /* RTE_OSPI0 */

/* OSPI1 driver instance */
#if RTE_OSPI1
OSPI_RESOURCES OSPI1 = {
    .reg_base               = (OSPI_RegInfo*) OSPI1_BASE,
    .irq                    = XNVM_OSPI1_IRQ,
    .cb_event               = NULL,
    .irq_priority           = RTE_OSPI1_IRQ_PRIORITY,
    .drv_instance           = OSPI_INSTANCE_1,
    .chip_selection_pin     = RTE_OSPI1_CHIP_SELECTION_PIN,
    .spi_frf                = RTE_OSPI1_SPI_FRAME_FORMAT,
    .tx_fifo_threshold      = RTE_OSPI1_TX_FIFO_THRESHOLD,
    .tx_fifo_start_level    = RTE_OSPI1_TX_FIFO_LEVEL_TO_START_TRANSFER,
    .rx_fifo_threshold      = RTE_OSPI1_RX_FIFO_THRESHOLD
};

void XNVM_OSPI1_IRQHandler(void)
{
    OSPI_IRQHandler(&OSPI1);
}

static int32_t ARM_OSPI1_Initialize(ARM_OSPI_SignalEvent_t cb_event)
{
    return ARM_OSPI_Initialize(&OSPI1, cb_event);
}

static int32_t ARM_OSPI1_Uninitialize(void)
{
    return ARM_OSPI_Uninitialize(&OSPI1);
}

static int32_t ARM_OSPI1_PowerControl(ARM_POWER_STATE state)
{
    return ARM_OSPI_PowerControl(&OSPI1, state);
}

static int32_t ARM_OSPI1_Send(const void *data, uint32_t num)
{
    return ARM_OSPI_Send(&OSPI1, data, num);
}

static int32_t ARM_OSPI1_Receive(void *data, uint32_t num)
{
    return ARM_OSPI_Receive(&OSPI1, data, num);
}

static int32_t ARM_OSPI1_Transfer(const void *data_out, void *data_in, uint32_t num)
{
    return ARM_OSPI_Transfer(&OSPI1, data_out, data_in, num);
}

static uint32_t ARM_OSPI1_GetDataCount(void)
{
    return ARM_OSPI_GetDataCount(&OSPI1);
}

static int32_t ARM_OSPI1_Control(uint32_t control, uint32_t arg)
{
    return ARM_OSPI_Control(&OSPI1, control, arg);
}

static ARM_OSPI_STATUS ARM_OSPI1_GetStatus(void)
{
    return ARM_OSPI_GetStatus(&OSPI1);
}

extern ARM_DRIVER_OSPI Driver_OSPI1;
ARM_DRIVER_OSPI Driver_OSPI1 = {
    ARM_OSPI_GetVersion,
    ARM_OSPI_GetCapabilities,
    ARM_OSPI1_Initialize,
    ARM_OSPI1_Uninitialize,
    ARM_OSPI1_PowerControl,
    ARM_OSPI1_Send,
    ARM_OSPI1_Receive,
    ARM_OSPI1_Transfer,
    ARM_OSPI1_GetDataCount,
    ARM_OSPI1_Control,
    ARM_OSPI1_GetStatus
};
#endif /* RTE_OSPI1 */
