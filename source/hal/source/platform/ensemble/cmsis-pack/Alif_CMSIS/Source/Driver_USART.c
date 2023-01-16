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
 * @file     Driver_USART.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     20-May-2020
 * @brief    ARM CMSIS-Driver for UART RS232.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/

/* Project Includes */
#include "Driver_USART.h"
#include "UART_dev.h"

#if !(RTE_UART0 || RTE_UART1 || RTE_UART2 || RTE_UART3 || RTE_UART4 || RTE_UART5 || RTE_UART6 || RTE_UART7)
#error "UART is not enabled in the RTE_Device.h"
#endif

#if (defined(RTE_Drivers_USART0) && !RTE_UART0)
#error "UART0 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_USART1) && !RTE_UART1)
#error "UART1 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_USART2) && !RTE_UART2)
#error "UART2 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_USART3) && !RTE_UART3)
#error "UART3 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_USART4) && !RTE_UART4)
#error "UART4 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_USART5) && !RTE_UART5)
#error "UART5 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_USART6) && !RTE_UART6)
#error "UART6 not configured in RTE_Device.h!"
#endif

#if (defined(RTE_Drivers_USART7) && !RTE_UART7)
#error "UART7 not configured in RTE_Device.h!"
#endif

/* Macros ----------------------------------------------------------------------------- */

#define ARM_USART_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /* driver version */

/* convert baudrate to divisor */
#define UART_CONVERT_BAUD_TO_DIVISOR(peripheral_freq, baud)  ((peripheral_freq) / ((baud)*16))

/* enable transmit/receive interrupt */
#define UART_ENABLE_TRANSMITTER_INT                 (1U)    /* enable transmitter interrupt  */
#define UART_ENABLE_RECEIVER_INT                    (2U)    /* enable receiver interrupt     */

/* disable transmit/receive interrupt */
#define UART_DISABLE_TRANSMITTER_INT                (3U)    /* disable transmitter interrupt */
#define UART_DISABLE_RECEIVER_INT                   (4U)    /* disable receiver interrupt    */

/* UART Driver Instances 0-7 */
#define UART0_INSTANCE_NUM                           0      /* UART0 instance */
#define UART1_INSTANCE_NUM                           1      /* UART1 instance */
#define UART2_INSTANCE_NUM                           2      /* UART2 instance */
#define UART3_INSTANCE_NUM                           3      /* UART3 instance */
#define UART4_INSTANCE_NUM                           4      /* UART4 instance */
#define UART5_INSTANCE_NUM                           5      /* UART5 instance */
#define UART6_INSTANCE_NUM                           6      /* UART6 instance */
#define UART7_INSTANCE_NUM                           7      /* UART7 instance */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_USART_API_VERSION,
    ARM_USART_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_USART_CAPABILITIES DriverCapabilities = {
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator available */
    1, /* RTS Flow Control available */
    1, /* CTS Flow Control available */
    1, /* Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    1, /* Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USART_EVENT_RI */
    0  /* Reserved (must be zero) */
};

/*  Driver Capabilities Without RTS-CTS.
 *  As per PINMUX,
 *    UART0-UART3 instances has RS232 with RTC/CTS functionality and
 *    UART4-UART7 instances has RS232 without RTC/CTS and RS485 functionality.
 */
static const ARM_USART_CAPABILITIES DriverCapabilities_WO_RTS_CTS = {
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator available */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    1, /* Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    1, /* Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USART_EVENT_RI */
    0  /* Reserved (must be zero) */
};

//
//   Functions
//
/**
 * @fn		ARM_DRIVER_VERSION ARM_USART_GetVersion(void)
 * @brief	get uart version
 * @note	none
 * @param	none
 * @retval	driver version
 */
static ARM_DRIVER_VERSION ARM_USART_GetVersion(void)
{
  return DriverVersion;
}

/**
 * @fn		ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void)
 * @brief	get uart capabilites
 * @note	none
 * @param	none
 * @retval	driver capabilites
 */
static ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void)
{
  return DriverCapabilities;
}

/**
 * @fn		ARM_USART_CAPABILITIES ARM_USART_GetCapabilities_WO_RTS_CTS(void)
 * @brief	get uart capabilites without RTS-CTS.
 *			As per PinMux Only UART0-UART3(not UART4-UART7) instances has RTS/CTS functionality.
 *			UART4-UART7 instances has RS485 functionality.
 * @note	none
 * @param	none
 * @retval	driver capabilites without RTS-CTS.
 */
static ARM_USART_CAPABILITIES ARM_USART_GetCapabilities_WO_RTS_CTS(void)
{
  return DriverCapabilities_WO_RTS_CTS;
}

/**
 * @fn		int32_t uart_tx_ready (uart_reg_set_t *uart_reg_ptr)
 * @brief	check whether uart is ready to send; 1 ready, 0 not ready
 * @note	none
 * @param	uart_reg_ptr: Pointer to uart register set structure
 * @retval	1 ready to send, 0 not ready to send
 */
__STATIC_INLINE int32_t uart_tx_ready (uart_reg_set_t *uart_reg_ptr)
{
	/* read TFNF transmitt_fifo_not_full bit from usr uart status register */
	return ((uart_reg_ptr->usr & UART_USR_TRANSMIT_FIFO_NOT_FULL) ? 1 : 0);
}

/**
 * @fn		int32_t uart_rx_ready (uart_reg_set_t *uart_reg_ptr)
 * @brief	check whether uart is ready to receive; 1 ready, 0 not ready
 * @note	none
 * @param	uart_reg_ptr: Pointer to uart register set structure
 * @retval	1 ready to receive, 0 not ready to receive
 */
__STATIC_INLINE int32_t uart_rx_ready (uart_reg_set_t *uart_reg_ptr)
{
	/* read RFNE receive_fifo_not_empty bit from usr uart status register */
	return ((uart_reg_ptr->usr & UART_USR_RECEIVE_FIFO_NOT_EMPTY) ? 1 : 0);
}

/**
 * @fn		void uart_send_a_char_to_thr (uart_reg_set_t *uart_reg_ptr,
                                          char            chr)
 * @brief	write a char to uart transmit holding register
 * @note	none
 * @param	uart_reg_ptr : Pointer to uart register set structure
 * @param	chr          : char to send to thr register
 * @retval	none
 */
__STATIC_INLINE void uart_send_a_char_to_thr (uart_reg_set_t *uart_reg_ptr,
                                              char            chr)
{
	/* write a char to thr transmit holding register */
	uart_reg_ptr->rbr_thr_dll = chr;
}

/**
 * @fn		int32_t uart_receive_a_char_from_rbr (uart_reg_set_t *uart_reg_ptr)
 * @brief	read data from uart receive buffer register
 * @note	none
 * @param	uart_reg_ptr: Pointer to uart register set structure
 * @retval	received data
 */
__STATIC_INLINE int32_t uart_receive_a_char_from_rbr (uart_reg_set_t *uart_reg_ptr)
{
	/* read a char from receive buffer register */
	return (int32_t)uart_reg_ptr->rbr_thr_dll;
}

/**
 * @fn		void uart_software_reset (uart_reg_set_t *uart_reg_ptr)
 * @brief	uart software reset
 * @note	none
 * @param	uart_reg_ptr: Pointer to uart register set structure
 * @retval	none
 */
__STATIC_INLINE void uart_software_reset (uart_reg_set_t *uart_reg_ptr)
{
	/* use shadow register srr software reset register */
	uart_reg_ptr->srr = UART_SRR_UART_RESET|UART_SRR_RCVR_FIFO_RESET|UART_SRR_TRANSMIT_FIFO_RESET;

	/* wait until software reset completed */
	while(uart_reg_ptr->usr & UART_USR_BUSY);
}

/**
 * @fn		void uart_set_baudrate (uart_resources_t *uart,
                                    uint32_t          arg)
 * @brief	set uart baudrate
 * @note	added support for fraction in dlf divisor latch fraction register
 * @param	uart    : Pointer to uart resources structure
 * @param	arg     : baudrate
 * @retval	none
 */
static void uart_set_baudrate (uart_resources_t *uart,
                               uint32_t          arg)
{
	uint32_t baud           = 0;
	int32_t baud_divisor    = 0;
	int32_t fraction        = 0;
	float decimal           = 0.0;
	int32_t i               = 0;
	uint32_t freq           = 0;

	uart_info_t     *uart_info_ptr = uart->info;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* take frequency as a peripheral clock */
	freq = uart->clk;
	baud = arg;

	if(baud)
	{
		decimal = UART_CONVERT_BAUD_TO_DIVISOR((float)freq, (float)baud);
		baud_divisor = (int32_t)UART_CONVERT_BAUD_TO_DIVISOR(freq, baud);
		fraction = (int32_t)((decimal - baud_divisor)*(1 << UART_DLF_SIZE));
	}

	/* update baudrate to uart resource structure */
	uart_info_ptr->baudrate = baud;

	/* enable DLAB divisor latch access bit in lcr line control register  */
	uart_reg_ptr->lcr |= UART_LCR_DLAB;

	/* setting uart baudrate registers	 */
	uart_reg_ptr->rbr_thr_dll = baud_divisor & 0xff;    /* DLL divisor latch low register       */
	uart_reg_ptr->ier_dlh = (baud_divisor>>8) & 0xff;   /* DLH divisor latch high register      */
	uart_reg_ptr->dlf = fraction;                       /* DLF divisor latch fraction register  */

	/* disable DLAB */
	uart_reg_ptr->lcr &= ~(UART_LCR_DLAB);

	/* hardware requires this delay before operating on new baud */
	for(i = 0; i < (32 * baud_divisor * 1); i++);
}

/**
 * @fn		int32_t uart_set_asynchronous_mode (uart_resources_t *uart,
                                                uint32_t          control,
                                                uint32_t          arg)
 * @brief	set uart asynchronous parameters
 *          baudrate, data bits, parity, stop bits, flow control
 * @note	none
 * @param	uart                          : Pointer to uart resources structure
 * @param	control                       : Control Operation
 * @param	arg                           : Argument of operation (optional)
 * @retval	\ref execution_status
 */
static int32_t uart_set_asynchronous_mode (uart_resources_t *uart,
                                           uint32_t          control,
                                           uint32_t          arg)
{
	int lcr               = 0;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* set the uart baudrate */
	uart_set_baudrate(uart, arg);

	/* UART Data bits */
	switch (control & ARM_USART_DATA_BITS_Msk)
	{
		/* Data bit is not configurable */
		/* set DLS data_length_select bit in lcr line control register */
		case ARM_USART_DATA_BITS_5: lcr |= (UART_LCR_DATA_LENGTH_5); break;
		case ARM_USART_DATA_BITS_6: lcr |= (UART_LCR_DATA_LENGTH_6); break;
		case ARM_USART_DATA_BITS_7: lcr |= (UART_LCR_DATA_LENGTH_7); break;
		case ARM_USART_DATA_BITS_8: lcr |= (UART_LCR_DATA_LENGTH_8); break;
		default: return ARM_USART_ERROR_DATA_BITS;
	}

	/* UART Parity */
	switch (control & ARM_USART_PARITY_Msk)
	{
		/* set PEN parity enable, EPS even parity select bit in lcr line control register */
		case ARM_USART_PARITY_NONE: lcr |= (UART_LCR_PARITY_NONE); break;
		case ARM_USART_PARITY_EVEN: lcr |= (UART_LCR_PARITY_EVEN); break;
		case ARM_USART_PARITY_ODD:  lcr |= (UART_LCR_PARITY_ODD);  break;
		default: return ARM_USART_ERROR_PARITY;
	}

	/* UART Stop bits */
	switch (control & ARM_USART_STOP_BITS_Msk)
	{
		/* set STOP number_of_stop_bits in lcr line control register */
		case ARM_USART_STOP_BITS_1: lcr |= (UART_LCR_STOP_1BIT); break;
		case ARM_USART_STOP_BITS_2: lcr |= (UART_LCR_STOP_2BIT); break;
		default: return ARM_USART_ERROR_STOP_BITS;
	}

	/* clear data,parity,stop bits */
	uart_reg_ptr->lcr &= (~UART_LCR_DATA_PARITY_STOP_MASK);

	/* set data,parity,stop bits */
	uart_reg_ptr->lcr |= lcr;

	/* uart flow control */
	switch (control & ARM_USART_FLOW_CONTROL_Msk)
	{
		/* set flow control bit in mcr modem control register */
		case ARM_USART_FLOW_CONTROL_NONE:
			uart_reg_ptr->mcr &= ~(UART_MCR_AFCE|UART_MCR_RTS);
			break;
		case ARM_USART_FLOW_CONTROL_RTS:

			/*  As per PINMUX,
			 *    UART0-UART3 instances has RS232 with RTC/CTS functionality and
			 *    UART4-UART7 instances has RS232 without RTC/CTS and RS485 functionality.
			 */
			if( (uart->instance_num >= 4) && (uart->instance_num <= 7) )
				return ARM_USART_ERROR_FLOW_CONTROL;   /* Unsupported. */

			uart_reg_ptr->mcr |= (UART_MCR_AFCE|UART_MCR_RTS);
			break;
		case ARM_USART_FLOW_CONTROL_CTS:

			/*  As per PINMUX,
			 *    UART0-UART3 instances has RS232 with RTC/CTS functionality and
			 *    UART4-UART7 instances has RS232 without RTC/CTS and RS485 functionality.
			 */
			if( (uart->instance_num >= 4) && (uart->instance_num <= 7) )
				return ARM_USART_ERROR_FLOW_CONTROL;   /* Unsupported. */

			uart_reg_ptr->mcr |= (UART_MCR_AFCE);
			break;
		case ARM_USART_FLOW_CONTROL_RTS_CTS:

			/*  As per PINMUX,
			 *    UART0-UART3 instances has RS232 with RTC/CTS functionality and
			 *    UART4-UART7 instances has RS232 without RTC/CTS and RS485 functionality.
			 */
			if( (uart->instance_num >= 4) && (uart->instance_num <= 7) )
				return ARM_USART_ERROR_FLOW_CONTROL;   /* Unsupported. */

			uart_reg_ptr->mcr |= (UART_MCR_AFCE|UART_MCR_RTS);
			break;
		default: return ARM_USART_ERROR_FLOW_CONTROL;
	}

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_set_rx_trigger (uart_resources_t  *uart,
                                         uart_rx_trigger_t  rx_trigger)
 * @brief	set uart receiver trigger level
 *			This is used to select the trigger level in the receiver FIFO
 *			at which the Received Data Available Interrupt will be generated.
 * @note	/ref uart_rx_trigger_t enum
 * @param	uart		: Pointer to uart resources structure
 * @param	rx_trigger	: enum uart_rx_trigger_t
 * @retval	\ref execution_status
 */
static int32_t uart_set_rx_trigger (uart_resources_t  *uart,
                                    uart_rx_trigger_t  rx_trigger)
{
	uart_reg_set_t *uart_reg_ptr = (uart_reg_set_t *)uart->reg_base;

	if(rx_trigger > UART_RX_FIFO_TWO_LESS_FULL)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* update srt shadow receiver trigger register */
	uart_reg_ptr->srt = rx_trigger;
	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_set_tx_trigger (uart_resources_t  *uart,
                                         uart_tx_trigger_t  tx_trigger)
 * @brief	set uart transmitter trigger level
 *			This is used to select the empty threshold level in the transmitter FIFO
 *			at which the THRE Interrupts will be generated.
 * @note	/ref uart_tx_trigger_t enum
 * @param	uart		: Pointer to uart resources structure
 * @param	tx_trigger	: enum uart_tx_trigger_t
 * @retval	\ref execution_status
 */
static int32_t uart_set_tx_trigger (uart_resources_t  *uart,
                                    uart_tx_trigger_t  tx_trigger)
{
	uart_reg_set_t *uart_reg_ptr = (uart_reg_set_t *)uart->reg_base;

	if(tx_trigger > UART_TX_FIFO_HALF_FULL)
		return ARM_DRIVER_ERROR_PARAMETER;

	/* update stet shadow TX empty trigger register */
	uart_reg_ptr->stet = tx_trigger;
	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_get_tx_fifo_available_count (uart_resources_t *uart)
 * @brief	get available transmit fifo count
 * @note	useful in interrupt callback
 * @param	uart: Pointer to uart resources structure
 * @retval	available transmit fifo count
 */
static int32_t uart_get_tx_fifo_available_count (uart_resources_t *uart)
{
	uart_reg_set_t  *uart_reg_ptr	= (uart_reg_set_t *)uart->reg_base;

	/* read tfl transmit FIFO level register,
	 * TX_fifo available count = fifo depth - data entries in transmit fifo
	 */
	return (uart->info->tx_fifo_length - uart_reg_ptr->tfl);
}

/**
 * @fn		int32_t uart_get_rx_fifo_available_count (uart_resources_t *uart)
 * @brief	get available receive fifo count
 * @note	useful in interrupt callback
 * @param	uart: Pointer to uart resources structure
 * @retval	available receive fifo count
 */
static int32_t uart_get_rx_fifo_available_count (uart_resources_t *uart)
{
	uart_reg_set_t *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* read rfl receive FIFO level register */
	return (uart_reg_ptr->rfl);
}

/**
 * @fn		void uart_set_break_control (uart_resources_t *uart)
 * @brief	set uart break control
 * @note	none
 * @param	uart: Pointer to uart resources structure
 * @retval	none
 */
static void uart_set_break_control (uart_resources_t *uart)
{
	uart_reg_set_t *uart_reg_ptr = (uart_reg_set_t *)uart->reg_base;
	/* set break_control bit in lcr line control register. */
	uart_reg_ptr->lcr |= UART_LCR_BREAK;
}

/**
 * @fn		void uart_clear_break_control (uart_resources_t *uart)
 * @brief	clear uart break control
 * @note	none
 * @param	uart: Pointer to uart resources structure
 * @retval	none
 */
static void uart_clear_break_control (uart_resources_t *uart)
{
	uart_reg_set_t *uart_reg_ptr = (uart_reg_set_t *)uart->reg_base;
	/* clear break_control bit in lcr line control register. */
	uart_reg_ptr->lcr &= ~UART_LCR_BREAK;
}

/**
 * @fn		static int32_t uart_enable_irq (uart_resources_t *uart,
                                            uint32_t          arg)
 * @brief	enable uart transmit and receive interrupt
 * @note	only one combined interrupt is available for TX/RX
 *			so if any one is enable then no need to enable interrupt again.
 * @param	uart    : Pointer to uart resources structure
 * @param	arg     : UART_ENABLE_TRANSMITTER_INT or UART_ENABLE_RECEIVER_INT
 * @retval	\ref execution_status
 */
static int32_t uart_enable_irq (uart_resources_t *uart,
                                uint32_t          arg)
{
	uart_info_t     *uart_info_ptr = uart->info;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* pass only if parameter is TX/RX enable interrupt */
	if( (arg != UART_ENABLE_TRANSMITTER_INT) && (arg != UART_ENABLE_RECEIVER_INT) )
		return ARM_DRIVER_ERROR_PARAMETER;

	/* enable transmitter interrupt */
	if(arg == UART_ENABLE_TRANSMITTER_INT)
	{
		/* enable transmit_holding_register_empty bit in ier interrupt enable register */
		uart_reg_ptr->ier_dlh |= UART_IER_ENABLE_TRANSMIT_HOLD_REG_EMPTY;

		/* set TX interrupt enable flag */
		uart_info_ptr->int_status |= UART_FLAG_TX_INT_ENABLE;
	}

	/* enable receiver interrupt */
	if(arg == UART_ENABLE_RECEIVER_INT)
	{
		/* enable receive_data_available_interrupt bit in ier interrupt enable register */
		uart_reg_ptr->ier_dlh |= UART_IER_ENABLE_RECEIVED_DATA_AVAILABLE;

		/* set RX interrupt enable flag */
		uart_info_ptr->int_status |= UART_FLAG_RX_INT_ENABLE;
	}

	/* only one combined interrupt is available for TX/RX
	 * so if any one is enable then no need to enable interrupt again.
	 */

	/* if TX and RX both are disable then only enable interrupt. */
	if ((uart_info_ptr->int_status & UART_FLAG_TX_OR_RX_INT_ENABLE) == 0) /* TX and RX both are disable. */
	{
		/* anyone should be enable either TX or RX */
		if ( (uart_info_ptr->int_status & UART_FLAG_TX_INT_ENABLE) ||
			(uart_info_ptr->int_status & UART_FLAG_RX_INT_ENABLE) )
		{
			/* enable the NVIC interrupt. */
			NVIC_EnableIRQ(uart->irq_num);
			/* set the global flag as TX or RX interrupt enable. */
			uart_info_ptr->int_status |= UART_FLAG_TX_OR_RX_INT_ENABLE;
		}
	}
	/* else interrupt is already enabled. */

	return ARM_DRIVER_OK;
}

/**
 * @fn		static int32_t uart_disable_irq (uart_resources_t *uart,
                                             uint32_t          arg)
 * @brief	disable uart transmit and receive interrupt
 * @note	only one combined interrupt is available for TX/RX
 *			so if both TX and RX are disable then only disable interrupt.
 * @param	uart    : Pointer to uart resources structure
 * @param	arg     : UART_DISABLE_TRANSMITTER_INT or UART_DISABLE_RECEIVER_INT
 * @retval	\ref execution_status
 */
static int32_t uart_disable_irq (uart_resources_t *uart,
                                 uint32_t          arg)
{
	uart_info_t     *uart_info_ptr = uart->info;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* pass only if parameter is TX/RX diable interrupt */
	if( (arg != UART_DISABLE_TRANSMITTER_INT) && (arg != UART_DISABLE_RECEIVER_INT) )
		return ARM_DRIVER_ERROR_PARAMETER;

	/* disable transmit interrupt */
	if(arg == UART_DISABLE_TRANSMITTER_INT)
	{
		/* disable transmit_holding_register_empty bit in ier interrupt enable register */
		uart_reg_ptr->ier_dlh &= ~UART_IER_ENABLE_TRANSMIT_HOLD_REG_EMPTY;

		/* reset TX interrupt enable flag */
		uart_info_ptr->int_status &= ~UART_FLAG_TX_INT_ENABLE;
	}

	/* disable receiver interrupt */
	if(arg == UART_DISABLE_RECEIVER_INT)
	{
		/* disable receive_data_available_interrupt bit in ier interrupt enable register */
		uart_reg_ptr->ier_dlh &= ~UART_IER_ENABLE_RECEIVED_DATA_AVAILABLE;

		/* reset RX interrupt enable flag */
		uart_info_ptr->int_status &= ~UART_FLAG_RX_INT_ENABLE;
	}

	/* only one combined interrupt is available for TX/RX
	 * so if both TX and RX are disable then only disable interrupt.
	 */

	/* disable if anyone TX or RX interrupt is already enabled. */
	if (uart_info_ptr->int_status & UART_FLAG_TX_OR_RX_INT_ENABLE)
	{
		/* both TX and RX flag should be disable then only disable interrupt. */
		if ( ((uart_info_ptr->int_status & UART_FLAG_TX_INT_ENABLE) == 0) &&
			 ((uart_info_ptr->int_status & UART_FLAG_RX_INT_ENABLE) == 0) )
		{
			/* disable the NVIC interrupt. */
			NVIC_DisableIRQ(uart->irq_num);
			/* set the global flag as TX or RX interrupt is enable. */
			uart_info_ptr->int_status &= ~UART_FLAG_TX_OR_RX_INT_ENABLE;
		}
	}

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_enable_send_interrupt (uart_resources_t *uart,
                                                const void       *data,
                                                uint32_t          len)
 * @brief	setup for send interrupt
 *			this function will fill the uart transfer structure as per user input details
 *			and enable the transmitter interrupt
 * @note	none
 * @param	uart	: Pointer to uart resources structure
 * @param	data	: Pointer to input data
 * @param	len	: total length of the send data
 * @retval	\ref execution_status
 */
static int32_t uart_enable_send_interrupt (uart_resources_t *uart,
                                           const void       *data,
                                           uint32_t          len)
{
	uart_info_t *uart_info_ptr  = uart->info;
	int32_t ret = ARM_DRIVER_OK;

	/* check previous TX is completed or not? */
	if (uart_info_ptr->transfer.tx_busy != UART_STATUS_FREE)
	{
		/* return busy as previous send is not yet completed */
		return ARM_DRIVER_ERROR_BUSY;
	}

	/* Set TX busy flag to active */
	uart_info_ptr->transfer.tx_busy = UART_STATUS_BUSY;

	/* fill the uart transfer structure as per user input */
	uart_info_ptr->transfer.tx_buf = (uint8_t *)data;
	uart_info_ptr->transfer.tx_total_num = len;
	uart_info_ptr->transfer.tx_curr_cnt = 0U;

	/* enable the transmitter interrupt.*/
	ret = uart_enable_irq(uart, UART_ENABLE_TRANSMITTER_INT);
	return ret;
}

/**
 * @fn		int32_t uart_enable_receive_interrupt (uart_resources_t *uart,
                                                   const void       *data,
                                                   uint32_t          len)
 * @brief	setup for receive interrupt
 *			this function will fill the uart receiver structure as per user input details
 *			and enable the receiver interrupt
 * @note	none
 * @param	uart	: Pointer to uart resources structure
 * @param	data	: Pointer to output data
 * @param	len	: total length of the receive data
 * @retval	\ref execution_status
 */
static int32_t uart_enable_receive_interrupt (uart_resources_t *uart,
                                              const void       *data,
                                              uint32_t          len)
{
	uart_info_t *uart_info_ptr = uart->info;
	int32_t ret = ARM_DRIVER_OK;

	/* check previous receive is completed or not? */
	if (uart->info->rx_status.rx_busy == UART_STATUS_BUSY)
	{
		/* return busy as previous receive is not yet completed */
		return ARM_DRIVER_ERROR_BUSY;
	}

	/* set rx busy flag to active */
	uart_info_ptr->rx_status.rx_busy = UART_STATUS_BUSY;

	/* clear rx status */
	uart_info_ptr->rx_status.rx_break          = 0U;
	uart_info_ptr->rx_status.rx_framing_error  = 0U;
	uart_info_ptr->rx_status.rx_overflow       = 0U;
	uart_info_ptr->rx_status.rx_parity_error   = 0U;

	/* fill the uart transfer structure as per user input */
	uart_info_ptr->transfer.rx_buf = (uint8_t *)data;
	uart_info_ptr->transfer.rx_total_num = len;
	uart_info_ptr->transfer.rx_curr_cnt = 0U;

	/* enable the receiver interrupt. */
	ret = uart_enable_irq(uart, UART_ENABLE_RECEIVER_INT);
	return ret;
}

/**
 * @fn		void uart_reset_txfifo (uart_reg_set_t *uart_reg_ptr)
 * @brief	reset transmit fifo
 * @note	none
 * @param	uart_reg_ptr	: Pointer to uart register set structure
 * @retval	none
 */
__STATIC_INLINE void uart_reset_txfifo (uart_reg_set_t *uart_reg_ptr)
{
	/* set XMIT_FIFO_Reset bit in shadow register srr software reset register */
	uart_reg_ptr->srr  = UART_SRR_TRANSMIT_FIFO_RESET;

	/* wait until software reset completed */
	while(uart_reg_ptr->usr & UART_USR_BUSY);
}

/**
 * @fn		void uart_reset_rxfifo (uart_reg_set_t *uart_reg_ptr)
 * @brief	reset receiver fifo
 * @note	none
 * @param	uart_reg_ptr	: Pointer to uart register set structure
 * @retval	none
 */
__STATIC_INLINE void uart_reset_rxfifo (uart_reg_set_t *uart_reg_ptr)
{
	/* set RCVR_FIFO_Reset bit in shadow register srr software reset register */
	uart_reg_ptr->srr  = UART_SRR_RCVR_FIFO_RESET;

	/* wait until software reset completed */
	while(uart_reg_ptr->usr & UART_USR_BUSY);
}

/**
 * @fn		void uart_flush_rxfifo (uart_resources_t *uart)
 * @brief	flush receiver fifo
 * @note	/ref uart_abort_rx
 * @param	uart: Pointer to uart resources structure
 * @retval	none
 */
void uart_flush_rxfifo (uart_resources_t *uart)
{
	uart_info_t     *uart_info_ptr = uart->info;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* read all the bytes which are available in RX fifo till given rx total length */
	while ( uart_rx_ready(uart_reg_ptr) && (uart_info_ptr->transfer.rx_curr_cnt != uart_info_ptr->transfer.rx_total_num) )
	{
		/* read a char from rbr receive buffer register. */
		uart_info_ptr->transfer.rx_buf[uart_info_ptr->transfer.rx_curr_cnt] = uart_receive_a_char_from_rbr(uart_reg_ptr);
		uart_info_ptr->transfer.rx_curr_cnt++;
	}
}

/**
 * @fn		void uart_abort_tx (uart_resources_t *uart)
 * @brief	abort transmitter
 * @note	none
 * @param	uart: Pointer to uart resources structure
 * @retval	none
 */
static void uart_abort_tx (uart_resources_t *uart)
{
	uart_info_t     *uart_info_ptr = uart->info;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* if tx interrupt flag is enable then only disable transmit interrupt */
	if (uart_info_ptr->int_status & UART_FLAG_TX_INT_ENABLE)
	{
		/* disable the transmit interrupt.*/
		uart_disable_irq(uart, UART_DISABLE_TRANSMITTER_INT);
	}

	/* reset TX fifo */
	uart_reset_txfifo(uart_reg_ptr);

	/* clear TX busy flag */
	uart_info_ptr->transfer.tx_busy = UART_STATUS_FREE;

	/* Reset the tx_buffer */
	uart_info_ptr->transfer.tx_total_num = 0U;
}

/**
 * @fn		void uart_abort_rx (uart_resources_t *uart)
 * @brief	abort receiver
 * @note	none
 * @param	uart: Pointer to uart resources structure
 * @retval	none
 */
static void uart_abort_rx (uart_resources_t *uart)
{
	uart_info_t     *uart_info_ptr = uart->info;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* if rx interrupt flag is enable then only disable receiver interrupt */
	if (uart_info_ptr->int_status & UART_FLAG_RX_INT_ENABLE)
	{
		/* disable the receiver interrupt. */
		uart_disable_irq(uart, UART_DISABLE_RECEIVER_INT);
	}

	/* flush-out rx fifo */
	uart_flush_rxfifo(uart);

	/* Reset rx fifo */
	uart_reset_rxfifo(uart_reg_ptr);

	/* clear Receive busy flag */
	uart_info_ptr->rx_status.rx_busy      = UART_STATUS_FREE;
	uart_info_ptr->transfer.rx_total_num  = 0U;
}

/**
 * @fn		void uart_disable_interrupt (uart_resources_t *uart)
 * @brief	disable transmit and receive interrupt
 * @note	none
 * @param	uart: Pointer to uart resources structure
 * @retval	none
 */
static void uart_disable_interrupt (uart_resources_t *uart)
{
	/* disable uart transmit interrupt  */
	uart_disable_irq(uart, UART_DISABLE_TRANSMITTER_INT);

	/* disable uart receive interrupt  */
	uart_disable_irq(uart, UART_DISABLE_RECEIVER_INT);

	/* disable uart interrupt */
	NVIC_DisableIRQ(uart->irq_num);

	/* reset the global flag for interrupt. */
	uart->info->int_status &= ~(UART_FLAG_TX_OR_RX_INT_ENABLE|UART_FLAG_TX_INT_ENABLE|UART_FLAG_RX_INT_ENABLE);
}

/**
 * @fn		void uart_irq_handler (uart_resources_t *uart)
 * @brief	uart interrupt handler
 * @note	only one combined interrupt for
 *              -TX / RX
 *              -RX_Character_Timeout
 *              -Modem status
 *              -Receiver Line status
 *          in RX_Timeout case not clearing rx busy flag, it is up to user to decide whether
 *          to wait for remaining bytes or call the abort rx. /ref abort_rx
 * @param	uart: Pointer to uart resources structure
 * @retval	none
 */
static void uart_irq_handler (uart_resources_t *uart)
{
	uint32_t uart_int_status = 0U;      /* uart interrupt status */
	uint32_t event           = 0U;      /* callback event */
	uint32_t tx_fifo_available_cnt  = 0U;   /* TX fifo Available count. */
	uint32_t rx_fifo_available_cnt  = 0U;   /* RX fifo Available count. */
	uint32_t i = 0U;


	uart_info_t     *uart_info_ptr = uart->info;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* get uart interrupt status from iir interrupt identity register */
	uart_int_status = (uart_reg_ptr->iir_fcr) & UART_IIR_INTERRUPT_ID_MASK;

	switch (uart_int_status)
	{
		case UART_IIR_MODEM_STATUS: /* modem status */

			(void)(uart_reg_ptr->msr);
			/* yet not implemented. */
			break;

		case UART_IIR_RECEIVER_LINE_STATUS: /* receiver line status */

			/* yet not implemented. */
			break;

		case UART_IIR_TRANSMIT_HOLDING_REG_EMPTY: /* transmit holding register empty */

			do
			{
				/* Query how many characters are available in TX fifo. */
				tx_fifo_available_cnt = uart_get_tx_fifo_available_count (uart);

				/* Write maximum number of characters to the TX fifo as per available space. */
				for(i=0; i<tx_fifo_available_cnt; i++)
				{
					if(uart_info_ptr->transfer.tx_curr_cnt >= uart_info_ptr->transfer.tx_total_num)
					{
						/* Come out as it transmitted all the user data. */
						break;
					}

					/* send character to thr register. */
					uart_send_a_char_to_thr(uart_reg_ptr, uart_info_ptr->transfer.tx_buf[uart_info_ptr->transfer.tx_curr_cnt]);
					uart_info_ptr->transfer.tx_curr_cnt++; /* increment the tx current count */
				}

				/* write again to tx fifo if it is not full and still there is some user data which needs to be send. */
			} while( uart_tx_ready(uart_reg_ptr) && (uart_info_ptr->transfer.tx_curr_cnt < uart_info_ptr->transfer.tx_total_num) );

			/* check whether it transmitted all the bytes? */
			if (uart_info_ptr->transfer.tx_curr_cnt >= uart_info_ptr->transfer.tx_total_num)
			{
				/* yes then disable the transmitter interrupt */
				uart_disable_irq(uart, UART_DISABLE_TRANSMITTER_INT);

				/* clear TX busy flag */
				uart_info_ptr->transfer.tx_busy = UART_STATUS_FREE;

				/* mark event as send Complete */
				event |= ARM_USART_EVENT_SEND_COMPLETE;
			}
			break;

		case UART_IIR_CHARACTER_TIMEOUT:        /* character timeout */
		case UART_IIR_RECEIVED_DATA_AVAILABLE:  /* received data available. */

			do
			{
				/* Query how many characters are available in RX fifo. */
				rx_fifo_available_cnt = uart_get_rx_fifo_available_count (uart);

				/* Read maximum number of characters available from the RX fifo or till rx total number. */
				for(i=0; i<rx_fifo_available_cnt; i++)
				{
					if (uart_info_ptr->transfer.rx_curr_cnt >= uart_info_ptr->transfer.rx_total_num)
					{
						/* Come out as it received all the user data. */
						break;
					}

					/* read character from rbr receive buffer register. */
					uart_info_ptr->transfer.rx_buf[uart_info_ptr->transfer.rx_curr_cnt] = uart_receive_a_char_from_rbr(uart_reg_ptr);
					uart_info_ptr->transfer.rx_curr_cnt++;
				}

				/* read again from rx fifo if it is not empty and user data is still remaining to read. */
			} while( uart_rx_ready(uart_reg_ptr) && (uart_info_ptr->transfer.rx_curr_cnt < uart_info_ptr->transfer.rx_total_num) );


			/* check whether it received all the bytes? */
			if (uart_info_ptr->transfer.rx_curr_cnt >= uart_info_ptr->transfer.rx_total_num)
			{
				/* yes than disable the receiver interrupt */
				uart_disable_irq(uart, UART_DISABLE_RECEIVER_INT);

				/* clear rx busy flag */
				uart_info_ptr->rx_status.rx_busy = UART_STATUS_FREE;

				/* mark event as receive complete */
				event |= ARM_USART_EVENT_RECEIVE_COMPLETE;
			}
			else /* fifo is empty. */ /* Added on 9-Oct-2020 */
			{
				/* FIXME After debugging we found that without else case, code is getting hang here as
				 * 1.) fifo is empty here, (as we are reading fifo in while loop),
				 *     and if we have not read total number of user RX data.
				 * 2.) Further RX_CHAR_TIMEOUT interrupt will not come as fifo is empty.
				 *     (As per Datasheet : Interrupt source for RX_CHAR_TIMEOUT
				 *          No characters in or out of the RCVR FIFO during the last 4 character times
				 *          and there is at least 1 character in it during this time)
				 * 3.) Added this to fix timeout issue (work-around)
				 * 4.) scenario: expecting 10 bytes but receiving only 1 byte? needs one more upper layer timeout.
				 * 5.) or Mark as separate event for "FIFO_EMPTY", Don't include in RX_Timeout.
				 */

				/* in RX_Timeout case not clearing rx busy flag
				 * it is up to user to decide whether
				 * to wait for remaining bytes or call the abort rx.
				*/
				/* mark event as RX Timeout */
				event |= ARM_USART_EVENT_RX_TIMEOUT;
			}

			/* got character Timeout? mark event as a RX Timeout. */
			if (uart_int_status == UART_IIR_CHARACTER_TIMEOUT)
			{
				/* in RX_Timeout case not clearing rx busy flag
				 * it is up to user to decide whether
				 * to wait for remaining bytes or call the abort rx.
				 */
				/* mark event as RX Timeout */
				event |= ARM_USART_EVENT_RX_TIMEOUT;
			}

			break;

		default:
			/* read the usr uart status register */
			(void)(uart_reg_ptr->usr);
			break;
	}

	/* call the user callback if any event occurs */
	if ((event != 0U) && (uart_info_ptr->cb_event != NULL) )
	{
		/* call the user callback */
		uart_info_ptr->cb_event(event);
	}

	return;
}

/* --------------------------------- RS485 Functions ------------------------------------------------------- */
#if RS485_SUPPORT   /* Check if UART RS485 mode is enabled? */

/*  @Note:
 *    As per PINMUX,
 *      UART0-UART3 instances has RS232 with RTC/CTS functionality and
 *      UART4-UART7 instances has RS232 without RTC/CTS and RS485 functionality.
 */

/* @Note: Observations with RS485 Transfer Modes:
 * 1.) RS485_HW_CONTROL_HALF_DULPLX_MODE: Able to send and receive data properly.
 *      -Both DE and RE signal can be enable at a time,
 *         as Hardware will take care of switching between Drive Enable and Receive Enable signal.
 * 2.) RS485_SW_CONTROL_HALF_DULPLX_MODE: Able to send and receive data properly with below conditions.
 *      -Drive Enable and Receive Enable signal are mutually exclusive.
 *      -Both DE and RE signal can not be enable at a time.
 *      -Proper Tuning between TX and RX is required with proper delay between TX-RX.
 *      -@Note: Currently Driver APIs are not exposed for SW Control Half Duplex mode.
 * 3.) RS485_FULL_DULPLX_MODE: Not tested.
 */


/**
 * @fn		int32_t uart_set_rs485_transfer_mode (uart_resources_t *uart, uart_rs485_transfer_mode_t mode)
 * @brief	set RS485 transfer modes \ref uart_rs485_transfer_mode_t
 * @note	none
 * @param	uart: Pointer to uart resources structure
 * @param	mode: Available RS485 transfer modes
 *					- UART_RS485_FULL_DULPLX_MODE            : Not tested as Hardware is not supporting Full duplex mode.
 *					- UART_RS485_SW_CONTROL_HALF_DULPLX_MODE : Currently Driver APIs are not exposed for SW Control Half Duplex mode.
 *					- UART_RS485_HW_CONTROL_HALF_DULPLX_MODE : Tested and Verified, Able to send and receive data properly.
 * @retval	\ref execution_status
 */
int32_t uart_set_rs485_transfer_mode (uart_resources_t *uart, uart_rs485_transfer_mode_t mode)
{
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	if ((mode < UART_RS485_FULL_DULPLX_MODE) || (mode > UART_RS485_HW_CONTROL_HALF_DULPLX_MODE ))
		return ARM_DRIVER_ERROR_PARAMETER;

	/* clear Transfer modes bits[4:3] */
	uart_reg_ptr->tcr &= (~UART_TCR_XFER_MODE_MASK);

	if(mode == UART_RS485_FULL_DULPLX_MODE)            uart_reg_ptr->tcr |= UART_TCR_XFER_MODE_FULL_DUPLEX;
	if(mode == UART_RS485_SW_CONTROL_HALF_DULPLX_MODE) uart_reg_ptr->tcr |= UART_TCR_XFER_MODE_SW_CONTROL_HALF_DUPLEX;
	if(mode == UART_RS485_HW_CONTROL_HALF_DULPLX_MODE) uart_reg_ptr->tcr |= UART_TCR_XFER_MODE_HW_CONTROL_HALF_DUPLEX;

	return ARM_DRIVER_OK;
}

/**
 * @fn		uart_rs485_transfer_mode_t uart_get_rs485_transfer_mode (uart_resources_t *uart)
 * @brief	get selected RS485 transfer mode \ref uart_rs485_transfer_mode_t
 * @note	none
 * @param	uart: Pointer to uart resources structure
 * @retval	selected RS485 transfer mode
 */
uart_rs485_transfer_mode_t uart_get_rs485_transfer_mode (uart_resources_t *uart)
{
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;
	return ((uart_reg_ptr->tcr & UART_TCR_XFER_MODE_MASK) >> 3);
}

/**
 * @fn		int32_t uart_set_rs485_de_assertion_time (uart_resources_t *uart, uint32_t assertion_time)
 * @brief	set RS485 DE Assertion time for
 *			DET driver output enable timing register
 * @note	DE Assertion time: 8 bit only, DET register bits (7:0)
 * @param	uart			: Pointer to uart resources structure
 * @param	assertion_time	: 8-bit DE Assertion time
 * @retval	\ref execution_status
 */
int32_t uart_set_rs485_de_assertion_time (uart_resources_t *uart, uint32_t assertion_time)
{
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* DE Assertion time: 8 bit only. */
	if(assertion_time > 0xFF)
		return ARM_DRIVER_ERROR_PARAMETER; /* error maximum 8-bit only. */

	/* clear DE Assertion time: bits (7:0). */
	uart_reg_ptr->det &= (~UART_DET_TIME_MASK);

	/* DE Assertion time: bits (7:0). */
	uart_reg_ptr->det |= (assertion_time & UART_DET_TIME_MASK);

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_set_rs485_de_deassertion_time (uart_resources_t *uart, uint32_t deassertion_time)
 * @brief	set RS485 DE De-Assertion time for
 *			DET driver output enable timing register
 * @note	DE Assertion time: 8 bit only, DET register bits (23:16)
 * @param	uart				: Pointer to uart resources structure
 * @param	deassertion_time	: 8-bit DE De-Assertion time
 * @retval	\ref execution_status
 */
int32_t uart_set_rs485_de_deassertion_time (uart_resources_t *uart, uint32_t deassertion_time)
{
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* DE De-Assertion time: 8 bit only. */
	if(deassertion_time > 0xFF)
		return ARM_DRIVER_ERROR_PARAMETER; /* error maximum 8-bit only. */

	/* clear DE De-Assertion time: bits (23:16). */
	uart_reg_ptr->det &= ( ~ (UART_DET_TIME_MASK << UART_DET_DE_DEASSERTION_TIME_BIT_SHIFT) );

	/* DE De-Assertion time: bits (23:16). */
	uart_reg_ptr->det |= ( (deassertion_time & UART_DET_TIME_MASK) << UART_DET_DE_DEASSERTION_TIME_BIT_SHIFT );

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_set_rs485_de_to_re_turn_around_time (uart_resources_t *uart, uint32_t de_to_re_time)
 * @brief	set RS485 Driver Enable DE to Receive Enable RE Turn Around time for
 *			TAT turn-around timing register
 * @note	TAT DE to RE Turn Around time: 16 bit , TAT register bits (15:0)
 * @param	uart				: Pointer to uart resources structure
 * @param	de_to_re_time		: 16-bit DE to RE time
 * @retval	\ref execution_status
 */
int32_t uart_set_rs485_de_to_re_turn_around_time (uart_resources_t *uart, uint32_t de_to_re_time)
{
	uart_reg_set_t	*uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* Driver Enable DE to Receive Enable RE Turn Around time: 16 bit . */
	if(de_to_re_time > 0xFFFF)
		return ARM_DRIVER_ERROR_PARAMETER; /* error maximum 16-bit . */

	/* Clear TAT DE to RE Turn Around time bits (15:0). */
	uart_reg_ptr->tat &= (~UART_TAT_TIME_MASK);

	/* TAT DE to RE Turn Around time bits (15:0). */
	uart_reg_ptr->tat |= (de_to_re_time & UART_TAT_TIME_MASK);

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_set_rs485_re_to_de_turn_around_time (uart_resources_t *uart, uint32_t re_to_de_time)
 * @brief	set RS485 Receive Enable RE to Driver Enable DE Turn Around time for
 *			TAT turn-around timing register
 * @note	TAT RE to DE Turn Around time: 16 bit , TAT register bits (31:16)
 * @param	uart				: Pointer to uart resources structure
 * @param	re_to_de_time		: 16-bit RE to DE time
 * @retval	\ref execution_status
 */
int32_t uart_set_rs485_re_to_de_turn_around_time (uart_resources_t *uart, uint32_t re_to_de_time)
{
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* Receive Enable RE to Driver Enable DE Turn Around time: 16 bit . */
	if(re_to_de_time > 0xFFFF)
		return ARM_DRIVER_ERROR_PARAMETER; /* error maximum 16-bit . */

	/* Clear TAT RE to DE Turn Around time bits (31:16). */
	uart_reg_ptr->tat &= ( ~ (UART_TAT_TIME_MASK << UART_TAT_RE_TO_DE_TIME_BIT_SHIFT) );

	/* TAT RE to DE Turn Around time bits (31:16). */
	uart_reg_ptr->tat |= ( (re_to_de_time & UART_TAT_TIME_MASK) << UART_TAT_RE_TO_DE_TIME_BIT_SHIFT );

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_set_rs485_de_en (uart_resources_t *uart, uint32_t arg)
 * @brief	set RS485 DE Driver Enable Signal State
 * @note	none
 * @param	uart		: Pointer to uart resources structure
 * @param	arg		: 0=disable, 1=enable
 * @retval	\ref execution_status
 */
int32_t uart_set_rs485_de_en (uart_resources_t *uart, uint32_t arg)
{
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;
	uint32_t mode;

	if( (arg != UART_DE_EN_DISABLE) && (arg != UART_DE_EN_ENABLE) )
		return ARM_DRIVER_ERROR_PARAMETER;

	if(arg == UART_DE_EN_DISABLE)
	{
		/* Disable the DE Driver Enable signal. */
		uart_reg_ptr->de_en = UART_DE_EN_DISABLE;
	}

	if(arg == UART_DE_EN_ENABLE)
	{
		/* Special check only for Software control Half-Duplex Mode as DE and RE are mutually exclusive here. */

		/* Check rs485 transfer mode. */
		mode = uart_get_rs485_transfer_mode(uart);

		if(mode == UART_RS485_SW_CONTROL_HALF_DULPLX_MODE)
		{
			/* In Software control Half-Duplex Mode DE and RE are mutually exclusive.
			 * so anyone either DE or RE can be enable at a time.
			 */

			/* in S/W Half-Duplex Mode first disable RE signal before enabling DE signal. */
			/* disable RE. */
			uart_reg_ptr->re_en = UART_RE_EN_DISABLE;
		}

		/* enable DE Driver Enable signal. */
		uart_reg_ptr->de_en = UART_DE_EN_ENABLE;
	}

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_set_rs485_re_en (uart_resources_t *uart, uint32_t arg)
 * @brief	set RS485 RE Receiver Enable Signal State
 * @note	none
 * @param	uart		: Pointer to uart resources structure
 * @param	arg		: 0=disable, 1=enable
 * @retval	\ref execution_status
 */
int32_t uart_set_rs485_re_en (uart_resources_t *uart, uint32_t arg)
{
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;
	uint32_t mode;

	if( (arg != UART_RE_EN_DISABLE) && (arg != UART_RE_EN_ENABLE) )
		return ARM_DRIVER_ERROR_PARAMETER;

	if(arg == UART_RE_EN_DISABLE)
	{
		/* Disable the RE Receiver Enable signal. */
		uart_reg_ptr->re_en = UART_RE_EN_DISABLE;
	}

	if(arg == UART_RE_EN_ENABLE)
	{
		/* Special check only for Software control Half-Duplex Mode as DE and RE are mutually exclusive here. */

		/* Check rs485 transfer mode. */
		mode = uart_get_rs485_transfer_mode(uart);

		if(mode == UART_RS485_SW_CONTROL_HALF_DULPLX_MODE)
		{
			/* In Software control Half-Duplex Mode DE and RE are mutually exclusive.
			 * so anyone either DE or RE can be enable at a time.
			 */

			/* in S/W Half-Duplex Mode first disable DE signal before enabling RE signal. */
			/* disable DE. */
			uart_reg_ptr->de_en = UART_DE_EN_DISABLE;
		}

		/* enable RE Receiver Enable signal. */
		uart_reg_ptr->re_en = UART_RE_EN_ENABLE;
	}

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_rs485_configuration (uart_resources_t *uart)
 * @brief	uart RS485 configuration from RTE_device.h
 * @note	none
 * @param	uart	: Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t uart_rs485_configuration (uart_resources_t *uart)
{
	int32_t ret = ARM_DRIVER_OK;

	/* uart RS485 configuration from RTE_device.h */

	/* uart set RS485 transfer mode */
	ret = uart_set_rs485_transfer_mode(uart, uart->rs485_cfg->rs485_transfer_mode);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* uart set RS485 Driver Enable DE Assertion Time (8-bit) */
	ret = uart_set_rs485_de_assertion_time(uart, uart->rs485_cfg->rs485_de_assertion_time_8bit);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* uart set RS485 Driver Enable DE De-Assertion Time (8-bit) */
	ret = uart_set_rs485_de_deassertion_time(uart, uart->rs485_cfg->rs485_de_deassertion_time_8bit);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* uart set RS485 Turn Around Time TAT for Driver Enable DE to Receive Enable RE */
	ret = uart_set_rs485_de_to_re_turn_around_time(uart, uart->rs485_cfg->rs485_de_to_re_turn_around_time_16bit);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* uart set RS485 Turn Around Time TAT for Receive Enable RE to Driver Enable DE */
	ret = uart_set_rs485_re_to_de_turn_around_time(uart, uart->rs485_cfg->rs485_re_to_de_turn_around_time_16bit);
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ret;
}

/**
 * @fn		int32_t uart_enable_rs485 (uart_resources_t *uart)
 * @brief	enable uart RS485 mode with default configuration as per RTE_device.h
 * @note	none
 * @param	uart	: Pointer to uart resources structure
 * @retval	\ref execution_status
 */
int32_t uart_enable_rs485 (uart_resources_t *uart)
{
	int32_t ret = ARM_DRIVER_OK;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* reset TCR transceiver control register. */
	uart_reg_ptr->tcr = 0;

	/* enable RS485 mode in TCR transceiver control register. */
	uart_reg_ptr->tcr |= UART_TCR_RS485_ENABLE;

	/* de polarity: active high, re polarity: active low/high. */
	uart_reg_ptr->tcr |= UART_TCR_DE_POL_ACTIVE_HIGH;
	uart_reg_ptr->tcr |= UART_TCR_RE_POL_ACTIVE_HIGH;

	ret = uart_rs485_configuration (uart);
	return ret;
}

/**
 * @fn		void uart_disable_rs485 (uart_resources_t *uart)
 * @brief	disable uart RS485 mode
 * @note	none
 * @param	uart: Pointer to uart resources structure
 * @retval	none
 */
void uart_disable_rs485 (uart_resources_t *uart)
{
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;

	/* disable RS485 mode in TCR transceiver control register. */
	uart_reg_ptr->tcr &= ~UART_TCR_RS485_ENABLE;
}

#endif /* ---------------------- END of RS485 Functions --------------------------*/

/**
 * @fn		int32_t uart_configure_clocksource (bool enable, uart_resources_t *uart)
 * @brief	uart peripheral clock configuration.
 * @note	none
 * @param	enable: if enable then update selected Clock source
 * @param	uart:	Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t uart_configure_clocksource (bool enable, uart_resources_t *uart)
{
	uint32_t clock_select = 0;

	if (enable)
	{
		if (uart->cfg->clk_source == UART_CLK_SOURCE_0)
			clock_select = UART_CLK_38P4MHZ;   /* 38.4 Mhz */
		else if (uart->cfg->clk_source == UART_CLK_SOURCE_1)
			clock_select = UART_CLK_100MHZ;    /* 100 Mhz */
		else if (uart->cfg->clk_source == UART_CLK_SOURCE_2)
			clock_select = UART_CLK_20MHZ;     /* 20 Mhz fpga */
		else
			return ARM_DRIVER_ERROR_PARAMETER;

		/* update the peripheral clock frequency. */
		uart->clk = (uint32_t)clock_select;
	}

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_configure_control_reg (bool enable, uart_resources_t *uart)
 * @brief	uart EXPMST0 control register configuration
 *			This function will configure
 *			 - Enable bits for selected UART Clock Source
 *			 - Enable selected UART Module
 * @note	none
 * @param	enable : enable/disable UART module
 * @param	uart   : Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t uart_configure_control_reg (bool enable, uart_resources_t *uart)
{

#define BYPASS_CLK       1
#define UART_CTRL_OFFSET (0x08) /* UART ctrl offset */

#if BYPASS_CLK /* bypass clock gating */
	__IOM uint32_t *clkreg_expmst0 = (uint32_t *) CFGMST0_BASE;
	*clkreg_expmst0 = (1 << 0) | (1 << 4);
#endif

	/* uart EXPMST0 config settings */
	__IOM uint32_t *reg_uart_ctrl = (uint32_t *) (CFGMST0_BASE + UART_CTRL_OFFSET);
	uint8_t uart_instance_num = uart->instance_num;

	if(enable)
	{
		/* UART clock select. bits 8-15. (one bit for each instance.)
		 *      0: 38.4 MHz
		 *      1: APB bus clock, 100 MHz
		 */
		if (uart->cfg->clk_source == UART_CLK_SOURCE_0)         /* 38.4 Mhz */
		{
			*reg_uart_ctrl &= ( ~ ( (1 << uart_instance_num) << 8 ) );
		}
		else if (uart->cfg->clk_source == UART_CLK_SOURCE_1)    /* 100 Mhz */
		{
			*reg_uart_ctrl |= ( (1 << uart_instance_num) << 8 );
		}
		else if (uart->cfg->clk_source == UART_CLK_SOURCE_2)    /* 20 Mhz fpga */
		{
			; /* do nothing for fpga clock source. */
		}
		else
		{
			return ARM_DRIVER_ERROR;
		}

		/* Enable selected UART module (instance). bits 0-7. (one bit for each instance.) */
		*reg_uart_ctrl |= (1 << uart_instance_num);
	}
	else /* Disable */
	{
		/* Disable selected UART module (instance). bits 0-7. (one bit for each instance.) */
		*reg_uart_ctrl &= ( ~ (1 << uart_instance_num) );
	}

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_initialize (uart_resources_t *uart)
 * @brief	initialize uart
 * @note	this function will
 *				-set tx/rx fifo length,
 *				-reset uart,
 *				-disable the interrupt,
 *				-intialize all the variable to 0
 * @param	uart: Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t uart_initialize (uart_resources_t *uart)
{
	uart_info_t     *uart_info_ptr = uart->info;
	uart_reg_set_t  *uart_reg_ptr  = (uart_reg_set_t *)uart->reg_base;
	int32_t ret = ARM_DRIVER_OK;

	/* UART FIFO Depth is 32. */
	uart_info_ptr->tx_fifo_length = UART_FIFO_DEPTH;
	uart_info_ptr->rx_fifo_length = UART_FIFO_DEPTH;

	/* FIXME : Do the pin muxing based on the RTE Configuration. */

	/* uart EXPMST0 configuration, Enable clock source and selected UART Module. */
	ret = uart_configure_control_reg(true, uart);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* Configure the UART Peripheral Clock */
	ret = uart_configure_clocksource (true, uart);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* reset the uart. */
	uart_software_reset(uart_reg_ptr);

	/* enable uart fifo fcr FIFO control register */
	uart_reg_ptr->iir_fcr = UART_FCR_FIFO_ENABLE;

	/* disable all uart interrupt ier interrupt enable register */
	uart_reg_ptr->ier_dlh = 0x0;

	/* disable TX/RX interrupt. */
	uart_disable_interrupt(uart);

	/* initialize the tx_buffer */
	uart_info_ptr->transfer.tx_buf              = NULL;
	uart_info_ptr->transfer.tx_total_num        = 0U;
	uart_info_ptr->transfer.tx_curr_cnt         = 0U;

	/* clear TX busy flag */
	uart_info_ptr->transfer.tx_busy             = UART_STATUS_FREE;

	/* initialize the rx_buffer */
	uart_info_ptr->transfer.rx_buf              = NULL;
	uart_info_ptr->transfer.rx_total_num        = 0U;
	uart_info_ptr->transfer.rx_curr_cnt         = 0U;

	/* clear Receive busy flag */
	uart_info_ptr->rx_status.rx_busy            = UART_STATUS_FREE;

	/* Clear RX status */
	uart_info_ptr->rx_status.rx_break           = 0U;
	uart_info_ptr->rx_status.rx_framing_error   = 0U;
	uart_info_ptr->rx_status.rx_overflow        = 0U;
	uart_info_ptr->rx_status.rx_parity_error    = 0U;

#if RS485_SUPPORT  /* Check if UART RS485 mode is enabled? */
	if(uart->rs485_cfg->rs485_control == UART_RS485_MODE_ENABLE)
	{
		/* enable UART RS485 mode */
		ret = uart_enable_rs485(uart);
		if(ret != ARM_DRIVER_OK)
			return ret;
	}
	/* else   RS485 mode is disable. */
#endif //END of RS485_SUPPORT

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t uart_uninitialize (uart_resources_t *uart)
 * @brief	uninitialize uart
 * @note	this function will
 *				-disable the interrupt,
 *				-abort TX/RX,
 *				-reset tx/rx fifo,
 *				-set baudrate to 0
 *				-initialize all the variable to 0
 * @note	needs to initialize first if wants to use it again.
 * @param	uart: Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t uart_uninitialize (uart_resources_t *uart)
{
	uart_info_t *uart_info_ptr = uart->info;
	int32_t ret = ARM_DRIVER_OK;

	/* FIXME : Do the pin muxing based on the RTE Configuration. */

	/* disable transmit and Receive interrupt. */
	uart_disable_interrupt(uart);

	/* abort tx and rx */
	uart_abort_tx(uart);
	uart_abort_rx(uart);

	/* set baud to 0 */
	uart_set_baudrate(uart, 0);

	/* initialize all variables to 0 */

	/* initialize the tx_buffer */
	uart_info_ptr->transfer.tx_buf              = NULL;
	uart_info_ptr->transfer.tx_total_num        = 0U;
	uart_info_ptr->transfer.tx_curr_cnt         = 0U;

	/* clear TX busy flag */
	uart_info_ptr->transfer.tx_busy             = UART_STATUS_FREE;

	/* initialize the rx_buffer */
	uart_info_ptr->transfer.rx_buf              = NULL;
	uart_info_ptr->transfer.rx_total_num        = 0U;
	uart_info_ptr->transfer.rx_curr_cnt         = 0U;

	/* clear Receive active flag */
	uart_info_ptr->rx_status.rx_busy            = UART_STATUS_FREE;

	/* Clear RX status */
	uart_info_ptr->rx_status.rx_break           = 0U;
	uart_info_ptr->rx_status.rx_framing_error   = 0U;
	uart_info_ptr->rx_status.rx_overflow        = 0U;
	uart_info_ptr->rx_status.rx_parity_error    = 0U;

#if RS485_SUPPORT  /* Check if UART RS485 mode is enabled? */
	if(uart->rs485_cfg->rs485_control == UART_RS485_MODE_ENABLE)
	{
		/* disable UART RS485 mode */
		uart_disable_rs485(uart);
	}
#endif //END of RS485_SUPPORT

	/* uart EXPMST0 configuration, Disable the selected UART Module. */
	ret = uart_configure_control_reg(false, uart);
	return ret;
}

/**
 * @fn		int32_t ARM_USART_PowerControl (ARM_POWER_STATE    state,
                                            uart_resources_t  *uart)
 * @brief	CMSIS-Driver uart power control
 * @note	none
 * @param	state	: Power state
 * @param	uart	: Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t ARM_USART_PowerControl (ARM_POWER_STATE   state,
                                       uart_resources_t *uart)
{
	switch (state)
	{
		case ARM_POWER_OFF:

			/* Disable uart IRQ */
			NVIC_DisableIRQ (uart->irq_num);

			/* Clear Any Pending IRQ*/
			NVIC_ClearPendingIRQ (uart->irq_num);

			/* Reset the power status of uart. */
			uart->info->flags &= ~UART_FLAG_POWERED;
			break;

		case ARM_POWER_FULL:

			if ( (uart->info->flags & UART_FLAG_INITIALIZED) == 0U)
			{
				/* error: Driver is not initialized /ref ARM_USART_Initialize */
				return ARM_DRIVER_ERROR;
			}

			if ( (uart->info->flags & UART_FLAG_POWERED) )
			{
				return ARM_DRIVER_OK;
			}

			/* Enable uart IRQ*/
			NVIC_ClearPendingIRQ (uart->irq_num);
			NVIC_SetPriority(uart->irq_num, uart->irq_priority);
			NVIC_EnableIRQ (uart->irq_num);

			/* Set the power flag enabled */
			uart->info->flags |= UART_FLAG_POWERED;
			break;

		case ARM_POWER_LOW:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t ARM_USART_Initialize (ARM_USART_SignalEvent_t   cb_event,
                                          uart_resources_t         *uart)
 * @brief	CMSIS-Driver uart initialize
 * @note	none
 * @param	cb_event	: Pointer to USART Event \ref ARM_USART_SignalEvent
 * @param	uart		: Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t ARM_USART_Initialize (ARM_USART_SignalEvent_t   cb_event,
                                     uart_resources_t         *uart)
{
	int ret = ARM_DRIVER_OK;

	if (uart->info->flags & UART_FLAG_INITIALIZED)
	{
		/* Driver is already initialized */
		return ARM_DRIVER_OK;
	}

	/* set the user callback event. */
	uart->info->cb_event = cb_event;

	/* calling uart initialize lower level api */
	ret = uart_initialize(uart);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* set the flag as initialized. */
	uart->info->flags = UART_FLAG_INITIALIZED;

#if RS485_SUPPORT  /* Check if UART RS485 mode is enabled? */
	if(uart->rs485_cfg->rs485_control == UART_RS485_MODE_ENABLE)
	{
		/* set the flag as RS485 enabled. */
		uart->info->flags |= UART_FLAG_RS485_ENABLE;
	}
#endif

	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t ARM_USART_Uninitialize (uart_resources_t *uart)
 * @brief	CMSIS-Driver uart uninitialize
 * @note	none
 * @param	uart		: Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t ARM_USART_Uninitialize (uart_resources_t *uart)
{
	int ret = ARM_DRIVER_OK;

	if ( (uart->info->flags & UART_FLAG_INITIALIZED) == 0)
	{
		/* Driver is not initialized */
		return ARM_DRIVER_OK;
	}

	/* set the user callback event to NULL. */
	uart->info->cb_event = NULL;

	/* Disable the power. */
	ARM_USART_PowerControl (ARM_POWER_OFF, uart);

	/* calling uart uninitialize lower level api */
	ret = uart_uninitialize(uart);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* Reset UART flags. */
	uart->info->flags = 0U;
	return ARM_DRIVER_OK;
}

/**
 * @fn		int32_t ARM_USART_Send (const void       *data,
                                    uint32_t          num,
                                    uart_resources_t *uart)
 * @brief	CMSIS-Driver uart send
 *          Start sending data to UART transmitter.
 * @note	tx flag UART_FLAG_TX_ENABLED should be enabled first /ref ARM_USART_CONTROL_TX
 * @param	data    : Pointer to buffer with data to send to USART transmitter
 * @param	num     : Number of data items to send
 * @param	uart    : Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t ARM_USART_Send (const void       *data,
                               uint32_t          num,
                               uart_resources_t *uart)
{
	int ret = ARM_DRIVER_OK;

	if ((data == NULL) || (num == 0U)) {
		/* Invalid parameters */
		return ARM_DRIVER_ERROR_PARAMETER;
	}

	if (!(uart->info->flags & UART_FLAG_POWERED))
		return ARM_DRIVER_ERROR;

	if ((uart->info->flags & UART_FLAG_TX_ENABLED) == 0U) {
	    /* error: UART is not configured (mode not selected)
	     * tx flag UART_FLAG_TX_ENABLED should be enabled first /ref ARM_USART_CONTROL_TX
	     */
	    return ARM_DRIVER_ERROR;
	}

	/* fill the user input details for uart send transfer structure
	 * and enable the send interrupt.
	 */
	ret = uart_enable_send_interrupt(uart, data, num);  /* non-blocked */

	return ret;
}

/**
 * @fn		int32_t ARM_USART_Receive (void             *data,
                                       uint32_t          num,
                                       uart_resources_t *uart)
 * @brief	CMSIS-Driver uart receive
 *          Start receiving data from USART receiver.
 * @note	none
 * @param	data                       : Pointer to buffer for data to receive from UART receiver
 * @param	num                        : Number of data items to receive
 * @param	uart                       : Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t ARM_USART_Receive (void             *data,
                                  uint32_t          num,
                                  uart_resources_t *uart)
{
	int ret = ARM_DRIVER_OK;

	if ((data == NULL) || (num == 0U)) {
		/* Invalid parameters */
		return ARM_DRIVER_ERROR_PARAMETER;
	}

	if (!(uart->info->flags & UART_FLAG_POWERED))
		return ARM_DRIVER_ERROR;

	if ((uart->info->flags & UART_FLAG_RX_ENABLED) == 0U) {
	    /* error: UART is not configured (mode not selected) */
	    return ARM_DRIVER_ERROR;
	}

	/* enable the receiver interrupt. */
	ret = uart_enable_receive_interrupt(uart, data, num);  /* non-blocked */

	return ret;
}

/**
 * @fn		int32_t ARM_USART_Transfer (const void       *data_out,
                                        void             *data_in,
                                        uint32_t          num,
                                        uart_resources_t *uart)
 * @brief	CMSIS-Driver uart transfer
 *          Start sending/receiving data to/from UART transmitter/receiver.
 * @note	This function used in synchronous mode, currently our driver is not supporting it.
 * @param	data_out    : Pointer to buffer with data to send to USART transmitter
 * @param	data_in     : Pointer to buffer for data to receive from USART receiver
 * @param	num         : Number of data items to transfer
 * @param	uart        : Pointer to uart resources structure
 * @retval	ARM_DRIVER_ERROR_UNSUPPORTED
 */
static int32_t ARM_USART_Transfer (const void       *data_out,
                                   void             *data_in,
                                   uint32_t          num,
                                   uart_resources_t *uart)
{
	/* Added to fix Warning: unused parameter [-Wunused-parameter] */
	ARG_UNUSED(data_out);
	ARG_UNUSED(data_in);
	ARG_UNUSED(num);
	ARG_UNUSED(uart);

	/* Use with Synchronous mode only */
	/* Not supported as our driver is only Asynchronous. */
	return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @fn		uint32_t ARM_USART_GetTxCount (uart_resources_t *uart)
 * @brief	CMSIS-Driver uart get transmitted data count
 * @note	none
 * @param	uart	: Pointer to uart resources structure
 * @retval	transmitted data count
 */
static uint32_t ARM_USART_GetTxCount (uart_resources_t *uart)
{
	uint32_t cnt;
	cnt = uart->info->transfer.tx_curr_cnt;
	return cnt;
}

/**
 * @fn		uint32_t ARM_USART_GetRxCount (uart_resources_t *uart)
 * @brief	CMSIS-Driver uart get received data count
 * @note	none
 * @param	uart	: Pointer to uart resources structure
 * @retval	received data count
 */
static uint32_t ARM_USART_GetRxCount (uart_resources_t *uart)
{
	uint32_t cnt;
	cnt = uart->info->transfer.rx_curr_cnt;
	return cnt;
}

/**
 * @fn		int32_t ARM_USART_Control (uint32_t          control,
                                       uint32_t          arg,
                                       uart_resources_t *uart)
 * @brief	CMSIS-Driver uart control.
 *          Control USART Interface.
 * @note	none
 * @param	control   : Operation \ref Driver_USART.h: USART Control Codes
 * @param	arg       : Argument of operation (optional)
 * @param	uart      : Pointer to uart resources structure
 * @retval	common \ref execution_status and driver specific \ref usart_execution_status
 */
static int32_t ARM_USART_Control (uint32_t           control,
                                  uint32_t           arg,
                                  uart_resources_t  *uart)
{
	int32_t ret = ARM_DRIVER_OK;

	/* if not powered? then return error */
	if(!(uart->info->flags & UART_FLAG_POWERED))
		return ARM_DRIVER_ERROR;

	switch (control & ARM_USART_CONTROL_Msk)
	{
		case ARM_USART_MODE_ASYNCHRONOUS:
			/* uart asynchronous mode */

			/* set uart asynchronous mode parameters as per arg
			 * set baudrate, data length, parity, stop bits,
			 * and flow control.
			 */
			ret = uart_set_asynchronous_mode(uart, control, arg);
			break;

		case ARM_USART_CONTROL_TX:
			/* uart enable/disable transmitter */

			if (arg) /* uart enable transmitter */
			{

#if RS485_SUPPORT  /* Check if UART RS485 mode is enabled? */
				if (uart->info->flags & UART_FLAG_RS485_ENABLE)
				{
					/* uart RS485 mode is enabled, now enable Driver-Enable DE_EN signal. */
					ret = uart_set_rs485_de_en(uart, UART_DE_EN_ENABLE);
				}
#endif

				/* uart set transmitter trigger level as per RTE configuration */
				ret = uart_set_tx_trigger(uart, uart->cfg->tx_fifo_trg_lvl);

				/* setting TX flag to enabled. */
				 uart->info->flags |= UART_FLAG_TX_ENABLED;
			}
			else /* uart disable transmitter */
			{

#if RS485_SUPPORT /* Check if UART RS485 mode is enabled? */
				if (uart->info->flags & UART_FLAG_RS485_ENABLE)
				{
					/* uart RS485 mode is enabled, now disable Driver-Enable DE_EN signal. */
					ret = uart_set_rs485_de_en(uart, UART_DE_EN_DISABLE);
				}
#endif
				/* clear TX flag to enabled. */
				uart->info->flags &= ~UART_FLAG_TX_ENABLED;
			}
			break;

		case ARM_USART_CONTROL_RX:
			/* uart enable/disable receiver */

			if (arg) /* uart enable receiver */
			{

#if RS485_SUPPORT  /* Check if UART RS485 mode is enabled? */
				if (uart->info->flags & UART_FLAG_RS485_ENABLE)
				{
					/* uart RS485 mode is enabled, now enable Receiver-Enable RE_EN signal. */
					ret = uart_set_rs485_re_en(uart, UART_RE_EN_ENABLE);
				}
#endif
				/* uart set receiver trigger level as per RTE configuration */
				ret = uart_set_rx_trigger(uart, uart->cfg->rx_fifo_trg_lvl);

				/* setting RX flag to enabled. */
				uart->info->flags |= UART_FLAG_RX_ENABLED;
			}
			else /* uart disable receiver */
			{

#if RS485_SUPPORT  /* Check if UART RS485 mode is enabled? */
				if (uart->info->flags & UART_FLAG_RS485_ENABLE)
				{
					/* uart RS485 mode is enabled, now disable Receiver-Enable RE_EN signal. */
					ret = uart_set_rs485_re_en(uart, UART_RE_EN_DISABLE);
				}
#endif

				/* clear RX flag to enabled. */
				uart->info->flags &= ~UART_FLAG_RX_ENABLED;
			}
			break;

		case ARM_USART_ABORT_SEND:
			/* uart abort transmitter */
			if ((uart->info->flags & UART_FLAG_TX_ENABLED) == 0U)
			{
				/* error: UART transmitter is not enabled
				 * ref ARM_USART_CONTROL_TX
				 */
				return ARM_DRIVER_ERROR;
			}

			uart_abort_tx(uart);
			break;

		case ARM_USART_ABORT_RECEIVE:
			/* uart abort receiver */
			if ((uart->info->flags & UART_FLAG_RX_ENABLED) == 0U)
			{
				/* error: UART receiver is not enabled
				 * ref ARM_USART_CONTROL_RX
				 */
				return ARM_DRIVER_ERROR;
			}

			uart_abort_rx(uart);
			break;

		case ARM_USART_CONTROL_BREAK:
			/* set/clear break */

			if (arg)
			{
				uart_set_break_control(uart);
			}
			else
			{
				uart_clear_break_control(uart);
			}
			break;
		/* Unsupported command */
		default:
			ret =  ARM_DRIVER_ERROR_UNSUPPORTED;
			break;
	}
	return ret;
}

/**
 * @fn		ARM_USART_STATUS ARM_USART_GetStatus (uart_resources_t *uart)
 * @brief	CMSIS-Driver uart get status
 * @note	none.
 * @param	uart	: Pointer to uart resources structure
 * @retval	ARM_USART_STATUS
 */
static ARM_USART_STATUS ARM_USART_GetStatus (uart_resources_t *uart)
{
	/* -TX/RX busy flag is implemented.
	 * -TX/RX errors are not implemented yet.
	 */

	ARM_USART_STATUS status =
	{
		.tx_busy = uart->info->transfer.tx_busy,
		.rx_busy = uart->info->rx_status.rx_busy,
	};

	return status;
}

/**
 * @fn		int32_t ARM_USART_SetModemControl (ARM_USART_MODEM_CONTROL   control,
                                               uart_resources_t         *uart)
 * @brief	CMSIS-Driver Set UART Modem Control line state.
 * @note	not implemented yet.
 * @param	control : \ref ARM_USART_MODEM_CONTROL
 * @param	uart	: Pointer to uart resources structure
 * @retval	\ref execution_status
 */
static int32_t ARM_USART_SetModemControl (ARM_USART_MODEM_CONTROL   control,
                                          uart_resources_t         *uart)
{
	/* Added to fix Warning: unused parameter [-Wunused-parameter] */
	ARG_UNUSED(control);
	ARG_UNUSED(uart);

	/* not implemented yet. */
	return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
 * @fn		ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus (uart_resources_t *uart)
 * @brief	CMSIS-Driver uart Get UART Modem Status lines state.
 * @note	not implemented yet.
 * @param	uart	: Pointer to uart resources structure
 * @retval	modem status \ref ARM_USART_MODEM_STATUS
 */
static ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus (uart_resources_t *uart)
{
	/* Added to fix Warning: unused parameter [-Wunused-parameter] */
	ARG_UNUSED(uart);

	/* not implemented yet. */
	ARM_USART_MODEM_STATUS status = {0, 0, 0, 0, 0};
	return status;
}

/**
 * @fn		void ARM_USART_IRQHandler (uart_resources_t *uart)
 * @brief	CMSIS-Driver uart interrupt handler
 * @note	none
 * @param	uart	: Pointer to uart resources structure
 * @retval	none
 */
static void ARM_USART_IRQHandler (uart_resources_t *uart)
{
	uart_irq_handler (uart);
}

// End UART Interface


/* UART0 Driver Instance */
#if (RTE_UART0)

static uart_config_info_t uart0_config =
{
	.clk_source      = RTE_UART0_CLK_SOURCE,
	.rx_fifo_trg_lvl = RTE_UART0_RX_TRIG_LVL,
	.tx_fifo_trg_lvl = RTE_UART0_TX_TRIG_LVL,
};
static uart_info_t uart0_info = {0};

#if RS485_SUPPORT     /* UART0 does not support RS485. */
static uart_config_rs485_info_t uart0_rs485_config = {0};
#endif

/* UART0 Driver Resources */
static uart_resources_t UART0_Resources =
{
	.reg_base       = (uint32_t)UART0_BASE,
	.clk            = 0,
	.cfg            = &uart0_config,
	.info           = &uart0_info,
	.irq_num        = (IRQn_Type) UART0_IRQ,
	.irq_priority   = (uint32_t)RTE_UART0_IRQ_PRI,
	.instance_num   = UART0_INSTANCE_NUM,

#if RS485_SUPPORT
	.rs485_cfg      = &uart0_rs485_config,
#endif
};


static int32_t UART0_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	return (ARM_USART_Initialize(cb_event,  &UART0_Resources));
}

static int32_t UART0_Uninitialize(void)
{
	return (ARM_USART_Uninitialize(&UART0_Resources));
}

static int32_t UART0_PowerControl(ARM_POWER_STATE state)
{
	return (ARM_USART_PowerControl(state, &UART0_Resources));
}

static int32_t UART0_Send(void const * const p_data, uint32_t num)
{
	return (ARM_USART_Send(p_data, num , &UART0_Resources));
}

static int32_t UART0_Receive(void * const p_data, uint32_t num)
{
	return (ARM_USART_Receive(p_data, num, &UART0_Resources));
}

static int32_t UART0_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
{
	return (ARM_USART_Transfer(p_data_out, p_data_in, num, &UART0_Resources));
}

static uint32_t UART0_GetTxCount(void)
{
	return (ARM_USART_GetTxCount(&UART0_Resources));
}

static uint32_t UART0_GetRxCount(void)
{
	return (ARM_USART_GetRxCount(&UART0_Resources));
}

static int32_t UART0_Control(uint32_t control, uint32_t arg)
{
	return (ARM_USART_Control(control, arg, &UART0_Resources));
}

static ARM_USART_STATUS UART0_GetStatus(void)
{
	return (ARM_USART_GetStatus(&UART0_Resources));
}

static ARM_USART_MODEM_STATUS UART0_GetModemStatus(void)
{
	return (ARM_USART_GetModemStatus(&UART0_Resources));
}

static int32_t UART0_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return (ARM_USART_SetModemControl(control, &UART0_Resources));
}

void UART0_IRQHandler (void)
{
	ARM_USART_IRQHandler (&UART0_Resources);
}

extern ARM_DRIVER_USART Driver_USART0;
ARM_DRIVER_USART Driver_USART0 =
{
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    UART0_Initialize,
    UART0_Uninitialize,
    UART0_PowerControl,
    UART0_Send,
    UART0_Receive,
    UART0_Transfer,
    UART0_GetTxCount,
    UART0_GetRxCount,
    UART0_Control,
    UART0_GetStatus,
    UART0_SetModemControl,
    UART0_GetModemStatus
};
#endif /* RTE_UART0 */


/* UART1 Driver Instance */
#if (RTE_UART1)

static uart_config_info_t uart1_config =
{
	.clk_source      = RTE_UART1_CLK_SOURCE,
	.rx_fifo_trg_lvl = RTE_UART1_RX_TRIG_LVL,
	.tx_fifo_trg_lvl = RTE_UART1_TX_TRIG_LVL,
};
static uart_info_t uart1_info = {0};

#if RS485_SUPPORT     /* UART1 does not support RS485. */
static uart_config_rs485_info_t uart1_rs485_config = {0};
#endif

/* UART1 Driver Resources */
static uart_resources_t UART1_Resources =
{
	.reg_base       = (uint32_t)UART1_BASE,
	.clk            = 0,
	.cfg            = &uart1_config,
	.info           = &uart1_info,
	.irq_num        = (IRQn_Type) UART1_IRQ,
	.irq_priority   = (uint32_t)RTE_UART1_IRQ_PRI,
	.instance_num   = UART1_INSTANCE_NUM,

#if RS485_SUPPORT
	.rs485_cfg      = &uart1_rs485_config,
#endif
};


static int32_t UART1_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	return (ARM_USART_Initialize(cb_event,  &UART1_Resources));
}

static int32_t UART1_Uninitialize(void)
{
	return (ARM_USART_Uninitialize(&UART1_Resources));
}

static int32_t UART1_PowerControl(ARM_POWER_STATE state)
{
	return (ARM_USART_PowerControl(state, &UART1_Resources));
}

static int32_t UART1_Send(void const * const p_data, uint32_t num)
{
	return (ARM_USART_Send(p_data, num , &UART1_Resources));
}

static int32_t UART1_Receive(void * const p_data, uint32_t num)
{
	return (ARM_USART_Receive(p_data, num, &UART1_Resources));
}

static int32_t UART1_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
{
	return (ARM_USART_Transfer(p_data_out, p_data_in, num, &UART1_Resources));
}

static uint32_t UART1_GetTxCount(void)
{
	return (ARM_USART_GetTxCount(&UART1_Resources));
}

static uint32_t UART1_GetRxCount(void)
{
	return (ARM_USART_GetRxCount(&UART1_Resources));
}

static int32_t UART1_Control(uint32_t control, uint32_t arg)
{
	return (ARM_USART_Control(control, arg, &UART1_Resources));
}

static ARM_USART_STATUS UART1_GetStatus(void)
{
	return (ARM_USART_GetStatus(&UART1_Resources));
}

static ARM_USART_MODEM_STATUS UART1_GetModemStatus(void)
{
	return (ARM_USART_GetModemStatus(&UART1_Resources));
}

static int32_t UART1_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return (ARM_USART_SetModemControl(control, &UART1_Resources));
}

void UART1_IRQHandler (void)
{
	ARM_USART_IRQHandler (&UART1_Resources);
}

extern ARM_DRIVER_USART Driver_USART1;
ARM_DRIVER_USART Driver_USART1 =
{
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    UART1_Initialize,
    UART1_Uninitialize,
    UART1_PowerControl,
    UART1_Send,
    UART1_Receive,
    UART1_Transfer,
    UART1_GetTxCount,
    UART1_GetRxCount,
    UART1_Control,
    UART1_GetStatus,
    UART1_SetModemControl,
    UART1_GetModemStatus
};
#endif /* RTE_UART1 */


/* UART2 Driver Instance */
#if (RTE_UART2)

static uart_config_info_t uart2_config =
{
	.clk_source      = RTE_UART2_CLK_SOURCE,
	.rx_fifo_trg_lvl = RTE_UART2_RX_TRIG_LVL,
	.tx_fifo_trg_lvl = RTE_UART2_TX_TRIG_LVL,
};
static uart_info_t uart2_info = {0};

#if RS485_SUPPORT     /* UART2 does not support RS485. */
static uart_config_rs485_info_t uart2_rs485_config = {0};
#endif

/* UART2 Driver Resources */
static uart_resources_t UART2_Resources =
{
	.reg_base       = (uint32_t)UART2_BASE,
	.clk            = 0,
	.cfg            = &uart2_config,
	.info           = &uart2_info,
	.irq_num        = (IRQn_Type) UART2_IRQ,
	.irq_priority   = (uint32_t)RTE_UART2_IRQ_PRI,
	.instance_num   = UART2_INSTANCE_NUM,

#if RS485_SUPPORT
	.rs485_cfg      = &uart2_rs485_config,
#endif

};


static int32_t UART2_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	return (ARM_USART_Initialize(cb_event,  &UART2_Resources));
}

static int32_t UART2_Uninitialize(void)
{
	return (ARM_USART_Uninitialize(&UART2_Resources));
}

static int32_t UART2_PowerControl(ARM_POWER_STATE state)
{
	return (ARM_USART_PowerControl(state, &UART2_Resources));
}

static int32_t UART2_Send(void const * const p_data, uint32_t num)
{
	return (ARM_USART_Send(p_data, num , &UART2_Resources));
}

static int32_t UART2_Receive(void * const p_data, uint32_t num)
{
	return (ARM_USART_Receive(p_data, num, &UART2_Resources));
}

static int32_t UART2_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
{
	return (ARM_USART_Transfer(p_data_out, p_data_in, num, &UART2_Resources));
}

static uint32_t UART2_GetTxCount(void)
{
	return (ARM_USART_GetTxCount(&UART2_Resources));
}

static uint32_t UART2_GetRxCount(void)
{
	return (ARM_USART_GetRxCount(&UART2_Resources));
}

static int32_t UART2_Control(uint32_t control, uint32_t arg)
{
	return (ARM_USART_Control(control, arg, &UART2_Resources));
}

static ARM_USART_STATUS UART2_GetStatus(void)
{
	return (ARM_USART_GetStatus(&UART2_Resources));
}

static ARM_USART_MODEM_STATUS UART2_GetModemStatus(void)
{
	return (ARM_USART_GetModemStatus(&UART2_Resources));
}

static int32_t UART2_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return (ARM_USART_SetModemControl(control, &UART2_Resources));
}

void UART2_IRQHandler (void)
{
	ARM_USART_IRQHandler (&UART2_Resources);
}

extern ARM_DRIVER_USART Driver_USART2;
ARM_DRIVER_USART Driver_USART2 =
{
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    UART2_Initialize,
    UART2_Uninitialize,
    UART2_PowerControl,
    UART2_Send,
    UART2_Receive,
    UART2_Transfer,
    UART2_GetTxCount,
    UART2_GetRxCount,
    UART2_Control,
    UART2_GetStatus,
    UART2_SetModemControl,
    UART2_GetModemStatus
};
#endif /* RTE_UART2 */


/* UART3 Driver Instance */
#if (RTE_UART3)

static uart_config_info_t uart3_config =
{
	.clk_source      = RTE_UART3_CLK_SOURCE,
	.rx_fifo_trg_lvl = RTE_UART3_RX_TRIG_LVL,
	.tx_fifo_trg_lvl = RTE_UART3_TX_TRIG_LVL,
};
static uart_info_t uart3_info = {0};

#if RS485_SUPPORT     /* UART3 does not support RS485. */
static uart_config_rs485_info_t uart3_rs485_config = {0};
#endif

/* UART3 Driver Resources */
static uart_resources_t UART3_Resources =
{
	.reg_base       = (uint32_t)UART3_BASE,
	.clk            = 0,
	.cfg            = &uart3_config,
	.info           = &uart3_info,
	.irq_num        = (IRQn_Type) UART3_IRQ,
	.irq_priority   = (uint32_t)RTE_UART3_IRQ_PRI,
	.instance_num   = UART3_INSTANCE_NUM,

#if RS485_SUPPORT
	.rs485_cfg      = &uart3_rs485_config,
#endif
};


static int32_t UART3_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	return (ARM_USART_Initialize(cb_event,  &UART3_Resources));
}

static int32_t UART3_Uninitialize(void)
{
	return (ARM_USART_Uninitialize(&UART3_Resources));
}

static int32_t UART3_PowerControl(ARM_POWER_STATE state)
{
	return (ARM_USART_PowerControl(state, &UART3_Resources));
}

static int32_t UART3_Send(void const * const p_data, uint32_t num)
{
	return (ARM_USART_Send(p_data, num , &UART3_Resources));
}

static int32_t UART3_Receive(void * const p_data, uint32_t num)
{
	return (ARM_USART_Receive(p_data, num, &UART3_Resources));
}

static int32_t UART3_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
{
	return (ARM_USART_Transfer(p_data_out, p_data_in, num, &UART3_Resources));
}

static uint32_t UART3_GetTxCount(void)
{
	return (ARM_USART_GetTxCount(&UART3_Resources));
}

static uint32_t UART3_GetRxCount(void)
{
	return (ARM_USART_GetRxCount(&UART3_Resources));
}

static int32_t UART3_Control(uint32_t control, uint32_t arg)
{
	return (ARM_USART_Control(control, arg, &UART3_Resources));
}

static ARM_USART_STATUS UART3_GetStatus(void)
{
	return (ARM_USART_GetStatus(&UART3_Resources));
}

static ARM_USART_MODEM_STATUS UART3_GetModemStatus(void)
{
	return (ARM_USART_GetModemStatus(&UART3_Resources));
}

static int32_t UART3_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return (ARM_USART_SetModemControl(control, &UART3_Resources));
}

void UART3_IRQHandler (void)
{
	ARM_USART_IRQHandler (&UART3_Resources);
}

extern ARM_DRIVER_USART Driver_USART3;
ARM_DRIVER_USART Driver_USART3 =
{
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    UART3_Initialize,
    UART3_Uninitialize,
    UART3_PowerControl,
    UART3_Send,
    UART3_Receive,
    UART3_Transfer,
    UART3_GetTxCount,
    UART3_GetRxCount,
    UART3_Control,
    UART3_GetStatus,
    UART3_SetModemControl,
    UART3_GetModemStatus
};
#endif /* RTE_UART3 */


/* UART4 Driver Instance */
#if (RTE_UART4)

static uart_config_info_t uart4_config =
{
	.clk_source      = RTE_UART4_CLK_SOURCE,
	.rx_fifo_trg_lvl = RTE_UART4_RX_TRIG_LVL,
	.tx_fifo_trg_lvl = RTE_UART4_TX_TRIG_LVL,
};

static uart_info_t uart4_info = {0};

#if RS485_SUPPORT /* Check if UART4 RS485 mode is enabled? */
#if RTE_UART4_RS485_ENABLE
static uart_config_rs485_info_t uart4_rs485_config =
{
	.rs485_control                          = RTE_UART4_RS485_ENABLE,
	.rs485_transfer_mode                    = RTE_UART4_RS485_TRANSFER_MODE,
	.rs485_de_assertion_time_8bit           = RTE_UART4_RS485_DE_ASSERTION_TIME_8BIT,
	.rs485_de_deassertion_time_8bit         = RTE_UART4_RS485_DE_DEASSERTION_TIME_8BIT,
	.rs485_de_to_re_turn_around_time_16bit  = RTE_UART4_RS485_DE_TO_RE_TURN_AROUND_TIME_16BIT,
	.rs485_re_to_de_turn_around_time_16bit  = RTE_UART4_RS485_RE_TO_DE_TURN_AROUND_TIME_16BIT,
};
#else
static uart_config_rs485_info_t uart4_rs485_config = {0};
#endif

#endif //END of RS485_SUPPORT

/* UART4 Driver Resources */
static uart_resources_t UART4_Resources =
{
	.reg_base       = (uint32_t)UART4_BASE,
	.clk            = 0,
	.cfg            = &uart4_config,
	.info           = &uart4_info,
	.irq_num        = (IRQn_Type) UART4_IRQ,
	.irq_priority   = (uint32_t)RTE_UART4_IRQ_PRI,
	.instance_num   = UART4_INSTANCE_NUM,

#if RS485_SUPPORT
	.rs485_cfg      = &uart4_rs485_config,
#endif
};


static int32_t UART4_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	return (ARM_USART_Initialize(cb_event,  &UART4_Resources));
}

static int32_t UART4_Uninitialize(void)
{
	return (ARM_USART_Uninitialize(&UART4_Resources));
}

static int32_t UART4_PowerControl(ARM_POWER_STATE state)
{
	return (ARM_USART_PowerControl(state, &UART4_Resources));
}

static int32_t UART4_Send(void const * const p_data, uint32_t num)
{
	return (ARM_USART_Send(p_data, num , &UART4_Resources));
}

static int32_t UART4_Receive(void * const p_data, uint32_t num)
{
	return (ARM_USART_Receive(p_data, num, &UART4_Resources));
}

static int32_t UART4_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
{
	return (ARM_USART_Transfer(p_data_out, p_data_in, num, &UART4_Resources));
}

static uint32_t UART4_GetTxCount(void)
{
	return (ARM_USART_GetTxCount(&UART4_Resources));
}

static uint32_t UART4_GetRxCount(void)
{
	return (ARM_USART_GetRxCount(&UART4_Resources));
}

static int32_t UART4_Control(uint32_t control, uint32_t arg)
{
	return (ARM_USART_Control(control, arg, &UART4_Resources));
}

static ARM_USART_STATUS UART4_GetStatus(void)
{
	return (ARM_USART_GetStatus(&UART4_Resources));
}

static ARM_USART_MODEM_STATUS UART4_GetModemStatus(void)
{
	return (ARM_USART_GetModemStatus(&UART4_Resources));
}

static int32_t UART4_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return (ARM_USART_SetModemControl(control, &UART4_Resources));
}

void UART4_IRQHandler (void)
{
	ARM_USART_IRQHandler (&UART4_Resources);
}

extern ARM_DRIVER_USART Driver_USART4;
ARM_DRIVER_USART Driver_USART4 =
{
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities_WO_RTS_CTS,
    UART4_Initialize,
    UART4_Uninitialize,
    UART4_PowerControl,
    UART4_Send,
    UART4_Receive,
    UART4_Transfer,
    UART4_GetTxCount,
    UART4_GetRxCount,
    UART4_Control,
    UART4_GetStatus,
    UART4_SetModemControl,
    UART4_GetModemStatus
};
#endif /* RTE_UART4 */


/* UART5 Driver Instance */
#if (RTE_UART5)

static uart_config_info_t uart5_config =
{
	.clk_source      = RTE_UART5_CLK_SOURCE,
	.rx_fifo_trg_lvl = RTE_UART5_RX_TRIG_LVL,
	.tx_fifo_trg_lvl = RTE_UART5_TX_TRIG_LVL,
};

static uart_info_t uart5_info = {0};

#if RS485_SUPPORT /* Check if UART5 RS485 mode is enabled? */
#if RTE_UART5_RS485_ENABLE
static uart_config_rs485_info_t uart5_rs485_config =
{
	.rs485_control                          = RTE_UART5_RS485_ENABLE,
	.rs485_transfer_mode                    = RTE_UART5_RS485_TRANSFER_MODE,
	.rs485_de_assertion_time_8bit           = RTE_UART5_RS485_DE_ASSERTION_TIME_8BIT,
	.rs485_de_deassertion_time_8bit         = RTE_UART5_RS485_DE_DEASSERTION_TIME_8BIT,
	.rs485_de_to_re_turn_around_time_16bit  = RTE_UART5_RS485_DE_TO_RE_TURN_AROUND_TIME_16BIT,
	.rs485_re_to_de_turn_around_time_16bit  = RTE_UART5_RS485_RE_TO_DE_TURN_AROUND_TIME_16BIT,
};
#else
static uart_config_rs485_info_t uart5_rs485_config = {0};
#endif

#endif //END of RS485_SUPPORT

/* UART5 Driver Resources */
static uart_resources_t UART5_Resources =
{
	.reg_base       = (uint32_t)UART5_BASE,
	.clk            = 0,
	.cfg            = &uart5_config,
	.info           = &uart5_info,
	.irq_num        = (IRQn_Type) UART5_IRQ,
	.irq_priority   = (uint32_t)RTE_UART5_IRQ_PRI,
	.instance_num   = UART5_INSTANCE_NUM,

#if RS485_SUPPORT
	.rs485_cfg      = &uart5_rs485_config,
#endif
};


static int32_t UART5_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	return (ARM_USART_Initialize(cb_event,  &UART5_Resources));
}

static int32_t UART5_Uninitialize(void)
{
	return (ARM_USART_Uninitialize(&UART5_Resources));
}

static int32_t UART5_PowerControl(ARM_POWER_STATE state)
{
	return (ARM_USART_PowerControl(state, &UART5_Resources));
}

static int32_t UART5_Send(void const * const p_data, uint32_t num)
{
	return (ARM_USART_Send(p_data, num , &UART5_Resources));
}

static int32_t UART5_Receive(void * const p_data, uint32_t num)
{
	return (ARM_USART_Receive(p_data, num, &UART5_Resources));
}

static int32_t UART5_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
{
	return (ARM_USART_Transfer(p_data_out, p_data_in, num, &UART5_Resources));
}

static uint32_t UART5_GetTxCount(void)
{
	return (ARM_USART_GetTxCount(&UART5_Resources));
}

static uint32_t UART5_GetRxCount(void)
{
	return (ARM_USART_GetRxCount(&UART5_Resources));
}

static int32_t UART5_Control(uint32_t control, uint32_t arg)
{
	return (ARM_USART_Control(control, arg, &UART5_Resources));
}

static ARM_USART_STATUS UART5_GetStatus(void)
{
	return (ARM_USART_GetStatus(&UART5_Resources));
}

static ARM_USART_MODEM_STATUS UART5_GetModemStatus(void)
{
	return (ARM_USART_GetModemStatus(&UART5_Resources));
}

static int32_t UART5_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return (ARM_USART_SetModemControl(control, &UART5_Resources));
}

void UART5_IRQHandler (void)
{
	ARM_USART_IRQHandler (&UART5_Resources);
}

extern ARM_DRIVER_USART Driver_USART5;
ARM_DRIVER_USART Driver_USART5 =
{
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities_WO_RTS_CTS,
    UART5_Initialize,
    UART5_Uninitialize,
    UART5_PowerControl,
    UART5_Send,
    UART5_Receive,
    UART5_Transfer,
    UART5_GetTxCount,
    UART5_GetRxCount,
    UART5_Control,
    UART5_GetStatus,
    UART5_SetModemControl,
    UART5_GetModemStatus
};
#endif /* RTE_UART5 */


/* UART6 Driver Instance */
#if (RTE_UART6)

static uart_config_info_t uart6_config =
{
	.clk_source      = RTE_UART6_CLK_SOURCE,
	.rx_fifo_trg_lvl = RTE_UART6_RX_TRIG_LVL,
	.tx_fifo_trg_lvl = RTE_UART6_TX_TRIG_LVL,
};
static uart_info_t uart6_info = {0};

#if RS485_SUPPORT  /* Check if UART6 RS485 mode is enabled? */
#if RTE_UART6_RS485_ENABLE
static uart_config_rs485_info_t uart6_rs485_config =
{
	.rs485_control                          = RTE_UART6_RS485_ENABLE,
	.rs485_transfer_mode                    = RTE_UART6_RS485_TRANSFER_MODE,
	.rs485_de_assertion_time_8bit           = RTE_UART6_RS485_DE_ASSERTION_TIME_8BIT,
	.rs485_de_deassertion_time_8bit         = RTE_UART6_RS485_DE_DEASSERTION_TIME_8BIT,
	.rs485_de_to_re_turn_around_time_16bit  = RTE_UART6_RS485_DE_TO_RE_TURN_AROUND_TIME_16BIT,
	.rs485_re_to_de_turn_around_time_16bit  = RTE_UART6_RS485_RE_TO_DE_TURN_AROUND_TIME_16BIT,
};
#else
static uart_config_rs485_info_t uart6_rs485_config = {0};
#endif

#endif //END of RS485_SUPPORT

/* UART6 Driver Resources */
static uart_resources_t UART6_Resources =
{
	.reg_base       = (uint32_t)UART6_BASE,
	.clk            = 0,
	.cfg            = &uart6_config,
	.info           = &uart6_info,
	.irq_num        = (IRQn_Type) UART6_IRQ,
	.irq_priority   = (uint32_t)RTE_UART6_IRQ_PRI,
	.instance_num   = UART6_INSTANCE_NUM,

#if RS485_SUPPORT
	.rs485_cfg      = &uart6_rs485_config,
#endif
};


static int32_t UART6_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	return (ARM_USART_Initialize(cb_event,  &UART6_Resources));
}

static int32_t UART6_Uninitialize(void)
{
	return (ARM_USART_Uninitialize(&UART6_Resources));
}

static int32_t UART6_PowerControl(ARM_POWER_STATE state)
{
	return (ARM_USART_PowerControl(state, &UART6_Resources));
}

static int32_t UART6_Send(void const * const p_data, uint32_t num)
{
	return (ARM_USART_Send(p_data, num , &UART6_Resources));
}

static int32_t UART6_Receive(void * const p_data, uint32_t num)
{
	return (ARM_USART_Receive(p_data, num, &UART6_Resources));
}

static int32_t UART6_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
{
	return (ARM_USART_Transfer(p_data_out, p_data_in, num, &UART6_Resources));
}

static uint32_t UART6_GetTxCount(void)
{
	return (ARM_USART_GetTxCount(&UART6_Resources));
}

static uint32_t UART6_GetRxCount(void)
{
	return (ARM_USART_GetRxCount(&UART6_Resources));
}

static int32_t UART6_Control(uint32_t control, uint32_t arg)
{
	return (ARM_USART_Control(control, arg, &UART6_Resources));
}

static ARM_USART_STATUS UART6_GetStatus(void)
{
	return (ARM_USART_GetStatus(&UART6_Resources));
}

static ARM_USART_MODEM_STATUS UART6_GetModemStatus(void)
{
	return (ARM_USART_GetModemStatus(&UART6_Resources));
}

static int32_t UART6_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return (ARM_USART_SetModemControl(control, &UART6_Resources));
}

void UART6_IRQHandler (void)
{
	ARM_USART_IRQHandler (&UART6_Resources);
}

extern ARM_DRIVER_USART Driver_USART6;
ARM_DRIVER_USART Driver_USART6 =
{
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities_WO_RTS_CTS,
    UART6_Initialize,
    UART6_Uninitialize,
    UART6_PowerControl,
    UART6_Send,
    UART6_Receive,
    UART6_Transfer,
    UART6_GetTxCount,
    UART6_GetRxCount,
    UART6_Control,
    UART6_GetStatus,
    UART6_SetModemControl,
    UART6_GetModemStatus
};
#endif /* RTE_UART6 */


/* UART7 Driver Instance */
#if (RTE_UART7)

static uart_config_info_t uart7_config =
{
	.clk_source      = RTE_UART7_CLK_SOURCE,
	.rx_fifo_trg_lvl = RTE_UART7_RX_TRIG_LVL,
	.tx_fifo_trg_lvl = RTE_UART7_TX_TRIG_LVL,
};
static uart_info_t uart7_info = {0};

#if RS485_SUPPORT  /* Check if UART7 RS485 mode is enabled? */
#if RTE_UART7_RS485_ENABLE
static uart_config_rs485_info_t uart7_rs485_config =
{
	.rs485_control                          = RTE_UART7_RS485_ENABLE,
	.rs485_transfer_mode                    = RTE_UART7_RS485_TRANSFER_MODE,
	.rs485_de_assertion_time_8bit           = RTE_UART7_RS485_DE_ASSERTION_TIME_8BIT,
	.rs485_de_deassertion_time_8bit         = RTE_UART7_RS485_DE_DEASSERTION_TIME_8BIT,
	.rs485_de_to_re_turn_around_time_16bit  = RTE_UART7_RS485_DE_TO_RE_TURN_AROUND_TIME_16BIT,
	.rs485_re_to_de_turn_around_time_16bit  = RTE_UART7_RS485_RE_TO_DE_TURN_AROUND_TIME_16BIT,
};
#else
static uart_config_rs485_info_t uart7_rs485_config = {0};
#endif

#endif //END of RS485_SUPPORT

/* UART7 Driver Resources */
static uart_resources_t UART7_Resources =
{
	.reg_base       = (uint32_t)UART7_BASE,
	.clk            = 0,
	.cfg            = &uart7_config,
	.info           = &uart7_info,
	.irq_num        = (IRQn_Type) UART7_IRQ,
	.irq_priority   = (uint32_t)RTE_UART7_IRQ_PRI,
	.instance_num   = UART7_INSTANCE_NUM,

#if RS485_SUPPORT
	.rs485_cfg      = &uart7_rs485_config,
#endif
};


static int32_t UART7_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	return (ARM_USART_Initialize(cb_event,  &UART7_Resources));
}

static int32_t UART7_Uninitialize(void)
{
	return (ARM_USART_Uninitialize(&UART7_Resources));
}

static int32_t UART7_PowerControl(ARM_POWER_STATE state)
{
	return (ARM_USART_PowerControl(state, &UART7_Resources));
}

static int32_t UART7_Send(void const * const p_data, uint32_t num)
{
	return (ARM_USART_Send(p_data, num , &UART7_Resources));
}

static int32_t UART7_Receive(void * const p_data, uint32_t num)
{
	return (ARM_USART_Receive(p_data, num, &UART7_Resources));
}

static int32_t UART7_Transfer(void const * const p_data_out, void * const p_data_in, uint32_t num)
{
	return (ARM_USART_Transfer(p_data_out, p_data_in, num, &UART7_Resources));
}

static uint32_t UART7_GetTxCount(void)
{
	return (ARM_USART_GetTxCount(&UART7_Resources));
}

static uint32_t UART7_GetRxCount(void)
{
	return (ARM_USART_GetRxCount(&UART7_Resources));
}

static int32_t UART7_Control(uint32_t control, uint32_t arg)
{
	return (ARM_USART_Control(control, arg, &UART7_Resources));
}

static ARM_USART_STATUS UART7_GetStatus(void)
{
	return (ARM_USART_GetStatus(&UART7_Resources));
}

static ARM_USART_MODEM_STATUS UART7_GetModemStatus(void)
{
	return (ARM_USART_GetModemStatus(&UART7_Resources));
}

static int32_t UART7_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return (ARM_USART_SetModemControl(control, &UART7_Resources));
}

void UART7_IRQHandler (void)
{
	ARM_USART_IRQHandler (&UART7_Resources);
}

extern ARM_DRIVER_USART Driver_USART7;
ARM_DRIVER_USART Driver_USART7 =
{
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities_WO_RTS_CTS,
    UART7_Initialize,
    UART7_Uninitialize,
    UART7_PowerControl,
    UART7_Send,
    UART7_Receive,
    UART7_Transfer,
    UART7_GetTxCount,
    UART7_GetRxCount,
    UART7_Control,
    UART7_GetStatus,
    UART7_SetModemControl,
    UART7_GetModemStatus
};
#endif /* RTE_UART7 */


/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
