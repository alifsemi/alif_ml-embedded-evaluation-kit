/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <tgmath.h>

#include "RTE_Components.h"

#include "arm_math_f16.h"
#if __ARM_FEATURE_MVE
#include "arm_mve.h"
#endif

#include "audio_data.h"
#include "mic_listener.h"
#include "platform_drivers.h"

// At the time of writing, GCC produces incorrect assembly
#if defined(__ARMCC_VERSION) && (__ARM_FEATURE_MVE & 2)
#define ENABLE_MVE_COPY_AUDIO_REC_TO_IN 1
#else
#define ENABLE_MVE_COPY_AUDIO_REC_TO_IN 0
#endif

#if !(__ARM_FEATURE_MVE & 1)
static int32_t srshr(int32_t n, unsigned shift)
{
    return (n + (1 << (shift - 1))) >> shift;
}
#endif

#define AUDIO_REC_SAMPLES 512

/* Per-mic sample widths. I2S captures 32-bit words (used because the SAI 24-bit
 * mode does not sign-extend), PDM captures 16-bit words. */
#define AUDIO_REC_WIDTH_I2S 32
#define AUDIO_REC_WIDTH_PDM 16

#define AUDIO_L_ONLY 1
#define AUDIO_R_ONLY 2
#define AUDIO_LR_MIX 3

#ifndef USE_MIC
#define USE_MIC             AUDIO_LR_MIX
#endif

#define AUDIO_MICS          USE_MIC

#define MAX_GAIN 1000.0f // 80dB
//#define MAX_GAIN_INC_PER_STRIDE 1.05925373f // 0.5dB, so 1dB per second
#define MAX_GAIN_INC_PER_STRIDE 1.12201845f // 1dB, so 2dB per second

// Define with number of raw samples to store, for debugging
//#define STORE_AUDIO (16000*10)

#if !defined(USE_I2S_MICS) && !defined(USE_PDM_MICS)
#error "USE_I2S_MICS or USE_PDM_MICS must be defined"
#endif

/* Per-mic stream state. One instance per compiled-in microphone. */
struct audio_stream_state {
    audio_callback_t user_cb;
    int current_rec_buf;
    int32_t current_dc;
    float current_gain;
    bool auto_gain;
    int16_t *user_ptr;
    int user_length;
    atomic_int received;
    atomic_int async_error;
};

#ifdef USE_I2S_MICS
static struct audio_stream_state stream_i2s = {
    .current_gain = MAX_GAIN,
    .auto_gain    = true,
};
static int32_t audio_rec_i2s[2][AUDIO_REC_SAMPLES * 2] __ALIGNED(32) __attribute__((section(".bss.audio_rec")));
#endif

#ifdef USE_PDM_MICS
static struct audio_stream_state stream_pdm = {
    .current_gain = MAX_GAIN,
    .auto_gain    = true,
};
static int16_t audio_rec_pdm[2][AUDIO_REC_SAMPLES * 2] __ALIGNED(32) __attribute__((section(".bss.audio_rec")));
#endif

static struct audio_stream_state *stream_for(audio_mic_t mic)
{
    switch (mic) {
#ifdef USE_I2S_MICS
    case AUDIO_MIC_I2S:
        return &stream_i2s;
#endif
#ifdef USE_PDM_MICS
    case AUDIO_MIC_PDM:
        return &stream_pdm;
#endif
    default:
        return NULL;
    }
}

static mic_type_t mic_listener_type_for(audio_mic_t mic)
{
    return (mic == AUDIO_MIC_PDM) ? MIC_TYPE_PDM : MIC_TYPE_I2S;
}

#ifdef STORE_AUDIO
/* Debug storage buffer; when both mics are enabled it stores the I2S stream. */
#ifdef USE_I2S_MICS
typedef int32_t audio_store_t;
#else
typedef int16_t audio_store_t;
#endif
audio_store_t audio_store[STORE_AUDIO * 2] __attribute__((section(".bss.camera_frame_buf"))); // stereo record buffer
static size_t store_pos;
#endif

/* ------------------------------------------------------------------------- */
/*   copy_*_rec_to_in - stereo -> mono conversion + DC tracking + f16 pack    */
/* ------------------------------------------------------------------------- */

#ifdef USE_I2S_MICS
/* 32-bit I2S source -> float16 output. */
static void copy_i2s_rec_to_in(struct audio_stream_state *s,
                               float16_t * __RESTRICT in,
                               const int32_t * __RESTRICT rec,
                               int len)
{
    const int32_t *input = rec;
    float16_t *output = in;
    int32_t offset = s->current_dc;
    int64_t sum = 0;
    int samples_to_go = len;
#if ENABLE_MVE_COPY_AUDIO_REC_TO_IN
    while (samples_to_go >= 8) {
        // Use 4-way deinterleave to load 4 sets of L/R/L/R.
        // Four vectors in produce one vector out, due to stereo->mono
        // conversion and 32-bit to 16-bit reduction.
        int32x4x4_t stereox2 = vld4q(input);
#if AUDIO_MICS == AUDIO_LR_MIX
        // Average left and right
        int32x4x2_t mono = {
            vrhaddq(stereox2.val[0], stereox2.val[1]),
            vrhaddq(stereox2.val[2], stereox2.val[3])
        };
#elif AUDIO_MICS == AUDIO_L_ONLY
        int32x4x2_t mono = { stereox2.val[0], stereox2.val[2] };
#elif AUDIO_MICS == AUDIO_R_ONLY
        int32x4x2_t mono = { stereox2.val[1], stereox2.val[3] };
#else
#error "which microphone?"
#endif
        sum = vaddlvaq(sum, mono.val[0]);
        sum = vaddlvaq(sum, mono.val[1]);
        mono.val[0] = vqsubq(mono.val[0], offset);
        mono.val[1] = vqsubq(mono.val[1], offset);
        float32x4x2_t mono_f32 = { vcvtq_n(mono.val[0], 31), vcvtq_n(mono.val[1], 31) };
        float16x8_t mono_f16 = vuninitializedq_f16();
        mono_f16 = vcvtbq_f16_f32(mono_f16, mono_f32.val[0]);
        mono_f16 = vcvttq_f16_f32(mono_f16, mono_f32.val[1]);
        vst1q(output, mono_f16);
        input += 16;
        output += 8;
        samples_to_go -= 8;
    }
#endif // ENABLE_MVE_COPY_AUDIO_REC_TO_IN
    while (samples_to_go > 0) {
#if AUDIO_MICS == AUDIO_LR_MIX
        int32_t mono = srshr(input[0], 1) + (input[1] >> 1);
#elif AUDIO_MICS == AUDIO_L_ONLY
        int32_t mono = input[0];
#elif AUDIO_MICS == AUDIO_R_ONLY
        int32_t mono = input[1];
#else
#error "which microphone?"
#endif
        sum += mono;
        mono = __QSUB(mono, offset);
        float mono_f32 = mono * 0x1p-31f;
        *output++ = (float16_t) mono_f32;
        input += 2;
        samples_to_go -= 1;
    }
    int32_t mean = (int32_t) (sum / len);
    s->current_dc = (s->current_dc / 8) * 7 + mean / 8;
}
#endif // USE_I2S_MICS

#ifdef USE_PDM_MICS
/* 16-bit PDM source -> float16 output. */
static void copy_pdm_rec_to_in(struct audio_stream_state *s,
                               float16_t * __RESTRICT in,
                               const int16_t * __RESTRICT rec,
                               int len)
{
    const int16_t *input = rec;
    float16_t *output = in;
    int32_t offset = s->current_dc;
    int32_t sum = 0;
    int samples_to_go = len;
#if ENABLE_MVE_COPY_AUDIO_REC_TO_IN
    while (samples_to_go >= 8) {
        // Classic 2-way deinterleave to get L and R
        int16x8x2_t stereo = vld2q(input);
#if AUDIO_MICS == AUDIO_LR_MIX
        int16x8_t mono = vrhaddq(stereo.val[0], stereo.val[1]);
#elif AUDIO_MICS == AUDIO_L_ONLY
        int16x8_t mono = stereo.val[0];
#elif AUDIO_MICS == AUDIO_R_ONLY
        int16x8_t mono = stereo.val[1];
#else
#error "which microphone?"
#endif
        sum = vaddvaq(sum, mono);
        mono = vqsubq(mono, offset);
        float16x8_t mono_f16 = vcvtq_n(mono, 15);
        vst1q(output, mono_f16);
        input += 16;
        output += 8;
        samples_to_go -= 8;
    }
#endif // ENABLE_MVE_COPY_AUDIO_REC_TO_IN
    while (samples_to_go > 0) {
#if AUDIO_MICS == AUDIO_LR_MIX
        int32_t mono = srshr(input[0], 1) + (input[1] >> 1);
#elif AUDIO_MICS == AUDIO_L_ONLY
        int32_t mono = input[0];
#elif AUDIO_MICS == AUDIO_R_ONLY
        int32_t mono = input[1];
#else
#error "which microphone?"
#endif
        sum += mono;
        mono = __QSUB(mono, offset);
        float mono_f32 = mono * 0x1p-15f;
        *output++ = (float16_t) mono_f32;
        input += 2;
        samples_to_go -= 1;
    }
    int32_t mean = (int32_t) (sum / len);
    s->current_dc = (s->current_dc / 8) * 7 + mean / 8;
}
#endif // USE_PDM_MICS

/* ------------------------------------------------------------------------- */
/*   Async RX plumbing                                                        */
/* ------------------------------------------------------------------------- */

static void audio_start_next_rx(audio_mic_t mic, int data_to_go)
{
    struct audio_stream_state *s = stream_for(mic);
    if (!s) {
        return;
    }
    if (data_to_go > AUDIO_REC_SAMPLES) {
        data_to_go = AUDIO_REC_SAMPLES;
    }
    void *buf = NULL;
    switch (mic) {
#ifdef USE_I2S_MICS
    case AUDIO_MIC_I2S:
        buf = audio_rec_i2s[s->current_rec_buf];
        break;
#endif
#ifdef USE_PDM_MICS
    case AUDIO_MIC_PDM:
        buf = audio_rec_pdm[s->current_rec_buf];
        break;
#endif
    default:
        return;
    }
    int err = receive_voice_data_ex(mic_listener_type_for(mic), buf, data_to_go * 2);
    if (err) {
        s->async_error = err;
    }
}

static void voice_data_cb_common(audio_mic_t mic)
{
    struct audio_stream_state *s = stream_for(mic);
    if (!s) {
        return;
    }
    int previous_rec_buf = s->current_rec_buf;
    s->current_rec_buf = !s->current_rec_buf;
    int samples = AUDIO_REC_SAMPLES;
    int new_total = s->received + AUDIO_REC_SAMPLES;
    if (new_total < s->user_length) {
        audio_start_next_rx(mic, s->user_length - new_total);
    } else if (new_total > s->user_length) {
        samples = s->user_length - s->received;
        new_total = s->user_length;
    }
    switch (mic) {
#ifdef USE_I2S_MICS
    case AUDIO_MIC_I2S: {
#ifdef STORE_AUDIO
        if (store_pos < sizeof audio_store / sizeof audio_store[0]) {
            memcpy(audio_store + store_pos, audio_rec_i2s[previous_rec_buf],
                   samples * 2 * sizeof(int32_t));
            store_pos += 2 * samples;
        }
#endif
        copy_i2s_rec_to_in(s, (float16_t *) s->user_ptr + s->received,
                           audio_rec_i2s[previous_rec_buf], samples);
        break;
    }
#endif
#ifdef USE_PDM_MICS
    case AUDIO_MIC_PDM: {
        copy_pdm_rec_to_in(s, (float16_t *) s->user_ptr + s->received,
                           audio_rec_pdm[previous_rec_buf], samples);
        break;
    }
#endif
    default:
        break;
    }
    (void)previous_rec_buf;
    s->received = new_total;
    if (s->received >= s->user_length || s->async_error) {
        if (s->user_cb) {
            s->user_cb(s->async_error);
        }
    }
}

#ifdef USE_I2S_MICS
static void voice_data_cb_i2s(uint32_t event)
{
    (void) event;
    voice_data_cb_common(AUDIO_MIC_I2S);
}
#endif

#ifdef USE_PDM_MICS
static void voice_data_cb_pdm(uint32_t event)
{
    (void) event;
    voice_data_cb_common(AUDIO_MIC_PDM);
}
#endif

/* ------------------------------------------------------------------------- */
/*   Public per-mic API                                                       */
/* ------------------------------------------------------------------------- */

void audio_set_callback_ex(audio_mic_t mic, audio_callback_t callback)
{
    struct audio_stream_state *s = stream_for(mic);
    if (s) {
        s->user_cb = callback;
    }
}

int audio_init_ex(audio_mic_t mic, int sampling_rate)
{
    struct audio_stream_state *s = stream_for(mic);
    if (!s) {
        return -1;
    }
    int32_t ret = (int32_t)enable_audio_peripheral_clocks();
    if (ret != 0) {
        printf("audio_init enable_audio_peripheral_clocks failed: %" PRIi32 "\n", ret);
        return ret;
    }
    uint32_t width;
    voice_callback_t cb;
    switch (mic) {
#ifdef USE_I2S_MICS
    case AUDIO_MIC_I2S:
        width = AUDIO_REC_WIDTH_I2S;
        cb = voice_data_cb_i2s;
        break;
#endif
#ifdef USE_PDM_MICS
    case AUDIO_MIC_PDM:
        width = AUDIO_REC_WIDTH_PDM;
        cb = voice_data_cb_pdm;
        break;
#endif
    default:
        return -1;
    }
    int32_t err = init_microphone_ex(mic_listener_type_for(mic), sampling_rate, width);
    if (err == 0) {
        err = enable_microphone_ex(mic_listener_type_for(mic), cb);
    }
    return err;
}

int audio_uninit_ex(audio_mic_t mic)
{
    return (int)disable_microphone_ex(mic_listener_type_for(mic));
}

int get_audio_samples_received_ex(audio_mic_t mic)
{
    struct audio_stream_state *s = stream_for(mic);
    return s ? (int)s->received : 0;
}

int get_audio_data_ex(audio_mic_t mic, int16_t *data, int len)
{
    struct audio_stream_state *s = stream_for(mic);
    if (!s) {
        return -1;
    }
    s->user_ptr = data;
    s->user_length = len;
    s->received = 0;
    s->async_error = 0;
    audio_start_next_rx(mic, s->user_length);
    return s->async_error;
}

int wait_for_audio_ex(audio_mic_t mic)
{
    struct audio_stream_state *s = stream_for(mic);
    if (!s) {
        return -1;
    }
    while (s->received < s->user_length && s->async_error == 0) {
        __WFE();
    }
    return s->async_error;
}

static void convert_to_s16_from_f16_with_gain(void *ptr, int length, float16_t gain)
{
    while (length > 0) {
#if __ARM_FEATURE_MVE & 2
        // Check whether we're doing 8 or fewer
        mve_pred16_t p = vctp16q(length);
        // Load up to 8 samples
        float16x8_t fp = vld1q_z_f16(ptr, p);
        // Multiply by gain
        fp = vmulq_x(fp, gain, p);
        // Convert back to int16_t, rescaling for q15 (can't specify rounding with rescale)
        int16x8_t data = vcvtq_x_n_s16_f16(fp, 15, p);
        // Store back up to 8 samples
        vst1q_p_s16(ptr, data, p);
        ptr = (int16_t *) ptr + 8;
        length -= 8;
#else
        float16_t fp = *(float16_t *) ptr;
        fp *= gain;
        int16_t data = (int16_t) (fp * 0x1p15f);
        *(int16_t *) ptr = data;
        ptr = (int16_t *) ptr + 1;
        length -= 1;
#endif
    }
}


void set_audio_gain_ex(audio_mic_t mic, float gain_db)
{
    struct audio_stream_state *s = stream_for(mic);
    if (!s) {
        return;
    }
    if (isnan(gain_db)) {
        s->auto_gain = true;
    } else {
        s->auto_gain = false;
        s->current_gain = gain_db;
    }
}

/* Reads the input in float16 format
 * Adjusts gain up or down, attempting to get full-scale input
 * Applies
 */
void audio_preprocessing_ex(audio_mic_t mic, int16_t *audio, int samples)
{
    struct audio_stream_state *s = stream_for(mic);
    if (!s) {
        return;
    }
    float16_t *audio_fp = (float16_t *) audio;
    float16_t audio_mean, audio_absmax;

    arm_mean_f16(audio_fp, samples, &audio_mean);
    arm_absmax_no_idx_f16(audio_fp, samples, &audio_absmax);
    //if (audio_absmax == INT16_MIN) audio_absmax = INT16_MAX; // CMSIS-DSP issue #66
#ifndef GPIO_PROFILING
    // printf("Original sample stats: absmax = %ld, mean = %ld\n", lround(32768*audio_absmax), lround(32768*audio_mean));
#endif
    if (s->auto_gain) {
        // Rescale to full range while converting to integer
        float new_gain = fmin(1.0f / audio_absmax, MAX_GAIN);
        // Reduce gain immediately if necessary to avoid clipping, or increase slowly
        s->current_gain = fmin(new_gain, s->current_gain * MAX_GAIN_INC_PER_STRIDE);
    }
    convert_to_s16_from_f16_with_gain(audio, samples, s->current_gain);

    q15_t audio_mean_q15, audio_absmax_q15;
    arm_mean_q15(audio, samples, &audio_mean_q15);
    arm_absmax_no_idx_q15(audio, samples, &audio_absmax_q15);
    if (audio_absmax_q15 == INT16_MIN) audio_absmax_q15 = INT16_MAX; // CMSIS-DSP issue #66
#ifndef GPIO_PROFILING
    // printf("Normalized sample stats: absmax = %d, mean = %d (gain = %.0f dB)\n", audio_absmax_q15, audio_mean_q15, 20 * log10f(s->current_gain) );
#endif
}

/* ------------------------------------------------------------------------- */
/*   Backwards-compatible single-mic API                                      */
/*   When both mics are compiled in the legacy API drives the I2S stream.     */
/* ------------------------------------------------------------------------- */

#if defined(USE_I2S_MICS)
#define AUDIO_DEFAULT_MIC AUDIO_MIC_I2S
#else
#define AUDIO_DEFAULT_MIC AUDIO_MIC_PDM
#endif

void audio_set_callback(audio_callback_t callback)
{
    audio_set_callback_ex(AUDIO_DEFAULT_MIC, callback);
}

int audio_init(int sampling_rate)
{
    return audio_init_ex(AUDIO_DEFAULT_MIC, sampling_rate);
}

int audio_uninit()
{
    return audio_uninit_ex(AUDIO_DEFAULT_MIC);
}

int get_audio_samples_received(void)
{
    return get_audio_samples_received_ex(AUDIO_DEFAULT_MIC);
}

int get_audio_data(int16_t *data, int len)
{
    return get_audio_data_ex(AUDIO_DEFAULT_MIC, data, len);
}

int wait_for_audio(void)
{
    return wait_for_audio_ex(AUDIO_DEFAULT_MIC);
}

void set_audio_gain(float gain_db)
{
    set_audio_gain_ex(AUDIO_DEFAULT_MIC, gain_db);
}

void audio_preprocessing(int16_t *audio, int samples)
{
    audio_preprocessing_ex(AUDIO_DEFAULT_MIC, audio, samples);
}
