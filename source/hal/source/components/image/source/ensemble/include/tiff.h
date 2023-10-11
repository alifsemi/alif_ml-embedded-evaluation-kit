/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef TIFF_H_
#define TIFF_H_

#include <stdint.h>

#define TIFF_HDR_NUM_ENTRY 12

typedef struct tiff_ifd_entry {
    uint16_t tag;
    uint16_t type;
    uint32_t count;
    uint32_t value_offset;
} tiff_ifd_entry_t;

typedef struct tiff_header {
    uint16_t byte_order;
    uint16_t magic;
    uint32_t first_ifd_offset;
    uint16_t padding1;
    uint16_t ifd_entries;
    tiff_ifd_entry_t ifd_entry[TIFF_HDR_NUM_ENTRY];
    uint32_t next_ifd_offset;
    uint16_t value_bps[3];
    uint32_t value_xyres[2];
} tiff_header_t;

void write_tiff_header(tiff_header_t *header, uint32_t width, uint32_t height);

#endif /* TIFF_H_ */
