/*
 * tgu_M55_HE.h Contains routine(s) to program the TGU configuration registers
 * and look up table registers.
 *
 * Author   : Silesh C V <silesh@alifsemi.com>
 *
 * Copyright (C) 2021 ALIF SEMICONDUCTOR
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TGU_M55_HE_H
#define TGU_M55_HE_H
#include <stdint.h>
#include <limits.h>

#include "M55_HE_map.h"

#define ITGU_BASE                 (0xE001E500UL)
#define ITGU_CTRL                 (ITGU_BASE + 0x0)
#define ITGU_CFG                  (ITGU_BASE + 0x4)
#define ITGU_LUT(n)               (ITGU_BASE + 0x10 + 4 * n)

#define DTGU_BASE                 (0xE001E600UL)
#define DTGU_CTRL                 (DTGU_BASE + 0x0)
#define DTGU_CFG                  (DTGU_BASE + 0x4)
#define DTGU_LUT(n)               (DTGU_BASE + 0x10 + 4 * n)
#define TGU_LUT(base, n)          ((base) + 0x10 + 4 * (n))

#define DTGU_CFG_BLKSZ            (0xFU << 0)
#define ITGU_CFG_BLKSZ            (0xFU << 0)

#define SET_BIT_RANGE(from, to) (~0U >> ((CHAR_BIT * sizeof(unsigned int)) - (to) - 1)) \
                                    & (~0U << (from))

enum tcm_type {ITCM, DTCM};

struct mem_region {
    uint32_t start;
    uint32_t end;
    enum tcm_type type;
};

/*
 * TGU_Setup()
 * Set up the TGU look up tables as per the information provided by
 * the platform in ns_regions array.
 */
void TGU_Setup(void);
#endif /* TGU_M55_HE_H */
