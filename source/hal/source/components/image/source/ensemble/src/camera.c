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
#include "camera.h"
#include "Driver_GPIO.h"

#ifndef RTE_CPI
#include "Driver_Camera_Controller.h"
extern ARM_DRIVER_CAMERA_CONTROLLER Driver_CAMERA0;
static const ARM_DRIVER_CAMERA_CONTROLLER * const camera = &Driver_CAMERA0;

#define CPI_CAMERA_SENSOR_CONFIGURE             CAMERA_SENSOR_CONFIGURE
#define CPI_EVENTS_CONFIGURE                    CAMERA_EVENTS_CONFIGURE
#define CPI_CAMERA_SENSOR_GAIN                  CAMERA_SENSOR_GAIN
#define ARM_CPI_EVENT_CAMERA_CAPTURE_STOPPED    ARM_CAMERA_CONTROLLER_EVENT_CAMERA_CAPTURE_STOPPED
#define RESOLUTION_PARAMETER                    CAMERA_RESOLUTION_560x560

#ifdef BOARD_CAMERA_POWER_GPIO_PORT
#define MANUAL_CAMERA_POWER
extern ARM_DRIVER_GPIO ARM_Driver_GPIO_(BOARD_CAMERA_POWER_GPIO_PORT);
static ARM_DRIVER_GPIO * const GPIO_Driver_PWR = &ARM_Driver_GPIO_(BOARD_CAMERA_POWER_GPIO_PORT);
#endif
#else
#include "Driver_CPI.h"
extern ARM_DRIVER_CPI Driver_CPI;
static const ARM_DRIVER_CPI * const camera = &Driver_CPI;
#endif

#include "Driver_Common.h"
#include "image_processing.h"

#include <stdatomic.h>


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
#ifdef RESOLUTION_PARAMETER
    int32_t res = camera->Initialize(RESOLUTION_PARAMETER, CameraEventHandler);
#else
    int32_t res = camera->Initialize(CameraEventHandler);
#endif

    if (res != ARM_DRIVER_OK) {
        return res;
    }

    res = camera->PowerControl(ARM_POWER_FULL);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

#ifdef RESOLUTION_PARAMETER
    res = camera->Control(CPI_CAMERA_SENSOR_CONFIGURE, RESOLUTION_PARAMETER);
#else
    res = camera->Control(CPI_CAMERA_SENSOR_CONFIGURE, 0);
#endif
    if (res != ARM_DRIVER_OK) {
        return res;
    }

#ifdef CPI_CONFIGURE
    res = camera->Control(CPI_CONFIGURE, 0);
    if (res != ARM_DRIVER_OK) {
        return res;
    }
#endif

    res = camera->Control(CPI_EVENTS_CONFIGURE, ARM_CPI_EVENT_CAMERA_CAPTURE_STOPPED);
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
    return camera->Control(CPI_CAMERA_SENSOR_GAIN, gain);
}

int32_t camera_wait(uint32_t timeout_ms)
{
    while (!image_received) {
        __WFE();
    };
    return ARM_DRIVER_OK;
}
