/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/* System Includes */
#include "RTE_Components.h"
#include "RTE_Device.h"
#include "global_map.h"

/* Project Includes */
#include "Driver_CPI.h"
#include "Driver_Camera_Controller.h"
#include "Driver_GPIO.h"
#include "Driver_PINMUX_AND_PINPAD.h"

#include "Driver_Common.h"
#include "image_processing.h"

#include <stdatomic.h>

extern ARM_DRIVER_CAMERA_CONTROLLER Driver_CAMERA0;
static const ARM_DRIVER_CAMERA_CONTROLLER* camera = &Driver_CAMERA0;

static uint8_t* buf              = 0;
static atomic_int image_received = 0;

static void CameraEventHandler(uint32_t event)
{
    (void)event;

    // only capture stopped event is configured to cause interrupt
    image_received = 1;
}

static int32_t camera_hw_init()
{
    int32_t res = PINMUX_Config(PORT_NUMBER_3, PIN_NUMBER_8, PINMUX_ALTERNATE_FUNCTION_3);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    /* Configure GPIO Pin : P3_9 as I3C_SCL_B */
    res = PINMUX_Config(PORT_NUMBER_3, PIN_NUMBER_9, PINMUX_ALTERNATE_FUNCTION_4);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    /* i3c Pin-Pad */

    /* Pin-Pad P3_8 as I3C_SDA_B
     * Pad function: weak pull up(0x8) + read enable(0x01)
     *               + Output drive strength 4mA(0x20)
     */
    res = PINPAD_Config(PORT_NUMBER_3,
                        PIN_NUMBER_8,
                        (PAD_FUNCTION_READ_ENABLE | PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_PULL_UP |
                         PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_04_MILI_AMPS));
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    /* Pin-Pad P3_9 as I3C_SCL_B
     * Pad function: weak pull up(0x8) + read enable(0x01)
     *               + Output drive strength 4mA(0x20)
     */
    res = PINPAD_Config(PORT_NUMBER_3,
                        PIN_NUMBER_9,
                        (PAD_FUNCTION_READ_ENABLE | PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_PULL_UP |
                         PAD_FUNCTION_OUTPUT_DRIVE_STRENGTH_04_MILI_AMPS));
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    res = PINMUX_Config(PORT_NUMBER_2, PIN_NUMBER_7, PINMUX_ALTERNATE_FUNCTION_6);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    return res;
}

int32_t camera_init(uint8_t* buffer)
{
    // Pin configurations
    int32_t res = camera_hw_init();
    if (res != ARM_DRIVER_OK) {
        return res;
    }
    //////////////////////////////////////////////////////////////////////////////
    // Camera initialization
    //////////////////////////////////////////////////////////////////////////////
    res = camera->Initialize(CAMERA_RESOLUTION_560x560, CameraEventHandler);

    if (res != ARM_DRIVER_OK) {
        return res;
    }

    res = camera->PowerControl(ARM_POWER_FULL);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    res = camera->Control(CAMERA_SENSOR_CONFIGURE, CAMERA_RESOLUTION_560x560);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    res = camera->Control(CAMERA_EVENTS_CONFIGURE, ARM_CAMERA_CONTROLLER_EVENT_CAMERA_CAPTURE_STOPPED);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    buf = buffer;

    return res;
}

void camera_start(uint32_t mode)
{
    image_received = 0;
    if (mode == CAMERA_MODE_SNAPSHOT) {
        camera->CaptureFrame(buf);
    } else {
        camera->CaptureVideo(buf);
    }
}

int32_t camera_wait(uint32_t timeout_ms)
{
    while (!image_received) {
        __WFE();
    };
    return ARM_DRIVER_OK;
}
