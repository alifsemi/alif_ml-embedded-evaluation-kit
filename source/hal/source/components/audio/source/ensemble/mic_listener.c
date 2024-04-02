/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*System Includes */
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "mic_listener.h"
#include <RTE_Device.h>

voice_callback_t rx_callback;

#ifdef USE_I2S_MICS
#include <Driver_SAI.h>

ARM_DRIVER_SAI       *i2s_drv;

#define Driver_SAI4 Driver_SAILP

extern ARM_DRIVER_SAI ARM_Driver_SAI_(BOARD_I2S_INSTANCE);

/**
  \fn          void sai_callback(uint32_t event)
  \brief       Callback routine from the i2s driver
  \param[in]   event Event for which the callback has been called
*/
void i2s_callback(uint32_t event)
{
    if (event & ARM_SAI_EVENT_RECEIVE_COMPLETE) {
        rx_callback(event);
    } else if (event & ARM_SAI_EVENT_RX_OVERFLOW) {
        printf("*** i2s_callback with event: ARM_SAI_EVENT_RX_OVERFLOW ***\n");
    } else if (event & ARM_SAI_EVENT_FRAME_ERROR) {
        printf("*** i2s_callback with event: ARM_SAI_EVENT_FRAME_ERROR ***\n");
    } else {
        printf("*** i2s_callback with event: %" PRIu32 "***\n", event);
    }
}

int32_t init_microphone(uint32_t sampling_rate, uint32_t data_bit_len)
{
    ARM_SAI_CAPABILITIES cap;
    int32_t status;

    /* Use the I2S2 as Receiver */
    i2s_drv = &ARM_Driver_SAI_(BOARD_I2S_INSTANCE);

    /* Verify if I2S protocol is supported */
    cap = i2s_drv->GetCapabilities();
    if (!cap.protocol_i2s) {
        printf("I2S is not supported\n");
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* Initializes I2S2 interface */
    status = i2s_drv->Initialize(i2s_callback);
    if (status != ARM_DRIVER_OK) {
        printf("I2S Initialize failed status = %" PRId32 "\n", status);
        return status;
    }

    /* Enable the power for I2S2 */
    status = i2s_drv->PowerControl(ARM_POWER_FULL);
    if (status != ARM_DRIVER_OK) {
        printf("I2S Power failed status = %" PRId32 "\n", status);
        return status;
    }

    /* configure I2S2 Receiver to Asynchronous Master */
    status = i2s_drv->Control(ARM_SAI_CONFIGURE_RX |
                              ARM_SAI_MODE_MASTER  |
                              ARM_SAI_ASYNCHRONOUS |
                              ARM_SAI_PROTOCOL_I2S |
                              ARM_SAI_DATA_SIZE(data_bit_len), data_bit_len*2, sampling_rate);
                                // once bug fixed in i2s driver we can use above style
                                //ARM_SAI_DATA_SIZE(data_bit_len), ARM_SAI_FRAME_LENGTH(data_bit_len*2), sampling_rate);

    if (status != ARM_DRIVER_OK) {
        printf("I2S Control status = %" PRId32 "\n", status);
        i2s_drv->PowerControl(ARM_POWER_OFF);
    }

    printf(" (OUT) I2S Control status = %" PRId32 "\n", status);
    return ARM_DRIVER_OK;
}

int32_t enable_microphone(voice_callback_t callback)
{
    /* enable Receiver */
    int32_t status = i2s_drv->Control(ARM_SAI_CONTROL_RX, 1, 0);
    if (status != ARM_DRIVER_OK) {
        printf("I2S enabled failed = %" PRId32 "\n", status);
        i2s_drv->PowerControl(ARM_POWER_OFF);
        rx_callback = NULL;
        return status;
    }

    rx_callback = callback;

    return ARM_DRIVER_OK;
}

int32_t disable_microphone()
{
    /* Stop the RX */
    int32_t status = i2s_drv->Control(ARM_SAI_CONTROL_RX, 0, 0);
    if (status != ARM_DRIVER_OK) {
        printf("I2S disable failed status = %" PRId32 "\n", status);
    }

    return status;
}

int32_t receive_voice_data(void *data, uint32_t data_len)
{
    return i2s_drv->Receive(data, data_len);
}
#endif

#ifdef USE_PDM_MICS
#include "Driver_PDM.h"

static void PDM_fifo_callback(uint32_t event)
{
    if(event & ARM_PDM_EVENT_ERROR )
    {
        printf("*** PDM_fifo_callback: ARM_PDM_EVENT_ERROR ***\n");
    }

    if(event & ARM_PDM_EVENT_CAPTURE_COMPLETE)
    {
        rx_callback(event);
    }

    if(event & ARM_PDM_EVENT_AUDIO_DETECTION)
    {
        printf("*** PDM_fifo_callback: ARM_PDM_EVENT_AUDIO_DETECTION ***\n");
    }
}

/* PDM driver instance */
extern ARM_DRIVER_PDM Driver_PDM;
static ARM_DRIVER_PDM* const PDMdrv = &Driver_PDM;

static const uint32_t ch_fir[18] = { 0x00000001, 0x00000003, 0x00000003, 0x000007F4, 0x00000004, 0x000007ED, 0x000007F5, 0x000007F4, 0x000007D3,
                                     0x000007FE, 0x000007BC, 0x000007E5, 0x000007D9, 0x00000793, 0x00000029, 0x0000072C, 0x00000072, 0x000002FD };

static int32_t pdm_mode(uint32_t sampling_rate)
{
    // selects the mode based on requested sampling rate
    switch(sampling_rate)
    {
        case 8000:
            return ARM_PDM_MODE_STANDARD_VOICE_512_CLK_FRQ;

        case 16000:
            return ARM_PDM_MODE_HIGH_QUALITY_1024_CLK_FRQ;

        case 32000:
            return ARM_PDM_MODE_WIDE_BANDWIDTH_AUDIO_1536_CLK_FRQ;

        case 48000:
            return ARM_PDM_MODE_FULL_BANDWIDTH_AUDIO_3071_CLK_FRQ;

        case 96000:
            return ARM_PDM_MODE_ULTRASOUND_4800_CLOCK_FRQ;

        default:
            return -1;
    }
}

static int32_t resolution(uint32_t data_bit_len)
{
    switch(data_bit_len)
    {
        case 16:
            return ARM_PDM_16BIT_RESOLUTION;
        case 32:
            // there's an API to request 32bit data but currently the driver fails
            // to init with this value.
            return ARM_PDM_32BIT_RESOLUTION;

        default:
            return -1;
    }
}

int32_t init_microphone(uint32_t sampling_rate, uint32_t data_bit_len)
{
    int32_t ret;
    /* Initialize PDM driver */
    ret = PDMdrv->Initialize(PDM_fifo_callback);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: PDM init failed\n");
        return -1;
    }

    /* Enable the power for PDM */
    ret = PDMdrv->PowerControl(ARM_POWER_FULL);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: PDM Power up failed\n");
        return -1;
    }

    /* To select the PDM channel 4 and channel 5 */
    ret = PDMdrv->Control(ARM_PDM_SELECT_CHANNEL, (ARM_PDM_MASK_CHANNEL_4 | ARM_PDM_MASK_CHANNEL_5));
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: PDM channel select control failed\n");
        return -1;
    }

    /* Select PDM mode based on the requested sampling rate*/
    int32_t mode = pdm_mode(sampling_rate);
    if(mode < 0)
    {
        printf("\r\n Error: Invalid sampling rate (%" PRIu32 ") for PDM.", sampling_rate);
        return -1;
    }

    ret = PDMdrv->Control(ARM_PDM_MODE, (uint32_t)mode);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: PDM mode control failed\n");
        return -1;
    }

    /* Select resolution */
    int32_t pdm_resolution = resolution(data_bit_len);
    if(pdm_resolution < 0)
    {
        printf("\r\n Error: Invalid data bit len (%" PRIu32 ") for PDM.", data_bit_len);
        return -1;
    }

    ret = PDMdrv->Control(ARM_PDM_SELECT_RESOLUTION, (uint32_t)pdm_resolution);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: PDM resolution control failed\n");
        return -1;
    }

    /* Select the DC blocking IIR filter */
    ret = PDMdrv->Control(ARM_PDM_BYPASS_IIR_FILTER, 1);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: PDM DC blocking IIR control failed\n");
        return -1;
    }

    /* Channel 4 configuration values */
    PDM_CH_CONFIG pdm_coef_reg;
    pdm_coef_reg.ch_num              = 4;    /* Channel 4 */
    memcpy(pdm_coef_reg.ch_fir_coef, ch_fir, sizeof(pdm_coef_reg.ch_fir_coef)); /* Channel 4 fir coefficient */
    pdm_coef_reg.ch_iir_coef         = 0x00000004;   /* Channel IIR Filter Coefficient */
    pdm_coef_reg.ch_phase            = 0x0000001F;   /* Channel Phase Control */
    pdm_coef_reg.ch_gain             = 0x00000800;   /* Channel gain control */
    pdm_coef_reg.ch_peak_detect_th   = 0x00060002;   /* Channel Peak Detector Threshold */
    pdm_coef_reg.ch_peak_detect_itv  = 0x0004002D;   /* Channel Peak Detector Interval */

    ret = PDMdrv->Config(&pdm_coef_reg);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: PDM Channel_Config failed\n");
        return -1;
    }

    /* Channel 5 configuration values */
    pdm_coef_reg.ch_num              = 5;    /* Channel 5 */
    memcpy(pdm_coef_reg.ch_fir_coef, ch_fir, sizeof(pdm_coef_reg.ch_fir_coef)); /* Channel 5 fir coefficient */
    pdm_coef_reg.ch_iir_coef         = 0x00000004;   /* Channel IIR Filter Coefficient */
    pdm_coef_reg.ch_phase            = 0x0000001F;   /* Channel Phase Control */
    pdm_coef_reg.ch_gain             = 0x00000800;   /* Channel gain control */
    pdm_coef_reg.ch_peak_detect_th   = 0x00060002;   /* Channel Peak Detector Threshold */
    pdm_coef_reg.ch_peak_detect_itv  = 0x0004002D;   /* Channel Peak Detector Interval */

    ret = PDMdrv->Config(&pdm_coef_reg);
    if(ret != ARM_DRIVER_OK){
        printf("\r\n Error: PDM Channel_Config failed\n");
        return -1;
    }

    return 0;
}

int32_t enable_microphone(voice_callback_t callback)
{
    rx_callback = callback;
    return 0;
}

int32_t disable_microphone()
{
    return 0;
}

int32_t receive_voice_data(void* data, uint32_t data_len)
{
    return PDMdrv->Receive(data, data_len);
}

#endif
