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

/* Project Includes */
#include "camera.h"
#include "Driver_GPIO.h"

#if defined(RTE_Drivers_CPI)

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

#include "image_processing.h"
#include "hal_log.h"

#include <stdatomic.h>
#include <stdio.h>

#if RTE_ISP
#include "Driver_ISP.h"
#include "isp_buffer.h"
#include "isp_param.h"

extern ARM_DRIVER_ISP Driver_ISP;
static bool isp_buffers_configured = false;
#endif

static uint8_t* camera_capture_buffer = 0;
static atomic_int image_received = 0;
static bool init_done = false;

    


static void CameraEventHandler(uint32_t event)
{
    switch (event) {
    case ARM_CPI_EVENT_CAMERA_CAPTURE_STOPPED:
        image_received = 1;
        break;
#if RTE_ISP
    case ARM_ISP_EVENT_FRAME_VSYNC_DETECTED:
    case ARM_ISP_EVENT_FRAME_IN_DETECTED:
        break;
    case ARM_ISP_MI_EVENT_MP_FRAME_END_DETECTED:
    case ARM_ISP_MI_EVENT_FILL_MP_Y_DETECTED:
    case ARM_ISP_MI_EVENT_MP_Y_WRAP_DETECTED:
        image_received = 1;
        break;
    case ARM_ISP_EVENT_AWB_DONE:
        break;
    case ARM_ISP_EVENT_EXP_MEASURE_DONE:
        break;
#endif
    default:
        break;
    }
}

#if RTE_ISP
int32_t isp_configure(uint32_t width, uint32_t height)
{
    info("Configuring ISP for %ux%u capture\n", (unsigned)width, (unsigned)height);

    int32_t res = 0;
    if (isp_buffers_configured) {
        res = isp_buffer_deconfigure();
        if (res != ARM_DRIVER_OK) {
            printf("Failed to deconfigure ISP buffers: %ld\n", res);
            return res;
        }
        isp_buffers_configured = false;
    }

    res = Driver_ISP.PowerControl(ARM_POWER_OFF);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    res = Driver_ISP.Uninitialize();
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    isp_param_set_square_crop();

    // Configure ISP output resolution
    isp_param_set_output_dimensions(width, height);

    res = Driver_ISP.Initialize(CameraEventHandler);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    res = Driver_ISP.PowerControl(ARM_POWER_FULL);
    if (res != ARM_DRIVER_OK) {
        return res;
    }

    // Configure ISP buffers for camera output
    res = isp_buffer_configure(width, height);
    if (res != ARM_DRIVER_OK) {
        return res;
    }
    isp_buffers_configured = true;

    info("ISP configured. input=%ux%u crop=%ux%u output=%ux%u\n",
         port_attr.snsRect.width, port_attr.snsRect.height,
         port_attr.outFormRect.width, port_attr.outFormRect.height,
         chan_attr.chnFormat.width, chan_attr.chnFormat.height);
    return 0;
}
#endif

int32_t camera_configure(uint32_t width, uint32_t height)
{
#if RTE_ISP
    // Dynamic resolution configuration is only supported with ISP
    return isp_configure(width, height);
#else
    // No additional configuration needed for CPI; resolution is set at initialization through RTE config
    // SW image processing will handle cropping/resizing as needed
    (void)width;
    (void)height;
    return 0;
#endif
}

int32_t camera_init(uint8_t* buffer)
{
    if (init_done) {
        printf("camera_init, already initialized!\n");
        return 0;
    }
#ifdef MANUAL_CAMERA_POWER
    GPIO_Driver_PWR->SetValue(BOARD_CAMERA_POWER_PIN_NO, GPIO_PIN_OUTPUT_STATE_HIGH);
#endif

    //////////////////////////////////////////////////////////////////////////////
    // Camera initialization
    //////////////////////////////////////////////////////////////////////////////
    int32_t res = 0;
#ifdef RESOLUTION_PARAMETER
    res = camera->Initialize(RESOLUTION_PARAMETER, CameraEventHandler);
#else
    res = camera->Initialize(CameraEventHandler);
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

    camera_capture_buffer = buffer;
    init_done = true;

    return res;
}

void camera_uninit()
{
    if (init_done) {
        camera->Stop();
    }

#if RTE_ISP
    if (isp_buffers_configured) {
        isp_buffer_deconfigure();
        isp_buffers_configured = false;
    }
#endif

    if (init_done) {
        camera->PowerControl(ARM_POWER_OFF);
        camera->Uninitialize();
        init_done = false;
    }
}

void camera_start(uint32_t mode)
{
    image_received = 0;

    /* A non-null framebuffer address is required by the CPI driver.
     * Use dummy address when ISP is enabled.
     * CPI writing to memory is disabled in RTE config (RTE_CPI_AXI_PORT) */
#if RTE_ISP
#if RTE_CPI_AXI_PORT
    // HW actually supports this but we want to get only the ISP output and save memory
    #error "CPI framebuffer writes should be disabled in RTE config when using ISP"
#endif
    camera_capture_buffer = (uint8_t*)0xFA57CAFE; // Dummy
#endif
    int32_t res;
    if (mode == CAMERA_MODE_SNAPSHOT) {
        res = camera->CaptureFrame(camera_capture_buffer);
    } else {
        res = camera->CaptureVideo(camera_capture_buffer);
    }
    if (res != ARM_DRIVER_OK) {
        printf("Error: camera capture start failed: %ld\n", res);
    }
}

int32_t camera_gain(uint32_t gain)
{
    return camera->Control(CPI_CAMERA_SENSOR_GAIN, gain);
}

int32_t camera_process_frame_end()
{
#if RTE_ISP
    int32_t res = camera->Control(ISP_PROCESS_FRAME_END, 0);
    if (res != ARM_DRIVER_OK) {
        printf("Error: ISP Process Frame End failed: %ld\n", res);
        return res;
    }

#if RTE_ISP_AE_MODULE
    // Apply AE-computed exposure and gain to the sensor. 
    static uint32_t prev_int_line = 0;
    static uint32_t prev_gain_q16_16 = 0;
    struct isp_ae_cached_values ae = {0};
    res = Driver_ISP.Control(ISP_CONTROL_AE_GET_CACHED, (uint32_t)&ae);
    if (res == ARM_DRIVER_OK && ae.int_line != 0) {
        uint32_t gain_q16_16 = (ae.again * ae.dgain) / 16;
        if (ae.int_line != prev_int_line || gain_q16_16 != prev_gain_q16_16) {
            debug("AE set: intLine=%lu again=%lu dgain=%lu\n",
                    (unsigned long)ae.int_line,
                    (unsigned long)ae.again,
                    (unsigned long)ae.dgain);
            res = camera->Control(CPI_ISP_CAMERA_SENSOR_EXPOSURE, ae.int_line);
            if (res != ARM_DRIVER_OK) {
                printf("Error: Setting camera exposure failed: %ld\n", res);
                return res;
            }
            res = camera->Control(CPI_ISP_CAMERA_SENSOR_GAIN, gain_q16_16);
            if (res != ARM_DRIVER_OK) {
                printf("Error: Setting camera gain failed: %ld\n", res);
                return res;
            }
            prev_int_line = ae.int_line;
            prev_gain_q16_16 = gain_q16_16;
        }
    }
#endif /* RTE_ISP_AE_MODULE */

#endif

    return ARM_DRIVER_OK;
}

bool camera_image_ready()
{
    return image_received;
}

#endif // RTE_Drivers_CPI
