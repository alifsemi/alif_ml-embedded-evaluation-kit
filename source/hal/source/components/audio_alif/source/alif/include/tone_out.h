/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef TONE_OUT_H_
#define TONE_OUT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Test tone on the headphone output of the board's WM8904 codec (I2S2 at
 * 48 kHz), used for microphone calibration. Call after audio_init(), which
 * enables the audio clocks. All functions return -1 on boards without the
 * codec.
 */

/** @brief Start the codec and play silence */
int32_t tone_out_init(void);

/**
 * @brief Play a sine on both headphone channels
 *
 * @param freq_hz   multiple of 100 Hz up to 7900 Hz, 0 for silence
 * @param amplitude peak value, 0..32767
 */
int32_t tone_out_set(uint32_t freq_hz, uint32_t amplitude);

/**
 * @brief Set the headphone volume
 *
 * @param volume 0..63 in 1 dB steps, 57 = 0 dB
 */
int32_t tone_out_set_volume(uint32_t volume);

#ifdef __cplusplus
}
#endif

#endif /* TONE_OUT_H_ */
