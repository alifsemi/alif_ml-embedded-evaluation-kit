/* Copyright (c) 2024 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#include "wm8904_driver.h"
#include "system_utils.h"
#include <stdio.h>
#include "Driver_I2C.h"
#include <stdatomic.h>
#include <inttypes.h>

extern ARM_DRIVER_I2C Driver_I2C0;
static ARM_DRIVER_I2C *I2C_MstDrv = &Driver_I2C0;

/* Golbal variables */
static uint8_t tx_data[3];
static uint8_t rx_data[4];

static atomic_int cb_status = 0;

static int32_t I2C_Master_WriteData(int addr, uint8_t* data, uint32_t len)
{
    int32_t ret = I2C_MstDrv->MasterTransmit(addr, data, len, 0);

     if (ret != ARM_DRIVER_OK)
     {
         printf("\r\n Error: I2C Master Transmit failed\n");
         return ret;
     }

     /* wait for transmission to finish. */
     while(cb_status == 0);
     int status = cb_status;
     cb_status = 0;
     return status == ARM_I2C_EVENT_TRANSFER_DONE ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}

static int32_t WM8904_WriteData(uint16_t Address, uint16_t data)
{
	tx_data[0]= Address;
	tx_data[2]= (uint8_t)(data & 0xFF);
	tx_data[1]= (uint8_t)(data >> 8);
	return I2C_Master_WriteData(WM8904_SLAVE,tx_data,3);
}

static int32_t WM8904_Write(uint16_t Address)
{
	tx_data[0]= Address;
	return I2C_Master_WriteData(WM8904_SLAVE,tx_data,1);
}

static void i2c_callback(uint32_t event)
{
    if (event != ARM_I2C_EVENT_TRANSFER_DONE) {
        printf("i2c_callback: %" PRIu32 "\n", event);
    }
    cb_status = event;
}

static int32_t I2C_Init()
{
    int32_t ret = 0;
    ARM_DRIVER_VERSION version;
    version = I2C_MstDrv->GetVersion();
    printf("\r\n I2C version api:0x%X driver:0x%X...\r\n",version.api, version.drv);

    /* Initialize Master I2C driver */
    ret = I2C_MstDrv->Initialize(i2c_callback);
    if (ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error: I2C master init failed\n");
        return ret;
    }

    /* I2C Master Power control  */
    ret = I2C_MstDrv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error: I2C Master Power up failed\n");
        return ret;
    }

    /* I2C Master Control */
    ret = I2C_MstDrv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
    if (ret != ARM_DRIVER_OK)
    {
        printf("\r\n Error: I2C master control failed\n");
        return ret;
    }

    return ret;
}

static int32_t I2C_Master_ReadData(int addr, uint8_t* data, uint32_t len)
{
    int32_t ret = I2C_MstDrv->MasterReceive(addr, data, len, false);
    if (ret != ARM_DRIVER_OK)
    {
         printf("\r\n Error: I2C Master Receive failed with %" PRId32 "\n", ret);
         return ret;
    }

    /* wait for master/slave callback. */
    while(cb_status == 0);
    int status = cb_status;
    cb_status = 0;
    return status == ARM_I2C_EVENT_TRANSFER_DONE ? ARM_DRIVER_OK : ARM_DRIVER_ERROR;
}

/**
 \fn: void WM8904_QSUS(void)
 \brief: Detects the WM8904 and executes the WM8904 Quick Startup routine.
 */
static int32_t WM8904_QSUS(void)
{
	uint16_t DeviceID = 0x00;
	/* Read Device ID 8904h */
	WM8904_WriteData(WM8904_SW_RESET_AND_ID,0x0101);
	I2C_Master_ReadData (WM8904_SLAVE,rx_data,2);
	DeviceID = rx_data[0];
	DeviceID <<= 8;
	DeviceID |= rx_data[1];
	if(DeviceID == WM8904_DEV_ID)
	{
		printf("WM8904 device Found, initiating Quick startup\n");

		/*	The Write Sequencer needs SYSCLK to be enabled to function */
        WM8904_WriteData(WM8904_CLOCK_RATES_2, (CLK_RTE2_CLK_SYS_ENA | CLK_RTE2_CLK_DSP_ENA));

        /* Start the Quick Startup Sequence as described in the WM8904 manual */
		WM8904_WriteData(WM8904_WRITE_SEQUENCER_0, 0x0100);
		/*  Start the Write Sequencer at Index address 0 (00h) */
		WM8904_WriteData(WM8904_WRITE_SEQUENCER_3, 0x0100);

        // wait until quick startup write sequencer finishes (approx 300ms)
        do {
            sys_busy_loop_us(50000);
            WM8904_Write(0x70);
            I2C_Master_ReadData(WM8904_SLAVE, rx_data, 2);
        } while(rx_data[1] & 0x1);

        /* This un-mutes the DACs, final phase of the Quick Start-up. */
		WM8904_WriteData(WM8904_DAC_DIGITAL_1, 0x0000);

        printf("Quick startup finished.\n");
        return ARM_DRIVER_OK;
	}
	else
	{
		 printf("WM8904 device not found\n");
         return ARM_DRIVER_ERROR_UNSUPPORTED;
	}
}

int32_t WM8904_Codec_Init(void)
{
    int32_t ret = I2C_Init();
	if(ret != ARM_DRIVER_OK) {
        return ret;
    }
   	return WM8904_QSUS();
}

void WM8904_Set_Volume(uint8_t volume)
{
    if(volume > 100) {
        return;
    }
    uint16_t new_output_volume;
    if(volume > 60) {
        // one step of db (-0,375db) per step
        new_output_volume = 152 + (volume - 60);
    }
    else {
        // 2,5 steps of db (-0,95db) per step (on average..)
        new_output_volume = (uint16_t)(volume * 2.5333333f);
    }
    printf("volume: %" PRIu8 " => %" PRIu16 "\n", volume, new_output_volume);

    WM8904_WriteData(WM8904_DAC_DIGITAL_VOLUME_LEFT, new_output_volume);              // set left volume but don't update yet
    WM8904_WriteData(WM8904_DAC_DIGITAL_VOLUME_RIGHT, DAC_DG_VU | new_output_volume); // set right volume and update volumes for both channels
}
