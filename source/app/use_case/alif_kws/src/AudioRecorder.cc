/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifdef KWS_RECORD_MODE

#include "UseCaseHandler.hpp"

#include "hal.h"
#include "mlek/log/log_macros.h"
#include "tone_out.h"
#include "uart_tracelib.h"

extern "C" {
#include "mic_listener.h"
}

#include <cinttypes>
#include <cstdio>
#include <cstdlib>

/*
 * Protocol on the console UART (115200 8N1):
 *   host -> board: 'R' to record, or 'S' to resend the last block
 *   board -> host: "\nKWSREC <rate> <samples> <crc32 hex>\n", <samples> little-endian
 *                  float16 values, "\nKWSEND\n"
 * There is no flow control, so the host checks the length and CRC and resends on error.
 *
 * Microphone calibration, 'C' followed by a line ending in '\n':
 *   "CT <freq> <amp>"         codec headphone tone, see tone_out_set()
 *   "CV <volume>"             headphone volume, see tone_out_set_volume()
 *   "CP <ch> <phase> <gain>"  PDM channel 0, 1 or 2 (both), see pdm_mic_set_channel()
 *   "CM <decimation>"         PDM decimation, see pdm_mic_set_decimation()
 *   board -> host: "\nOK\n" or "\nERR\n"
 *   "CW <frames>"             raw stereo capture, board -> host: "\nKWSRAW <rate> <frames> 2
 *                             <crc32 hex>\n", interleaved little-endian int16 L/R, "\nKWSEND\n",
 *                             or "\nERR\n"
 */

namespace alif {
namespace app {

    static constexpr char RECORD_COMMAND    = 'R';
    static constexpr char RESEND_COMMAND    = 'S';
    static constexpr char CALIBRATE_COMMAND = 'C';
    static constexpr uint32_t UART_CHUNK    = 4096;

    struct Block {
        char header[64];
        int headerLen;
        const uint8_t* data;
        size_t bytes;
    };

    static uint32_t Crc32(const uint8_t* data, size_t len)
    {
        uint32_t crc = 0xFFFFFFFFu;
        for (size_t i = 0; i < len; ++i) {
            crc ^= data[i];
            for (int bit = 0; bit < 8; ++bit) {
                crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
            }
        }
        return ~crc;
    }

    static void SendRaw(const uint8_t* data, size_t len)
    {
        while (len > 0) {
            const uint32_t chunk = len > UART_CHUNK ? UART_CHUNK : static_cast<uint32_t>(len);
            uart_send(reinterpret_cast<const char*>(data), chunk);
            data += chunk;
            len -= chunk;
        }
    }

    static void SendBlock(const Block& block)
    {
        uart_send(block.header, static_cast<uint32_t>(block.headerLen));
        SendRaw(block.data, block.bytes);
        uart_send("\nKWSEND\n", 8);
    }

    static void Reply(bool ok)
    {
        if (ok) {
            uart_send("\nOK\n", 4);
        } else {
            uart_send("\nERR\n", 5);
        }
    }

    static void ReadLine(char* line, size_t size)
    {
        size_t len = 0;
        for (char c = uart_getchar(); c != '\n'; c = uart_getchar()) {
            if (len + 1 < size) {
                line[len++] = c;
            }
        }
        line[len] = '\0';
    }

    /* Parses count unsigned decimal numbers separated by spaces. */
    static bool ParseUints(const char* text, unsigned* values, int count)
    {
        for (int i = 0; i < count; ++i) {
            char* end;
            values[i] = static_cast<unsigned>(strtoul(text, &end, 10));
            if (end == text) {
                return false;
            }
            text = end;
        }
        return true;
    }

    static bool s_toneStarted = false;

    static bool StartTone()
    {
        if (!s_toneStarted) {
            s_toneStarted = tone_out_init() == 0;
        }
        return s_toneStarted;
    }

    static bool SetChannels(unsigned ch, unsigned phase, unsigned gain)
    {
        for (unsigned i = 0; i < 2; ++i) {
            if ((ch == 2 || ch == i) &&
                pdm_mic_set_channel(get_default_microphone(), i, phase, gain) != 0) {
                return false;
            }
        }
        return ch <= 2;
    }

    /* Records frames of raw stereo after a 100 ms pre-roll that lets the PDM filters settle. */
    static bool
    CaptureRaw(int audioRate, unsigned frames, int16_t* buffer, size_t bufferSamples, Block& block)
    {
        const size_t preRoll = audioRate / 10;
        const size_t total   = preRoll + frames;
        if (frames == 0 || total * 3 > bufferSamples) {
            return false;
        }
        int16_t* raw = buffer + total;
        if (audio_set_raw_capture(raw) != 0) {
            return false;
        }
        hal_get_audio_data(buffer, total);
        const int err = hal_wait_for_audio();
        audio_set_raw_capture(nullptr);
        if (err) {
            printf_err("hal_get_audio_data failed with error: %d\n", err);
            return false;
        }

        block.data      = reinterpret_cast<const uint8_t*>(raw + 2 * preRoll);
        block.bytes     = frames * 2 * sizeof(int16_t);
        block.headerLen = snprintf(block.header,
                                   sizeof(block.header),
                                   "\nKWSRAW %d %u 2 %08" PRIX32 "\n",
                                   audioRate,
                                   frames,
                                   Crc32(block.data, block.bytes));
        return true;
    }

    /* Returns true when a block was captured into block, otherwise replies OK or ERR. */
    static bool Calibrate(int audioRate, int16_t* buffer, size_t bufferSamples, Block& block)
    {
        char line[48];
        ReadLine(line, sizeof(line));
        const char* args = line + 1;
        unsigned v[3]    = {};
        bool ok          = false;
        switch (line[0]) {
        case 'T':
            ok = ParseUints(args, v, 2) && StartTone() && tone_out_set(v[0], v[1]) == 0;
            break;
        case 'V':
            ok = ParseUints(args, v, 1) && StartTone() && tone_out_set_volume(v[0]) == 0;
            break;
        case 'P':
            ok = ParseUints(args, v, 3) && SetChannels(v[0], v[1], v[2]);
            break;
        case 'M':
            ok = ParseUints(args, v, 1) &&
                 pdm_mic_set_decimation(get_default_microphone(), audioRate, v[0]) == 0;
            break;
        case 'W':
            if (ParseUints(args, v, 1) &&
                CaptureRaw(audioRate, v[0], buffer, bufferSamples, block)) {
                return true;
            }
            break;
        default:
            break;
        }
        Reply(ok);
        return false;
    }

    bool RecordAudioHandler(int audioRate, int16_t* buffer, size_t bufferSamples)
    {
        /* Pre-roll lets the driver's DC offset tracking settle after the idle gap. */
        const size_t preRollSamples = audioRate / 4;
        const size_t clipSamples    = audioRate * 3 / 2;

        if (preRollSamples + clipSamples > bufferSamples) {
            printf_err("Record buffer too small: %zu < %zu samples\n",
                       bufferSamples,
                       preRollSamples + clipSamples);
            return false;
        }

        int err = hal_audio_alif_init(audioRate);
        if (err) {
            printf_err("hal_audio_alif_init failed with error: %d\n", err);
            return false;
        }

        info("KWS recorder ready: send '%c' to record %zu samples at %d Hz, '%c' to calibrate\n",
             RECORD_COMMAND,
             clipSamples,
             audioRate,
             CALIBRATE_COMMAND);

        Block block;
        bool haveBlock = false;

        while (true) {
            const char command = uart_getchar();
            if (command == RECORD_COMMAND) {
                hal_get_audio_data(buffer, preRollSamples + clipSamples);
                err = hal_wait_for_audio();
                if (err) {
                    printf_err("hal_get_audio_data failed with error: %d\n", err);
                    return false;
                }
                block.data      = reinterpret_cast<const uint8_t*>(buffer + preRollSamples);
                block.bytes     = clipSamples * sizeof(int16_t);
                block.headerLen = snprintf(block.header,
                                           sizeof(block.header),
                                           "\nKWSREC %d %zu %08" PRIX32 "\n",
                                           audioRate,
                                           clipSamples,
                                           Crc32(block.data, block.bytes));
                haveBlock       = true;
            } else if (command == CALIBRATE_COMMAND) {
                if (!Calibrate(audioRate, buffer, bufferSamples, block)) {
                    continue;
                }
                haveBlock = true;
            } else if (command != RESEND_COMMAND || !haveBlock) {
                continue;
            }
            SendBlock(block);
        }
    }

} /* namespace app */
} /* namespace alif */

#endif /* KWS_RECORD_MODE */
