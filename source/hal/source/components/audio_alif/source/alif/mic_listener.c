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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "board_defs.h"
#include "mic_listener.h"
#include <RTE_Device.h>
#include "RTE_Components.h"
#include CMSIS_device_header

/* I2S microphone driver ---------------------------------------------------- */
#ifdef USE_I2S_MICS
#include <Driver_SAI.h>

#define Driver_SAI4 Driver_SAILP

extern ARM_DRIVER_SAI ARM_Driver_SAI_(BOARD_MIC_INPUT_I2S_INSTANCE);

static ARM_DRIVER_SAI *i2s_drv;
static voice_callback_t i2s_rx_callback;

/**
  \fn          void sai_callback(uint32_t event)
  \brief       Callback routine from the i2s driver
  \param[in]   event Event for which the callback has been called
*/
static void i2s_callback(uint32_t event)
{
    if (event & ARM_SAI_EVENT_RECEIVE_COMPLETE) {
        if (i2s_rx_callback) {
            i2s_rx_callback(event);
        }
    } else if (event & ARM_SAI_EVENT_RX_OVERFLOW) {
        printf("*** i2s_callback with event: ARM_SAI_EVENT_RX_OVERFLOW ***\n");
    } else if (event & ARM_SAI_EVENT_FRAME_ERROR) {
        printf("*** i2s_callback with event: ARM_SAI_EVENT_FRAME_ERROR ***\n");
    } else {
        printf("*** i2s_callback with event: %" PRIu32 "***\n", event);
    }
}

static int32_t init_microphone_i2s(uint32_t sampling_rate, uint32_t data_bit_len)
{
    ARM_SAI_CAPABILITIES cap;
    int32_t status;

    /* Use the I2S2 as Receiver */
    i2s_drv = &ARM_Driver_SAI_(BOARD_MIC_INPUT_I2S_INSTANCE);

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

    return status;
}

static int32_t enable_microphone_i2s(voice_callback_t callback)
{
    /* enable Receiver */
    int32_t status = i2s_drv->Control(ARM_SAI_CONTROL_RX, 1, 0);
    if (status != ARM_DRIVER_OK) {
        printf("I2S enabled failed = %" PRId32 "\n", status);
        i2s_drv->PowerControl(ARM_POWER_OFF);
        i2s_rx_callback = NULL;
        return status;
    }

    i2s_rx_callback = callback;

    return ARM_DRIVER_OK;
}

static int32_t disable_microphone_i2s(void)
{
    /* Stop the RX */
    int32_t status = i2s_drv->Control(ARM_SAI_CONTROL_RX, 0, 0);
    if (status != ARM_DRIVER_OK) {
        printf("I2S disable failed status = %" PRId32 "\n", status);
    }

    return status;
}

static int32_t receive_voice_data_i2s(void *data, uint32_t data_len)
{
    return i2s_drv->Receive(data, data_len);
}
#endif /* USE_I2S_MICS */

/* PDM / LPPDM microphone drivers -------------------------------------------
 *
 * Two PDM-type microphone streams can be compiled in:
 *   USE_PDM_MICS   - the "PDM" stream. Driven by Driver_PDM, except on boards
 *                    where the microphones are wired to the low power PDM
 *                    (BOARD_LPPDM_ENABLED == 1) and no separate LPPDM stream
 *                    is requested; then it is driven by Driver_LPPDM.
 *   USE_LPPDM_MICS - the "LPPDM" stream. Always driven by Driver_LPPDM. With
 *                    USE_PDM_MICS this gives four PDM microphones (two per
 *                    PDM block) that can be captured in parallel.
 * Both streams share the code below; each one has its own struct pdm_mic.
 */
#if defined(USE_PDM_MICS) || defined(USE_LPPDM_MICS)
#include "Driver_PDM.h"

/* Channel numbers used for channel configuration and status register of the
 * PDM block. The LPPDM stream has its own selection so that both PDM blocks
 * can be used at the same time. */
#ifndef PRIMARY_CHANNEL
#define PRIMARY_CHANNEL      4
#endif

#ifndef SECONDARY_CHANNEL
#define SECONDARY_CHANNEL    5
#endif

#ifndef LPPDM_PRIMARY_CHANNEL
#define LPPDM_PRIMARY_CHANNEL      0
#endif

#ifndef LPPDM_SECONDARY_CHANNEL
#define LPPDM_SECONDARY_CHANNEL    1
#endif

/* PDM Channel configurations.
 *
 * The two PDM channels on a shared PDM clock sample on opposite clock edges,
 * so each channel needs its own phase, gain and FIR coefficients. Phase,
 * peak-detect and FIR values match the Alif Baremetal demo_pdm.c reference.
 * The gains are raised ~10x above the demo values because our audio
 * preprocessing step applies an additional ~8x software gain and the
 * low-level demo values were set for direct raw-PDM playback. Tune these
 * if the PDM channel ends up too quiet or clipped. Max register value is
 * PDM_MAX_GAIN_CTRL = 0xFFF. */
#define PDM_CH_PRIMARY_PHASE             0x0000001F
#define PDM_CH_PRIMARY_GAIN              0x00000200
#define PDM_CH_PRIMARY_PEAK_DETECT_TH    0x00060002
#define PDM_CH_PRIMARY_PEAK_DETECT_ITV   0x0004002D

#define PDM_CH_SECONDARY_PHASE           0x00000003
#define PDM_CH_SECONDARY_GAIN            0x000002E0
#define PDM_CH_SECONDARY_PEAK_DETECT_TH  0x00060002
#define PDM_CH_SECONDARY_PEAK_DETECT_ITV 0x00020027

#define PDM_IIR_COEF                     0x00000004

/* FIR coefficients for the two channels. Even and odd PDM channels sample on
 * opposite edges of the shared PDM clock so each needs its own FIR response. */
static const uint32_t ch_fir_primary[18] = {
    0x00000001, 0x00000003, 0x00000003, 0x000007F4, 0x00000004, 0x000007ED,
    0x000007F5, 0x000007F4, 0x000007D3, 0x000007FE, 0x000007BC, 0x000007E5,
    0x000007D9, 0x00000793, 0x00000029, 0x0000072C, 0x00000072, 0x000002FD };

static const uint32_t ch_fir_secondary[18] = {
    0x00000000, 0x000007FF, 0x00000000, 0x00000004, 0x00000004, 0x000007FC,
    0x00000000, 0x000007FB, 0x000007E4, 0x00000000, 0x0000002B, 0x00000009,
    0x00000016, 0x00000049, 0x00000793, 0x000006F8, 0x00000045, 0x00000178 };

struct pdm_ch_params {
    uint32_t phase;
    uint32_t gain;
    uint32_t peak_detect_th;
    uint32_t peak_detect_itv;
    const uint32_t *fir;
};

static const struct pdm_ch_params pdm_params_primary = {
    .phase           = PDM_CH_PRIMARY_PHASE,
    .gain            = PDM_CH_PRIMARY_GAIN,
    .peak_detect_th  = PDM_CH_PRIMARY_PEAK_DETECT_TH,
    .peak_detect_itv = PDM_CH_PRIMARY_PEAK_DETECT_ITV,
    .fir             = ch_fir_primary,
};

static const struct pdm_ch_params pdm_params_secondary = {
    .phase           = PDM_CH_SECONDARY_PHASE,
    .gain            = PDM_CH_SECONDARY_GAIN,
    .peak_detect_th  = PDM_CH_SECONDARY_PEAK_DETECT_TH,
    .peak_detect_itv = PDM_CH_SECONDARY_PEAK_DETECT_ITV,
    .fir             = ch_fir_secondary,
};

/* State of one PDM block (PDM or LPPDM) */
struct pdm_mic {
    const char *name;
    ARM_DRIVER_PDM *drv;
    ARM_PDM_SignalEvent_t event_cb;
    uint32_t primary_ch;
    uint32_t secondary_ch;
    voice_callback_t rx_callback;
    uint32_t err_count;
    /* The block is the low power PDM (see receive_voice_data_pdm_block()). */
    bool is_lppdm;
};

/* Audio detect interrupt enable bits (one per channel) in PDM_IRQ_ENABLE */
#define PDM_AUDIO_DETECT_IRQ_MASK (0xFFU << 8U)

static void pdm_event_handler(struct pdm_mic *m, uint32_t event)
{
    if (event & ARM_PDM_EVENT_ERROR) {
        m->err_count++;
        if (m->err_count == 1) {
            printf("*** %s callback: ARM_PDM_EVENT_ERROR (FIFO overflow, further occurrences suppressed)***\n",
                   m->name);
        }
    }

    if (event & ARM_PDM_EVENT_CAPTURE_COMPLETE) {
        if (m->rx_callback) {
            m->rx_callback(event);
        }
    }

    if (event & ARM_PDM_EVENT_AUDIO_DETECTION) {
        printf("*** %s callback: ARM_PDM_EVENT_AUDIO_DETECTION ***\n", m->name);
    }
}

static int32_t pdm_mode(uint32_t sampling_rate)
{
    // selects the mode based on requested sampling rate
    switch(sampling_rate)
    {
        case 8000:
            return ARM_PDM_MODE_AUDIOFREQ_8K_DECM_64;

        case 16000:
            return ARM_PDM_MODE_AUDIOFREQ_16K_DECM_64;

        case 32000:
            return ARM_PDM_MODE_AUDIOFREQ_32K_DECM_48;

        case 48000:
            return ARM_PDM_MODE_AUDIOFREQ_48K_DECM_64;

        case 96000:
            return ARM_PDM_MODE_AUDIOFREQ_96K_DECM_50;

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

/* Issues a driver Control() call and prints an error on failure. */
static int32_t pdm_control(const struct pdm_mic *m, uint32_t control, uint32_t arg1,
                           uint32_t arg2, const char *what)
{
    int32_t ret = m->drv->Control(control, arg1, arg2);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: %s %s failed\n", m->name, what);
        return -1;
    }
    return 0;
}

/* Programs phase, gain, peak detect and filter coefficients of one channel. */
static int32_t pdm_configure_channel(const struct pdm_mic *m, uint32_t ch,
                                     const struct pdm_ch_params *p)
{
    if (pdm_control(m, ARM_PDM_CHANNEL_PHASE, ch, p->phase, "Channel_Config") ||
        pdm_control(m, ARM_PDM_CHANNEL_GAIN, ch, p->gain, "Channel_Config") ||
        pdm_control(m, ARM_PDM_CHANNEL_PEAK_DETECT_TH, ch, p->peak_detect_th, "Channel_Config") ||
        pdm_control(m, ARM_PDM_CHANNEL_PEAK_DETECT_ITV, ch, p->peak_detect_itv, "Channel_Config")) {
        return -1;
    }

    PDM_CH_CONFIG pdm_coef_reg;
    pdm_coef_reg.ch_num = ch;
    memcpy(pdm_coef_reg.ch_fir_coef, p->fir, sizeof(pdm_coef_reg.ch_fir_coef));
    pdm_coef_reg.ch_iir_coef = PDM_IIR_COEF; /* Channel IIR Filter Coefficient */

    int32_t ret = m->drv->Config(&pdm_coef_reg);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: %s Channel_Config failed\n", m->name);
        return -1;
    }
    return 0;
}

static int32_t init_microphone_pdm_block(struct pdm_mic *m, uint32_t sampling_rate,
                                         uint32_t data_bit_len)
{
    int32_t ret;
    /* Initialize PDM driver */
    ret = m->drv->Initialize(m->event_cb);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: %s init failed\n", m->name);
        return -1;
    }

    /* Enable the power for PDM */
    ret = m->drv->PowerControl(ARM_POWER_FULL);
    if (ret != ARM_DRIVER_OK) {
        printf("\r\n Error: %s Power up failed\n", m->name);
        return -1;
    }

    /* Select the two PDM channels in use */
    if (pdm_control(m, ARM_PDM_SELECT_CHANNEL,
                    ((1U << m->primary_ch) | (1U << m->secondary_ch)), 0, "channel select")) {
        return -1;
    }

    /* Select PDM mode based on the requested sampling rate*/
    int32_t mode = pdm_mode(sampling_rate);
    if (mode < 0) {
        printf("\r\n Error: Invalid sampling rate (%" PRIu32 ") for %s.", sampling_rate, m->name);
        return -1;
    }

    if (pdm_control(m, ARM_PDM_MODE, (uint32_t)mode, 0, "mode control")) {
        return -1;
    }

    /* Select resolution */
    int32_t pdm_resolution = resolution(data_bit_len);
    if (pdm_resolution < 0) {
        printf("\r\n Error: Invalid data bit len (%" PRIu32 ") for %s.", data_bit_len, m->name);
        return -1;
    }

    if (pdm_control(m, ARM_PDM_SELECT_RESOLUTION, (uint32_t)pdm_resolution, 0,
                    "resolution control")) {
        return -1;
    }

    /* Enable the DC blocking IIR filter (0 == don't bypass). The Alif demos
     * pass 1 here (= bypass) which is fine for raw-PDM analysis but leaves a
     * large DC component that is heard as rumble/noise on voice playback. */
    if (pdm_control(m, ARM_PDM_BYPASS_IIR_FILTER, 0, 0, "DC blocking IIR control")) {
        return -1;
    }

    /* Primary and secondary channel */
    if (pdm_configure_channel(m, m->primary_ch, &pdm_params_primary) ||
        pdm_configure_channel(m, m->secondary_ch, &pdm_params_secondary)) {
        return -1;
    }

    return 0;
}

static int32_t enable_microphone_pdm_block(struct pdm_mic *m, voice_callback_t callback)
{
    m->rx_callback = callback;
    return 0;
}

static int32_t disable_microphone_pdm_block(struct pdm_mic *m)
{
    (void)m;
    return 0;
}

static int32_t receive_voice_data_pdm_block(struct pdm_mic *m, void *data, uint32_t data_len)
{
    if (!m->is_lppdm) {
        return m->drv->Receive(data, data_len);
    }

    /* LPPDM: Driver_PDM's Receive() unmasks the per-channel audio detect
     * interrupts, but the LPPDM interrupt is the OR of FIFO warning, error and
     * audio detect, and its handler only services the FIFO warning. The audio
     * detect status is never read there, so the shared IRQ line would stay
     * asserted and the CPU would be stuck in the LPPDM ISR forever. This
     * stream does not use audio detection, so mask it again. Done with
     * interrupts disabled so that the ISR can't run in between. */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    int32_t ret = m->drv->Receive(data, data_len);
    LPPDM->PDM_IRQ_ENABLE &= ~PDM_AUDIO_DETECT_IRQ_MASK;
    __set_PRIMASK(primask);
    return ret;
}

/* "PDM" stream ------------------------------------------------------------- */
#ifdef USE_PDM_MICS
#if defined(BOARD_LPPDM_ENABLED) && (BOARD_LPPDM_ENABLED == 1) && !defined(USE_LPPDM_MICS)
/* Microphones are wired to the low power PDM and there is no separate LPPDM
 * stream, so the PDM stream is served by the LPPDM block. */
extern ARM_DRIVER_PDM Driver_LPPDM;
#define PDM_STREAM_NAME   "LPPDM"
#define PDM_STREAM_DRIVER (&Driver_LPPDM)
#define PDM_STREAM_IS_LPPDM true
#else
extern ARM_DRIVER_PDM Driver_PDM;
#define PDM_STREAM_NAME   "PDM"
#define PDM_STREAM_DRIVER (&Driver_PDM)
#define PDM_STREAM_IS_LPPDM false
#endif

static void pdm_event_cb(uint32_t event);

static struct pdm_mic pdm_mic = {
    .name         = PDM_STREAM_NAME,
    .is_lppdm     = PDM_STREAM_IS_LPPDM,
    .drv          = PDM_STREAM_DRIVER,
    .event_cb     = pdm_event_cb,
    .primary_ch   = PRIMARY_CHANNEL,
    .secondary_ch = SECONDARY_CHANNEL,
};

static void pdm_event_cb(uint32_t event)
{
    pdm_event_handler(&pdm_mic, event);
}

static int32_t init_microphone_pdm(uint32_t sampling_rate, uint32_t data_bit_len)
{
    return init_microphone_pdm_block(&pdm_mic, sampling_rate, data_bit_len);
}

static int32_t enable_microphone_pdm(voice_callback_t callback)
{
    return enable_microphone_pdm_block(&pdm_mic, callback);
}

static int32_t disable_microphone_pdm(void)
{
    return disable_microphone_pdm_block(&pdm_mic);
}

static int32_t receive_voice_data_pdm(void *data, uint32_t data_len)
{
    return receive_voice_data_pdm_block(&pdm_mic, data, data_len);
}
#endif /* USE_PDM_MICS */

/* "LPPDM" stream ----------------------------------------------------------- */
#ifdef USE_LPPDM_MICS
extern ARM_DRIVER_PDM Driver_LPPDM;

static void lppdm_event_cb(uint32_t event);

static struct pdm_mic lppdm_mic = {
    .name         = "LPPDM",
    .is_lppdm     = true,
    .drv          = &Driver_LPPDM,
    .event_cb     = lppdm_event_cb,
    .primary_ch   = LPPDM_PRIMARY_CHANNEL,
    .secondary_ch = LPPDM_SECONDARY_CHANNEL,
};

static void lppdm_event_cb(uint32_t event)
{
    pdm_event_handler(&lppdm_mic, event);
}

static int32_t init_microphone_lppdm(uint32_t sampling_rate, uint32_t data_bit_len)
{
    return init_microphone_pdm_block(&lppdm_mic, sampling_rate, data_bit_len);
}

static int32_t enable_microphone_lppdm(voice_callback_t callback)
{
    return enable_microphone_pdm_block(&lppdm_mic, callback);
}

static int32_t disable_microphone_lppdm(void)
{
    return disable_microphone_pdm_block(&lppdm_mic);
}

static int32_t receive_voice_data_lppdm(void *data, uint32_t data_len)
{
    return receive_voice_data_pdm_block(&lppdm_mic, data, data_len);
}
#endif /* USE_LPPDM_MICS */
#endif /* USE_PDM_MICS || USE_LPPDM_MICS */

/* Per-mic dispatch --------------------------------------------------------- */

int32_t init_microphone_ex(mic_type_t mic, uint32_t sampling_rate, uint32_t data_bit_len)
{
    switch (mic) {
#ifdef USE_I2S_MICS
    case MIC_TYPE_I2S:
        return init_microphone_i2s(sampling_rate, data_bit_len);
#endif
#ifdef USE_PDM_MICS
    case MIC_TYPE_PDM:
        return init_microphone_pdm(sampling_rate, data_bit_len);
#endif
#ifdef USE_LPPDM_MICS
    case MIC_TYPE_LPPDM:
        return init_microphone_lppdm(sampling_rate, data_bit_len);
#endif
    default:
        return -1;
    }
}

int32_t enable_microphone_ex(mic_type_t mic, voice_callback_t callback)
{
    switch (mic) {
#ifdef USE_I2S_MICS
    case MIC_TYPE_I2S:
        return enable_microphone_i2s(callback);
#endif
#ifdef USE_PDM_MICS
    case MIC_TYPE_PDM:
        return enable_microphone_pdm(callback);
#endif
#ifdef USE_LPPDM_MICS
    case MIC_TYPE_LPPDM:
        return enable_microphone_lppdm(callback);
#endif
    default:
        return -1;
    }
}

int32_t disable_microphone_ex(mic_type_t mic)
{
    switch (mic) {
#ifdef USE_I2S_MICS
    case MIC_TYPE_I2S:
        return disable_microphone_i2s();
#endif
#ifdef USE_PDM_MICS
    case MIC_TYPE_PDM:
        return disable_microphone_pdm();
#endif
#ifdef USE_LPPDM_MICS
    case MIC_TYPE_LPPDM:
        return disable_microphone_lppdm();
#endif
    default:
        return -1;
    }
}

int32_t receive_voice_data_ex(mic_type_t mic, void *data, uint32_t data_len)
{
    switch (mic) {
#ifdef USE_I2S_MICS
    case MIC_TYPE_I2S:
        return receive_voice_data_i2s(data, data_len);
#endif
#ifdef USE_PDM_MICS
    case MIC_TYPE_PDM:
        return receive_voice_data_pdm(data, data_len);
#endif
#ifdef USE_LPPDM_MICS
    case MIC_TYPE_LPPDM:
        return receive_voice_data_lppdm(data, data_len);
#endif
    default:
        return -1;
    }
}

/* Single-mic API preserved for existing callers.
 * When several mics are compiled in, the single-mic API drives I2S first, then
 * PDM (arbitrary primary choice); callers that need more than one mic must use
 * the *_ex variants. */
#if defined(USE_I2S_MICS)
#define MIC_LISTENER_DEFAULT MIC_TYPE_I2S
#elif defined(USE_PDM_MICS)
#define MIC_LISTENER_DEFAULT MIC_TYPE_PDM
#elif defined(USE_LPPDM_MICS)
#define MIC_LISTENER_DEFAULT MIC_TYPE_LPPDM
#endif

#ifdef MIC_LISTENER_DEFAULT
int32_t init_microphone(uint32_t sampling_rate, uint32_t data_bit_len)
{
    return init_microphone_ex(MIC_LISTENER_DEFAULT, sampling_rate, data_bit_len);
}

int32_t enable_microphone(voice_callback_t callback)
{
    return enable_microphone_ex(MIC_LISTENER_DEFAULT, callback);
}

int32_t disable_microphone(void)
{
    return disable_microphone_ex(MIC_LISTENER_DEFAULT);
}

int32_t receive_voice_data(void *data, uint32_t data_len)
{
    return receive_voice_data_ex(MIC_LISTENER_DEFAULT, data, data_len);
}
#endif /* MIC_LISTENER_DEFAULT */
