/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     GT911_touch_driver.c
 * @author   Prasanna Ravi and Ahmad Rashed
 * @email    prasanna.ravi@alifsemi.com and ahmad.rashed@alifsemi.com
 * @version  V1.0.0
 * @date     5-January-23
 * @brief    GT911 touch screen driver.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "RTE_Components.h"
#include "Driver_GPIO.h"
#include "Driver_I3C.h"
#include "Driver_PINMUX_AND_PINPAD.h"
#include "system_utils.h"

#include "Driver_Touch_Screen.h"

#if (!defined(RTE_Drivers_GT911))
#error "GT911 Touch not configured in RTE_Components.h!"
#endif

#if !( RTE_TOUCH_SCREEN)
#error "Touch Driver not enabled in the RTE_Device.h"
#endif

#if !(RTE_GT911)
#error "GT911 Touch is not enabled in the RTE_Device.h"
#endif

#if !(RTE_ACTIVE_TOUCH_POINTS)
#error "GT911 at least enable one active touch point"
#endif

/*Touch reset pin control GPIO port*/
#if (RTE_GT911_TOUCH_RESET_GPIO_PORT == 1)
extern ARM_DRIVER_GPIO Driver_GPIO1;
static ARM_DRIVER_GPIO *GPIO_Driver_RST = &Driver_GPIO1;
#elif (RTE_GT911_TOUCH_RESET_GPIO_PORT == 2)
extern ARM_DRIVER_GPIO Driver_GPIO2;
static ARM_DRIVER_GPIO *GPIO_Driver_RST = &Driver_GPIO2;
#elif (RTE_GT911_TOUCH_RESET_GPIO_PORT == 3)
extern ARM_DRIVER_GPIO Driver_GPIO3;
static ARM_DRIVER_GPIO *GPIO_Driver_RST = &Driver_GPIO3;
#elif (RTE_GT911_TOUCH_RESET_GPIO_PORT == 4)
extern ARM_DRIVER_GPIO Driver_GPIO4;
static ARM_DRIVER_GPIO *GPIO_Driver_RST = &Driver_GPIO4;
#else
#error "GPIO Port for touch Reset not found"
#endif

/*Touch INT pin control GPIO port*/
#if (RTE_GT911_TOUCH_INT_GPIO_PORT == 1)
extern ARM_DRIVER_GPIO Driver_GPIO1;
static ARM_DRIVER_GPIO *GPIO_Driver_INT = &Driver_GPIO1;
#elif (RTE_GT911_TOUCH_INT_GPIO_PORT == 2)
extern ARM_DRIVER_GPIO Driver_GPIO2;
static ARM_DRIVER_GPIO *GPIO_Driver_INT = &Driver_GPIO2;
#elif (RTE_GT911_TOUCH_INT_GPIO_PORT == 3)
extern ARM_DRIVER_GPIO Driver_GPIO3;
static ARM_DRIVER_GPIO *GPIO_Driver_INT = &Driver_GPIO3;
#elif (RTE_GT911_TOUCH_INT_GPIO_PORT == 4)
extern ARM_DRIVER_GPIO Driver_GPIO4;
static ARM_DRIVER_GPIO *GPIO_Driver_INT = &Driver_GPIO4;
#else
#error "GPIO Port for display BL LED not found"
#endif

/*I3C Driver used for Touch screen communication*/
extern ARM_DRIVER_I3C Driver_I3C0;
static ARM_DRIVER_I3C *I3C_Driver = &Driver_I3C0;

/*GT911 Driver specific macros*/
#define GT911_SLAVE_ADDR                 0x5D
#define GT911_COMMAND_REG                0x8040
#define GT911_PRODUCT_ID                 0x8140
#define GT911_TOUCH_STATUS               0x814E
#define GT911_NUM_TOUCH_POINTS_MSK       0x7
#define GT911_FIRST_TOUCHPOINT_DATA      0x8150

#define GT911_REG_INDX_SIZE             2
#define TOUCH_I2C_TIMEOUT_US            100000

/*GT911 Driver status*/
#define GT911_FLAG_DRV_INIT_DONE    (1U << 0)
#define GT911_FLAG_POWER_ENABLED    (1U << 1)

/*GT911 Touch event variables*/
volatile uint8_t touch_event_gpio;
volatile uint8_t touch_event_i2c;

/*GT911 Touch driver Info variable*/
static struct GT911_DRV_INFO {
	uint32_t touch_drv_status;
}gt911_drv_info;

#define ARM_TOUCH_SCREEN_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)/*Driver version*/

/*Driver version*/
static const ARM_DRIVER_VERSION DriverVersion =
{
	ARM_TOUCH_SCREEN_API_VERSION,
	ARM_TOUCH_SCREEN_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_TOUCH_SCREEN_CAPABILITIES DriverCapabilities =
{
	0, /* Not supports reentrant_operation */
	1, /* Multiple touch points supported*/
	0  /* reserved (must be zero)*/
};

/**
  \fn          ARM_DRIVER_VERSION ARM_TOUCH_SCREEN_GetVersion (void)
  \brief       Get touch screen driver version.
  \return      \ref ARM_DRIVER_VERSION.
*/
static ARM_DRIVER_VERSION ARM_TOUCH_SCREEN_GetVersion (void)
{
	return DriverVersion;
}

/**
  \fn          ARM_TOUCH_SCREEN_CAPABILITIES ARM_TOUCH_SCREEN_GetCapabilities (void)
  \brief       Get touch screen driver capabilities.
  \return      \ref ARM_MIPI_DPHY_CAPABILITIES.
*/
static ARM_TOUCH_SCREEN_CAPABILITIES ARM_TOUCH_SCREEN_GetCapabilities (void)
{
	return DriverCapabilities;
}

/**
  \fn           int32_t TOUCH_Write(uint8_t dev_addr, uint16_t reg_addr,
                                    uint8_t *reg_data, uint8_t num_bytes)
  \brief        write register value to GT911 Touch screen registers.
  \param[in]    dev_addr : I2c slave address.
  \param[in]    reg_addr : Register address.
  \param[in]    reg_data : pointer to register data.
  \param[in]    num_bytes : Number of bytes to write.
  \return       \ref execution_status.
  */
static int32_t TOUCH_Write(uint8_t dev_addr, uint16_t reg_addr,
		uint8_t *reg_data, uint8_t num_bytes)
{
	uint32_t ms_wait = 0;
	uint8_t  tx_data[12] = {0};
	uint8_t  i;
	int32_t  ret;
	int32_t  lp_count = 0;

	tx_data[0] = (reg_addr >> 8) & 0xFF;
	tx_data[1] = (reg_addr) & 0xFF;

	if (num_bytes)
	{
		for(i = 0; i < num_bytes; i++)
			tx_data[i + GT911_REG_INDX_SIZE] = reg_data[i];

	}

	touch_event_i2c = 0;

	do
	{
		ret = I3C_Driver->MasterTransmit(dev_addr, tx_data,
				num_bytes + GT911_REG_INDX_SIZE);

		if(lp_count++ < TOUCH_I2C_TIMEOUT_US)
			PMU_delay_loop_us(1);
		else
			break;

	} while(ret == ARM_DRIVER_ERROR_BUSY);

	if(ret != ARM_DRIVER_OK)
		return ret;

	lp_count = 0;

	while (touch_event_i2c == 0)
	{
		if(lp_count++ < TOUCH_I2C_TIMEOUT_US)
			PMU_delay_loop_us(1);
		else
			return ARM_DRIVER_ERROR;
	}

	if (!(touch_event_i2c & ARM_I3C_EVENT_TRANSFER_DONE))
		return ARM_DRIVER_ERROR;

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t TOUCH_Read(uint8_t dev_addr, uint16_t reg_addr,
                                    uint8_t *reg_data, uint8_t num_bytes)
  \brief        read register value from GT911 Touch screen registers.
  \param[in]    dev_addr : I2c slave address.
  \param[in]    reg_addr : Register address.
  \param[in]    reg_data : pointer to register data.
  \param[in]    num_bytes : Number of bytes to read.
  \return       \ref execution_status.
  */
static int32_t TOUCH_Read(uint8_t dev_addr, uint16_t reg_addr,
		uint8_t *reg_data, uint8_t num_bytes)
{
	uint32_t ms_wait;
	uint8_t  rx_data[12] = {0};
	uint8_t  i;
	int32_t  ret;
	int32_t  lp_count = 0;

	ret = TOUCH_Write(dev_addr, reg_addr, NULL, 0);
	if (ret != ARM_DRIVER_OK)
		return ret;

	touch_event_i2c = 0;

	ret = I3C_Driver->MasterReceive(dev_addr, rx_data, num_bytes);

	if(ret != ARM_DRIVER_OK)
		return ret;

	while (touch_event_i2c == 0)
	{
		if(lp_count++ < TOUCH_I2C_TIMEOUT_US)
			PMU_delay_loop_us(1);
		else
			return ARM_DRIVER_ERROR;
	}

	if (!(touch_event_i2c & ARM_I3C_EVENT_TRANSFER_DONE))
		return ARM_DRIVER_ERROR;

	memcpy(reg_data, rx_data, num_bytes);

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t TOUCH_IntEnable(bool enable)
  \brief        GT911 Touch screen INT line GPIO control.
  \param[in]    enable : true or false.
  \return       \ref execution_status.
  */
static int32_t TOUCH_IntEnable(bool enable)
{
	uint32_t arg = ARM_GPIO_IRQ_POLARITY_LOW | ARM_GPIO_IRQ_EDGE_SENSITIVE_SINGLE | ARM_GPIO_IRQ_SENSITIVE_EDGE;
	int32_t ret;
	if(enable)
		ret = GPIO_Driver_INT->Control(PIN_NUMBER_20, ARM_GPIO_ENABLE_INTERRUPT, &arg);
	else
		ret = GPIO_Driver_INT->Control(PIN_NUMBER_20, ARM_GPIO_DISABLE_INTERRUPT, NULL);

	return ret;
}

/**
  \fn           void TOUCH_I2C_CB(uint32_t event)
  \brief        GT911 Touch screen I2C callback event.
  \param[in]    event: \ref I3C Event.
  \return       \ref execution_status.
  */
static void TOUCH_I2C_CB(uint32_t event)
{
	touch_event_i2c |= event;
}

/**
  \fn           void TOUCH_GPIO_CB(uint32_t event)
  \brief        GT911 Touch screen GPIO callback event.
  \param[in]    event: \ref GPIO Interrupt events.
  \return       \ref execution_status.
  */
static void TOUCH_GPIO_CB(uint32_t event)
{
	(void)TOUCH_IntEnable(false);
	touch_event_gpio = 1;
}

/**
  \fn           int32_t TOUCH_Reset(void)
  \brief        Hard Reset GT911 Touch Screen.
  \param[in]    none.
  \return       \ref execution_status.
  */
static int32_t TOUCH_Reset(void)
{
	int32_t ret;

	/* TOUCH INT pin */
	ret = GPIO_Driver_INT->Control(PIN_NUMBER_20, ARM_GPIO_DISABLE_INTERRUPT, NULL);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = GPIO_Driver_INT->SetValue(PIN_NUMBER_20, GPIO_PIN_OUTPUT_STATE_LOW);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = GPIO_Driver_INT->SetDirection(PIN_NUMBER_20, GPIO_PIN_DIRECTION_OUTPUT);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* TOUCH RST pin */
	ret = GPIO_Driver_RST->Control(PIN_NUMBER_2, ARM_GPIO_DISABLE_INTERRUPT, NULL);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = GPIO_Driver_RST->SetValue(PIN_NUMBER_2, GPIO_PIN_OUTPUT_STATE_LOW);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = GPIO_Driver_RST->SetDirection(PIN_NUMBER_2, GPIO_PIN_DIRECTION_OUTPUT);
	if(ret != ARM_DRIVER_OK)
		return ret;

	PMU_delay_loop_us(1000);

	ret = GPIO_Driver_RST->SetValue(PIN_NUMBER_2, GPIO_PIN_OUTPUT_STATE_HIGH);
	if(ret != ARM_DRIVER_OK)
		return ret;

	PMU_delay_loop_us(10000);

	ret = GPIO_Driver_INT->SetValue(PIN_NUMBER_20, GPIO_PIN_OUTPUT_STATE_LOW);
	if(ret != ARM_DRIVER_OK)
		return ret;

	PMU_delay_loop_us(50000);

	ret = GPIO_Driver_INT->SetDirection(PIN_NUMBER_20, GPIO_PIN_DIRECTION_INPUT);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = GPIO_Driver_INT->Initialize(PIN_NUMBER_20, &TOUCH_GPIO_CB);
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ARM_DRIVER_OK ;

}

/**
  \fn           int32_t TOUCH_Init(void)
  \brief        Initialize GT911 Touch Screen.
  this function will
  - initialize i2c using i3c instance.
  - reset GT911 Touch Screen.
  \param[in]    none.
  \return       \ref execution_status.
  */
int32_t TOUCH_Init(void)
{
	int32_t ret;

	ret = I3C_Driver->Initialize(TOUCH_I2C_CB);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = I3C_Driver->PowerControl(ARM_POWER_FULL);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = I3C_Driver->Control(
			I3C_MASTER_SET_BUS_MODE,
			I3C_BUS_MODE_MIXED_FAST_I2C_FM_SPEED_400_KBPS);

	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = I3C_Driver->AttachI2Cdev(GT911_SLAVE_ADDR);
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t TOUCH_Setup(void)
  \brief        Setup GT911 Touch Screen.
  this function will
  - read GT911 product id, proceed only it is correct.
  \param[in]    none.
  \return       \ref execution_status.
  */
int32_t TOUCH_Setup(void)
{
	uint8_t data = 0;
	uint8_t product_id_1st_byte = 0;
	int32_t ret;

	ret = TOUCH_Reset();
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = TOUCH_IntEnable(true);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = TOUCH_Write(GT911_SLAVE_ADDR, GT911_COMMAND_REG, &data, 1);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = TOUCH_Read(GT911_SLAVE_ADDR, GT911_PRODUCT_ID, &product_id_1st_byte, 1);
	if(ret != ARM_DRIVER_OK)
		return ret;

	/* "911" product id is in ascii character*/
	if (product_id_1st_byte != '9')
		return ARM_DRIVER_ERROR;

	return ARM_DRIVER_OK;
}

/**
  \fn           int32_t TOUCH_Deinit(void)
  \brief        Un-initialize GT911 Touch Screen.
  \param[in]    none.
  \return       \ref execution_status.
  */
int32_t TOUCH_Deinit(void)
{
	int32_t ret;

	ret = I3C_Driver->Detachdev(GT911_SLAVE_ADDR);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = I3C_Driver->PowerControl(ARM_POWER_OFF);
	if(ret != ARM_DRIVER_OK)
		return ret;

	ret = I3C_Driver->Uninitialize();
	if(ret != ARM_DRIVER_OK)
		return ret;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t  ARM_TOUCH_SCREEN_Initialize (void)
  \brief       Initialize touch screen Interface.
  \param[in]   none.
  \return      \ref execution_status.
 */
static int32_t  ARM_TOUCH_SCREEN_Initialize (void)
{
	int32_t ret;

	if(gt911_drv_info.touch_drv_status & GT911_FLAG_DRV_INIT_DONE)
		return ARM_DRIVER_OK;

	ret = TOUCH_Init();
	if(ret != ARM_DRIVER_OK)
		return ret;

	gt911_drv_info.touch_drv_status = GT911_FLAG_DRV_INIT_DONE;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_TOUCH_SCREEN_Uninitialize (void)
  \brief       uninitialize touch screen Interface.
  \return      \ref execution_status.
 */
static int32_t ARM_TOUCH_SCREEN_Uninitialize (void)
{
	int32_t ret;

	ret = TOUCH_Deinit();
	if(ret != ARM_DRIVER_OK)
		return ret;

	gt911_drv_info.touch_drv_status = 0;

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_TOUCH_SCREEN_PowerControl (ARM_POWER_STATE state)
  \brief       Control touch screen Interface Power.
  \param[in]   state  Power state.
  \return      \ref execution_status.
 */
static int32_t ARM_TOUCH_SCREEN_PowerControl (ARM_POWER_STATE state)
{
	int32_t ret;

	switch (state)
	{
		case ARM_POWER_OFF:

			/*Disabling the IRQs*/
			ret = TOUCH_IntEnable(false);
			if(ret != ARM_DRIVER_OK)
				return ret;

			gt911_drv_info.touch_drv_status &= ~GT911_FLAG_POWER_ENABLED;

			break;

		case ARM_POWER_FULL:

			if (!(gt911_drv_info.touch_drv_status & GT911_FLAG_DRV_INIT_DONE))
					return ARM_DRIVER_ERROR;

			if (gt911_drv_info.touch_drv_status & GT911_FLAG_POWER_ENABLED)
					return ARM_DRIVER_OK;

			ret = TOUCH_Setup();
			if(ret != ARM_DRIVER_OK)
				return ret;

			gt911_drv_info.touch_drv_status |= GT911_FLAG_POWER_ENABLED;

			break;

		case ARM_POWER_LOW:
		default:
			return ARM_DRIVER_ERROR_UNSUPPORTED;
	}

	return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ARM_TOUCH_SCREEN_GetState (ARM_TOUCH_STATE *state)
  \brief       Get touch screen touch state.
  \param[in]   state pointer to ARM_TOUCH_STATE.
  \return      \ref execution_status.
  */
static int32_t ARM_TOUCH_SCREEN_GetState (ARM_TOUCH_STATE *state)
{
	uint8_t data;
	uint16_t nextXY = GT911_FIRST_TOUCHPOINT_DATA;
	uint8_t rx_buf[4] = {0};
	int32_t ret;

	if (!(gt911_drv_info.touch_drv_status & GT911_FLAG_POWER_ENABLED))
		return ARM_DRIVER_ERROR;

	state->numtouches = 0;

	if(touch_event_gpio)
	{

		touch_event_gpio = 0;

		ret = TOUCH_Read(GT911_SLAVE_ADDR, GT911_TOUCH_STATUS, &data, 1);
		if(ret != ARM_DRIVER_OK)
			return ret;

		/* Get buffer status from GT911_TOUCH_STATUS 7th bit*/
		bool msb = data & 0x80;

		/* Get number of touch points from GT911_TOUCH_STATUS 0-3 bit*/
		state->numtouches = ((data & GT911_NUM_TOUCH_POINTS_MSK) > RTE_ACTIVE_TOUCH_POINTS ?
				RTE_ACTIVE_TOUCH_POINTS : (data & GT911_NUM_TOUCH_POINTS_MSK));

		if (msb)
		{
			if(state->numtouches)
			{

				for(uint8_t i = 0; i < state->numtouches; i++)
				{
					ret = TOUCH_Read(GT911_SLAVE_ADDR, nextXY, rx_buf, 4);
					if(ret != ARM_DRIVER_OK)
						return ret;

					state->coordinates[i].x = rx_buf[1];
					state->coordinates[i].y = rx_buf[3];
					state->coordinates[i].x <<= 8;
					state->coordinates[i].y <<= 8;
					state->coordinates[i].x |= rx_buf[0];
					state->coordinates[i].y |= rx_buf[2];

					nextXY += 8U;
				}

			}
			/*Clear status register*/
			data = 0;
			ret = TOUCH_Write(GT911_SLAVE_ADDR, GT911_TOUCH_STATUS, &data, 1);
			if(ret != ARM_DRIVER_OK)
				return ret;
		}

		/* re-enable gt911 interrupt */
		ret = TOUCH_IntEnable(true);
		if(ret != ARM_DRIVER_OK)
			return ret;
	}

	return ARM_DRIVER_OK;
}

extern ARM_DRIVER_TOUCH_SCREEN GT911;

ARM_DRIVER_TOUCH_SCREEN GT911 =
{
	ARM_TOUCH_SCREEN_GetVersion,
	ARM_TOUCH_SCREEN_GetCapabilities,
	ARM_TOUCH_SCREEN_Initialize,
	ARM_TOUCH_SCREEN_Uninitialize,
	ARM_TOUCH_SCREEN_PowerControl,
	ARM_TOUCH_SCREEN_GetState
};
