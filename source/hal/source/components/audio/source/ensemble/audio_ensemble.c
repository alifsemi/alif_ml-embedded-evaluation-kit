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

#ifdef USE_I2S_MICS
#define AUDIO_REC_WIDTH 32
#elif defined(USE_PDM_MICS)
#define AUDIO_REC_WIDTH 16
#endif

#define AUDIO_L_ONLY 1
#define AUDIO_R_ONLY 2
#define AUDIO_LR_MIX 3

#define AUDIO_MICS  AUDIO_LR_MIX

#define MAX_GAIN 10000.0f // 80dB
//#define MAX_GAIN_INC_PER_STRIDE 1.05925373f // 0.5dB, so 1dB per second
#define MAX_GAIN_INC_PER_STRIDE 1.12201845f // 1dB, so 2dB per second

// Define with number of raw samples to store, for debugging
//#define STORE_AUDIO (16000*10)

// Two options for input format
#if AUDIO_REC_WIDTH == 32
// Note that we use 32-bit input for any >16-bit precision. The I2S peripheral has a 24-bit
// mode but doesn't sign extend, so 32-bit is easier to work with.
#define audio_rec_t int32_t
#elif AUDIO_REC_WIDTH == 16
#define audio_rec_t int16_t
#else
#error "AUDIO_REC_WIDTH must be 16 or 32"
#endif

static void copy_audio_rec_to_in(float16_t * __RESTRICT in, const audio_rec_t * __RESTRICT rec, int samples);

// Stereo record buffer
static audio_rec_t audio_rec[2][AUDIO_REC_SAMPLES * 2] __ALIGNED(32) __attribute__((section(".bss.audio_rec"))); // stereo record buffer

#ifdef STORE_AUDIO
audio_rec_t audio_store[STORE_AUDIO * 2] __attribute__((section(".bss.camera_frame_buf"))); // stereo record buffer
static size_t store_pos;
#endif


static audio_callback_t user_audio_callback =  NULL;

static int audio_current_rec_buf;

static int32_t current_dc = 0;
static float current_gain = MAX_GAIN;
static bool auto_gain = true;

static int16_t * restrict user_ptr;
static int user_length;
static atomic_int audio_received;
static atomic_int audio_async_error;


static void audio_start_next_rx(int data_to_go)
{
    if (data_to_go > AUDIO_REC_SAMPLES) {
        data_to_go = AUDIO_REC_SAMPLES;
    }
    int err = receive_voice_data(audio_rec[audio_current_rec_buf], data_to_go * 2);
    if (err) {
        audio_async_error = err;
    }
}

// Perform stereo->mono conversion and DC adjustment as we copy
// Gain will be handled later.
static void copy_audio_rec_to_in(float16_t * __RESTRICT in, const audio_rec_t * __RESTRICT rec, int len)
{
    const audio_rec_t *input = rec;
    float16_t *output = in;
    int32_t offset = current_dc;
#if AUDIO_REC_WIDTH == 32
    int64_t sum = 0;
#else
    int32_t sum = 0;
#endif
    int samples_to_go = len;
#if ENABLE_MVE_COPY_AUDIO_REC_TO_IN
    while (samples_to_go >= 8) {
#if AUDIO_REC_WIDTH == 32
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
        // Add it up to track the pre-adjustment mean
        sum = vaddlvaq(sum, mono.val[0]);
        sum = vaddlvaq(sum, mono.val[1]);
        // Subtract the current DC offset (necessary to not lose accuracy
        // when converting to float16)
        mono.val[0] = vqsubq(mono.val[0], offset);
        mono.val[1] = vqsubq(mono.val[1], offset);
        // Convert to float32, in range -1,+1, so as not to overflow float16
        float32x4x2_t mono_f32 = { vcvtq_n(mono.val[0], 31), vcvtq_n(mono.val[1], 31) };
        // Convert to float16 and pack
        float16x8_t mono_f16 = vuninitializedq_f16();
        mono_f16 = vcvtbq_f16_f32(mono_f16, mono_f32.val[0]);
        mono_f16 = vcvttq_f16_f32(mono_f16, mono_f32.val[1]);
#else // AUDIO_REC_WIDTH == 16
        // Classic 2-way deinterleave to get L and R
        int16x8x2_t stereo = vld2q(input);
#if AUDIO_MICS == AUDIO_LR_MIX
        // Average left and right
        int16x8_t mono = vrhaddq(stereo.val[0], stereo.val[1]);
#elif AUDIO_MICS == AUDIO_L_ONLY
        int16x8_t mono = stereo.val[0];
#elif AUDIO_MICS == AUDIO_R_ONLY
        int16x8_t mono = stereo.val[1];
#else
#error "which microphone?"
#endif
        // Add it up to track the pre-adjustment mean (32-bit sum - VADDLV only available for 32-bit vectors)
        sum = vaddvaq(sum, mono);
        // Subtract the current DC offset (necessary to not lose accuracy
        // when converting to float16)
        mono = vqsubq(mono, offset);
        // Convert to float16, in range -1,+1
        float16x8_t mono_f16 = vcvtq_n(mono, 15);
#endif // AUDIO_REC_WIDTH
        // Store 8 output values.
        vst1q(output, mono_f16);
        input += 16;
        output += 8;
        samples_to_go -= 8;
    }
#endif // ENABLE_MVE_COPY_AUDIO_REC_TO_IN
    while (samples_to_go > 0) {
#if AUDIO_MICS == AUDIO_LR_MIX
        // Average left and right
        int32_t mono = srshr(input[0], 1) + (input[1] >> 1);
#elif AUDIO_MICS == AUDIO_L_ONLY
        int32_t mono = input[0];
#elif AUDIO_MICS == AUDIO_R_ONLY
        int32_t mono = input[1];
#else
#error "which microphone?"
#endif
        // Add it up to track the pre-adjustment mean
        sum += mono;
        // Subtract the current DC offset (necessary to not lose accuracy
        // when converting to float16)
        mono = __QSUB(mono, offset);
        // Convert to float32, in range -1,+1, so as not to overflow float16
#if AUDIO_REC_WIDTH == 32
        float mono_f32 = mono * 0x1p-31f;
#else
        float mono_f32 = mono * 0x1p-15f;
#endif
        // Convert to float16
        *output++ = (float16_t) mono_f32;
        input += 2;
        samples_to_go -= 1;
    }
    // Update the DC offset based on the mean of this buffer
    int32_t mean = (int32_t) (sum / len);
    current_dc = (current_dc / 8) * 7 + mean / 8;
}

static void voice_data_cb(uint32_t event)
{
    (void) event;
    audio_current_rec_buf = !audio_current_rec_buf;
    int samples = AUDIO_REC_SAMPLES;
    int new_total = audio_received + AUDIO_REC_SAMPLES;
    if (new_total < user_length) {
        audio_start_next_rx(user_length - new_total);
    } else if (new_total > user_length) {
        samples = new_total - user_length;
        new_total = user_length;
    }
#ifdef STORE_AUDIO
    if (store_pos < sizeof audio_store / sizeof audio_store[0]) {
        memcpy(audio_store + store_pos,  audio_rec[!audio_current_rec_buf], samples * 2 * sizeof(int32_t));
        store_pos += 2 * samples;
    }
#endif
    copy_audio_rec_to_in((float16_t *) user_ptr + audio_received, audio_rec[!audio_current_rec_buf], samples);
    audio_received = new_total;
    if (audio_received >= user_length || audio_async_error) {
        if (user_audio_callback) {
            user_audio_callback(audio_async_error);
        }
    }
}

void audio_set_callback(audio_callback_t callback)
{
    user_audio_callback = callback;
}

int audio_init(int sampling_rate)
{
    int32_t err = init_microphone(sampling_rate, AUDIO_REC_WIDTH);

    if (err == 0) {
        // Enable microphone
        err = enable_microphone(voice_data_cb);
    }

    return err;
}

int get_audio_samples_received(void)
{
    return audio_received;
}

int get_audio_data(int16_t *data, int len)
{
    user_ptr = data;
    user_length = len;
    audio_received = 0;
    audio_async_error = 0;
    audio_start_next_rx(user_length);

    return audio_async_error;
}

int wait_for_audio(void)
{
    while (audio_received < user_length && audio_async_error == 0) {
        __WFE();
    }
    return audio_async_error;
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


void set_audio_gain(float gain_db)
{
    if (isnan(gain_db)) {
        auto_gain = true;
    } else {
        auto_gain = false;
        current_gain = gain_db;
    }
}


/* Reads the input in float16 format
 * Adjusts gain up or down, attempting to get full-scale input
 * Applies
 */
void audio_preprocessing(int16_t *audio, int samples)
{
    float16_t *audio_fp = (float16_t *) audio;
    float16_t audio_mean, audio_absmax;

    arm_mean_f16(audio_fp, samples, &audio_mean);
    arm_absmax_no_idx_f16(audio_fp, samples, &audio_absmax);
    //if (audio_absmax == INT16_MIN) audio_absmax = INT16_MAX; // CMSIS-DSP issue #66
    printf("Original sample stats: absmax = %ld, mean = %ld\n", lround(32768*audio_absmax), lround(32768*audio_mean));

    if (auto_gain) {
        // Rescale to full range  while converting to integer
        float new_gain = fmin(1.0f / audio_absmax, MAX_GAIN);
        // Reduce gain immediately if necessary to avoid clipping, or increase slowly
        current_gain = fmin(new_gain, current_gain * MAX_GAIN_INC_PER_STRIDE);
    }
    convert_to_s16_from_f16_with_gain(audio, samples, current_gain);

    q15_t audio_mean_q15, audio_absmax_q15;
    arm_mean_q15(audio, samples, &audio_mean_q15);
    arm_absmax_no_idx_q15(audio, samples, &audio_absmax_q15);
    if (audio_absmax_q15 == INT16_MIN) audio_absmax_q15 = INT16_MAX; // CMSIS-DSP issue #66
    printf("Normalized sample stats: absmax = %d, mean = %d (gain = %.0f dB)\n", audio_absmax_q15, audio_mean_q15, 20 * log10f(current_gain) );
}
