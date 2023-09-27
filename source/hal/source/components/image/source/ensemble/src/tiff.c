/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stddef.h>

#include "tiff.h"

enum tiff_tag {
    ImageWidth = 256,
    ImageLength = 257,
    BitsPerSample = 258,
    Compression = 259,
    PhotometricInterpretation = 262,
    StripOffsets = 273,
    SamplesPerPixel = 277,
    RowsPerStrip = 278,
    StripByteCounts = 279,
    XResolution = 282,
    YResolution = 283,
    ResolutionUnit = 296
};

enum tiff_type {
    SHORT = 3,
    LONG = 4,
    RATIONAL = 5
};

static const tiff_header_t tiff_header = {
    .byte_order = 0x4949, // Little-endian
    .magic = 42,
    .first_ifd_offset = offsetof(tiff_header_t, ifd_entries),
    .ifd_entries = TIFF_HDR_NUM_ENTRY,
    .ifd_entry = {
        [ 0] = { ImageWidth, LONG, 1, 0 }, // to be filled
        [ 1] = { ImageLength, LONG, 1, 0 }, // to be filled
        [ 2] = { BitsPerSample, SHORT, 3, offsetof(tiff_header_t, value_bps) },
        [ 3] = { Compression, SHORT, 1, 1 }, // None
        [ 4] = { PhotometricInterpretation, SHORT, 1, 2 }, // RGB
        [ 5] = { StripOffsets, LONG, 1, sizeof(tiff_header_t) },
        [ 6] = { SamplesPerPixel, SHORT, 1, 3 },
        [ 7] = { RowsPerStrip, LONG, 1, 0 }, // to be filled
        [ 8] = { StripByteCounts, LONG, 1, 0 }, // to be filled
        [ 9] = { XResolution, RATIONAL, 1, offsetof(tiff_header_t, value_xyres ) },
        [10] = { YResolution, RATIONAL, 1, offsetof(tiff_header_t, value_xyres ) },
        [11] = { ResolutionUnit, SHORT, 1, 1 } // None
    },
    .next_ifd_offset = 0,
    .value_bps = { 8, 8, 8 },
    .value_xyres = { 1, 1 }
};

void write_tiff_header(tiff_header_t *header, uint32_t width, uint32_t height)
{
    *header = tiff_header;

	// ImageWidth
	header->ifd_entry[0].value_offset = width;

    // ImageLength
    header->ifd_entry[1].value_offset = height;

	// RowsPerStrip
    header->ifd_entry[7].value_offset = height;

	// StripByteCounts
    header->ifd_entry[8].value_offset = width * height * 3;
};

