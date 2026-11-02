/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "board_defs.h"
#include "tone_out.h"
#include <RTE_Device.h>
#include "RTE_Components.h"
#include CMSIS_device_header

#if defined(RTE_Drivers_WM8904_CODEC) && defined(BOARD_WM8904_CODEC_PRESENT) && \
    (BOARD_WM8904_CODEC_PRESENT == 1)

#include "Driver_SAI.h"
#include "WM8904_codec_i2c.h"
#include "pinconf.h"
#include "sys_utils.h"

/* WM8904 registers (WM8904_Private.h has the same, plus unused static prototypes) */
#define WM8904_SLAVE             0x1AU
#define WM8904_DEV_ID            0x8904U
#define WM8904_SW_RESET_ID       0x00U
#define WM8904_BIAS_CONTROL0     0x04U
#define WM8904_VMID_CONTROL0     0x05U
#define WM8904_POWER_MGMT2       0x0EU
#define WM8904_POWER_MGMT6       0x12U
#define WM8904_CLOCK_RATES1      0x15U
#define WM8904_CLOCK_RATES2      0x16U
#define WM8904_AUDIO_INTERFACE0  0x18U
#define WM8904_AUDIO_INTERFACE1  0x19U
#define WM8904_DAC_DIGITAL1      0x21U
#define WM8904_ANALOG_OUT1_LEFT  0x39U
#define WM8904_ANALOG_OUT1_RIGHT 0x3AU
#define WM8904_DC_SERVO0         0x43U
#define WM8904_DC_SERVO1         0x44U
#define WM8904_ANALOG_HP0        0x5AU
#define WM8904_CHARGE_PUMP0      0x62U
#define WM8904_CLASS_W0          0x68U
#define WM8904_FLL_CONTROL1      0x74U
#define WM8904_FLL_CONTROL2      0x75U
#define WM8904_FLL_CONTROL3      0x76U
#define WM8904_FLL_CONTROL4      0x77U
#define WM8904_FLL_CONTROL5      0x78U

#define TONE_SAMPLE_RATE 48000U
#define TONE_WORD_BITS   16U
/* 10 ms: a whole number of periods of every multiple of 100 Hz */
#define TONE_FRAMES      480U
#define TONE_MAX_FREQ    7900U

/* The codec has no MCLK requirement this way: its FLL locks to the I2S bit
 * clock. RTE_I2S2_WSS_CLOCK_CYCLES = 32 bit clocks per channel, so BCLK is
 * 64 * 48 kHz = 3.072 MHz. FLL: Fvco = BCLK * N * FRATIO = 98.304 MHz (must be
 * 90..100 MHz), SYSCLK = Fvco / OUTDIV = 12.288 MHz = 256 * fs. */
#define FLL_N            32U
#define FLL_OUTDIV       8U
#define FLL_CONTROL2_VAL (((FLL_OUTDIV - 1U) << 8) | 0U /* FRATIO = 1, Fref > 1 MHz */)
#define FLL_CONTROL4_VAL (FLL_N << 5)
#define FLL_REF_BCLK     0x0001U
#define FLL_ENA          0x0001U
#define CLOCK_RATES1_VAL 0x0C05U /* CLK_SYS_RATE = 256 * fs, SAMPLE_RATE = 48 kHz */
#define CLOCK_RATES2_VAL 0x4006U /* SYSCLK from FLL, SYSCLK and DSP clock on */
#define HPOUT_VU         0x0080U
#define HPOUT_VOL_MAX    63U

extern ARM_DRIVER_SAI ARM_Driver_SAI_(BOARD_DAC_OUTPUT_I2S_INSTANCE);
extern ARM_DRIVER_I2C ARM_Driver_I2C_(RTE_WM8904_CODEC_I2C_INSTANCE);

static ARM_DRIVER_SAI *i2s_dac = &ARM_Driver_SAI_(BOARD_DAC_OUTPUT_I2S_INSTANCE);

static WM8904_CODEC_SLAVE_I2C_CONFIG codec_i2c = {
    .drv_i2c                 = &ARM_Driver_I2C_(RTE_WM8904_CODEC_I2C_INSTANCE),
    .bus_speed               = ARM_I2C_BUS_SPEED_STANDARD,
    .wm8904_codec_slave_addr = WM8904_SLAVE,
};

/* Interleaved left/right; aligned to 4 bytes as the word select size is > 16 */
static int16_t tone_buf[TONE_FRAMES * 2] __ALIGNED(4);
static bool initialized;

static void dac_callback(uint32_t event)
{
    if (event & ARM_SAI_EVENT_SEND_COMPLETE) {
        /* Keep playing the same period buffer */
        i2s_dac->Send(tone_buf, TONE_FRAMES * 2);
    }
}

static int32_t codec_write(uint8_t reg, uint16_t value)
{
    int32_t ret = WM8904_codec_i2c_write(&codec_i2c, reg, value);
    if (ret != ARM_DRIVER_OK) {
        printf("tone_out: codec write 0x%02X failed\n", reg);
    }
    return ret;
}

static int32_t i2s_pins_config(void)
{
    if (pinconf_set(PORT_(BOARD_DAC_OUTPUT_SDO_GPIO_PORT), BOARD_DAC_OUTPUT_SDO_GPIO_PIN,
                    BOARD_DAC_OUTPUT_SDO_ALTERNATE_FUNCTION, 0) ||
        pinconf_set(PORT_(BOARD_DAC_OUTPUT_WS_GPIO_PORT), BOARD_DAC_OUTPUT_WS_GPIO_PIN,
                    BOARD_DAC_OUTPUT_WS_ALTERNATE_FUNCTION, 0) ||
        pinconf_set(PORT_(BOARD_DAC_OUTPUT_SCLK_GPIO_PORT), BOARD_DAC_OUTPUT_SCLK_GPIO_PIN,
                    BOARD_DAC_OUTPUT_SCLK_ALTERNATE_FUNCTION, 0)) {
        return -1;
    }
    return 0;
}

/* The bit clock must already run: the FLL locks to it. */
static int32_t codec_init(void)
{
    uint16_t value;

    if (WM8904_codec_i2c_init(&codec_i2c) != ARM_DRIVER_OK) {
        printf("tone_out: codec I2C init failed\n");
        return -1;
    }
    if (WM8904_codec_i2c_read(&codec_i2c, WM8904_SW_RESET_ID, &value) != ARM_DRIVER_OK ||
        value != WM8904_DEV_ID) {
        printf("tone_out: WM8904 not found\n");
        return -1;
    }

    /* The FLL needs the bias and VMID running. The default write sequence would
     * restart them and so stop SYSCLK, which hangs the sequencer: power up by hand. */
    if (codec_write(WM8904_SW_RESET_ID, 0) ||
        codec_write(WM8904_BIAS_CONTROL0, 0x0008) || /* ISEL = high performance */
        codec_write(WM8904_VMID_CONTROL0, 0x0047)) { /* VMID_BUF_ENA, fast start, VMID_ENA */
        return -1;
    }
    sys_busy_loop_us(100000);
    if (codec_write(WM8904_VMID_CONTROL0, 0x0043) || /* normal VMID resistance */
        codec_write(WM8904_BIAS_CONTROL0, 0x0009) || /* BIAS_ENA */
        codec_write(WM8904_FLL_CONTROL1, 0) ||
        codec_write(WM8904_FLL_CONTROL2, FLL_CONTROL2_VAL) ||
        codec_write(WM8904_FLL_CONTROL3, 0) ||
        codec_write(WM8904_FLL_CONTROL4, FLL_CONTROL4_VAL) ||
        codec_write(WM8904_FLL_CONTROL5, FLL_REF_BCLK) ||
        codec_write(WM8904_FLL_CONTROL1, FLL_ENA)) {
        return -1;
    }
    sys_busy_loop_us(5000);

    if (codec_write(WM8904_CLOCK_RATES1, CLOCK_RATES1_VAL) ||
        codec_write(WM8904_CLOCK_RATES2, CLOCK_RATES2_VAL)) {
        return -1;
    }

    if (WM8904_codec_i2c_read(&codec_i2c, WM8904_CLOCK_RATES2, &value) != ARM_DRIVER_OK ||
        value != CLOCK_RATES2_VAL) {
        /* Register access needs SYSCLK once it is selected */
        printf("tone_out: no codec SYSCLK, is the I2S bit clock running?\n");
        return -1;
    }

    if (codec_write(WM8904_AUDIO_INTERFACE0, 0x0050) || /* AIFADCR_SRC = AIFDACR_SRC = 1 */
        codec_write(WM8904_AUDIO_INTERFACE1, 0x0002) || /* I2S, 16-bit words */
        codec_write(WM8904_DAC_DIGITAL1, 0x0040) ||     /* DAC_OSR128, unmuted */
        codec_write(WM8904_POWER_MGMT2, 0x0003) ||      /* HPL/HPR PGA */
        codec_write(WM8904_POWER_MGMT6, 0x000C) ||      /* DACL/DACR */
        codec_write(WM8904_CHARGE_PUMP0, 0x0001) ||     /* CP_ENA */
        codec_write(WM8904_CLASS_W0, 0x0001)) {         /* CP_DYN_PWR */
        return -1;
    }
    sys_busy_loop_us(5000);

    /* Headphone power-up: input and intermediate stages, DC servo on HPOUTL/R,
     * output stage, remove the output shorts */
    if (codec_write(WM8904_ANALOG_HP0, 0x0011) ||
        codec_write(WM8904_ANALOG_HP0, 0x0033) ||
        codec_write(WM8904_DC_SERVO0, 0x0003) ||
        codec_write(WM8904_DC_SERVO1, 0x0030)) {
        return -1;
    }
    sys_busy_loop_us(100000);
    if (codec_write(WM8904_ANALOG_HP0, 0x0077) ||
        codec_write(WM8904_ANALOG_HP0, 0x00FF)) {
        return -1;
    }
    return 0;
}

int32_t tone_out_init(void)
{
    int32_t status;

    if (initialized) {
        return 0;
    }
    if (i2s_pins_config()) {
        printf("tone_out: I2S pinmux failed\n");
        return -1;
    }

    status = i2s_dac->Initialize(dac_callback);
    if (status == ARM_DRIVER_OK) {
        status = i2s_dac->PowerControl(ARM_POWER_FULL);
    }
    if (status == ARM_DRIVER_OK) {
        status = i2s_dac->Control(ARM_SAI_CONFIGURE_TX | ARM_SAI_MODE_MASTER |
                                      ARM_SAI_ASYNCHRONOUS | ARM_SAI_PROTOCOL_I2S |
                                      ARM_SAI_DATA_SIZE(TONE_WORD_BITS),
                                  TONE_WORD_BITS * 2, TONE_SAMPLE_RATE);
    }
    if (status == ARM_DRIVER_OK) {
        status = i2s_dac->Control(ARM_SAI_CONTROL_TX, 1, 0);
    }
    if (status == ARM_DRIVER_OK) {
        status = i2s_dac->Send(tone_buf, TONE_FRAMES * 2);
    }
    if (status != ARM_DRIVER_OK) {
        printf("tone_out: I2S start failed: %" PRId32 "\n", status);
    }
    if (status != ARM_DRIVER_OK || codec_init()) {
        i2s_dac->PowerControl(ARM_POWER_OFF);
        i2s_dac->Uninitialize();
        return -1;
    }

    initialized = true;
    return 0;
}

int32_t tone_out_set(uint32_t freq_hz, uint32_t amplitude)
{
    if (!initialized || freq_hz % 100U || freq_hz > TONE_MAX_FREQ || amplitude > INT16_MAX) {
        return -1;
    }
    for (uint32_t i = 0; i < TONE_FRAMES; ++i) {
        const float phase = 6.28318531f * (float)(freq_hz * i) / (float)TONE_SAMPLE_RATE;
        const int16_t s   = (int16_t)lrintf((float)amplitude * sinf(phase));
        tone_buf[2 * i]     = s;
        tone_buf[2 * i + 1] = s;
    }
    return 0;
}

int32_t tone_out_set_volume(uint32_t volume)
{
    if (!initialized || volume > HPOUT_VOL_MAX) {
        return -1;
    }
    if (codec_write(WM8904_ANALOG_OUT1_LEFT, (uint16_t)volume) ||
        codec_write(WM8904_ANALOG_OUT1_RIGHT, (uint16_t)(volume | HPOUT_VU))) {
        return -1;
    }
    return 0;
}

#else /* no codec */

int32_t tone_out_init(void)
{
    return -1;
}

int32_t tone_out_set(uint32_t freq_hz, uint32_t amplitude)
{
    (void)freq_hz;
    (void)amplitude;
    return -1;
}

int32_t tone_out_set_volume(uint32_t volume)
{
    (void)volume;
    return -1;
}

#endif
