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

#include "Driver_Common.h"
#include "image_processing.h"

#include <stdatomic.h>

/* New-enough CMSIS packs are expected to handle power for us */
#if !defined RTE_CPI && defined BOARD_CAMERA_POWER_GPIO_PORT
#define MANUAL_CAMERA_POWER
extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(BOARD_CAMERA_POWER_GPIO_PORT);
static ARM_DRIVER_GPIO * const GPIO_Driver_PWR = &ARM_Driver_GPIO_(BOARD_CAMERA_POWER_GPIO_PORT);
#endif

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

int32_t camera_init(uint8_t* buffer)
{
#ifdef MANUAL_CAMERA_POWER
    GPIO_Driver_PWR->SetValue(BOARD_CAMERA_POWER_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
#endif

    //////////////////////////////////////////////////////////////////////////////
    // Camera initialization
    //////////////////////////////////////////////////////////////////////////////
    int32_t res = camera->Initialize(CAMERA_RESOLUTION_560x560, CameraEventHandler);

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

int32_t camera_gain(uint32_t gain)
{
    return camera->Control(CAMERA_SENSOR_GAIN, gain);
}

int32_t camera_wait(uint32_t timeout_ms)
{
    while (!image_received) {
        __WFE();
    };
    return ARM_DRIVER_OK;
}
