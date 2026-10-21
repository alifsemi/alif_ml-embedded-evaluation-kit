/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/* Audio streaming over Ethernet as RTP (RFC 3550) with L16 payload
 * (RFC 3551: 16-bit big-endian PCM, interleaved channels) over UDP/IPv4.
 * Configured at build time, see the ETH_STREAM_* options in usecase.cmake. */

#ifndef ETH_STREAM_H
#define ETH_STREAM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise the Ethernet MAC for streaming. Takes a few ms (MAC reset), does
 * not wait for the link or the receiver's MAC address.
 * @param sample_rate Audio sample rate, used for the RTP clock (log only).
 * @return 0 on success, negative on error. On error eth_stream_send() is a
 *         no-op.
 */
int32_t eth_stream_init(uint32_t sample_rate);

/**
 * Send interleaved stereo 16-bit frames. Never blocks: as many packets as the
 * MAC TX ring (8 packets) takes are queued right away, the rest are sent from
 * the TX complete interrupt. In that case the buffer is read until then
 * (~80 us per packet at 100 Mbit/s), so it must stay unchanged meanwhile.
 * While the receiver's MAC address is not known yet the audio is dropped, and
 * audio still unsent when the next call comes is dropped too.
 * @param frames     Interleaved L/R samples.
 * @param num_frames Number of stereo frames.
 */
void eth_stream_send(const int16_t* frames, uint32_t num_frames);

#ifdef __cplusplus
}
#endif

#endif /* ETH_STREAM_H */
